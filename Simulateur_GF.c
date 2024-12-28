
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#define NBR_BLOCS 100 // Nombre total de blocs dans la mémoire
#define FB 10 // Taille d'un bloc

 typedef struct {
  char nom [21];
  int id ;
  float prix ;
  int is_deleted ;
  struct Produit* suivant;
 }Produit ;

 typedef struct {
  Produit Produits[FB];
  struct BLOC* suivant; // Pointeur vers le bloc suivant
  int nbr_produit ;
  char nom [21] ;
  int organisation_globale ;
  int organisation_interne ;
 }BLOC ;

 typedef struct {
 char nom [21];
 int taille ; //taille du fichier en bloc
 int nbr_produit ;
 int organisation_globale; //1 pour contigu et 0 pour chainee
 BLOC* premier_bloc; // Adresse du premier bloc (mode chainee)
 int premier_bloc_contigue; //index premierbloc mode contigu
 int organisation_interne ;// 1 pour triee et 0 pour non triee
 }Meta ;

typedef struct {
 BLOC blocs[NBR_BLOCS];
 int Table_dallocation[NBR_BLOCS]; //libre 0 ou non 1
 }MS ;

 typedef struct {
    int bloc_index;
    int position;
} Position;

  void initialiserTableAllocation(MS *ms){
   for (int i = 0; i < NBR_BLOCS; i++){
            ms->Table_dallocation[i] = 0;
        }
    printf("Table d'allocation initialisee\n");
    }

 void AllouerBlocsContigus(Meta* fichier, MS* ms, int taille_fichier) {

    fichier->premier_bloc_contigue = -1;  // Initialisation à -1
    int blocs_alloues = 0;                 // Compteur pour les blocs trouvés
    int i = 0;                              // Index pour parcourir les blocs

    while (blocs_alloues < taille_fichier) {
        // Chercher un bloc libre
        while (i < NBR_BLOCS && ms->Table_dallocation[i] == 1) {
            i++; // Passer les blocs occupés
        }

        // Si on a trouvé un bloc libre
        if (i < NBR_BLOCS) {
            if (blocs_alloues == 0) { // Début d'une nouvelle séquence
                fichier->premier_bloc_contigue = i;
            }

            blocs_alloues++; // Augmenter le nombre de blocs alloués
            ms->Table_dallocation[i] = 1; // Marquer le bloc comme occupé

            // Si on a atteint le nombre requis d'enregistrements
            if (blocs_alloues == taille_fichier) {
                printf("Blocs alloués à partir du bloc: %d\n", fichier->premier_bloc_contigue);
                return;
            }
        } else {
            // Si on sort de la boucle sans avoir alloué tous les blocs nécessaires
            printf("Echec de l'allocation en raison du manque de blocs libres contigus.\n");
            return;
        }

        i++; // Passer au bloc suivant
    }
}


 void AllouerBlocsChaines(MS *ms , Meta *fichier, int taille_fichier) {
    int blocs_alloues = 0;
    BLOC* dernier_bloc = NULL;

    for (int i = 0; i < NBR_BLOCS && blocs_alloues < taille_fichier; i++) {
        if (ms->Table_dallocation[i] == 0) { // Si le bloc est libre
            ms->Table_dallocation[i] = 1;   // On alloue le bloc

            BLOC* nouveau_bloc = (BLOC*)malloc(sizeof(BLOC));
            if (nouveau_bloc == NULL) { // Vérification de l'allocation mémoire
                printf("Erreur : memoire insuffisante pour allouer un nouveau bloc.\n");
                return;
            }
            nouveau_bloc->suivant = NULL;

            if (dernier_bloc == NULL) { // Premier bloc à allouer
                fichier->premier_bloc = nouveau_bloc; // Affectation de l'adresse
            } else {
                dernier_bloc->suivant = nouveau_bloc;
            }

            dernier_bloc = nouveau_bloc;
            blocs_alloues++;
        }
    }

    if (blocs_alloues < taille_fichier) {
        printf("Échec de l'allocation : pas assez de blocs libres disponibles.\n");
    } else {
        printf("Blocs chaines alloues avec succes.\n");
    }
}
  void creerfichier(Meta *f) {
    int choix1, choix2;

    printf("- entrer le nom du fichier :\n");
    scanf("%s", f->nom);

    do {
        printf("- entrer le nombre d'enregistrement:\n");
        scanf("%d", &f->nbr_produit);
    } while (f->nbr_produit <= 0);

    do {
        printf("- choissisez le mode d'organisation globale :\n");
        printf("0) organisation chainee\n");
        printf("1) organisation contigu\n");
        printf("votre choix :\n");
        scanf("%d", &choix1);

        if (choix1 == 1) {
            printf("vous avez choisi l'organisation contigue\n");
            f->organisation_globale = 1;
        } else if (choix1 == 0) {
            printf("vous avez choisit l'organisation chainee \n");
            f->organisation_globale = 0;
        } else {
            printf("choix invalid , veuillez reessayer\n");
        }
    } while (choix1 != 1 && choix1 != 0);

    do {
        printf("- choissisez le mode d'organisation interne :\n");
        printf("0) organisation non trie\n");
        printf("1) organisation trie\n");
        printf("votre choix :\n");
        scanf("%d", &choix2);
        if (choix2 == 0) {
            printf("vous avez choisit le mode non trie\n");
            f->organisation_interne = 0;
        } else if (choix2 == 1) {
            printf("vous avez choisit le mode trie\n");
            f->organisation_interne = 1;
        } else {
            printf("choix invalid , veuillez reessayer\n");
        }
    } while (choix2 != 1 && choix2 != 0);
}

    void chargerDansMS(Meta *f, MS *ms) {
    //on calcule la taille du fichier dans chaque bloc
    f->taille = ( f->nbr_produit + FB - 1 )/FB ;
    //on ajoute un bloc de taille si jamais le nombre nest pas fixe (bloc et demi par exemple )
    if ((f->nbr_produit + FB - 1 ) % FB != 0 ){
        f->taille++;
    }

    if (f->organisation_globale == 1) {
    //on fait appel a la fonction allouerBlocsContigus pour allouer de lespace et recuperer laddresse du premier bloc allouee
        AllouerBlocsContigus(ms, f, f->taille);

        int i = f->premier_bloc_contigue;
        int produits_restants = f->nbr_produit;

        while (i < f->premier_bloc_contigue + f->taille && produits_restants > 0) {
            int produits_dans_bloc;
            if (produits_restants > FB) {
                produits_dans_bloc = FB;
            } else {
                produits_dans_bloc = produits_restants;
            }

            ms->blocs[i].nbr_produit = produits_dans_bloc;
            strcpy(ms->blocs[i].nom, f->nom);
            ms->blocs[i].organisation_globale = f->organisation_globale;
            ms->blocs[i].organisation_interne = f->organisation_interne;

            produits_restants = produits_restants - produits_dans_bloc;
            ms->Table_dallocation[i] = 1;
            i++;
        }

        if (produits_restants == 0) {
            printf("Le fichier est bien chargé\n");
        } else {
            printf("Erreur : tous les produits n'ont pas pu être chargés\n");
        }
    }
     else { // Organisation chaînée
        AllouerBlocsChaines(ms ,f, f->taille);

        // Charger les métadonnées dans les blocs chaînés
        BLOC *current_bloc = f->premier_bloc; // Premier bloc alloué dans l'organisation chaînée
        int produits_restants = f->nbr_produit;

        while (current_bloc != NULL && produits_restants > 0) {
            current_bloc->nbr_produit = produits_restants > FB ? FB : produits_restants;
            strncpy(current_bloc->nom, f->nom, 20);
            current_bloc->nom[20] = '\0';
            current_bloc->organisation_globale = f->organisation_globale;
            current_bloc->organisation_interne = f->organisation_interne;
            produits_restants -= FB;
            current_bloc = current_bloc->suivant;
        }
           printf("le fichier est bien chargee");
    }
}

   void afficherMeta(Meta *f) {
      if (f == NULL) {
        printf("Les metadonnees ne peuvent pas etre affichees : le pointeur est nul.\n");
        return;
    }
    printf("- Les details des metadonnees du fichier-- :\n");
    printf("- Nom du fichier : %s\n", f->nom);
    printf("- Nombre d'enregistrements : %d\n", f->nbr_produit);
    printf("- Mode d'organisation globale : %d\n", f->organisation_globale);
    if (f->organisation_globale == 1 ){
        printf("- premiere addresse du bloc :%d\n", f->premier_bloc_contigue);
    }else {
        printf("- premiere addresse du bloc :%d\n", f->premier_bloc);
    }
    f->taille = (f->nbr_produit + (FB - 1 ))/FB ;
    printf("- Taille du fichier en blocs : %d\n", f->taille);
    printf("- Mode d'organisation interne : %d\n", f->organisation_interne);
}


 void mettreajourMeta (Meta *f ,char newname [21], int nvtaille , int nbproduitnew , int orgglobale ,BLOC *adressechain,int nvadressecon ,int orginterne){
   printf("mettre a jour les Metadonnees :\n");
   strcpy(f->nom,newname);
   f->taille = nvtaille ;
   f->nbr_produit = nbproduitnew ;
   f->organisation_globale = orgglobale ;
   if (f->organisation_globale == 1 ){
    f->premier_bloc_contigue = nvadressecon ;
   }else {
    f->premier_bloc = adressechain ;
   }
   f->organisation_interne = orginterne ;
 }

 void renamefile(Meta *f ,char newname[31]){
          strcpy(f->nom , newname);
      }

 void libererblocs(Meta *f, MS *ms) {
    if (f->organisation_globale == 1) {
        for (int i = f->premier_bloc_contigue; i < f->premier_bloc_contigue + f->taille; i++) {
            ms->Table_dallocation[i] = 0;
            memset(&ms->blocs[i], 0, sizeof(BLOC));
        }
    } else {
        BLOC* current_bloc = f->premier_bloc;
        while (current_bloc != NULL) {
            int index = (current_bloc - ms->blocs) / sizeof(BLOC);
            ms->Table_dallocation[index] = 0;
            memset(current_bloc, 0, sizeof(BLOC));
            current_bloc = current_bloc->suivant;
        }
    }
    printf("Tous les blocs associés ont été physiquement libérés.\n");
}



 void Supprimerfichier(Meta *f, MS *ms) {

    if (f == NULL) {
        printf("Le fichier n'existe pas.\n");
        return;
    }
    mettreajourMeta(f ,"",0,0,0,0,0,0); //supprimer le fichier logiquement
    libererblocs(f, ms); //supprimer le fichier physiquement
     printf("Le fichier a été bien supprimé");
}

