#include "BooleanMatrix.hpp"
#include "Matrix.hpp"
#include "ParallelSort.hpp"
#include "PointMatrix.hpp"
#include "RandomNumber.hpp"
#include "RealMatrix.hpp"
#include "WeightedPoint.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

#include <tbb\blocked_range.h>
#include <tbb\parallel_for.h>
#include <tbb\parallel_reduce.h>

const int NUMBER_OF_ROWS = 10000, NUMBER_OF_COLUMNS = 11000;
const int QUIT_TEST = 0, RANDOM_NUMBER_GENERATION = 1, WEIGHTED_POINT_SELECTION = 2, OUTER_PRODUCT = 3, MATRIX_VECTOR_PRODUCT = 4;
std::chrono::milliseconds runTime1InMilliseconds, runTime2InMilliseconds;

void randomNumberGenerationSerial() {

	std::cout << "Serial Matrix" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	SerialMatrix serialMatrix(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS, true);
	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Serial Matrix initialization took " << runTime1InMilliseconds.count() << " ms." << std::endl;
	//serialMatrix.showMatrix();

}

void randomNumberGenerationParallel() {

	std::cout << "Parallel Matrix" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	ParallelMatrix parallelMatrix(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS, true);
	runTime2InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Parallel Matrix initialization took " << runTime2InMilliseconds.count() << " ms." << std::endl;
	std::cout << "Speedup was " << runTime1InMilliseconds.count() * 1.0 / runTime2InMilliseconds.count() << std::endl;
	//parallelMatrix.showMatrix();

}

void weightedPointSelectionSerial(int rows, int columns) {

	std::cout << "Weighted Points Selection - Serial" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	SerialMatrix integerWeights(rows, columns, true);
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

	std::cout << "Weighted Points Selection - Parallel" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	ParallelMatrix integerWeights(rows, columns, true);
	//integerWeights.showMatrix();
	ParallelBooleanMatrix eligiblePoints(rows, columns);
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

	//Parallel sort the weighted points by increasing weight
	SortableCollection weightedPointsCollection(weightedPoints);
	weightedPointsCollection.doSerialSort();

	//Randomly determine number of points to select
	int pointsToSelect = weightedPoints.size() * std::rand() / RAND_MAX;
	int offsetForPoints = weightedPoints.size() / pointsToSelect;

	//Do serial operration if less than 500 points
	std::unique_ptr<WeightedPoint[]> outputVector = std::unique_ptr<WeightedPoint[]>(new WeightedPoint[pointsToSelect]);
	if (pointsToSelect < 500) {
		//Vector to store output
		for (unsigned int currentOffset = 0, indexCounter = 0; currentOffset < weightedPoints.size(); currentOffset += offsetForPoints, ++indexCounter) {
			outputVector[indexCounter] = weightedPointsCollection.getData().at(currentOffset);
		}
	}
	else {

		tbb::parallel_for(
			tbb::blocked_range<int>(0, pointsToSelect),
			[=, &outputVector, &weightedPoints](tbb::blocked_range<int> range) {
			for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
				outputVector[indexCounter] = weightedPoints[offsetForPoints * indexCounter];
			}
		}
		);

	}

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Weighted Points Selection - Parallel took " << runTime1InMilliseconds.count() << " ms." << std::endl;

}

void outerProductSerial() {

	std::cout << "Outer Product - Serial" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	SerialPointMatrix pointVector(NUMBER_OF_ROWS, 1);

	//Fill interpoint distances off the diagonal
	SerialRealMatrix interPointDistances(NUMBER_OF_ROWS, NUMBER_OF_ROWS, false);
	double maximumDistance = 0.0, currentDistance;
	for (int rowCounter = 0; rowCounter < NUMBER_OF_ROWS; ++rowCounter) {
		for (int columnCounter = 0; columnCounter < NUMBER_OF_ROWS; ++columnCounter) {
			if (rowCounter != columnCounter) {
				currentDistance = pointVector.getElementAt(rowCounter, 0).distanceFrom(pointVector.getElementAt(columnCounter, 0));
				interPointDistances.setElementAt(rowCounter, columnCounter, currentDistance);
				if (currentDistance > maximumDistance) {
					maximumDistance = currentDistance;
				}
			}
		}
	}

	//Fill in diagonal values
	maximumDistance *= NUMBER_OF_ROWS;
	for (int rowCounter = 0; rowCounter < NUMBER_OF_ROWS; ++rowCounter) {
		interPointDistances.setElementAt(rowCounter, rowCounter, maximumDistance);
	}

	//Origin to point distances
	SerialRealMatrix originToPointDistances(NUMBER_OF_ROWS, 1, false);
	for (int rowCounter = 0; rowCounter < NUMBER_OF_ROWS; ++rowCounter) {
		originToPointDistances.setElementAt(rowCounter, 0, pointVector.getElementAt(rowCounter, 0).distanceFrom(Point(0, 0)));
	}

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Outer Product - Serial took " << runTime1InMilliseconds.count() << " ms." << std::endl;

}

