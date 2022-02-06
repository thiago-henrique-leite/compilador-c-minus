%{
  #include "globals.h"

  char id_token[MAX_TOKEN_LENGTH], num_token[MAX_TOKEN_LENGTH], str_token[MAX_TOKEN_LENGTH];
%}

digit [0-9]
letter [a-zA-Z]
identifier {letter}({letter}|{digit})*
newline \n
number {digit}+
space [ \t]+

%%
"="      { return ASSIGN; }
"]"      { return CBRACKET; }
"}"      { return CKEY; }
","      { return COMMA; }
")"      { return CPARENT; }
"!="     { return DIF; }
"else"   { return ELSE; }
"=="     { return EQUAL;}
">"      { return GT; }
">="     { return GTE; }
"if"     { return IF; }
"int"    { return INT; }
"<"      { return LT; }
"<="     { return LTE; }
"-"      { return MINUS;}
"["      { return OBRACKET; }
"{"      { return OKEY; }
"("      { return OPARENT; }
"+"      { return PLUS; }
"return" { return RETURN; }
";"      { return SEMICOLON; }
"/"      { return SLASH; }
"*"      { return TIMES; }
"void"   { return VOID; }
"while"  { return WHILE; }
"/*"     {
            char current, previous;

            do {
              current = input();
              if(current == EOF) break;
              if(current == '\n') line_number++;
              if (previous == '*' && current == '/')
                break;
              else
                previous = current;
            } while(true);
         }
{identifier} { 
                strncpy(id_token, yytext, MAX_TOKEN_LENGTH);
                return ID; 
             }
{newline} { line_number++; }
{number} { 
            strncpy(num_token, yytext, MAX_TOKEN_LENGTH); 
            return NUM;
         }
{space} {}

. { 
    printf("ERRO LÉXICO: %s | LINHA: %d\n", yytext, line_number);
    return ERROR; 
  }

<<EOF>> { return EOF;}
%%

token_t getToken(void) {
    static bool first_time = 1;
    token_t token;

    if (first_time) {
        first_time = 0;
        line_number++;
        yyin = input_file;
        yyout = stdout;
    }

    token = yylex();
    
    strncpy(str_token, yytext, MAX_TOKEN_LENGTH);
    
    return token;
}
