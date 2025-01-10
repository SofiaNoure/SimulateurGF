# Simulateur Simplifié d'un Système de Gestion de Fichiers (SGF)

## Description du Projet

Ce projet vise à développer un simulateur simplifié de Système de Gestion de Fichiers (SGF). Il modélise les principes fondamentaux de la gestion des fichiers, y compris l'organisation de la mémoire secondaire, l'allocation de l'espace pour les fichiers, et la gestion des métadonnées, tout en permettant de réaliser des opérations sur les fichiers telles que la création, l'insertion, la recherche et la suppression.

## Objectifs du Projet

Ce simulateur permet de reproduire les mécanismes de base d'un SGF réel, tels que :

* L'organisation de la mémoire secondaire en blocs
* L'allocation de l'espace pour les fichiers
* La gestion des métadonnées
* Les opérations courantes sur les fichiers (création, insertion, recherche, suppression)

## Prérequis

### Environnement de Développement

* **Compilateur C** : Un compilateur compatible avec le langage C (par exemple, GCC)
* **IDE** : Il est recommandé d'utiliser Code::Blocks ou un éditeur de texte avec des outils de compilation

### Bibliothèques Utilisées

```c
#ifdef _WIN32
#include  // Pour la configuration de la console sous Windows
#endif
#include    // Pour les entrées/sorties standard
#include   // Pour les fonctions utilitaires (malloc, free, etc.)
#include  // Pour le type booléen
#include   // Pour la configuration des paramètres régionaux
```

## Fonctionnalités

### 1. Initialiser la mémoire secondaire

Permet de définir la mémoire secondaire, notamment le nombre total de blocs et la taille des blocs.

### 2. Créer un fichier

Permet de créer un nouveau fichier en spécifiant :

* Le nom du fichier
* Le nombre d'enregistrements à stocker
* Les modes d'organisation (globale et interne)

### 3. Afficher l'état de la mémoire secondaire

Affiche graphiquement :

* L'état de chaque bloc de la mémoire (libre ou occupé)
* Les informations sur les fichiers stockés dans chaque bloc

### 4. Afficher les métadonnées des fichiers

Présente un tableau contenant :

* Le nom du fichier
* La taille
* Autres informations associées

### 5. Rechercher un enregistrement

Recherche par ID avec retour de :

* Numéro de bloc
* Déplacement dans le bloc

### 6. Insérer un nouvel enregistrement

Ajout d'enregistrements dans un fichier existant selon les modes d'organisation spécifiés.

### 7. Supprimer un enregistrement

Deux types de suppression disponibles :

* **Suppression logique** : Marquage de l'enregistrement comme supprimé
* **Suppression physique** : Réorganisation des blocs pour libérer l'espace

### 8. Défragmenter un fichier

Réorganisation des blocs pour optimiser l'espace après les suppressions logiques.

### 9. Renommer un fichier

Modification du nom d'un fichier existant.

### 10. Supprimer un fichier

Suppression complète avec libération des blocs.

### 11. Compactage de la mémoire

Optimisation de l'espace disque par regroupement des fichiers.

### 12. Vider la mémoire

Réinitialisation complète de la mémoire secondaire.

## Installation

1. Cloner le projet :
```bash
git clone https://github.com/SofiaNoure/SimulateurGF.git
```

2. Compiler le programme :
```bash
gcc -o simulateur_GF simulateur_GF.c
```

3. Exécuter le programme :
```bash
./simulateur_GF
```

## Structure du Code

* **simulateur_GF.c** : Fichier principal contenant la logique du simulateur
* **README.md** : Documentation du projet

## Exemple d'Utilisation

1. Démarrer le programme
2. Initialiser la mémoire secondaire (définir nombre et taille des blocs)
3. Créer un fichier avec ses paramètres d'organisation
4. Insérer des enregistrements
5. Rechercher un enregistrement par ID
6. Effectuer des opérations de suppression
7. Optimiser avec la défragmentation
8. Consulter les métadonnées
9. Quitter le programme

## Auteurs

Le projet a été développé par une équipe de 5 personnes :

* Delhoum Lina
* Akkouchi Nesrine
* Sofia Noureddine
* Ouari Yasmine
* Isra Foudili

