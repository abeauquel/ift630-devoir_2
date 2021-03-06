#include <stdio.h>
#include <stdlib.h>
#define TAILLE_MOT               7  


void mystrncpy(char* result_compare_hash,char * a_copier, int taille){
        for(int i = 0; i < taille; i++) {
                result_compare_hash[i]= a_copier[i];
        }
}
//Cette  fonction  effectue  une  s u b s t i t u t i o n  decaracteres  en  additionnant  une  valeur
void ADD(char* mot, int cle) {
  int taille;
  for(int i = 0; i < TAILLE_MOT; i++) mot[i] = mot[i] + cle * i;
}

// //Cette  fonction  effectue  un ou  exclusif entre  la  chaine  de  caracteres  et  la  cle
void XOR(char* mot, char* cle) {
  for(int i = 0; i < TAILLE_MOT; i++){
    mot[i] = mot[i] ^ cle[i];
  }
}

//∗Cette  fonction  effectue  un decalage  c i r c u l a i r evers  la  droite  de ”nb”  caracteres∗∗
void decale(char* mot, int nb) {
    char t[TAILLE_MOT];
    //char t[nb]; todo vérfier que c'est ok
    int i=0;
    int max = TAILLE_MOT - 1;
    for(i = 0; i < nb; i++){
        t[i] = mot[max- i];
    } 
    for(i = max; i >= nb; i--){
        mot[i] = mot[i- nb];
    } 
    for(i = 0; i < nb; i++){
        mot[i] = t[nb- i- 1];
    } 
}

//∗Cette  fonction  effectue  une  t r a n s l a t i o nentre  l e s  caracteres  du mot d ’ une  distancededuite  de  la  cle3
void echange(char* mot, int cle) {
  for(int i = 0; i + cle < TAILLE_MOT; i++) {
    char temp;
    temp = mot[i];
    mot[i] = mot[i + cle];
    mot[i + cle] = temp;
  }
}
// //∗Cette  fonction  genere  une  cle  numerique∗∗/
int getCle(char* motParam) {
  char mot[TAILLE_MOT];
  mystrncpy(mot, motParam, 7);
  int cle, temp;
  temp = 0;
  for(int i = 0; i < TAILLE_MOT; i++) temp = temp + (unsigned int ) mot[i];
  cle = (temp % 4) + 1;
  return abs(cle);
}

///Cette  fonction  encode  le  message/
void encode(char*  motParam, char* mot){
  char copie[TAILLE_MOT];
  int cle ;
  mystrncpy(mot, motParam, TAILLE_MOT);
  mystrncpy(copie, motParam, TAILLE_MOT);
  for(int j =0;  j<TAILLE_MOT;  j++){
    copie[j] = mot[j];
  }
  for(int j =0;  j<3;  j++){
    cle = getCle (mot );
    decale (mot , cle /2);
    ADD(mot , cle );
    echange (mot , cle ) ;
    XOR(mot , copie ) ;
    }
}

void incrementIndex(int *my_index, int rangIndex){
    my_index[rangIndex]+=1;
    if(my_index[rangIndex] >= 26){
        my_index[rangIndex]=0;
        if(rangIndex != 0)//On ne change pas la premiere lettre, c'est une constante
            incrementIndex(my_index, rangIndex-1);
    }
}

int compareHash(char* hash1, char* hash2){
  int result_compare_hash = 1;
  for(int j =0;  j<TAILLE_MOT;  j++){
    if(hash1[j] != hash2[j]){
      result_compare_hash= 0;
    }
  }
  return result_compare_hash;
}

int main(){
    char hash_a_trouver[7];
    char hash_test[7];
    char alphabet[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    int id_thread = 0;

    int my_index[7] = {0,0,0,0,0,0,0};
    char mot_a_tester[7]; 
    char hash_a_tester[7]; 
    int i = 0;
    int j;
    char lettre_predefini = alphabet[id_thread%26];
    int result_compare_hash = 0;
    printf("kernel void decoder(), process %d, avec lettre : %c \n",id_thread, lettre_predefini);

    mot_a_tester[0]='a';
    mot_a_tester[1]='a';
    mot_a_tester[2]='n';
    mot_a_tester[3]='j';
    mot_a_tester[4]='o';
    mot_a_tester[5]='u';
    mot_a_tester[6]='r';
    printf("mot_a_tester %.7s \n", mot_a_tester);
    encode(mot_a_tester, hash_a_trouver);

    mot_a_tester[0] = lettre_predefini;

    // printf("hash_a_tester %.7s \n", hash_a_tester);
    // printf("hash_a_trouver %.7s \n", hash_a_trouver);
    // printf("result_compare_hash %d \n", result_compare_hash);

    while((mot_a_tester[0] == lettre_predefini) && (result_compare_hash == 0)){
        mot_a_tester[0]= lettre_predefini;
        for (i=1; i < TAILLE_MOT; i++)
        {
            mot_a_tester[i]=alphabet[my_index[i]];
        }
        //printf("mot à tester : %s \n",mot_a_tester);

        encode(mot_a_tester, hash_a_tester);
        incrementIndex(my_index, TAILLE_MOT-1);

        result_compare_hash=compareHash(hash_a_tester, hash_a_trouver);
    }

        printf("HASH TROUVE : \n");
    for(j =0;  j<TAILLE_MOT;  j++){
      hash_test[j] = hash_a_tester[j];
      printf("%c ", hash_test[j]);
    }
        printf("\n");
        printf("MOT TROUVE : \n");

    for(j =0;  j<TAILLE_MOT;  j++){
      printf("%c ", mot_a_tester[j]);
    }

    printf("\n fin kernel void decoder() , process %d \n",id_thread);
    return 0;
}

