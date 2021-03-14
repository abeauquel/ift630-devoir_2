#include <cstdlib>
#include <iostream>
using namespace std;

//Cette  fonction  effectue  une  s u b s t i t u t i o n  decaracteres  en  additionnant  une  valeur
string ADD(string mot, int cle) {
  int taille;
  taille = mot.length();
  for(int i = 0; i < taille; i++) mot[i] = mot[i] + cle * i;
  return mot;
}

//Cette  fonction  effectue  un ou  e x c l u s i fentre  la  chaine  de  caracteres  et  la  cle
string XOR(string mot, string cle) {
  int taille;
  taille = mot.length();
  for(int i = 0; i < taille; i++) mot[i] = mot[i] ^ cle[i];
  return mot;
}

//∗Cette  fonction  effectue  un decalage  c i r c u l a i r evers  la  droite  de ”nb”  caracteres∗∗
string decale(string mot, int nb) {
  char t[nb];
  int max = mot.length()- 1;
  for(int i = 0; i < nb; i++) t[i] = mot[max- i];
  for(int i = max; i >= nb; i--) mot[i] = mot[i- nb];
  for(int i = 0; i < nb; i++) mot[i] = t[nb- i- 1];
  return mot;
}

//∗Cette  fonction  effectue  une  t r a n s l a t i o nentre  l e s  caracteres  du mot d ’ une  distancededuite  de  la  cle3
string echange(string mot, int cle) {
  int taille = mot.length();
  for(int i = 0; i + cle < taille; i++) {
    char temp;
    temp = mot[i];
    mot[i] = mot[i + cle];
    mot[i + cle] = temp;
  }
  return mot;
}
//∗Cette  fonction  genere  une  cle  numerique∗∗/
int getCle(string mot) {
  int cle, temp;
  temp = 0;
  for(int i = 0; i < mot.length(); i++) temp = temp + (unsigned int ) mot[i];
  cle = (temp % 4) + 1;
  return abs(cle);
}

///Cette  fonction  encode  le  message/
string  encode(string  mot){
  string copie ;
  int cle ;
  copie = mot ;
  for(int j =0;  j<3;  j++){
    cle = getCle (mot );
    mot = decale (mot , cle /2);
    mot = ADD(mot , cle );
    mot = echange (mot , cle ) ;
    mot = XOR(mot , copie ) ;
    }
    return mot;
}
