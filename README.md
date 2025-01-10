
# Simulateur Simplifié d'un Système de Gestion de Fichiers (SGF)

## Titre du projet
Simulateur Simplifié d'un Système de Gestion de Fichiers (SGF) 

## Description du Projet 
Ce projet vise à développer un simulateur simplifié de Système de Gestion de Fichiers (SGF) Il modélise les principes fondamentaux de la gestion des fichiers, y compris l'organisation de la
mémoire secondaire, l'allocation de l'espace pour les fichiers, et la gestion des métadonnées, tout en permettant de réaliser des opérations sur les fichiers telles que la création, l'insertion, la recherche et la suppression

## Objectifs du projet
Ce projet a pour objectif de développer un simulateur simplifié de Système de Gestion de Fichiers (SGF) afin de modéliser les principes fondamentaux de la gestion des fichiers.
Il permet de reproduire les mécanismes de base d'un SGF réel, tels que :
- L'organisation de la mémoire secondaire en blocs,
- L'allocation de l'espace pour les fichiers,
- La gestion des métadonnées,
- Et les opérations courantes sur les fichiers (création, insertion, recherche, suppression).

## Prérequis
- Compilateur C : Un compilateur compatible avec le langage C (par exemple, GCC).
- Environnement de développement : Il est recommandé d'utiliser un IDE comme Code::Blocks ou un éditeur de texte avec des outils de compilation pour développer et tester le projet
- Bibliothèques utilisées :
```c
#ifdef _WIN32
#include <windows.h> // Pour la configuration de la console sous Windows
#endif
#include <stdio.h>   // Pour les entrées/sorties standard
#include <stdlib.h>  // Pour les fonctions utilitaires (malloc, free, etc.)
#include <stdbool.h> // Pour le type booléen
#include <locale.h>  // Pour la configuration des paramètres régionaux
```

# Simulateur du Système de Gestion de Fichiers (SGF)
Le simulateur SGF vous permet d'effectuer diverses opérations sur la gestion des fichiers et de la mémoire secondaire. Il propose un menu interactif pour gérer les fichiers stockés dans la mémoire et les opérations associées. Ce fichier `README` vous guidera dans l'utilisation du simulateur.

## Menu Principal
Après avoir lancé le programme, un menu principal s'affichera avec les options suivantes :

### 1. Initialiser la mémoire secondaire
Cette option vous permet de définir la mémoire secondaire, notamment le nombre total de blocs et la taille des blocs.

### 2. Créer un fichier
Vous serez invité à entrer le nom du fichier, le nombre d'enregistrements à stocker, ainsi que les modes d'organisation (globale et interne). Cette option alloue les blocs nécessaires et charge le fichier dans la mémoire secondaire.

### 3. Afficher l'état de la mémoire secondaire
Affiche graphiquement l'état de chaque bloc de la mémoire (libre ou occupé), ainsi que les informations sur les fichiers stockés dans chaque bloc.

### 4. Afficher les métadonnées des fichiers
Affiche les métadonnées des fichiers sous forme de tableau, incluant le nom du fichier, la taille et d'autres informations associées.

### 5. Rechercher un enregistrement par son ID
Permet de rechercher un enregistrement spécifique dans un fichier donné. Le simulateur retourne sa position, comprenant le numéro de bloc et son déplacement.

### 6. Insérer un nouvel enregistrement
Permet d'ajouter un enregistrement dans un fichier existant en respectant les modes d'organisation spécifiés.

### 7. Supprimer un enregistrement
Il existe deux types de suppression d'enregistrement :
- **Logique** : Marque l'enregistrement comme supprimé.
- **Physique** : Réorganise les blocs pour libérer l'espace occupé par l'enregistrement supprimé.

### 8. Défragmenter un fichier
Réorganise les blocs d'un fichier pour récupérer l'espace inutilisé après des suppressions logiques, améliorant ainsi la gestion de l'espace disque.

### 9. Renommer un fichier
Permet de renommer un fichier existant dans la mémoire secondaire.

### 10. Supprimer un fichier
Supprime un fichier existant et libère les blocs dans la mémoire secondaire.

### 11. Compactage de la mémoire secondaire
Regroupe tous les fichiers dans la mémoire secondaire afin d'éliminer la fragmentation, optimisant l'utilisation de l'espace disque.

### 12. Vider la mémoire secondaire
Efface toutes les données dans la mémoire secondaire et réinitialise son état, supprimant tous les fichiers et leurs enregistrements.

### 13. Quitter le programme
Ferme le simulateur.

## Exemple d'utilisation
Voici un exemple de déroulement d'une session de simulation :

1. **Démarrez le programme.**
2. Sélectionnez l'option "Initialiser la mémoire secondaire" et définissez le nombre de blocs ainsi que la taille des blocs.
3. Créez un fichier en précisant son nom et les modes d'organisation.
4. Insérez des enregistrements dans ce fichier.
5. Recherchez un enregistrement spécifique par son ID.
6. Effectuez une suppression logique ou physique d'un enregistrement.
7. Défragmentez le fichier et affichez les métadonnées des fichiers.
8. Quittez le programme après avoir terminé.

## Installation
Clonez ou téléchargez ce projet sur votre machine locale :
```bash
git clone https://github.com/SofiaNoure/SimulateurGF.git
```

Compilez le programme avec GCC :
```bash
gcc -o simulateur_GF simulateur_GF.c
```

Exécutez le programme :
```bash
./simulateur_GF
```

## Structure du code
Le code source est structuré de la manière suivante :

- simulateur_GF.c : Le fichier principal qui contient toute la logique du simulateur, y compris la gestion de la mémoire secondaire, des fichiers et des enregistrements.
- README.md : Ce fichier de documentation expliquant le projet, son installation et son utilisation.
   
## Auteur
Ce projet a été développé par un groupe de 5 personnes :
- Delhoum Lina
- Akkouchi Nesrine
- Sofia Noureddine
- Ouari Yasmine
- Isra Foudili

