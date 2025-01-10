#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#define NBR_BLOCS 100 // Nombre total de blocs dans la mémoire
#define FB 10 // Taille d un bloc

void configConsole() {
#ifdef _WIN32
    // Pour Windows : Active l'encodage UTF-8 dans la console
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

 typedef struct {
  char nom [21];
  int id ;
  float prix ;
  int is_deleted ;

 }Produit ;

 typedef struct BLOC {
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
    int bloc;
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

    // Demander le nom du fichier
    printf("- Entrer le nom du fichier :\n");
    scanf("%s", f->nom);

    // Demander le nombre d'enregistrements (produits)
    do {
        printf("- Entrer le nombre d'enregistrements (produits) :\n");
        scanf("%d", &f->nbr_produit);
    } while (f->nbr_produit <= 0); // Vérifier que le nombre est positif

    // Choisir l'organisation globale (chaînée ou contiguë)
    do {
        printf("- Choisissez le mode d'organisation globale :\n");
        printf("0) Organisation chaînée\n");
        printf("1) Organisation contiguë\n");
        printf("Votre choix :\n");
        scanf("%d", &choix1);

        if (choix1 == 1) {
            printf("Vous avez choisi l'organisation contiguë\n");
            f->organisation_globale = 1;
        } else if (choix1 == 0) {
            printf("Vous avez choisi l'organisation chaînée\n");
            f->organisation_globale = 0;
        } else {
            printf("Choix invalide, veuillez réessayer.\n");
        }
    } while (choix1 != 1 && choix1 != 0); // Répéter jusqu'à un choix valide

    // Choisir l'organisation interne (triée ou non triée)
    do {
        printf("- Choisissez le mode d'organisation interne :\n");
        printf("0) Organisation non triée\n");
        printf("1) Organisation triée\n");
        printf("Votre choix :\n");
        scanf("%d", &choix2);

        if (choix2 == 0) {
            printf("Vous avez choisi le mode non trié\n");
            f->organisation_interne = 0;
        } else if (choix2 == 1) {
            printf("Vous avez choisi le mode trié\n");
            f->organisation_interne = 1;
        } else {
            printf("Choix invalide, veuillez réessayer.\n");
        }
    } while (choix2 != 1 && choix2 != 0); // Répéter jusqu'à un choix valide
}

void chargerDansMS(Meta *f, MS *ms) {
    // Calcul de la taille du fichier en blocs
    f->taille = (f->nbr_produit + FB - 1) / FB;

    if ((f->nbr_produit + FB - 1) % FB != 0) {
        f->taille++;
    }

    if (f->organisation_globale == 1) { // Organisation contiguë
        // Allouer de l'espace pour les blocs contigus
        AllouerBlocsContigus(f, ms, f->taille);

        int i = f->premier_bloc_contigue;
        int produits_restants = f->nbr_produit;
        int produit_index = 0; // Index pour suivre les produits ajoutés

        while (i < f->premier_bloc_contigue + f->taille && produits_restants > 0) {
            int produits_dans_bloc = (produits_restants > FB) ? FB : produits_restants;
            ms->blocs[i].nbr_produit = produits_dans_bloc;
            strcpy(ms->blocs[i].nom, f->nom);
            ms->blocs[i].organisation_globale = f->organisation_globale;
            ms->blocs[i].organisation_interne = f->organisation_interne;

            // Remplir les produits dans le bloc
            for (int j = 0; j < produits_dans_bloc; j++) {
                // Demander à l'utilisateur d'entrer les informations d'un produit
                Produit p;
                printf("Entrez le nom du produit %d: ", produit_index + 1);
                scanf("%s", p.nom);
                printf("Entrez l'ID du produit %d: ", produit_index + 1);
                scanf("%d", &p.id);
                printf("Entrez le prix du produit %d: ", produit_index + 1);
                scanf("%f", &p.prix);

                // Ajouter le produit au bloc
                ms->blocs[i].Produits[j] = p;
                produit_index++;
            }

            produits_restants -= produits_dans_bloc;
            ms->Table_dallocation[i] = 1; // Marquer ce bloc comme alloué
            i++;
        }

        if (produits_restants == 0) {
            printf("Le fichier est bien chargé\n");
        } else {
            printf("Erreur : tous les produits n'ont pas pu être chargés\n");
        }
    } else { // Organisation chaînée
        AllouerBlocsChaines(ms, f, f->taille);

        // Charger les produits dans les blocs chaînés
        BLOC *current_bloc = f->premier_bloc;
        int produits_restants = f->nbr_produit;

        while (current_bloc != NULL && produits_restants > 0) {
            current_bloc->nbr_produit = (produits_restants > FB) ? FB : produits_restants;
            strncpy(current_bloc->nom, f->nom, 20);
            current_bloc->nom[20] = '\0';
            current_bloc->organisation_globale = f->organisation_globale;
            current_bloc->organisation_interne = f->organisation_interne;

            // Remplir les produits dans le bloc
            for (int j = 0; j < current_bloc->nbr_produit; j++) {
                Produit p;
                printf("Entrez le nom du produit: ");
                scanf("%s", p.nom);
                printf("Entrez l'ID du produit: ");
                scanf("%d", &p.id);
                printf("Entrez le prix du produit: ");
                scanf("%f", &p.prix);

                // Ajouter le produit au bloc
                current_bloc->Produits[j] = p;
            }

            produits_restants -= current_bloc->nbr_produit;
            current_bloc = current_bloc->suivant; // Passer au bloc suivant
        }

        printf("Le fichier est bien chargé dans l'organisation chaînée\n");
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

void insererEnregistrementNonTrieChaine(Produit nouvProduit, Meta* meta, MS* ms) {
    // Validate input metadata and memory storage
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    BLOC* current = meta->premier_bloc;
    BLOC* last = NULL;

    // Traverse existing blocks to find space
    while (current != NULL) {
        last = current;

        // Look for an empty slot in the current block
        for (int i = 0; i < FB; i++) {
            if (current->Produits[i].nom[0] == '\0') { // Empty slot found
                current->Produits[i] = nouvProduit;
                meta->nbr_produit++;
                current->nbr_produit++;

                printf("Produit inséré avec succès.\n");
                return;
            }
        }
        current = current->suivant;
    }

    // If no space is available, allocate a new block
    int pos = FirstEmptyBloc(ms);
    if (pos == -1) {
        printf("Erreur : Aucun bloc libre disponible pour l'allocation.\n");
        return;
    }

    ms->Table_dallocation[pos] = 1; // Mark the block as allocated

    BLOC* newBlock = &ms->blocs[pos];
    initializeBloc(newBlock); // Initialize the block
    newBlock->Produits[0] = nouvProduit; // Insert the new product
    newBlock->nbr_produit = 1;

    // Update the chain
    if (last) {
        last->suivant = newBlock;
    } else {
        meta->premier_bloc = newBlock; // This is the first block
    }

    meta->nbr_produit++; // Update the metadata
    printf("Produit inséré avec succès dans un nouveau bloc.\n");
}

Position rechercherEnregistrementNonTrieChaine(Meta* meta, MS* ms, int id) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return (Position){-1, -1}; // Return invalid position
    }

    BLOC* current = meta->premier_bloc;
    int blocIndex = 0;

    // Traverse the linked list of blocks
    while (current != NULL) {
        for (int i = 0; i < current->nbr_produit; i++) {
            if (current->Produits[i].id == id) { // Check for matching ID
                return (Position){blocIndex, i}; // Return block index and position
            }
        }
        current = current->suivant;
        blocIndex++;
    }

    printf("Produit non trouvé.\n");
    return (Position){-1, -1}; // Return invalid position if not found
}

Position rechercherEnregistrementTrieChaine(Meta* meta, MS* ms, int id) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return (Position){-1, -1};
    }

    BLOC* bloc = meta->premier_bloc;
    int blocIndex = 0;

    while (bloc != NULL) {
        if (ms->Table_dallocation[blocIndex] == 1) { // Check if the block is allocated
            int beg = 0;
            int end = bloc->nbr_produit - 1;

            // Perform binary search within the block
            while (beg <= end) {
                int mid = (beg + end) / 2;
                if (bloc->Produits[mid].id == id) {
                    return (Position){blocIndex, mid}; // Return block index and position
                } else if (bloc->Produits[mid].id > id) {
                    end = mid - 1;
                } else {
                    beg = mid + 1;
                }
            }
        }
        bloc = bloc->suivant;
        blocIndex++;
    }

    printf("Produit non trouvé.\n");
    return (Position){-1, -1};
}

