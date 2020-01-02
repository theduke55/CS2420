//Copyright 2017, Bradley Peterson, Weber State University, all rights reserved.

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <sstream>

//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#else
#include <cstdio>
#endif

//forward declarations 
template <typename T> class HashTable;

using std::cout;
using std::endl;
using std::map;
using std::list;
using std::string;
using std::stringstream;


//************************************************************************
//A class I designed to help keep track of how much memory you allocate
//Do not modify, this is not part of your assignment, it just helps test it.
//For this to work, a class needs to inherit off of this one. 
//Then this does the rest of the work, since it 
//overloads new, new[], delete, and delete[].
//************************************************************************
class ManageMemory {
public:

	static std::size_t getTotalSize() {
		std::size_t total = 0;
		std::map<void *, std::size_t>::iterator iter;
		for (iter = mapOfAllocations.begin(); iter != mapOfAllocations.end(); ++iter) {
			total += iter->second;
		}
		return total;
	}

	//I overloaded the new and delete keywords so I could manually track allocated memory.
	void* operator new(std::size_t x){
		void *ptr = ::operator new(x);
	mapOfAllocations[ptr] = x;
	return ptr;
	}
		void* operator new[](std::size_t x) {
		void *ptr = ::operator new[](x);
		mapOfAllocations[ptr] = x;
		return ptr;
	}
		void operator delete(void* x) {
		mapOfAllocations.erase(x);
		::operator delete(x);
	}
	void operator delete[](void* x) {
		mapOfAllocations.erase(x);
		::operator delete[](x);
	}
private:
	static std::map<void *, std::size_t> mapOfAllocations;
};
std::map<void *, std::size_t> ManageMemory::mapOfAllocations;


//************************************************************************************
//A quick and simple class that simulates a product object.
//************************************************************************************
class product {
public:
	void setCost(int cost);
	void setName(const string&);
	string getName();
	int getCost();
	string getAllInfo();
private:
	string name;
	int cost;
};
void product::setCost(int cost) {
	this->cost = cost;
}
void product::setName(const string& name) {
	this->name = name;
}
string product::getName() {
	return name;
}
int product::getCost() {
	return cost;
}
string product::getAllInfo() {
	stringstream ss;
	ss << "Name: " << name << ", Cost: " << cost;
	return ss.str();
}



//****************************
//The keyvalue pair class
//****************************
template <typename T>
class KeyValuePair {
public:
	//Default constructor
	KeyValuePair() {}

	//Copy assignment operator
	KeyValuePair& operator=(const KeyValuePair& obj) {
		KeyValuePair temp;
		return temp;
	}

	//Copy constructor
	KeyValuePair(const KeyValuePair<T>& obj) {
		this->key = obj.key;
		this->value = obj.value;
	}

	//Move consturctor
	KeyValuePair(KeyValuePair<T>&& obj) {
		this->key = std::move(obj.key);
		this->value = std::move(obj.value);
	}

	//Constructor to copy L-values
	KeyValuePair(const string& key, const T& value) {
		this->key = key;
		this->value = value;
	}

	//Constructor to move R-values
	KeyValuePair(const string& key, T&& value) {

		this->key = key;
		this->value = std::move(value);
	}

	string key;
	T value;
};


//******************
//The HashTable class
//******************
template <typename T>
class HashTable : public ManageMemory
{
public:

	static const unsigned int NUM_BUCKETS = 100000;
	unsigned int getNumBuckets() { return HashTable<T>::NUM_BUCKETS; }
	unsigned int getTotalCount() const;
	unsigned int getWorstClump() const;

	HashTable() //TODO: Create the array
	{ 
		this->arr = new list<KeyValuePair<T>>[NUM_BUCKETS];
	}  

	~HashTable() //Destruct the array
	{ 
		delete[] arr;
	} 

	HashTable(const HashTable<T>& obj) 
	{
		cout << "Failed homework issue:  You hit the HashTable copy constructor.  That's bad!" << endl;
	}

