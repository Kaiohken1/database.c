#include "btree.h"

Node *createNode(uint64_t value) {
    Node *node = (Node*)malloc(sizeof(*node));

    if(node == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique\n");
        exit(EXIT_FAILURE);
    }
    node->keys[0] = value;
    node->children = (Node**)malloc(MAX_KEYS * sizeof(Node*));

    if (node->children == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique pour les enfants\n");
        free(node);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_KEYS; i++) {
        node->children[i] = NULL;
    }

    node->parent = NULL;
    node->numKeys = 1;

    return node;
};

BTree *createTree(Node *root) {
    BTree *tree = (BTree*)malloc(sizeof(*tree));
    if (tree == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique pour l'arbre\n");
        exit(EXIT_FAILURE);
    }
    tree->root = root ? root : NULL;
    return tree;
}

void freeNode(Node *node) {
    if(node != NULL) {
        for(int i = 0; i < MAX_KEYS; i++) {
            freeNode(node->children[i]);
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