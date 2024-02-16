#include <stdio.h>
#include <omp.h>
#include <cstdlib>

using namespace std;
int main()
{
	omp_set_num_threads(8);
	int n = 1000;
	for (int i = 0; i < 7; i++) {
		double t1, t2;
		int m = 0;
		double x, y, pi;
		t1 = omp_get_wtime();
#pragma omp parallel for private(x,y) shared(n) reduction(+:m)
		for (int i = 0; i < n; i++) {
			x = (double)rand() / (RAND_MAX);
			y = (double)rand() / (RAND_MAX);
			if (x * x + y * y < 1) {
				m++;
			}

		}
		t2 = omp_get_wtime();
		pi = (double)m * 4.0 / n;
		printf("N= %d,  pi = %.12f, Time=%lf \n", n, pi, t2 - t1);
		n = n * 10;
	}
	system("pause");
}
