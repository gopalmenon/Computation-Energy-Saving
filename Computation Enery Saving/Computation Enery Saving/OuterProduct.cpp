#include "OuterProduct.hpp"

SerialMatrix getInterPointDistancesSeries(SerialPointsVector pointsVector) {
	return SerialMatrix(1, 1);
}

ParallelMatrix getInterPointDistancesParallel(ParallelPointsVector pointsVector) {
	return ParallelMatrix(1, 1);
}

SerialPointsVector getOriginToPointDistancesSeries(SerialPointsVector pointsVector) {
	return SerialPointsVector(1);
}

ParallelPointsVector getOriginToPointDistancesParallel(ParallelPointsVector pointsVector) {
	return ParallelPointsVector(1);
}