int IndexToInsert(BLOC* bloc, int id) {
    for (int i = 0; i < bloc->nbr_produit; i++) {
        if (bloc->Produits[i].id >= id) {
            return i; // Return the index where the product should be inserted
        }
    }
    // If no valid index is found and the block is not full
    if (bloc->nbr_produit < FB) {
        return bloc->nbr_produit; // Insert at the end of the block
    }
    return -1; // Block is full, no valid index
}
void insererEnregistrementTrieChaine(Produit nouvProduit, Meta* meta, MS* ms) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    BLOC* current = meta->premier_bloc;
    BLOC* last = NULL;
    bool inserted = false;

    while (current != NULL && !inserted) {
        int index = IndexToInsert(current, nouvProduit.id);

        if (index != -1) { // Valid insertion point found
            if (current->nbr_produit < FB) {
                // Shift products to make space
                for (int i = current->nbr_produit; i > index; i--) {
                    current->Produits[i] = current->Produits[i - 1];
                }
                current->Produits[index] = nouvProduit; // Insert new product
                current->nbr_produit++;
                meta->nbr_produit++;
                printf("Insertion réussie.\n");
                return;
            } else {
                // Handle overflow by shifting the last product
                Produit shiftedProduct = current->Produits[FB - 1];
                for (int i = FB - 1; i > index; i--) {
                    current->Produits[i] = current->Produits[i - 1];
                }
                current->Produits[index] = nouvProduit; // Insert new product

                // Try to insert the shifted product in the next block
                BLOC* nextBlock = current->suivant;
                while (nextBlock != NULL) {
                    if (nextBlock->nbr_produit < FB) {
                        int insertIndex = IndexToInsert(nextBlock, shiftedProduct.id);
                        for (int i = nextBlock->nbr_produit; i > insertIndex; i--) {
                            nextBlock->Produits[i] = nextBlock->Produits[i - 1];
                        }
                        nextBlock->Produits[insertIndex] = shiftedProduct;
                        nextBlock->nbr_produit++;
                        printf("Insertion réussie dans le bloc suivant.\n");
                        return;
                    } else {
                        // Continue shifting overflow to subsequent blocks
                        Produit temp = nextBlock->Produits[FB - 1];
                        for (int i = FB - 1; i > 0; i--) {
                            nextBlock->Produits[i] = nextBlock->Produits[i - 1];
                        }
                        nextBlock->Produits[0] = shiftedProduct;
                        shiftedProduct = temp;
                        nextBlock = nextBlock->suivant;
                    }
                }

                // If no space is available in subsequent blocks, allocate a new block
                int newIndex = FirstEmptyBloc(ms);
                if (newIndex != -1) {
                    BLOC* newBlock = &ms->blocs[newIndex];
                    initializeBloc(newBlock);
                    newBlock->Produits[0] = shiftedProduct;
                    newBlock->nbr_produit = 1;
                    newBlock->suivant = NULL;

                    if (last) {
                        last->suivant = newBlock;
                    } else {
                        meta->premier_bloc = newBlock;
                    }

                    ms->Table_dallocation[newIndex] = 1;
                    printf("Insertion réussie dans un nouveau bloc.\n");
                    return;
                } else {
                    printf("Erreur : Aucun bloc libre disponible pour l'allocation.\n");
                    return;
                }
            }
        }

        last = current;
        current = current->suivant;
    }

    // If no blocks exist, create a new one
    if (!inserted) {
        int pos = FirstEmptyBloc(ms);
        if (pos != -1) {
            BLOC* newBlock = &ms->blocs[pos];
            initializeBloc(newBlock);
            newBlock->Produits[0] = nouvProduit;
            newBlock->nbr_produit = 1;
            newBlock->suivant = NULL;

            ms->Table_dallocation[pos] = 1;

            if (last) {
                last->suivant = newBlock;
            } else {
                meta->premier_bloc = newBlock;
            }
            meta->nbr_produit++;
            printf("Insertion réussie dans un nouveau bloc (aucun bloc existant).\n");
        } else {
            printf("Erreur : Aucun bloc vide disponible.\n");
        }
    }
}
int compareProduit(const void* a, const void* b) {
    Produit* p1 = (Produit*)a;
    Produit* p2 = (Produit*)b;
    return p1->id - p2->id; // Tri par ID du produit
}


