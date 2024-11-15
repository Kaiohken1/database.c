#include "btree.h"
#include "database.h"

uint64_t globalCounter = 0;

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

//Recherche dichotomique
void selectRow(Node *node, char *value, Bool isID, Bool isName, Bool idSearch, uint64_t idValue) {
    if (node == NULL) {
         return;
    }

    uint16_t left = 0;
    uint16_t right = node->numKeys - 1;
    Bool match = FALSE;

    while (left <= right) {
        uint16_t mid = (left + right) / 2;

        if ((idSearch && node->rows[mid]->id == idValue) || (!idSearch && strcmp(node->rows[mid]->name, value) == 0)) {
            match = TRUE;
            if (isID) {
                printf("ID : [%ld] ", node->rows[mid]->id);
            }
            if (isName) {
                printf("Nom : '%s' ", node->rows[mid]->name);
            }
            printf("\n");

            globalCounter++;
            break;
        }

        if ((idSearch && idValue < node->rows[mid]->id) ||
            (!idSearch && strcmp(value, node->rows[mid]->name) < 0)) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    if (node->children) {
        if (!match && left < node->numKeys) {
            selectRow(node->children[left], value, isID, isName, idSearch, idValue);
        } else {
            if (left > 0) {
                selectRow(node->children[left - 1], value, isID, isName, idSearch, idValue);
            }
            if (left < node->numKeys) {
                selectRow(node->children[left + 1], value, isID, isName, idSearch, idValue);
            }
        }
    }
}

void initSelect(Node *root, char *value, char *columnName, char columns[]) {
    Bool isID = FALSE, isName = FALSE;
    char *parser = strtok(columns, " ,");
    while (parser != NULL) {
        if (strcmp(parser, "id") == 0) isID = TRUE;
        else if (strcmp(parser, "name") == 0) isName = TRUE;
        parser = strtok(NULL, " ,");
    }

    char *ptr;
    uint64_t idValue = strtol(value, &ptr, 10);
    Bool idSearch = (*ptr == '\0');

    globalCounter = 0;
    selectRow(root, value, isID, isName, idSearch, idValue);
    printf("%ld résultat%s\n", globalCounter, globalCounter == 1 ? "" : "s");
}

void deleteRow(BTree *tr, Node *node, char *value, Bool idSearch, uint64_t idValue) {
    if (node == NULL) {
        return;
    }

    uint16_t left = 0;
    uint16_t right = node->numKeys - 1;
    Bool match = FALSE;

    while (left <= right) {
        uint16_t mid = (left + right) / 2;

        if ((idSearch && node->rows[mid]->id == idValue) || (!idSearch && strcmp(node->rows[mid]->name, value) == 0)) {
            match = TRUE;

            deleteKey(tr, node->rows[mid]->id, FALSE);
            return;
        }

        if ((idSearch && idValue < node->rows[mid]->id) ||
            (!idSearch && strcmp(value, node->rows[mid]->name) < 0)) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    if (node->children) {
        if (left <= node->numKeys) {
            deleteRow(tr, node->children[left], value, idSearch, idValue);
        }
    }
}



void initDelete(BTree *tr, char *value) {
    char *ptr;
    uint64_t idValue = strtol(value, &ptr, 10);
    Bool idSearch = (*ptr == '\0');

    deleteRow(tr, tr->root, value, idSearch, idValue);
}

