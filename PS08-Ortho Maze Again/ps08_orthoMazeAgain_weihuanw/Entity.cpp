/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 8: Ortho Maze Again
Due: Tues. Nov. 14, 2023
Program overall: Maze navigation using keyboard to create simple interactive game
This is the cpp file of the Entity class with all it's declared functions.
Tasked feature: created the A* algorithm to find the shortest path and added full editable maze feature
*/

#include <queue>
#include <iostream>
#include "fssimplewindow.h"
//#include "ysglfontdata.h"
#include "DrawingUtilNG.h"
#include "Maze.h"  // not included in Entity.h
#include "Entity.h"

void Entity::reset()
{
	// mark everything as not traveled and not visited and not "parented"
	for (int i = 1; i < Maze::MAX_MAZE_SIZE; i++)
		for (int j = 1; j < Maze::MAX_MAZE_SIZE; j++) {
			traveled[i][j] = false;
			visited[i][j] = false;     // could call clearShortestPath() here, but since I
			parent[i][j] = { -1,-1 };  // already have a loop running  . . .
		}

	// put entity back at start loc
	locRow = myMaze->getStart().row;
	locCol = myMaze->getStart().col;

	// reset celebrateCounter
	celebrateCounter = 0;
}

void Entity::clearShortestPath()
{
	for (int i = 1; i < Maze::MAX_MAZE_SIZE; i++)
		for (int j = 1; j < Maze::MAX_MAZE_SIZE; j++) {
			visited[i][j] = false;
			parent[i][j] = { -1,-1 };
		}
}

bool Entity::move(int direction)
{
	int testCol = locCol, testRow = locRow;

	switch (direction) {
	case FSKEY_UP: testRow--;
		break;
	case FSKEY_DOWN: testRow++;
		break;
	case FSKEY_LEFT: testCol--;
		break;
	case FSKEY_RIGHT: testCol++;
		break;
	}
	if (myMaze->areConnected({ locRow, locCol }, { testRow, testCol })) {
		// record location where entity used to be
		traveled[locRow][locCol] = true;
		// change location of entity
		locCol = testCol;
		locRow = testRow;
		return true;
	}
	else
		return false;
}

void Entity::paintTraveledTrack()
{
	int xCenter;
	int yCenter;

	glColor3ub(0, 0, 200); // blue

	// draw motion track
	for (int i = 1; i < Maze::MAX_MAZE_SIZE; i++)
		for (int j = 1; j < Maze::MAX_MAZE_SIZE; j++)
			if (traveled[i][j]) {
				xCenter = (j + 0.5) * Maze::MAZE_BLOCK_SIZE;
				yCenter = (i + 0.5) * Maze::MAZE_BLOCK_SIZE;
				DrawingUtilNG::drawRectangle(xCenter - 2, yCenter - 2, 4, 4, false);
			}
}
void Entity::paintSearchedCells()
{
	int xCenter;
	int yCenter;

	glColor3ub(220, 220, 120); // orange

	// shaded square where maze was visited in finding shortest path
	for (int i = 1; i < Maze::MAX_MAZE_SIZE; i++)
		for (int j = 1; j < Maze::MAX_MAZE_SIZE; j++)
			if (visited[i][j]) {
				xCenter = (j + 0.5) * Maze::MAZE_BLOCK_SIZE;
				yCenter = (i + 0.5) * Maze::MAZE_BLOCK_SIZE;
				DrawingUtilNG::drawRectangle(xCenter - 7, yCenter - 7, 14, 14, true);
			}
}

