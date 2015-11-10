#ifndef POINT_MATRIX_H
#define POINT_MATRIX_H

#include <memory>
#include <vector>

#include "Point.hpp"
typedef std::unique_ptr<Point[]> matrixPointRow;

//Base class to hold a matrix with accessor method for elements
class PointMatrix {

protected:

	std::unique_ptr<matrixPointRow[]> matrixRows;
	int numberOfRows;
	int numberOfColumns;

	virtual void initializeMatrix() = 0;

public:

	PointMatrix(int rows, int columns);

	Point getElementAt(int row, int column);

	void setElementAt(int row, int column, Point point);

	void showMatrix();
};

class SerialPointMatrix : public PointMatrix {

public:

	SerialPointMatrix(int rows, int columns);

private:

	void initializeMatrix();

};


class ParallelPointMatrix : public PointMatrix {

public:

	ParallelPointMatrix(int rows, int columns);

private:

	void initializeMatrix();

};

#endif
