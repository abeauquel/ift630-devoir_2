#include <sys/time.h>
#include <stdio.h>
#include "mpi.h"
#define N               4        /* number of rows and columns in matrix */
#define T               -1
#define K               20

MPI_Status status;

int m1[N+2][N+2]= {
        {-1, -1, -1, -1, -1, -1},
        {-1, 1, 2, 3, 4, -1},  
        {-1, 5, 6, 7, 8, -1},  
        {-1, 9, 10, 11, 12, -1},  
        {-1, 13, 14, 15, 16, -1},
        {-1, -1, -1, -1, -1, -1}  
    };

int result[N+2][N+2];

int main(int argc, char **argv)
{
    int getValue(int i, int j);
    int numtasks,taskid,numworkers,source,dest = 0;
    
    int x, y, i, j, xy, nbProduit, nbProduitTotal = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    
    numworkers = numtasks-1;

    if(numworkers < 1){
        MPI_Finalize();
        return -1;
    }

    /*----------- taches sur le master :------*/
    if (taskid == 0) {
        printf("NumTasks : %d   \n", numtasks);
        printf("Numworkers : %d   \n", numworkers);


        //Init result matrix
        for(int i = 0; i < N + 2; i++){
            for(int j = 0; j < N + 2; j++){
                if(i == 0 || j == 0)
                    result[i][j] = T;
                result[i][j] = 0;
            }
        }


        /* send matrix data to the worker tasks */
        nbProduitTotal=N*N*K;
        int resteProduit = nbProduitTotal % numworkers;

        for (dest=1; dest<=numworkers; dest++)
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

int getValue(int i, int j){
    if(i < 0 || j < 0 || i > N-1 || j > N-1)
        throw;
    if(i == 0 || j == 0)
        return T;
    else
        return m1[i][j];
}
