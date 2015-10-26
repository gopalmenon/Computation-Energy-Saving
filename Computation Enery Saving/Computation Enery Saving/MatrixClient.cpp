#include "Matrix.hpp"
#include "RandomNumber.hpp"

#include <iostream>

int main() {

	SerialMatrix serialMatrix(5, 4);
	_int64 randomValue = 1;
	for (int i = 0; i < 10; ++i) {
		randomValue = getNextRandomNumber(randomValue);
		std::cout << randomValue << std::endl;
	}

}