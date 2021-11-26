#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <iostream>

#define SEED 35791246
/*NELLA VERSIONE FINALE METTI IL GENERATORE DI SEED RANDOM*/

double d_gen(double Min_val, double Max_val);

int main ( int argc , char *argv[ ] )
{
    /*VARIABILE PER ATTIVARE CONTROLLI EXTRA*/
    int check = 0, flag = 1;
    double start_time, end_time;
    int n_proc, irank, master = 0;
    int periodic = 0, left_p, right_p;

    int dim_x, dim_y, dim_z, volume;

    dim_x = atoi(argv[1]);
    dim_y = atoi(argv[2]);
    dim_z = atoi(argv[3]);

    /*Creiamo le matrici base*/
    double*** matrix1;
    matrix1 = new double**[dim_x];

    for (int i = 0; i < dim_x; i++)
        {
            matrix1[i] = new double*[dim_y];
            for (int j = 0; j < dim_y; j++)
            {
                matrix1[i][j] =  new double [dim_z];
            }
        }


    for (int i = 0; i < dim_x; i++)
        {
            for (int j = 0; j < dim_y; j++)
            {
                for (int k = 0; k < dim_z; k++)
                {
                    matrix1[i][j][k] = d_gen(0,100);
                }
            }
        }

    std::cout << "------------MATRIX 1------------\n";
    for (int i = 0; i < dim_x; i++)
    {
    for (int j = 0; j < dim_y; j++)
    {
        std::cout << matrix1[i][j][1] << "  ";
    }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

double d_gen(double Min_val, double Max_val)
{
    double f = (double)rand() / RAND_MAX;
    return Min_val + f*(Max_val - Min_val);
}