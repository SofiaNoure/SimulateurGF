#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NBR_BLOCS 100 // Nombre total de blocs dans la mémoire
#define FB 10 // Taille d'un bloc

 typedef struct {
  int Table_dallocation[NBR_BLOCS]; //libre 0 ou non 1
  Produit Produits[FB];
  struct BLOC* suivant; // Pointeur vers le bloc suivant
 }BLOC ;

 typedef struct {
  char nom [21];
  int id ;
  float prix ;
 }Produit ;

 typedef struct {
 char nom [21];
 int taille ; //taille du fichier en bloc
 int nbr_produit ;
 int organisation_globale; //1 pour contigu et 0 pour chainee
 BLOC* premier_bloc; // Adresse du premier bloc (mode chainee)
 int organisation_interne ;// 1 pour triee et 0 pour non triee
 }Meta ;

