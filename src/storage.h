#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"


void saveRow(FILE *file, Row *row);
void saveNode(FILE *file, Node *node);
void saveBTree(BTree *tree);
Row* loadRow(FILE *file);
Node* loadNode(FILE *file);
BTree* loadBTree();

#endif
