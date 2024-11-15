#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include <ctype.h>

uint64_t getNextId(Node *node);
void insertData(char *name, BTree *tr);
void selectAll(Node *node);
void selectRow(Node *node, char *value, Bool isID, Bool isName, Bool idSearch, uint64_t idValue);
void initSelect(Node *root, char *value, char *columnName, char columns[]);
void deleteRow(BTree *tr, Node *node, char *value, Bool idSearch, uint64_t idValue);
void initDelete(BTree *tr, char *value);

#endif