void initializeMetadata(FILE* f, MS* ms) {
    Meta meta = {0};
    meta.nbr_produit = 0;
    meta.taille = 0;
    meta.organisation_globale = 0;
    meta.organisation_interne = 0;
    meta.premier_bloc = NULL;

    for (int i = 0; i < NBR_BLOCS; i++) {
        ms->Table_dallocation[i] = 0;
    }

    fseek(f, 0, SEEK_SET);
    if (fwrite(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Echec de l'ecriture des metadonnees.\n");
    } else {
        printf("Metadonnees initialisees avec succes.\n");
    }
}

int FirstEmptyBloc(MS* ms){
    for(int i=0; i<NBR_BLOCS; i++){
        if(ms->Table_dallocation[i] == 0){
            return i;
        }
    }
    return -1;
}

void initializeBloc(BLOC* bloc) {
    for (int i = 0; i < FB; i++) {
        bloc->Produits[i].id = 0;
        bloc->Produits[i].prix = 0.0f;
        bloc->Produits[i].nom[0] = '\0';
        bloc->Produits[i].is_deleted = 0;
    }
    bloc->nbr_produit = 0;
    bloc->suivant = NULL;
}

void insererEnregistrementNonTrieChaine(Produit nouvProduit, FILE* f, MS* ms) {
    Meta meta;

    // Valider la lecture du fichier
    fseek(f, 0, SEEK_SET);
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Impossible de lire les metadonnees.\n");
        return;
    }

    BLOC* current = meta.premier_bloc;
    BLOC* last = NULL;

    // Parcourir les blocs existants
    while (current != NULL) {
        last = current;

        // Chercher un emplacement vide
        for (int i = 0; i < FB; i++) {
            if (current->Produits[i].nom[0] == '\0') {
                current->Produits[i] = nouvProduit;
                meta.nbr_produit++;
                current->nbr_produit++;

                // Ecrire les metadonnees
                fseek(f, 0, SEEK_SET);
                if (fwrite(&meta, sizeof(Meta), 1, f) != 1) {
                    printf("Erreur : Echec de l'ecriture des metadonnees.\n");
                    return;
                }
                printf("Produit insere avec succes.\n");
                return;
            }
        }
        current = current->suivant;
    }

    // Allouer un nouveau bloc si aucun espace n'est trouve
    BLOC* newBlock = (BLOC*)malloc(sizeof(BLOC));
    if (!newBlock) {
        printf("Erreur : Echec de l'allocation memoire.\n");
        return;
    }
    int pos = FirstEmptyBloc(ms);
    if (pos != -1) {
        ms->Table_dallocation[pos] = 1; // Marquer le bloc comme alloue
    }

    initializeBloc(newBlock); // Reinitialiser le bloc nouvellement alloue
    newBlock->Produits[0] = nouvProduit;
    newBlock->nbr_produit = 1;
    newBlock->suivant = NULL;

    if (last) {
        last->suivant = newBlock;
        current = last->suivant;
    } else {
        meta.premier_bloc = newBlock;
        current = meta.premier_bloc;
    }

    meta.nbr_produit++;

    // Ecrire les metadonnees et le nouveau bloc dans le fichier
    fseek(f, 0, SEEK_SET);
    if (fwrite(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Echec de l'ecriture des metadonnees.\n");
        return;
    }

    printf("Produit insere avec succes.\n");
}

void rechercherEnregistrementNonTrieChaine(FILE* f, int id) {
    Meta meta;

    // Valider la lecture du fichier
    fseek(f, 0, SEEK_SET);
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Impossible de lire les metadonnees.\n");
        return;
    }

    BLOC* current = meta.premier_bloc;
    int blocIndex = 0;

    while (current != NULL) {
        for (int i = 0; i < current->nbr_produit; i++) {
            if (current->Produits[i].id == id) {
                printf("Produit trouve : Bloc %d, Index %d\n", blocIndex, i);
                return;
            }
        }
        current = current->suivant;
        blocIndex++;
    }

    printf("Produit non trouve.\n");
}

