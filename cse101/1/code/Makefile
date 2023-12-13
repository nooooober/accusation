
all: List.o ListTest.o Lex.o lex test

lex: List.o Lex.o
	gcc -std=c17 List.o Lex.o -o Lex


test: List.o ListTest.o
	gcc -std=c17 List.o ListTest.o -o ListTest

Lex.o: Lex.c
	gcc -std=c17 Lex.c -c 

List.o: List.c
	gcc -std=c17 List.c -c 

ListTest.o: ListTest.c
	gcc -std=c17 ListTest.c -c

clean:
	rm Lex ListTest *.o 