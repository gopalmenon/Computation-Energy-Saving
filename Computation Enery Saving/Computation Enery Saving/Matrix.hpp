#ifndef MATRIX_X
#define MATRIX_X

//Base class to hold a matrix with accessor method for elements
class Matrix {

protected:
	
	int[][] matrxiElement;
	int numberOfRows;
	int numberOfColumns;

public:

	virtual Matrx(int rows, int columns) = 0;

	virtual int getElementAt(int row, int column) = 0;

};

//Class that holds a matrix that has elements populated by random values at initialization. 
//The randon population is done serially.
class SerialMatrix : public Matrix {

};

//Class that holds a matrix that has elements populated by random values at initialization. 
//The randon population is done in parallel.
class ParallelMatrix : public Matrix {


};

#endif