void rechercherEnregistrementTrieChaine(FILE* f, int id, MS* ms) {
    Meta meta;
    fseek(f, 0, SEEK_SET);

    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Impossible de lire les metadonnees.\n");
        return;
    }

    BLOC* bloc = meta.premier_bloc;
    int index = 0;

    while (bloc != NULL) {
        if (ms->Table_dallocation[index] == 1) {
            int beg = 0;
            int end = bloc->nbr_produit - 1;

            while (beg <= end) {
                int mid = (beg + end) / 2;
                if (bloc->Produits[mid].id == id) {
                    printf("Produit trouve : Bloc %d, Deplacement %d\n", index, mid);
                    return;
                } else if (bloc->Produits[mid].id > id) {
                    end = mid - 1;
                } else {
                    beg = mid + 1;
                }
            }
        }
        bloc = bloc->suivant;
        index++;
    }

    printf("Produit non trouve.\n");
}

int IndexToInsert(BLOC* bloc, int id) {
    for (int i = 0; i < bloc->nbr_produit; i++) {
        if (bloc->Produits[i].id >= id) {
            if (i == FB && bloc->Produits[i].nom[0] != '\0') {
                return -1;
            }
            return i;
        }
    }
    return -1;
}

void insererEnregistrementTrieChaine(Produit nouvProduit, FILE* f, MS* ms) {
    Meta meta;
    fseek(f, 0, SEEK_SET);
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Impossible de lire les metadonnees.\n");
        return;
    }
    BLOC* current = meta.premier_bloc;
    BLOC* last = NULL;
    bool inserted = false;

    while (current != NULL && !inserted) {
        int index = IndexToInsert(current, nouvProduit.id);

        if (index != -1) {
            if (current->nbr_produit < FB) {
                for (int i = current->nbr_produit; i > index; i--) {
                    current->Produits[i] = current->Produits[i - 1];
                }
                current->Produits[index] = nouvProduit;
                current->nbr_produit++;
                meta.nbr_produit++;
                printf("Insertion reussie.\n");
                inserted = true;
                return;
            } else {
                Produit shiftedProduct = current->Produits[current->nbr_produit - 1];
                if (index < current->nbr_produit) {
                    for (int i = current->nbr_produit - 1; i > index; i--) {
                        current->Produits[i] = current->Produits[i - 1];
                    }
                    current->Produits[index] = nouvProduit;
                } else {
                    shiftedProduct = nouvProduit;
                }

                BLOC* nextBlock = current->suivant;
                while (nextBlock != NULL) {
                    if (nextBlock->nbr_produit < FB) {
                        int insertIndex = IndexToInsert(nextBlock, shiftedProduct.id);
                        for (int i = nextBlock->nbr_produit; i > insertIndex; i--) {
                            nextBlock->Produits[i] = nextBlock->Produits[i - 1];
                        }
                        nextBlock->Produits[insertIndex] = shiftedProduct;
                        nextBlock->nbr_produit++;
                        inserted = true;
                        printf("Insertion reussie.1.1\n");
                        return;
                    } else {
                        Produit temp = nextBlock->Produits[nextBlock->nbr_produit - 1];
                        for (int i = nextBlock->nbr_produit - 1; i > 0; i--) {
                            nextBlock->Produits[i] = nextBlock->Produits[i - 1];
                        }
                        nextBlock->Produits[0] = shiftedProduct;
                        shiftedProduct = temp;
                        nextBlock = nextBlock->suivant;
                    }
                }

                if (!inserted) {
                    int newIndex = FirstEmptyBloc(ms);
                    if (newIndex != 0) {
                        BLOC* newBlock = malloc(sizeof(BLOC));
                        initializeBloc(newBlock);
                        newBlock->Produits[0] = shiftedProduct;
                        newBlock->nbr_produit = 1;
                        ms->Table_dallocation[newIndex] = 1;
                        newBlock->suivant = NULL;
                        current->suivant = newBlock;
                        inserted = true;
                        printf("Insertion reussie.\n");
                        return;
                    } else {
                        printf("Pas de place pour allouer un nouveau bloc\n");
                        return;
                    }
                }
            }
        }

        last = current;
        current = current->suivant;
    }

    if (!inserted) {
        // Creer un nouveau bloc si aucune position appropriee n'a ete trouvee
        int pos = FirstEmptyBloc(ms);
        if (pos != -1) {
            BLOC* newBlock = malloc(sizeof(BLOC));
            initializeBloc(newBlock);
            newBlock->Produits[0] = nouvProduit;
            newBlock->nbr_produit = 1;
            newBlock->suivant = NULL;

            ms->Table_dallocation[pos] = 1;

            if (last) {
                last->suivant = newBlock;
            } else {
                meta.premier_bloc = newBlock;
            }
        } else {
            printf("Erreur : Aucun bloc vide disponible.\n");
            return;
        }
    }

    printf("Insertion reussie\n");
    fseek(f, 0, SEEK_SET);
    fwrite(&meta, sizeof(Meta), 1, f);
}

