#include <sys/time.h>
#include <stdio.h>
#include "mpi.h"
#include <cmath>
#define N               4        /* number of rows and columns in matrix */
#define K               12  
#define LIMIT           -1.0 
#define EPSILON         0.1

MPI_Status status;

double m1[N][N]= {  
        {1.0,1.0,1.0,1.0},  
        {2.0,2.0,2.0,2.0},  
        {3.0,3.0,3.0,3.0},  
        {4.0,4.0,4.0,4.0}  
    };

double result[N][N]{  
        {0.0,0.0,0.0,0.0},  
        {0.0,0.0,0.0,0.0},  
        {0.0,0.0,0.0,0.0},  
        {0.0,0.0,0.0,0.0}  
    };

int main(int argc, char **argv)
{
    int numtasks,taskid,numworkers, closeEnough, qtyToRecv, source,dest = 0;
    
    int i, j, nbProduit, nbProduitTotal = 0;
    double x, y, z, w, moyenne = 0.0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    
    numworkers = numtasks-2;
    

    if(numworkers < 1){
        MPI_Finalize();
        return -1;
    }

    /*----------- taches sur le master :------*/
    if (taskid == 0) {
        printf("NumTasks : %d   \n", numtasks);
        printf("Numworkers : %d   \n", numworkers);

        nbProduitTotal=N*N;

        int resteProduit = nbProduitTotal % numworkers;

        for (int k = 0; k < K; k++)
        {
            dest=0;

            if(closeEnough == 0){ //If there is at least another pass to do
                for (dest=2; dest<=(numworkers+1); dest++)
                {
                    nbProduit=nbProduitTotal/numworkers;
                    if(resteProduit > 0){
                        nbProduit+=1;
                        resteProduit-=1;
                    }
                    //printf("Envoi : %d vers le worker %d  \n", nbProduit,dest);
                    MPI_Send(&nbProduit, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                }

                //Send the receiver how many he will receive
                //printf("Envoi : %d vers le worker 1  \n", nbProduitTotal);
                MPI_Send(&nbProduitTotal, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);



                for (int i = 0; i < N; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        x = (i-1) < 0 ? LIMIT : m1[i-1][j];
                        y = (j-1) < 0 ? LIMIT : m1[i][j-1];
                        z = (i+1) >= N ? LIMIT : m1[i+1][j];
                        w = (j+1) >= N ? LIMIT : m1[i][j+1];

                        MPI_Send(&x, 1, MPI_DOUBLE, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                        MPI_Send(&y, 1, MPI_DOUBLE, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                        MPI_Send(&z, 1, MPI_DOUBLE, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                        MPI_Send(&w, 1, MPI_DOUBLE, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                        MPI_Send(&i, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                        MPI_Send(&j, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                        dest+=1;
                    }
                }

                MPI_Recv(&result, nbProduitTotal, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD,  &status);
                closeEnough = 1;
                for (size_t i = 0; i < N; i++)
                {
                    for (size_t j = 0; j < N; j++)
                    {
                        m1[i][j]=result[i][j];

                        //If at least one value is not close enough, continue
                        if(!(m1[i][j] <= LIMIT + EPSILON && m1[i][j] >= LIMIT - EPSILON)){
                            closeEnough = 0;
                        }

                    }
                }

            }else{ //Send stop notice to all and finalize because we are close enough to the limit
                nbProduit = 0;
                printf("Close enough after %d iterations, stopping...\n", k);

                for (dest=2; dest<=(numworkers+1); dest++)
                {
                    MPI_Send(&nbProduit, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                }

                MPI_Send(&nbProduit, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                k = K;
            }
        }
        if(closeEnough == 0){ //If max K reached, send stop notice
            nbProduit = 0;
            printf("MAX iterations reached, stopping...\n");

            for (dest=2; dest<=(numworkers+1); dest++)
            {
                MPI_Send(&nbProduit, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            }

            MPI_Send(&nbProduit, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }

    
        printf("Here is the result matrix:\n");
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++)
                printf("%f   ", result[i][j]);
            printf ("\n");
        }
    }

    /*----------- taches sur le master Receiver :------*/
    if (taskid == 1) {
        dest, qtyToRecv, i,j=0;

        while(closeEnough == 0){
            //Receive qty to receive
            MPI_Recv(&qtyToRecv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            //printf("worker %d, receive nb produit %d  \n ", taskid,qtyToRecv);


            if(qtyToRecv > 0){
                source=0;
                for (size_t v = 0; v < qtyToRecv; v++)
                {

                    MPI_Recv(&moyenne, 1, MPI_DOUBLE, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                    MPI_Recv(&i, 1, MPI_INT, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                    MPI_Recv(&j, 1, MPI_INT, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                    result[i][j]=moyenne;
                    source+=1;
                }

                MPI_Send(&result, N*N, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            }else{
                closeEnough = 1;
            }
        }
    }

    /*---------------------------- worker----------------------------*/
    if (taskid > 1) {
        source = 0;
        dest = 1;

        MPI_Recv(&nbProduit, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
        //printf("worker %d, receive nb produit %d  \n ", taskid,nbProduit);

        if(nbProduit == 0)
            closeEnough = 1;

        while(closeEnough == 0){

            for (size_t k = 0; k < nbProduit; k++)
            {
                
                MPI_Recv(&x, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&y, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&z, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&w, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&i, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&j, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);

                moyenne = (x+y+z+w)/4;
                MPI_Send(&moyenne, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&i, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&j, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            }
            MPI_Recv(&nbProduit, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            //printf("worker %d, receive nb produit %d  \n ", taskid,nbProduit);

            if(nbProduit == 0)
                closeEnough = 1;
        }
        
    }
    
    printf("Finalize worker %d   \n", taskid);
    MPI_Finalize();

    return 0;
}
