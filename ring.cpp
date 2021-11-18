#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fstream>

int main ( int argc , char *argv[ ] )
{
    int irank, t = 1;
    int n_proc, n_mess = 0;
    int right_p, left_p;
    int recv_tag_r, recv_tag_l;
    int l_value, r_value;
    double start_time, end_time, time, sum_time;
    double time_tot;
    MPI_Status status_r, status_l, status;
    MPI_Comm ring_com;

    MPI_Init(&argc,&argv);
    /*Mi dice quanti processori sono assegnati a un comunicatore*/
    MPI_Comm_size(MPI_COMM_WORLD,&n_proc);

    /*Cart create crea un communicator da un altro communicator
    che rispetta la topologia da noi scelta. I suoi argoemtni sono
    il vecchio communicator, numero righe griglia, numero colonne griglia
    un array di true o false a indicare se la riga è periodica,
    una variabile binaria che decide se riordinare o no l'array, e il 
    nome del nuovo communicator*/
    MPI_Cart_create( MPI_COMM_WORLD, 1, &n_proc, &t, 1, &ring_com );
    /*Definisco il movimento a destra */
    MPI_Cart_shift( ring_com, 0, 1, &left_p, &right_p );
    /*Assegno il rank usando questo comunicatore*/
    MPI_Comm_rank(ring_com,&irank);
    /*Indico quanti processori sono assegnati al comunicatore*/
    MPI_Comm_size(ring_com,&n_proc);

    /*Assegno il valore iniziale a ogni processore, che è uguale al suo rank*/
    for (int i = 0; i < n_proc; i++)
    {
        if (irank == i)
        {
            l_value = -i;
            r_value = i;
            recv_tag_r = i*10;
            recv_tag_l = i*10;
        }
    }
    

    //std::cout << "Starting point" << std::endl;
    //std::cout << "I am " << irank << " and my value is " << r_value << std::endl;

    start_time = MPI_Wtime();
    for (int i = 0; i < n_proc; i++)
    {
        /*GIRO DI DESTRA*/
        MPI_Send(&r_value, 1, MPI_INT, right_p, recv_tag_r, ring_com);
        MPI_Recv( &r_value, 1, MPI_INT, left_p, MPI_ANY_TAG, ring_com, &status_r );
        /*dentro la variabile status è salvato il tag del messagio ricevuto, quindi
        la posso salvare per il prossimo send*/
        recv_tag_r = status_r.MPI_TAG;
        n_mess += 1;

        /*GIRO DI SINISTRA*/
        MPI_Send(&l_value, 1, MPI_INT, left_p, recv_tag_l, ring_com);
        MPI_Recv( &l_value, 1, MPI_INT, right_p, MPI_ANY_TAG, ring_com, &status_l );
        /*dentro la variabile status è salvato il tag del messagio ricevuto, quindi
        la posso salvare per il prossimo send*/
        recv_tag_l = status_l.MPI_TAG;
        n_mess += 1;

        /*Sommo o sottraggo il mio rank a ogni mesaggio che mi arriva*/
        if (i < n_proc-1)
        {
            r_value += irank;
            l_value -= irank;
        }
        //std::cout << "Iteration number " << i << std::endl;
        //std::cout << "I am " << irank << " and my value is " << r_value << " with tag " << recv_tag  <<std::endl;
    }
    
    if (recv_tag_r != recv_tag_l)
    {
        std::cout << "The two tags don't match" << std::endl;
        MPI_Finalize() ; // let MPI finish up 
        exit;
    }
   end_time=MPI_Wtime();

    std::cout << "I am process " << irank << " and I have received " << n_mess <<
     " messages. My final messages have tag " << recv_tag_r << " and value " << l_value << " , " 
     << r_value <<std::endl;
    
    time = end_time - start_time;

    //std::cout << "On process " << irank << " the walltime is " << time << std::endl;

    if (irank == 0) {
     time_tot = time;
     for (int i = 1; i < n_proc; i++) {
        MPI_Recv( &time, 1, MPI_DOUBLE, i, 666, MPI_COMM_WORLD, &status);
        time_tot += time;
     }
    }
    else {
        MPI_Send(&time, 1, MPI_DOUBLE, 0, 666, MPI_COMM_WORLD);
    }

    if (irank == 0)
    {
     std::cout << "The mean is " << time_tot/double(n_proc) << std::endl;
    }
    

    MPI_Finalize() ; // let MPI finish up 
}