#include <cstdlib>
#include <omp.h>
#include <stdio.h>

#define T0 25.0
#define T1 70.0

int main() {
	double t1, t2;
	double time;
	int tests[] = { 50,100, 200, 400, 600, 800, 1000,1400,1500, 2000 };
	double eps = 0.001;
	for (int i = 0; i < 10; i++) {
		int size = tests[i];
		double* matrix = new double[size * size];
		double* savedMas = new double[size * size];
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				matrix[size * i + j] = 0;
			}
		}
		for (int i = 0; i < size; i++) {
			matrix[i] = T0;
			matrix[(size - 1) * size + i] = T1;
		}
		for (int j = 1; j < size - 1; j++) {
			matrix[size * j] = T0 + T1 / (size - 1) * j;
			matrix[size * j + size - 1] = T0 + T1 / (size - 1) * j;
		}
		bool running = true;
		t1 = omp_get_wtime();
		int flag = 0;
		do
		{
			running = false;
			{

				{
					for (int i = 1; i < size / 2; i++) {
						for (int j = 1; j < size - 1; j++) {
							double u = (matrix[size * i + j + 1] + matrix[size * (i + 1) + j] + matrix[size * i + j - 1] + matrix[size * (i - 1) + j]) / 4;
							if ((running != true) && fabs(u - matrix[size * i + j]) > eps) {
								running = true;
							}
							matrix[size * i + j] = u;
						}
					}

					while (flag == 1);
					for (int j = 1; j < size - 1; j++) {
						double u = (matrix[size * (size / 2) + j + 1] + matrix[size * (size / 2 + 1) + j]
							+ matrix[size * (size / 2) + j - 1] + matrix[size * (size / 2 - 1) + j]) / 4;
						if ((running != true) && fabs(u - matrix[size * (size / 2) + j]) > eps) {
							running = true;
						}
						matrix[size * (size / 2) + j] = u;
					}
					flag = 1;
				}

				{
					while (flag == 0);
					for (int j = 1; j < size - 1; j++) {
						double u = (matrix[size * (size / 2 + 1) + j + 1] + matrix[size * (size / 2 + 2) + j]
							+ matrix[size * (size / 2 + 1) + j - 1] + matrix[size * (size / 2) + j]) / 4;
						if ((running != true) && fabs(u - matrix[size * (size / 2 + 1) + j]) > eps) {
							running = true;
						}
						matrix[size * (size / 2 + 1) + j] = u;
					}
					flag = 0;
					for (int i = size / 2 + 2; i < size - 1; i++) {
						for (int j = 1; j < size - 1; j++) {
							double u = (matrix[size * i + j + 1] + matrix[size * (i + 1) + j] + matrix[size * i + j - 1] + matrix[size * (i - 1) + j]) / 4;
							if ((running != true) && fabs(u - matrix[size * i + j]) > eps) {
								running = true;
							}
							matrix[size * i + j] = u;
						}
					}
				}
			}
		} while (running);
		t2 = omp_get_wtime();
		time = t2 - t1;
		printf("n = %d\n", size);
		printf("time = %f\n\n", time);

	}
	system("pause");
}
