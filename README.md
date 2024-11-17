
## Description

Ce projet a pour but d’implémenter une base de donnée en C afin de pouvoir gérer manuellement la mémoire allouée et organiser la donnée de manière efficace et optimale.

La gestion des données est effectuée grâce à un **Btree** (variante de l’arbre binaire).

Les opérations supportées sont **INSERT, SELECT, DELETE**.

La table configurée par défaut est constituée d’une colonne Name qui prends des entrées de type chaîne de caractère. Ainsi que d’une colonne ID qui s’incrèmente automatiquement à chaque nouvelle insertion. 

### Prérequis

- Le compilateur gcc ou tout autre compilateur en C
- Valgrind pour lancer la détection de fuites (optionnel)
- gdb pour lancer le programme en mode debug (optionnel)

### Commandes Makefile - Linux

Lancement par défaut du programme
```
make 
```

Lancer le programme avec valgrind (installation requise) afin de détecter les fuites de mémoire potentielles
```
make valgrind
```

Lancer le programme en mode debug avec gdb (installation requise)
```
make gdb
```

## Instructions

Les instructions relatives à la base suivent le même format qu’en SQL (en minuscule), et sont au nombre de trois. Les autres commencent par un point.
### Select
```
select [colonne(s)] where [colonne] = [nom ou id recherché]
```

> select peut prendre plusieurs colonnes en premier argument, il faut simplement les séparer d’une virgule ex : “select id, name”
### Insert
```
insert [chaîne de caractère]
```

### Delete
```
delete where [colonne] = [valeur à supprimer]
```

### Sauvegarder l’état de la base

```
.save
```

### Charger la base depuis l’état du fichier de stockage

```
.load
```

### Quitter le programme

```
.exit
```
## Structure du projet

repl - “Read eval print loop” contient toutes les opération nécessaires au traitement des entrées clavier

btree - gère entièrement la gestion de l’arbre binaire de la bdd

database - sert de lien entre la logique métier du btree et les instructions enregistrée par repl

storage - code relatif à l’enregistrement de l’état de la base de donnée au moment de la sauvegarde et de son chargement

## Btree (Arbre binaire)

### Fonctionnement

Un arbre binaire est une structure de données particulièrement adaptée pour les bases de données car elle permet un accès efficace à la donnée de triée étant donnée que sa complexité de temps est faible.

### Insertion de clé

Un arbre binaire classique fonctionne avec un nœud parent qui possède au maximum deux nœuds enfants. Ces mêmes enfants peuvent avoir deux nœuds enfants eux aussi et ainsi de suite. 

En général on trie les données en fonction de cette configuration avec par exemple les valeurs plus petites que le parent à gauche et les plus grandes à droite.

Le problème que l’on retrouve est que l’arbre binaire est susceptible d’être déséquilibré, ce qui impact son optimalité. Notamment si on insère les données de manière croissante.

![[img/arbre_binaire.png]]
*Arbre binaire déséquilibré suite à une insertion croissante de valeurs allant de 1 à 5*

À la différence, un Btree peut accepter plusieurs valeurs dans ses nœuds et un noeud à donc un nombre d’enfants possible égal au nombre de valeurs + 1. Les valeurs d’un noeud sont toujours triées dans l’ordre croissant.

On donne un nombre maximum d’enfants (4 par défaut ici) ainsi qu’un minimum, excepté pour le noeud racine de l’arbre (2 par défaut).

Lorsqu’un nœud dépasse la limite de valeurs autorisées, celui-ci est scindé en deux et la valeur médiane est poussé vers le nœud parent. S’il y en pas parce qu’on est déjà à la racine, un nouveau nœud est créé et deviens la nouvelle racine de l’arbre

Fonctions utilisées : `createTree`, `createNode`, `splitNode`, `insertKey`

![[img/inser_btree.png]]
*Rééquilibrage automatique du Btree suite à une insertion croissante de valeurs allant de 1 à 5*

En plus de l’équilibrage automatique, la présence de plusieurs clés par noeuds permet de créer des blocs de taille fixe, ce qui optimise également l’utilisation de la mémoire.

### Recherche de clé

Grâce à la capacité de chaque nœud à stocker plusieurs clés, la profondeur est beaucoup plus faible. Cela réduit le nombre de comparaisons et améliore les performances.

Et comme chaque clé est triée dans l’ordre croissant, on utilise également la logique de gauche et droite, d’abord par rapport au valeurs d’un nœud, puis pour le choix de l’enfant dans lequel se déplacer. 

### Suppression de clé

Le défi majeur lors de la suppression de clé est de s’assurer qu’il n’y a pas de déséquilibre dans l’organisation de l’arbre, mais aussi que les bornes minimales et maximales soient respectées quant au nombre de clés dans un nœuds.

Lorsqu’une clé est supprimée, il y a donc un rééquilibrage qui est fait récursivement dès lors qu’un ou plusieurs nœuds finissent par avoir un nombre de clé inférieur au minimal après l’opération.  

Fonctions utilisées : `deleteKey`, `rebalanceNode`

#### Remplacement

La première solution est de prendre une clé au parent et de la remplacer par une autre clé d’un nœud adjacent. La plus grande pour celui de gauche et la plus petite pour celui de droite.

![[img/shiftkeys.png]]
*Décallage des clés après suppression de 12 dans le deuxième enfant*

Fonctions utilisées : `shiftKeys`

### Fusion

Si cela n’est pas possible car les deux nœuds adjacents n’ont pas assez de clés pour pouvoir en perdre une sans tomber en dessous du minimum permis. Alors une fusion est effectuée avec un de ces deux là et notre nœud actuel, pour n’en former plus qu’un qui est réajusté avec le parent. 

![[img/mergenodes.png]]
*Fusion des noeuds adjaçants après suppression de 12 dans le précédent deuxième enfant*

Fonctions utilisées : `mergeNodes`

### Format de la table avec l’arbre

Le Btree correspond dans son ensemble à une seule table, les lignes de la table sont donc elles aussi stockées dans les nœuds.

Les valeurs sont indexées dans l’arbre par leur id, c’est à dire que la clé d’un nœud est toujours égale à l’id d’une ligne, et c’est depuis celle-ci que se fait la recherche.