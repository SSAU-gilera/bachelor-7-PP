#include <iostream>
#include <vector>
#include <ctime>
#include <mpi.h>
#include "omp.h"

using namespace std;

int findMaxInColumn(const vector<vector<double>>& matrix, int col, int n) {
	int max = abs(matrix[col][col]);
	int maxPos = col;
	for (int i = col + 1; i < n; i++) {
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
		for (int j = i + 1; j < n; ++j) {
			if (matrix[i][i] == 0)
				exit(1);
			double mul = -matrix[j][i] / matrix[i][i];
			for (int k = i; k < n; ++k)
				matrix[j][k] += matrix[i][k] * mul;
		}
	}
	return swapCount;
}

void swapCoefficientsWithConsts(vector<vector<double>>& matrix, vector<double>& constantTermsSLAU, int i, int n) {
	double temp;
	for (int j = 0; j < n; j++) {
		temp = matrix[j][i];
		matrix[j][i] = constantTermsSLAU[j];
		constantTermsSLAU[j] = temp;
	}
}

double findDetByGauss(vector<vector<double>>& matrix, int n) {
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

void parallelMetodCramer(vector<vector<double>>& ñoefficientsSLAU, vector<double>& constantTermsSLAU, int n, int numOfOperations, int rank, int size) {
	MPI_Status status;
	vector<vector<double>> basicMatrix, tempMatrix;
	vector<double> basicSolution, solution;
	vector<int> localCounts(size), offsets(size);
	int remainder = n % size, sum = 0;
	double mainDet, timeStart = 0, timeFinish = 0;
	for (int i = 0; i < size; i++) {
		localCounts[i] = n / size;
		if (remainder > 0) {
			localCounts[i] += 1;
			remainder--;
		}
		offsets[i] = sum;
		sum += localCounts[i];
	}
	solution.resize(localCounts[rank]);
	if (rank == 0)
		timeStart = MPI_Wtime();
	for (int i = 0; i < numOfOperations; i++) {
		if (rank == 0) {
			basicSolution.resize(n);
			basicMatrix = ñoefficientsSLAU;
			tempMatrix = ñoefficientsSLAU;
			mainDet = findDetByGauss(basicMatrix, n);
		}
		else {
			tempMatrix.resize(n);
			for (int j = 0; j < n; j++)
				tempMatrix[j].resize(n);
		}
		for (int j = 0; j < n; j++)
			MPI_Bcast(tempMatrix[j].data(), n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&mainDet, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		for (int j = offsets[rank]; j < offsets[rank] + localCounts[rank]; j++) {
			swapCoefficientsWithConsts(tempMatrix, constantTermsSLAU, j, n);
			vector<vector<double>> tempTriangleMatrix = tempMatrix;
			solution[j - offsets[rank]] = findDetByGauss(tempTriangleMatrix, n) / mainDet;
			swapCoefficientsWithConsts(tempMatrix, constantTermsSLAU, j, n);
		}
		MPI_Gatherv(solution.data(), localCounts[rank], MPI_DOUBLE, basicSolution.data(), localCounts.data(), offsets.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
		timeFinish = MPI_Wtime();
		cout << "n = " << n << ", MPI t = " << (timeFinish - timeStart) / numOfOperations << endl;
	}
}

int random(int a, int b)
{
	if (a > 0)
		return a + rand() % (b - a);
	else
		return a + rand() % (abs(a) + b);
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "Russian");
	int n = 20, rank, size, numOfOperations = 5;
	vector<vector<double>> ñoefficientsSLAU;
	vector<double> constantTermsSLAU;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	srand(time(NULL));
	do {
		ñoefficientsSLAU.resize(n);
		constantTermsSLAU.resize(n);
		for (int i = 0; i < n; i++) {
			ñoefficientsSLAU[i].resize(n);
			for (int j = 0; j < n; j++)
				ñoefficientsSLAU[i][j] = random(-30, 30);
			constantTermsSLAU[i] = random(-100, 100);
		}
		parallelMetodCramer(ñoefficientsSLAU, constantTermsSLAU, n, numOfOperations, rank, size);
		n += 20;
	} while (n <= 200);
	MPI_Finalize();

	return 0;
}


