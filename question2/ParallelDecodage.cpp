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
#define THREAD_COUNT            4

char alphabet[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
int start0[TAILLE_MOT] = {0,0,0,0,0,0,0}, start1[TAILLE_MOT] = {0,0,0,0,0,0,1}, start2[TAILLE_MOT] = {0,0,0,0,0,0,2}, start3[TAILLE_MOT] = {0,0,0,0,0,0,3};
int found = 0;
string motTest;
string hashTest;
int password[TAILLE_MOT];
int res[TAILLE_MOT];

int main(int argc, char **argv)
{   
    void incrementIndex(int rangIndex, int startVector[]);
    string findPassword(int startVector[], string hashATrouver, int threadID);

    //Encrypter un mot test
    string motATrouver = "aanjour";
    string hashATrouver= "";
    
    
    hashATrouver = encode(motATrouver);

    cout << "motATrouver : " << motATrouver << endl;
    cout << "hashATrouver : " << hashATrouver << endl;
    
    //Afficher le hash à décoder
    for (int i=0; i<TAILLE_MOT; i++)
        cout << hex << (int)hashATrouver[i] << " ";
    cout << "\n" << endl;




    //Démarrer le compteur
    auto start = high_resolution_clock::now();

    //Start 4 threads with different start points
    auto future1 = async(findPassword, start0, hashATrouver, 1);
    auto future2 = async(findPassword, start1, hashATrouver, 2);
    auto future3 = async(findPassword, start2, hashATrouver, 3);
    auto future4 = async(findPassword, start3, hashATrouver, 4);

    while(found==0){
        std::this_thread::sleep_for(500ms);
    }

    //Stopper le compteur
    auto stop = high_resolution_clock::now();
    string password;
    cout << "found by thread " << found << endl;
    switch(found){
        case 1:
            password = future1.get();
            break;
        case 2:
            password = future2.get();
            break;
        case 3:
            password = future3.get();
            break;
        case 4:
            password = future4.get();
            break;
    }
    

    cout << "---------------------------------------- \n";
    cout << "mot trouvé : "<< password << " par le thread " << found << endl;
    cout << "\n" << endl;
    auto duration = duration_cast<seconds>(stop - start); 

    cout << "Time taken by function: " << duration.count() << " seconds" << endl;
}

string findPassword(int startVector[TAILLE_MOT], string hashATrouver, int threadID) {
    void incrementIndex(int rangIndex, int testVector[]);
    int testVector[TAILLE_MOT] = {0, 0, 0, 0, 0, 0, 0};
    string currentTest;

    //Transfer data in a temp buffer to keep original intact
    for (int i = 0; i < TAILLE_MOT; i++)
        testVector[i] = startVector[i];

    //for (size_t i = 0; i < TAILLE_MOT; i++)
    //   printf("thread %d, test[%ld] = %d\n", threadID, i, startVector[i]);


    bool finished = false;
    while (found == 0 && !finished) {
        //printf("dans le while %d\n", threadID);


        //for(int i = 0; i < 26; i++)
        //    printf("alpha = %d\n", alpha[i]);


        currentTest = alphabet[testVector[0]];
        //printf("currentTest = %s\n", currentTest);

        for (size_t i = 1; i < TAILLE_MOT; i++)
        {
            //printf("ajoute la lettre %d\n", alpha[testVector[i]]);
            currentTest += alphabet[testVector[i]];
        }

        //printf("test de %s\n", currentTest);
        hashTest = encode(currentTest);
        //cout << "currentTest = " << currentTest << " || " << hashATrouver <<endl;
        //cout << "Thread " << threadID << " trying " << currentTest << endl;
        //cout << "hashTest = " << hashTest << endl;           
        if(hashATrouver.compare(hashTest)==0){
            found = threadID;
            finished = true;
        }else{
            try{
                for(int i = 0; i < THREAD_COUNT; i++)
                    incrementIndex(TAILLE_MOT - 1, testVector); 
            }catch(const std::exception& e){
                printf("End of tests for thread %d \n", threadID);
                finished = true;
            } 
        }
            
    }
    string ret = "";
    if(found == threadID){
        for(int i = 0; i < TAILLE_MOT; i++)
            ret += alphabet[testVector[i]];
        }
    return ret;
}

void incrementIndex(int rangIndex, int testVector[]){
    testVector[rangIndex]+=1;
    if(testVector[rangIndex] >= 26){
        testVector[rangIndex]=0;
        if(rangIndex != 0)
            incrementIndex(rangIndex-1, testVector);
    }
}