void Entity::paint()
{
	int xCenter = (locCol + 0.5) * Maze::MAZE_BLOCK_SIZE;
	int yCenter = (locRow + 0.5) * Maze::MAZE_BLOCK_SIZE;

	glColor3ub(0, 0, 255);  // blue

	// legs
	glBegin(GL_LINES);
	glVertex2i(xCenter - 8, yCenter - 5);
	glVertex2i(xCenter + 8, yCenter + 5);

	glVertex2i(xCenter + 8, yCenter - 5);
	glVertex2i(xCenter - 8, yCenter + 5);

	glVertex2i(xCenter + 8, yCenter - 0);
	glVertex2i(xCenter - 8, yCenter + 0);
	glEnd();

	// head
	glBegin(GL_LINE_LOOP);
	glVertex2i(xCenter, yCenter - 2);
	glVertex2i(xCenter - 3, yCenter - 6);
	glVertex2i(xCenter - 2, yCenter - 7);
	glVertex2i(xCenter + 2, yCenter - 7);
	glVertex2i(xCenter + 3, yCenter - 6);
	glEnd();

	// draw the shortest path using parent array, starting at end
	// and working back to beginning
	glColor3ub(150, 150, 0);  // umber
	Cell endCell = myMaze->getEnd();

	Cell currCell = parent[endCell.row][endCell.col];
	while (currCell.row != -1) {
		xCenter = (currCell.col + 0.5) * Maze::MAZE_BLOCK_SIZE - 0;
		yCenter = (currCell.row + 0.5) * Maze::MAZE_BLOCK_SIZE - 0;
		glTranslatef(xCenter, yCenter, 0);
		glRotatef(45.f, 0, 0, 1);
		DrawingUtilNG::drawRectangle(-5, -5, 10, 10, false);
		glRotatef(-45.f, 0, 0, 1);
		glTranslatef(-xCenter, -yCenter, 0);

		currCell = parent[currCell.row][currCell.col];
	}

	// map hiding
	if (theTorchShape != EVERYTHING) {

		// Draw a big grey blob, with a hole in the middle

		using namespace DrawingUtilNG;
		float outerDimension = 4. * Maze::MAX_MAZE_SIZE * Maze::MAZE_BLOCK_SIZE;

		glColor3ub(200, 200, 200);

		if (theTorchShape == CIRCLE)
			drawCircle((locCol + 0.5) * Maze::MAZE_BLOCK_SIZE, (locRow + 0.5) * Maze::MAZE_BLOCK_SIZE,
				torchSize * Maze::MAZE_BLOCK_SIZE, torchSize * Maze::MAZE_BLOCK_SIZE,
				true, outerDimension, outerDimension);

		else if (theTorchShape == SQUARE)
			drawRectangle((locCol - torchSize / 2. + 0.5) * Maze::MAZE_BLOCK_SIZE, (locRow - torchSize / 2. + 0.5) * Maze::MAZE_BLOCK_SIZE,
				torchSize * Maze::MAZE_BLOCK_SIZE, torchSize * Maze::MAZE_BLOCK_SIZE,
				true, outerDimension, outerDimension);

		// nothing yet for oval (more view ahead than behind)
		// nothing yet for cloud (torch has changing shape)
	}
}

bool Entity::reachedGoal()
{
	if (myMaze != nullptr)
		return (myMaze->getEnd().row != -1
			&& locRow == myMaze->getEnd().row && locCol == myMaze->getEnd().col);
	else
		return false;

}

void Entity::celebrateGoal()
{
	using namespace DrawingUtilNG;

	int flashRate = 6;
	float radius = (celebrateCounter / flashRate + 2) * Maze::MAZE_BLOCK_SIZE / 2;
	int xCenter = (locCol + 0.5) * Maze::MAZE_BLOCK_SIZE; // the 0.5 centers on the block
	int yCenter = (locRow + 0.5) * Maze::MAZE_BLOCK_SIZE;

	glColor3ub(255, 255, 0);
	glLineWidth(4);
	drawCircle(xCenter, yCenter, radius, radius * 1.2, false);
	drawCircle(xCenter, yCenter, radius * 1.5, radius * 1.8, false);
	glLineWidth(1);

	// increase counter and reset if needed
	if (++celebrateCounter > flashRate * 5)
		celebrateCounter = 0;
}

void Entity::findShortestPath()
{
	if (locRow > 0) {
		using namespace std;

		Cell endCell = myMaze->getEnd();
		Cell currCell;
		queue<Cell> processQueue;  // this is the key to BFS

		// for cycling through adjacent cells (will change later)
		int rowAdjust[] = { -1, 0, 1, 0 };
		int colAdjust[] = { 0, 1, 0, -1 };
		int testRow, testCol;

		clearShortestPath();

		//Mark the root/start vertex as visited.
		visited[locRow][locCol] = true;

		//	Add the root vertex to the process queue
		processQueue.push({ locRow,locCol });
		bool foundTheEnd = false;

		//	While the end vertex is yet to be discovered and there are vertices to process :
		while (processQueue.size() > 0 && !foundTheEnd) {
			//Get the current vertex at the head of the queue (and remove it from the queue)
			currCell = processQueue.front();
			processQueue.pop();  // remove from queue

			//	If current vertex is the target vertex, we are done
			if (currCell.row == endCell.row && currCell.col == endCell.col)
				foundTheEnd = true;
			else {
				//	Else, for each of the adjacent vertices that have not been visited :

				for (int i = 0; i < 4; i++) {
					testRow = currCell.row + rowAdjust[i];
					testCol = currCell.col + colAdjust[i];

					// check visited and whether it is a valid neighbor
					if (!visited[testRow][testCol]
						&& myMaze->areConnected(currCell, { testRow, testCol })) {
						//Set its parent to current vertex
						parent[testRow][testCol] = currCell;
						//	Mark it as visited
						visited[testRow][testCol] = true;
						//	Add it to the queue
						processQueue.push({ testRow,testCol });
					}
				}
			}
		}
		if (parent[endCell.row][endCell.col].row == -1) {
			cout << "Could not find a path " << endl;
		}
	}
}

