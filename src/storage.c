#include "storage.h"

#define DEFAULT_FILE "database.txt"


void saveRow(FILE *file, Row *row) {
    if (row == NULL) {
        return; 
    }
    fprintf(file, "%lu;%s\n", row->id, row->name);
}


void saveNode(FILE *file, Node *node) {
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

BTree* loadBTree() {
    FILE *file = fopen(DEFAULT_FILE, "r");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        return NULL;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    uint64_t totalId = atoi(buffer); 

    fgets(buffer, sizeof(buffer), file);
    uint64_t value = atoi(buffer);

    Row *row = malloc(sizeof(Row));
    if (row == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        fclose(file);
        return NULL;
    }
    strcpy(row->name, "Test");

    Row *rows[1];
    rows[0] = row;

    uint64_t values[] = {value};
    Node *root = createNode(values, 1, TRUE, rows);
    BTree *tree = createTree(root);
    tree->totalId = totalId;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *token = strtok(buffer, ";");
        if (token != NULL) {
            uint64_t value = atoi(token); 
            token = strtok(NULL, "\n");
            if (token != NULL) {
                insertKey(value, tree, token);
            }
        }
    }

    fclose(file);
    return tree;
}

