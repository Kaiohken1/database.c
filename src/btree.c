#include "btree.h"

/**
 * Création d'un noeud
 * @param values Tableau de clés à insérer
 * @param numValues Nombre de clés à insérer
 * @param isRoot Etat du noeud
 * @param rows Tableau de lignes
 * @return Node
 */
Node *createNode(uint64_t values[], uint8_t numValues, Bool isRoot, Row *rows[]) {
    Node *node = (Node*)malloc(sizeof(*node));
    if (node == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique\n");
        exit(EXIT_FAILURE);
    }
    node->children = NULL;
    node->numKeys = 0;
    memset(node->keys, 0, sizeof(node->keys));
    memset(node->rows, 0, sizeof(node->keys));

    for (uint8_t i = 0; i < numValues; i++) {
        if (values[i] != 0) {
            node->keys[i] = values[i];
            node->rows[i] = rows[i];
            if(node->rows != NULL)
                node->rows[i]->id = values[i];
            node->numKeys++;
        }
    }
    node->parent = NULL;
    node->isRoot = isRoot;

    assert(node != NULL);

    return node;
}

/**
 * Création du Btree
 * @param root Noeud racine de l'arbre
 * @return Btree
 */
BTree *createTree(Node *root) {
    if(root != NULL && root->isRoot == FALSE) {
        printf("Erreur : impossible de créer un arbre sans un noeud de type racine\n");
        exit(EXIT_FAILURE);
    }

    BTree *tree = (BTree*)malloc(sizeof(*tree));
    if (tree == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique pour l'arbre\n");
        exit(EXIT_FAILURE);
    }

    tree->totalId = 0;
    tree->root = root ? root : NULL;

    assert(tree != NULL);
    return tree;
}

/**
 * Libération récursive de la mémoire d'un noeud et de ses enfants
 * @param node Noeud à libérer
 * @return void
 */
void freeNode(Node *node) {
    if (node != NULL) {
        if (node->children != NULL) {
            for (int i = 0; i <= node->numKeys; i++) {
                if (node->children[i] != NULL) {
                    freeNode(node->children[i]);
                }
            }
            free(node->children);
            node->children = NULL;
        }

        for(uint8_t i = 0; i <= node->numKeys; i++) {
                if (node->rows[i] != 0) {
                    free(node->rows[i]);
                    node->rows[i] = NULL;
                }
        }
        free(node);
    }
}

/**
 * Libération du Btree et de tous ses noeuds
 * @param tree Arbre B à libérer
 * @return void
 */
void freeBTree(BTree *tree) {
    if (tree != NULL) {
        if (tree->root != NULL) {
            freeNode(tree->root);
        }
        free(tree);
    }
}

/**
 * Insértion d'une clé dans le Btree
 * @param value valeur de la clé
 * @param tree arbre dans lequel insérer la clé
 * @param name valeur pour la colonne nom de la table
 * @return void
 */
void insertKey(uint64_t value, BTree *tree, char name[50]) {
    Node *node = tree->root;

    if (node == NULL) {
        fprintf(stderr, "Erreur : l'arbre est vide\n");
        return;
    }

    while (node->children != NULL) {
        int8_t childPos = node->numKeys;
        while (childPos > 0 && value < node->keys[childPos - 1]) {
            childPos--;
        }

        Node *child = node->children[childPos];

        if (child == NULL) {
            break;
        }
        node = child;
    }

    Row *row = malloc(sizeof(Row));
    if (row == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return;
    }
    strcpy(row->name, name);

    insertKeyOnNode(value, node, row);

    if (node->numKeys == MAX_KEYS + 1) {
        splitNode(node, tree);
    }
}

/**
 * Insertion d'une clé donnée dans un noeud avec tri croissant
 * @param value
 * @param node
 * @param row
 */
void insertKeyOnNode(uint64_t value, Node *node, Row *row) {
    int8_t i;
    for (i = node->numKeys - 1; i >= 0 && node->keys[i] > value; i--) {
        node->keys[i + 1] = node->keys[i];
        node->rows[i + 1] = node->rows[i];  
    }

    node->keys[i + 1] = value;
    row->id = value;
    node->rows[i + 1] = row;

    node->numKeys++;
}


