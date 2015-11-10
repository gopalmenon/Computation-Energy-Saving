#ifndef POINT_H
#define POINT_H

#include <string>

class Point {

private:

	_int64 xCoordinate;
	_int64 yCoordinate;

public:

	Point();

	Point(_int64 xCoordinate, _int64 yCoordinate);

	double distanceFrom(Point otherPoint);

	void setCoordinates(_int64 xCoordinate, _int64 yCoordinate);

	std::string to_string();

};

#endif
