#ifndef POINTS_VECTOR_H
#define POINTS_VECTOR_H

#include "Point.hpp"

#include <memory>

class PointsVector {

protected:

	std::unique_ptr<Point[]> points;

	int numberOfPoints;

	virtual void initializePoints() = 0;

public:

	PointsVector(int numberOfPoints);

	Point getElementAt(int index);

};

class SerialPointsVector : public PointsVector {

public:

	SerialPointsVector(int numberOfPoints);

private:

	void initializePoints();

};

class ParallelPointsVector : public PointsVector {

public:

	ParallelPointsVector(int numberOfPoints);

private:

	void initializePoints();

};

#endif
