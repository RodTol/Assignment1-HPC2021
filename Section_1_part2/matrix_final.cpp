#include <mpi.h>
#include <chrono>
#include <thread>

void sum_matrix(int paradigm, int n_proc, int dimensioni[39][3], MPI_Comm com, double*** matrix1, double*** matrix2, int dim[3]);

double d_gen(double Min_val, double Max_val);

void adapt_to_resto(int dim, int &dim_sub, int &dim_fake, int n);

void matrix_all(int x, int y, int z, double***& matrix);

void matrix_del(int x, int y, double***& matrix);

void matrix_to_buffer (double*** matrix, double*& buffer, int dim_x_sub, int dim_y_sub, int dim_z_sub, int dimensioni[32][3], int paradigma);
void buffer_to_matrix (double***& matrix, double* buffer, int dim_x_sub, int dim_y_sub, int dim_z_sub, int dimensioni[32][3], int paradigma);

int main ( int argc , char *argv[ ] )
{
    int dimensioni[39][3] = {
    // 1-Dimension
        {24, 1, 1} ,
    // 2-dimension
        {2, 12, 1} ,
        {3, 8, 1} ,
        {4,6,1} ,
        {6,4,1} ,
        {8,3,1} ,
        {12, 2, 1} ,
        {1,24,1} ,
        {24,1,1} ,
    // 3-dimension
        {2,2,6} ,
        {2,6,2} ,
        {6,2,2} ,
        {2,3,4} ,
        {2,4,3} ,
        {3,2,4} ,
        {3,4,2} ,
        {4,2,3} ,
        {4,3,2} ,
        {1,1,24},
        {1,24,1} ,
        {24,1,1} ,
        {1,2, 12} ,
        {1,12,2} ,
        {2,1,12} ,
        {2,12,1} ,
        {12,1,2} ,
        {12,2,1} ,
        {1,3,8} ,
        {1,8,3} ,
        {3,1,8} ,
        {3,8,1} ,
        {8,1,3} ,
        {8,3,1} ,
        {1,4,6} ,
        {1,6,4} ,
        {4,1,6} ,
        {4,6,1} ,
        {6,1,4} ,
        {6,4,1}
    };    

    int n_proc, irank, master = 0;
    int periodic = 0;

    int dim_x, dim_y, dim_z;
    int dim[3];

    double*** matrix1, *** matrix2;

    MPI_Comm OneD_com, TwoD_com, ThreeD_com;

    
    MPI_Init(&argc,&argv);
    /*Assegno il rank usando questo comunicatore*/
    MPI_Comm_rank(MPI_COMM_WORLD,&irank);
    /*Mi dice quanti processori sono assegnati a un comunicatore*/
    MPI_Comm_size(MPI_COMM_WORLD,&n_proc);

    dim_x = atoi(argv[1]);
    dim_y = atoi(argv[2]);
    dim_z = atoi(argv[3]);

    dim[0] = dim_x;
    dim[1] = dim_y;
    dim[2] = dim_z;


    /*Creiamo le matrici base*/
    matrix_all(dim_x, dim_y, dim_z, matrix1);
    matrix_all(dim_x, dim_y, dim_z, matrix2);
    
    /*Assegniamo i valori sono nel master*/
    if (irank == master) { 
        /*Creiamo i valori casuali nel core 0 di tutte e 3
        le matrici*/
        for (int i = 0; i < dim_x; i++) {
            for (int j = 0; j < dim_y; j++) {
                for (int k = 0; k < dim_z; k++) {
                    matrix1[i][j][k] = d_gen(0,100);
                    matrix2[i][j][k] = d_gen(0,100);
                }
            }
        }
    }

    MPI_Cart_create( MPI_COMM_WORLD, 1, &n_proc, &periodic, 1, &OneD_com);
    sum_matrix(0, n_proc, dimensioni, OneD_com, matrix1, matrix2, dim);
        
    for (int paradigm = 1; paradigm < 9; paradigm++) {
        if (irank == master) {
            std::cout << "-------------------CASO 2D-----------------------\n";
            std::cout << "Topologia " << dimensioni[paradigm][0] << "  " << dimensioni[paradigm][1] << " " << std::endl;
        }
        int periodicity[2] = {0,0}, topology[2] = {dimensioni[paradigm][0], dimensioni[paradigm][1]};
        MPI_Cart_create( MPI_COMM_WORLD, 2, topology, periodicity, 1, &TwoD_com);
        sum_matrix(paradigm, n_proc, dimensioni, TwoD_com, matrix1, matrix2, dim);
    }

    for (int paradigm = 9; paradigm < 39; paradigm++) {
        if (irank == master) {
            std::cout << "-------------------CASO 3D-----------------------\n";
            std::cout << "Topologia " << dimensioni[paradigm][0] << "  " << dimensioni[paradigm][1] << " " << 
            dimensioni[paradigm][2] << std::endl;
        }
        int periodicity[3] = {0,0,0}, topology[3] = {dimensioni[paradigm][0], dimensioni[paradigm][1],dimensioni[paradigm][2]};
        MPI_Cart_create( MPI_COMM_WORLD, 3, topology, periodicity, 1, &ThreeD_com);
        sum_matrix(paradigm, n_proc, dimensioni, ThreeD_com, matrix1, matrix2, dim);
    }
   
    matrix_del(dim_x, dim_y, matrix1);
    matrix_del(dim_x, dim_y, matrix2);

    MPI_Finalize() ;
}


