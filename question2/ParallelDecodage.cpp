#include <cstdlib>
#include <iostream>
#include "encodage.cpp"
#include <chrono> 
#include <thread>
#include <future>
#include <stdio.h> 
#include <sys/types.h>

using namespace std::chrono; 
#define TAILLE_MOT               7  

char alphabet[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int myIndex[TAILLE_MOT] = {0,0,0,0,0,0,0}, start1[TAILLE_MOT] = {0,0,0,0,0,0,1}, start2[TAILLE_MOT] = {0,0,0,0,0,0,2};
int found = 0;
string motTest;
string hashTest;
int password[TAILLE_MOT];
int res[TAILLE_MOT];

int main(int argc, char **argv)
{   
    void incrementIndex(int rangIndex, int startVector[]);
    int* findPassword(int startVector[], char alpha[26], string hashATrouver, int threadID);

    //Encrypter un mot test
    string motATrouver = "aaaaour";
    string hashATrouver = encode(motATrouver);

    //Afficher le hash à décoder
    for (int i=0; i<hashATrouver.length(); i++)
        cout << hex << (int)hashATrouver[i] << " ";
    cout << "\n" << endl;




    //Démarrer le compteur
    auto start = high_resolution_clock::now();

    //Start 4 threads with different start points
    auto future1 = async(findPassword, myIndex, alphabet, hashATrouver, 1);
    auto future2 = async(findPassword, start1, alphabet, hashATrouver, 2);
    //incrementIndex(TAILLE_MOT - 1, myIndex);
    //auto future3 = async(findPassword, myIndex, hashATrouver, 3);
    //incrementIndex(TAILLE_MOT - 1, myIndex);
    //auto future4 = async(findPassword, myIndex, hashATrouver, 4);

    while(found==0){
        std::this_thread::sleep_for(500ms);
    }

    //Stopper le compteur
    auto stop = high_resolution_clock::now();

    switch(found){
        case 1:
            for (int i = 0; i < sizeof(future1.get()); i++)
                res[i] = future1.get()[i];
            break;
        case 2:
            for (int i = 0; i < sizeof(future2.get()); i++)
                res[i] = future2.get()[i];
            break;
        /*case 3:
            for (int i = 0; i < sizeof(future3.get()); i++)
                res[i] = future3.get()[i];
            break;
        case 4:
            for (int i = 0; i < sizeof(future4.get()); i++)
                res[i] = future4.get()[i];
            break;*/
    }

    cout << "---------------------------------------- \n";
    cout << "mot trouvé : "<< motTest << "\n";
    cout << "hash trouvé : \n";
    for (size_t i=0; i< sizeof(res); i++)
        cout << hex << (int)res[i] << " ";
    cout << "\n" << endl;
    auto duration = duration_cast<seconds>(stop - start); 

    cout << "Time taken by function: " << duration.count() << " seconds" << endl;
}

int* findPassword(int startVector[TAILLE_MOT], char alpha[26], string hashATrouver, int threadID) {
    void incrementIndex(int rangIndex, int testVector[]);
    int testVector[TAILLE_MOT] = {0, 0, 0, 0, 0, 0, 0};

    //Transfer data in a temp buffer to keep original intact
    for (int i = 0; i < TAILLE_MOT; i++)
        testVector[i] = startVector[i];

    for (size_t i = 0; i < TAILLE_MOT; i++)
        printf("thread %d, test[%d] = %d\n", i, threadID, startVector[i]);


    bool finished = false;
    while (found == 0 && !finished) {
        printf("dans le while %d\n", threadID);

        motTest = alpha[testVector[0]];
        printf("motest = %s\n", motTest);

        for (size_t i = 1; i < TAILLE_MOT; i++)
        {
            printf("ajoute la lettre %s\n", alpha[testVector[i]]);
            motTest += alpha[testVector[i]];
        }

        printf("test de %s\n", motTest);
        hashTest = encode(motTest);
        try{
            for(int i = 0; i < 2; i++)
                incrementIndex(TAILLE_MOT - 1, testVector); 
        }catch(const std::exception& e){
            printf("End of tests for thread %d \n", threadID);
            finished = true;
        }        
        
        if(hashATrouver.compare(hashTest) != 0)
            found = threadID;
    }
    return testVector;
}

void incrementIndex(int rangIndex, int testVector[]){
    testVector[rangIndex]+=1;
    if(testVector[rangIndex] >= 26){
        testVector[rangIndex]=0;
        if(rangIndex != 0)
            incrementIndex(rangIndex-1, testVector);
    }
}