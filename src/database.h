#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdint.h>
#include "btree.h"

typedef struct Row {
    uint64_t id;
    char name[100];
} Row;

typedef struct Table {
    struct BTree *tree;
    char name;
} Table;

void insertRow(Table *table, Row row);
Table *createTable(char name[50]);

#endif