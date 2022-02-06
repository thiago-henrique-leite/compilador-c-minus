%{
  #define YYPARSER 
  #include "globals.h"
  #define YYSTYPE TreeNode*
  
  static int yylex();
  int yyerror(char *error_msg);

  static char *atr_name, *var_name, *func_name;

  static stack_t stack_func_names; 
  
  static bool first_id = true;
  
  static int cur_line_number, func_cur_line_number;
  
  static TreeNode* treeToReturn;
%}

%token IF ELSE WHILE RETURN INT VOID
%token ID NUM
%token ASSIGN EQUAL DIF LT GT LTE GTE PLUS MINUS TIMES SLASH
%token OPARENT CPARENT OBRACKET CBRACKET OKEY CKEY SEMICOLON COMMA COMENT
%token space newline
%token ERROR FIMARQ

%% 
program : list_decl { treeToReturn = $1;};

term : term mult factor {
            $$ = tree_create_exp_node(exp_op);

            $$->childs[0] = $1;
            $$->childs[1] = $3;
            $$->attributes.op = $2->attributes.op; 
          } | factor { $$ = $1; };

mult : TIMES { 
          $$ = tree_create_exp_node(exp_id); 
          $$->attributes.op = TIMES;
        }
        | SLASH {
          $$ = tree_create_exp_node(exp_id); 
          $$->attributes.op = SLASH;
        };

factor : OPARENT exp CPARENT { $$ = $1; }
        | var { $$ = $1; }
        | activate { $$ = $1; }
        | NUM { 
          $$ = tree_create_exp_node(exp_num);

          $$->attributes.value = atoi(num_token);
          $$->p_type = Integer;
        };

sum : PLUS { 
          $$ = tree_create_exp_node(exp_id); 
          $$->attributes.op = PLUS; 
        }
        | MINUS {
          $$ = tree_create_exp_node(exp_id); 
          $$->attributes.op = MINUS;
        };

exp : var ASSIGN exp
            { 
              $$ = tree_create_stmt_node(stm_atrib);

              $$->childs[0] = $1;
              $$->childs[1] = $3;
              $$->p_type = Integer;
              $$->attributes.op = ASSIGN; 
            }
            | exp_simple { $$ = $1; };

exp_decl : exp SEMICOLON { $$ = $1; } | SEMICOLON { $$ = NULL; };

exp_simple : exp_sum operator exp_sum
                { 
                  $$ = tree_create_exp_node(exp_op);
                  
                  $$->childs[0] = $1;
                  $$->childs[1] = $3;
                  $$->attributes.op = $2->attributes.op; 
                } | exp_sum { $$ = $1; };


exp_sum : exp_sum sum term
            { 
              $$ = tree_create_exp_node(exp_op);
              
              $$->childs[0] = $1;
              $$->childs[1] = $3;
              $$->attributes.op = $2->attributes.op; 
            } | term { $$ = $1; };

exp_kind : INT 
            { 
              $$ = tree_create_decl_node(decl_type); 
              $$->p_type = Integer; 
            } | 
           VOID 
            { 
              $$ = tree_create_decl_node(decl_type); 
              $$->p_type = Void; 
            };

decl : var_decl { $$ = $1; } | func_decl { $$ = $1; };

var_decl : exp_kind 
              ID 
                { 
                  atr_name = string_copy(id_token);
                  cur_line_number = line_number;
                }
              
              SEMICOLON 
                {
                  $$ = $1;
                  YYSTYPE tree_node = tree_create_decl_node(decl_var);

                  tree_node->attributes.name = atr_name;
                  tree_node->line_number = cur_line_number;
                  tree_node->p_type = $1->p_type;
                  
                  $$->childs[0] = tree_node;
                }
            | exp_kind 
              ID 
                { 
                  atr_name = string_copy(id_token);
                  cur_line_number = line_number;
                }
              
              OBRACKET NUM CBRACKET SEMICOLON 
                { 
                  $$ = $1;
                  YYSTYPE exp_node = tree_create_exp_node(exp_num);
                  
                  exp_node->attributes.value = atoi(num_token);
                  exp_node->p_type = Integer;
                  
                  YYSTYPE tree_node = tree_create_decl_node(decl_var);

                  tree_node->attributes.name = atr_name;
                  tree_node->line_number = cur_line_number;
                  tree_node->childs[0] = exp_node;

                  if($1->p_type == Integer)
                    tree_node->p_type = Array;
                  else
                    tree_node->p_type = Void;
                  
                  $$->childs[0] = tree_node; 
                };

