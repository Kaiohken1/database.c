#include "btree.h"

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

    return node;
}

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
    tree->root = root ? root : NULL;
    return tree;
}

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

void freeBTree(BTree *tree) {
    if (tree != NULL) {
        if (tree->root != NULL) {
            freeNode(tree->root);
        }
        free(tree);
    }
}


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


//Ici on reçoit un noeud qui dépasse la taille maximale autorisée
//il faut donc le scinder en deux et isoler sa valeur médianne pour la transformer en parent
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


void changeRoot(Node *node, BTree *tree) {
    tree->root = node;
    printf("Noeud racine changé à %ld\n", tree->root->keys[0]);
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
 * @param BTree tr
 * @param uint64_t value
 * @param Bool debug
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

                for (uint8_t j = i; j < maxIndex; j++) {
                    node->keys[j] = node->keys[j + 1];
                    node->rows[j] = node->rows[j + 1];
                }

                node->keys[maxIndex] = 0;
                node->rows[maxIndex] = NULL;
                node->numKeys--;

                keyDeleted = TRUE;

                if (node->numKeys < MIN_KEYS) {
                    rebalanceNode(node);
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

void rebalanceNode(Node *node) {
    Node *parent = node->parent;
    if(parent == NULL) {
        return;
    }

    int8_t index = -1;
    for(uint8_t i = 0; i <= parent->numKeys; i++) {
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
    } else if (rightSib) {
        mergeNodes(node, parent, rightSib, index, TRUE);
    } else if (leftSib) {
        mergeNodes(node, parent, rightSib, index, FALSE);
    } else {
        fprintf(stderr, "Erreur : impossible de supprimer la clé du noeud");
        return;
    };

    if (parent->numKeys < MIN_KEYS) {
        rebalanceNode(parent);  // Appel récursif sur le parent si déséquilibré
    }
}

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


void mergeNodes(Node *node, Node *parent, Node *sibling, uint8_t parentIndex, Bool rightDirection) {
    sibling->keys[sibling->numKeys] = parent->keys[parentIndex];
    sibling->rows[sibling->numKeys] = cloneRow(parent->rows[parentIndex]);
    sibling->numKeys++;

    for (uint8_t i = 0; i < node->numKeys; i++) {
        sibling->keys[sibling->numKeys] = node->keys[i];
        sibling->rows[sibling->numKeys] = cloneRow(node->rows[i]);
        sibling->numKeys++;
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








