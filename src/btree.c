#include "btree.h"

Node *createNode(uint64_t values[MAX_KEYS], Bool isRoot) {
    Node *node = (Node*)malloc(sizeof(*node));
    if (node == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique\n");
        exit(EXIT_FAILURE);
    }
    
    node->numKeys = 0;
    memset(node->keys, 0, sizeof(node->keys)); 

    for (uint8_t i = 0; i < MAX_KEYS; i++) {
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

void insertKey(Node *node, uint64_t value) {
    if (node->numKeys == MAX_KEYS) {
        printf("Erreur : Le noeud est plein, impossible d'insérer la clé %ld.\n", value);
        return;
    }
    int8_t i;  
    for(i = node->numKeys - 1; i >= 0 && node->keys[i] > value; i--) {
        node->keys[i+1] = node->keys[i];
    }

    node->keys[i+1] = value;
    node->numKeys++;
}

void printNode(Node *node) {
    for(uint8_t i = 0; i < MAX_KEYS; i++) {
        if(node->keys[i] != 0) {
            printf("[%d] ", node->keys[i]);
        }
    }
    printf(" %d clés", node->numKeys);
    printf("\n");
}