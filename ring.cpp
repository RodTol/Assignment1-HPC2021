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
    int recv_tag;
    int l_value, r_value;

    MPI_Status status;
    MPI_Request request;
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
        }
    }
    

    std::cout << "Starting point" << std::endl;
    std::cout << "I am " << irank << " and my value is " << r_value << std::endl;


    for (int i = 0; i < n_proc; i++)
    {
        if (i == 0) {
            MPI_Send(&r_value, 1, MPI_INT, right_p, irank*10, ring_com);
        }
        else  {
            MPI_Send(&r_value, 1, MPI_INT, right_p, recv_tag, ring_com);
        }

        MPI_Recv( &r_value, 1, MPI_INT, left_p, MPI_ANY_TAG, ring_com, &status );
        /*dentro la variabile status è salvato il tag del messagio ricevuto, quindi
        la posso salvare per il prossimo send*/
        recv_tag = status.MPI_TAG;
        n_mess += 1;

        if (i < n_proc-1)
        {
            r_value += irank;
        }
        std::cout << "Iteration number " << i << std::endl;
        std::cout << "I am " << irank << " and my value is " << r_value << " with tag " << recv_tag  <<std::endl;
    }
    
    
    std::cout << "I am process " << irank << " and I have recieved " << n_mess <<
     " messages. My final messages have tag " << recv_tag << " and value " << l_value << " , " 
     << r_value <<std::endl;

    MPI_Finalize() ; // let MPI finish up /
}