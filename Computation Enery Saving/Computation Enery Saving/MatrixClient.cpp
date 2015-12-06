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
#include <fstream>
#include <string>

#include <tbb\blocked_range.h>
#include <tbb\parallel_for.h>
#include <tbb\parallel_reduce.h>

const int QUIT_TEST = 0, RANDOM_NUMBER_GENERATION = 1, WEIGHTED_POINT_SELECTION = 2, OUTER_PRODUCT = 3, MATRIX_VECTOR_PRODUCT = 4;
std::chrono::milliseconds runTime1InMilliseconds, runTime2InMilliseconds;

void randomNumberGenerationSerial(int numberOfRows, int numberOfColumns) {

	std::cout << "Serial Matrix" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	SerialMatrix serialMatrix(numberOfRows, numberOfColumns, true);
	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Serial Matrix initialization took " << runTime1InMilliseconds.count() << " ms." << std::endl;
	//serialMatrix.showMatrix();

}

void randomNumberGenerationParallel(int numberOfRows, int numberOfColumns) {

	std::cout << "Parallel Matrix" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	ParallelMatrix parallelMatrix(numberOfRows, numberOfColumns, true);
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

void outerProductSerial(int numberOfRows) {

	std::cout << "Outer Product - Serial" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	SerialPointMatrix pointVector(numberOfRows, 1);

	//Fill interpoint distances off the diagonal
	SerialRealMatrix interPointDistances(numberOfRows, numberOfRows, false);
	double maximumDistance = 0.0, currentDistance;
	for (int rowCounter = 0; rowCounter < numberOfRows; ++rowCounter) {
		for (int columnCounter = 0; columnCounter < numberOfRows; ++columnCounter) {
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
	maximumDistance *= numberOfRows;
	for (int rowCounter = 0; rowCounter < numberOfRows; ++rowCounter) {
		interPointDistances.setElementAt(rowCounter, rowCounter, maximumDistance);
	}

	//Origin to point distances
	SerialRealMatrix originToPointDistances(numberOfRows, 1, false);
	for (int rowCounter = 0; rowCounter < numberOfRows; ++rowCounter) {
		originToPointDistances.setElementAt(rowCounter, 0, pointVector.getElementAt(rowCounter, 0).distanceFrom(Point(0, 0)));
	}

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Outer Product - Serial took " << runTime1InMilliseconds.count() << " ms." << std::endl;

}

void outerProductParallel(int numberOfRows) {

	std::cout << "Outer Product - Parallel" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	SerialPointMatrix pointVector(numberOfRows, 1);

	//Fill interpoint distances off the diagonal
	SerialRealMatrix interPointDistances(numberOfRows, numberOfRows, false);
	tbb::parallel_for(
		tbb::blocked_range<int>(0, numberOfRows * numberOfRows),
		[=, &interPointDistances, &pointVector](tbb::blocked_range<int> range) {
		int row, column;
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			row = indexCounter / numberOfRows;
			column = indexCounter % numberOfRows;
			if (row != column) {
				interPointDistances.setElementAt(row, column, pointVector.getElementAt(row, 0).distanceFrom(pointVector.getElementAt(column, 0)));
			}
		}
	}
	);

	//Find the maximum off diagonal value
	double maximumDistance = 
		tbb::parallel_reduce(
			tbb::blocked_range<int>(0, numberOfRows * numberOfRows),
			double(0),
			[=, &interPointDistances](const tbb::blocked_range<int> range, double maximumDistance)-> double {
					int row, column;
					double candidateDistance;
					for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
						row = indexCounter / numberOfRows;
						column = indexCounter % numberOfRows;
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
	maximumDistance *= numberOfRows;
	tbb::parallel_for(
		tbb::blocked_range<int>(0, numberOfRows),
		[=, &interPointDistances](tbb::blocked_range<int> range) {
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			interPointDistances.setElementAt(indexCounter, indexCounter, maximumDistance);
		}
	}
	);

	//Origin to point distances
	SerialRealMatrix originToPointDistances(numberOfRows, 1, false);
	tbb::parallel_for(
		tbb::blocked_range<int>(0, numberOfRows),
		[=, &originToPointDistances, &pointVector](tbb::blocked_range<int> range) {
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			originToPointDistances.setElementAt(indexCounter, 0, pointVector.getElementAt(indexCounter, 0).distanceFrom(Point(0, 0)));
		}
	}
	);

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Outer Product - Parallel took " << runTime1InMilliseconds.count() << " ms." << std::endl;

}

void matrixVectorProductSerial(int numberOfRows, int numberOfColumns) {

	std::cout << "Matrix Product - Serial" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();	
	
	SerialRealMatrix multiplicand(numberOfRows, numberOfColumns, true);
	SerialRealMatrix multiplierVector(numberOfColumns, 1, true);
	multiplicand.multiply(multiplierVector);

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Matrix Product - Serial took " << runTime1InMilliseconds.count() << " ms." << std::endl;

}

void matrixVectorProductParallel(int numberOfRows, int numberOfColumns) {

	std::cout << "Matrix Product - Parallel" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	ParallelRealMatrix multiplicand(numberOfRows, numberOfColumns, true);
	ParallelRealMatrix multiplierVector(numberOfColumns, 1, true);
	multiplicand.multiply(multiplierVector);

	runTime1InMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	std::cout << "Matrix Product - Parallel took " << runTime1InMilliseconds.count() << " ms." << std::endl;

}

int main() {
	/*
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
	*/
	int lineNumber = 0, numberOfRows, numberOfColumns, userSelection, serialMode;
	std::string line;
	std::ifstream paramsFile("params.txt");
	if (paramsFile.is_open()) {
		while (getline(paramsFile, line)) {
			switch (++lineNumber) {

			case 1: 
				numberOfRows = std::stoi(line);
				break;

			case 2:
				numberOfColumns = std::stoi(line);
				break;

			case 3:
				userSelection = std::stoi(line);
				break;

			case 4:
				serialMode = std::stoi(line);
				break;

			default:
				break;
			}
		}
		paramsFile.close();
	}

	else {
		std::cout << "Unable to open file";
		return 0;
	}

	if (userSelection == QUIT_TEST) {
		return 0;
	}
	else if (userSelection == RANDOM_NUMBER_GENERATION) {
		if (serialMode == 1) {
			randomNumberGenerationSerial(numberOfRows, numberOfColumns);
		}
		else {
			randomNumberGenerationParallel(numberOfRows, numberOfColumns);
		}
	}
	else if (userSelection == WEIGHTED_POINT_SELECTION) {
		if (serialMode == 1) {
			weightedPointSelectionSerial(numberOfRows, numberOfColumns);
		}
		else {
			weightedPointSelectionParallel(numberOfRows, numberOfColumns);
		}
	}
	else if (userSelection == OUTER_PRODUCT) {
		if (serialMode == 1) {
			outerProductSerial(numberOfRows);
		}
		else {
			outerProductParallel(numberOfRows);
		}
	}
	else if (userSelection == MATRIX_VECTOR_PRODUCT) {
		if (serialMode == 1) {
			matrixVectorProductSerial(numberOfRows, numberOfColumns);
		}
		else {
			matrixVectorProductParallel(numberOfRows, numberOfColumns);
		}
	}

}
