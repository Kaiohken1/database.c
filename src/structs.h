#ifndef __STRUCTS__
#define __STRUCTS__
#include <stdint.h>

typedef struct Table{
    char name;
};

enum DataType{
    INT,
    CHAR,
    FLOAT,
};

typedef struct Column {
    char name;
    DataType *type;
};

typedef struct TableData {
    uint8_t index;
    //Trouver comment ajouter la donnée
    TableData *next;
};


#endif