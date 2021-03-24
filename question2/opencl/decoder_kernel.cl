#define TAILLE_MOT               7  


void mystrncpy(char* result,char * a_copier, int taille){
        for(int i = 0; i < taille; i++) {
                result[i]= a_copier[i];
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
  int result = 1;
  for(int j =0;  j<TAILLE_MOT;  j++){
    if(hash1[j] != hash2[j]){
      return 0;
    }
  }
  return 1;
}

__kernel void decoder(__global char *hash_a_trouver, __global char *hash_test, __global char *alphabet, __global char *mot_trouve, __global int *is_result) {
    
    // Get the index of the current element
    int id_thread = get_global_id(0);
    int my_index[7] = {0,0,0,0,0,0,0};
    char copy_alphabet[26];
    char copy_hash_a_trouver[7];
    int copy_is_result = 0;
    char mot_a_tester[7]; 
    char hash_a_tester[7]; 
    char lettre_predefini = alphabet[id_thread%26];
    int i = 0;
    int j = 0;
    int result_compare_hash = 0;
    printf("kernel void decoder(), process %d, avec lettre : %c \n",id_thread, lettre_predefini);
   
    for(int j =0;  j<TAILLE_MOT;  j++){
      copy_hash_a_trouver[j] = hash_a_trouver[j];
      
    }
   
    for(int j =0;  j<26;  j++){
      copy_alphabet[j] = alphabet[j];
    }     
    
   
    mot_a_tester[0] = lettre_predefini;

    while(copy_is_result ==0 && (mot_a_tester[0] == lettre_predefini)){
      mot_a_tester[0]= lettre_predefini;
      for (i=1; i < TAILLE_MOT; i++)
      {
        mot_a_tester[i]=copy_alphabet[my_index[i]];
      }
      // if(id_thread == 25){
      //   printf("mot à tester : %.7s \n",mot_a_tester);
      // }
   
      encode(mot_a_tester, hash_a_tester);
      incrementIndex(my_index, TAILLE_MOT-1);
      result_compare_hash=compareHash(hash_a_tester, copy_hash_a_trouver);
       
      if(result_compare_hash != 0){
        is_result[0]=1;
        
      }

      copy_is_result=is_result[0];
    }
    
    if(result_compare_hash != 0){

      for(j =0;  j<TAILLE_MOT;  j++){
        hash_test[j] = hash_a_tester[j];
      }

      for(j =0;  j<TAILLE_MOT;  j++){
        mot_trouve[j] = mot_a_tester[j];
      }
      printf("Le process %d, a trouvé  le mot : %.7s \n", id_thread, mot_a_tester);
    }
    
    printf("fin kernel void decoder() , process %d \n",id_thread);
}

