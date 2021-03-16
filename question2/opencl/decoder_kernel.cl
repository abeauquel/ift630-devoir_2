#define TAILLE_MOT               7  

//Cette  fonction  effectue  une  s u b s t i t u t i o n  decaracteres  en  additionnant  une  valeur
char* ADD(char* mot, int cle) {
  int taille;
  for(int i = 0; i < TAILLE_MOT; i++) mot[i] = mot[i] + cle * i;
  return mot;
}

// //Cette  fonction  effectue  un ou  e x c l u s i fentre  la  chaine  de  caracteres  et  la  cle
// string XOR(string mot, string cle) {
//   int taille;
//   taille = mot.length();
//   for(int i = 0; i < taille; i++) mot[i] = mot[i] ^ cle[i];
//   return mot;
// }

// //∗Cette  fonction  effectue  un decalage  c i r c u l a i r evers  la  droite  de ”nb”  caracteres∗∗
// string decale(string mot, int nb) {
//   char t[nb];
//   int max = mot.length()- 1;
//   for(int i = 0; i < nb; i++) t[i] = mot[max- i];
//   for(int i = max; i >= nb; i--) mot[i] = mot[i- nb];
//   for(int i = 0; i < nb; i++) mot[i] = t[nb- i- 1];
//   return mot;
// }

// //∗Cette  fonction  effectue  une  t r a n s l a t i o nentre  l e s  caracteres  du mot d ’ une  distancededuite  de  la  cle3
// string echange(string mot, int cle) {
//   int taille = mot.length();
//   for(int i = 0; i + cle < taille; i++) {
//     char temp;
//     temp = mot[i];
//     mot[i] = mot[i + cle];
//     mot[i + cle] = temp;
//   }
//   return mot;
// }
// //∗Cette  fonction  genere  une  cle  numerique∗∗/
// int getCle(string mot) {
//   int cle, temp;
//   temp = 0;
//   for(int i = 0; i < mot.length(); i++) temp = temp + (unsigned int ) mot[i];
//   cle = (temp % 4) + 1;
//   return abs(cle);
// }

// ///Cette  fonction  encode  le  message/
// string encode(string  mot){
//   string copie ;
//   int cle ;
//   copie = mot ;
//   for(int j =0;  j<3;  j++){
//     cle = getCle (mot );
//     mot = decale (mot , cle /2);
//     mot = ADD(mot , cle );
//     mot = echange (mot , cle ) ;
//     mot = XOR(mot , copie ) ;
//     }
//     return mot;
// }

void incrementIndex(int *my_index, int rangIndex){
    my_index[rangIndex]+=1;
    if(my_index[rangIndex] >= 26){
        my_index[rangIndex]=0;
        if(rangIndex != 0)
            incrementIndex(my_index, rangIndex-1);
    }
}

__kernel void decoder(__global char *hash_a_trouver, __global char *hash_test, __global char *alphabet) {
    
    
    // Get the index of the current element
    int id_thread = get_global_id(0);
    int my_index[7] = {0,0,0,0,0,0,0};
    char mot_a_trouver[7]; 
    int i = 0;
    char lettre_predefini = alphabet[id_thread%26];
    printf("kernel void decoder(), process %d, avec lettre : %c \n",id_thread, lettre_predefini);
    //printf(" hash à trouver : %c \n",hash_a_trouver[1]);
    // printf(" hash à trouver : %c \n",alphabet[0]); //a
    // printf(" hash à trouver : %c \n",alphabet[1]); //b

    mot_a_trouver[0] = lettre_predefini;
    for (i=1; i < 7; i++)
    {
        mot_a_trouver[i]=alphabet[my_index[i]];
    }

    printf("hash à trouver : %s \n",mot_a_trouver); //a
    // // cout << hash_test << "\n";
    // hashTest=encode(hash_test);
    // incrementIndex(TAILLE_MOT-1);
    // while(hashATrouver.compare(hashTest) != 0){
    //     hash_test=alphabet[my_index[0]];
    //     for (size_t i = 1; i < TAILLE_MOT; i++)
    //     {
    //          hash_test+=alphabet[my_index[i]];
    //     }
        
    //    // cout << hash_test << "\n";
    //     hashTest=encode(hash_test);
    //     incrementIndex(TAILLE_MOT-1);
    // }
    printf("fin kernel void decoder() , process %d \n",id_thread);
}

