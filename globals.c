#include "globals.h"

static int tab_tree = 0;

int registers[MAX_REGISTERS];

void print_tokens(char* fileName, FILE *fout) {
    FILE* fc = fopen(fileName, "r");

    char line_string[MAX_TOKEN_LENGTH], aux_line_string[MAX_TOKEN_LENGTH], line[MAX_LINES];

    int counter = 0, correct_line = 0, next_line = 0;

    while(1) {
        if(fgets(line, MAX_LINES, fc) == NULL) break;

        if(line[0] == '\n') 
            if(fgets(line, MAX_LINES, fc) == NULL) 
                break;
        
        token_t token;
        
        correct_line = line_number;

        if(counter == 0) { correct_line++; counter++; }
        
        sprintf(line_string, "\n\n%d: ", correct_line);
        sprintf(aux_line_string, "%s", line);
        strcat(line_string, aux_line_string);
        file_write(fout, line_string);
        bzero(line_string, MAX_TOKEN_LENGTH);

        char *token_name = get_token_name(token);
        
        if(next_line) {
            sprintf(line_string, "%s ", token_name);
            file_write(fout, line_string);
            bzero(line_string, MAX_TOKEN_LENGTH);
            next_line = 0;
        }

        free(token_name);
        
        do {
            token = getToken();

            token_name = get_token_name(token);
            
            if(token == EOF ) { free(token_name); break; }

            if(line_number != correct_line) { next_line = 1; free(token_name); break; }
            
            sprintf(line_string, "%s ", token_name);
            file_write(fout, line_string);
            bzero(line_string, MAX_TOKEN_LENGTH);
            free(token_name);
        } while(1);
    }
    fclose(fc);
}

void file_write(FILE *fout, char *str) {
    fprintf(fout, "%s", str);
}

char *string_copy(char *str) { 
    char *t = malloc(strlen(str) + 1);

    strcpy(t, str);
    
    return t;
}

char *get_token_name(token_t t) {
    char *name = (char*) malloc(sizeof(char) * MAX_TOKEN_LENGTH);

    switch(t) 
    {
        case ASSIGN: strcpy(name, "ASSIGN"); break;
        case CBRACKET: strcpy(name, "CBRACKET"); break;
        case CKEY: strcpy(name, "CKEY"); break;
        case COMENT: strcpy(name, "COMENT"); break;
        case COMMA: strcpy(name, "COMMA"); break;
        case CPARENT: strcpy(name, "CPARENT"); break;
        case DIF: strcpy(name, "DIF"); break;
        case ELSE:  strcpy(name, "ELSE"); break;
        case EQUAL: strcpy(name, "EQUAL"); break;
        case GT: strcpy(name, "GT"); break;
        case GTE: strcpy(name, "GTE"); break;
        case ID: strcpy(name, "ID"); break;
        case IF: strcpy(name, "IF"); break;
        case INT: strcpy(name, "INT"); break;
        case LT: strcpy(name, "LT"); break;
        case LTE: strcpy(name, "LTE"); break;
        case MINUS: strcpy(name, "MINUS"); break;
        case newline: strcpy(name, "newline"); break;
        case NUM: strcpy(name, "NUM"); break;
        case OBRACKET: strcpy(name, "OBRACKET"); break;
        case OKEY: strcpy(name, "OKEY"); break;
        case OPARENT: strcpy(name, "OPARENT"); break;
        case PLUS: strcpy(name, "PLUS"); break;
        case RETURN: strcpy(name, "RETURN"); break;
        case SEMICOLON: strcpy(name, "SEMICOLON"); break;
        case SLASH: strcpy(name, "SLASH"); break;
        case space: strcpy(name, "space"); break;
        case TIMES: strcpy(name, "TIMES"); break;
        case VOID: strcpy(name, "VOID"); break;
        case WHILE: strcpy(name, "WHILE"); break;
        default: strcpy(name, "INVALID"); break;
    }
            
    return name;
}

