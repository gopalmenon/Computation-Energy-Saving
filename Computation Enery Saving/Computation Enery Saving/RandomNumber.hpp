#ifndef RANDOM_NUMBER_H
#define RANDOM_NUMBER_H

#include <cmath>

//Constants
const auto MODULUS = 4294967296;//=2^32
const auto MULTIPLIER = 1664525;
const auto INCREMENT = 1013904223;

//Next randowm number generator based on linear congruential generator
_int64 getNextRandomNumber(_int64 currentNumber);

#endif