void defragmentationContigueTrie(Meta* meta, MS* ms) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    // Check if the file uses contiguous organization
    if (meta->organisation_globale != 1) {
        printf("La défragmentation ne supporte que l'organisation en blocs contigus.\n");
        return;
    }

    // Collect all products into a buffer
    Produit buffer[FB * meta->taille];
    int totalProduits = 0;

    for (int i = 0; i < meta->taille; i++) {
        BLOC* bloc = &ms->blocs[meta->premier_bloc_contigue + i];
        for (int j = 0; j < bloc->nbr_produit; j++) {
            buffer[totalProduits++] = bloc->Produits[j];
        }
        bloc->nbr_produit = 0; // Clear the block
    }

    // Sort the products
    qsort(buffer, totalProduits, sizeof(Produit), compareProduit);

    // Redistribute the products into contiguous blocks
    int bufferIndex = 0;
    for (int i = 0; i < meta->taille; i++) {
        BLOC* bloc = &ms->blocs[meta->premier_bloc_contigue + i];
        for (int j = 0; j < FB && bufferIndex < totalProduits; j++) {
            bloc->Produits[j] = buffer[bufferIndex++];
            bloc->nbr_produit++;
        }
    }

    printf("Défragmentation contiguë triée terminée avec succès.\n");
}
void defragmentationContigueNonTrie(Meta* meta, MS* ms) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    // Check if the file uses contiguous organization
    if (meta->organisation_globale != 1) {
        printf("La défragmentation ne supporte que l'organisation en blocs contigus.\n");
        return;
    }

    // Verify if there is fragmentation
    if (meta->premier_bloc_contigue == -1) {
        printf("Aucune fragmentation détectée.\n");
        return;
    }

    int pos = -1;

    // Move blocks to eliminate fragmentation
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // If the block is allocated
            for (int j = 0; j < NBR_BLOCS; j++) {
                if (ms->Table_dallocation[j] == 0) { // Find a free block
                    ms->blocs[j] = ms->blocs[i]; // Move block contents
                    ms->Table_dallocation[j] = 1; // Mark as allocated
                    ms->Table_dallocation[i] = 0; // Mark the old block as free
                    break;
                }
            }
        }
    }

    // Update metadata with the new first block index
    meta->premier_bloc_contigue = 0;
    printf("Défragmentation terminée.\n");
}


