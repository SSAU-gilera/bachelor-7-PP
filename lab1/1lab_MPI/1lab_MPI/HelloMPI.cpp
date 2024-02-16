#include <stdio.h>
#include <cstdlib>
#include "mpi.h"

int main(int* argc, char** argv)
{
	const int size = 2000;
	double t1;
	double t2;


	MPI_Init(argc, &argv);       

	int n, r;
	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &n);               
	MPI_Comm_rank(MPI_COMM_WORLD, &r);				

	int chunk = size / n;

	double* MatrixA = (double*)malloc(sizeof(double) * size * size);

	if (r == 0) {
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				MatrixA[j + i * size] = 1.0;
	}

	double* MatrixB = (double*)malloc(sizeof(double) * size * size);

	if (r == 0) {
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				MatrixB[j + i * size] = 1.0;
	}

	double* MatrixPartRes = (double*)malloc(sizeof(double) * chunk * size);
	double* MatrixC = (double*)malloc(sizeof(double) * size * size);

	t1 = MPI_Wtime();

	if (r == 0) {

		for (int i = 1; i < n; i++) {
			MPI_Send(MatrixA + i * size * chunk, size * chunk, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(MatrixA, size * chunk, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}

	MPI_Bcast(MatrixB, size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	for (int i = 0; i < chunk; i++) {
		for (int j = 0; j < size; j++) {
			MatrixPartRes[i * size + j] = 0;
			for (int k = 0; k < size; k++)
				MatrixPartRes[i * size + j] += MatrixA[i * size + k] * MatrixB[k * size + j];
		}
	}

	if (r != 0) {
		MPI_Send(MatrixPartRes, size * chunk, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	if (r == 0) {
		for (int i = 0; i < chunk * size; i++) {
			MatrixC[i] = MatrixPartRes[i];
		}

		for (int i = 1; i < n; i++) {
			MPI_Recv(MatrixC + i * size * chunk, size * chunk, MPI_DOUBLE, i,
				MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
	}

	t2 = MPI_Wtime();

	if (r == 0) {
		printf(" t= %.15f \n", t2 - t1);

	}
	MPI_Finalize();           

	system("pause");
}
