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
	if (myMaze->areConnected({locRow, locCol}, { testRow, testCol })) {
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
		int wid, hei;
		FsGetWindowSize(wid, hei);  // get window size
		glColor3ub(200, 200, 200);

		if (theTorchShape == CIRCLE)
			drawCircle((locCol + 0.5) * Maze::MAZE_BLOCK_SIZE, (locRow + 0.5) * Maze::MAZE_BLOCK_SIZE,
				torchSize * Maze::MAZE_BLOCK_SIZE, torchSize * Maze::MAZE_BLOCK_SIZE,
				true, 2. * (wid + hei), 2. * (wid + hei));

		else if (theTorchShape == SQUARE)
			drawRectangle((locCol - torchSize / 2. + 0.5) * Maze::MAZE_BLOCK_SIZE, (locRow - torchSize / 2. + 0.5) * Maze::MAZE_BLOCK_SIZE,
				torchSize * Maze::MAZE_BLOCK_SIZE, torchSize * Maze::MAZE_BLOCK_SIZE,
				true, 2. * (wid + hei), 2. * (wid + hei));

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
	// NEED THIS FOR PS07
}


