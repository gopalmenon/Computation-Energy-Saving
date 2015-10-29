#include "RandomNumber.hpp"

#include <cstdlib>

_int64 getNextRandomNumber(_int64 currentNumber) {

	return (MULTIPLIER * currentNumber + INCREMENT) % MODULUS;

}

bool getNextRandomBoolean() {

	return (std::rand()) % 2 == 0;

}
