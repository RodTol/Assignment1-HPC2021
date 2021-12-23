#include <mpi.h>
#include <chrono>
#include <thread>

int main (int argc , char *argv[]) {
 
    int irank, n_proc, t = 1, n_run = 1000;
    int n_mess = 0;
    int right_p, left_p;
    int tag_r, tag_l;
    int l_value, r_value;

    double start_time, end_time, time, sum_time;
    double sum_of_times = 0, time_tot;

    MPI_Status status_r, status_l;
    MPI_Comm ring_com;

    /*I initialize the MPI process*/
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&n_proc);

    /*I create the virtual topology and define the left and right neighbour*/
    MPI_Cart_create( MPI_COMM_WORLD, 1, &n_proc, &t, 1, &ring_com );
    MPI_Cart_shift( ring_com, 0, 1, &left_p, &right_p );
    MPI_Comm_rank(ring_com,&irank);
    MPI_Comm_size(ring_com,&n_proc);

   /*Each execution of the program will make n_run ring routines and after that
   calculate the mean of the times*/ 
    for (int run = 0; run < n_run; run++) {

        /*I initialize each processor with its values*/
        for (int i = 0; i < n_proc; i++) {
            if (irank == i) {
                l_value = -i;
                r_value = i;
                tag_r = i*10;
                tag_l = i*10;
            }
        }

        start_time = MPI_Wtime();
        
        /*I'll make n_proc step and in each of them every processor will
        send and recieve a message from both its right and left neighbour*/
        for (int i = 0; i < n_proc; i++) {

            /*Right neighbour*/
            MPI_Send(&r_value, 1, MPI_INT, right_p, tag_r, ring_com);
            MPI_Recv( &r_value, 1, MPI_INT, left_p, MPI_ANY_TAG, ring_com, &status_r );
            /*The tag must be the same for all the routine, so I update its value from
             the MPI_Status variable*/
            tag_r = status_r.MPI_TAG;
            /*I update the count of message received*/
            n_mess += 1;

            /*Left neighbour*/
            MPI_Send(&l_value, 1, MPI_INT, left_p, tag_l, ring_com);
            MPI_Recv( &l_value, 1, MPI_INT, right_p, MPI_ANY_TAG, ring_com, &status_l );
            /*Same as above*/
            tag_l = status_l.MPI_TAG;
            n_mess += 1;

            /*At each step I have to add or subtract the rank to the value
             of the message. I excluded the last step in order to achieve the
             same value for all the processor.*/
            if (i < n_proc-1)
            {
                r_value += irank;
                l_value -= irank;
            }
        }

        end_time=MPI_Wtime();

        /*A simply check of the tags in order to understand if the
         routine has any problem*/
        if (tag_r != tag_l) {  
            std::cout << "The two tags don't match" << std::endl;
            MPI_Finalize() ; 
            exit;
        }

        /*I print my results*/
        if (run == n_run-1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50*irank));
            std::cout << "I am process " << irank << " and I have received " << n_mess <<
           " messages. My final messages have tag " << tag_r << " and value " << l_value << " , " 
             << r_value << std::endl;
        }       

        /*I sum the times of all the runs*/
        time = end_time - start_time;
        if (irank == 0) {
            sum_of_times += time;
         }

        n_mess = 0;
    }

    /*Mean of the times of the n_runs runs*/
    if (irank == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50*n_proc+10));
        std::cout << "The mean of time is :" << sum_of_times/double(n_run) << std::endl;
    }

    /*I end the MPI process*/
    MPI_Finalize();
}