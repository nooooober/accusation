
all: List.o Graph.o GraphTest.o FindPath.o GraphTest FindPath

FindPath: List.o Graph.o FindPath.o
	gcc -std=c17 List.o Graph.o FindPath.o -o FindPath

GraphTest: List.o Graph.o GraphTest.o
	gcc -std=c17 List.o Graph.o GraphTest.o -o GraphTest

FindPath.o: FindPath.c
	gcc -std=c17 FindPath.c -c

GraphTest.o: GraphTest.c
	gcc -std=c17 GraphTest.c -c

Graph.o: Graph.c
	gcc -std=c17 Graph.c -c

List.o: List.c
	gcc -std=c17 List.c -c

clean:
	rm -rf *.o FindPath GraphTest