void Entity::findShortestPathAStar()
{
	if (locRow > 0) {
		using namespace std;
		
		Cell endCell = myMaze->getEnd();
		Cell currCell;

		struct CellPlus { int row, col; int dist; };
		struct compareDistance {
			// return "true" if "c1" should be farther from root than ��c2��
			bool operator()(CellPlus const& c1, CellPlus const& c2) {
				return c1.dist > c2.dist;
			}
		};


		//queue<Cell> processQueue;  // this is the key to BFS
		priority_queue<CellPlus, vector<CellPlus>, compareDistance> processQueue;

		//Need to keep track of all the gScores(actual distance from
			//start of path to each node).Set default to ��infinity��
		int gScore[Maze::MAX_MAZE_SIZE][Maze::MAX_MAZE_SIZE];
		for (int i = 0; i < Maze::MAX_MAZE_SIZE; i++)
			for (int j = 0; j < Maze::MAX_MAZE_SIZE; j++)
				gScore[i][j] = INT_MAX;

		//Need to quickly check if a Cell is already in the queue
		bool isInQueue[Maze::MAX_MAZE_SIZE][Maze::MAX_MAZE_SIZE] = { false };



		// for cycling through adjacent cells (will change later)
		int rowAdjust[] = { -1, 0, 1, 0 };
		int colAdjust[] = { 0, 1, 0, -1 };
		int testRow, testCol;

		clearShortestPath();

		// Mark the root/start vertex as visited.( Not needed for A*)
		visited[locRow][locCol] = true;

		// Add the root vertex to the process queue
		processQueue.push({ locRow,locCol,0 }); //0 distance for start point
		isInQueue[locRow][locCol] = true; //do this everytime something is pushed
		gScore[locRow][locCol] = 0; // beginning of path has a gScore of zero

		bool foundTheEnd = false;
		while (!foundTheEnd && !processQueue.empty()) {
			auto topOfHeap = processQueue.top();
			currCell = { topOfHeap.row,topOfHeap.col };
			processQueue.pop();

			if (currCell.row == endCell.row && currCell.col == endCell.col) {
				foundTheEnd = true;
			}
			else {
				for (int i = 0; i < 4; i++) {
					testRow = currCell.row + rowAdjust[i];
					testCol = currCell.col + colAdjust[i];
					if (myMaze->areConnected(currCell, { testRow,testCol })) {
						// Calculate its tentative gScore as current�s g_score + 1
						int tentativeG = gScore[currCell.row][currCell.col] + 1;

						if (gScore[testRow][testCol] > tentativeG) {
							// Store better gScore for neighbor
							gScore[testRow][testCol] = tentativeG;
							// Set its parent to current vertex
							parent[testRow][testCol] = currCell;
							// Mark it as visited
							visited[testRow][testCol] = true;

							// If neighbor is not in queue, add it to queue
							if (!isInQueue[testRow][testCol]) {
								processQueue.push({ testRow,testCol,tentativeG + heuristic({testRow,testCol}) });
								isInQueue[testRow][testCol] = true;
							}
						}
					}
				}
			}
		}
	}
}

int Entity::heuristic(Cell aCell)
{
	Cell endCell = myMaze->getEnd();

	// calculate Manhattan distance
	int manhattanDistance = abs(endCell.row - aCell.row) + abs(endCell.col - aCell.col);
	// calculate Euclidean distance
	int euclideanDistance = sqrt(pow(endCell.row - aCell.row, 2) + pow(endCell.col - aCell.col, 2));
	
	return manhattanDistance;
	//return euclideanDistance;
	
	//return abs(endCell.row - aCell.row) + abs(endCell.col - aCell.col); //Manhattan
	//return sqrt(endCell.row - aCell.row, 2); //Euclidean 
}

bool Entity::pathIsAvailable()
{
	if (myMaze != nullptr) {
		Cell endCell = myMaze->getEnd();

		return parent[endCell.row][endCell.col].row != -1;
	}
	return false;
}