int compareProduit(const void* a, const void* b) {
    Produit* p1 = (Produit*)a;
    Produit* p2 = (Produit*)b;
    return p1->id - p2->id; // Tri par ID du produit
}


void defragmentationContigueTrie(FILE* f, MS* ms) {
    Meta meta;
    fseek(f, 0, SEEK_SET);

    // Lire les metadonnees du fichier
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur: Impossible de lire les metadonnees.\n");
        return;
    }

    // Verifier si le fichier utilise une organisation de blocs contigus
    if (meta.organisation_globale != 1) {
        printf("La defragmentation ne supporte que l'organisation en blocs contigus.\n");
        return;
    }

    // Verifier si le fichier n'est pas fragmente
    int firstbloc = meta.premier_bloc_contigue;
    if (firstbloc == 0) {
        printf("Aucune fragmentation detectee.\n");
        return;
    }

    bool emptybloc = false;
    int pos = -1;

    // Deplacer les blocs pour remplir les espaces crees par les suppressions logiques
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) {  // Si le bloc est utilise
            // Trouver le premier bloc libre
            for (int j = 0; j < NBR_BLOCS; j++) {
                if (ms->Table_dallocation[j] == 0) {
                    pos = j; // Obtenir la position du bloc vide
                    break;
                }
            }

            // S'il n'y a pas de bloc libre, quitter la boucle
            if (pos == -1) {
                printf("Aucun bloc libre disponible pour la defragmentation.\n");
                return;
            }

            // Deplacer le bloc actuel a la position du bloc vide
            ms->blocs[pos] = ms->blocs[i]; // Deplacer le contenu du bloc
            ms->Table_dallocation[pos] = 1; // Le marquer comme alloue
            ms->Table_dallocation[i] = 0; // Marquer l'ancien bloc comme libre
        }
    }

    // Trier les produits dans chaque bloc (interne)
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) {  // Si le bloc est utilise
            qsort(ms->blocs[i].Produits, ms->blocs[i].nbr_produit, sizeof(Produit), compareProduit);
        }
    }

    // Mettre a jour les metadonnees avec le nouveau premier bloc contigu
    meta.premier_bloc_contigue = 0;

    // Ecrire les metadonnees mises a jour dans le fichier
    fseek(f, 0, SEEK_SET);
    if (fwrite(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur: Echec de l'ecriture des metadonnees apres defragmentation.\n");
    }

    printf("Defragmentation terminee.\n");
}