void defragmentationChaineTriee(Meta* meta, MS* ms) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    if (meta->organisation_globale != 0 || meta->organisation_interne != 1) {
        printf("Erreur : Cette fonction est réservée aux structures chaînées triées.\n");
        return;
    }

    BLOC* current = meta->premier_bloc;
    Produit buffer[FB * meta->taille];
    int totalProduits = 0;

    // Collecter tous les produits triés
    while (current != NULL) {
        for (int i = 0; i < current->nbr_produit; i++) {
            buffer[totalProduits++] = current->Produits[i];
        }
        current->nbr_produit = 0; // Vider le bloc actuel
        current = current->suivant;
    }

    // Trier les produits collectés
    qsort(buffer, totalProduits, sizeof(Produit), compareProduit);

    // Réécrire les produits dans les blocs chaînés
    current = meta->premier_bloc;
    int bufferIndex = 0;

    while (current != NULL && bufferIndex < totalProduits) {
        for (int i = 0; i < FB && bufferIndex < totalProduits; i++) {
            current->Produits[i] = buffer[bufferIndex++];
            current->nbr_produit++;
        }
        current = current->suivant;
    }

    printf("Défragmentation chaînée triée terminée avec succès.\n");
}


void defragmentationChaineeNonTrie(Meta* meta, MS* ms) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    if (!meta->premier_bloc) {
        printf("Erreur : Aucun bloc trouvé pour effectuer la défragmentation.\n");
        return;
    }

    BLOC* current = meta->premier_bloc;

    // Traverse the blocks and compact the products
    while (current != NULL) {
        int writeIndex = 0;

        for (int i = 0; i < FB; i++) {
            if (current->Produits[i].nom[0] != '\0') { // Valid product
                if (writeIndex != i) {
                    current->Produits[writeIndex] = current->Produits[i];
                    current->Produits[i].nom[0] = '\0'; // Clear the old position
                }
                writeIndex++;
            }
        }

        current->nbr_produit = writeIndex; // Update the number of valid products
        current = current->suivant;
    }

    printf("Défragmentation non triée des blocs chaînés terminée avec succès.\n");
}

