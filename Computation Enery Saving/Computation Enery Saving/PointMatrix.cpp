#include "Point.hpp"
#include "PointMatrix.hpp"
#include "RandomNumber.hpp"

#include <ctime>
#include <iostream>
#include <stdexcept>

#include <tbb\blocked_range.h>
#include <tbb\parallel_for.h>

//Define matrix base class constructor
PointMatrix::PointMatrix(int rows, int columns) {

	if (rows <= 0 || columns <= 0) {
		throw std::invalid_argument("Rows and columns must be non-zero and positive");
	}

	this->numberOfRows = rows;
	this->numberOfColumns = columns;
	this->matrixRows = std::unique_ptr<matrixPointRow[]>(new matrixPointRow[rows]);
	for (int rowCounter = 0; rowCounter < this->numberOfRows; ++rowCounter) {
		this->matrixRows[rowCounter] = matrixPointRow(new Point[columns]);
	}
}

//Element accessor
Point PointMatrix::getElementAt(int row, int column) {

	if (row > this->numberOfRows - 1 || column > this->numberOfColumns - 1) {
		throw std::invalid_argument("Row or column is outside valid range");
	}

	return this->matrixRows[row][column];

}

//Element mutator
void PointMatrix::setElementAt(int row, int column, Point point) {

	if (row > this->numberOfRows - 1 || column > this->numberOfColumns - 1) {
		throw std::invalid_argument("Row or column is outside valid range");
	}

	this->matrixRows[row][column] = point;

}

//Matrix viewer
void PointMatrix::showMatrix() {

	bool firstRowElement;
	for (int rowCounter = 0; rowCounter < this->numberOfRows; ++rowCounter) {
		firstRowElement = true;
		for (int columnCounter = 0; columnCounter < this->numberOfColumns; ++columnCounter) {
			if (firstRowElement) {
				std::cout << "[";
				firstRowElement = false;
			}
			else {
				std::cout << ", ";
			}
			std::cout << getElementAt(rowCounter, columnCounter).to_string();
		}
		std::cout << "]" << std::endl;
	}
}

//Serial matrix constructor
SerialPointMatrix::SerialPointMatrix(int rows, int columns) : PointMatrix(rows, columns) {
	initializeMatrix();
}

//Initialize matrix elements in order
void SerialPointMatrix::initializeMatrix() {

	std::srand(std::time(0));
	for (int rowCounter = 0; rowCounter < this->numberOfRows; ++rowCounter) {
		for (int columnCounter = 0; columnCounter < this->numberOfColumns; ++columnCounter) {
			this->matrixRows[rowCounter][columnCounter] = Point(rand(), rand());
		}
	}

}

//Parallel matrix constructor
ParallelPointMatrix::ParallelPointMatrix(int rows, int columns) : PointMatrix(rows, columns) {
	initializeMatrix();
}

//Initialize matrix elements in parallel
void ParallelPointMatrix::initializeMatrix() {

	tbb::parallel_for(
		tbb::blocked_range<int>(0, this->numberOfRows * this->numberOfColumns),
		[=](tbb::blocked_range<int> range) {
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			this->matrixRows[indexCounter / (this->numberOfColumns)][indexCounter % (this->numberOfColumns)] = Point(rand(), rand());
		}
	}
	);

}