/**
 * Séparation d'un noeud en deux et passage de sa valeur médianne chez le parent
 * @param node
 * @param tree
 */
void splitNode(Node *node, BTree *tree) {
    uint8_t medianIndex = MAX_KEYS / 2;
    uint64_t medianValue = node->keys[medianIndex];
    Row *medianRow = node->rows[medianIndex];

    uint64_t leftVal[medianIndex];
    uint64_t rightVal[node->numKeys - medianIndex - 1];

    Row *leftRows[MAX_KEYS / 2];
    Row *rightRows[MAX_KEYS / 2];

    for (uint8_t i = 0; i < medianIndex; i++) {
        leftVal[i] = node->keys[i];
        leftRows[i] = node->rows[i];
    }

    for (uint8_t i = medianIndex + 1; i < node->numKeys; i++) {
        rightVal[i - (medianIndex + 1)] = node->keys[i];
        rightRows[i - (medianIndex + 1)] = node->rows[i];
    }

    Node *leftN = createNode(leftVal, medianIndex, FALSE, leftRows);
    Node *rightN = createNode(rightVal, medianIndex, FALSE, rightRows);

    // Redistribution des enfants
    if (node->children != NULL) {
        leftN->children = (Node**)malloc((MAX_KEYS + 2) * sizeof(Node*));
        rightN->children = (Node**)malloc((MAX_KEYS + 2) * sizeof(Node*));

        if (leftN->children == NULL || rightN->children == NULL) {
            fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique pour les enfants\n");
            free(leftN);
            free(rightN);
            exit(EXIT_FAILURE);
        }

        memset(leftN->children, 0, (MAX_KEYS + 2) * sizeof(Node*));
        memset(rightN->children, 0, (MAX_KEYS + 2) * sizeof(Node*));

        for (uint8_t i = 0; i <= medianIndex; i++) {
            leftN->children[i] = node->children[i];

            if (leftN->children[i] != NULL) {
                leftN->children[i]->parent = leftN;  
            }
        }

        for (uint8_t i = medianIndex + 1, j = 0; i <= node->numKeys; i++, j++) {
            rightN->children[j] = node->children[i];
            if (rightN->children[j] != NULL) {
                rightN->children[j]->parent = rightN;  
            }
        }
    }

    uint64_t medianVal[] = {medianValue};
    Row *medianRows[] = {medianRow};

    if (node->parent == NULL) {
        Node *parent = createNode(medianVal, 1, node->isRoot, medianRows);
        parent->children = (Node**)malloc((MAX_KEYS + 2) * sizeof(Node*));

        if (parent->children == NULL) {
            fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique pour les enfants\n");
            free(parent);
            exit(EXIT_FAILURE);
        }

        memset(parent->children, 0, (MAX_KEYS + 2) * sizeof(Node*));

        parent->children[0] = leftN;
        parent->children[1] = rightN;
        leftN->parent = parent;
        rightN->parent = parent;

        if (node->isRoot) {
            changeRoot(parent, tree);
        }
    } else {
        Node *parent = node->parent;
        insertKeyOnNode(medianValue, parent, medianRow);
        uint8_t i = findChildPosition(parent, node);

        for (uint8_t j = parent->numKeys; j > i + 1; j--) {
            parent->children[j] = parent->children[j - 1];
        }
        parent->children[i] = leftN;
        parent->children[i + 1] = rightN;

        leftN->parent = parent;
        rightN->parent = parent;

        if (parent->numKeys > MAX_KEYS) {
            splitNode(parent, tree);
        }
    }

    free(node->children);
    free(node);
}

/**
 * Récupération de l'index dans le parent selon l'enfant
 * 
 */
uint8_t findChildPosition(Node *parent, Node *child) {
    if(parent == NULL || child == NULL) {
        return -1;
    }

    for(uint8_t i = 0; i < parent->numKeys + 1; i++) {
        if(parent->children[i] == child) {
            return i;
        }
    }

    return -1;
}

