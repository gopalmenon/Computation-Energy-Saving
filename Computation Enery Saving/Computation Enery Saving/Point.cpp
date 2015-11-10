#include "Point.hpp"

#include <cmath>

//Constructors

Point::Point() {
}

Point::Point(_int64 xCoordinate, _int64 yCoordinate) {
	this->xCoordinate = xCoordinate;
	this->yCoordinate = yCoordinate;
}

double Point::distanceFrom(Point otherPoint) {

	return pow(pow(this->xCoordinate - otherPoint.xCoordinate, 2) + pow(this->yCoordinate - otherPoint.yCoordinate, 2), 0.5);

}

void Point::setCoordinates(_int64 xCoordinate, _int64 yCoordinate) {
	this->xCoordinate = xCoordinate;
	this->yCoordinate = yCoordinate;
}

std::string Point::to_string() {
	std::string returnValue("(");
	return returnValue.append(std::to_string(this->xCoordinate)).append(", ").append(std::to_string(this->yCoordinate)).append(")");
}