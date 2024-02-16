#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
#define size 400
#define T0 20.0
#define T1 60.0

static double matr[size][size];
double calculatePartMatrix(int start, int end) {
	double d_max = 0;
	for (int i = start; i < end; i++) {
		for (int j = 1; j < size - 1; j++) {
			double mp = matr[i][j];
			matr[i][j] = 0.25 * (matr[i + 1][j] + matr[i - 1][j] +
				matr[i][j + 1] + matr[i][j - 1]);
			double d;
			if (fabs(matr[i][j]) > 1) {
				d = fabs(matr[i][j] - mp) / matr[i][j];
			}
			else {
				d = fabs(matr[i][j] - mp);
			}
			if (d > d_max) d_max = d;
		}
	}
	return d_max;
}
int main(int argc, char** argv)
{
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matr[i][j] = 0.0;
		}
	}
	for (int i = 0; i < size; i++) {
		matr[i][0] = T0;
		matr[i][size - 1] = T1;
	}
	for (int j = 1; j < size - 1; j++) {
		matr[0][j] = T0 + T1 / (size - 1) * j;
		matr[size - 1][j] = T0 + T1 / (size - 1) * j;
	}
	MPI_Status status;
	double t1 = MPI_Wtime();
	double t2;
	double d_max = 0, d_left_max = 0;
		int middle = size / 2;
	int n = 0;
	int iter;
	if (rank == 1) {
		while (true) {
			d_left_max = calculatePartMatrix(1, middle);
			n++;
			MPI_Send(matr[middle - 1] + 1, size - 2, MPI_DOUBLE_PRECISION, 0,
				1, MPI_COMM_WORLD);
			MPI_Send(&d_left_max, 1, MPI_DOUBLE_PRECISION, 0, 2, MPI_COMM_WORLD);
			if (n > 0) MPI_Recv(&iter, 1, MPI_INT, 0, 3, MPI_COMM_WORLD,
				&status);
			if (iter > 0) {
				MPI_Send(matr[1], size * middle, MPI_DOUBLE_PRECISION, 0, 4,
					MPI_COMM_WORLD);
				break;
			}
			MPI_Recv(matr[middle] + 1, size - 2, MPI_DOUBLE_PRECISION, 0, 5,
				MPI_COMM_WORLD, &status);
		}
	}
	else if (rank == 0) {
		do {
			MPI_Recv(matr[middle - 1] + 1, size - 2, MPI_DOUBLE_PRECISION, 1,
				1, MPI_COMM_WORLD, &status);
			MPI_Recv(&d_max, 1, MPI_DOUBLE_PRECISION, 1, 2, MPI_COMM_WORLD,
				&status);
			iter = 0;
			MPI_Send(&iter, 1, MPI_INT, 1, 3, MPI_COMM_WORLD);
			MPI_Send(matr[middle] + 1, size - 2, MPI_DOUBLE_PRECISION, 1, 5,
				MPI_COMM_WORLD);
			d_max = calculatePartMatrix(middle, size - 1);
			n++;
		} while (d_max > 0.001);
		iter = 1;
		MPI_Send(&iter, 1, MPI_INT, 1, 3, MPI_COMM_WORLD);
		MPI_Recv(matr[1], size * middle, MPI_DOUBLE_PRECISION, 1, 4,
			MPI_COMM_WORLD, &status);
		calculatePartMatrix(middle + 1, size - 1);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
		t2 = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
			double time = t2 - t1;
		printf("n = %d\n", size);
		printf("time = %f\n", time);
	}
	MPI_Finalize();
	system("Pause");
	return 0;
}
