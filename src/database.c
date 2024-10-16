#include "btree.h"
#include "database.h"

uint64_t getNextId(Node *node) {
    uint64_t max = 0;

    if (node->children != NULL) {
        while (node->children[node->numKeys] != NULL) {
            node = node->children[node->numKeys];
        }
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
        return exit(EXIT_FAILURE);
    }
    strcpy(row->name, name);

    Row *rows[1];
    rows[0] = row;
    uint64_t nextId = getNextId(tr->root);
    insertKey(nextId, tr, name);
    return;
}