char *get_string_type(PrimitiveType t) {
    switch(t)
    {
        case Integer: return "int"; break;
        case Void: return "void"; break;
        case Array: return "array"; break;
        default: return "bool"; break;
    }
}

void stack_init(stack_t *stack) {
    stack = (stack_t*)malloc(sizeof(stack_t*));

    stack->top = NULL;
    stack->size = 0;
}

void stack_push(stack_t *stack, char* name) {
    name_t *name_i = (name_t*) malloc(sizeof(name_t*));

    name_i->next = stack->top;
    name_i->name = strdup(name);
    stack->top = name_i;
    stack->size++;
}

char *stack_pop(stack_t *stack) {
    name_t *old_top;
    char *name = NULL;

    if(stack->size == 0)
        return NULL;

    name = strdup(stack->top->name);

    old_top = stack->top;
    stack->top = old_top->next;

    free(old_top);
    
    stack->size--;

    return name;
}

void list_init(instruction_list_t *list) {
    list->first = NULL;
}

void list_insert(instruction_list_t *list, char *op, char *c1, char *c2, char *c3) {
    instruction_node_t *new_node = (instruction_node_t*) malloc(sizeof(instruction_node_t));

    new_node->next = NULL;

    instruction_t *new_instruction = (instruction_t*) malloc(sizeof(instruction_t));

    new_node->instruction = new_instruction;

    new_instruction->op = strdup(op); new_instruction->c1 = strdup(c1);
    new_instruction->c2 = strdup(c2); new_instruction->c3 = strdup(c3);
    
    if(list->first == NULL) {
        list->first = new_node;
    } else {
        instruction_node_t* instruction_node = list->first;

        while(instruction_node->next != NULL)
            instruction_node = instruction_node->next;

        instruction_node->next = new_node;
    }
}

void list_print(instruction_list_t *list, FILE *fout) {
    instruction_node_t* instruction_node = list->first;

    char quadruple[MAX_TOKEN_LENGTH];

    while(instruction_node != NULL) 
    {
        bzero(quadruple, MAX_TOKEN_LENGTH);
        strcat(quadruple, "(");
        strcat(quadruple, instruction_node->instruction->op);
        strcat(quadruple, ", ");
        strcat(quadruple, instruction_node->instruction->c1);
        strcat(quadruple, ", ");
        strcat(quadruple, instruction_node->instruction->c2);
        strcat(quadruple, ", ");
        strcat(quadruple, instruction_node->instruction->c3);
        strcat(quadruple, ")\n");

        file_write(fout, quadruple);

        instruction_node = instruction_node->next;
    }
}

TreeNode *tree_init_node(NodeType node_type) {
    TreeNode *node = (TreeNode*) malloc(sizeof(TreeNode));

    for(int i=0; i<MAX_CHILD_NODES; i++)
        node->childs[i] = NULL;

    node->sibling = NULL;
    node->line_number = line_number;
    node->node_type = node_type;
}

TreeNode *tree_create_decl_node(DeclType type) { 
    TreeNode *node = tree_init_node(decl_t);
    
    node->type.decl = type;

    return node;
}

TreeNode *tree_create_stmt_node(StmType type) { 
    TreeNode *node = tree_init_node(stmt_t);
    
    node->type.decl = type;

    return node;
}

TreeNode *tree_create_exp_node(ExpType type) { 
    TreeNode *node = tree_init_node(exp_t);
    
    node->type.decl = type;

    return node;
}