func_decl : exp_kind 
              ID 
              {
                func_name = string_copy(id_token);
                func_cur_line_number = line_number;
              }
              OPARENT params CPARENT bloc_decl
              {
                $$ = $1;

                YYSTYPE func_node = tree_create_decl_node(decl_func);
                
                func_node->childs[0] = $5;
                func_node->childs[1] = $7;
                func_node->attributes.name = func_name;
                func_node->line_number = func_cur_line_number;
                func_node->p_type = $1->p_type;
                
                $$->childs[0] = func_node;
              };

arg : exp_kind ID 
          { 
            $$ = $1;

            atr_name = string_copy(id_token);
            cur_line_number = line_number;
            
            YYSTYPE decl_node = tree_create_decl_node(decl_var); 
            
            decl_node->attributes.name = atr_name;
            decl_node->line_number = cur_line_number;
            decl_node->p_type = $1->p_type;
            
            $$->childs[0] = decl_node;
          } | 
        exp_kind 
          ID 
          {
            atr_name = string_copy(id_token);
            cur_line_number = line_number;
          }
          OBRACKET CBRACKET
          {    
            $$ = $1;

            YYSTYPE decl_node = tree_create_decl_node(decl_var);
            
            decl_node->attributes.name = atr_name;
            decl_node->line_number = cur_line_number;
            
            if($1->p_type == Integer)
              decl_node->p_type = Array;
            else
              decl_node->p_type = $1->p_type;

            $$->childs[0] = decl_node;
          };

params : list_params { $$ = $1; } | VOID { $$ = tree_create_exp_node(exp_id); $$->attributes.name = "void"; $$->p_type = Void; };

stmt : exp_decl  { $$ = $1; }
     | bloc_decl { $$ = $1; }
     | cond_decl  { $$ = $1; }
     | loop_decl { $$ = $1; }
     | return_decl  { $$ = $1; };

bloc_decl : OKEY local_decl list_stmt CKEY {{ 
                YYSTYPE tmp = $2;
                                                    
                if(tmp != NULL)
                {
                  while(tmp->sibling != NULL) tmp = tmp->sibling;
                  
                  tmp->sibling = $3;
                  
                  $$ = $2;
                }
                else
                {
                  $$ = $3;
                }
            }};

local_decl : local_decl var_decl { 
                  YYSTYPE tmp = $1;
                                    
                  if(tmp != NULL) 
                  {
                    while(tmp->sibling != NULL) tmp = tmp->sibling;

                    tmp->sibling = $2;

                    $$ = $1;
                  }
                  else
                  {
                    $$ = $2;
                  }
                } | { $$ = NULL; };
                        
cond_decl : IF OPARENT exp CPARENT stmt
              { 
                $$ = tree_create_stmt_node(stm_if);
              
                $$->childs[0] = $3;
                $$->childs[1] = $5;
              }
              | IF OPARENT exp CPARENT stmt ELSE stmt
              { 
                $$ = tree_create_stmt_node(stm_if);
                
                $$->childs[0] = $3;
                $$->childs[1] = $5;
                $$->childs[2] = $7;
              };

loop_decl : WHILE OPARENT exp CPARENT stmt
              { 
                $$ = tree_create_stmt_node(stm_while);

                $$->childs[0] = $3;
                $$->childs[1] = $5;
              };

return_decl : RETURN SEMICOLON 
              { 
                $$ = tree_create_stmt_node(stm_return);
              }    
              | RETURN exp SEMICOLON 
              {
                $$ = tree_create_stmt_node(stm_return);

                $$->childs[0] = $2;
              };