	HashTable& operator=(const HashTable& obj) 
	{
		cout << "Failed homework issue:  You hit the HashTable copy assignment.  That's bad!" << endl;
		HashTable temp;
		return temp;
	}

	//TODO: Add an operator= move method (see the operator= copy method above as a starting hint)
	HashTable& operator=(HashTable&& obj)
	{
		//this->arr = //obj's array
		//set obj's array to nullptr
		return *this;
	}
	//TODO: supply these methods
	//create(const string& key, const T& item) method for L-values
	//create(const string& key, T&& item) method for R-values
		//std::move(item) to turn the item into an R-value
	//t retrieve(const string& key) method  (return by value, acts as a read only retrieve)      
		//returns the value from a keyValuePair object or throw 1	
	//t& operator[](const string& key) method (return by reference which can allow for modification of values)
	//exists(const string& key) method (returns a boolean)
		//looks into bucket to just find it
	//remove(const string& key) method
		//use erase from the list method and return if you find the value

	void create(const string& key, const T& item);
	void create(const string& key, T&& item);
	T retrieve(const string& key);
	T& operator[](const string& key);
	bool exists(const string& key);
	void remove(const string& key);


private:
	
	int hash(const string& key) const;
	
	//TODO: Make an array of linked lists.  In class I suggested making it a stack array like so, but I was wrong!:
	//int arr[SOME_VAR_FOR_TOTAL_ITEMS];
	//The problem is this is a stack variable.  Stack variables cannot be "moved".  As your program runs, the stack
	//grows and shrinks, and stack variables just go with it.  You can't move a big stack array because it's part of the
	//stack, there is nowhere else to move it.  
	//So we need to do this the more practical way.  
	//int * arr;
	//And in the constructor,  arr = new int[SOME_VAR_FOR_TOTAL_ITEMS]
	//But remember, it's not an array of ints, it's an array of lists of KeyValuePair of Ts.  (It's not a KeyValuePair
	//of strings and Ts, that object already has built in strings).  
	//You will need to make appropriate changes in this private data member area, the constructor, and the destructor.
	
	list<KeyValuePair<T>>* arr{ nullptr };
	

};// end class HashTable

template <typename T>
void HashTable<T>::create(const string& key, const T& item) 
{
	//create a keyValuePair object to hold the key and item provided, emplace back
	int index = hash(key);
	arr[index].emplace_back(key, item);
}

template <typename T>
void HashTable<T>::create(const string& key, T&& item)
{
	int index = hash(key);
	arr[index].emplace_back(key, std::move(item));
}

template <typename T>
T HashTable<T>::retrieve(const string& key)
{
	for (auto& kvp : arr[hash(key)])
	{
		if (kvp.key == key)
		{
			return kvp.value;
		}
	}
	throw 1;
}

template <typename T>
T& HashTable<T>::operator[](const string& key)
{
	for (auto& kvp : arr[hash(key)])
	{
		if (kvp.key == key)
		{
			return kvp.value;
		}
	}
	throw 1;
}

template <typename T>
bool HashTable<T>::exists(const string& key)
{
	for (auto& kvp : arr[hash(key)])
	{
		if (kvp.key == key)
		{
			return true;
		}
		else
		{
			false;
		}
	}
}

template <typename T>
void HashTable<T>::remove(const string& key)
{
	for (auto iter = arr[hash(key)].begin(); iter != arr[hash(key)].end(); ++iter)
	{
		if (iter->key == key)
		{
			arr[hash(key)].erase(iter);
			return;
		}
	}
}

template <typename T>
int HashTable<T>::hash(const string& key) const {
	int temp = 0;
	for (int i = key.length(); i >= 0; i--) {
		temp = (13 * temp + key[i]) % NUM_BUCKETS;
	}
	return temp;
}

template <typename T>
unsigned int HashTable<T>::getTotalCount() const {
	unsigned int count = 0;
	for (int i = 0; i < NUM_BUCKETS; i++) {
		count += arr[i].size();
	}
	return count;
}

