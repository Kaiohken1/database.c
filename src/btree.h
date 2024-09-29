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
    uint8_t keys[MAX_KEYS + 1];
    uint8_t numKeys;
    struct Node **children;
    struct Node *parent;
    Bool isRoot;
} Node;

typedef struct BTree {
    struct Node *root;
} BTree;


Node *createNode(uint64_t values[], uint8_t numValues, Bool isRoot);
BTree *createTree(Node *root);
void freeNode(Node *node);
void freeBTree(BTree *tree);
void insertKey(Node *node, uint64_t value, BTree *tree);
void printNode(Node *node);
void splitNode(Node *node, BTree *tree);
void changeRoot(Node *node, BTree *tree);
void printTree(BTree *tree);
void printTreeHelper(Node *node);

#endif