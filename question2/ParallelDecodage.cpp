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
int myIndex[TAILLE_MOT] = {0,0,0,0,0,0,0};
int found = 0;
string motTest;
string hashTest;
int password[TAILLE_MOT];
int res[TAILLE_MOT];

int main(int argc, char **argv)
{   
    void incrementIndex(int rangIndex);
    int* findPassword(int startVector[], string hashATrouver, int threadID);

    //Encrypter un mot test
    string motATrouver = "aaajour";
    string hashATrouver = encode(motATrouver);

    //Afficher le hash à décoder
    for (int i=0; i<hashATrouver.length(); i++)
        cout << hex << (int)hashATrouver[i] << " ";
    cout << "\n" << endl;


    //Démarrer le compteur
    auto start = high_resolution_clock::now();

    fork();
    fork();
    //Start 4 threads with different start points

    //Stopper le compteur
    auto stop = high_resolution_clock::now(); 

    cout << "---------------------------------------- \n";
    cout << "mot trouvé : "<< motTest << "\n";
    cout << "hash trouvé : \n";
    for (size_t i=0; i< sizeof(res); i++)
        cout << hex << (int)res[i] << " ";
    cout << "\n" << endl;
    auto duration = duration_cast<seconds>(stop - start); 

    cout << "Time taken by function: " << duration.count() << " seconds" << endl;
}

int* findPassword(int startVector[TAILLE_MOT], string hashATrouver, int threadID) {
    void incrementIndex(int rangIndex, int testVector[]);
    int testVector[TAILLE_MOT] = {0, 0, 0, 0, 0, 0, 0};

    //Transfer data in a temp buffer to keep original intact
    for (int i = 0; i < sizeof(startVector); i++)
        testVector[i] = startVector[i];


    while (hashATrouver.compare(hashTest) != 0) {
        motTest = alphabet[testVector[0]];
        for (size_t i = 1; i < TAILLE_MOT; i++)
        {
            motTest += alphabet[testVector[i]];
        }

        // cout << motTest << "\n";
        hashTest = encode(motTest);
        for(int i = 0; i < 1; i++)
            incrementIndex(TAILLE_MOT - 1, testVector);
        cout << "test " << motTest << endl;
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