#ifndef __BTREE_H__
#define __BTREE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_KEYS 4
#define MIN_KEYS 2

typedef enum  {
    FALSE,
    TRUE,
} Bool;
typedef struct Node {
    uint64_t keys[MAX_KEYS + 1];
    uint8_t numKeys;
    struct Node **children;
    struct Node *parent;
    Bool isRoot;
    struct Row *rows[MAX_KEYS + 1];
} Node;

typedef struct BTree {
    struct Node *root;
} BTree;

typedef struct Row {
    uint64_t id;
    char name[50];
} Row;


Node *createNode(uint64_t values[], uint8_t numValues, Bool isRoot, Row *rows[]);
BTree *createTree(Node *root);
void freeNode(Node *node);
void freeBTree(BTree *tree);
void insertKey(uint64_t value, BTree *tree, char name[50]);
void printNode(Node *node, Bool withRows);
void splitNode(Node *node, BTree *tree);
void changeRoot(Node *node, BTree *tree);
void printTree(BTree *tree, Bool withRows);
void printTreeHelper(Node *node, Bool withRows);
void insertKeyOnNode(uint64_t value, Node *node, Row *row);
uint8_t findChildPosition(Node *parent, Node *child);
void insertTest(uint8_t max, BTree *tr, char name[50]);
uint64_t getKey(BTree *tr, uint64_t value);
void deleteKey(BTree *tr, uint64_t value, Bool debug);
void rebalanceNode(Node *node, BTree *tr);
void shiftKeys(Node *node, Node *parent, Node *sibling, int8_t index, Bool rightDirection);
Bool keyExists(BTree *tr, uint64_t value);
void mergeNodes(Node *node, Node *parent, Node *sibling, uint8_t parentIndex, Bool rightDirection, BTree *tr);
Row *cloneRow(Row* original);
void pushToRoot(Node *node, Node *root, int8_t index);
#endif