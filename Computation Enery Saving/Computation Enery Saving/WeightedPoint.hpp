#ifndef WEIGHTED_POINT_H
#define WEIGHTED_POINT_H

struct WeightedPoint {

	_int64 pointWeight;
	int xCoordinate;
	int ycoordinate;

};

bool compareWeightedPoints(WeightedPoint p1, WeightedPoint p2);

#endif
