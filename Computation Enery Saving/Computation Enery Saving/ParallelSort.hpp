#include <vector>

#include "WeightedPoint.hpp"

class SortableCollection {

private:

	//Threshold for parallel sorting
	ptrdiff_t PARALLEL_SORT_THESHOLD = 500;

	//Input data to be sorted
	std::vector<WeightedPoint> inputData;

	////Sorted data
	std::vector<WeightedPoint> sortedData;

	//Choose median of three keys from values to be sorted
	WeightedPoint* medianOfThree(WeightedPoint* x, WeightedPoint* y, WeightedPoint* z);

	//Choose a partition as median of medians
	WeightedPoint* choosePartitionKey(WeightedPoint* first, WeightedPoint* last);

	//Partition and return the position of the key
	WeightedPoint* divide(WeightedPoint* first, WeightedPoint* last);

	void parallelQuickSort(WeightedPoint* firstElement, WeightedPoint* lastElement);

public:

	//Constructor 
	SortableCollection(std::vector<WeightedPoint> inputData);

	std::vector<WeightedPoint> getData();

	//Do a parallel QuickSort on the input data and return the sorted result
	void doParallelSort();

};