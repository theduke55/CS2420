//Copyright 2017, Bradley Peterson, Weber State University, all rights reserved.


#include <cstdio>
#include <random>
#include <thread> //C++11 support!   Visual studio 2012+ users can use this if they want.
#include <mutex> 
#include <iostream>
#include <vector>

//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#else
#include <stdio.h>
#endif

using std::cout;
using std::endl;
using std::vector;
using std::thread;
using std::mutex;

//*** Prototypes ***
void recQuickSort(unsigned long *arr, int first, int last);
void pressAnyKeyToContinue();
class BucketsCollection;

//***GLOBAL VARIABLES***
unsigned long *list;
int listSize;
int numBuckets;
int numThreads;
BucketsCollection *globalBuckets;
const unsigned long ULONGMAX = 4294967295;
mutex myMutex;

class BucketsCollection {
public:
	BucketsCollection(const unsigned int numBuckets);
	~BucketsCollection();
	void addItem(unsigned long item);
	unsigned int getNumBuckets() const;
	unsigned int getNumItemsInABucket(const unsigned int bucket) const;
	unsigned long * getBucketArray(const unsigned int bucket);
	void copyOneBucketsIntoAnotherBuckets(BucketsCollection& smallBucket);
	void printAllBuckets() const;

private:
	//TODO: Make an array of vectors of unsigned longs.  
	//It must be named "arr"
	//Each vector is a bucket
	//The user supplies the amount of buckets
	vector<unsigned long>* arr{ nullptr };

	//TODO: 
	//Store the range of the buckets.  
	unsigned long bucketRange{ 0 };
	//The max is be given to you.  It is ULONGMAX.  

};

//*** Provide methods for the bucket class ***
BucketsCollection::BucketsCollection(const unsigned int numBuckets) {
	//DO NOT redeclare the buckets array here.
	//Instead make a new array of vectors and store it in the 
	//data member pointer for it.  
	arr = new vector<unsigned long>[numBuckets];
	if (numBuckets == 1)
	{
		bucketRange = ULONGMAX;
	}
	else
	{
		bucketRange = (ULONGMAX / numBuckets) + 1;
	}
}

BucketsCollection::~BucketsCollection() {
	//TODO: Delete the arr
	delete[] arr;
}

void BucketsCollection::addItem(unsigned long item) {
	//Pass in an item
	//Do the math to figure out which bucket
	//push back on that vector.  
	int bucket = item / bucketRange;
	arr[bucket].push_back(item);
}

unsigned int BucketsCollection::getNumBuckets() const {
	return numBuckets;
}

unsigned int BucketsCollection::getNumItemsInABucket(const unsigned int bucket) const {
	//You pass in the bucket #, it returns how many items that bucket contains.
	return arr[bucket].size();
}

void BucketsCollection::printAllBuckets() const {
	//Displays the contents of all buckets to the screen.

	// Just uncomment this code when you have arr properly declared as a data member
	printf("******\n");
	for (int i = 0; i < numBuckets; i++) {
		printf("Bucket number %d\n", i);
		for (int j = 0; j < arr[i].size(); j++) {
			printf("%08X ", arr[i][j]);

		}
		printf("\n");
	}
	printf("\n");
}

unsigned long * BucketsCollection::getBucketArray(const unsigned int bucket) {
	//You pass in the bucket #, it returns the array that contains the bucket's data.

	//Just uncomment this code when you have arr properly declared as a data member
	return arr[bucket].data();

}

void BucketsCollection::copyOneBucketsIntoAnotherBuckets(BucketsCollection& smallBucket) {
	//Copies all items in all buckets from one BucketsCollection object into another BucketsCollection object.  
	//Not for the first part of the homework assignment, only the multithreaded part.
	myMutex.lock();
	for (int i = 0; i < numThreads; i++)
	{
		this->arr[i].insert(this->arr[i].end(), smallBucket.arr[i].begin(), smallBucket.arr[i].end());
	}
	myMutex.unlock();
}


//***Functions that help our bucket sort work.***
void printList() {
	for (int i = 0; i < listSize; i++) {
		//cout << list[i] << " ";
		printf("%08X ", list[i]);
	}
}

void createList() {

	list = new unsigned long[listSize];

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned long> dis(0, ULONGMAX);

	for (int i = 0; i < listSize; ++i) {
		list[i] = dis(gen);
	}
}

void placeIntoBuckets(int threadID) {

	//TODO: Put the values into the appropriate buckets.
	//globalBuckets->
	int startIndex = threadID * listSize / numThreads;
	int endIndex = (threadID + 1) * listSize / numThreads;
	int i = startIndex;
	BucketsCollection* localVar = new BucketsCollection(numBuckets);
	while (i < endIndex)
	{
		localVar->addItem(list[i]);
		i++;
	}
	globalBuckets->copyOneBucketsIntoAnotherBuckets(*localVar);
	delete localVar;
}


void sortEachBucket(int threadID) {

	//TODO: Sort each individual bucket
	recQuickSort(globalBuckets->getBucketArray(threadID), 0, globalBuckets->getNumItemsInABucket(threadID));
}


void combineBuckets(int threadID) {

	//TODO: Copy each bucket back out to the original list[] array
	//Loop for the amount of buckets
	//Get the starting address for the bucket's array
	//Loop for the amount of items in that bucket
	//Copy that item from the bucket array into the original list

	int listIndex = 0;
	unsigned long* tempArr = globalBuckets->getBucketArray(threadID);
	unsigned int size = globalBuckets->getNumItemsInABucket(0);
	unsigned int prevSize = 0;
	int index = 0;
	int temp = 0;
	while (index != threadID)
	{
		prevSize = size;
		size = size + globalBuckets->getNumItemsInABucket(index + 1);
		index++;
	}
	for (int j = prevSize; j < size; j++)
	{
		list[j] = tempArr[temp];
		++temp;
	}

}


