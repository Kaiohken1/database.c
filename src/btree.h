#ifndef __BTREE_H__
#define __BTREE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_KEYS 4

typedef struct Node {
    uint8_t keys[4];
    uint8_t numKeys;
    struct Node **children;
    struct Node *parent;
} Node;

typedef struct BTree {
    struct Node *root;
} BTree;


Node *createNode(uint8_t value);
BTree *createTree(Node *root);
void freeNode(Node *node);
void freeBTree(BTree *tree);

#endif