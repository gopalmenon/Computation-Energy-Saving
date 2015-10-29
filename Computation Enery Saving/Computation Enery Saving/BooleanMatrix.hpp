#ifndef BOOLEAN_MATRIX_H
#define BOOLEAN_MATRIX_H

#include <memory>
#include <vector>

typedef std::unique_ptr<bool[]> matrixBooleanRow;

//Base class to hold a matrix with accessor method for elements
class BooleanMatrix {

protected:

	std::unique_ptr<matrixBooleanRow[]> matrixRows;
	int numberOfRows;
	int numberOfColumns;

	virtual void initializeMatrix() = 0;

public:

	BooleanMatrix(int rows, int columns);

	bool getElementAt(int row, int column);

	void showMatrix();
};

//Class that holds a matrix that has elements populated by random values at initialization. 
//The randon population is done serially.
class SerialBooleanMatrix : public BooleanMatrix {

public:

	SerialBooleanMatrix(int rows, int columns);

private:

	void initializeMatrix();

};

//Class that holds a matrix that has elements populated by random values at initialization. 
//The randon population is done in parallel.
class ParallelBooleanMatrix : public BooleanMatrix {

public:

	ParallelBooleanMatrix(int rows, int columns);

private:

	void initializeMatrix();

};

#endif
