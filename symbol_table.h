#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "globals.h"

typedef struct line_list_s { 
    struct line_list_s *next;
    int line_number;
} *line_list_t;

typedef struct symbol_table_block_s { 
    char *name, *_var_scope;
    int id, size, block_type; 
    struct symbol_table_block_s* next;
    line_list_t lines;
    PrimitiveType type; 
} *symbol_table_block_t;

static symbol_table_block_t hashtable[HASHTABLE_LENGTH];

static int vars_count = 0;

static int hash_table(char *key);

int already_declared_func(char *name);
int already_declared_var(char *name);
int already_declared_var_in_scope(char *name, char *_var_scope);
int already_declared_global_var(char *name);

void symbol_table_insert(char *name, int size, int line_number, char *_var_scope, PrimitiveType type, int is_function);
void symbol_table_print(FILE *fout);

#endif
