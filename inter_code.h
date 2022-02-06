#ifndef INTER_CODE_H
#define INTER_CODE_H

#include "globals.h"

static int arg = 0;
static int n_label = 0;
static int n_tmp = 0;
static int list_args_length = 0;

static char* scope = "global";
static char* list_args[496];

static bool stop = false;

static stack_t inter_code_stack;

instruction_list_t intermediate_code;

void generate_intermediate_code(TreeNode * tree);
static void generate_code(TreeNode *tree);
static void stmt_node(TreeNode *tree);
static void decl_node(TreeNode *tree);
static void exp_node( TreeNode * tree);
char* allocate_register(int num);
char* convert_number_to_str(int num);
char* label_num(int num);
void finish_inter_code();

#endif