void defragmentationContigueNonTrie(FILE* f, MS* ms) {
    Meta meta;
    fseek(f, 0, SEEK_SET);

    // Lire les metadonnees du fichier
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur: Impossible de lire les metadonnees.\n");
        return;
    }

    // Verifier si le fichier utilise une organisation de blocs contigus
    if (meta.organisation_globale != 1) {
        printf("La defragmentation ne supporte que l'organisation en blocs contigus.\n");
        return;
    }

    // Verifier si le fichier n'est pas fragmente
    int firstbloc = meta.premier_bloc_contigue;
    if (firstbloc == 0) {
        printf("Aucune fragmentation detectee.\n");
        return;
    }

    bool emptybloc = false;
    int pos = -1;

    // Trouver le premier bloc vide dans la Table_dallocation
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 0) {
            emptybloc = true;
            pos = i;
            break;
        }
    }

    if (!emptybloc) {
        printf("Aucun bloc libre disponible.\n");
        return;
    }

    // Debuter le deplacement des blocs pour defragmenter
    int currentBlock = firstbloc;
    int nextBlock = currentBlock;

    // Deplacer les blocs pour eliminer la fragmentation
    while (currentBlock != 0 && ms->Table_dallocation[currentBlock] != 0) {
        // Deplacer le bloc a la position du bloc vide
        ms->Table_dallocation[pos] = 1; // Marquer le bloc comme utilise
        ms->Table_dallocation[currentBlock] = 0; // Marquer le bloc actuel comme libre

        // Mettre a jour le contenu du bloc dans la memoire
        ms->blocs[pos] = ms->blocs[currentBlock]; // Copier le contenu du bloc vers le bloc vide

        // Trouver le bloc suivant a deplacer
        nextBlock = currentBlock + 1;

        if (nextBlock >= NBR_BLOCS || ms->Table_dallocation[nextBlock] == 0) {
            break; // Plus de blocs a deplacer
        }

        // Passer au bloc suivant
        currentBlock = nextBlock;

        // Trouver le prochain bloc vide pour placer les donnees
        pos = -1;
        for (int i = 0; i < NBR_BLOCS; i++) {
            if (ms->Table_dallocation[i] == 0) {
                pos = i;
                break;
            }
        }
    }
}

void defragmentationChaineTriee(FILE* f, MS* ms) {
    Meta meta;
    fseek(f, 0, SEEK_SET);

    // Lire les metadonnees
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur: Impossible de lire les metadonnees.\n");
        return;
    }

    BLOC* current = meta.premier_bloc;
    BLOC* next = NULL;
    Produit buffer[FB];
    int totalProduits = 0;
    int blocIndex = 0;

    // Etape 1: Collecter tous les produits tries
    while (current != NULL) {
        if (ms->Table_dallocation[blocIndex] == 1) {
            for (int i = 0; i < current->nbr_produit; i++) {
                buffer[totalProduits++] = current->Produits[i];
            }
            current->nbr_produit = 0; // Vider le bloc actuel
        }
        current = current->suivant;
        blocIndex++;
    }

    // Trier les produits collectes
    for (int i = 0; i < totalProduits - 1; i++) {
        for (int j = 0; j < totalProduits - i - 1; j++) {
            if (buffer[j].id > buffer[j + 1].id) {
                Produit temp = buffer[j];
                buffer[j] = buffer[j + 1];
                buffer[j + 1] = temp;
            }
        }
    }

    // Etape 2: Reecrire les produits dans les blocs
    current = meta.premier_bloc;
    blocIndex = 0;
    int bufferIndex = 0;

    while (current != NULL && bufferIndex < totalProduits) {
        if (ms->Table_dallocation[blocIndex] == 1) {
            for (int i = 0; i < FB && bufferIndex < totalProduits; i++) {
                current->Produits[i] = buffer[bufferIndex++];
                current->nbr_produit++;
            }
        }
        current = current->suivant;
        blocIndex++;
    }

    // Etape 3: Marquer les blocs inutilises comme libres
    while (current != NULL) {
        if (ms->Table_dallocation[blocIndex] == 1) {
            ms->Table_dallocation[blocIndex] = 0; // Liberer le bloc
            current->nbr_produit = 0;
        }
        current = current->suivant;
        blocIndex++;
    }

    // Ecrire les metadonnees mises a jour
    fseek(f, 0, SEEK_SET);
    if (fwrite(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur: Echec de l'ecriture des metadonnees.\n");
        return;
    }

    printf("Defragmentation terminee avec succes.\n");
}

