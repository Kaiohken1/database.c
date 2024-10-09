#include "database.h"

void insertRow(Table *table, Row *row) {
    if(table->tree == NULL) {
        uint64_t values[] = {row->id};
        Node *root = createNode(values, 1, TRUE, row);
        BTree *tr =  createTree(root);
        table->tree = tr;
    } else {
        insertKey(row->id, table->tree, row);
    }
}

Table *createTable(char name[50]) {
    Table *table = (Table*)malloc(sizeof(*table));
    if (table == NULL) {
        fprintf(stderr, "Erreur : Problème lors de l'allocation dynamique pour la table\n");
        exit(EXIT_FAILURE);
    }
    table->name = name;
    table->tree = NULL;
    return table;
}