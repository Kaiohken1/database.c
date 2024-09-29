#include "btree.h"

Node *createNode(uint64_t values[], uint8_t numValues, Bool isRoot) {
    Node *node = (Node*)malloc(sizeof(*node));
    if (node == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique\n");
        exit(EXIT_FAILURE);
    }
    
    node->numKeys = 0;
    memset(node->keys, 0, sizeof(node->keys)); 

    for (uint8_t i = 0; i < numValues; i++) {
        if (values[i] != 0) {
            node->keys[i] = values[i];
            node->numKeys++;
        }
    }

    node->children = (Node**)malloc((MAX_KEYS + 1) * sizeof(Node*));
    if (node->children == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique pour les enfants\n");
        free(node);
        exit(EXIT_FAILURE);
    }
    memset(node->children, 0, (MAX_KEYS + 1) * sizeof(Node*));

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
        for (int i = 0; i < MAX_KEYS; i++) {
            if (node->children[i] != NULL) {
                freeNode(node->children[i]);
            }
        }
        free(node->children);  
        free(node);
    }
}

void freeBTree(BTree *tree) {
    if (tree != NULL) {
        freeNode(tree->root);
        free(tree);
    }
}

void insertKey(Node *node, uint64_t value, BTree *tree) {
    int8_t i;  
    for(i = node->numKeys - 1; i >= 0 && node->keys[i] > value; i--) {
        node->keys[i+1] = node->keys[i];
    }

    node->keys[i+1] = value;
    node->numKeys++;

    if (node->numKeys == MAX_KEYS + 1) {
        // printf("Erreur : Le noeud est plein, split après %ld.\n", value);
        // return;
        splitNode(node, tree);
    }
}

//Ici on reçoit un noeud qui dépasse la taille maximale autorisée
//il faut donc le scinder en deux et isoler sa valeur médianne pour la transformer en parent
void splitNode(Node *node, BTree *tree) {
    uint8_t medianIndex = MAX_KEYS / 2;
    uint64_t medianValue = node->keys[medianIndex]; 

    uint64_t leftVal[MAX_KEYS];
    uint64_t rightVal[MAX_KEYS];

    for (uint8_t i = 0; i < medianIndex; i++) {
        leftVal[i] = node->keys[i];
    }

    for (uint8_t i = medianIndex + 1; i < node->numKeys; i++) {
        rightVal[i - (medianIndex + 1)] = node->keys[i]; 
    }

    Node *leftN = createNode(leftVal, medianIndex, FALSE);
    Node *rightN = createNode(rightVal, node->numKeys - medianIndex - 1, FALSE);

    uint64_t medianVal[] = {medianValue};
    Node *parent = createNode(medianVal, 1, node->isRoot);

    parent->children[0] = leftN;
    parent->children[1] = rightN;
    leftN->parent = parent;
    rightN->parent = parent;

    if (node->isRoot) {
        changeRoot(parent, tree); 
    }

    freeNode(node);
}


void changeRoot(Node *node, BTree *tree) {
    tree->root = node;
    printf("Noeud racine changé à %d\n", tree->root->keys[0]);
}

void printNode(Node *node) {
    for (uint8_t i = 0; i < node->numKeys; i++) { 
        printf("[%d] ", node->keys[i]); 
    }
    printf(" (%d)", node->numKeys);
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

    for (uint8_t i = 0; i <= node->numKeys; i++) { 
        if (node->children[i] != NULL) {
            printTreeHelper(node->children[i]);
        }
    }

    printNode(node);
}
