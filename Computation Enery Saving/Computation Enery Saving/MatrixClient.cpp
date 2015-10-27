#include "Matrix.hpp"
#include "RandomNumber.hpp"

#include <chrono>
#include <iostream>

int main() {

	const int NUMBER_OF_ROWS = 10000, NUMBER_OF_COLUMNS = 11000;

	std::cout << "Serial Matrix" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	SerialMatrix serialMatrix(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	std::chrono::milliseconds runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Serial Matrix initialization took " << runTime1InMilliseconds.count() << " ms."<< std::endl;
	//serialMatrix.showMatrix();

	std::cout << "Parallel Matrix" << std::endl;
	start = std::chrono::high_resolution_clock::now();
	ParallelMatrix parallelMatrix(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	std::chrono::milliseconds runTime2InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Parallel Matrix initialization took " << runTime2InMilliseconds.count() << " ms." << std::endl;
	std::cout << "Speedup was " << runTime1InMilliseconds.count() * 1.0 / runTime2InMilliseconds.count() << std::endl;
	//parallelMatrix.showMatrix();
}