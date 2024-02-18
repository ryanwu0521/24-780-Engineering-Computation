/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 7: Ortho Maze
Due: Tues. Nov. 7, 2023
Program overall: Maze navigation using keyboard to create simple interactive game
This is the cpp file of the MazeManager class with all it's declared functions.
Tasked feature: created a new random maze using Aldous-Broder algorithm and user input
*/

#include <iostream>
#include <string>
#include "fssimplewindow.h"
#include "DrawingUtilNG.h"
#include "StringPlus.h"
#include "MazeManager.h"

using namespace std;

MazeManager::MazeManager(Maze* aMaze)
{
	theMaze = aMaze;
	if (YSOK != yesMoveSound.LoadWav("click_x.wav"))
		cout << "Can't find yesMoveSound data" << endl;
	if (YSOK != cantMoveSound.LoadWav("gulp_x.wav"))
		cout << "Can't find cantMoveSound data" << endl;
	theSoundPlayer.Start();

	theEntity = new Entity(theMaze);
	theEntity->reset();

	arial.setColorHSV(0, 0, 1, 1); // white
}
bool MazeManager::manage()
{
	Cell modelMouseLoc;

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);

	// take care of window resizing ?
	int wid, hei;
	FsGetWindowSize(wid, hei);
	glViewport(0, 0, wid, hei);
	//glMatrixMode(GL_PROJECTION);
	//glOrtho(0, (float)wid - 1, (float)hei - 1, 0, -1, 1);

	glLoadIdentity();
	// draw all the stuff
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glScaled(scale, scale, 1.);  // scale everything
	glTranslated(-originX, -originY, 0);

	theMaze->draw();

	if (showVisited)
		theEntity->paintSearchedCells();
	theEntity->paintTraveledTrack();
	theEntity->paint();

	if (theEntity->reachedGoal()) {
		theEntity->disableTorch(); // show everything
		theEntity->celebrateGoal();
	}

	// take care of user interactions
	switch (key) {
	case FSKEY_ESC:
		return false;
		break;
	case FSKEY_UP:
	case FSKEY_DOWN:
	case FSKEY_LEFT:
	case FSKEY_RIGHT:
		if (theEntity->move(key)) {
			if (soundIsOn) theSoundPlayer.PlayOneShot(yesMoveSound);
		}
		else
			if (soundIsOn) theSoundPlayer.PlayOneShot(cantMoveSound);
		break;
	case FSKEY_Q:
		soundIsOn = !soundIsOn;
		break;
	case FSKEY_L:
		loadFile();
		theEntity->reset();
		zoomAll();
		//showMenu();
		break;
	case FSKEY_R:
		theEntity->reset();
		break;
	case FSKEY_H:
		theEntity->toggleTorchShape();
		break;
	case FSKEY_PLUS:
		theEntity->increaseTorch();
		break;
	case FSKEY_MINUS:
		theEntity->decreaseTorch();
		break;
	case FSKEY_W:
		saveFile();
		//showMenu();
		break;
	case FSKEY_C:
		createNewMaze();
		theEntity->reset();
		break;
	case FSKEY_S:
	case FSKEY_E:
		modelMouseLoc = getModelCoords(screenX, screenY);
		theMaze->changeCell(modelMouseLoc.col, modelMouseLoc.row, key);
		break;
	case FSKEY_G:
		if (pathIsFound)
			theEntity->clearShortestPath();
		else {
			theEntity->findShortestPath();
			showVisited = false;
		}
		pathIsFound = !pathIsFound;
		break;
	case FSKEY_Z:
		zoomAll();
		break;
	case FSKEY_V:
		showVisited = !showVisited;
		break;
	}

	// handle mouse events

	if (mouseEvent == FSMOUSEEVENT_RBUTTONDOWN || rightButton) { // also do it if right button is held down
		//if (FsGetKeyState(FSKEY_SHIFT))

	}

	// handle mouse input

	if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN
		|| mouseEvent == FSMOUSEEVENT_MBUTTONDOWN
		|| mouseEvent == FSMOUSEEVENT_RBUTTONDOWN
		|| key == FSKEY_WHEELUP || key == FSKEY_WHEELDOWN) {
		prevScreenX = screenX; // need it for pan and zoom
		prevScreenY = screenY; // need it for pan and zoom
	}

	if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		prevScreenX = screenX; prevScreenY = screenY;

	}

	if (middleButton || (mouseEvent == FSMOUSEEVENT_MOVE && leftButton
		&& FsGetKeyState(FSKEY_CTRL))) {

		// pan the view
		originX += (prevScreenX - screenX) / scale;
		originY += (prevScreenY - screenY) / scale;

		prevScreenX = screenX; prevScreenY = screenY;
	}

	if (key == FSKEY_WHEELUP || key == FSKEY_WHEELDOWN
		|| (mouseEvent == FSMOUSEEVENT_MOVE && leftButton
			&& FsGetKeyState(FSKEY_SHIFT))) {
		// zoom the view
		float oldScale = scale;  // need to hold this value
		if (key == FSKEY_WHEELUP || screenY < prevScreenY)
			scale *= scaleFactor;
		else if (key == FSKEY_WHEELDOWN || screenY > prevScreenY)
			scale /= scaleFactor;

		// adjust originX and originY so that we can zoom in/out on
		// a specific spot
		originX += screenX / oldScale - screenX / scale;
		originY += screenY / oldScale - screenY / scale;

		prevScreenX = screenX; prevScreenY = screenY;
	}

	if (leftButton) {
		// write coords on screen if left button is held down
		modelMouseLoc = getModelCoords(screenX, screenY);
		Cell currCell = theMaze->getCellFromMouse(modelMouseLoc.col, modelMouseLoc.row);
		string message = to_string(currCell.row) + ", " + to_string(currCell.col);
		glColor3ub(100, 100, 100);
		glLoadIdentity();
		DrawingUtilNG::drawRectangle(screenX + 1, screenY + 5, 80, 40, true);
		arial.drawText(message, screenX + 8, screenY + 35, 0.25);
	}

	return true;
}

