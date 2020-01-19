logo: main.o lex.yy.o grammar.tab.o commands.o
	gcc -o logo main.o lex.yy.o grammar.tab.o commands.o -lm

grammar.tab.h grammar.tab.c: grammar.y
	bison -d grammar.y

lex.yy.c: lexer.l
	flex lexer.l

main.o: main.c grammar.tab.h

lex.yy.o: lex.yy.c grammar.tab.h
