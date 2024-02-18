/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 7: Ortho Maze
Due: Tues. Nov. 7, 2023
Program overall: Maze navigation using keyboard to create simple interactive game
This is the cpp file of the Maze class with all it's declared functions.
Tasked feature: created a new random maze using Aldous-Broder algorithm and user input
*/

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include "fssimplewindow.h"
#include "DrawingUtilNG.h"
#include "Maze.h"

using namespace std;

void Maze::createRandom(int wid, int hei, Cell beginCell, Cell endCell)
{
	// clear connMatrix and setup width, height, start and end
	clear();
	width = wid;
	height = hei;
	start = beginCell;
	end = endCell;
	overallSize = (width + 1) * (height + 1);
	connMatrix = new bool[overallSize * overallSize]();
	bool visited[MAX_MAZE_SIZE][MAX_MAZE_SIZE] = { false };

	// Aldous-Broder algorithm
	
	// start at the center of the maze
	Cell currCell = { height / 2, width / 2 };
	Cell neighborCell;
	int visitedCells = width * height;
	visited[currCell.row][currCell.col] = true;
	visitedCells--;
	// go through all unvisited neighbor cells
	while (visitedCells > 0) {
		// radom neighbor
		neighborCell = getRandomNeighbor(currCell);
		if (visited[neighborCell.row][neighborCell.col] != true) {
			// connect currCell and neighborCell
			connect(currCell, neighborCell);
			connect(neighborCell, currCell);
			// mark neighborCell as visited
			visited[neighborCell.row][neighborCell.col] = true;
			visitedCells--;
		}
		currCell = neighborCell;
	}
}

Maze::Maze(int wid, int hei)
{
	width = wid;
	height = hei;
	start = { 1,1 };
	end = { height, width };
	overallSize = (width + 1) * (height + 1);
	connMatrix = new bool[overallSize * overallSize]();// the () makes them all zero
}

void Maze::readFile(std::ifstream& input)
{
	string wholeLineString; // used to read file one whole line at a time 
	stringstream lineStream; // used to easily get numbers from part of line
	int colonLocation; // used to store location of colon in line 
	Cell cellA, cellB;

	// delete old maze data
	clear();

	while (!input.eof()) {
		lineStream.clear();// just in case 

		getline(input, wholeLineString); // read the whole line into a string

		// if there is a colon, set up lineStream to start just after colon 
		// also, remember colonLocation, just in case 
		if ((colonLocation = wholeLineString.find(":")) != string::npos)
			lineStream.str(wholeLineString.substr(colonLocation + 1));
		else {
			colonLocation = -1;
			lineStream.str(wholeLineString);
		}

		// check for keywords in the line
		if (wholeLineString.find("Start:") != string::npos) // find() returns npos when not found 
			lineStream >> start.row >> start.col; // put values into class variables 

		else if (wholeLineString.find("End:") != string::npos) // find() returns npos when not found 
			lineStream >> end.row >> end.col; // put values into class variables 

		else if (wholeLineString.find("Conn:") != string::npos && connMatrix != nullptr) {
			lineStream >> cellA.row >> cellA.col >> cellB.row >> cellB.col;
			connect(cellA, cellB);  // note this is NOT bidirectional
		}
		else if (wholeLineString.find("Begin:") != string::npos)
			lineStream >> width >> height;

		else if (wholeLineString.find("Finish:") != string::npos) {
			break;  //possible exit from while loop
		}

		if (connMatrix == nullptr && width > 0 && height > 0) {
			overallSize = (width + 1) * (height + 1);
			connMatrix = new bool[overallSize * overallSize]();
		}
	}
}

