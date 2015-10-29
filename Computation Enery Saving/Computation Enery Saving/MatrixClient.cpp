#include "BooleanMatrix.hpp"
#include "Matrix.hpp"
#include "RandomNumber.hpp"
#include "WeightedPoint.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>

const int NUMBER_OF_ROWS = 5000, NUMBER_OF_COLUMNS = 5100;
const int QUIT_TEST = 0, RANDOM_NUMBER_GENERATION = 1, WEIGHTED_POINT_SELECTION = 2, OUTER_PRODUCT = 3, MATRIX_VECTOR_PRODUCT = 4;
std::chrono::milliseconds runTime1InMilliseconds, runTime2InMilliseconds;

void randomNumberGenerationSerial() {

	std::cout << "Serial Matrix" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	SerialMatrix serialMatrix(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Serial Matrix initialization took " << runTime1InMilliseconds.count() << " ms." << std::endl;
	//serialMatrix.showMatrix();

}

void randomNumberGenerationParallel() {

	std::cout << "Parallel Matrix" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	ParallelMatrix parallelMatrix(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	runTime2InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Parallel Matrix initialization took " << runTime2InMilliseconds.count() << " ms." << std::endl;
	std::cout << "Speedup was " << runTime1InMilliseconds.count() * 1.0 / runTime2InMilliseconds.count() << std::endl;
	//parallelMatrix.showMatrix();

}

void weightedPointSelectionSerial(int rows, int columns) {

	std::cout << "Weighted Points Selection - Serial" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	SerialMatrix integerWeights(rows, columns);
	//integerWeights.showMatrix();
	SerialBooleanMatrix eligiblePoints(rows, columns);
	//eligiblePoints.showMatrix();

	//Assume that 50% of the integer weights are selected
	std::vector<WeightedPoint> weightedPoints(rows * columns / 50);

	//Fill the vector with weighted points
	for (int rowCounter = 0; rowCounter < rows; ++rowCounter) {
		for (int columnCounter = 0; columnCounter < columns; ++columnCounter) {
			if (eligiblePoints.getElementAt(rowCounter, columnCounter)) {
				weightedPoints.push_back({ integerWeights.getElementAt(rowCounter, columnCounter), rowCounter, columnCounter });
			}
		}
	}

	//Sort the weighted points by increasing weight
	std::sort(weightedPoints.begin(), weightedPoints.end(), compareWeightedPoints);

	//Randomly determine number of points to select
	int pointsToSelect = weightedPoints.size() * std::rand() / RAND_MAX;
	int offsetForPoints = weightedPoints.size() / pointsToSelect;

	//Vector to store output
	std::vector<WeightedPoint> outputVector(pointsToSelect);
	for (unsigned int currentOffset = 0; currentOffset < weightedPoints.size(); currentOffset += offsetForPoints) {
		outputVector.push_back(weightedPoints.at(currentOffset));
	}

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Weighted Points Selection - Serial took " << runTime1InMilliseconds.count() << " ms." << std::endl;


}

void weightedPointSelectionParallel(int rows, int columns) {


}

void outerProductSerial() {

}

void outerProductParallel() {

}

void matrixVectorProductSerial() {

}

void matrixVectorProductParallel() {

}

int main() {

	//Ask user to select what to test for
	std::cout << "What do you want to test:" << std::endl;
	std::cout << RANDOM_NUMBER_GENERATION << "=Random Number Generation, " << WEIGHTED_POINT_SELECTION << "=Weighted Point Selection, " << OUTER_PRODUCT << "=Outer Product," << std::endl;
	std::cout << MATRIX_VECTOR_PRODUCT << "=Matrix Vector Product or " << QUIT_TEST << "=Quit" << std::endl;

	int userSelection;

	//Get a valid input from the user
	while (true) {
		std::cin >> userSelection;
		if (userSelection == QUIT_TEST || userSelection == RANDOM_NUMBER_GENERATION || userSelection == WEIGHTED_POINT_SELECTION || userSelection == OUTER_PRODUCT || userSelection == MATRIX_VECTOR_PRODUCT) {
			break;
		}
		else {
			std::cout << "Invalid option, try again." << std::endl;
		}
	}

	if (userSelection == QUIT_TEST) {
		return 0;
	}
	else if (userSelection == RANDOM_NUMBER_GENERATION) {
		randomNumberGenerationSerial();
		randomNumberGenerationParallel();
	}
	else if (userSelection == WEIGHTED_POINT_SELECTION) {
		weightedPointSelectionSerial(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
		weightedPointSelectionParallel(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	}
	else if (userSelection == OUTER_PRODUCT) {
		outerProductSerial();
		outerProductParallel();
	}
	else if (userSelection == MATRIX_VECTOR_PRODUCT) {
		matrixVectorProductSerial();
		matrixVectorProductParallel();
	}

}
