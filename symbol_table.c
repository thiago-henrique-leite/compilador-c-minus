#include "symbol_table.h"

static int hash_table( char * key ) { 
    int i = 0, tmp = 0;
    
    while(key[i] != '\0') { 
        tmp = ((tmp << TAB_SIZE) + key[i]) % HASHTABLE_LENGTH; i++;
    }
    
    return tmp;
}

void symbol_table_insert(char *name, int size, int line_number, char *_var_scope, PrimitiveType type, int is_function) {
    int hash_key = hash_table(name);

    symbol_table_block_t hash_value = hashtable[hash_key];
    
    while(hash_value != NULL && (strcmp(name, hash_value->name) != 0 || strcmp(hash_value->_var_scope, _var_scope) != 0 ))
        hash_value = hash_value->next;

    if(hash_value == NULL) {
        hash_value = (symbol_table_block_t) malloc(sizeof(struct symbol_table_block_s));
        
        hash_value->name = name;
        hash_value->size = size;
        hash_value->lines = (line_list_t) malloc(sizeof(struct line_list_s));
        hash_value->lines->line_number = line_number;

        if(!is_function) hash_value->id = ++vars_count;

        if(strcmp(_var_scope, "global") == 0) {
            char* scope_global = malloc(sizeof(char) * 8);

            strcat(scope_global, "global");

            hash_value->_var_scope = scope_global;
        } else {
            hash_value->_var_scope = _var_scope;
        }

        hash_value->block_type = is_function;
        hash_value->lines->next = NULL;
        hash_value->type = type;
        hash_value->next = hashtable[hash_key];
        
        hashtable[hash_key] = hash_value; 
    } else { 
        line_list_t hash_lines = hash_value->lines;
        
        while (hash_lines->next != NULL)
            hash_lines = hash_lines->next;
        
        hash_lines->next = (line_list_t) malloc(sizeof(struct line_list_s));

        hash_lines->next->next = NULL;
        hash_lines->next->line_number = line_number;
    }
}

int already_declared_func(char *name) {
    int hash_key = hash_table(name);

    symbol_table_block_t hash_value = hashtable[hash_key];

    while(hash_value != NULL && (strcmp(name,hash_value->name) != 0 || hash_value->block_type == 0))
        hash_value = hash_value->next;
    
    return (hash_value == NULL) ? 0 : 1;
}

int already_declared_var(char *name) {
    int hash_key = hash_table(name);
    
    symbol_table_block_t hash_value = hashtable[hash_key];
    
    while(hash_value != NULL && (strcmp(name, hash_value->name) != 0 || hash_value->block_type == 1))
        hash_value = hash_value->next;
    
    return (hash_value == NULL) ? 0 : 1;
}

int already_declared_var_in_scope(char *name, char *_var_scope) {
    int hash_key = hash_table(name);

    symbol_table_block_t hash_value = hashtable[hash_key];
    
    while(hash_value != NULL) {
        if((hash_value->block_type == 0 && strcmp(name, hash_value->name) == 0) && strcmp(_var_scope, hash_value->_var_scope) == 0)
            break;

        hash_value = hash_value->next;
    }

    return (hash_value == NULL) ? 0 : 1;
}

int already_declared_global_var(char *name) {
    int hash_key = hash_table(name);

    symbol_table_block_t hash_value = hashtable[hash_key];
    
    while(hash_value != NULL) {
        if((hash_value->block_type == 0 && strcmp(name, hash_value->name) == 0) && strcmp(hash_value->_var_scope,"global") == 0)
            break;

        hash_value = hash_value->next;
    }

    return (hash_value == NULL) ? 0 : 1;
}

void get_func_type(char *name, PrimitiveType *p_type) {
    int hash_key = hash_table(name);
    
    symbol_table_block_t hash_value =  hashtable[hash_key];
    
    while((hash_value != NULL) && ((strcmp(name,hash_value->name) != 0)||(hash_value->block_type == 0)))
        hash_value = hash_value->next;

    if(hash_value != NULL) *p_type = hash_value->type;
}

void get_var_type(char *name, char *_var_scope, PrimitiveType *p_type) {
    int hash_key = hash_table(name);
    
    symbol_table_block_t hash_value = hashtable[hash_key];
    
    while((hash_value != NULL) && ((strcmp(name,hash_value->name) != 0)|| (strcmp(_var_scope,hash_value->_var_scope) != 0) ||(hash_value->block_type == 1)))
        hash_value = hash_value->next;

    if(hash_value != NULL) *p_type = hash_value->type;
}

void symbol_table_print(FILE * fout) {
    file_write(fout," |   Nome   | Id   |   PrimitiveType |   Escopo  |  Tamanho |  Referenciado nas Linhas  \n");
    file_write(fout," |------------------------------------------------------------------------------------\n");

    for(int i = 0; i < HASHTABLE_LENGTH; i++) { 
        if(hashtable[i] != NULL) { 
            symbol_table_block_t hash_value = hashtable[i];

            while(hash_value != NULL) { 
                line_list_t hash_lines = hash_value->lines;

                fprintf(fout, " |  %-6s ", hash_value->name);

                if(hash_value->id < 10)
                    fprintf(fout, " |  0%-1d ", hash_value->id);
                else
                    fprintf(fout, " |  %-1d ", hash_value->id);
        
                fprintf(fout, " |  %-13s ", get_string_type(hash_value->type));
                fprintf(fout, " |  %-8s", hash_value->_var_scope);
                fprintf(fout, " |  %-8d|", hash_value->size);
                
                while(hash_lines != NULL) { 
                    fprintf(fout, "%4d ", hash_lines->line_number);
                    
                    hash_lines = hash_lines->next;
                }
                
                file_write(fout,"\n");
                
                hash_value = hash_value->next;
            }
        }
    }

    file_write(fout," |-----------------------------------------------------------------------------------\n");
} 
