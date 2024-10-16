#include <stdio.h>
#include <stdlib.h>
#include "repl.h"
#include <stdbool.h>
#include <string.h>
#include "btree.h"
#include "database.h"


int main(int argc, char* argv[], char* envp[]){

  uint64_t values[] = {10};
  Row *row = malloc(sizeof(Row));
    if (row == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return 1;
    }
  strcpy(row->name, "aa");

  Row *rows[1];
  rows[0] = row;

  Node *root = createNode(values, 1, TRUE, rows);
  BTree *tr =  createTree(root);

  printf("Création de l'arbre avec comme valeur : %ld\n", tr->root->keys[0]);
  // insertKey(15, tr);
  // insertKey(9, tr);
  // insertKey(20, tr);
  // insertKey(7, tr);
  // insertKey(39, tr);
  // insertKey(18, tr);
  // insertKey(95, tr);
  // insertKey(100, tr);
  // insertKey(25, tr);
  // insertKey(79, tr);
  // insertKey(19, tr);
  // insertKey(17, tr);
  // insertKey(16, tr);
  // insertKey(26, tr);
  // insertKey(28, tr);
  // insertKey(70, tr);

  insertTest(1, tr, row->name);
  insertData("Test", tr);
  printTree(tr);
  freeBTree(tr);
  return 0;
}
