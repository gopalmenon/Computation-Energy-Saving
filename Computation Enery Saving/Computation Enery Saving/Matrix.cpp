#include "Matrix.hpp"
#include "RandomNumber.hpp"

#include <iostream>
#include <stdexcept>

//Define matrix base class constructor
Matrix::Matrix(int rows, int columns) {

	if (rows <= 0 || columns <= 0) {
		throw std::invalid_argument("Rows and columns must be non-zero and positive");
	}

	this->numberOfRows = rows;
	this->numberOfColumns = columns;
	std::vector<std::vector<_int64>> matrixRows(rows, std::vector<_int64>(columns));

}

//Element accessor
_int64 Matrix::getElementAt(int row, int column) {

	if (row > this->numberOfRows - 1 || column > this->numberOfColumns - 1) {
		throw std::invalid_argument("Row or column is outside valid range");
	}

	return this->matrixRows.at(row).at(column);

}

//Matrix viewer
void Matrix::showMatrix() {

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
			std::cout << this->getElementAt(rowCounter, columnCounter);
		}
		std::cout << "]" << std::endl;
	}

}

//Serial matrix constructor
SerialMatrix::SerialMatrix(int rows, int columns) : Matrix(rows, columns) {
	initializeMatrix();
}

//Initialize matrix elements in order
void SerialMatrix::initializeMatrix() {

	_int64 seedValue = 1;
	for (int rowCounter = 0; rowCounter < this->numberOfRows; ++rowCounter) {
		std::vector<_int64> currentRow;
		for (int columnCounter = 0; columnCounter < this->numberOfColumns; ++columnCounter) {
			seedValue = getNextRandomNumber(seedValue);
			currentRow.push_back(seedValue);
		}
		matrixRows.push_back(currentRow);
	}

}

//Parallel matrix constructor
ParallelMatrix::ParallelMatrix(int rows, int columns) : Matrix(rows, columns) {
	initializeMatrix();
}

//Initialize matrix elements in parallel
void ParallelMatrix::initializeMatrix() {

}