double d_gen(double Min_val, double Max_val)
{
    double f = (double)rand() / RAND_MAX;
    return Min_val + f*(Max_val - Min_val);
}

void adapt_to_resto(int dim, int &dim_sub, int &dim_fake, int n) {
    int resto = dim -n*dim_sub;
    if (resto !=0)
    {
        /*Dimensioni della matrice espansa*/
        dim_fake = dim + n - resto ; 
        /*Dimensioni delle sub_matrix che dividono a resto 0
        le matrici fake*/
        dim_sub = dim_fake/n;
    }
}

void matrix_all(int x, int y, int z, double***& matrix) {

    matrix = new double**[x];
    for (int i = 0; i < x; i++)
        {
            matrix[i] = new double*[y];
            for (int j = 0; j < y; j++)
            {
                matrix[i][j] =  new double [z];
            }
        }
}

void matrix_del(int x, int y, double***& matrix) {

    for (int i = 0; i < x; i++)
        {
            for (int j = 0; j < y; j++)
            {
                delete[] matrix[i][j];
            }
            delete [] matrix[i]; 
        }
    delete[] matrix;
}

void matrix_to_buffer (double*** matrix, double*& buffer, int dim_x_sub, int dim_y_sub, int dim_z_sub, int dimensioni[32][3], int paradigma) {
 int b;
 int count_sub;
 int volume = dim_x_sub*dim_y_sub*dim_z_sub;
    for (int k_sub = 0; k_sub < dimensioni[paradigma][2]; k_sub++)
    {
        for (int j_sub = 0; j_sub < dimensioni[paradigma][1]; j_sub++)
        {
            for (int i_sub = 0; i_sub < dimensioni[paradigma][0]; i_sub++)
            {
                for (int i = i_sub*dim_x_sub; i < dim_x_sub*(i_sub+1); i++)
                {
                    for (int j = j_sub*dim_y_sub; j < dim_y_sub*(j_sub+1); j++)
                    {
                        for (int k = k_sub*dim_z_sub; k < dim_z_sub*(k_sub+1); k++)   
                        {
                        /*Ricorda che le x sono verticali a lato, le x la base in basso e le z la profondità*/
                        /*Vedo la mia separazione come una matrice da cui scegliere una sub_matrix e questo 
                        li conta*/
                        count_sub = j_sub+(dimensioni[paradigma][1])*(i_sub+dimensioni[paradigma][0]*k_sub);
                        b = (k-k_sub*dim_z_sub)*dim_x_sub*dim_y_sub + (i-i_sub*dim_x_sub)*dim_y_sub +
                            (j-j_sub*dim_y_sub);
                        buffer[b+volume*count_sub] = matrix[i][j][k];
                        }
                    }
                }
            }
        }
    }
}

