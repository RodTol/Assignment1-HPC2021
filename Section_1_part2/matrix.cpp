#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <chrono>
#include <thread>

#define SEED 35791246

double d_gen(double Min_val, double Max_val);

int main(int argc, char* argv[]) {

	
	int dimensions[39][3] = {
		{24, 1, 1} ,
	    	/*2-dinensional*/
		{12, 2, 1} ,
		{3, 8, 1} ,
		{4,6,1} ,
		{6,4,1} ,
		{8,3,1} ,
		{2, 12, 1} ,
		{1,24,1} ,
		{24,1,1} ,
	    	/*3-dimensional*/
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



	int dim_x = atoi(argv[1]);
	int dim_y = atoi(argv[2]);
	int dim_z = atoi(argv[3]);

	int volume;

	int size;
	int irank;
	int periodic=0, master=0;
	
	// Communicator
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &irank);



	double start_time, end_time;	

	// The 2 matrices to be sum and the result matrix are created 
	double matrix1[dim_x][dim_y][dim_z];
        double matrix2[dim_x][dim_y][dim_z];
	double matrix_result[dim_x][dim_y][dim_z];	

	// Random values are assigned to the initial matrices
	if (irank == master) {
		for (int i{0}; i < dim_x; i++) {
			for (int j{0}; j < dim_y; j++) {
				for (int k{0}; k < dim_z; k++) {
					matrix1[i][j][k] = d_gen(0,100);
					matrix2[i][j][k] = d_gen(0,100);
				}
			}
		}
	}
	
	// A new communicator for the 1D case is created
	MPI_Comm comm1d;
	MPI_Cart_create(MPI_COMM_WORLD, 1, &size, &periodic, 1, &comm1d);
	//The rank is assigned
    	MPI_Comm_rank(comm1d,&rank);
    	//How many processors are assigned to the communicator
    	MPI_Comm_size(comm1d,&size);


	// The dimensions of the submatrices
    	dim_x_sub = dim_x/dimensions[0][0];
    	dim_y_sub = dim_y/dimensions[0][1];
    	dim_z_sub = dim_z/dimensions[0][2];
    	
    	// Is the reminder null?
    	reminder_x = dim_x - dimensions[0][0]*dim_x_sub;
    	reminder_y = dim_y - dimensions[0][1]*dim_y_sub;
    	reminder_z = dim_z - dimensions[0][2]*dim_z_sub;
    	
    	if (reminder_x !=0)
    	{
		// Expanded dimension
		dim_x_fake = dim_x + dimensions[0][0] - reminder_x ; 
		// Expanded sub-dimension
		dim_x_sub = dim_x_fake/dimensions[0][0];
    	}	
    	if (reminder_y !=0)
    	{
		// Expanded dimension
		dim_y_fake = dim_y + dimensions[0][1] - reminder_y;  
		// Expanded sub-dimension
		dim_y_sub = dim_y_fake/dimensions[0][1];
    	}
    	if (resto_z !=0)
    	{
        	// Expanded dimension
        	dim_z_fake = dim_z + dimensioni[0][2] - resto_z; 
        	// Expanded sub-dimension
        	dim_z_sub = dim_z_fake/dimensioni[0][2];
    	}	
    	
    	// The expanded matrix are initialised
	double matrix1fake[dim_x_fake][dim_y_fake][dim_z_fake];
        double matrix2fake[dim_x_fake][dim_y_fake][dim_z_fake];
        double matrix_result_fake[dim_x_fake][dim_y_fake][dim_z_fake];

	double submatrix1[dim_x_sub][dim_y_sub][dim_z_sub];
        double submatrix2[dim_x_sub][dim_y_sub][dim_z_sub];
        double submatrix_result[dim_x_sub][dim_y_sub][dim_z_sub];
        
        volume =  dim_x_sub*dim_y_sub*dim_z_sub;
        
        // The values are assigned to the extended matrix in the root
	if (irank == master) {		
		for (int i{0}; i < dim_x; i++) {
			for (int j{0}; j < dim_y; j++) {
				for (int k{0}; k < dim_z; k++) {
					matrix1fake[i][j][k] = matrix1[i][j][k];
                                	matrix2fake[i][j][k] = matrix2[i][j][k];					
				}
			}
		}
	}
	
	start_time = MPI_Wtime();

	// Scatter
	MPI_Scatter(&matrix1fake, volume, MPI_DOUBLE, &submatrix1, volume, MPI_DOUBLE, 0, comm1d);
        MPI_Scatter(&matrix2fake, volume, MPI_DOUBLE, &submatrix2, volume, MPI_DOUBLE, 0, comm1d);
        
        // The sums are performed
	for (int i{0}; i < dim_x_sub; i++) {
		for (int j{0}; j < dim_y_sub; j++) {
			for (int k{0}; k < dim_z_sub; k++) {
				submatrix_result[i][j][k] = submatrix1[i][j][k] + submatrix2[i][j][k];
			}
		}
	}
	
	// Gather
	MPI_Gather(&submatrix_result, volume, MPI_DOUBLE, &matrix_result_fake, volume, MPI_DOUBLE, 0, comm1d);
	
	end_time = MPI_Wtime();
	total_time = end_time - start_time;
	if (irank == master) {		
		std::cout << " For distribution " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << " time is: " << total_time <<std::endl;
		}
		
	for (int i{0}; i < dim_x; i++) {
			for (int j{0}; j < dim_y; j++) {
				for (int k{0}; k < dim_z; k++) {
					matrix_result[i][j][k] = matrix_result_fake[i][j][k];
				}
			}
		}


/*
	for (int i{1}; i < 9; i++) {

		int dist_x = dimensions[i][0];
		int dist_y = dimensions[i][1];
		int dist_z = dimensions[i][2];

		int distribution[3] = {dist_x, dist_y, dist_z};

		// asking MPI to decompose our processes into a 3D cartesian grid
		int dims[3] = {procs_on_x, procs_on_y, procs_on_z};
		MPI_Dims_create(size, 3, dims);

		// here we make dimensions non periodic
		int periods[3] = {false, false, false};

		// letting MPI decide to assign arbitrarily ranks if it is necessary
		int reorder = true;

		// now we create a new communicator given the new created topology
		MPI_Comm new_comm;
		MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, reorder, &new_comm);

		// to know which is my rank in the new communicator 
		int my_rank;
		MPI_Comm_rank(new_comm, &my_rank);

		// to get my new coordinates in the new communicator
		int my_coords[3];
		MPI_Cart_coords(new_comm, my_rank, 3, my_coords);

		// to check if evertything is working fine
		// std::cout << "I am proc, old " << old_rank << ", new " << my_rank << " and I am located at coordinates " << my_coords[0] << my_coords[1] << my_coords[2];
 
		
		// now I can compute the size of the extended matrix, which are the ones with the side size divisible by the corresponding number of processes
		int extended_size[3] = {0, 0, 0};
		
		for (int i{0}; i < 3; i++) {
			if (sizes[i] % distribution[i] == 0) {
				extended_size[i] = sizes[i];
			}
			else {
				extended_size[i] = sizes[i] + (distribution[i] - sizes[i] % distribution[i]);
			}
		}
		if (my_rank == 0) {
			//std::cout << extended_size[0] << " " << extended_size[1] << " " << extended_size[2] << "\n";
		}		


		// now I can create the extended matrices and the submatrices in which the scatter will be executed
		double matrix_A_ext[extended_size[0]][extended_size[1]][extended_size[2]];
                double matrix_B_ext[extended_size[0]][extended_size[1]][extended_size[2]];
                double matrix_final_ext[extended_size[0]][extended_size[1]][extended_size[2]];

		int submatrix_sizes[3] = {extended_size[0]/distribution[0], extended_size[1]/distribution[1], extended_size[2]/distribution[2]};

		double submatrix_A[submatrix_sizes[0]][submatrix_sizes[1]][submatrix_sizes[2]];
                double submatrix_B[submatrix_sizes[0]][submatrix_sizes[1]][submatrix_sizes[2]];
                double submatrix_sums[submatrix_sizes[0]][submatrix_sizes[1]][submatrix_sizes[2]];
		
		// now in the master, I create the extended matrices:
		if (my_rank == 0) {		
			for (int i{0}; i < dim_x; i++) {
				for (int j{0}; j < size_2_dim; j++) {
					for (int k{0}; k < size_3_dim; k++) {
						matrix_A_ext[i][j][k] = matrix_A[i][j][k];
                                        	matrix_B_ext[i][j][k] = matrix_B[i][j][k];					
					}
				}
			}
		}


		// I can finally perform the scatter
		int n_elem_to_each_process = submatrix_sizes[0]*submatrix_sizes[1]*submatrix_sizes[2]; 

		t_start = MPI_Wtime();

		MPI_Scatter(&matrix_A_ext, n_elem_to_each_process, MPI_DOUBLE, &submatrix_A, n_elem_to_each_process, MPI_DOUBLE, 0, new_comm);
                MPI_Scatter(&matrix_B_ext, n_elem_to_each_process, MPI_DOUBLE, &submatrix_B, n_elem_to_each_process, MPI_DOUBLE, 0, new_comm);
 
		// std::cout << " I am process " << my_rank << " and I am done scattering \n";  


		// do the summations
		for (int i{0}; i < submatrix_sizes[0]; i++) {
			for (int j{0}; j < submatrix_sizes[1]; j++) {
				for (int k{0}; k < submatrix_sizes[2]; k++) {
					submatrix_sums[i][j][k] = submatrix_A[i][j][k] + submatrix_B[i][j][k];
				}
			}
		}

		
		// perform the gather now
		MPI_Gather(&submatrix_sums, n_elem_to_each_process, MPI_DOUBLE, &matrix_final_ext, n_elem_to_each_process, MPI_DOUBLE, 0, new_comm);
		
		t_end = MPI_Wtime();

		// print the timings
		double total_time_3D = t_end - t_start;
		
		if (my_rank == 0) {		
			std::cout << " for distribution " << distribution[0] << " " << distribution[1] << " " << distribution[2] << " time is: " << total_time_3D <<std::endl;
		}


		// last thing to do is to assign the correct values to the final matrix, which is the one with the correct dimensions (the same as the input matrices)
		for (int i{0}; i < dim_x; i++) {
			for (int j{0}; j < size_2_dim; j++) {
				for (int k{0}; k < size_3_dim; k++) {
					matrix_final[i][j][k] = matrix_final_ext[i][j][k];
				}
			}
		} 
/*

		if (my_rank == 0) {
			// check on a slice of the matrix
	        	for (int i{0}; i < dim_x; i++) {
        	        	for (int j{0}; j < size_2_dim; j++) {
                	        	std::cout << matrix_A[i][j][0] << " ";
                		}
				std::cout << "\n ";
       			}
			std:: cout << "\n";



                	for (int i{0}; i < dim_x; i++) {
                        	for (int j{0}; j < size_2_dim; j++) {
                                	std::cout << matrix_B[i][j][0] << " ";
                        	}
                        	std::cout << "\n ";
                	}
			std::cout << "\n";


                	for (int i{0}; i < dim_x; i++) {
                        	for (int j{0}; j < size_2_dim; j++) {
                                	std::cout << matrix_final[i][j][0] << " ";
                        	}
                       		std::cout << "\n ";
                	}
			std::cout << "\n";
		}

*/
	}


	MPI_Finalize();
	return 0;
}


double d_gen(double Min_val, double Max_val)
{
    double f = (double)rand() / RAND_MAX;
    return Min_val + f*(Max_val - Min_val);
}

