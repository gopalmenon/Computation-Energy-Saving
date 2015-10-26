#ifndef MATRIX_X
#define MATRIX_X

#include <memory>
#include <vector>

//Base class to hold a matrix with accessor method for elements
class Matrix {

protected:
	
	std::vector<std::vector<_int64>> matrixRows;
	//std::unique_ptr<std::unique_ptr<std::array<int, 3>>> matrixRows;
	int numberOfRows;
	int numberOfColumns;

	virtual void initializeMatrix() = 0;

public:

	Matrix(int rows, int columns);

	_int64 getElementAt(int row, int column);

	void showMatrix();
};

//Class that holds a matrix that has elements populated by random values at initialization. 
//The randon population is done serially.
class SerialMatrix : public Matrix {

public:

	SerialMatrix(int rows, int columns);

private:

	void initializeMatrix();

};

//Class that holds a matrix that has elements populated by random values at initialization. 
//The randon population is done in parallel.
class ParallelMatrix : public Matrix {

public:

	ParallelMatrix(int rows, int columns);

private:

	void initializeMatrix();

};

#endif