void buffer_to_matrix (double***& matrix, double* buffer, int dim_x_sub, int dim_y_sub, int dim_z_sub, int dimensioni[32][3], int paradigma) {
 int b;
 int count_sub;
 int volume = dim_x_sub*dim_y_sub*dim_z_sub;
    for (int k_sub = 0; k_sub < dimensioni[paradigma][2]; k_sub++)
    {
        for (int j_sub = 0; j_sub < dimensioni[paradigma][1]; j_sub++)
        {
            for (int i_sub = 0; i_sub < dimensioni[paradigma][0]; i_sub++)
            {
                for (int i = i_sub*dim_x_sub; i < dim_x_sub*(i_sub+1); i++)
                {
                    for (int j = j_sub*dim_y_sub; j < dim_y_sub*(j_sub+1); j++)
                    {
                        for (int k = k_sub*dim_z_sub; k < dim_z_sub*(k_sub+1); k++)   
                        {
                        count_sub = j_sub+(dimensioni[paradigma][1])*(i_sub+dimensioni[paradigma][0]*k_sub);
                        b = (k-k_sub*dim_z_sub)*dim_x_sub*dim_y_sub + (i-i_sub*dim_x_sub)*dim_y_sub +
                                (j-j_sub*dim_y_sub);
                        matrix[i][j][k] = buffer[b+volume*count_sub];
                        }
                    }
                }
            }
        }
    }
}