void bucketSort(bool displayOutput) {

	//For the upcoming homeowork assignment, I think it will help you the most to split your work into these three functions.  
	//Create thread managing objects
	thread * threads = new thread[numThreads];
	

	//placeIntoBuckets();
	for (int i = 0; i < numThreads; i++)
	{
		threads[i] = thread(placeIntoBuckets, i);
	}

	for (int i = 0; i < numThreads; i++)
	{
		threads[i].join();
	}

	if (displayOutput) {
		printf("Displaying each bucket's contents before sorting: \n");
		globalBuckets->printAllBuckets();
	}

	//sortEachBucket(int threadID);
	for (int i = 0; i < numThreads; i++)
	{
		threads[i] = thread(sortEachBucket, i);
	}

	for (int i = 0; i < numThreads; i++) 
	{
		threads[i].join();
	}



	//combineBuckets();
	for (int i = 0; i < numThreads; i++)
	{
		threads[i] = thread(combineBuckets, i);
	}

	for (int i = 0; i < numThreads; i++)
	{
		threads[i].join();
	}

	//clean up the thread managing objects
	delete[] threads;
	if (displayOutput) {
		printf("Displaying each bucket's contents after sorting: \n");
		globalBuckets->printAllBuckets();
		pressAnyKeyToContinue();
		printf("Displaying what is hopefully a sorted array: \n");
		printList(); //See if it's all sorted.
	}


}


void swap(unsigned long *arr, int first, int second) {

	unsigned long temp;

	temp = arr[first];
	arr[first] = arr[second];
	arr[second] = temp;
}


int partition(unsigned long *arr, int first, int last) {
	unsigned long pivot;
	int index, smallIndex;

	//Take the middle value as the pivot.
	//swap(first, (first + last) / 2);
	pivot = arr[first];
	smallIndex = first;
	for (index = first + 1; index < last; index++) {
		if (arr[index] < pivot) {
			smallIndex++;
			swap(arr, smallIndex, index);
		}
	}

	//Move pivot into the sorted location
	swap(arr, first, smallIndex);

	//Tell where the pivot is
	return smallIndex;

}


void recQuickSort(unsigned long *arr, int first, int last) {
	//first is the first index
	//last is the one past the last index (or the size of the array
	//if first is 0)

	if (first < last) {
		//Get this sublist into two other sublists, one smaller and one bigger
		int pivotLocation = partition(arr, first, last);
		recQuickSort(arr, first, pivotLocation);
		recQuickSort(arr, pivotLocation + 1, last);
	}
}

void verifySort(unsigned long *arr, unsigned int arraySize) {
	for (int i = 1; i < arraySize; i++) {
		if (arr[i] < arr[i - 1]) {
			printf("ERROR, this list was not sorted correctly.  At index %d is value %08X.  At index %d is value %08X\n", i - 1, arr[i - 1], i, arr[i]);
			return;
		}
	}
	printf("PASSED SORT TEST - The list was sorted correctly\n");
}
void pressAnyKeyToContinue() {
	printf("Press any key to continue\n");

	//Linux and Mac users with g++ don't need this
	//But everyone else will see this message.
#ifndef __GNUC__
	_getch();
#else
	int c;
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
#endif

}

int main() {



	//Set the listSize, numBuckets, and numThreads global variables.  
	listSize = 100;

	numBuckets = 2;
	numThreads = 2;
	createList();
	globalBuckets = new BucketsCollection(numBuckets);
	printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	printf("Displaying the unsorted list array:\n");
	printList(); //useful for debugging small amounts of numbers.  
	pressAnyKeyToContinue();
	bucketSort(true);
	verifySort(list, listSize);
	pressAnyKeyToContinue();
	delete globalBuckets;
	delete[] list;

	numBuckets = 4;
	numThreads = 4;
	createList();
	printf("\nStarting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	globalBuckets = new BucketsCollection(numBuckets);
	pressAnyKeyToContinue();
	bucketSort(true);
	verifySort(list, listSize);
	pressAnyKeyToContinue();
	delete globalBuckets;
	delete[] list;

	printf("\nFor timing purposes, please make sure all printf statements are commented out\n");
	pressAnyKeyToContinue();

	listSize = 4000000;
	numBuckets = 1;
	numThreads = 1;
	createList();
	globalBuckets = new BucketsCollection(numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d, this is effectively a quick sort\n", listSize, numBuckets, numThreads);
	auto start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> diff = end - start;
	printf("Finished quick sort simulation in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;


	listSize = 4000000;
	numBuckets = 2;
	numThreads = 2;
	createList();
	globalBuckets = new BucketsCollection(numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	printf("Finished bucket sort in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;

	numBuckets = 4;
	numThreads = 4;
	createList();
	globalBuckets = new BucketsCollection(numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	printf("Finished bucket sort in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;

	numBuckets = 8;
	numThreads = 8;
	createList();
	globalBuckets = new BucketsCollection(numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	printf("Finished bucket sort in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;

	numBuckets = 16;
	numThreads = 16;
	createList();
	globalBuckets = new BucketsCollection(numBuckets);
	printf("Starting bucket sort for listSize = %d, numBuckets = %d, numThreads = %d\n", listSize, numBuckets, numThreads);
	start = std::chrono::high_resolution_clock::now();
	bucketSort(false);
	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	printf("Finished bucket sort in %1.3lf milliseconds\n", diff.count());
	verifySort(list, listSize);
	delete globalBuckets;
	delete[] list;

	pressAnyKeyToContinue();
	return 0;
}
