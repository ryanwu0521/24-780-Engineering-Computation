/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 9: Word Count
Due: Tues. Nov. 21, 2023
Program overall: Create a word count, word cloud, and use a Binary Search Tree and a Hash Table to perform various time performance tests.
This is the cpp file of word cloud with all it's functions and features.
Tasked feature: generate word cloud and its GUI for taking user inputs.
*/

//#pragma warning(disable:4996)
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <ctype.h>
#include <string>
#include <chrono>           // provide timing data


#include "fssimplewindow.h"
#include "GraphicFont.h"
#include "DrawingUtilNG.h"
#include "StringPlus.h"

//#include "BST.h"
#include "WordCollection.h"

using namespace std;

void readFile(const string fileName, WordCollection& theWords);
void printMenu();
int randBetween(int low, int high);
void setRandomCoords(int* xCoords, int* yCoords,   // call by reference on array
	int size, int minX, int maxX, int minY, int maxY);
void setRandomColors(int* colors, int size);

int main(void)
{
	// seed random generator
	srand(time(NULL));

	WordCollection myWords;
	stringstream fullStream;

	//string inputFileName = "WiseMessageA.txt";
	//string inputFileName = "WiseMessageB.txt";
	string inputFileName = "CountOfMontecristo.txt";

	auto startTime = std::chrono::system_clock::now();
	
	// read file (Task 3 case 1)
	readFile(inputFileName, myWords);

	// add and remove "count" 1 million times (Task 3 case 2)
	//myWords.removeAddTime("count", 1000000);

	std::chrono::duration<double> duration = std::chrono::system_clock::now() - startTime;
	
	// Use for testing reading file time Task 3 case 1
	cout << "Time for reading file >>" << inputFileName
		<< "<< = " << duration.count() << " seconds" << endl;
	
	// Use for generating word cloud Task 2
	//myWords.printInOrder(cout); // Use this line when testing word cloud

	cout << endl << string(60, '=') << endl << endl; // long line of ========

	cout << "The word 'nation' appears " << myWords.itemCount("nation") << " times." << endl;
	cout << "The word 'superman' appears " << myWords.itemCount("superman") << " times." << endl;
	cout << "There are " << myWords.uniqueWordCount() << " unique words, "
		<< myWords.totalWordCount() << " total." << endl;

	// =========================================================================
	// testing of BST ends here. The rest of the code is for implementing wordcloud
	// put break point on next line when debugging
	// =========================================================================

	// Use true when testing word cloud (Task 2), use false when testing run times (Task 3)
	//bool showTheWordCloud = true;
	bool showTheWordCloud = false;

	if (showTheWordCloud) {
		using namespace DrawingUtilNG;
		int scrnWid = 1100, scrnHei = 800;
		// must "open window" before declaring fonts
		FsOpenWindow(16, 16, scrnWid, scrnHei, 1, "Word Cloud");

		int key = FSKEY_NULL;

		// select a font by uncommenting only one line
		//ImpactFont myFont;
		//GaramondFont myFont;
		//JokermanFont myFont;
		//ComicSansFont myFont;
		TimesNewRomanFont myFont;
		//OldEnglishFont myFont;
		//CourierNewFont myFont;
		//ArialFont myFont;

		int maxNodeCount = myWords.uniqueWordCount();
		int currCount;
		string currString;
		float baseScale = 0.2;

		// generate random screen locations and colors (storing random so we can use over and over)
		int* randX = new int[maxNodeCount];
		int* randY = new int[maxNodeCount];
		int* randC = new int[maxNodeCount];

		setRandomCoords(randX, randY, maxNodeCount,
			scrnWid * 0.1, scrnWid * 0.9,
			scrnHei * 0.1, scrnHei * 0.9);
		setRandomColors(randC, maxNodeCount);

		bool rainbowColors = true;

		// collection of unwanted words, add more if wanted/needed
		WordCollection unwantedWords;
		unwantedWords += "it";
		unwantedWords += "the";
		unwantedWords += "that";
		unwantedWords += "and";
		unwantedWords += "a";
		unwantedWords += "to";
		unwantedWords += "of";
		unwantedWords += "you";
		bool showAll = false;
		bool showCount = false;
		int lowerLimit = 4;

		printMenu();

		// make copy of words as ordered vector
		fullStream.str("");
		myWords.printInOrder(fullStream);
		vector<string> allData = StringPlus::split(fullStream.str(), " :{}\n");

		while (FSKEY_ESC != key)
		{
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			// draw black rectangle
			glColor3d(0.0, 0.0, 0.0);
			drawRectangle(0, 0, scrnWid, scrnHei, true);

			// draw words

			for (int i = 0; i < allData.size(); i += 2) {
				currString = allData.at(i);
				currCount = stoi(allData.at(i + 1)); // convert from string to integer

				if (rainbowColors)
					myFont.setColorHSV(randC[i / 2], 1, 1, .6);
				else
					myFont.setColorHSV(280, 1, 1, .6); // purple, 60% transparent


				if (currCount > lowerLimit && (showAll || unwantedWords.itemCount(currString) == 0)) {
					myFont.drawText(currString, randX[i / 2], randY[i / 2],
						baseScale * currCount, 0, true);
					if (showCount) {
						float width = myFont.getWordWidth(currString, baseScale * currCount);
						myFont.drawText("(" + to_string(currCount) + ")", randX[i / 2] + width / 2., randY[i / 2],
							baseScale * currCount / 3, 0, false);
					}
				}
			}

			FsPollDevice();
			switch (key = FsInkey()) {
			case FSKEY_UP: // zooming in
				baseScale *= 1.1;
				break;
			case FSKEY_DOWN: // zooming out
				baseScale /= 1.1;
				break;

			case FSKEY_C: // toggle unwanted words on or off
				showCount = !showCount;
				break;
			case FSKEY_U: // toggle unwanted words on or off
				showAll = !showAll;
				break;
			case FSKEY_A: // show words of all counts
				lowerLimit = 0;
				break;
			case FSKEY_PLUS: // show more words
				lowerLimit--;
				if (lowerLimit < 0)
					lowerLimit = 0;
				break;
			case FSKEY_MINUS: // show fewer words
				lowerLimit++;
				break;
			case FSKEY_R: // toggle rainbow colors on or off
				rainbowColors = !rainbowColors;
				break;

			case FSKEY_SPACE: // Re-order
				setRandomCoords(randX, randY, maxNodeCount,
					scrnWid * 0.1, scrnWid * 0.9,
					scrnHei * 0.1, scrnHei * 0.9);
				setRandomColors(randC, maxNodeCount);
				break;
			}

			FsSwapBuffers();
			FsSleep(30);

		}

		// just to be nice and polite
		delete[]randX;
		delete[]randY;
		delete[]randC;
	}
}

