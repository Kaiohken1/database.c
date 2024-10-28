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

    if(tr->root != NULL) {
        uint64_t nextId = getNextId(tr->root);
        insertKey(nextId, tr, name);
        free(row);
    } else {
        uint64_t values[] = {1};
        Node *node = createNode(values, 1, TRUE, rows);
        tr->root = node;
    }

    return;
}

void selectAll(Node *node) {
    uint64_t counter = 0;
    if (node == NULL) {
        printf("%ld résultat%s\n", counter, counter == 1 ? "" : "s");
        return; 
    }

    if (node->children != NULL) {
        for (uint8_t i = 0; i < node->numKeys; i++) {
            if (node->children[i] != NULL) {
                selectAll(node->children[i]);
            }

            if (node->rows[i] != NULL) {
                printf("ID : [%ld] Nom : '%s'\n", node->rows[i]->id, node->rows[i]->name);
                counter++;
            }
        }

        if (node->children[node->numKeys] != NULL) {
            selectAll(node->children[node->numKeys]);
        }
    } else {
        for (uint8_t i = 0; i < node->numKeys; i++) {
            if (node->rows[i] != NULL) {
                printf("ID : [%ld] Nom : '%s'\n", node->rows[i]->id, node->rows[i]->name);
                counter++;
            }
        }
    }
    printf("%ld résultat%s\n", counter, counter == 1 ? "" : "s");
}

void selectRow(Node *node, char *value, char columns[]) {
    uint64_t counter = 0;
    if (node == NULL) {
        printf("%ld résultat%s\n", counter, counter == 1 ? "" : "s");
        return; 
    }
    Bool isID = FALSE;
    Bool isName = FALSE;
    char *parser = strtok(columns, " ,");
    while (parser != NULL) {
        if (strcmp(parser, "id") == 0) {
            isID = TRUE;
        } else if (strcmp(parser, "name") == 0) {
            isName = TRUE;
        }
        parser = strtok(NULL, " ,");
    }

    char *ptr;
    uint64_t idValue = strtol(value, &ptr, 10);
    Bool idSearch = (*ptr == '\0');

    if (node->children != NULL) {
        for (uint8_t i = 0; i < node->numKeys; i++) {
            if (node->children[i] != NULL) {
                selectRow(node->children[i], value, columns);
            }

            if (node->rows[i] != NULL) {
                if ((idSearch && node->rows[i]->id == idValue) ||
                    (!idSearch && strcmp(node->rows[i]->name, value) == 0)) {
                    if (isID) {
                        printf("ID : [%ld] ", node->rows[i]->id);
                    }
                    if (isName) {
                        printf("Nom : '%s' ", node->rows[i]->name);
                    }
                    printf("\n");
                    counter++;
                }
            }
        }
        if (node->children[node->numKeys] != NULL) {
            selectRow(node->children[node->numKeys], value, columns);
        }
    } else {
        for (uint8_t i = 0; i < node->numKeys; i++) {
            if (node->rows[i] != NULL) {
                if ((idSearch && node->rows[i]->id == idValue) ||
                    (!idSearch && strcmp(node->rows[i]->name, value) == 0)) {
                    if (isID) {
                        printf("ID : [%ld] ", node->rows[i]->id);
                    }
                    if (isName) {
                        printf("Nom : '%s' ", node->rows[i]->name);
                    }
                    printf("\n");
                    counter++;
                }
            }
        }
    }
    printf("%ld résultat%s\n", counter, counter == 1 ? "" : "s");
}