void MazeManager::loadFile()
{
	bool continueAsking = true;
	string fileToOpen;
	string errorString = " ";
	string lowerCaseFile;

	glLoadIdentity();  // get rid of zoom and pan

	while (continueAsking) {
		glLoadIdentity();
		fileToOpen = DrawingUtilNG::getStringFromScreen(errorString,
			"Enter file name of maze to load (use a, b, c as shortcuts).",
			"Press ENTER when done, ESC to cancel.");

		if (fileToOpen.length() > 0) {
			// so I don't have to type so much during testing
			if (fileToOpen == "a")
				fileToOpen = "MazeA_10x10.map";
			else if (fileToOpen == "b")
				fileToOpen = "MazeB_20x20.map";
			else if (fileToOpen == "c")
				fileToOpen = "MazeC_20x20.map";
			else {
				lowerCaseFile = StringPlus::lowerCase(fileToOpen);
				if (lowerCaseFile.find(".map") == string::npos)
					fileToOpen += ".map";     // allows user to omit extension
			}

			ifstream inFile(fileToOpen);
			if (inFile.is_open()) {
				theMaze->readFile(inFile);
				continueAsking = false;
				inFile.close();
			}
			else {
				errorString = "Could not open file >" + fileToOpen + "< for input";
			}
		}
		else // blank entry for filename so cancel load file
			continueAsking = false;
	}
}

