#include "btree.h"

Node *createNode(uint64_t values[], uint8_t numValues, Bool isRoot) {
    Node *node = (Node*)malloc(sizeof(*node));
    if (node == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique\n");
        exit(EXIT_FAILURE);
    }
    node->children = NULL;

    node->numKeys = 0;
    memset(node->keys, 0, sizeof(node->keys));

    for (uint8_t i = 0; i < numValues; i++) {
        if (values[i] != 0) {
            node->keys[i] = values[i];
            node->numKeys++;
        }
    }
    node->parent = NULL;
    node->isRoot = isRoot;

    return node;
}

BTree *createTree(Node *root) {
    if(root->isRoot == FALSE) {
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


void insertKey(uint64_t value, BTree *tree) {
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

    insertKeyOnNode(value, node);

    if (node->numKeys == MAX_KEYS + 1) {
        splitNode(node, tree);
    }
}

void insertKeyOnNode(uint64_t value, Node *node) {
    int8_t i;
    for (i = node->numKeys - 1; i >= 0 && node->keys[i] > value; i--) {
        node->keys[i + 1] = node->keys[i];
    }

    node->keys[i + 1] = value;
    node->numKeys++;
}

//Ici on reçoit un noeud qui dépasse la taille maximale autorisée
//il faut donc le scinder en deux et isoler sa valeur médianne pour la transformer en parent
void splitNode(Node *node, BTree *tree) {
    uint8_t medianIndex = MAX_KEYS / 2;
    uint64_t medianValue = node->keys[medianIndex];

    uint64_t leftVal[MAX_KEYS / 2];
    uint64_t rightVal[MAX_KEYS / 2];

    for (uint8_t i = 0; i < medianIndex; i++) {
        leftVal[i] = node->keys[i];
    }

    for (uint8_t i = medianIndex + 1; i < node->numKeys; i++) {
        rightVal[i - (medianIndex + 1)] = node->keys[i];
    }

    Node *leftN = createNode(leftVal, medianIndex, FALSE);
    Node *rightN = createNode(rightVal, medianIndex, FALSE);

    // Redistribution des enfants
    if (node->children != NULL) {
        leftN->children = (Node**)malloc((MAX_KEYS + 1) * sizeof(Node*));
        rightN->children = (Node**)malloc((MAX_KEYS + 1) * sizeof(Node*));

        if (leftN->children == NULL || rightN->children == NULL) {
            fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique pour les enfants\n");
            free(leftN);
            free(rightN);
            exit(EXIT_FAILURE);
        }

        memset(leftN->children, 0, (MAX_KEYS + 1) * sizeof(Node*));
        memset(rightN->children, 0, (MAX_KEYS + 1) * sizeof(Node*));

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

    if (node->parent == NULL) {
        Node *parent = createNode(medianVal, 1, node->isRoot);
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
        insertKeyOnNode(medianValue, parent);
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

void printNode(Node *node) {
    for (uint8_t i = 0; i < node->numKeys; i++) {
        printf("[%ld] ", node->keys[i]);
    }
    node->isRoot ? printf(" (%d) -> Root", node->numKeys) : printf(" (%d)", node->numKeys);
    printf("\n");
}

void printTree(BTree *tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("L'arbre est vide.\n");
        return;
    }

    printTreeHelper(tree->root);
}

void printTreeHelper(Node *node) {
    if (node == NULL) {
        return;
    }

    printNode(node);

    if (node->children != NULL) {
        for (uint8_t i = 0; i <= node->numKeys; i++) {
            if (node->children[i] != NULL) {
                printTreeHelper(node->children[i]);
            }
        }
    }
}

