#include <stdio.h>
#include <stdlib.h>
#include "repl.h"
#include <stdbool.h>
#include <string.h>
#include "btree.h"


int main(int argc, char* argv[], char* envp[]){
  
  uint64_t values[] = {10};
  Node *root = createNode(values, 1, TRUE);
  BTree *tr =  createTree(root);

  printf("Création de l'arbre avec comme valeur : %d\n", tr->root->keys[0]);
  insertKey(root, 15, tr);
  insertKey(root, 9, tr);
  insertKey(root, 20, tr);
  insertKey(root, 7, tr);
  printTree(tr);
  freeBTree(tr);
  return 0;
  
}