template <typename T>
unsigned int HashTable<T>::getWorstClump() const {
	unsigned int count = 0;
	for (int i = 0; i < NUM_BUCKETS; i++) {
		if (arr[i].size() > count) {
			count = arr[i].size();
		}
	}
	return count;
}


//This helps with testing, do not modify.
template <typename T>
string NumberToString(T Number)
{
	ostringstream ss;
	ss << Number;
	return ss.str();
}

//This helps with testing, do not modify.
bool checkEmpty(string testName, string whatItIs) {

	if (whatItIs != "") {
		cout << "Passed " << testName << ", the data was " << whatItIs << endl;
		return true;
	}
	else {
		cout << "***Failed test " << testName << " *** " << endl << "   No data was found! " << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTest(string testName, string whatItShouldBe, string whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else if (whatItShouldBe == "") {
		cout << "****** Failed test " << testName << " ****** " << endl << "     Output was '" << whatItIs << endl << "'     Output should have been blank" << endl;
		return false;

	}
	else {
		cout << "****** Failed test " << testName << " ****** " << endl << "     Output was " << whatItIs << endl << "     Output should have been " << whatItShouldBe << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTest(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "****** Failed test " << testName << " ****** " << endl << "     Output was " << whatItIs << endl << "     Output should have been " << whatItShouldBe << endl;
		return false;
	}
}

//This helps with testing, do not modify.
bool checkTestMemory(string testName, int whatItShouldBe, int whatItIs) {

	if (whatItShouldBe == whatItIs) {
		cout << "Passed " << testName << endl;
		return true;
	}
	else {
		cout << "***Failed test " << testName << " *** " << endl << "  There are " << whatItIs << " bytes of memory yet to be reclaimed!" << endl;
		return false;
	}
}


//This helps with testing, do not modify.
void testSimpleIntHash() {

	HashTable<string> myHash;

	//Test #1, add "Jazz" into our hash with a key of 6.  Try to retrieve it.  
	myHash.create("6", "Jazz");
	checkTest("testSimpleIntHash #1", "Jazz", myHash.retrieve("6"));

	//Test #2, attempt to get the Jazz using the operator[] code.
	checkTest("testSimpleIntHash #2", "Jazz", myHash["6"]);

	//Test #3, attempt to change the value at this location:
	myHash["6"] = "Nuggets";

	checkTest("testSimpleIntHash #3", "Nuggets", myHash["6"]);

	//Test #4, 
	//See if we can find it
	if (myHash.exists("6")) {
		checkTest("testSimpleIntHash #4", "", "");
	}
	else {
		checkTest("testSimpleIntHash #4", "This test should have found an item with key \"6\"", "This test did NOT find an item with key \"6\"");
	}

	//Test #5, see if we can find something that doesn't exist yet.
	if (myHash.exists("1234")) {
		checkTest("testSimpleIntHash #5", "This test should NOT have found an item with key \"1234\".", "This test found an item with key \"1234\"");
	}
	else {
		checkTest("testSimpleIntHash #5", "", "");
	}

	//Test #6, removing it from the hash
	myHash.remove("6");
	if (myHash.exists("6")) {
		checkTest("testSimpleIntHash #6", "This test should NOT have found an item with key \"6\".", "This test found an item with key \"6\"");
	}
	else {
		checkTest("testSimpleIntHash #6", "", "");
	}

	//Add more into the hash
	myHash.create("13", "Lakers");
	myHash.create("25", "Bulls");
	myHash.create("101", "Pelicans");
	myHash.create("77", "Bucks");
	myHash.create("12", "Thunder");
	myHash.create("42", "Nets");
	myHash.create("97", "Timberwolves");
	myHash.create("123", "Hornets");
	myHash.create("500", "Mavericks");

	//Attempt to retrieve some
	checkTest("testSimpleIntHash #7", "Thunder", myHash.retrieve("12"));
	checkTest("testSimpleIntHash #8", "Bucks", myHash.retrieve("77"));
	checkTest("testSimpleIntHash #9", "Hornets", myHash.retrieve("123"));

	//Now count up how many are in there
	checkTest("testSimpleIntHash #10", 9, myHash.getTotalCount());

	//Now just verify that they aren't clumping up badly.
	int worst = 0;
	worst = myHash.getWorstClump();
	if (worst > 4) {
		cout << "Failed testSimpleIntHash #11!  There exists a clump of " << worst << " consecutive items, it shouldn't be worse than 4." << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #11.  Your worst clump was " << worst << " items." << endl;
	}

	//Thow in more items.
	int key = 0;
	stringstream out;
	for (unsigned int i = 0; i < myHash.getNumBuckets(); i++) {

		//this next part just helps create some variation in generated W#s...
		if (i % 2 == 0) {
			key += 17;
		}
		else if (i % 3 == 0) {
			key += 23;
		}
		else if (i % 5 == 0) {
			key += 51;
		}
		else if (i % 7 == 0) {
			key += 13;
		}
		else {
			key += 71;
		}
		//convert an int to a string via help from the stringstream class
		out.str("");
		out << key;
		string temp = out.str();

		myHash.create(temp, "a"); //Just add a bunch of letter a's
	}

	//Make sure they all go in there.  We put in 9 manually, then we added many more.  
	checkTest("testSimpleIntHash #12", myHash.getNumBuckets() + 9, myHash.getTotalCount());

	worst = myHash.getWorstClump();
	if (worst > 10) {
		cout << "Failed testSimpleIntHash #13!  There exists a clump of " << worst << " consecutive items, it shouldn't be worse than 10." << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #13.  Your worst clump was " << worst << " items." << endl;
	}

	//Remove the key "2437968". 
	myHash.remove("2437968");
	if (myHash.exists("2437968")) {
		checkTest("testSimpleIntHash #14", "This test should NOT have found an item with key \"2437968\".", "This test found an item with key \"2437968\"");
	}
	else {
		checkTest("testSimpleIntHash #14", "", "");
	}
	//There should be one less value now
	checkTest("testSimpleIntHash #15", myHash.getNumBuckets() + 8, myHash.getTotalCount());

}


void testHashOfObjects() {

	//Create a HashTable. We want this to be a hash table with int keys, string object values,
	//And we also supply the hash function we want to use for integers..

	HashTable<product> myHash;

	//Test #1, add in a studentObject.  Try to retrive it.  
	product tempProduct;
	tempProduct.setCost(5);
	tempProduct.setName("Silly string");
	myHash.create("12341-51231", tempProduct);
	checkTest("testHashOfObjects #1", "Silly string", myHash.retrieve("12341-51231").getName());

	//Test #2, attempt to get the product using its ID code
	checkTest("testHashOfObjects #2", "Silly string", myHash["12341-51231"].getName());

	//Test #3, see what happens if two products have the same ID code.  This should overwrite the former.
	tempProduct.setCost(18);
	tempProduct.setName("Novelty foam hat");
	myHash["12341-51231"] = tempProduct;
	checkTest("testHashOfObjects #3", "Novelty foam hat", myHash["12341-51231"].getName());

	//Test #4, 
	//See if we can find it
	if (myHash.exists("12341-51231")) {
		checkTest("testHashOfObjects #4", "", "");
	}
	else {
		checkTest("testHashOfObjects #4", "This test should have found an item with key 12341-51231", "This test did NOT find an item with key 12341-51231");
	}

	//Test #5, see if we can find something that doesn't exist yet.
	if (myHash.exists("56756-75675")) {
		checkTest("testHashOfObjects #5", "This test should NOT have found an item with key 56756-75675.", "This test found an item with key56756-75675");
	}
	else {
		checkTest("testHashOfObjects #5", "", "");
	}

	//Test #6, removing it from the hash
	myHash.remove("12341-51231");
	if (myHash.exists("12341-51231")) {
		checkTest("testHashOfObjects #6", "This test should NOT have found an item with key 12341-51231.", "This test found an item with key 12341-51231");
	}
	else {
		checkTest("testHashOfObjects #6", "", "");
	}

	//Now throw in many more items.
	int value = 0;
	string key;
	stringstream out;
	for (unsigned int i = 0; i < myHash.getNumBuckets(); i++) {
		//this next part just helps create some variation for our produce ID codes.
		if (i % 2 == 0) {
			value += 107;
		}
		else if (i % 3 == 0) {
			value += 83;
		}
		else if (i % 5 == 0) {
			value += 47;
		}
		else if (i % 7 == 0) {
			value += 131;
		}
		else {
			value += 53;
		}
		key = "12345-";
		out.str("");
		if (value < 100000)
			key = key + "0";
		if (value < 10000)
			key = key + "0";
		if (value < 1000)
			key = key + "0";
		if (value < 100)
			key = key + "0";
		if (value < 10)
			key = key + "0";
		out << value;
		string temp = out.str();
		if (temp.length() > 5) {
			temp = temp.substr(0, 5);
		}
		key = key + temp;
		//Whew, that took a while, but the W# is in key, and is ready to go

		//Create the student record, and load in values.
		tempProduct.setName("Acme product");
		tempProduct.setCost(rand() % 41);

		//Testing the hash table "add" method
		myHash.create(key, tempProduct);
	}

	//Make sure one went in correctly.  Retrieve it.
	checkEmpty("testHashOfObjects #7", myHash["12345-002112"].getAllInfo());

	//Make sure they all go in there.
	checkTest("testHashOfObjects #8", myHash.getNumBuckets(), myHash.getTotalCount());

	//Now test how good your int hash function is.  
	int worst = myHash.getWorstClump();
	if (worst > 15) {
		cout << "Failed testSimpleIntHash #9!  There exists a clump of " << worst << " consecutive items, it shouldn't be worse than 15." << endl;
	}
	else {
		cout << "Passed testSimpleIntHash #9.  Your worst clump was " << worst << " items." << endl;
	}

}


//This helps with testing, do not modify.
void testHashofHashes() {

	HashTable< HashTable<int> > studentAssignments;
	studentAssignments.create("Alice", HashTable<int>());

	HashTable<int> tempHash2;
	studentAssignments.create("Bob", HashTable<int>());

	HashTable<int> tempHash3;
	studentAssignments.create("Karl", HashTable<int>());

	//Give alice some assignment scores
	studentAssignments["Alice"].create("1", 73);
	studentAssignments["Alice"].create("2", 65);
	studentAssignments["Alice"].create("4", 91);
	//Ensure it went in
	checkTest("testHashofHashes #1", 65, studentAssignments["Alice"]["2"]);

	//And Bob
	studentAssignments["Bob"].create("1", 90);
	studentAssignments["Bob"].create("3", 84);
	studentAssignments["Bob"].create("4", 99);

	//And Karl
	studentAssignments["Karl"].create("1", 92);
	studentAssignments["Karl"].create("2", 92);
	studentAssignments["Karl"].create("3", 87);
	studentAssignments["Karl"].create("4", 10);

	//Now find the average of assignment 4 scores
	int average = (studentAssignments["Alice"]["4"] + studentAssignments["Bob"]["4"] + studentAssignments["Karl"]["4"]) / 3;
	checkTest("testHashofHashes #2", 66, average);

}


void pressAnyKeyToContinue() {
	cout << "Press any key to continue...";

	//Linux and Mac users with g++ don't need this
	//But everyone else will see this message.
#ifndef __GNUC__
	_getch();

#else
	int c;
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
#endif
	cout << endl;
}

int main() {

	testSimpleIntHash();
	checkTestMemory("Memory Leak/Allocation Test #1", 0, ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	testHashOfObjects();
	checkTestMemory("Memory Leak/Allocation Test #2", 0, ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	testHashofHashes();
	checkTestMemory("Memory Leak/Allocation Test #5", 0, ManageMemory::getTotalSize());
	pressAnyKeyToContinue();

	return 0;
}