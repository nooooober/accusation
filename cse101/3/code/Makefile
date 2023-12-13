
all: List.o Graph.o GraphTest.o FindComponents.o GraphTest FindComponents

FindComponents: List.o Graph.o FindComponents.o
	gcc -std=c17 List.o Graph.o FindComponents.o -o FindComponents

GraphTest: List.o Graph.o GraphTest.o
	gcc -std=c17 List.o Graph.o GraphTest.o -o GraphTest

FindComponents.o: FindComponents.c
	gcc -std=c17 FindComponents.c -c

GraphTest.o: GraphTest.c
	gcc -std=c17 GraphTest.c -c

Graph.o: Graph.c
	gcc -std=c17 Graph.c -c

List.o: List.c
	gcc -std=c17 List.c -c

clean:
	rm -rf *.o FindComponents GraphTest