void MazeManager::saveFile()
{
	bool continueAsking = true;
	string fileToSave;
	string errorString = " ";
	string lowerCaseFile;

	glLoadIdentity();  // get rid of zoom and pan

	while (continueAsking) {
		fileToSave = DrawingUtilNG::getStringFromScreen(errorString,
			"Enter name of file to save.",
			"Press ENTER when done, ESC to cancel.");

		if (fileToSave.length() > 0) {
			lowerCaseFile = StringPlus::lowerCase(fileToSave);
			if (lowerCaseFile.find(".map") == string::npos)
				fileToSave += ".map";     // allows user to omit extension

			ofstream outFile(fileToSave);
			if (outFile.is_open()) {
				outFile << *theMaze;  // note the asterisk
				outFile.close();
				continueAsking = false;
			}
			else {
				errorString = "Could not open file >" + fileToSave + "< for output";
			}
		}
		else // blank entry for filename so cancel save file
			continueAsking = false;

	}
}

void MazeManager::showMenu()
{
	cout << endl;
	cout << "Instructions: " << endl;
	cout << "    Arrows to move entity." << endl;
	cout << "    'R' to Reset entity to start" << endl;
	cout << "    'C' to Create a new maze" << endl;
	cout << "    'L' to Load a maze file" << endl;
	cout << "    'W' to write maze to file" << endl;
	cout << "    'H' to Hide/unHide map" << endl;
	cout << "       '+/-' to increase/decrease torch size" << endl;
	cout << "    'Q' to toggle sound Quiet/loud" << endl;
	cout << endl;
	cout << "    'Z' for zoom all." << endl;
	cout << "    Middle button or Ctrl drag to pan" << endl;
	cout << "    Mouse wheel turn or Shift drag to zoom" << endl;
	cout << endl;
	cout << "  Letter on a square to set start/end" << endl;
	cout << "     'S' for start" << endl;
	cout << "     'E' for end" << endl;
	cout << endl;
	cout << "  Path Finding (press letter again to hide)" << endl;
	cout << "    'G' use BFS to find shortest path from entity to end." << endl;
	cout << "    'V' show Visited squares in path-finding" << endl;
	cout << endl;
}

void MazeManager::zoomAll()
{
	originX = originY = 0.;
	scale = 1.0;

	if (theMaze->getWidth() > 0) {
		float scaleX, scaleY;
		int hei, wid;
		FsGetWindowSize(wid, hei);
		scaleX = wid / float((theMaze->getWidth() + 2) * Maze::MAZE_BLOCK_SIZE);
		scaleY = hei / float((theMaze->getHeight() + 2) * Maze::MAZE_BLOCK_SIZE);

		scale = min(scaleX, scaleY);
		if (scale == scaleX) // adjust originY so as to center vertically
			originY -= 0.5 * (hei / scale - ((theMaze->getHeight() + 2) * Maze::MAZE_BLOCK_SIZE));
		else // adjust originX so as to center horizontally
			originX -= 0.5 * (wid / scale - ((theMaze->getWidth() + 2) * Maze::MAZE_BLOCK_SIZE));
	}

}

Cell MazeManager::getModelCoords(int screenX, int screenY) {

	// note row is Y and column is X
	return { int(screenY / scale + originY), int(screenX / scale + originX) };
}
 
void MazeManager::createNewMaze()
{
	bool continueAsking = true;
	int width, height;

	// user input for new maze's width and height
	glLoadIdentity();  // get rid of zoom and pan

	// take new width as string and convert to int
	string widthString = DrawingUtilNG::getStringFromScreen( 
		"Enter custom width for the new maze (1-100): ", 
		"Press ENTER when done, ESC to cancel.");

	width = atoi(widthString.c_str());

	// take new height as string and convert to int
	string heightString = DrawingUtilNG::getStringFromScreen( 
		"Enter custom height for the new maze (1-100): ",
		"Press ENTER when done, ESC to cancel.");

	height = atoi(heightString.c_str());

	// create new maze using Aldous-Broder algorithm (from createRandom function in maze.cpp)
	theMaze->createRandom(width, height);
	
	// zoom to fit the new maze
	zoomAll();
}