void readFile(string fileName, WordCollection& theWords)
{
	string tempStr;
	string delim = " .,;:_`!?-=+/\\\"\'[]()|<>\t\n";  // did I miss any ?

	ifstream inFile(fileName);
	if (inFile.is_open()) { // may not have found file

		while (!inFile.eof()) {
			// read whole line
			getline(inFile, tempStr);

			// split line into words
			vector<string> tokens = StringPlus::split(tempStr, delim);

			// insert each of the words into collection (faster to do it "backwards" when popping)
			while (!tokens.empty()) {
				theWords += StringPlus::lowerCase(tokens.back());
				tokens.pop_back();
			}
		}
		inFile.close();
	}
}

void printMenu()
{
	cout << endl << endl << "Instructions: " << endl;
	cout << "    Arrow UP to zoom in." << endl;
	cout << "    Arrow DOWN to zoom out." << endl;
	cout << "    'U' to toggle display of \"common\" words." << endl;
	cout << "    'A' show all words (no matter how rare)" << endl;
	cout << "    '+' show more rare words" << endl;
	cout << "    '-' show fewer rare words" << endl;
	cout << "    'C' to toggle display of word count." << endl;
	cout << "    'R' to toggle rainbow colors mode." << endl;
	cout << "  SPACE to regenerate graphics." << endl;
	cout << endl;
}

int randBetween(int low, int high) {
	return low + ((double)rand() / RAND_MAX * (high - low));
}

void setRandomCoords(int* xCoords, int* yCoords,   // call by reference
	int size, int minX, int maxX, int minY, int maxY) {
	for (int i = 0; i < size; i++) {
		xCoords[i] = randBetween(minX, maxX);
		yCoords[i] = randBetween(minY, maxY);
	}
}

void setRandomColors(int* colors, int size) {
	for (int i = 0; i < size; i++) {
		colors[i] = randBetween(0, 35) * 10; // 0 to 350 by tens
	}
}
