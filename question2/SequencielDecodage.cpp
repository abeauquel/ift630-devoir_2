#include <cstdlib>
#include <iostream>
#include "encodage.cpp"
using namespace std;
#define TAILLE_MOT               7  

char alphabet[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int myIndex[TAILLE_MOT] = {0,0,0,0,0,0,0};

int main(int argc, char **argv)
{   
    void incrementIndex(int rangIndex);
    string motATrouver = "bonjour";
    string hashATrouver = encode(motATrouver);
    string motTest;
    string hashTest;

    for (int i=0; i<hashATrouver.length(); i++)
        cout << hex << (int)hashATrouver[i] << " ";
    cout << "\n" << endl;

    while(hashATrouver.compare(hashTest) != 0){
        motTest=alphabet[myIndex[0]];
        for (size_t i = 1; i < TAILLE_MOT; i++)
        {
             motTest+=alphabet[myIndex[i]];
        }
        
        cout << motTest << "\n";
        hashTest=encode(motTest);
        incrementIndex(TAILLE_MOT-1);
    }

    cout << hex << "---------------------------------------- \n";
    cout << hex << "hash trouvé : \n";
    for (int i=0; i<hashTest.length(); i++)
        cout << hex << (int)hashTest[i] << " ";
    cout << "\n" << endl;
}

void incrementIndex(int rangIndex){
    myIndex[rangIndex]+=1;
    if(myIndex[rangIndex] >= 26){
        myIndex[rangIndex]=0;
        if(rangIndex != 0)
            incrementIndex(rangIndex-1);
    }
}