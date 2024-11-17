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


/**
 * Insértion d'une valeur pour la colonne name dans la base
 * @param name Nom à insérer
 * @param tr Arbre dans lequel insérer la valeur
 */
void insertData(char name[50], BTree *tr) {
    Row *row = malloc(sizeof(Row));
    if (row == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        exit(EXIT_FAILURE);
    }
    strcpy(row->name, name);

    Row *rows[1];
    rows[0] = row;

    tr->totalId++;

    if(tr->root != NULL) {
        insertKey(tr->totalId, tr, name);
        free(row);
    } else {
        uint64_t values[] = {tr->totalId};
        Node *node = createNode(values, 1, TRUE, rows);
        tr->root = node;
    }

    return;
}

/**
 * Sélection de toutes les données dans la bdd
 */
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
                printf("ID : [%ld] Name : '%s'\n", node->rows[i]->id, node->rows[i]->name);
                globalCounter++;
            }
        }

        if (node->children[node->numKeys] != NULL) {
            selectAll(node->children[node->numKeys]);
        }
    } else { 
        for (uint8_t i = 0; i < node->numKeys; i++) {
            if (node->rows[i] != NULL) {
                printf("ID : [%ld] Name : '%s'\n", node->rows[i]->id, node->rows[i]->name);
                globalCounter++;
            }
        }
    }
}

/**
 * Préparation du compteur de résultats et lancement de l'instruction
 * @param root Racine de l'arbre
 */
void initSelectAll(Node *root) {
    globalCounter= 0;
    selectAll(root);
    printf("%ld résultat%s\n", globalCounter, globalCounter == 1 ? "" : "s");
}


/**
 * Recherche dichotomique d'une valeur spécifique dans toute la base (récursif)
 * @param node Noeud dans lequel rechercher la valeur
 * @param value Valeur à rechercher
 * @param isID Détermine si l'Id doit être affiché
 * @param isName Détermine si le nom doit être affiché
 * @param idSearch Détemrine si la recherche se fait sur l'id ou le nom
 * @param idValue Valeur de l'id si la recherche se fait par celui-ci
 * 
 */
void selectRow(Node *node, char *value, Bool isID, Bool isName, Bool idSearch, uint64_t idValue) {
    if (node == NULL) {
         return;
    }

    int16_t left = 0;
    int16_t right = node->numKeys - 1;
    Bool match = FALSE;

    while (left <= right) {
        int16_t mid = (left + right) / 2;

        if ((idSearch && node->rows[mid]->id == idValue) || (!idSearch && strcmp(node->rows[mid]->name, value) == 0)) {
            match = TRUE;
            if (isID) {
                printf("ID : [%ld] ", node->rows[mid]->id);
            }
            if (isName) {
                printf("Name : '%s' ", node->rows[mid]->name);
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

/**
 * Parsing pour préparer le lancement de la recherche
 * @param root noeud racine
 * @param value valeur à trouver
 * @param columns Colonnes à rechercher
 */
void initSelect(Node *root, char *value, char columns[]) {
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

/**
 * Supression d'une ligne dans la base
 * @param node Noeud dans lequel rechercher la valeur à supprimer
 * @param value Valeur à supprimer
 * @param idSearch Détemrine si la recherche se fait sur l'id ou le nom
 * @param idValue Valeur de l'id si la recherche se fait par celui-ci
 * @return void
 */
void deleteRow(BTree *tr, Node *node, char *value, Bool idSearch, uint64_t idValue) {
    if (node == NULL) {
        return;
    }
    int16_t left = 0;
    int16_t right = node->numKeys - 1;
    Bool match = FALSE;

    while (left <= right) {
        int16_t mid = (left + right) / 2;

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



/**
 * Parsing de l'instruction avant lancement de l'opération
 * @param tr 
 * @param value valeur à supprimer
 * @return voids
 */
void initDelete(BTree *tr, char *value) {
    char *ptr;
    uint64_t idValue = strtol(value, &ptr, 10);
    Bool idSearch = (*ptr == '\0');

    deleteRow(tr, tr->root, value, idSearch, idValue);
}