void supprimerEnregistrementChaineTrieePhysique(Meta* meta, MS* ms, int id_produit) {
    if (!meta || !ms || !meta->premier_bloc) {
        printf("Erreur : Métadonnées ou liste des blocs non valides.\n");
        return;
    }

    // Ensure the structure is chain-based and sorted
    if (meta->organisation_globale != 0 || meta->organisation_interne != 1) {
        printf("Erreur : La structure n'est pas en mode chaîné et trié.\n");
        return;
    }

    BLOC* courant = meta->premier_bloc;
    BLOC* precedent = NULL;

    // Traverse the blocks to find the product
    while (courant != NULL) {
        for (int i = 0; i < courant->nbr_produit; i++) {
            if (courant->Produits[i].id == id_produit) {
                // Shift remaining products to remove the product
                for (int j = i; j < courant->nbr_produit - 1; j++) {
                    courant->Produits[j] = courant->Produits[j + 1];
                }
                // Clear the last product slot
                courant->Produits[courant->nbr_produit - 1].nom[0] = '\0';
                courant->Produits[courant->nbr_produit - 1].id = 0;
                courant->Produits[courant->nbr_produit - 1].prix = 0.0;
                courant->Produits[courant->nbr_produit - 1].is_deleted = 0;

                courant->nbr_produit--;
                printf("Produit avec ID %d supprimé physiquement.\n", id_produit);
                return;
            } else if (courant->Produits[i].id > id_produit) {
                // Stop searching if IDs are sorted and exceed the target
                printf("Produit avec ID %d non trouvé.\n", id_produit);
                return;
            }
        }
        precedent = courant;
        courant = courant->suivant;
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
}

void supprimerEnregistrementContigueTrieePhysique(Meta* meta, MS* ms, int id) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    // Check if the structure is contiguous and sorted
    if (meta->organisation_globale != 1 || meta->organisation_interne != 1) {
        printf("Erreur : La structure n'est pas en mode contigu et trié.\n");
        return;
    }

    // Traverse contiguous blocks
    int blocIndex = meta->premier_bloc_contigue;
    while (blocIndex < meta->premier_bloc_contigue + meta->taille) {
        BLOC* bloc = &ms->blocs[blocIndex]; // Access the block
        for (int i = 0; i < bloc->nbr_produit; i++) {
            // Check if the ID matches
            if (bloc->Produits[i].id == id) {
                // Shift products to remove the element
                for (int j = i; j < bloc->nbr_produit - 1; j++) {
                    bloc->Produits[j] = bloc->Produits[j + 1];
                }

                // Clear the last product slot
                bloc->Produits[bloc->nbr_produit - 1].id = 0;
                strcpy(bloc->Produits[bloc->nbr_produit - 1].nom, "");
                bloc->Produits[bloc->nbr_produit - 1].prix = 0.0;
                bloc->Produits[bloc->nbr_produit - 1].is_deleted = 0;

                // Decrease the product count in the block
                bloc->nbr_produit--;
                printf("Enregistrement avec l'ID %d supprimé physiquement.\n", id);
                return;
            }
        }
        blocIndex++;
    }

    // If the record is not found
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

//Suppression logique (mode contigu trie)
void SuppressionLogiqueContiguTrie(Meta* meta, MS* ms, int id_produit) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Check if the block is allocated
            BLOC* bloc = &ms->blocs[i];
            int gauche = 0;
            int droite = bloc->nbr_produit - 1;

            // Perform binary search within the block
            while (gauche <= droite) {
                int milieu = (gauche + droite) / 2;
                if (bloc->Produits[milieu].id == id_produit) { // Match found
                    bloc->Produits[milieu].is_deleted = 1; // Mark as deleted
                    meta->nbr_produit--; // Decrement product count

                    printf("Produit avec ID %d supprimé logiquement dans un bloc trié.\n", id_produit);
                    return;
                } else if (bloc->Produits[milieu].id < id_produit) {
                    gauche = milieu + 1;
                } else {
                    droite = milieu - 1;
                }
            }
        }
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
}
void SuppressionLogiqueContiguNonTrie(Meta* meta, MS* ms, int id_produit) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Check if the block is allocated
            BLOC* bloc = &ms->blocs[i];

            for (int j = 0; j < bloc->nbr_produit; j++) {
                if (bloc->Produits[j].id == id_produit) { // Match found
                    bloc->Produits[j].is_deleted = 1; // Mark as deleted
                    meta->nbr_produit--; // Decrement product count

                    printf("Produit avec ID %d supprimé logiquement dans un bloc non trié.\n", id_produit);
                    return;
                }
            }
        }
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
}


