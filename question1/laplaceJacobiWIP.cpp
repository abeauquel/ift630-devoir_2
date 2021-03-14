#include <sys/time.h>
#include <stdio.h>
#include <cstdlib>
#include "mpi.h"
#define N               4        /* number of rows and columns in matrix */
#define K               5      
#define EPSILON         0.1
#define LIMIT         -1.0

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
    int numtasks,taskid,numworkers,source,dest = 0;
    
    int i, j, nbProduit, nbProduitTotal = 0;
    double x, y, z, w, moyenne = 0.0;
    int closeEnough = 0;

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

        nbProduitTotal=N*N; // 2* (N*N*K)/2 ///////////// (N*N)/2*K   *2

        int resteProduit = nbProduitTotal % numworkers;

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

        for (size_t k = 0; k < K; k++)
        {
            dest=0;
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    x= (i-1) < 0 ? LIMIT : m1[i-1][j];
                    y=(j-1) < 0 ? LIMIT : m1[i][j-1];
                    z=(i+1) >= N ? LIMIT : m1[i+1][j];
                    w=(j+1) >= N ? LIMIT : m1[i][j+1];

                    MPI_Send(&x, 1, MPI_DOUBLE, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    MPI_Send(&y, 1, MPI_DOUBLE, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    MPI_Send(&z, 1, MPI_DOUBLE, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    MPI_Send(&w, 1, MPI_DOUBLE, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    MPI_Send(&i, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    MPI_Send(&j, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
                    dest+=1;
                }
            }

           
            MPI_Recv(&result, N*N, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD,  &status);
            printf("result   \n");

            closeEnough=0;
            int limit= abs(LIMIT);
            int epsylon = abs(EPSILON);
                        
            for (size_t i = 0; i < N; i++)
            {
                for (size_t j = 0; j < N; j++)
                {
                    m1[i][j]=result[i][j];
                    int x=abs(m1[i][j]);

                    if(closeEnough > 0 && (x >= (limit - epsylon)) && (x <= (limit + epsylon))){
                        closeEnough=1;
                    }
                }
            }
            for (dest=1; dest<=(numworkers+1); dest++)
            {
                printf("Envoi closeEnough : %d vers le worker %d  \n", closeEnough, dest);
              //  MPI_Send(&closeEnough, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                //MPI_Send(&j, 1, MPI_INT, (dest % (numworkers))+2, 0, MPI_COMM_WORLD);
            }

            // if(closeEnough >0)
            //     k=K;

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
        dest,closeEnough, i,j=0;
        
        for (size_t k = 0; k < K; k++)
        {
            source=0;
            for (size_t v = 0; v < N*N; v++)
            {
               
                MPI_Recv(&moyenne, 1, MPI_DOUBLE, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&i, 1, MPI_INT, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&j, 1, MPI_INT, (source % (numworkers))+2, 0, MPI_COMM_WORLD, &status);
                result[i][j]=moyenne;
                 //printf("Recv moyenne i : %d, j : %d\n", i,j);
                source+=1;
            }
            source=0;
             printf("Send result \n");
            MPI_Send(&result, N*N, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);

           // MPI_Recv(&closeEnough, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            //printf("Recv worker %d  , k=%d \n", taskid, k);

            // if(closeEnough >0)
            //     //k=K;
        }
        
    }

    /*---------------------------- worker----------------------------*/
    if (taskid > 1) {
        closeEnough, source = 0;
        dest = 1;

        MPI_Recv(&nbProduit, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);

        printf("worker %d, receive nb produit %d  \n ", taskid,nbProduit);
        for (size_t k = 0; k < K; k++)
        {
            for (size_t z = 0; z < nbProduit; z++)
            {
                MPI_Recv(&x, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&y, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&z, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&w, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&i, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&j, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);

                moyenne = (x+y+z+w)/4;
                printf("Send moyenne i : %d, j : %d\n", i,j);
                MPI_Send(&moyenne, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&i, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&j, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

            }

            //MPI_Recv(&closeEnough, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
           // printf("Recv worker %d  , k=%d \n", taskid, k);

            // if(closeEnough > 0)
            //     //k=K;
        }
        
    }
    
    printf("Finalize worker %d   \n", taskid);
    MPI_Finalize();

    return 0;
}
