#include <iostream>
#include <omp.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#define N 50
double** A = (double**)malloc(N * sizeof(double*));
double** B = (double**)malloc(N * sizeof(double*));
double** C = (double**)malloc(N * sizeof(double*));
int i, j, k;

int main()
{
	setlocale(LC_ALL, "Russian");
	omp_set_num_threads(8);
	for (int i = 0; i < N; i++)
	{
		A[i] = (double*)malloc(N * sizeof(double));
		B[i] = (double*)malloc(N * sizeof(double));
		C[i] = (double*)malloc(N * sizeof(double));
	}
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			A[i][j] = 1;
			B[i][j] = 3;
		}
	double t1, t2;
	t1 = omp_get_wtime(); 
#pragma omp parallel for private(i,j,k) shared(A, B, C) schedule(dynamic)
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			C[i][j] = 0;
			for (int k = 0; k < N; k++)
				C[i][j] += A[i][k] * B[k][j];
		}
	t2 = omp_get_wtime(); 
	printf("t = %f\n", t2 - t1);
	for (int i = 0; i < N; i++)
	{
		free(A[i]);
		free(B[i]);
		free(C[i]);
	}
	free(A);
	free(B);
	free(C);
	system("pause");
}
