#include <stdio.h>
#include <omp.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

int findMaxInColumn(const vector<vector<double>>& matrix, int col, int n) {
	int max = abs(matrix[col][col]);
	int maxPos = col;
	for (int i = col + 1; i < n; ++i) {
		int element = abs(matrix[i][col]);
		if (element > max) {
			max = element;
			maxPos = i;
		}
	}
	return maxPos;
}

int toTriangularMatrix(vector<vector<double>>& matrix, int n) {
	unsigned int swapCount = 0;
	for (int i = 0; i < n - 1; ++i) {
		unsigned int imax = findMaxInColumn(matrix, i, n);
		if (i != imax) {
			swap(matrix[i], matrix[imax]);
			++swapCount;
		}

#pragma omp parallel for 
		for (int j = i + 1; j < n; ++j) {
			double mul = -matrix[j][i] / matrix[i][i];
			for (int k = i; k < n; ++k)
				matrix[j][k] += matrix[i][k] * mul;
		}
	}
	return swapCount;
}

void swapCoefficientsWithConsts(vector<vector<double>> & matrix, vector<double> & constantTermsSLAU, int i, int n) {
	double temp;
	for (int j = 0; j < n; j++) {
		temp = matrix[j][i];
		matrix[j][i] = constantTermsSLAU[j];
		constantTermsSLAU[j] = temp;
	}
}

double findDetByGauss(vector<vector<double>> & matrix, int n) {
	unsigned int swapCount = toTriangularMatrix(matrix, n);
	double det;
	if (swapCount % 2 == 1)
		det = -1;
	else
		det = 1;
	for (int i = 0; i < n; ++i)
		det *= matrix[i][i];
	return det;
}

vector<double> metodCramer(vector<vector<double>> & сoefficientsSLAU, vector<double> & constantTermsSLAU, int n) {
	vector<double> solution(n);
	vector<vector<double>> basicMatrix(сoefficientsSLAU);
	vector<vector<double>> tempMatrix(сoefficientsSLAU);
	double mainDet = findDetByGauss(basicMatrix, n);
	if (abs(mainDet) < 0.0001)
		throw invalid_argument("Определитель исходной матрицы меньше нуля, применение метода Крамера невозможно!");
	else {

#pragma omp parallel for
		for (int i = 0; i < n; ++i) {
			swapCoefficientsWithConsts(tempMatrix, constantTermsSLAU, i, n);
			vector<vector<double>> tempTriangleMatrix = tempMatrix;
			solution[i] = findDetByGauss(tempTriangleMatrix, n) / mainDet;
			swapCoefficientsWithConsts(tempMatrix, constantTermsSLAU, i, n);
		}
		return solution;
	}
}

int random(int a, int b)
{
	srand(time(NULL));
	if (a > 0)
		return a + rand() % (b - a);
	else
		return a + rand() % (abs(a) + b);
}

int main() {
	setlocale(LC_ALL, "Russian");

	omp_set_num_threads(2);//задаём количество потоков

	cout << "\n Курсовая работа по предмету \"Параллельное программирование\"" << endl;
	cout << "\n\n Параллельный алгоритм решения СЛАУ методом Крамера" << endl;
	cout << " с помощью OpenMP (p=2)\n" << endl;
	int n = 20;
	double timeStart, timeFinish;
	vector<vector<double>> сoefficientsSLAU;
	vector<double> constantTermsSLAU;
	do {
		сoefficientsSLAU.resize(n);
		constantTermsSLAU.resize(n);
		for (int i = 0; i < n; i++) {
			сoefficientsSLAU[i].resize(n);
			for (int j = 0; j < n; j++)
				сoefficientsSLAU[i][j] = random(-30, 30);
			constantTermsSLAU[i] = random(-100, 100);
		}
		timeStart = omp_get_wtime();
		vector<double> solution = metodCramer(сoefficientsSLAU, constantTermsSLAU, n);
		timeFinish = omp_get_wtime();
		cout << "  n = " << n << ", t = " << timeFinish - timeStart << endl;
		n += 20;
	} while (n <= 200);

	cout << "\n\n Выполнили: Гижевская В.Д и Сусликова М.С." << endl;
	system("pause");
}