void defragmentationChaineeNonTrie(FILE* f, MS* ms) {
    Meta meta;

    // Lire les metadonnees du fichier
    fseek(f, 0, SEEK_SET);
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Impossible de lire les metadonnees.\n");
        return;
    }

    if (meta.premier_bloc == NULL) {
        printf("Erreur : Aucun bloc trouve pour effectuer la defragmentation.\n");
        return;
    }

    BLOC* current = meta.premier_bloc;

    // Parcourir les blocs
    while (current != NULL) {
        int write_index = 0;

        // Reorganiser les produits pour eliminer les espaces vides
        for (int i = 0; i < FB; i++) {
            if (current->Produits[i].nom[0] != '\0') { // Produit valide
                if (write_index != i) {
                    current->Produits[write_index] = current->Produits[i];
                    current->Produits[i].nom[0] = '\0'; // Effacer l'ancien emplacement
                }
                write_index++;
            }
        }

        // Mettre a jour le nombre de produits dans le bloc
        current->nbr_produit = write_index;

        // Passer au bloc suivant
        current = current->suivant;
    }

    // Reecrire les metadonnees dans le fichier
    fseek(f, 0, SEEK_SET);
    if (fwrite(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Echec de l'ecriture des metadonnees apres defragmentation.\n");
    } else {
        printf("Defragmentation interne chainee terminee avec succes.\n");
    }
}

void supprimerEnregistrementChaineTrieePhysique(Produit** tete, int id_produit) {
    Produit* courant = *tete;
    Produit* precedent = NULL;

    // Parcours de la liste pour trouver le produit à supprimer
    while (courant != NULL) {
        if (courant->id == id_produit) {
            // Réajuster les pointeurs pour exclure l'élément
            if (precedent == NULL) {
                // Si c'est le premier élément
                *tete = courant->suivant;
            } else {
                precedent->suivant = courant->suivant;
            }
            free(courant); // Libérer physiquement la mémoire
            printf("Produit avec ID %d supprimé avec succès.\n", id_produit);
            return;
        } else if (courant->id > id_produit) {
            // Arrêter la recherche si l'ID dépasse (liste triée)
            printf("Produit avec ID %d non trouvé.\n", id_produit);
            return;
        }
        precedent = courant;
        courant = courant->suivant;
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
}



void supprimerlogiquementEnregistrementChaineTriee(FILE* f, int id) {
    Meta meta;

    // Valider la lecture du fichier
    fseek(f, 0, SEEK_SET);
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Impossible de lire les métadonnées.\n");
        return;
    }

    BLOC* current = meta.premier_bloc;
    int blocIndex = 0;

    // Parcourir les blocs
    while (current != NULL) {

        for (int i = 0; i < current->nbr_produit; i++){
            if (current->Produits[i].id == id) {

                current->Produits[i].is_deleted = 1;
                printf("Enregistrement avec l'ID %d supprimé logiquement.\n", id);


                fseek(f, sizeof(Meta) + blocIndex * sizeof(BLOC), SEEK_SET);
                fwrite(current, sizeof(BLOC), 1, f);
                return;
            }
        }
        current = current->suivant;
        blocIndex++;
    }

    printf("Enregistrement avec l'ID %d non trouvé.\n", id);
}


void supprimerEnregistrementContigueTrieePhysique(FILE* f, MS* ms, int id) {
    Meta meta;

    // Valider la lecture du fichier
    fseek(f, 0, SEEK_SET);
    if (fread(&meta, sizeof(Meta), 1, f) != 1) {
        printf("Erreur : Impossible de lire les métadonnées.\n");
        return;
    }

    // Vérifier si le fichier est en mode contigu et trié
    if (meta.organisation_globale != 1 || meta.organisation_interne != 1) {
        printf("Erreur : Le fichier n'est pas en mode contigu et trié.\n");
        return;
    }

    // Parcourir les blocs contigus
    int blocIndex = meta.premier_bloc_contigue;
    while (blocIndex < meta.premier_bloc_contigue + meta.taille) {
        BLOC* bloc = &ms->blocs[blocIndex]; // Accéder au bloc correspondant
        for (int i = 0; i < bloc->nbr_produit; i++) {
            // Vérifier si l'ID correspond
            if (bloc->Produits[i].id == id) {
                // Déplacer les enregistrements pour supprimer l'élément
                for (int j = i; j < bloc->nbr_produit - 1; j++) {
                    bloc->Produits[j] = bloc->Produits[j + 1];
                }

                // Remettre à zéro le dernier enregistrement
                bloc->Produits[bloc->nbr_produit - 1].id = 0;
                strcpy(bloc->Produits[bloc->nbr_produit - 1].nom, "");
                bloc->Produits[bloc->nbr_produit - 1].prix = 0.0;
                bloc->Produits[bloc->nbr_produit - 1].is_deleted = 0;

                // Réduire le nombre de produits dans le bloc
                bloc->nbr_produit--;
                printf("Enregistrement avec l'ID %d supprimé physiquement.\n", id);

                // Écrire les données mises à jour dans le fichier
                fseek(f, sizeof(Meta) + blocIndex * sizeof(BLOC), SEEK_SET);
                fwrite(bloc, sizeof(BLOC), 1, f);
                return;
            }
        }
        blocIndex++;
    }

    // Si l'enregistrement n'est pas trouvé
    printf("Enregistrement avec l'ID %d non trouvé.\n", id);
}

