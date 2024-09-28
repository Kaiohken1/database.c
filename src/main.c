#include <stdio.h>
#include <stdlib.h>
#include "repl.h"
#include <stdbool.h>
#include <string.h>
#include "btree.h"


int main(int argc, char* argv[], char* envp[]){
  
  // repl();
  Node *root = createNode(10);
  BTree *tr =  createTree(root);

  printf("Création de l'arbre avec comme valeur : %d\n", tr->root->keys[0]);
  free(tr);

  return 0;
  
}
