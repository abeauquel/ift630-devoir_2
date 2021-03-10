#include <sys/time.h>
#include <stdio.h>
#include "mpi.h"
#define N               4        /* number of rows and columns in matrix */

int m1[N][N]= {  
        {1,2,3,4},  
        {5,6,7,8},  
        {9,10,11,12},  
        {13,14,15,16}  
    };

int m2[N][N]= {  
        {17,18,19,20},  
        {21,22,23,24},  
        {25,26,27,28},  
        {29,30,31,32}  
    };

int result[N][N];

main(int argc, char **argv)
{
    int numtasks,taskid,numworkers,source,dest,rows,offset,i,j,k;
    struct timeval start, stop;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    
    numworkers = numtasks-1;

    /*----------- taches sur le master :------*/
    if (taskid == 0) {
        gettimeofday(&start, 0);

        /* send matrix data to the worker tasks */
        rows = N/numworkers;
        offset = 0;
        for (dest=1; dest<=numworkers; dest++)
        {
        MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
        MPI_Send(&m1[offset][0], rows*N, MPI_INT,dest,1, MPI_COMM_WORLD);
        MPI_Send(&m2, N*N, MPI_INT, dest, 1, MPI_COMM_WORLD);
        offset = offset + rows;
        }
    }

    /*---------------------------- worker----------------------------*/
    if (taskid > 0) {
    }
}
