#include "Matrix.hpp"

#include <stdexcept>

//Define matrix base class constructor
Matrix::Matrix(int rows, int columns) {

	if (rows <= 0 || columns <= 0) {
		throw std::invalid_argument("Rows and columns must be non-zero and positive");
	}

	this->numberOfRows = rows;
	this->numberOfColumns = columns;
	std::vector<std::vector<int>> matrixRows(rows, std::vector<int>(columns));

}

//Element accessor
int Matrix::getElementAt(int row, int column) {

	if (row > this->numberOfRows - 1 || column > this->numberOfColumns - 1) {
		throw std::invalid_argument("Row or column is outside valid range");
	}

	return this->matrixRows.at(row).at(column);

}

//Serial matrix constructor
SerialMatrix::SerialMatrix(int rows, int columns) : Matrix(rows, columns) {
	initializeMatrix();
}

//Initialize matrix elements in order
void SerialMatrix::initializeMatrix() {

}

//Parallel matrix constructor
ParallelMatrix::ParallelMatrix(int rows, int columns) : Matrix(rows, columns) {
	initializeMatrix();
}

//Initialize matrix elements in parallel
void ParallelMatrix::initializeMatrix() {

}