/**
 * Changement du noeud racine avec celui donné
 * @param noeud Nouvelle racine
 * @param tree Arbre recevant le nouveau noeud racine
 */
void changeRoot(Node *node, BTree *tree) {
    tree->root = node;
    // printf("Noeud racine changé à %ld\n", tree->root->keys[0]);
}

void printNode(Node *node, Bool withRows) {
    for (uint8_t i = 0; i < node->numKeys; i++) {
        printf("[%ld] ", node->keys[i]);
    }

    if(node->isRoot) {
        printf(" (%d) -> Root", node->numKeys);
    } else if (node->parent != NULL && node->parent->isRoot) {
        printf(" (%d) -> Root Child", node->numKeys);
    } else {
        printf(" (%d)", node->numKeys);
    }
    printf("\n");

    if(withRows) {
        for(uint8_t i = 0; i < node->numKeys; i++) {
            if (node->rows[i] != 0) {
                printf("ID : [%ld] Nom : '%s'\n", node->rows[i]->id, node->rows[i]->name);
            }
        }
    }
}

void printTree(BTree *tree, Bool withRows) {
    if (tree == NULL || tree->root == NULL) {
        printf("L'arbre est vide.\n");
        return;
    }

    printTreeHelper(tree->root, withRows);
}

void printTreeHelper(Node *node, Bool withRows) {
    if (node == NULL) {
        return;
    }

    printNode(node, withRows);

    if (node->children != NULL) {
        for (uint8_t i = 0; i <= node->numKeys; i++) {
            if (node->children[i] != NULL) {
                printTreeHelper(node->children[i], withRows);
            }
        }
    }
}


void insertTest(uint8_t max, BTree *tr, char name[50]) {
    if (max > 100) {
        max = 100;
    }

    for(uint8_t i = 2; i <= max; i++) {
        insertKey(i, tr, name);
        tr->totalId++;
    }
}

uint64_t getKey(BTree *tr, uint64_t value) {
    Node *node = tr->root;

    while (node != NULL) {
        int i;
        for (i = 0; i < node->numKeys; i++) {
            if (node->keys[i] == value) {
                return node->keys[i];
            } else if (node->keys[i] > value) {
                break;
            }
        }
        if (node->children == NULL) {
            return 0;
        } else {
            node = node->children[i];
        }
    }
    return 0;
}

/**
 * Supression d'une clé dans l'arbre
 * @param tr Arbre dans lequel chercher la clé
 * @param value valeur de la clé à supprimer
 * @param debug Activer le print pour debug
 * @return void
 */
void deleteKey(BTree *tr, uint64_t value, Bool debug) {
    Node *node = tr->root;
    Bool keyDeleted = FALSE;

    while (node != NULL && !keyDeleted) {
        for (int8_t i = 0; i < node->numKeys; i++) {
            uint8_t maxIndex = node->numKeys - 1;
            if (node->keys[i] == value) {
                free(node->rows[i]);
                node->rows[i] = NULL;

                if (node->children != NULL) {
                    takeFromChild(node, i, tr);
                    keyDeleted = TRUE;
                    break;
                }

                for (uint8_t j = i; j < maxIndex; j++) {
                    node->keys[j] = node->keys[j + 1];
                    node->rows[j] = node->rows[j + 1];
                }

                node->keys[maxIndex] = 0;
                node->rows[maxIndex] = NULL;
                node->numKeys--;

                keyDeleted = TRUE;

                if (node->numKeys < MIN_KEYS && !node->isRoot) {
                    rebalanceNode(node, tr);
                }
                break;
            }
        }

        if (keyDeleted) {
            break;
        }

        int8_t childPos = node->numKeys;
        while (childPos > 0 && value < node->keys[childPos - 1]) {
            childPos--;
        }

        node = node->children[childPos];
    }

    if(!keyDeleted) {
        printf("Clé %ld non trouvée dans l'arbre.\n", value);
    }

    if (debug) {
        printf("--------- Supression de %ld -------------\n", value);
        printTree(tr, TRUE);
    }
}