void compactageMemoireContigus(MS* ms, Meta* fichier){
 int index_libre = 0;
 int index_occupe = 0;
 //Recherche de l'index du premier bloc libre
 while(index_libre < NBR_BLOCS && ms->Table_dallocation[index_libre] == 1){
  index_libre++;
 }

 for(index_occupe = 0; index_occupe < NBR_BLOCS; index_occupe++){
  if(ms->Table_dallocation[index_occupe] == 1){
   if(index_occupe != index_libre){
    memcpy(&ms->blocs[index_libre], &ms->blocs[index_occupe], sizeof(BLOC)); //Deplacer les donnees du bloc occupe vers un bloc libre
    ms->Table_dallocation[index_libre] = 1;
    ms->Table_dallocation[index_occupe] = 0;
    index_libre++;
   }
  }
 }
 fichier->premier_bloc_contigue = 0;
 printf("Compactage de la memoire effectue (mode contigu) \n");
}



void CompactageMemoireChaine(MS* ms, Meta* fichier){
 BLOC* bloc_courant = fichier ->premier_bloc;
 BLOC* precedent = NULL;
 BLOC* bloc_libre = NULL;

 //Rechercher un bloc libre
 while(bloc_courant != NULL){
  if(bloc_courant->nbr_produit == 0){
   if(bloc_libre == NULL){
    bloc_libre = bloc_courant;
   }
   ms->Table_dallocation[bloc_courant - ms->blocs] = 0;
  }else{
   if(bloc_libre != NULL){
    memcpy(bloc_libre, bloc_courant, sizeof(BLOC)); //Deplacer les donnes du bloc courant vers un bloc libre
    ms->Table_dallocation[bloc_libre - ms->blocs] = 1;
    ms->Table_dallocation[bloc_courant - ms->blocs] = 0;

    if(precedent != NULL){
     precedent->suivant = bloc_libre;
    }
    bloc_libre->suivant = bloc_courant->suivant;
    bloc_courant->suivant = NULL;
    //avancer au bloc libre suivant
    bloc_libre = bloc_libre->suivant;
   }
  }
  precedent = bloc_courant;
  bloc_courant = bloc_courant->suivant;
 }
 printf("compactage de la memoire effectue (mode chaine ) \n");
}

void initializeMetadata(FILE* f, MS* ms);
void initializeBloc(BLOC* bloc); // Declare the function prototype

void insererEnregistrementNonTrieContigu(FILE* f, MS* ms, Produit* nouveau_produit) {
    Meta meta;
    fseek(f, 0, SEEK_SET);
    fread(&meta, sizeof(Meta), 1, f);

    // Chercher un bloc alloué avec de l'espace disponible
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Vérifier si le bloc est alloué
            BLOC* bloc = &ms->blocs[i];
            if (bloc->nbr_produit < FB) { // Vérifier si le bloc a de l'espace
                bloc->Produits[bloc->nbr_produit] = *nouveau_produit; // Insérer le produit
                bloc->nbr_produit++;
                meta.nbr_produit++;

                // Mise à jour dans le fichier
                fseek(f, sizeof(Meta) + i * sizeof(BLOC), SEEK_SET);
                fwrite(bloc, sizeof(BLOC), 1, f);
                fseek(f, 0, SEEK_SET);
                fwrite(&meta, sizeof(Meta), 1, f);

                printf("Produit inséré avec succès dans le bloc %d.\n", i);
                return;
            }
        }
    }

    // Si aucun bloc alloué n'a d'espace, chercher un bloc libre et l'allouer
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 0) { // Si le bloc est libre
            ms->Table_dallocation[i] = 1; // Allouer le bloc
            BLOC* nouveau_bloc = &ms->blocs[i];
            nouveau_bloc->nbr_produit = 1;
            nouveau_bloc->Produits[0] = *nouveau_produit; // Insérer le produit
            nouveau_bloc->suivant = NULL;

            meta.nbr_produit++;

            // Mise à jour dans le fichier
            fseek(f, sizeof(Meta) + i * sizeof(BLOC), SEEK_SET);
            fwrite(nouveau_bloc, sizeof(BLOC), 1, f);
            fseek(f, 0, SEEK_SET);
            fwrite(&meta, sizeof(Meta), 1, f);

            printf("Produit inséré avec succès dans un nouveau bloc %d.\n", i);
            return;
        }
    }

    printf("Erreur : Pas assez d'espace disponible pour insérer le produit.\n");
}


