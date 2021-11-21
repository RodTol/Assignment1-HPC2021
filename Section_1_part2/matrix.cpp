#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <vector>
#define SEED 35791246
/*NELLA VERSIONE FINALE METTI IL GENERATORE DI SEED RANDOM*/

double d_gen(double Min_val, double Max_val);

int main ( int argc , char *argv[ ] )
{
    int n_proc, irank;
    int dim_x, dim_y, dim_z;
    std::vector<std::vector< std::vector <double> > > matrix1, matrix2;

    dim_x = atoi(argv[1]);
    dim_y = atoi(argv[2]);
    dim_z = atoi(argv[3]);
    
    std::cout << "Dimensioni " << dim_x << ' ' <<  dim_y << ' ' << dim_z << std::endl;

    matrix1.resize(dim_x);
    for (int i = 0; i < dim_x; i++)
    {
        matrix1[i].resize(dim_y);
        for (int j = 0; j < dim_y; j++)
        {
            matrix1[i][j].resize(dim_z);
        }  
    }
    
    matrix2.resize(dim_x);
    for (int i = 0; i < dim_x; i++)
    {
        matrix2[i].resize(dim_y);
        for (int j = 0; j < dim_y; j++)
        {
            matrix2[i][j].resize(dim_z);
        }  
    }

    /*Creiamo i valori casuali*/
    for (int i = 0; i < dim_x; i++)
    {
        for (int j = 0; j < dim_y; j++)
        {
            for (int k = 0; k < dim_z; k++)
            {
                matrix1[i][j][k] = d_gen(0,100);
                matrix2[i][j][k] = d_gen(0,100);
            }
            
        }
        
    }
    

    std::cout << "------------MATRIX 1------------\n";
    /*Check dei valori di una fetta*/
    for (int i = 0; i < dim_x; i++)
    {
     for (int j = 0; j < dim_y; j++)
     {
        std::cout << matrix1[i][j][1] << "  ";
     }
     
        std::cout << "\n";
    }
    std::cout << "------------MATRIX 2------------\n";
    /*Check dei valori di una fetta*/
    for (int i = 0; i < dim_x; i++)
    {
     for (int j = 0; j < dim_y; j++)
     {
        std::cout << matrix2[i][j][1] << "  ";
     }
     
        std::cout << "\n";
    }
    std::cout << std::endl;
   

    MPI_Init(&argc,&argv);
    /*Mi dice quanti processori sono assegnati a un comunicatore*/
    MPI_Comm_size(MPI_COMM_WORLD,&n_proc);

    MPI_Finalize() ;
}

double d_gen(double Min_val, double Max_val)
{
    double f = (double)rand() / RAND_MAX;
    return Min_val + f*(Max_val - Min_val);
}