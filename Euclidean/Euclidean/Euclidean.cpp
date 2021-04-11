#include <iostream>
#include <iomanip>
#include <random>
#include <utility>
#include <chrono>
#include <string>
using namespace std;
double randomNumbers() {
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(1.0, 2.0);
	double num = dis(gen);
	return num;
}
void crearMatrix(double** matriz10k, int sizeObj, int sizeVector) {

	for (int i = 0; i < sizeObj; i++) {
		for (int j = 0; j < sizeVector; j++) {
			matriz10k[i][j] = randomNumbers();
		}
	}
}
void printMatrix(double** matriz10k, int sizeObj, int sizeVector) {

	for (int i = 0; i < sizeObj; i++) {
		for (int j = 0; j < sizeVector; j++) {

			cout << matriz10k[i][j] << " ";
		}
		cout << endl;
	}
}
void distEuclidiana(double** matriz10k, int sizeObj, int sizeVector) {

	double acumulador = 0;
	double dist = 0;
	for (int i = 0; i < sizeObj - 1; i++) {
		for (int j = 0; j < sizeObj - i - 1; j++) {
			acumulador = 0;

			for (int k = 0; k < sizeVector; k++) {
				dist = pow((matriz10k[i][k]) - (matriz10k[j + 1][k]), 2);
				acumulador += dist;
			}
			double distEucli = sqrt(acumulador);
			//cout <<"rpta "<< distEucli << endl;
		}
	}
}

int main()
{
	
	int sizeObj3 = 10000;
	int sizeVec3 = 10;
	double** matrizk1 = new double* [sizeObj3];
	for (int i = 0; i < sizeObj3; i++)
		matrizk1[i] = new double[sizeVec3];
	crearMatrix(matrizk1, sizeObj3, sizeVec3);
	//printMatrix(matriz10k, sizeObj, sizeVec);


	std::chrono::time_point<std::chrono::high_resolution_clock> start1, end1;
	start1 = std::chrono::high_resolution_clock::now();
	distEuclidiana(matrizk1, sizeObj3, sizeVec3);
	end1 = std::chrono::high_resolution_clock::now(); int64_t duration1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end1 - start1).count();
	cout << "Time de 10k con vectores de size 10 ---" + std::to_string(duration1) + "ns\n";


	return 0;
}
