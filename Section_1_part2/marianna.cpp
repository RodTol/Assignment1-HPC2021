#include <iostream>
#include <mpi.h>


int main(int argc, char* argv[]) {

	double t_start, t_end;	
	
	MPI_Init(&argc, &argv);

	// size of default communicator
	int size;
	int old_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &old_rank);

	int size_1_dim = atoi(argv[1]);
	int size_2_dim = atoi(argv[2]);
	int size_3_dim = atoi(argv[3]);
	int sizes[3] = {size_1_dim, size_2_dim, size_3_dim};

	int distributions[39][3] = {{24,1,1},  // 1D 
				   {24,1,1}, {1,24,1}, {2,12,1}, {12,2,1}, {3,8,1}, {8,3,1}, {4,6,1}, {6,4,1},
				   {24,1,1}, {1,24,1}, {1,1,24}, {2,1,12}, {2,12,1}, {1,2,12}, {1,12,2}, {12,1,2}, {12,2,1}, {3,1,8}, {3,8,1}, {1,3,8}, {1,8,3}, {8,1,3},
				   {8,3,1}, {4,1,6}, {4,6,1}, {1,4,6}, {1,6,4}, {6,1,4}, {6,4,1}, {2,3,4}, {2,4,3}, {3,2,4}, {3,4,2}, {4,2,3}, {4,3,2}, {2,2,6}, {2,6,2},
				   {6,2,2}};

	// First, I create the two initial matrices
	double matrix_A[size_1_dim][size_2_dim][size_3_dim];
        double matrix_B[size_1_dim][size_2_dim][size_3_dim];
	double matrix_final[size_1_dim][size_2_dim][size_3_dim];	

	if (old_rank == 0) {
		for (int i{0}; i < size_1_dim; i++) {
			for (int j{0}; j < size_2_dim; j++) {
				for (int k{0}; k < size_3_dim; k++) {
					matrix_A[i][j][k] = ((double) rand()/RAND_MAX);
					matrix_B[i][j][k] = ((double) rand()/RAND_MAX);
				}
			}
		}
	}


	for (int i{0}; i < 39; i++) {

		int procs_on_x = distributions[i][0];
		int procs_on_y = distributions[i][1];
		int procs_on_z = distributions[i][2];

		int distribution[3] = {procs_on_x, procs_on_y, procs_on_z};

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
			for (int i{0}; i < size_1_dim; i++) {
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
		for (int i{0}; i < size_1_dim; i++) {
			for (int j{0}; j < size_2_dim; j++) {
				for (int k{0}; k < size_3_dim; k++) {
					matrix_final[i][j][k] = matrix_final_ext[i][j][k];
				}
			}
		} 
/*

		if (my_rank == 0) {
			// check on a slice of the matrix
	        	for (int i{0}; i < size_1_dim; i++) {
        	        	for (int j{0}; j < size_2_dim; j++) {
                	        	std::cout << matrix_A[i][j][0] << " ";
                		}
				std::cout << "\n ";
       			}
			std:: cout << "\n";



                	for (int i{0}; i < size_1_dim; i++) {
                        	for (int j{0}; j < size_2_dim; j++) {
                                	std::cout << matrix_B[i][j][0] << " ";
                        	}
                        	std::cout << "\n ";
                	}
			std::cout << "\n";


                	for (int i{0}; i < size_1_dim; i++) {
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
