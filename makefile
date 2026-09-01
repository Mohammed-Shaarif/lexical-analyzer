all: main.c lexer.c
	gcc main.c lexer.c -o lexer

main: main.c lexer.c
	gcc main.c lexer.c -o lexer

rm: 
	rm -f lexer