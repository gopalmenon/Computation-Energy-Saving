#ifndef REAL_MATRIX_H
#define REAL_MATRIX_H

#include <memory>
#include <vector>

typedef std::unique_ptr<double[]> realMatrixRow;

//Base class to hold a matrix with accessor method for elements
class RealMatrix {

protected:

	std::unique_ptr<realMatrixRow[]> matrixRows;
	int numberOfRows;
	int numberOfColumns;

	virtual void initializeMatrix() = 0;

public:

	RealMatrix(int rows, int columns);

	double getElementAt(int row, int column);

	void setElementAt(int row, int column, double);

	void showMatrix();

	virtual void multiply(RealMatrix& multiplicand) = 0;
};

//Class that holds a matrix that has elements populated by random values at initialization. 
//The randon population is done serially.
class SerialRealMatrix : public RealMatrix {

public:

	SerialRealMatrix(int rows, int columns, bool initialize);
	void multiply(SerialRealMatrix& multiplicand);

private:

	void initializeMatrix();

};

//Class that holds a matrix that has elements populated by random values at initialization. 
//The randon population is done in parallel.
class ParallelRealMatrix : public RealMatrix {

public:

	ParallelRealMatrix(int rows, int columns, bool initialize);
	void multiply(ParallelRealMatrix& multiplicand);

private:

	void initializeMatrix();

};

#endif
