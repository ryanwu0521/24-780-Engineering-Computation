/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 9: Word Count
Due: Tues. Nov. 21, 2023
Program overall: Create a word count, word cloud, and use a Binary Search Tree and a Hash Table to perform various time performance tests.
This is the cpp file of the WordCollection class with all it's functions.
Tasked feature: generate word count and improve time performance of the program
*/

//#pragma warning(disable:4996)
# include <iostream>
# include <map>
# include <sstream>
# include <fstream>
# include <chrono>
# include "WordCollection.h"


// Insert item into Collection (BST). If newItem is invalid, do nothing
// If item in BST, update the item's count, o/w insert it and make count = 1
void WordCollection::insert(const std::string& newItem)
{
	if (!newItem.empty()) {
		auto it = myWordsMap.find(newItem);

		if (it != myWordsMap.end()) {
			it->second++;
		}
		else {
			myWordsMap[newItem] = 1;
		}
	}
}

void WordCollection::operator+=(const std::string & newItem) 
{
	insert(newItem);
}

// Remove item from Collection (BST).
// Reduce count if item is in collection. Eliminate item if its counts goes to zero
bool WordCollection::remove(const std::string& wantedItem) 
{
	if (myWordsMap.find(wantedItem) != myWordsMap.end()) {
		myWordsMap[wantedItem]--;
		if (myWordsMap[wantedItem] == 0) {
			myWordsMap.erase(wantedItem);
		}
		return true;
	}
	return false;
}

void WordCollection::operator-=(const std::string& wantedItem) 
{
	remove(wantedItem);
}

bool WordCollection::removeAll(const std::string& wantedItem) 
{
	auto it = myWordsMap.find(wantedItem);
	if (it != myWordsMap.end()) {
			myWordsMap.erase(it);
			return true;
		}
	return false;
}

// Find the item and return the item count.
// Returns zero if the item is not in the collection
int WordCollection::itemCount(const std::string& wantedItem) 
{
	auto it = myWordsMap.find(wantedItem);

	if (it != myWordsMap.end()) {
		return it->second;
	}
	return 0;
}

// Traverses the collection and prints out to a stringstream each of the items
// in the BST in correct order, along with the item's count, separated by '\n'
void WordCollection::printInOrder(std::stringstream& out) 
{
	for (const auto& pair : myWordsMap) {
		out << pair.first << ": " << pair.second << '\n';
	}
}

void WordCollection::printInOrder(std::ostream& out) 
{
	for (const auto& pair : myWordsMap) {
		out << pair.first << ": " << pair.second << '\n';
	}
}

// Returns number of unique words
int WordCollection::uniqueWordCount() 
{
	return myWordsMap.size();
}

// Returns total of all the words
long int WordCollection::totalWordCount() 
{
	long int totalCount = 0;
	for (const auto& pair : myWordsMap) {
		totalCount += pair.second;
	}
	return totalCount;
}

// Measure time for removing and then re-adding the word "count" 1 million times
void WordCollection::removeAddTime(const std::string& word, int repetitions)
{
	auto startTime = std::chrono::system_clock::now();
	for (int i = 0; i < repetitions; i++) {
		this->remove(word);
		this->insert(word);
	}
	std::chrono::duration<double> duration = std::chrono::system_clock::now() - startTime;
	std::cout << "Time for removing and re-adding the word >>" << word
		<< "<< " << repetitions << " times = " << duration.count() << " seconds" << std::endl;
}