void outerProductParallel() {

	std::cout << "Outer Product - Parallel" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	SerialPointMatrix pointVector(NUMBER_OF_ROWS, 1);

	//Fill interpoint distances off the diagonal
	SerialRealMatrix interPointDistances(NUMBER_OF_ROWS, NUMBER_OF_ROWS, false);
	tbb::parallel_for(
		tbb::blocked_range<int>(0, NUMBER_OF_ROWS * NUMBER_OF_ROWS),
		[=, &interPointDistances, &pointVector](tbb::blocked_range<int> range) {
		int row, column;
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			row = indexCounter / NUMBER_OF_ROWS;
			column = indexCounter % NUMBER_OF_ROWS;
			if (row != column) {
				interPointDistances.setElementAt(row, column, pointVector.getElementAt(row, 0).distanceFrom(pointVector.getElementAt(column, 0)));
			}
		}
	}
	);

	//Find the maximum off diagonal value
	double maximumDistance = 
		tbb::parallel_reduce(
			tbb::blocked_range<int>(0, NUMBER_OF_ROWS * NUMBER_OF_ROWS),
			double(0),
			[=, &interPointDistances](const tbb::blocked_range<int> range, double maximumDistance)-> double {
					int row, column;
					double candidateDistance;
					for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
						row = indexCounter / NUMBER_OF_ROWS;
						column = indexCounter % NUMBER_OF_ROWS;
						candidateDistance = interPointDistances.getElementAt(row, column);
						if (candidateDistance > maximumDistance) {
							maximumDistance = candidateDistance;
						}
					}
					return maximumDistance;
				},
			[=](double distance1, double distance2)->double {
				return std::max(distance1, distance2);
			}
		);


	//Fill in diagonal values
	maximumDistance *= NUMBER_OF_ROWS;
	tbb::parallel_for(
		tbb::blocked_range<int>(0, NUMBER_OF_ROWS),
		[=, &interPointDistances](tbb::blocked_range<int> range) {
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			interPointDistances.setElementAt(indexCounter, indexCounter, maximumDistance);
		}
	}
	);

	//Origin to point distances
	SerialRealMatrix originToPointDistances(NUMBER_OF_ROWS, 1, false);
	tbb::parallel_for(
		tbb::blocked_range<int>(0, NUMBER_OF_ROWS),
		[=, &originToPointDistances, &pointVector](tbb::blocked_range<int> range) {
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			originToPointDistances.setElementAt(indexCounter, 0, pointVector.getElementAt(indexCounter, 0).distanceFrom(Point(0, 0)));
		}
	}
	);

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Outer Product - Parallel took " << runTime1InMilliseconds.count() << " ms." << std::endl;

}

void matrixVectorProductSerial() {

	std::cout << "Matrix Product - Serial" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();	
	
	SerialRealMatrix multiplicand(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS, true);
	SerialRealMatrix multiplierVector(NUMBER_OF_COLUMNS, 1, true);
	multiplicand.multiply(multiplierVector);

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Matrix Product - Serial took " << runTime1InMilliseconds.count() << " ms." << std::endl;

}

void matrixVectorProductParallel() {

	std::cout << "Matrix Product - Parallel" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	ParallelRealMatrix multiplicand(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS, true);
	ParallelRealMatrix multiplierVector(NUMBER_OF_COLUMNS, 1, true);
	multiplicand.multiply(multiplierVector);

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Matrix Product - Parallel took " << runTime1InMilliseconds.count() << " ms." << std::endl;

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
