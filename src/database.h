#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

uint64_t getNextId(Node *node);
void insertData(char *name, BTree *tr);
void selectAll(Node *node);

#endif