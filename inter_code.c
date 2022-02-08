#include "inter_code.h"

void generate_intermediate_code(TreeNode * tree) {
    stack_init(&inter_code_stack);
    generate_code(tree);
    finish_inter_code();
}

char* allocate_register(int n) {
   char* register_name = (char*)malloc(sizeof(char) * 4);
   sprintf(register_name, "t%d", n);
   return register_name;
}

char* label_num(int n) {
    char* label_n = (char*)malloc(sizeof(char) * 6); 
    sprintf(label_n, "L%d", n);
    return label_n;
}

char* convert_number_to_str(int n) {
    int n_original = n, digit = 0;

    while(n != 0) { n /= 10; digit++; }

    char* str_number = malloc(sizeof(char) * (digit + 1)); 

    sprintf(str_number, "%d", n_original);
    
    return str_number;
}

static void stmt_node(TreeNode *tree) {
    TreeNode *c1, *c2, *c3;

    int reg_num_1, reg_num_2, reg_num_3, label_1, label_2, label_3;

    switch (tree->type.stmt)
    {
        case stm_if:
            c1 = tree->childs[0]; c2 = tree->childs[1]; c3 = tree->childs[2];

            generate_code(c1);
            
            reg_num_1 = n_tmp; label_1 = n_label++; label_2 = n_label++;

            list_insert(&intermediate_code, "IFF", allocate_register(reg_num_1), label_num(label_1), " ");

            generate_code(c2);
            list_insert(&intermediate_code, "GOTO", label_num(label_2), " ", " ");
            list_insert(&intermediate_code, "LAB",  label_num(label_1), " ", " ");
            
            generate_code(c3);
            list_insert(&intermediate_code, "LAB", label_num(label_2), " ", " ");
            
            break; 
        case stm_while:
            c1 = tree->childs[0]; c2 = tree->childs[1];

            label_1 = n_label++; label_2 = n_label++; reg_num_1 = n_tmp;
            
            list_insert(&intermediate_code, "LAB", label_num(label_1), " ", " ");

            generate_code(c1);
            list_insert(&intermediate_code, "IFF", allocate_register(reg_num_1), label_num(label_2), " ");

            generate_code(c2);
            list_insert(&intermediate_code, "GOTO", label_num(label_1), " ", " ");
            list_insert(&intermediate_code, "LAB", label_num(label_2), " ", " ");

            break;
        case stm_atrib:
            c1 = tree->childs[0]; c2 = tree->childs[1];

            stack_push(&inter_code_stack, c1->attributes.name);
            
            generate_code(c1); reg_num_1 = n_tmp;
            generate_code(c2); reg_num_2 = n_tmp;

            list_insert(&intermediate_code, "ASSIGN", allocate_register(reg_num_1), allocate_register(reg_num_2), " ");
            
            char *nome_var = stack_pop(&inter_code_stack);
            
            if(c1->childs[0] != NULL) {
                list_insert(&intermediate_code, "STORE", nome_var, allocate_register(reg_num_1), allocate_register(reg_num_1-1));
                
                registers[reg_num_1 - 1] = 0;
            } else {
                list_insert(&intermediate_code, "STORE", nome_var, allocate_register(reg_num_1), " ");
            }

            registers[reg_num_1] = registers[reg_num_2] = 0;
            
            break;
        case stm_return:
            c1 = tree->childs[0];
            
            generate_code(c1);
            
            list_insert(&intermediate_code, "RET", allocate_register(n_tmp), " ", " ");
            
            break;
        case stm_call:
            c1 = tree->childs[1];

            stop = true;
            
            int aux = 0;
            
            while(c1 != NULL)
            {
                generate_code(c1);
                
                list_insert(&intermediate_code, "PARAM", allocate_register(n_tmp), " ", " ");
                
                c1 = c1->sibling;
                
                aux++;
            }

            stop = false;
            
            n_tmp = register_use();

            list_insert(&intermediate_code, "CALL", tree->attributes.name, convert_number_to_str(aux), allocate_register(n_tmp));
            
            break;
        default:
            break;
    }
}

