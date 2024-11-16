#include "storage.h"

#define DEFAULT_FILE "database.txt"


void saveRow(FILE *file, Row *row) {
    if (row == NULL) {
        return; 
    }
    fprintf(file, "%lu|%s\n", row->id, row->name);
}


void saveNode(FILE *file, Node *node) {
    for (int i = 0; i < node->numKeys; i++) {
        fprintf(file, "%lu|", node->keys[i]);
    }
    fprintf(file, "\n");
    
    for (int i = 0; i < node->numKeys; i++) {
        saveRow(file, node->rows[i]);
    }
    
    if (node->children != NULL) {
        for (int i = 0; i <= node->numKeys; i++) {
            saveNode(file, node->children[i]);
        }
    }
}

void saveBTree(BTree *tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("Arbre non initialisé.\n");
        return;
    }

    FILE *file = fopen(DEFAULT_FILE, "w");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier");
        return;
    }

    fprintf(file, "%lu\n", tree->totalId);

    saveNode(file, tree->root);

    fclose(file);
}