var : ID 
      { 
        atr_name = string_copy(id_token);
        cur_line_number = line_number;

        $$ = tree_create_exp_node(exp_id);
        
        $$->attributes.name = atr_name;
        $$->line_number = cur_line_number;
        $$->p_type = Void;
      }
      | ID 
      { 
        var_name = string_copy(id_token);
        cur_line_number = line_number;
      } 
      OBRACKET exp CBRACKET 
      {
        $$ = tree_create_exp_node(exp_id);

        $$->attributes.name = var_name;
        $$->line_number = cur_line_number;
        $$->childs[0] = $4;
        $$->p_type = Integer; 
      };

operator : LTE { 
              $$ = tree_create_exp_node(exp_id); 
              $$->attributes.op = LTE; 
            }
            | LT { 
              $$ = tree_create_exp_node(exp_id); 
              $$->attributes.op = LT; 
            }
            | GT { 
              $$ = tree_create_exp_node(exp_id); 
              $$->attributes.op = GT; 
            }
            | GTE { 
              $$ = tree_create_exp_node(exp_id); 
              $$->attributes.op = GTE; 
            }
            | EQUAL { 
              $$ = tree_create_exp_node(exp_id); 
              $$->attributes.op = EQUAL; 
            }
            | DIF { 
              $$ = tree_create_exp_node(exp_id); 
              $$->attributes.op = DIF; 
            };

activate : ID { 
              if(first_id == true)
              {
                stack_init(&stack_func_names);

                first_id = false;
              }

              stack_push(&stack_func_names, string_copy(id_token));
              
              cur_line_number = line_number;
            }
            OPARENT arguments CPARENT { 
              $$ = tree_create_stmt_node(stm_call);

              $$->childs[1] = $4; 
              $$->attributes.name = stack_pop(&stack_func_names);
              $$->line_number = cur_line_number;
            };

arguments : list_arg { $$ = $1; } | { $$ = NULL; };

list_decl : list_decl decl
                { 
                  YYSTYPE tmp = $1;
                  
                  if(tmp != NULL) 
                  {
                    while(tmp->childs[0]->sibling != NULL) { tmp = tmp->childs[0]->sibling; }
                    tmp->childs[0]->sibling = $2;

                    $$ = $1;
                  }
                  else
                  {
                    $$ = $2;
                  }
                }   
              | decl { $$ = $1; };

list_arg : list_arg COMMA exp { 
                YYSTYPE tmp = $1;
                                    
                if(tmp != NULL)
                {
                    while(tmp->sibling != NULL)
                        tmp = tmp->sibling;

                    tmp->sibling = $3;
                    $$ = $1;
                }
                else
                {
                    $$ = $3;
                }
              } | arg { $$ = $1; } | exp { $$ = $1; };

list_params : list_params COMMA arg { 
                YYSTYPE tmp = $1;

                if(tmp != NULL)
                {
                  while(tmp->sibling != NULL) tmp = tmp->sibling;

                  tmp->sibling = $3;
                  
                  $$ = $1;
                } 
                else
                { 
                  $$ = $3; 
                }
              } | arg { $$ = $1; };

list_stmt : list_stmt stmt { 
                  YYSTYPE tmp = $1;
                                    
                  if(tmp != NULL)
                  {
                    while(tmp->sibling != NULL) tmp = tmp->sibling;

                    tmp->sibling = $2;
                    
                    $$ = $1;
                  }
                  else
                  {
                    $$ = $2;
                  }
                } | { $$ = NULL; };
%%

int yyerror(char *error_msg) {
    char* token_name = get_token_name(yychar);

    if(yychar == ID || yychar == NUM) { fprintf(stdout,"ERRO SINTÁTICO %s | LINHA : %d\n", token_name, line_number); }
    else{ fprintf(stdout,"ERRO SINTÁTICO %s (%s) | LINHA: %d\n", token_name, yytext, line_number); }

    error = true; free(token_name);
    
    return -1;
}

static int yylex(void) { return getToken(); }
TreeNode *parse(void) { yyparse(); return treeToReturn; }
