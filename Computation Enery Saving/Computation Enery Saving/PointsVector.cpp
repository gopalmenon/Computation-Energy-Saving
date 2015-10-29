#include "Point.hpp"
#include "PointsVector.hpp"
#include "RandomNumber.hpp"

#include <memory>
#include <stdexcept>

#include <tbb\blocked_range.h>
#include <tbb\parallel_for.h>

PointsVector::PointsVector(int numberOfPoints) {

	if (numberOfPoints <= 0) {
		throw std::invalid_argument("Number of points must be non-zero and positive");
	}

	this->numberOfPoints = numberOfPoints;

	this->points = std::unique_ptr<Point[]>(new Point[numberOfPoints]);

}

Point PointsVector::getElementAt(int index) {

	if (index > this->numberOfPoints - 1) {
		throw std::invalid_argument("Index is outside valid range.");
	}

	return this->points[index];

}

//Serial points vector constructor
SerialPointsVector::SerialPointsVector(int numberOfPoints) : PointsVector(numberOfPoints) {
	initializePoints();
}

void SerialPointsVector::initializePoints() {

	_int64 xSeedValue = 1;
	_int64 ySeedValue = 2;
	for (int index = 0; index < this->numberOfPoints; ++index) {
		xSeedValue = getNextRandomNumber(xSeedValue);
		ySeedValue = getNextRandomNumber(ySeedValue);
		this->points[index].setCoordinates(xSeedValue, ySeedValue);
	}
}

//Parallel points vector constructor
ParallelPointsVector::ParallelPointsVector(int numberOfPoints) : PointsVector(numberOfPoints) {
	initializePoints();
}

void ParallelPointsVector::initializePoints() {

	tbb::parallel_for(
		tbb::blocked_range<int>(0, this->numberOfPoints),
		[=](tbb::blocked_range<int> range) {
		for (int indexCounter = range.begin(); indexCounter != range.end(); ++indexCounter) {
			_int64 xCoordinate = getNextRandomNumber(indexCounter);
			_int64 yCoordinate = getNextRandomNumber(xCoordinate);
			this->points[indexCounter].setCoordinates(xCoordinate, yCoordinate);
		}
	}
	);

}