static void decl_node(TreeNode *tree) { 
   TreeNode *c1, *c2, *c3;

   switch(tree->type.decl) 
   {
        case decl_type:
            generate_code(tree->childs[0]);
            break;
        case decl_func:
            c1 = tree->childs[0]; c2 = tree->childs[1];
            list_insert(&intermediate_code, "FUN", get_string_type(tree->p_type), tree->attributes.name, " ");
            
            if(strcmp(scope, tree->attributes.name) != 0) {
                scope = tree->attributes.name;

                for(int i=0; i<MAX_REGISTERS; i++) { registers[i] = 0; }
            }

            arg = 1; generate_code(c1);
            arg = 0; generate_code(c2);
            
            list_insert(&intermediate_code, "END", tree->attributes.name, " ", " ");
            
            list_args_length = 0;
            
            break;
        case decl_var:
            if(arg == 1) {
                list_insert(&intermediate_code, "ARG", get_string_type(tree->p_type), tree->attributes.name, scope);
                list_args[list_args_length++] = strdup(tree->attributes.name);
                
            } else {
                if(tree->childs[0] != NULL)
                    list_insert(&intermediate_code, "ALLOC", tree->attributes.name, scope, convert_number_to_str(tree->childs[0]->attributes.value));  
                else
                    list_insert(&intermediate_code, "ALLOC", tree->attributes.name, scope, " ");
            }

            break;
        default:
            break;
   }
}

static void exp_node(TreeNode *tree) {
    TreeNode *c1, *c2;

    switch (tree->type.exp) 
    {
        case exp_num:
            n_tmp = register_use();

            list_insert(&intermediate_code, "LOAD", allocate_register(n_tmp), convert_number_to_str(tree->attributes.value), " ");

            break;
        case exp_id:
            if(strcmp(tree->attributes.name, "void") == 0) break;
            
            if(tree->childs[0] != NULL) {
                generate_code(tree->childs[0]);
                
                int reg_num_1 = n_tmp; n_tmp = register_use();
                
                list_insert(&intermediate_code, "LOAD", allocate_register(n_tmp), tree->attributes.name, allocate_register(n_tmp-1));
            } else {
                n_tmp = register_use();

                list_insert(&intermediate_code, "LOAD", allocate_register(n_tmp), tree->attributes.name, " ");
            }
        break; 

        case exp_op:
            c1 = tree->childs[0]; c2 = tree->childs[1];
            
            generate_code(c1);
            
            int reg_num_1 = n_tmp; generate_code(c2);
            int reg_num_2 = n_tmp; 
            
            n_tmp = register_use();
            
            switch(tree->attributes.op) {
                case DIF :
                    list_insert(&intermediate_code, "NEQ", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case EQUAL :
                    list_insert(&intermediate_code, "EQUAL", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case GT :
                    list_insert(&intermediate_code, "GT", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case GTE:
                    list_insert(&intermediate_code, "GEQ", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case LT :
                    list_insert(&intermediate_code, "LT", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case LTE:
                    list_insert(&intermediate_code, "LEQ", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case MINUS :
                    list_insert(&intermediate_code, "SUB", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case PLUS :
                    list_insert(&intermediate_code, "ADD", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case SLASH :
                    list_insert(&intermediate_code, "DIV", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                case TIMES :
                    list_insert(&intermediate_code, "MUL", allocate_register(reg_num_1), allocate_register(reg_num_2), allocate_register(n_tmp));
                    break;
                default:
                    printf("Operador de comparação inválido!\n"); error = 1;
                    break;
            }
            registers[reg_num_1] = 0; registers[reg_num_2] = 0;
            
            break; 
        default:
            break;
    }
} 

static void generate_code(TreeNode *tree) {
    if(n_tmp == 32) {
        for(int i=0; i<MAX_REGISTERS; i++) { registers[i] = 0; }
        error = true;
    }

    if(tree != NULL) {
        switch (tree->node_type) 
        {
            case stmt_t: stmt_node(tree); break;
            case exp_t: exp_node(tree); break;
            case decl_t: decl_node(tree); break;
            default: break;
        }

        if(!stop) generate_code(tree->sibling);
    } else {
      error = true;    
    }
}

void finish_inter_code() {
   list_insert(&intermediate_code, "HALT", " ", " ", " ");
}
