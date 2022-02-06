compiler:
	flex scanner.l
	bison -d parse.y && gcc -c lex.yy.c
	gcc analyze.c globals.c inter_code.c lex.yy.o main.c parse.tab.c symbol_table.c -o compiler.x -lfl
	
remove:
	rm *.x
	rm *.o
	rm *.tab.c
	rm *.tab.h
	rm *.yy.c
