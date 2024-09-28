#include <stdio.h>
#include <stdlib.h>
#include "repl.h"
#include <stdbool.h>
#include <string.h>
#include "btree.h"


int main(int argc, char* argv[], char* envp[]){
  
  uint64_t values[MAX_KEYS] = {10, 0, 0, 0};
  Node *root = createNode(values, TRUE);
  BTree *tr =  createTree(root);

  printf("Création de l'arbre avec comme valeur : %d\n", tr->root->keys[0]);
  insertKey(root, 15);
  insertKey(root, 9);
  insertKey(root, 20);
  insertKey(root, 7);
  printNode(root);
  freeBTree(tr);
  
  return 0;
  
}
