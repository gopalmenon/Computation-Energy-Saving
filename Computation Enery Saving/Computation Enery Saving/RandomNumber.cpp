#include "RandomNumber.hpp"

_int64 getNextRandomNumber(_int64 currentNumber) {

	return (MULTIPLIER * currentNumber + INCREMENT) % MODULUS;

}