void insererEnregistrementNonTrieContigu(Meta* meta, MS* ms, Produit* nouveau_produit) {
    if (!meta || !ms || !nouveau_produit) {
        printf("Erreur : Métadonnées, mémoire secondaire ou produit non valides.\n");
        return;
    }

    // Search for an allocated block with available space
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Block is allocated
            BLOC* bloc = &ms->blocs[i];
            if (bloc->nbr_produit < FB) { // Block has space
                bloc->Produits[bloc->nbr_produit] = *nouveau_produit; // Insert product
                bloc->nbr_produit++;
                meta->nbr_produit++;
                printf("Produit inséré avec succès dans le bloc %d.\n", i);
                return;
            }
        }
    }

    // If no allocated block has space, allocate a new block
    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 0) { // Block is free
            ms->Table_dallocation[i] = 1; // Allocate the block
            BLOC* nouveau_bloc = &ms->blocs[i];
            nouveau_bloc->nbr_produit = 1;
            nouveau_bloc->Produits[0] = *nouveau_produit; // Insert product
            nouveau_bloc->suivant = NULL;

            meta->nbr_produit++;
            printf("Produit inséré avec succès dans un nouveau bloc %d.\n", i);
            return;
        }
    }

    printf("Erreur : Pas assez d'espace disponible pour insérer le produit.\n");
}
void insererEnregistrementTrieContigu(Meta* meta, MS* ms, Produit* nouveau_produit) {
    if (!meta || !ms || !nouveau_produit) {
        printf("Erreur : Métadonnées, mémoire secondaire ou produit non valides.\n");
        return;
    }

    int blocIndex = meta->premier_bloc_contigue;
    int totalBlocs = meta->taille;

    // Traverse the contiguous blocks
    for (int i = 0; i < totalBlocs; i++) {
        BLOC* bloc = &ms->blocs[blocIndex + i];

        // Find the correct position to insert the product
        int j;
        for (j = bloc->nbr_produit - 1; j >= 0; j--) {
            if (bloc->Produits[j].id > nouveau_produit->id) {
                bloc->Produits[j + 1] = bloc->Produits[j]; // Shift products to the right
            } else {
                break;
            }
        }

        // Insert the product if there is space
        if (bloc->nbr_produit < FB) {
            bloc->Produits[j + 1] = *nouveau_produit; // Insert new product
            bloc->nbr_produit++;
            meta->nbr_produit++;
            printf("Produit inséré dans le bloc %d de manière triée.\n", blocIndex + i);
            return;
        } else {
            // Handle overflow by shifting the last product to the next block
            Produit shiftedProduct = bloc->Produits[FB - 1];
            for (int k = FB - 1; k > j + 1; k--) {
                bloc->Produits[k] = bloc->Produits[k - 1];
            }
            bloc->Produits[j + 1] = *nouveau_produit; // Insert new product

            // Shift the last product to the next block
            if (i + 1 < totalBlocs) {
                BLOC* nextBloc = &ms->blocs[blocIndex + i + 1];
                for (int k = nextBloc->nbr_produit; k > 0; k--) {
                    nextBloc->Produits[k] = nextBloc->Produits[k - 1];
                }
                nextBloc->Produits[0] = shiftedProduct;
                nextBloc->nbr_produit++;
            } else {
                // If no space is available, allocate a new block
                int newIndex = FirstEmptyBloc(ms);
                if (newIndex != -1) {
                    BLOC* newBloc = &ms->blocs[newIndex];
                    initializeBloc(newBloc);
                    newBloc->Produits[0] = shiftedProduct;
                    newBloc->nbr_produit = 1;
                    ms->Table_dallocation[newIndex] = 1;
                    meta->taille++; // Increase the file size
                } else {
                    printf("Erreur : Pas assez d'espace disponible pour insérer le produit.\n");
                    return;
                }
            }
        }
    }

    printf("Produit inséré avec succès.\n");
}
Position rechercherEnregistrementNonTrieContigu(Meta* meta, MS* ms, int id_produit) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return (Position){-1, -1};
    }

    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Block is allocated
            BLOC* bloc = &ms->blocs[i];
            for (int j = 0; j < bloc->nbr_produit; j++) {
                if (bloc->Produits[j].id == id_produit) { // Match found
                    return (Position){i, j}; // Return block and position
                }
            }
        }
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
    return (Position){-1, -1}; // Return invalid position if not found
}
Position rechercherEnregistrementTrieContigu(Meta* meta, MS* ms, int id_produit) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return (Position){-1, -1};
    }

    int blocIndex = meta->premier_bloc_contigue;
    int totalBlocs = meta->taille;

    // Perform binary search across all contiguous blocks
    int left = 0;
    int right = totalBlocs * FB - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int bloc = blocIndex + (mid / FB);
        int pos = mid % FB;

        if (ms->blocs[bloc].Produits[pos].id == id_produit) {
            return (Position){bloc, pos}; // Return block and position
        } else if (ms->blocs[bloc].Produits[pos].id < id_produit) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
    return (Position){-1, -1};
}
void supprimerEnregistrementNonTrieContiguPhysique(Meta* meta, MS* ms, int id_produit) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    for (int i = 0; i < NBR_BLOCS; i++) {
        if (ms->Table_dallocation[i] == 1) { // Block is allocated
            BLOC* bloc = &ms->blocs[i];
            for (int j = 0; j < bloc->nbr_produit; j++) {
                if (bloc->Produits[j].id == id_produit) { // Match found
                    // Shift products to fill the gap
                    for (int k = j; k < bloc->nbr_produit - 1; k++) {
                        bloc->Produits[k] = bloc->Produits[k + 1];
                    }
                    bloc->nbr_produit--; // Decrease product count in block
                    meta->nbr_produit--; // Update total product count
                    printf("Produit avec ID %d supprimé avec succès.\n", id_produit);
                    return;
                }
            }
        }
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
}