void Maze::draw()
{
	if (connMatrix != nullptr) {
		using namespace DrawingUtilNG;

		int wallThick = MAZE_BLOCK_SIZE / 5;  // thickness of walls

		// start and end blocks
		glColor3ub(0, 255, 0);  // green
		drawRectangle(start.col * MAZE_BLOCK_SIZE, start.row * MAZE_BLOCK_SIZE,
			MAZE_BLOCK_SIZE, MAZE_BLOCK_SIZE, true);
		glColor3ub(255, 0, 0);  // red
		drawRectangle(end.col * MAZE_BLOCK_SIZE, end.row * MAZE_BLOCK_SIZE,
			MAZE_BLOCK_SIZE, MAZE_BLOCK_SIZE, true);

		glColor3ub(60, 60, 90);  // bluish gray
		// top wall
		drawRectangle(1 * MAZE_BLOCK_SIZE - wallThick / 2, (0 + 1) * MAZE_BLOCK_SIZE - wallThick / 2,
			MAZE_BLOCK_SIZE * width + wallThick, wallThick, true);

		// left wall
		drawRectangle((0 + 1) * MAZE_BLOCK_SIZE - wallThick / 2,
			1 * MAZE_BLOCK_SIZE - wallThick / 2, wallThick, MAZE_BLOCK_SIZE * height + wallThick, true);

		// interior walls
		for (int i = 1; i <= height; i++)
			for (int j = 1; j <= width; j++) {
				if (!areConnected({ i,j }, { i + 1,j })) { // no connection to cell to the south
					drawRectangle(j * MAZE_BLOCK_SIZE - wallThick / 2, (i + 1) * MAZE_BLOCK_SIZE - wallThick / 2,
						MAZE_BLOCK_SIZE + wallThick, wallThick, true);
				}
				if (!areConnected({ i,j }, { i,j + 1 })) { // no connection to cell to the east
					drawRectangle((j + 1) * MAZE_BLOCK_SIZE - wallThick / 2,
						i * MAZE_BLOCK_SIZE - wallThick / 2, wallThick, MAZE_BLOCK_SIZE + wallThick, true);
				}
			}
	}
}

Cell Maze::getCellFromMouse(int mouseX, int mouseY)
{
	Cell mouseCell = { mouseY / MAZE_BLOCK_SIZE , mouseX / MAZE_BLOCK_SIZE };

	if (0 < mouseCell.row && mouseCell.row <= height
		&& 0 < mouseCell.col && mouseCell.col <= width) {
		return mouseCell;
	}
	else
		return { -1,-1 };

}

Cell Maze::getRandomNeighbor(Cell currCell)
{
	int newRow = currCell.row, newCol = currCell.col;
	string possibilities = "";

	// determine what possible directions neighbor can be: North, South, East or West
	// note that odds can be easily manipulated here by adding more of one letter
	// inapporpriate letters are avoided (e.g., no North if currCell is at top row)
	if (currCell.row > 1)
		possibilities += "N";
	if (currCell.row < height)
		possibilities += "S";
	if (currCell.col > 1)
		possibilities += "W";
	if (currCell.col < width)
		possibilities += "E";

	// select random
	char selection = possibilities[rand() % possibilities.length()];

	// make adjustments based on selection
	switch (selection) {
	case 'N': newRow--; break;
	case 'S': newRow++; break;
	case 'W': newCol--; break;
	case 'E': newCol++; break;
	}

	// return neighbor
	return { newRow, newCol };
}

void Maze::changeCell(int mouseX, int mouseY, int key)
{
	Cell mouseCell = getCellFromMouse(mouseX, mouseY);

	if (mouseCell.row != -1) {
		switch (key) {
		case FSKEY_S:
			start = mouseCell;
			break;
		case FSKEY_E:
			end = mouseCell;
			break;
		}
	}
}

bool Maze::setConnState(Cell aCell, Cell anotherCell, bool newState)
{
	int anIndex = getIndex(aCell);
	int anotherIndex = getIndex(anotherCell);
	if (anIndex >= 0 && anotherIndex >= 0) {
		connMatrix[anIndex * overallSize + anotherIndex] = newState;
		return true;
	}
	return false;
}

void Maze::clear() {
	if (connMatrix != nullptr)
		delete[] connMatrix;
	connMatrix = nullptr;
	width = -1;
	height = -1;
	start = { 1, 1 };
	end = { height,width };
}

std::ostream& operator<<(std::ostream& out, const Maze& aMaze)
{
	out << "Start: " << aMaze.start.row << " " << aMaze.start.col << endl;
	out << "End: " << aMaze.end.row << " " << aMaze.end.col << endl;
	out << "Map Begin: " << aMaze.width << " " << aMaze.height << endl;

	Cell cell1, cell2;
	int overallSize = aMaze.overallSize;
	for (int i = 0; i < overallSize; i++)
		for (int j = 0; j < overallSize; j++) {
			if (aMaze.connMatrix[i * overallSize + j]) {
				cell1 = aMaze.getCell(i);
				cell2 = aMaze.getCell(j);
				out << "\tConn: " << cell1.row << " " << cell1.col
					<< " " << cell2.row << " " << cell2.col << endl;
			}
		}

	out << "Map Finish:" << endl;
	return out;
}