/**
 * Rééquilibrage d'un noeud selon la disponibilité des noeuds adjaçants (récursif)
 * @param node Noeud à rééquilibrer
 * @param tr Arbre contenant le noeud à rééquilibrer
 */
void rebalanceNode(Node *node, BTree *tr) {
    Node *parent = node->parent;
    if(parent == NULL) {
        return;
    }

    int8_t index = -1;
    for(uint8_t i = 0; i < parent->numKeys + 1; i++) {
        if(parent->children[i] == node) {
            index = i;
            break;
        }
    }

    if(index == -1) {
        fprintf(stderr, "Erreur : le noeud donné n'a pas été retrouvé chez son parent");
        return;
    }


    Node *leftSib = index > 0 ? parent->children[index - 1] : NULL;
    Node *rightSib = index < parent->numKeys ? parent->children[index + 1]: NULL;
    if (rightSib != NULL && rightSib->numKeys > MIN_KEYS) {
        shiftKeys(node, parent, rightSib, index, TRUE);
    } else if (leftSib != NULL && leftSib->numKeys > MIN_KEYS) {
        shiftKeys(node, parent, leftSib, index, FALSE);
    } else if (rightSib != NULL) {
        mergeNodes(node, parent, rightSib, index, TRUE, tr, &parent);
    } else if (leftSib != NULL) {
        mergeNodes(node, parent, leftSib, index, FALSE, tr, &parent);
    } else {
        fprintf(stderr, "Erreur : impossible de supprimer la clé du noeud");
        return;
    }
    
    if (parent != NULL && parent->numKeys < MIN_KEYS && !parent->isRoot) {
        rebalanceNode(parent, tr);
    }
}

/**
 * Déplacement d'une clé parent vers le noeud enfant qui n'en a plus assez, et remplacement de celle-ci par la clé d'un noeud adjaçant
 * @param node Le noeud ayant besoin d'être rééquilibré
 * @param parent Le parent chez qui chercher la clé
 * @param sibling Le noeud adjaçant chez qui prendre une clé pour remplacer celle prise dans le parent
 * @param index Position du noeud par rapport au tableau des enfants du parent
 * @param rightDirection Précision si on prend à droite ou a gauche
 * @return void
 */
void shiftKeys(Node *node, Node *parent, Node *sibling, int8_t index, Bool rightDirection) {
    if(rightDirection) {
        node->keys[node->numKeys] = parent->keys[index];
        node->rows[node->numKeys] = parent->rows[index];
        node->numKeys++;

        parent->keys[index] = sibling->keys[0];
        parent->rows[index] = sibling->rows[0];

        for (uint8_t i = 0; i < sibling->numKeys - 1; i++) {
            sibling->keys[i] = sibling->keys[i + 1];
            sibling->rows[i] = sibling->rows[i + 1];
        }

        sibling->keys[sibling->numKeys - 1] = 0;
        sibling->rows[sibling->numKeys - 1] = NULL;
        sibling->numKeys--;
    } else {
        index -= 1;

        for (int8_t i = node->numKeys; i > 0; i--) {
            node->keys[i] = node->keys[i - 1];
            node->rows[i] = node->rows[i - 1];
        }
        node->numKeys++;

        node->keys[0] = parent->keys[index];
        node->rows[0] = parent->rows[index];

        parent->keys[index] = sibling->keys[sibling->numKeys - 1];
        parent->rows[index] = sibling->rows[sibling->numKeys - 1];

        sibling->numKeys--;
    }
}

/**
 * Récupération des clés chez le parent et chez un noeud adjaçant puis fusion des deux
 * @param node Le noeud ayant besoin de clés
 * @param parent Le parent chez qui prendre une clé
 * @param sibling Le noeud adjaçant à fusionner avec le noeud actuel
 * @param parentIndex La position du noeud actuel dans le tableau des enfants du parent
 * @param rightDirection Précision si on prend à droite ou a gauche
 * @param tr Arbre dans lequel effectuer l'opération
 * @param parentPtr Pointeur de pointeur du parent pour pouvoir le libérer si besoin
 * @return void
 * 
 */
