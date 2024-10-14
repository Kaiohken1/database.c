#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <stdint.h>

typedef struct Row {
    uint64_t id;
    char name[50];
} Row;

void insertData(char name[50]);

#endif