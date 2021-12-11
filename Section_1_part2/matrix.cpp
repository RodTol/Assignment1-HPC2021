#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <chrono>
#include <thread>

#define SEED 35791246
/*NELLA VERSIONE FINALE METTI IL GENERATORE DI SEED RANDOM*/

double d_gen(double Min_val, double Max_val);

int main ( int argc , char *argv[ ] )
{
    /*VARIABILE PER ATTIVARE CONTROLLI EXTRA*/
    int check = 0, flag = 1;
    double start_time, end_time;
    int n_proc, irank, master = 0;

    MPI_Init(&argc,&argv);
    /*Assegno il rank usando questo comunicatore*/
    MPI_Comm_rank(MPI_COMM_WORLD,&irank);
    /*Mi dice quanti processori sono assegnati a un comunicatore*/
    MPI_Comm_size(MPI_COMM_WORLD,&n_proc);

    int dim_x, dim_y, dim_z, volume;
    int resto_x, resto_y, resto_z;
    int dim_x_fake, dim_y_fake, dim_z_fake;
    int dim_x_sub, dim_y_sub, dim_z_sub;

    dim_x = atoi(argv[1]);
    dim_y = atoi(argv[2]);
    dim_z = atoi(argv[3]);

    /*Creiamo le matrici base*/
    double matrix1[dim_x][dim_y][dim_z], matrix2[dim_x][dim_y][dim_z], matrix3[dim_x][dim_y][dim_z],
     matrix_true[dim_x][dim_y][dim_z];

    /*Sto indicando come master, quello indicato da comm_world che mi 
    aspetto sia lo stesso per i comunicatori "cartesiani", dato che sono 
    creati usando quest'ultimo*/
    if (irank == master)
    {
        /*Creiamo i valori casuali nel core 0 di tutte e 3
        le matrici*/
        for (int i = 0; i < dim_x; i++)
        {
            for (int j = 0; j < dim_y; j++)
            {
                for (int k = 0; k < dim_z; k++)
                {
                    matrix1[i][j][k] = d_gen(0,100);
                    matrix2[i][j][k] = d_gen(0,100);
                    matrix_true[i][j][k] = matrix1[i][j][k]+matrix2[i][j][k];
                }
            }
        }
    }

    dim_x_fake = atoi(argv[1]);
    dim_y_fake = atoi(argv[2]);
    dim_z_fake = atoi(argv[3]);

    int dimensioni[39][3] = {
        {24, 1, 1} ,
    /*2-dinension*/
        {12, 2, 1} ,
        {3, 8, 1} ,
        {4,6,1} ,
        {6,4,1} ,
        {8,3,1} ,
        {2, 12, 1} ,
        {1,24,1} ,
        {24,1,1} ,
    /*3-dimension*/
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

if (irank == 0)
{
    std::cout << "Caso uno-dimensionale-----------------------------------------" << std::endl;
}

    int periodic = 0, left_p, right_p;
    MPI_Comm OneD_com;

    /*Creo un comunicatore per una virtual topology 1-D*/
    MPI_Cart_create( MPI_COMM_WORLD, 1, &n_proc, &periodic, 1, &OneD_com);
    /*Definisco il movimento a destra */
    MPI_Cart_shift( OneD_com, 0, 1, &left_p, &right_p );
    /*Assegno il rank usando questo comunicatore*/
    MPI_Comm_rank(OneD_com,&irank);
    /*Indico quanti processori sono assegnati al comunicatore*/
    MPI_Comm_size(OneD_com,&n_proc);

    /*Calcole le dimensioni delle sottomatrici*/
    dim_x_sub = dim_x/dimensioni[0][0];
    dim_y_sub = dim_y/dimensioni[0][1];
    dim_z_sub = dim_z/dimensioni[0][2];
    /*Nel report segna che il codice potrebbe essere allegerito, rimuovendo
    delle parti che sicuramente sono inutili per il caso 1-D ma in un ottica
    di lasciare una sorta di "template", la scrittura è molto simile a quella di 
    2-D e 3-D.*/

    /*Vedo se il resto è nullo*/
    resto_x = dim_x - dimensioni[0][0]*dim_x_sub;
    resto_y = dim_y - dimensioni[0][1]*dim_y_sub;
    resto_z = dim_z - dimensioni[0][2]*dim_z_sub;

    if (irank == master)
    {
        std::cout << "Dimensioni sub matrix " << dim_x_sub << " " << dim_y_sub << " " << dim_z_sub << std::endl;
        std::cout << "Resto x " << resto_x << "  Resto y " << resto_y << "  Resto z " << resto_z << std::endl;
    }

    if (resto_x !=0)
    {
        /*Dimensioni della matrice espansa*/
        dim_x_fake = dim_x + dimensioni[0][0] - resto_x ; 
        /*Dimensioni delle sub_matrix che dividono a resto 0
        le matrici fake*/
        dim_x_sub = dim_x_fake/dimensioni[0][0];
    }

    if (resto_y !=0)
    {
        /*Dimensioni della matrice espansa*/
        dim_y_fake = dim_y + dimensioni[0][1] - resto_y;  
        /*Dimensioni delle sub_matrix che dividono a resto 0
        le matrici fake*/
        dim_y_sub = dim_y_fake/dimensioni[0][1];
    }

    if (resto_z !=0)
    {
        /*Dimensioni della matrice espansa*/
        dim_z_fake = dim_z + dimensioni[0][2] - resto_z; 
        /*Dimensioni delle sub_matrix che dividono a resto 0
        le matrici fake*/
        dim_z_sub = dim_z_fake/dimensioni[0][2];
    }
    
    /*Creiamo le nostre matrici modificate, che nel caso ogni
    resto sia nullo, avranno dimensione nulla*/
    double matrix1_fake[dim_x_fake][dim_y_fake][dim_z_fake] = {}, matrix2_fake[dim_x_fake][dim_y_fake][dim_z_fake] = {},
    matrix3_fake[dim_x_fake][dim_y_fake][dim_z_fake] = {};

    /*Creiamo finalemnte le sottomatrici con le dimensioni corrette
    per eseguire i conti*/
    double sub_matrix1[dim_x_sub][dim_y_sub][dim_z_sub], sub_matrix2[dim_x_sub][dim_y_sub][dim_z_sub],
     sub_matrix3[dim_x_sub][dim_y_sub][dim_z_sub];
    /*Il volume indica il numero di double da spostare (che teoricamente sarà
    sempre 100x100x100)*/
    volume =  dim_x_sub*dim_y_sub*dim_z_sub;

    if (irank == master)
    {
        std::cout << "Dimensioni fake " << dim_x_fake << " " << dim_y_fake << " " << dim_z_fake << std::endl;
        std::cout << "Dimensioni sub matrix  finali " << dim_x_sub << " " << dim_y_sub << " " << dim_z_sub << std::endl;
    }
    
    /*Assegniamo i valori sono nel master*/
    if (irank == master)
    { 
        /*Creiamo i valori casuali nel core 0 di tutte e 3
        le matrici*/
        for (int i = 0; i < dim_x; i++)
        {
            for (int j = 0; j < dim_y; j++)
            {
                for (int k = 0; k < dim_z; k++)
                {
                    matrix1_fake[i][j][k] = matrix1[i][j][k];
                    matrix2_fake[i][j][k] = matrix2[i][j][k];
                }
            }
        }

        if (check == 1)
        {
            std::cout << "------------MATRIX 1------------\n";
            /*Check dei valori di una fetta*/
            for (int i = 0; i < 5; i++)
            {
            for (int j = 0; j < dim_y; j++)
            {
                std::cout << matrix1[i][j][0] << "  ";
            }
            
                std::cout << "\n";
            }
            std::cout << "------------MATRIX 2------------\n";
            /*Check dei valori di una fetta*/
            for (int i = 0; i < 5; i++)
            {
            for (int j = 0; j < dim_y; j++)
            {
                std::cout << matrix2[i][j][0] << "  ";
            }
            
                std::cout << "\n";
            }
            std::cout << "------------MATRIX TRUE------------\n";
            /*Check dei valori di una fetta*/
            for (int i = 20; i < dim_x_fake; i++)
            {
                for (int j = 0; j < dim_y; j++)
                {      
                std::cout << matrix_true[i][j][0] << "  ";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;

            std::cout << "RIGHE EXTRA MATRICE FAKE \n";
            for (int i = dim_x; i < dim_x_fake; i++)
            {
                for (int j = 0; j < dim_y_fake; j++)
                {
                        std::cout << matrix1_fake[i][j][0] << " ";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;
        }
        
    }
    
    start_time = MPI_Wtime();

    MPI_Scatter(&matrix1_fake, volume, MPI_DOUBLE, &sub_matrix1, volume, MPI_DOUBLE, 0, OneD_com);
    MPI_Scatter(&matrix2_fake, volume, MPI_DOUBLE, &sub_matrix2, volume, MPI_DOUBLE, 0, OneD_com);

    if (check == 1)
    {
        for (int i = 0; i < 5; i++)
        {
            if (irank == i)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(i*10));
                std::cout << "I am " << irank << " and my values are\n";

                std::cout << "------------MATRIX 1------------\n";
                for (int i = 0; i < dim_x_sub; i++)
                {
                for (int j = 0; j < dim_y_sub; j++)
                {
                    std::cout << sub_matrix1[i][j][1] << "  ";
                }
                    std::cout << "\n";
                }
                std::cout << "------------MATRIX 2------------\n";

                for (int i = 0; i < dim_x_sub; i++)
                {
                for (int j = 0; j < dim_y_sub; j++)
                {
                    std::cout << sub_matrix2[i][j][1] << "  ";
                }
                    std::cout << "\n";
                }
                std::cout << std::endl;
                
            }
        }
    }

    /*Eseguiamo la somma*/
    for (int i = 0; i < dim_x_sub; i++)
        {
            for (int j = 0; j < dim_y_sub; j++)
            {
                for (int k = 0; k < dim_z_sub; k++)
                {
                    sub_matrix3[i][j][k] = sub_matrix1[i][j][k]+sub_matrix2[i][j][k];
                }
            }
        }

    MPI_Gather(&sub_matrix3, volume, MPI_DOUBLE, &matrix3_fake, volume, MPI_DOUBLE, 0, OneD_com);

    end_time = MPI_Wtime();

    if (irank == master)
    {
        /*Assegno i valori alla matrice di dimensioni corrette*/
        for (int i = 0; i < dim_x; i++)
        {
            for (int j = 0; j < dim_y; j++)
            {
                for (int k = 0; k < dim_z; k++)
                {
                    matrix3[i][j][k] =  matrix3_fake[i][j][k];
                }
            }
        }

        if (check == 1) {
        std::cout << "I am " << irank << " and my values are\n";
        std::cout << "------------MATRIX 3------------\n";
        for (int i = 20; i < dim_x; i++)
        {
            for (int j = 0; j < dim_y; j++)
            {
            std::cout << matrix3[i][j][0] << "  ";
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
        }


        /*Confronto la matrice true con il mio risultato ottenuto dal gather*/
        for (int i = 0; i < dim_x; i++)
        {
            for (int j = 0; j < dim_y; j++)
            {
                for (int k = 0; k < dim_z; k++)
                {
                    if (matrix3[i][j][k] != matrix_true[i][j][k])
                    {
                        std::cout << "Valori diversi negl' indici " << i << " " << j << " " << k << std::endl;
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
if (irank == master)
{
    std::cout << "Caso bi-dimensionale-----------------------------------------" << std::endl;
}
    //  OCIO AL NUMERO DI CASI
    
    for (int kallla= 1; kallla < 3; kallla++)
    {
        int paradigm = kallla;
        if (irank == master)
        {
            std::cout << "Paradigma " << dimensioni[paradigm][0] << "  " << dimensioni[paradigm][1] << 
            "-------------------------------" << std::endl;
        }
        
        int periodic[2] = {0,0}, topolo[2] = {dimensioni[paradigm][0], dimensioni[paradigm][1]};

        MPI_Comm TwoD_com;

        /*Creo un comunicatore per una virtual topology 1-D*/
        MPI_Cart_create( MPI_COMM_WORLD, 2, topolo, periodic, 1, &TwoD_com);
        /*Assegno il rank usando questo comunicatore*/
        MPI_Comm_rank(TwoD_com,&irank);
        /*Indico quanti processori sono assegnati al comunicatore*/
        MPI_Comm_size(TwoD_com,&n_proc);

        /*Calcole le dimensioni delle sottomatrici*/
        dim_x_sub = dim_x/dimensioni[paradigm][0];
        dim_y_sub = dim_y/dimensioni[paradigm][1];
        dim_z_sub = dim_z/dimensioni[paradigm][2];

        /*Vedo se il resto è nullo*/
        resto_x = dim_x - dimensioni[paradigm][0]*dim_x_sub;
        resto_y = dim_y - dimensioni[paradigm][1]*dim_y_sub;
        resto_z = dim_z - dimensioni[paradigm][2]*dim_z_sub;

        if (irank == master)
        {
            std::cout << "Dimensioni sub matrix " << dim_x_sub << " " << dim_y_sub << " " << dim_z_sub << std::endl;
            std::cout << "Resto x " << resto_x << "  Resto y " << resto_y << "  Resto z " << resto_z << std::endl;
        }

        if (resto_x !=0)
        {
            /*Dimensioni della matrice espansa*/
            dim_x_fake = dim_x + dimensioni[paradigm][0] - resto_x ; 
            /*Dimensioni delle sub_matrix che dividono a resto 0
            le matrici fake*/
            dim_x_sub = dim_x_fake/dimensioni[paradigm][0];
        }

        if (resto_y !=0)
        {
            /*Dimensioni della matrice espansa*/
            dim_y_fake = dim_y + dimensioni[paradigm][1] - resto_y;  
            /*Dimensioni delle sub_matrix che dividono a resto 0
            le matrici fake*/
            dim_y_sub = dim_y_fake/dimensioni[paradigm][1];
        }

        if (resto_z !=0)
        {
            /*Dimensioni della matrice espansa*/
            dim_z_fake = dim_z + dimensioni[paradigm][2] - resto_z; 
            /*Dimensioni delle sub_matrix che dividono a resto 0
            le matrici fake*/
            dim_z_sub = dim_z_fake/dimensioni[paradigm][2];
        }
        
        /*Creiamo le nostre matrici modificate, che nel caso ogni
        resto sia nullo, avranno dimensione nulla*/
        double matrix1_fake[dim_x_fake][dim_y_fake][dim_z_fake] = {}, matrix2_fake[dim_x_fake][dim_y_fake][dim_z_fake] = {},
        matrix3_fake[dim_x_fake][dim_y_fake][dim_z_fake] = {};

        /*Creiamo finalemnte le sottomatrici con le dimensioni corrette
        per eseguire i conti*/
        double sub_matrix1[dim_x_sub][dim_y_sub][dim_z_sub], sub_matrix2[dim_x_sub][dim_y_sub][dim_z_sub],
        sub_matrix3[dim_x_sub][dim_y_sub][dim_z_sub];
        /*Il volume indica il numero di double da spostare (che teoricamente sarà
        sempre 100x100x100)*/
        volume =  dim_x_sub*dim_y_sub*dim_z_sub;

        if (irank == master)
        {
            std::cout << "Dimensioni fake " << dim_x_fake << " " << dim_y_fake << " " << dim_z_fake << std::endl;
            std::cout << "Dimensioni sub matrix  finali " << dim_x_sub << " " << dim_y_sub << " " << dim_z_sub << std::endl;
        }
        
        /*Assegniamo i valori sono nel master*/
        if (irank == master)
        { 
            /*Creiamo i valori casuali nel core 0 di tutte e 3
            le matrici*/
            for (int i = 0; i < dim_x; i++)
            {
                for (int j = 0; j < dim_y; j++)
                {
                    for (int k = 0; k < dim_z; k++)
                    {
                        matrix1_fake[i][j][k] = matrix1[i][j][k];
                        matrix2_fake[i][j][k] = matrix2[i][j][k];
                    }
                }
            }

            if (check == 1)
            {
                std::cout << "------------MATRIX 1------------\n";
                /*Check dei valori di una fetta*/
                for (int i = 0; i < 5; i++)
                {
                for (int j = 0; j < dim_y; j++)
                {
                    std::cout << matrix1[i][j][0] << "  ";
                }
                
                    std::cout << "\n";
                }
                std::cout << "------------MATRIX TRUE------------\n";
                /*Check dei valori di una fetta*/
                for (int i = 20; i < dim_x_fake; i++)
                {
                    for (int j = 0; j < dim_y; j++)
                    {      
                    std::cout << matrix_true[i][j][0] << "  ";
                    }
                    std::cout << "\n";
                }
                std::cout << std::endl;

                std::cout << "RIGHE EXTRA MATRICE FAKE X\n";
                for (int i = dim_x; i < dim_x_fake; i++)
                {
                    for (int j = 0; j < dim_y_fake; j++)
                    {
                            std::cout << matrix1_fake[i][j][0] << " ";
                    }
                    std::cout << "\n";
                }
                std::cout << std::endl;
                std::cout << "RIGHE EXTRA MATRICE FAKE Y\n";
                for (int i = 0; i < dim_x_fake; i++)
                {
                    for (int j = dim_y; j < dim_y_fake; j++)
                    {
                            std::cout << matrix1_fake[i][j][0] << " ";
                    }
                    std::cout << "\n";
                }
                std::cout << std::endl;
            }
            
        }
        
        start_time = MPI_Wtime();

        MPI_Scatter(&matrix1_fake, volume, MPI_DOUBLE, &sub_matrix1, volume, MPI_DOUBLE, 0, TwoD_com);
        MPI_Scatter(&matrix2_fake, volume, MPI_DOUBLE, &sub_matrix2, volume, MPI_DOUBLE, 0, TwoD_com);

        if (check == 1)
        {
            for (int i = 0; i < 5; i++)
            {
                if (irank == i)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(i*10));
                    std::cout << "I am " << irank << " and my values are\n";

                    std::cout << "------------MATRIX 1------------\n";
                    for (int i = 0; i < dim_x_sub; i++)
                    {
                    for (int j = 0; j < dim_y_sub; j++)
                    {
                        std::cout << sub_matrix1[i][j][1] << "  ";
                    }
                        std::cout << "\n";
                    }
                    std::cout << "------------MATRIX 2------------\n";

                    for (int i = 0; i < dim_x_sub; i++)
                    {
                    for (int j = 0; j < dim_y_sub; j++)
                    {
                        std::cout << sub_matrix2[i][j][1] << "  ";
                    }
                        std::cout << "\n";
                    }
                    std::cout << std::endl;
                    
                }
            }
        }

        /*Eseguiamo la somma*/
        for (int i = 0; i < dim_x_sub; i++)
            {
                for (int j = 0; j < dim_y_sub; j++)
                {
                    for (int k = 0; k < dim_z_sub; k++)
                    {
                        sub_matrix3[i][j][k] = sub_matrix1[i][j][k]+sub_matrix2[i][j][k];
                    }
                }
            }

        MPI_Gather(&sub_matrix3, volume, MPI_DOUBLE, &matrix3_fake, volume, MPI_DOUBLE, 0, TwoD_com);

        end_time = MPI_Wtime();

        if (irank == master)
        {
            /*Assegno i valori alla matrice di dimensioni corrette*/
            for (int i = 0; i < dim_x; i++)
            {
                for (int j = 0; j < dim_y; j++)
                {
                    for (int k = 0; k < dim_z; k++)
                    {
                        matrix3[i][j][k] =  matrix3_fake[i][j][k];
                    }
                }
            }

            if (check == 1) {
            std::cout << "I am " << irank << " and my values are\n";
            std::cout << "------------MATRIX 3------------\n";
            for (int i = 20; i < dim_x; i++)
            {
                for (int j = 0; j < dim_y; j++)
                {
                std::cout << matrix3[i][j][0] << "  ";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;
            }


            /*Confronto la matrice true con il mio risultato ottenuto dal gather*/
            for (int i = 0; i < dim_x; i++)
            {
                for (int j = 0; j < dim_y; j++)
                {
                    for (int k = 0; k < dim_z; k++)
                    {
                        if (matrix3[i][j][k] != matrix_true[i][j][k])
                        {
                            std::cout << "Valori diversi negl' indici " << i << " " << j << " " << k << std::endl;
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
    }


    MPI_Finalize() ;
}

double d_gen(double Min_val, double Max_val)
{
    double f = (double)rand() / RAND_MAX;
    return Min_val + f*(Max_val - Min_val);
}