void insererEnregistrementTrieContigu(FILE* f, MS* ms, Produit* nouveau_produit) {
    Meta meta;
    fseek(f, 0, SEEK_SET);
    fread(&meta, sizeof(Meta), 1, f);

    // Chercher un bloc alloué avec de l'espace disponible
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Vérifier si le bloc est alloué
            BLOC* bloc = &ms->blocs[i];
            if (bloc->nbr_produit < FB) { // Vérifier si le bloc a de l'espace
                // Trouver l'endroit où insérer le produit de manière triée
                int j;
                for (j = bloc->nbr_produit - 1; j >= 0; j--) {
                    if (strcmp(bloc->Produits[j].nom, nouveau_produit->nom) > 0) {
                        bloc->Produits[j + 1] = bloc->Produits[j]; // Décalage
                    } else {
                        break;
                    }
                }
                bloc->Produits[j + 1] = *nouveau_produit;
                bloc->nbr_produit++;
                meta.nbr_produit++;

                // Mise à jour dans le fichier
                fseek(f, sizeof(Meta) + i * sizeof(BLOC), SEEK_SET);
                fwrite(bloc, sizeof(BLOC), 1, f);
                fseek(f, 0, SEEK_SET);
                fwrite(&meta, sizeof(Meta), 1, f);

                printf("Produit inséré dans le bloc %d de manière triée.\n", i);
                return;
            }
        }
    }

    // Si aucun bloc alloué n'a d'espace, chercher un bloc libre et l'allouer
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 0) { // Si le bloc est libre
            ms->Table_dallocation[i] = 1; // Allouer le bloc
            BLOC* nouveau_bloc = &ms->blocs[i];
            nouveau_bloc->nbr_produit = 1;
            nouveau_bloc->Produits[0] = *nouveau_produit; // Insérer le produit trié
            nouveau_bloc->suivant = NULL;

            meta.nbr_produit++;

            // Mise à jour dans le fichier
            fseek(f, sizeof(Meta) + i * sizeof(BLOC), SEEK_SET);
            fwrite(nouveau_bloc, sizeof(BLOC), 1, f);
            fseek(f, 0, SEEK_SET);
            fwrite(&meta, sizeof(Meta), 1, f);

            printf("Produit inséré dans un nouveau bloc %d de manière triée.\n", i);
            return;
        }
    }

    printf("Erreur : Pas assez d'espace disponible pour insérer le produit.\n");
}


Position rechercherEnregistrementNonTrieContigu(FILE* f, MS* ms, int id_produit) {
    Meta meta;
    fseek(f, 0, SEEK_SET);
    fread(&meta, sizeof(Meta), 1, f);

    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Vérifier si le bloc est alloué
            BLOC* bloc = &ms->blocs[i];
            for (int j = 0; j < bloc->nbr_produit; j++) {
                if (bloc->Produits[j].id == id_produit) { // Recherche par ID
                    Position pos = {i, j}; // Numéro de bloc et position
                    return pos; // Retourner la position
                }
            }
        }
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
    return (Position){-1, -1}; // Retourner une position invalide si non trouvé
}


Position rechercherEnregistrementTrieContigu(FILE* f, MS* ms, int id_produit) {
    Meta meta;
    fseek(f, 0, SEEK_SET);
    fread(&meta, sizeof(Meta), 1, f);

    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) {
            BLOC* bloc = &ms->blocs[i];
            int gauche = 0, droite = bloc->nbr_produit - 1;

            while (gauche <= droite) {
                int milieu = (gauche + droite) / 2;
                if (bloc->Produits[milieu].id == id_produit) { // Recherche par ID
                    Position pos = {i, milieu}; // Numéro de bloc et position
                    return pos; // Retourner la position
                } else if (bloc->Produits[milieu].id < id_produit) {
                    gauche = milieu + 1;
                } else {
                    droite = milieu - 1;
                }
            }
        }
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
    return (Position){-1, -1}; // Retourner une position invalide si non trouvé
}

void supprimerEnregistrementNonTrieContigu(FILE* f, MS* ms, int id_produit) {
    Meta meta;
    fseek(f, 0, SEEK_SET);
    fread(&meta, sizeof(Meta), 1, f);

    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Vérifier si le bloc est alloué
            BLOC* bloc = &ms->blocs[i];
            for (int j = 0; j < bloc->nbr_produit; j++) {
                if (bloc->Produits[j].id == id_produit) { // Recherche par ID
                    // Déplacer les produits pour combler l'espace laissé par la suppression
                    for (int k = j; k < bloc->nbr_produit - 1; k++) {
                        bloc->Produits[k] = bloc->Produits[k + 1];
                    }
                    bloc->nbr_produit--; // Diminuer le nombre de produits dans le bloc
                    meta.nbr_produit--; // Mettre à jour le total des produits

                    // Écrire les modifications dans le fichier
                    fseek(f, sizeof(Meta) + i * sizeof(BLOC), SEEK_SET);
                    fwrite(bloc, sizeof(BLOC), 1, f);
                    fseek(f, 0, SEEK_SET);
                    fwrite(&meta, sizeof(Meta), 1, f);

                    printf("Produit avec ID %d supprimé avec succès.\n", id_produit);
                    return;
                }
            }
        }
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
}





