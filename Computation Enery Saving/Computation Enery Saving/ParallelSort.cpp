#include "ParallelSort.hpp"

#include <tbb\task_group.h>

#include <algorithm>
#include <vector>

//Choose median of three keys from values to be sorted
WeightedPoint* SortableCollection::medianOfThree(WeightedPoint* x, WeightedPoint* y, WeightedPoint* z) {

	return (*x).pointWeight < (*y).pointWeight ? (*y).pointWeight < (*z).pointWeight ? y : (*x).pointWeight < (*z).pointWeight ? z : x : (*z).pointWeight < (*y).pointWeight ? y : (*z).pointWeight < (*x).pointWeight ? z : x;

}

//Choose a partition as median of medians
WeightedPoint* SortableCollection::choosePartitionKey(WeightedPoint* first, WeightedPoint* last) {

	size_t offset = (last - first) / 8;

	return medianOfThree(medianOfThree(first, first + offset, first + offset * 2),
		medianOfThree(first + offset * 3, first + offset * 4, last - (3 * offset + 1)),
		medianOfThree(last - (2 * offset + 1), last - (offset + 1), last - 1));

}

//Partition and return the position of the key
WeightedPoint* SortableCollection::divide(WeightedPoint* first, WeightedPoint* last) {

	//Move the partition key to the front of the array
	std::swap(*first, *choosePartitionKey(first, last));

	//Partition the array
	WeightedPoint* key = first;
	WeightedPoint* middle = std::partition(first + 1, last, [=](const WeightedPoint& data)->bool {return data.pointWeight < key->pointWeight; }) - 1;

	if (middle != first) {
		//Move the key between the two partitions
		std::swap(*first, *middle);
	}
	else {
		//Return null if all keys are equal since there is no need to sort
		if (last == std::find_if(first + 1, last, [=](const WeightedPoint& data){return key->pointWeight < data.pointWeight; })) {
			return nullptr;
		}
	}
	return middle;
}

void SortableCollection::parallelQuickSort(WeightedPoint* firstElement, WeightedPoint* lastElement) {

	tbb::task_group parallelSortGroup;

	//Do parallel sort for larger data size
	while (lastElement - firstElement > PARALLEL_SORT_THESHOLD) {

		//Partition the array
		WeightedPoint* middleElement = divide(firstElement, lastElement);
		//If all elements are same, no more partitioning is required
		if (middleElement == nullptr) {
			parallelSortGroup.wait();
			return;
		}

		//The array has now been partitioned into two
		if (middleElement - firstElement < lastElement - (middleElement + 1)) {

			//The left partition is smaller and so spawn its sort
			parallelSortGroup.run([=]{parallelQuickSort(firstElement, middleElement); });

			//The next iteration will sort the right part of the array
			firstElement = middleElement + 1;

		}
		else {

			//The right partition is smaller and so spawn its sort
			parallelSortGroup.run([=]{parallelQuickSort(middleElement + 1, lastElement); });

			//The next iteration will sort the left part of the array
			lastElement = middleElement;
		}

	}

	//Number of elements is below the parallel threshold. So do serial sort.
	std::sort(firstElement, lastElement + 1, compareWeightedPoints);
	parallelSortGroup.wait();
}

//Constructor having path and file name of input data as parameter 
SortableCollection::SortableCollection(std::vector<WeightedPoint> inputData) {

	this->inputData = inputData;

}

std::vector<WeightedPoint> SortableCollection::getData() {

	//Return a copy of the input data
	return std::vector<WeightedPoint>(this->inputData);
}

//Do a parallel QuickSort on the input data and return the sorted result
void SortableCollection::doParallelSort() {

	//Get internal array representation of the vector to be sorted
	int numberOfElements = this->inputData.size();
	WeightedPoint* elements = this->inputData.data();
	WeightedPoint* firstElement = &elements[0];
	WeightedPoint* lastElement = &elements[numberOfElements - 1];

	//Do the sorting in parallel
	parallelQuickSort(firstElement, lastElement);
}
