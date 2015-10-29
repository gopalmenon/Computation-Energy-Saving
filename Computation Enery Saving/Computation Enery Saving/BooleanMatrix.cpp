#include "BooleanMatrix.hpp"
#include "RandomNumber.hpp"

#include <ctime>
#include <iostream>
#include <stdexcept>

//Define matrix base class constructor
BooleanMatrix::BooleanMatrix(int rows, int columns) {

	if (rows <= 0 || columns <= 0) {
		throw std::invalid_argument("Rows and columns must be non-zero and positive");
	}

	this->numberOfRows = rows;
	this->numberOfColumns = columns;
	this->matrixRows = std::unique_ptr<matrixBooleanRow[]>(new matrixBooleanRow[rows]);
	for (int rowCounter = 0; rowCounter < this->numberOfRows; ++rowCounter) {
		this->matrixRows[rowCounter] = matrixBooleanRow(new bool[columns]);
	}
}

//Element accessor
bool BooleanMatrix::getElementAt(int row, int column) {

	if (row > this->numberOfRows - 1 || column > this->numberOfColumns - 1) {
		throw std::invalid_argument("Row or column is outside valid range");
	}

	return this->matrixRows[row][column];

}

//Matrix viewer
void BooleanMatrix::showMatrix() {

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
			std::cout << getElementAt(rowCounter, columnCounter);
		}
		std::cout << "]" << std::endl;
	}

}

//Serial matrix constructor
SerialBooleanMatrix::SerialBooleanMatrix(int rows, int columns) : BooleanMatrix(rows, columns) {
	initializeMatrix();
}

//Initialize matrix elements in order
void SerialBooleanMatrix::initializeMatrix() {

	std::srand(std::time(0));
	for (int rowCounter = 0; rowCounter < this->numberOfRows; ++rowCounter) {
		for (int columnCounter = 0; columnCounter < this->numberOfColumns; ++columnCounter) {
			this->matrixRows[rowCounter][columnCounter] = getNextRandomBoolean();
		}
	}

}

//Parallel matrix constructor
ParallelBooleanMatrix::ParallelBooleanMatrix(int rows, int columns) : BooleanMatrix(rows, columns) {
	initializeMatrix();
}

//Initialize matrix elements in parallel
void ParallelBooleanMatrix::initializeMatrix() {




}
