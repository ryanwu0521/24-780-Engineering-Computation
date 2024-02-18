/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 9: Word Count
Due: Tues. Nov. 21, 2023
Program overall: Create a word count, word cloud, and use a Binary Search Tree and a Hash Table to perform various time performance tests.
This is the header file of the WordCollection class.
Tasked feature: generate word count and improve time performance of the program
*/

#pragma once
#include <iostream>
#include <map>
#include <unordered_map>
#include <sstream>

class WordCollection
{
private:
    // Binary Search Tree
	std::map<std::string, int> myWordsMap;

    // Hash Table
    //std::unordered_map<std::string, int> myWordsMap;

public:
    // Insert item into Collection (BST). If newItem is invalid, do nothing
    // If item in BST, update the item's count, o/w insert it and make count = 1
    virtual void insert(const std::string& newItem);

    void operator+=(const std::string& newItem);

    // Remove item from Collection (BST).
    // Reduce count if item is in collection. Eliminate item if its counts goes to zero
    virtual bool remove(const std::string& wantedItem);

    void operator-=(const std::string& wantedItem);

    virtual bool removeAll(const std::string& wantedItem);

    // Find the item and return the item count.
    // Returns zero if the item is not in the collection
    int itemCount(const std::string& wantedItem);

    // Traverses the collection and prints out to a stringstream each of the items
    // in the BST in correct order, along with the item's count, separated by '\n'
    void printInOrder(std::stringstream& out);

    void printInOrder(std::ostream& out);

    // Returns number of unique words
    int uniqueWordCount();

    // Returns total of all the words
    long int totalWordCount();

    // Measure time for removing and then re-adding the word "count" 1 million times
    void removeAddTime(const std::string& word, int repetitions);
 };

/* 
PS09 Task 3 Findings
For Task 3, we are to implement a Binary Search Tree and a Hash Table to perform and compare time performance for reading "CountOfMonteCristo.txt" and removing and re-adding the word "count" 1 million times.
Condition: I am running the reading CountOfMonteCristo.txt and removing and re-adding the word "count" 1 million times at different times (4 runs in total).

Reading CountOfMonteCristo.txt
Binary Search Tree Time: 1.86308 seconds
Hash Table Time: 1.42103 seconds
Hash Table is faster than Binary Search Tree by 0.44205 seconds or 23.7%.

Removing and Re-adding "count" 1 million times
Binary Search Tree Time: 2.64768 seconds
Hash Table Time: 1.34702 seconds
Hash Table is faster than Binary Search Tree by 1.30066 seconds or 49.1%.

Overall, the Hash Table method is faster than the Binary Search Tree method.

*/