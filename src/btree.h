#ifndef __BTREE_H__
#define __BTREE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_KEYS 4
#define MIN_KEYS 2

typedef enum  {
    FALSE,
    TRUE,
} Bool;
typedef struct Node {
    uint8_t keys[4];
    uint8_t numKeys;
    struct Node **children;
    struct Node *parent;
    Bool isRoot;
} Node;

typedef struct BTree {
    struct Node *root;
} BTree;


Node *createNode(uint64_t values[MAX_KEYS], Bool isRoot);
BTree *createTree(Node *root);
void freeNode(Node *node);
void freeBTree(BTree *tree);
void insertKey(Node *node, uint64_t value);
void printNode(Node *node);

#endif