void tree_print(TreeNode *tree, FILE *fout) { 
    tab_tree += TAB_SIZE;

    while(tree != NULL) 
    {
        for(int i=0; i<tab_tree; i++)
            file_write(fout, " ");

        if(tree->node_type == exp_t) 
        { 
            switch (tree->type.exp)
            {
                case exp_id:
                    if(strcmp(tree->attributes.name,"void") == 0) {
                        file_write(fout, "Void\n");
                    } else {
                        sprintf(str_token, "Id: %s\n",tree->attributes.name);
                        file_write(fout, str_token);
                    }
                    break;
                default:
                    file_write(fout, "Expression desconhecida!\n");
                    break;
                case exp_num:
                    sprintf(str_token, "Num: %d\n", tree->attributes.value);
                    file_write(fout, str_token);
                    break;
                case exp_op:  
                    sprintf(str_token, "Operador: %s\n", get_token_name(tree->attributes.op));
                    file_write(fout, str_token);
                    break;
            }
        }
        else if(tree->node_type == stmt_t) 
        { 
            switch (tree->type.stmt) 
            {
                case stm_if: file_write(fout, "If\n"); break;
                case stm_while: file_write(fout, "While\n"); break;
                case stm_atrib: file_write(fout, "Atribuição\n"); break;
                case stm_return: file_write(fout, "Retorno\n"); break;
                case stm_call:
                    sprintf(str_token, "Chamada de Função: %s\n", tree->attributes.name);
                    file_write(fout, str_token);
                    break;
                default: file_write(fout, "Statement inválido!\n"); break;
            }
        }
        else if (tree->node_type == decl_t) {
            switch(tree->type.decl)
            {
                case decl_type:
                    if (tree->p_type == Integer)
                        file_write(fout, "PrimitiveType int\n");
                    else if(tree->p_type == Array) 
                        file_write(fout, "PrimitiveType int[]\n");
                    else
                        file_write(fout, "PrimitiveType void\n");
                    break;
                case decl_func:
                    sprintf(str_token, "Função: %s\n",tree->attributes.name);
                    file_write(fout, str_token);
                    break;
                case decl_var:
                    sprintf(str_token, "Variável: %s\n",tree->attributes.name);
                    file_write(fout, str_token);
                    break;         
                default:
                    file_write(fout, "Declaração inválida!\n");
                    break;
            } 
        } else {
            file_write(fout, "PrimitiveType desconhecido!\n");
        }

        bzero(str_token, MAX_TOKEN_LENGTH);

        for(int j=0; j<MAX_CHILD_NODES; j++)
            tree_print(tree->childs[j], fout);

        tree = tree->sibling;
    }

    tab_tree -= TAB_SIZE;
}

int register_use() {
    for(int i=0; i<MAX_REGISTERS; i++)
        if(registers[i] == 0) { registers[i] = 1; return i; }

    printf("ERRO DE SÍNTESE: Nenhum registrador disponível!\n");

    error = 1; return 0;
}

void make_lexical_analysis(char *input_file_name, FILE *output_file) {
    input_file = fopen(input_file_name, "r");

    if(input_file == NULL) {
        perror("fopen()"); 
        
        return;
    }

    file_write(output_file, "ETAPA 01: Análise Léxica");

    print_tokens(input_file_name, output_file);

    fclose(input_file);
}

void make_syntax_tree(char *input_file_name, FILE *output_file) {
    input_file = fopen(input_file_name, "r");
    
    syntax_tree = parse();

    if(!error) {
        file_write(output_file, "\n\nETAPA 02: Geração da Árvore Sintática\n\n");
        tree_print(syntax_tree, output_file);
    }
}

void make_symbol_table(FILE *output_file) {
    if(!error) {
        file_write(output_file, "\n\nETAPA 03: Geração da Tabela de Símbolos\n\n");
        symbol_table_build(syntax_tree, output_file);
    }
}

void make_node_types_verify(FILE *output_file) {
    if(!error) {
        file_write(output_file, "\n\nETAPA 04: Análise Semântica\n\n");
        verify_node_types(syntax_tree, output_file);
    }
}

void make_intermediate_code(FILE *output_file) {
    if(!error) {
        file_write(output_file, "\n\nETAPA 05: Geração do Código Intermediário\n\n");
        list_init(&intermediate_code);
        generate_intermediate_code(syntax_tree);
        list_print(&intermediate_code, output_file);
    }

    fclose(input_file);
}
