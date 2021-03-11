#include <sys/time.h>
#include <stdio.h>
#include "mpi.h"
#define N               4        /* number of rows and columns in matrix */

MPI_Status status;

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

int result[N][N]{  
        {0,0,0,0},  
        {0,0,0,0},  
        {0,0,0,0},  
        {0,0,0,0}  
    };;

int main(int argc, char **argv)
{
    int numtasks,taskid,numworkers,source,dest = 0;
    
    int x, y, i, j, xy, nbProduit, nbProduitTotal = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    
    numworkers = numtasks-2;
    

    if(numworkers < 1){
        MPI_Finalize();
        return -1;
    }

    /*----------- taches sur le master sender :------*/
    if (taskid == 0) {
        printf("NumTasks : %d   \n", numtasks);
        printf("Numworkers : %d   \n", numworkers);

        /* send matrix data to the worker tasks */
        nbProduitTotal=N*N*N;
        int resteProduit = nbProduitTotal % numworkers;

        MPI_Send(&nbProduitTotal, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        for (dest=2; dest<=(numworkers+1); dest++)
        {
            nbProduit=nbProduitTotal/numworkers;
            if(resteProduit > 0){
                nbProduit+=1;
                resteProduit-=1;
            }
            printf("Envoi : %d vers le worker %d  \n", nbProduit, dest);
            MPI_Send(&nbProduit, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
        
        dest=0;
        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = 0; j < N; j++)
            {
                for (size_t k = 0; k < N; k++)
                {
                    x=m1[i][k];
                    y=m2[k][j];
                    MPI_Send(&x, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    MPI_Send(&y, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    MPI_Send(&i, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    MPI_Send(&j, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    dest+=1;
                }            
            } 
        }
    
    }

    /*----------- taches sur le master Receiver :------*/
    if (taskid == 1) {
        source=0;

        MPI_Recv(&nbProduitTotal, 1, MPI_INT, source, 0, MPI_COMM_WORLD,  &status);

        printf("master Receiver %d, receive nbProduitTotal %d   \n", taskid,nbProduitTotal);

        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = 0; j < N; j++)
            {
                for (size_t k = 0; k < N; k++)
                {
                    MPI_Recv(&xy, 1, MPI_INT, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                    MPI_Recv(&i, 1, MPI_INT, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                    MPI_Recv(&j, 1, MPI_INT, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                    source+=1;
                    result[i][j]+=xy;
                }            
            } 
        }

        printf("Here is the result matrix:\n");
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++)
                printf("%d   ", result[i][j]);
            printf ("\n");
        }
    }

    /*---------------------------- worker----------------------------*/
    if (taskid > 1) {
        source = 0;
        dest = 1;

        MPI_Recv(&nbProduit, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);

        printf("worker %d, receive nb produit %d  \n ", taskid,nbProduit);

        for (size_t k = 0; k < nbProduit; k++)
        {
            MPI_Recv(&x, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&y, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&i, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&j, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);

            xy = x* y;
            MPI_Send(&xy, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&i, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&j, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
        
    }
    
    MPI_Finalize();

    return 0;
}
