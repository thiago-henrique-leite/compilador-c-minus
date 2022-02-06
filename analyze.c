#include "analyze.h"

void symbol_table_build(TreeNode *tree, FILE *fout) { 
    tree_pre_order(tree);

    if(!hasMainFunc) {
        fprintf(stdout, "ERRO SEMÂNTICO: Função main não declarada.\n");

        error = true;
    } else {
        symbol_table_print(fout);
    }
}

static void tree_insert_node(TreeNode *t) { 
    PrimitiveType *p_type = (PrimitiveType*) malloc(sizeof(PrimitiveType));

    switch (t->node_type)
    {
        case decl_t:
            switch (t->type.decl)
            { 
                case decl_func:
                    if(strcmp(t->attributes.name, "main") == 0) hasMainFunc = true;

                    if(strcmp(t->attributes.name, "input")    == 0 || 
                       strcmp(t->attributes.name, "output")   == 0 || 
                       strcmp(t->attributes.name, "hd_read")  == 0 || 
                       strcmp(t->attributes.name, "hd_write") == 0
                    ) {
                        printf("ERRO SEMÂNTICO | LINHA %d: Função %s é reservada!\n", t->line_number, t->attributes.name);
                        
                        error = true;
                    } else {
                        if(already_declared_func(t->attributes.name) == 0) {
                            symbol_table_insert(t->attributes.name, 0, t->line_number, "global", t->p_type, 1);

                            _var_scope = t->attributes.name;
                        } else {
                            printf("ERRO SEMÂNTICO | LINHA %d: Função %s já declarada!\n", t->line_number, t->attributes.name);
                            
                            error = true;
                        }
                    }
                    break;
                case decl_var:
                    if(t->p_type == Void) {
                        show_error(t, "Variável não é do tipo <INT>.\n");
                        
                        error = true;
                    }

                    if (already_declared_var(t->attributes.name) == 0) {
                        if(t->childs[0] == NULL)
                            symbol_table_insert(t->attributes.name, 1, t->line_number, _var_scope, t->p_type, 0);
                        else
                            symbol_table_insert(t->attributes.name, t->childs[0]->attributes.value, t->line_number, _var_scope, t->p_type, 0);
                    } else {
                        if(strcmp(_var_scope, "global") != 0) {
                            if(already_declared_var_in_scope(t->attributes.name, _var_scope) == 0) {
                                if(t->childs[0] == NULL)
                                    symbol_table_insert(t->attributes.name, 1, t->line_number, _var_scope, t->p_type, 0);
                                else
                                    symbol_table_insert(t->attributes.name, t->childs[0]->attributes.value, t->line_number, _var_scope, t->p_type, 0);
                            } else {
                                printf("ERRO SEMÂNTICO | LINHA %d: Variável %s já declarada!\n", t->line_number, t->attributes.name);
                                
                                error = true;
                            }
                        } else {
                            printf("ERRO SEMÂNTICO | LINHA %d: Variável global %s já declarada!\n", t->line_number, t->attributes.name);
                            
                            error = true;
                        }
                    }
                    break;
                default: break;
            }
        break;
        case stmt_t:
            switch (t->type.stmt)
            { 
                case stm_call:
                    if(strcmp(t->attributes.name, "input") == 0) { t->p_type = Integer; } 
                    else if(strcmp(t->attributes.name, "output") == 0) { t->p_type = Integer; } 
                    else if(strcmp(t->attributes.name, "hd_read") == 0) { t->p_type = Integer; } 
                    else if(strcmp(t->attributes.name, "hd_write") == 0) { t->p_type = Integer; } 
                    else if(already_declared_func(t->attributes.name) == 0) { 
                        printf("ERRO SEMÂNTICO | LINHA: %d: Função %s não declarada!\n", t->line_number, t->attributes.name);

                        error = true;
                    } else {
                        get_func_type(t->attributes.name, p_type);
                        t->p_type = *p_type;
                        symbol_table_insert(t->attributes.name, 0, t->line_number, "global", t->p_type, 1);
                    }
                    break;
                default: break;
            }
        break;
            case exp_t:
                switch (t->type.exp)
                {
                    case exp_id:
                        if(strcmp(t->attributes.name, "void") != 0) {
                            if(already_declared_var(t->attributes.name) == 0) {
                                printf("ERRO SEMÂNTICO | LINHA %d: Variável %s não declarada\n", t->line_number, t->attributes.name);
                                
                                error = true;
                            } else if(already_declared_var_in_scope(t->attributes.name, _var_scope) == 1) {
                                get_var_type(t->attributes.name, _var_scope, p_type);

                                if(t->p_type == Void) t->p_type = *p_type;
                                
                                symbol_table_insert(t->attributes.name, 0, t->line_number, _var_scope, t->p_type, 0);
                            } else {
                                if(already_declared_global_var(t->attributes.name) == 0) {
                                    printf("ERRO SEMÂNTICO | LINHA %d: Variável %s não declarada\n",t->line_number,t->attributes.name);
                                    
                                    error = true;
                                } else {
                                    get_var_type(t->attributes.name, "global", p_type);
                                    
                                    if(t->p_type == Void) t->p_type = *p_type;
                                    
                                    symbol_table_insert(t->attributes.name, 0, t->line_number, "global", t->p_type, 0);
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }
            break;
            default: break;
    }
    free(p_type);
}

static void tree_post_order(TreeNode *tree) { 
    if(tree != NULL) { 
        for(int i=0; i < MAX_CHILD_NODES; i++) { tree_post_order(tree->childs[i]); }

        verify_node(tree);

        tree_post_order(tree->sibling);
    }
}

static void tree_pre_order(TreeNode *tree) { 
    if(tree != NULL) { 
        tree_insert_node(tree);

        for(int i=0; i < MAX_CHILD_NODES; i++) { tree_pre_order(tree->childs[i]); }
        
        tree_pre_order(tree->sibling);
    }
}

static void verify_node(TreeNode *t) {
    switch (t->node_type)
    { 
    case exp_t:
        switch (t->type.exp)
        {
            case exp_op:
                if((t->childs[0]->p_type != Integer) || (t->childs[1]->p_type != Integer))
                    show_error(t," Operação aplicada entre não inteiros!\n");
                else
                    t->p_type = Integer;
                    
                t->p_type = Integer;

                if((t->attributes.op == EQUAL) ||
                    (t->attributes.op == DIF)  || 
                    (t->attributes.op == LT)   || 
                    (t->attributes.op == LTE)  || 
                    (t->attributes.op == GT)   || 
                    (t->attributes.op == GTE)
                )
                    t->p_type = Boolean;
                else
                    t->p_type = Integer;
                break;
            default: break;
        }
        break;
    case stmt_t:
        switch (t->type.stmt)
        {
        case stm_if:
            if (t->childs[0]->p_type == Integer) show_error(t->childs[0], "IF não é do tipo <boolean>.\n");
            break;
        case stm_atrib:
            if (t->childs[1]->p_type != t->childs[0]->p_type) show_error(t->childs[1], "Tipo da váriável e do valor a ser atribuído não condizentes.\n");
            break;
        case stm_while:
            if (t->childs[0]->p_type == Integer) show_error(t->childs[0], "WHILE não do tipo <boolean>.\n");
            break;
        default:
            break;
        }
        break;
    default: break;
    }
}

void verify_node_types(TreeNode * tree, FILE *fout) {
    fprintf(fout, "Verificando tipos dos nós da árvore...\n");
    tree_post_order(tree);
    if(!error) fprintf(fout, "Deu bom! Tipos estão corretos...\n");
}

static void show_error(TreeNode *t, char *message) {
    error = true;

    if(t->node_type != exp_t && t->type.exp != exp_id)
        fprintf(stdout,"ERRO SEMÂNTICO | LINHA %d: %s\n", t->line_number, message);
    else
        fprintf(stdout,"ERRO SEMÂNTICO:%s | LINHA %d: %s\n", t->attributes.name, t->line_number, message);
}