void suppressionlogiqueEnregistrementChaineNonTriee(Meta* meta, MS* ms, int id) {
    if (!meta || !ms) {
        printf("Erreur : Métadonnées ou mémoire secondaire non valides.\n");
        return;
    }

    // Start traversing from the first block
    BLOC* current = meta->premier_bloc;
    int blocIndex = 0;

    // Traverse the chain of blocks
    while (current != NULL) {
        for (int i = 0; i < current->nbr_produit; i++) {
            if (current->Produits[i].id == id) {
                // Mark the product as logically deleted
                current->Produits[i].is_deleted = 1;
                printf("Enregistrement avec l'ID %d supprimé logiquement dans le bloc %d, index %d.\n", id, blocIndex, i);
                return;
            }
        }

        // Move to the next block in the chain
        current = current->suivant;
        blocIndex++;
    }

    printf("Enregistrement avec l'ID %d non trouvé.\n", id);
}

void supprimerlogiquementEnregistrementChaineTriee(Meta* meta, int id) {
    if (!meta) {
        printf("Erreur : Métadonnées non valides.\n");
        return;
    }

    if (meta->organisation_globale != 0 || meta->organisation_interne != 1) {
        printf("Erreur : Cette fonction est réservée aux structures chaînées triées.\n");
        return;
    }

    BLOC* current = meta->premier_bloc;

    // Parcourir les blocs
    while (current != NULL) {
        for (int i = 0; i < current->nbr_produit; i++) {
            if (current->Produits[i].id == id) {
                current->Produits[i].is_deleted = 1; // Marquer comme supprimé
                printf("Enregistrement avec l'ID %d supprimé logiquement.\n", id);
                return;
            }
        }
        current = current->suivant;
    }

    printf("Enregistrement avec l'ID %d non trouvé.\n", id);
}

void supprimerEnregistrementChaineNonTrieePhysique(Meta* meta, MS* ms, int id_produit) {
    if (!meta || !ms || !meta->premier_bloc) {
        printf("Erreur : Métadonnées ou liste des blocs non valides.\n");
        return;
    }

    // Assurer que la structure est chaînée et non triée
    if (meta->organisation_globale != 0 || meta->organisation_interne != 0) {
        printf("Erreur : La structure n'est pas en mode chaîné et non trié.\n");
        return;
    }

    BLOC* courant = meta->premier_bloc;

    // Parcourir les blocs pour trouver le produit
    while (courant != NULL) {
        for (int i = 0; i < courant->nbr_produit; i++) {
            if (courant->Produits[i].id == id_produit) {
                // Marquer le produit comme supprimé
                courant->Produits[i].nom[0] = '\0'; // Effacer le nom
                courant->Produits[i].id = 0;         // Réinitialiser l'ID
                courant->Produits[i].prix = 0.0;     // Réinitialiser le prix
                courant->Produits[i].is_deleted = 1; // Indiquer que le produit est supprimé

                printf("Produit avec ID %d supprimé physiquement.\n", id_produit);
                return;
            }
        }
        courant = courant->suivant;
    }

    printf("Produit avec ID %d non trouvé.\n", id_produit);
}

void afficherMenu() {
    printf("\n  Menu de gestion des fichiers \n");
    printf("1. Initialiser la mémoire secondaire\n");
    printf("2. Créer un fichier\n");
    printf("3. Afficher les métadonnées des fichiers\n");
    printf("4. Rechercher un enregistrement par ID\n");
    printf("5. Insérer un nouvel enregistrement\n");
    printf("6. Supprimer un enregistrement (logique ou physique)\n");
    printf("7. Défragmenter un fichier\n");
    printf("8. Supprimer un fichier\n");
    printf("9. Renommer un fichier\n");
    printf("10. Compactage de la mémoire secondaire\n");
    printf("11. Vider la mémoire secondaire\n");
    printf("12. Quitter\n");
}

