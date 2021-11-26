#include <vector>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <chrono>
#include <thread>

#define SEED 35791246
/*NELLA VERSIONE FINALE METTI IL GENERATORE DI SEED RANDOM*/

double d_gen(double Min_val, double Max_val);


int main ( int argc , char *argv[ ] )
{
  std::vector<std::vector< std::vector <double> > > matrix1, matrix2;
  std::vector<std::vector< std::vector <double> > > sub_matrix1, sub_matrix2;
  int n_proc, irank;
  int periodic = 0, left_p, right_p;
  int dim_x, dim_y, dim_z;
  
  int dim_x_sub, dim_y_sub, dim_z_sub;

  MPI_Comm OneD_com;

  int dimensioni[39][3] = {
    {24, 1, 1} ,
    /*2-dinension*/
    {2, 12, 1} ,
    {3, 8, 1} ,
    {4,6,1} ,
    {6,4,1} ,
    {8,3,1} ,
    {12, 2, 1} ,
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


  MPI_Init(&argc,&argv);
  /*Assegno il rank usando questo comunicatore*/
  MPI_Comm_rank(MPI_COMM_WORLD,&irank);
  /*Mi dice quanti processori sono assegnati a un comunicatore*/
  MPI_Comm_size(MPI_COMM_WORLD,&n_proc);

  dim_x = atoi(argv[1]);
  dim_y = atoi(argv[2]);
  dim_z = atoi(argv[3]);

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

  if (irank == 0)
    {
      for (int j = 0; j < dim_y; j++)
	std::cout << "Dimensioni " << dim_x << ' ' <<  dim_y << ' ' << dim_z << std::endl;    

      /*Creiamo i valori casuali nel core 0*/
      for (int i = 0; i < dim_x; i++)
        {
	  for (int k = 0; k < dim_z; k++)
            for (int j = 0; j < dim_y; j++)
	      {
                matrix1[i][j][k] = d_gen(0,100);
                matrix2[i][j][k] = d_gen(0,100);
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
    }


  /*Creo un comunicatore per una virtual topology 1-D*/
  MPI_Cart_create( MPI_COMM_WORLD, 1, &n_proc, &periodic, 1, &OneD_com);
  /*Definisco il movimento a destra */
  MPI_Cart_shift( OneD_com, 0, 1, &left_p, &right_p );
  /*Assegno il rank usando questo comunicatore*/
  MPI_Comm_rank(OneD_com,&irank);
  /*Indico quanti processori sono assegnati al comunicatore*/
  MPI_Comm_size(OneD_com,&n_proc);

  dim_x_sub = dim_x/dimensioni[0][0];
  dim_y_sub = dim_y/dimensioni[0][1];
  dim_z_sub = dim_z/dimensioni[0][2];

  sub_matrix1.resize(dim_x_sub);
  for (int i = 0; i < dim_x_sub; i++)
    {
      sub_matrix1[i].resize(dim_y_sub);
      for (int j = 0; j < dim_y_sub; j++)
	{
	  sub_matrix1[i][j].resize(dim_z_sub);
	}  
    }

  sub_matrix2.resize(dim_x_sub);
  for (int i = 0; i < dim_x_sub; i++)
    {
      sub_matrix2[i].resize(dim_y_sub);
      for (int j = 0; j < dim_y_sub; j++)
	{
	  sub_matrix2[i][j].resize(dim_z_sub);
	}  
    }

  MPI_Scatter(&((matrix1.front()).front()).front(), 1, MPI_DOUBLE, &((sub_matrix1.front()).front()).front(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  std::cout << "fatto scatter" << std::endl;

  for (int i = 0; i < n_proc; i++)
    {
      if (irank == i)
        {
	  std::cout << "I am " << irank << " and my values are\n";

	  std::cout << "------------MATRIX 1------------\n";
	  /*Check dei valori di una fetta*/
	  for (int i = 0; i < dim_x_sub; i++)
            {
	      for (int j = 0; j < dim_y_sub; j++)
		{
		  std::cout << sub_matrix1[i][j][1] << "  ";
		}
	      std::cout << "\n";
            }
	  std::cout << "------------MATRIX 2------------\n";
	  /*Check dei valori di una fetta*/
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

  MPI_Finalize() ;
}


double d_gen(double Min_val, double Max_val){
  double f = (double)rand()/ RAND_MAX;
  return Min_val + f*(Max_val-Min_val);

}
