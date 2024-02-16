#include <stdio.h>
#include <cstdlib>
#include <mpi.h>
#define _CRT_SECURE_NO_WARNINGS

int main(int* argc, char** argv) {
	int n = 1000;
	int rank, s;
	double t1, t2;
	double x, y, pi, locpi;
	MPI_Init(argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &s);
	for (int i = 0; i < 7; i++) {
		int m = 0;
		MPI_Barrier(MPI_COMM_WORLD);
		t1 = MPI_Wtime();
		for (int i = rank + 1; i < n; i += s) {
			x = (double)rand() / (RAND_MAX);
			y = (double)rand() / (RAND_MAX);
			if (x * x + y * y < 1) {
				m++;
			}

		}
		locpi = (double)4.0 * m / n;
		MPI_Reduce(&locpi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		t2 = MPI_Wtime();
		if (rank == 0) {
			printf("N= %d,  pi = %.12f, Time=%lf \n", n, pi, t2 - t1);
		}
		n = n * 10;
	}
	MPI_Finalize();
	system("pause");

}
