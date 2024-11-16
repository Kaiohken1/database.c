#include <stdio.h>
#include <stdlib.h>
#include "repl.h"
#include <stdbool.h>
#include <string.h>
#include "btree.h"
#include "database.h"


int main(int argc, char* argv[], char* envp[]){

  // uint64_t values[] = {1};
  // Row *row = malloc(sizeof(Row));
  //   if (row == NULL) {
  //       fprintf(stderr, "Erreur d'allocation\n");
  //       return 1;
  //   }
  // strcpy(row->name, "aa");

  // Row *rows[1];
  // rows[0] = row;

  // Node *root = createNode(values, 1, TRUE, rows);
  // BTree *tr =  createTree(root);

  // // printf("Création de l'arbre avec comme valeur : %ld\n", tr->root->keys[0]);
  // // insertKey(7, tr, row->name);
  // // insertKey(8, tr, row->name);
  // // insertKey(9, tr, row->name);
  // // // insertKey(12, tr, row->name);
  // // insertKey(13, tr, row->name);
  // // insertKey(14, tr, row->name);
  // // // insertKey(16, tr, row->name);
  // // insertKey(18, tr, row->name);
  // // insertKey(15, tr, row->name);
  // // insertKey(10, tr, row->name);


  // insertTest(20, tr, row->name);
  // printTree(tr, TRUE);
  // deleteKey(tr, 9, TRUE);
  // // deleteKey(tr, 1, TRUE);
  // // insertKey(8, tr, row->name);
  // // insertKey(10, tr, row->name);
  // // insertKey(11, tr, row->name);
  // // printTree(tr, TRUE);

  // // deleteKey(tr, 15, TRUE);
  // // deleteKey(tr, 18, TRUE);
  // // insertKey(11, tr, row->name);
  // // deleteKey(tr, 7, TRUE);
  // // deleteKey(tr, 11, TRUE);
  // // // uint64_t id = getNextId(tr->root);
  // // // printf("Next id : %ld\n", id);
  // freeBTree(tr);

  repl();


  return 0;
}