int main() {
    configConsole(); // Configure the console
    MS memoireSecondaire;
    Meta fichier;
    int choix;
    Position pos;

    // Initialize the secondary memory
    initialiserTableAllocation(&memoireSecondaire);

    // Main menu loop
    do {
        afficherMenu();
        printf("\nEntrez votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                initialiserTableAllocation(&memoireSecondaire);
                printf("Mémoire secondaire initialisée.\n");
                break;

            case 2:
                creerfichier(&fichier);
                chargerDansMS(&fichier, &memoireSecondaire);
                printf("Fichier créé et chargé dans la mémoire secondaire.\n");
                break;

            case 3:
                afficherMeta(&fichier);
                break;

            case 4: {
                int id;
                printf("Entrez l'ID de l'enregistrement à rechercher : ");
                scanf("%d", &id);

                if (fichier.organisation_globale == 1) { // Contiguous
                    if (fichier.organisation_interne == 1)
                        pos = rechercherEnregistrementTrieContigu(&fichier, &memoireSecondaire, id);
                    else
                        pos = rechercherEnregistrementNonTrieContigu(&fichier, &memoireSecondaire, id);
                } else { // Chained
                    if (fichier.organisation_interne == 1)
                        pos = rechercherEnregistrementTrieChaine(&fichier, &memoireSecondaire, id);
                    else
                        pos = rechercherEnregistrementNonTrieChaine(&fichier, &memoireSecondaire, id);
                }

                if (pos.bloc != -1)
                    printf("Enregistrement trouvé dans le bloc %d, position %d.\n", pos.bloc, pos.position);
                else
                    printf("Enregistrement non trouvé.\n");
                break;
            }

            case 5: {
                Produit produit;
                printf("Entrez l'ID du produit : ");
                scanf("%d", &produit.id);
                printf("Entrez le nom du produit : ");
                scanf("%s", produit.nom);
                printf("Entrez le prix du produit : ");
                scanf("%f", &produit.prix);

                if (fichier.organisation_globale == 1) { // Contiguous
                    if (fichier.organisation_interne == 1)
                        insererEnregistrementTrieContigu(&fichier, &memoireSecondaire, &produit);
                    else
                        insererEnregistrementNonTrieContigu(&fichier, &memoireSecondaire, &produit);
                } else { // Chained
                    if (fichier.organisation_interne == 1)
                        insererEnregistrementTrieChaine(produit, &fichier, &memoireSecondaire);
                    else
                        insererEnregistrementNonTrieChaine(produit, &fichier, &memoireSecondaire);
                }
                break;
            }

            case 6: {
                int id, choixSuppression;
                printf("Entrez l'ID de l'enregistrement à supprimer : ");
                scanf("%d", &id);
                printf("Choisissez le type de suppression :\n");
                printf("1. Suppression logique\n");
                printf("2. Suppression physique\n");
                printf("Votre choix : ");
                scanf("%d", &choixSuppression);

                if (choixSuppression == 1) { // Logical deletion
                    if (fichier.organisation_globale == 1) { // Contiguous
                        if (fichier.organisation_interne == 1)
                            SuppressionLogiqueContiguTrie(&fichier, &memoireSecondaire, id);
                        else
                            SuppressionLogiqueContiguNonTrie(&fichier, &memoireSecondaire, id);
                    } else { // Chained
                        if (fichier.organisation_interne == 1)
                            supprimerlogiquementEnregistrementChaineTriee(&fichier, id);
                        else
                            suppressionlogiqueEnregistrementChaineNonTriee(&fichier, &memoireSecondaire, id);
                    }
                } else if (choixSuppression == 2) { // Physical deletion
                    if (fichier.organisation_globale == 1) { // Contiguous
                        if (fichier.organisation_interne == 1)
                            supprimerEnregistrementContigueTrieePhysique(&fichier, &memoireSecondaire, id);
                        else
                            supprimerEnregistrementNonTrieContiguPhysique(&fichier, &memoireSecondaire, id);
                    } else { // Chained
                        if (fichier.organisation_interne == 1)
                            supprimerEnregistrementChaineTrieePhysique(&fichier, &memoireSecondaire, id);
                        else
                            supprimerEnregistrementChaineNonTrieePhysique(&fichier, &memoireSecondaire, id);
                    }
                } else {
                    printf("Choix invalide pour le type de suppression.\n");
                }
                break;
            }

            case 7:
                if (fichier.organisation_globale == 1) { // Contiguous
                    if (fichier.organisation_interne == 1)
                        defragmentationContigueTrie(&fichier, &memoireSecondaire);
                    else
                        defragmentationContigueNonTrie(&fichier, &memoireSecondaire);
                } else { // Chained
                    if (fichier.organisation_interne == 1)
                        defragmentationChaineTriee(&fichier, &memoireSecondaire);
                    else
                        defragmentationChaineeNonTrie(&fichier, &memoireSecondaire);
                }
                break;

            case 8:
                Supprimerfichier(&fichier, &memoireSecondaire);
                break;

            case 9: {
                char nouveauNom[21];
                printf("Entrez le nouveau nom du fichier : ");
                scanf("%s", nouveauNom);
                renamefile(&fichier, nouveauNom);
                break;
            }

            case 10:
                if (fichier.organisation_globale == 1)
                    compactageMemoireContigus(&memoireSecondaire, &fichier);
                else
                    CompactageMemoireChaine(&memoireSecondaire, &fichier);
                break;

            case 11:
                initialiserTableAllocation(&memoireSecondaire);
                printf("Mémoire secondaire vidée.\n");
                break;

            case 12:
                printf("Quitter le programme.\n");
                break;

            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix != 12);

    return 0;
}



