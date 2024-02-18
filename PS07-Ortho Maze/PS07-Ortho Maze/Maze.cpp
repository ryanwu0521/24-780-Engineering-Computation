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

	
}

Maze::Maze(int wid, int hei)
{
	width = wid;
	height = hei;
	start = { 1,1 };
	end = { height, width };
	connMatrix = new bool[width * height * width * height](); // () makes them all zero

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

