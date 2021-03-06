#include "RealMatrix.hpp"
#include "RandomNumber.hpp"

#include <iostream>
#include <stdexcept>

#include <tbb\blocked_range.h>
#include <tbb\parallel_for.h>

//Define matrix base class constructor
RealMatrix::RealMatrix(int rows, int columns) {

	if (rows <= 0 || columns <= 0) {
		throw std::invalid_argument("Rows and columns must be non-zero and positive");
	}

	this->numberOfRows = rows;
	this->numberOfColumns = columns;
	this->matrixRows = std::unique_ptr<realMatrixRow[]>(new realMatrixRow[rows]);
	for (int rowCounter = 0; rowCounter < this->numberOfRows; ++rowCounter) {
		this->matrixRows[rowCounter] = realMatrixRow(new double[columns]);
	}
}

//Element accessor
double RealMatrix::getElementAt(int row, int column) {

	if (row > this->numberOfRows - 1 || column > this->numberOfColumns - 1) {
		throw std::invalid_argument("Row or column is outside valid range");
	}

	return this->matrixRows[row][column];

}

//Element mutator
void RealMatrix::setElementAt(int row, int column, double elementValue) {

	if (row > this->numberOfRows - 1 || column > this->numberOfColumns - 1) {
		throw std::invalid_argument("Row or column is outside valid range");
	}

	this->matrixRows[row][column] = elementValue;

}

//Matrix viewer
void RealMatrix::showMatrix() {

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

//Get the number of rows
int RealMatrix::getNumberOfRows() {
	return this->numberOfRows;
}

//Get the number of columns
int RealMatrix::getNumberOfColumns() {
	return this->numberOfColumns;
}

//Check if the multiplier is valid 
bool RealMatrix::isValidMultiplier(RealMatrix& multiplier) {

	if (this->numberOfColumns == multiplier.getNumberOfRows()) {
		return true;
	}
	else {
		return false;
	}

}

//Serial matrix constructor
SerialRealMatrix::SerialRealMatrix(int rows, int columns, bool initialize) : RealMatrix(rows, columns) {
	if (initialize) {
		initializeMatrix();
	}
}

//Initialize matrix elements in order
void SerialRealMatrix::initializeMatrix() {

	for (int rowCounter = 0; rowCounter < this->numberOfRows; ++rowCounter) {
		for (int columnCounter = 0; columnCounter < this->numberOfColumns; ++columnCounter) {
			this->matrixRows[rowCounter][columnCounter] = getNextRandomReal(columnCounter);
		}
	}

}

//Multiply with another matrix
RealMatrix& SerialRealMatrix::multiply(RealMatrix& multiplier) {

	//Check if the multiplier is valid
	if (!isValidMultiplier(multiplier)) {
		throw std::invalid_argument("Invalid multiplier size");
	}

	int returnNumberOfRows = this->numberOfRows, returnNumberOfColumns = multiplier.getNumberOfColumns();
	SerialRealMatrix returnValue(returnNumberOfRows, returnNumberOfColumns, false);

	//Fill the return matrix
	int components = this->numberOfColumns;
	double elementValue;
	for (int rowCounter = 0; rowCounter < returnNumberOfRows; ++rowCounter) {
		for (int columnCounter = 0; columnCounter < returnNumberOfColumns; ++columnCounter) {
			elementValue = 0.0;
			for (int componentCounter = 0; componentCounter < components; ++componentCounter) {
				elementValue += this->getElementAt(rowCounter, componentCounter) * multiplier.getElementAt(componentCounter, columnCounter);
			}
			returnValue.setElementAt(rowCounter, columnCounter, elementValue);
		}
	}

	return returnValue;
}

//Parallel matrix constructor
ParallelRealMatrix::ParallelRealMatrix(int rows, int columns, bool initialize) : RealMatrix(rows, columns) {
	if (initialize) {
		initializeMatrix();
	}
}

//Initialize matrix elements in parallel
void ParallelRealMatrix::initializeMatrix() {

	tbb::parallel_for(
		tbb::blocked_range<int>(0, this->numberOfRows * this->numberOfColumns),
		[=](tbb::blocked_range<int> range) {
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			this->matrixRows[indexCounter / (this->numberOfColumns)][indexCounter % (this->numberOfColumns)] = getNextRandomReal(indexCounter);
		}
	}
	);

}

//Multiply with another matrix
RealMatrix& ParallelRealMatrix::multiply(RealMatrix& multiplier) {

	//Check if the multiplier is valid
	if (!isValidMultiplier(multiplier)) {
		throw std::invalid_argument("Invalid multiplier size");
	}

	int returnNumberOfRows = this->numberOfRows, returnNumberOfColumns = multiplier.getNumberOfColumns();
	ParallelRealMatrix returnValue(returnNumberOfRows, returnNumberOfColumns, false);

	int components = this->numberOfColumns;

	//Fill the return matrix
	tbb::parallel_for(
		tbb::blocked_range<int>(0, returnNumberOfRows * returnNumberOfColumns),
		[=, &multiplier](tbb::blocked_range<int> range) {
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			double elementValue = 0.0;
			int elementRow = indexCounter / returnNumberOfColumns, elementColumn = indexCounter % returnNumberOfColumns;

			for (int componentCounter = 0; componentCounter < components; ++componentCounter) {
				elementValue += this->getElementAt(elementRow, componentCounter) * multiplier.getElementAt(componentCounter, elementColumn);
			}

			this->matrixRows[indexCounter / (this->numberOfColumns)][indexCounter % (this->numberOfColumns)] = elementValue;
		}
	}
	);

	return returnValue;
}
