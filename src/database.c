#include "btree.h"
#include "database.h"

uint64_t getNextId(Node *node) {
    uint64_t max = 0;

    while (node->children != NULL && node->numKeys > 0) {
        node = node->children[node->numKeys];  
    }

    if (node->numKeys > 0) {
        max = node->keys[node->numKeys - 1];
    }

    return max + 1;
}



void insertData(char name[50], BTree *tr) {
    Row *row = malloc(sizeof(Row));
    if (row == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        exit(EXIT_FAILURE);
    }
    strcpy(row->name, name);

    Row *rows[1];
    rows[0] = row;
    uint64_t nextId = getNextId(tr->root);
    insertKey(nextId, tr, name);
    free(row);
    return;
}

void selectAll(Node *node) {
    if (node == NULL) {
        return; 
    }

    if (node->children != NULL) {
        for (uint8_t i = 0; i < node->numKeys; i++) {
            if (node->children[i] != NULL) {
                selectAll(node->children[i]);
            }

            if (node->rows[i] != NULL) {
                printf("ID : [%ld] Nom : '%s'\n", node->rows[i]->id, node->rows[i]->name);
            }
        }

        if (node->children[node->numKeys] != NULL) {
            selectAll(node->children[node->numKeys]);
        }
    } else {
        for (uint8_t i = 0; i < node->numKeys; i++) {
            if (node->rows[i] != NULL) {
                printf("ID : [%ld] Nom : '%s'\n", node->rows[i]->id, node->rows[i]->name);
            }
        }
    }
}