void mergeNodes(Node *node, Node *parent, Node *sibling, uint8_t parentIndex, Bool rightDirection, BTree *tr, Node **parentPtr) {
    if (!rightDirection) {
        parentIndex -= 1;
    }
    sibling->keys[sibling->numKeys] = parent->keys[parentIndex];
    sibling->rows[sibling->numKeys] = cloneRow(parent->rows[parentIndex]);
    sibling->numKeys++;

    for (uint8_t i = 0; i < node->numKeys; i++) {
        sibling->keys[sibling->numKeys] = node->keys[i];
        sibling->rows[sibling->numKeys] = cloneRow(node->rows[i]);
        sibling->numKeys++;


        if(sibling->numKeys == MAX_KEYS) {
            break;
        }
    }

    if (parent->isRoot && parent->numKeys == 1) {
        sibling->parent = NULL;
        node->parent = NULL;

        free(parent->children);
        parent->children = NULL; 

        freeNode(parent);
        *parentPtr = NULL;

        freeNode(node);
        
        changeRoot(sibling, tr);
        return;
    }
    
    if (node->children != NULL) {
        for (uint8_t i = 0; i <= node->numKeys; i++) {
            sibling->children[sibling->numKeys + i] = node->children[i];
            node->children[i] = NULL;
        }

        for (uint8_t i = sibling->numKeys; i < sibling->numKeys + node->numKeys + 1; i++) {
            sibling->children[i]->parent = sibling;
        }

        free(node->children);
        node->children = NULL;
    }

    if (rightDirection) {
        for (uint8_t i = parentIndex; i < parent->numKeys; i++) {
            parent->children[i] = parent->children[i + 1];
        }
    } else {
        for (uint8_t i = parentIndex; i < parent->numKeys; i++) {
            parent->children[i] = parent->children[i - 1];
        }
    }
    parent->children[parent->numKeys] = NULL;
    parent->numKeys--;

    for (uint8_t i = 1; i < sibling->numKeys; i++) {
        uint64_t key = sibling->keys[i];
        Row *row = sibling->rows[i];
        uint8_t j = i;

        while (j > 0 && sibling->keys[j - 1] > key) {
            sibling->keys[j] = sibling->keys[j - 1];
            sibling->rows[j] = sibling->rows[j - 1];
            j--;
        }

        sibling->keys[j] = key;
        sibling->rows[j] = row;
    }

    freeNode(node);
}

/**
 * Copie d'une ligne d'un noeud
 * @param original La ligne à copier
 * @return Row
 */
Row *cloneRow(Row *original) {
    if (original == NULL) {
        return NULL;
    }

    Row* newRow = malloc(sizeof(Row));
    if (newRow) {
        *newRow = *original;
    }
    return newRow;
}

/**
 * Prendre une clé depuis un enfant
 */
void takeFromChild(Node *node, uint8_t i, BTree *tr) {
    if (i + 1 <= node->numKeys && node->children[i + 1] != NULL) {
        Node *child = node->children[i + 1];
        node->keys[i] = child->keys[0];
        node->rows[i] = cloneRow(child->rows[0]);

        free(child->rows[0]);

        for (int j = 0; j < child->numKeys - 1; j++) {
            child->keys[j] = child->keys[j + 1];
            child->rows[j] = cloneRow(child->rows[j + 1]);
        }

        child->numKeys--;

        if (child->numKeys < MIN_KEYS) {
            rebalanceNode(child, tr);
        }
        return;

    } else if (i >= 0 && node->children[i] != NULL) {
        Node *child = node->children[i];
        node->keys[i] = child->keys[child->numKeys - 1];
        node->rows[i] = cloneRow(child->rows[child->numKeys - 1]);

        free(child->rows[child->numKeys - 1]);

        child->numKeys--;

        if (child->numKeys < MIN_KEYS) {
            rebalanceNode(child, tr);
        }
        return;
    }

    fprintf(stderr, "Erreur lors de la suppression d'une clé avec des enfants liés");
    return;
}



