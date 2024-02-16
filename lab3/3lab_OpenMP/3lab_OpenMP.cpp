#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define T0 20.0
#define T1 60.0

int main() {
	double time1, time2;
	double resultTime;
	int sizes[] = { 50,100, 200, 400, 600, 800, 1000,1200,1500, 2000 };
	double e = 0.001;
	for (int i = 0; i < 10; i++) {
		int size = sizes[i];
		double* matr = new double[size * size];
		double* savedMas = new double[size * size];
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				matr[size * i + j] = 0.0;
			}
		}
		for (int i = 0; i < size; i++) {
			matr[i] = T0;
			matr[(size - 1) * size + i] = T1;
		}
		for (int j = 1; j < size - 1; j++) {
			matr[size * j] = T0 + T1 / (size - 1) * j;
			matr[size * j + size - 1] = T0 + T1 / (size - 1) * j;
		}


		bool running = true;
		time1 = omp_get_wtime();
		int flag = 0;

		do
		{
			running = false;
#pragma omp parallel sections num_threads(4)    shared(matr)

			{
#pragma omp section
				{
					for (int i = 1; i < size / 2; i++) {
						for (int j = 1; j < size - 1; j++) {
							double u = (matr[size * i + j + 1] + matr[size * (i + 1) + j] + matr[size * i + j - 1] + matr[size * (i - 1) + j]) / 4;
							if ((running != true) && fabs(u - matr[size * i + j]) > e) {
								running = true;
							}
							matr[size * i + j] = u;

						}
					}

					while (flag == 1);
					for (int j = 1; j < size - 1; j++) {
						double u = (matr[size * (size / 2) + j + 1] + matr[size * (size / 2 + 1) + j]
							+ matr[size * (size / 2) + j - 1] + matr[size * (size / 2 - 1) + j]) / 4;
						if ((running != true) && fabs(u - matr[size * (size / 2) + j]) > e) {
							running = true;
						}
						matr[size * (size / 2) + j] = u;
					}
					flag = 1;
				}
#pragma omp section
				{
					while (flag == 0);
					for (int j = 1; j < size - 1; j++) {
						double u = (matr[size * (size / 2 + 1) + j + 1] + matr[size * (size / 2 + 2) + j]
							+ matr[size * (size / 2 + 1) + j - 1] + matr[size * (size / 2) + j]) / 4;
						if ((running != true) && fabs(u - matr[size * (size / 2 + 1) + j]) > e) {
							running = true;
						}
						matr[size * (size / 2 + 1) + j] = u;

					}
					flag = 0;
					for (int i = size / 2 + 2; i < size - 1; i++) {
						for (int j = 1; j < size - 1; j++) {
							double u = (matr[size * i + j + 1] + matr[size * (i + 1) + j] + matr[size * i + j - 1] + matr[size * (i - 1) + j]) / 4;
							if ((running != true) && fabs(u - matr[size * i + j]) > e) {
								running = true;
							}
							matr[size * i + j] = u;
						}
					}
				}
			}
		} while (running);
		time2 = omp_get_wtime();
		resultTime = time2 - time1;
		printf("n = %d\n", size);
		printf("time = %f\n", resultTime);

	}
	system("pause");
}
