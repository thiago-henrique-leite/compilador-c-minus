#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINES 100
#define MAX_FUNCTIONS 100
#define MAX_TOKEN_LENGTH 100
#define MAX_REGISTERS 26
#define MAX_CHILD_NODES 3
#define HASHTABLE_LENGTH 211
#define TAB_SIZE 4

#ifndef YYPARSER
#include "parse.tab.h"
#endif

extern FILE* input_file;
static char* _var_scope = "global";
extern char* yytext;
extern int yylineno;

int line_number;

bool error;

extern int registers[MAX_REGISTERS];

extern char id_token[MAX_TOKEN_LENGTH];
extern char num_token[MAX_TOKEN_LENGTH];
extern char str_token[MAX_TOKEN_LENGTH];

static bool hasMainFunc = false;

typedef int token_t;

typedef enum {decl_t, exp_t, stmt_t} NodeType;
typedef enum {decl_func, decl_type, decl_var} DeclType;
typedef enum {stm_atrib, stm_call, stm_if, stm_return, stm_while} StmType;
typedef enum {exp_id, exp_num, exp_op} ExpType;
typedef enum {Array, Boolean, Integer, Void} PrimitiveType;

int already_declared_func(char *name);
int already_declared_var(char *name);
int already_declared_var_in_scope(char *name, char *scope);
int already_declared_global_var(char *name);

token_t getToken();

void print_tokens(char *fileName, FILE *fout);

void file_write(FILE *fout, char *str);

char *string_copy(char *str);
char *get_token_name(token_t token);
char *get_string_type(PrimitiveType t);


/******************** Data Structure ********************/

/* Stack */

typedef struct name_s { struct name_s *next; char *name; } name_t;
typedef struct { name_t* top; int size; } stack_t;

void  stack_init(stack_t *stack);
void  stack_push(stack_t *stack, char *name);
char *stack_pop(stack_t *stack);

/* List */

typedef struct { char *op, *c1, *c2, *c3; } instruction_t;
typedef struct instruction_s { instruction_t *instruction; struct instruction_s *next; } instruction_node_t;
typedef struct{ instruction_node_t* first; } instruction_list_t;

extern instruction_list_t intermediate_code;

void list_init(instruction_list_t *list);
void list_insert(instruction_list_t* list, char *op, char *c1, char *c2, char *c3);
void list_print(instruction_list_t *list, FILE *fout);

/* Tree */

typedef struct treeNode {
    struct treeNode *childs[MAX_CHILD_NODES];
    struct treeNode *sibling;

    int line_number;
    
    NodeType node_type;

    union {
        char *name;
        token_t op;
        int value;
    } attributes;

    union { 
        DeclType decl; 
        ExpType exp;
        StmType stmt; 
    } type;

    PrimitiveType p_type;
} TreeNode;

TreeNode *parse(void);
TreeNode *tree_init_node(NodeType node_type);
TreeNode *tree_create_decl_node(DeclType type);
TreeNode *tree_create_stmt_node(StmType type);
TreeNode *tree_create_exp_node(ExpType type);

TreeNode* syntax_tree;

void tree_print(TreeNode *tree, FILE *fout);

/********************************************************/

/* Lexical Analysis */

void make_lexical_analysis(char *input_file_name, FILE *output_file);

/* Syntax Tree */

void make_syntax_tree(char *input_file_name, FILE *output_file);

/* Symbol Table */

void symbol_table_insert(char *name, int length, int line_number, char *scope, PrimitiveType type, int isFunction);
void symbol_table_print(FILE *fout);
void symbol_table_build(TreeNode *tree, FILE *fout); 

void make_symbol_table(FILE *output_file);

/* Semantic Analysis */

void make_node_types_verify(FILE *out_file);

/* Intermediary Code */

int register_use();

void get_func_type(char *name, PrimitiveType *p_type);
void get_var_type(char* name, char* _var_scope, PrimitiveType* p_type);

void verify_node_types(TreeNode * tree, FILE *out);
void generate_intermediate_code(TreeNode * tree);

void make_intermediate_code(FILE *output_file);