void sum_matrix(int paradigm, int n_proc, int dimensioni[39][3], MPI_Comm com, double*** matrix1, double*** matrix2, int dim[3]) {
   //SISTEMA NUMERO DI PROCESSORI
    int rank;
    int dim_x = dim[0], dim_y = dim[1], dim_z = dim[2];
    int dim_x_fake = dim_x, dim_y_fake = dim_y, dim_z_fake = dim_z;
    int dim_x_sub, dim_y_sub, dim_z_sub, volume;

    double*** matrix3, *** matrix_true;
    double*** matrix1_fake, *** matrix2_fake, *** matrix3_fake;
    double* buffer1, *buffer2 , *buffer3;
    double *sub_buffer1, *sub_buffer2, *sub_buffer3;

    int flag = 1, master = 0;
    double start_time, end_time;

    /*Assegno il rank usando questo comunicatore*/
    MPI_Comm_rank(com,&rank);
    /*Indico quanti processori sono assegnati al comunicatore*/
    MPI_Comm_size(com,&n_proc);

    matrix_all(dim_x, dim_y, dim_z, matrix3);
    matrix_all(dim_x, dim_y, dim_z, matrix_true);

    dim_x_sub = dim_x/dimensioni[paradigm][0];
    dim_y_sub = dim_y/dimensioni[paradigm][1];       
    dim_z_sub = dim_z/dimensioni[paradigm][2];

    adapt_to_resto(dim_x, dim_x_sub, dim_x_fake, dimensioni[paradigm][0]);
    adapt_to_resto(dim_y, dim_y_sub, dim_y_fake, dimensioni[paradigm][1]);
    adapt_to_resto(dim_z, dim_z_sub, dim_z_fake, dimensioni[paradigm][2]);

    matrix_all(dim_x_fake, dim_y_fake, dim_z_fake, matrix1_fake);
    matrix_all(dim_x_fake, dim_y_fake, dim_z_fake, matrix2_fake);
    matrix_all(dim_x_fake, dim_y_fake, dim_z_fake, matrix3_fake);

    volume =  dim_x_sub*dim_y_sub*dim_z_sub;

    if (rank == master)
    {
        std::cout << "Dimensioni fake " << dim_x_fake << " " << dim_y_fake << " " << dim_z_fake << std::endl;
        std::cout << "Nuove dimensioni sub matrix " << dim_x_sub << " " << dim_y_sub << " " << dim_z_sub << std::endl;
    }

    if (rank == master)
    { 
        for (int i = 0; i < dim_x; i++)
        {
            for (int j = 0; j < dim_y; j++)
            {
                for (int k = 0; k < dim_z; k++)
                {
                    matrix_true[i][j][k] = matrix1[i][j][k]+matrix2[i][j][k];
                    matrix1_fake[i][j][k] = matrix1[i][j][k];
                    matrix2_fake[i][j][k] = matrix2[i][j][k];
                }
            }
        }
    }

    buffer1 = new double[dim_x_fake*dim_y_fake*dim_z_fake];
    buffer2 = new double[dim_x_fake*dim_y_fake*dim_z_fake];
    buffer3 = new double[dim_x_fake*dim_y_fake*dim_z_fake];

    if (rank == 0)
    {   
        matrix_to_buffer(matrix1_fake, buffer1, dim_x_sub, dim_y_sub, dim_z_sub, dimensioni, paradigm);
        matrix_to_buffer(matrix2_fake, buffer2, dim_x_sub, dim_y_sub, dim_z_sub, dimensioni, paradigm);
    }

    sub_buffer1 =  new double[dim_x_sub*dim_y_sub*dim_z_sub];
    sub_buffer2 =  new double[dim_x_sub*dim_y_sub*dim_z_sub];
    sub_buffer3 =  new double[dim_x_sub*dim_y_sub*dim_z_sub];

    start_time = MPI_Wtime();
    
    MPI_Scatter(buffer1, volume, MPI_DOUBLE, sub_buffer1, volume, MPI_DOUBLE, 0, com);
    MPI_Scatter(buffer2, volume, MPI_DOUBLE, sub_buffer2, volume, MPI_DOUBLE, 0, com);
    

    for (int i = 0; i < dim_x_sub*dim_y_sub*dim_z_sub; i++)
        {
          sub_buffer3[i] = sub_buffer1[i]+sub_buffer2[i];
        }

    MPI_Gather(sub_buffer3, volume, MPI_DOUBLE, buffer3, volume, MPI_DOUBLE, 0, com);

    end_time = MPI_Wtime();
    
    if (rank == master)
    {
        /*Assegno i valori alla matrice di dimensioni corrette dal buffer*/
        buffer_to_matrix(matrix3_fake, buffer3, dim_x_sub, dim_y_sub, dim_z_sub, dimensioni, paradigm);

        /*Assegno i valori alla matrice di dimensioni corrette*/
        for (int i = 0; i < dim_x; i++)
        {
            for (int j = 0; j < dim_y; j++)
            {
                for (int k = 0; k < dim_z; k++)
                {
                    matrix3[i][j][k] =  matrix3_fake[i][j][k];
                    if (matrix3[i][j][k] != matrix_true[i][j][k])
                    {
                        std::cout << "There is an error\n" << "In the indexes " << i << " " << j << " " << k << std::endl;
                        std::cout << matrix3[i][j][k] << "     " << matrix_true[i][j][k] << std::endl;
                        flag = 0;
                    }   
                }
            }
        }

        if (flag == 1) {
            std::cout << "Operation successful!\n";
            std::cout << "Time of comunication+operation is :" << end_time-start_time << std::endl;
        }
    }

    matrix_del(dim_x_fake, dim_y_fake, matrix1_fake);
    matrix_del(dim_x_fake, dim_y_fake, matrix2_fake);
    matrix_del(dim_x_fake, dim_y_fake, matrix3_fake);
    matrix_del(dim_x, dim_y, matrix3);
    matrix_del(dim_x, dim_y, matrix_true);

    delete [] buffer1;
    delete [] buffer2;
    delete [] buffer3;
    delete [] sub_buffer1;
    delete [] sub_buffer2;
    delete [] sub_buffer3;
}
