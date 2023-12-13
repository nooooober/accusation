all: List.o Matrix.o Sparse.o MatrixTest.o ListTest.o MatrixTest ListTest Sparse

Sparse: List.o Matrix.o Sparse.o
	gcc -std=c17 List.o Matrix.o Sparse.o -o Sparse

ListTest: List.o ListTest.o
	gcc -std=c17 List.o ListTest.o -o ListTest

MatrixTest: List.o Matrix.o MatrixTest.o
	gcc -std=c17 List.o Matrix.o MatrixTest.o -o MatrixTest

ListTest.o: ListTest.c
	gcc -std=c17 ListTest.c -c

MatrixTest.o: MatrixTest.c
	gcc -std=c17 MatrixTest.c -c

Sparse.o: Sparse.c
	gcc -std=c17 Sparse.c -c

Matrix.o: Matrix.c
	gcc -std=c17 Matrix.c -c

List.o: List.c
	gcc -std=c17 List.c -c

clean:
	rm -rf MatrixTest ListTest Sparse *.o