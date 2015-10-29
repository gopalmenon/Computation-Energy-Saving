#include "WeightedPoint.hpp"

bool compareWeightedPoints(WeightedPoint p1, WeightedPoint p2) {
	return p1.pointWeight < p2.pointWeight;
}