#include "globals.h"

FILE *input_file, *out_file;

int main(int argc, char** argv) {
    if(argc != 3) 
    {
        printf("Modo de executar o programa: %s codigo.c saida.txt\n", argv[0]);
        
        return -1;
    }

    FILE *out_file = fopen(argv[2], "w");

    make_lexical_analysis(argv[1], out_file);
    make_syntax_tree(argv[1], out_file);
    make_symbol_table(out_file);
    make_node_types_verify(out_file);
    make_intermediate_code(out_file);

    return 0;
}
