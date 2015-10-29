#ifndef OUTER_PRODUCT_H
#define OUTER_PRODUCT_H

#include "Matrix.hpp"
#include "PointsVector.hpp"

SerialMatrix getInterPointDistancesSeries(SerialPointsVector pointsVector);

ParallelMatrix getInterPointDistancesParallel(ParallelPointsVector pointsVector);

SerialPointsVector getOriginToPointDistancesSeries(SerialPointsVector pointsVector);

ParallelPointsVector getOriginToPointDistancesParallel(ParallelPointsVector pointsVector);

#endif
