#ifndef ANALYZE_H
#define ANALYZE_H

#include "globals.h"

static void tree_insert_node(TreeNode *t);
static void tree_post_order(TreeNode *tree);
static void tree_pre_order(TreeNode *tree);
static void verify_node(TreeNode *t);
void verify_node_types(TreeNode * tree, FILE *fout);
void symbol_table_build(TreeNode *tree, FILE *fout);
static void show_error(TreeNode *t, char *message);

#endif
