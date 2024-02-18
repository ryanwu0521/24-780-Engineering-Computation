/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 8: Ortho Maze Again
Due: Tues. Nov. 14, 2023
Program overall: Maze navigation using keyboard to create simple interactive game
This is the cpp file of the MazeManager class with all it's declared functions.
Tasked feature: created the A* algorithm to find the shortest path and added full editable maze feature
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
		std::cout << "Can't find yesMoveSound data" << endl;
	if (YSOK != cantMoveSound.LoadWav("gulp_x.wav"))
		std::cout << "Can't find cantMoveSound data" << endl;
	theSoundPlayer.Start();

	theEntity = new Entity(theMaze);
	theEntity->reset();

	arial.setColorHSV(0, 0, 1, 1); // white

	// buttons
	addButtons();
	buttonFont.setColorRGB(0, 0, 0); // black


}

bool MazeManager::manage()
{
	Cell modelMouseLoc;

	FsPollDevice();
	key = FsInkey();
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);

	// check if a button was clicked
	buttonKey = FSKEY_NULL;  // this resets the buttonKey if no buttons are clicked
	if (key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		buttonKey = myButtons.checkClick(screenX, screenY);

		if (buttonKey != FSKEY_NULL)
			key = buttonKey; // pretend the user pressed a key 
	}

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

	glLoadIdentity();
	myButtons.paint();
	myButtons.checkHover(screenX, screenY); // remove hover feedback for better performance ?
	if (pathIsFound && !theEntity->pathIsAvailable()) {
		arial.setColorRGB(1, 0, 0);
		arial.drawText("No path is possible", 400, 400, .7, 38., true);
		arial.setColorRGB(0, 0, 0);
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
		setButtonStates();
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
		setButtonStates();
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
	case FSKEY_A:  // A* algorithm
		if (pathIsFound)
			theEntity->clearShortestPath();
		else {
			theEntity->findShortestPathAStar();
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

		// these are just for fun
	case FSKEY_T:
		theMaze->quarterTurn();
		if (FsGetKeyState(FSKEY_SHIFT)) {
			theMaze->quarterTurn();
			theMaze->quarterTurn();
		}
		theEntity->reset();
		showVisited = false;
		pathIsFound = false;
		zoomAll();
		break;
	case FSKEY_F:
		theMaze->mirrorLeftRight();
		//theMaze->alter(Maze::MIRROR_LEFT_RIGHT);
		theEntity->reset();
		showVisited = false;
		pathIsFound = false;
		zoomAll();
		break;
	case FSKEY_U:
		theMaze->mirrorUpDown();
		//theMaze->alter(Maze::MIRROR_UP_DOWN);
		theEntity->reset();
		showVisited = false;
		pathIsFound = false;
		zoomAll();
		break;
	case FSKEY_M:
		theMaze->mirrorOnDiagonal();
		theEntity->reset();
		showVisited = false;
		pathIsFound = false;
		zoomAll();
		break;

	}

	// handle mouse input
	if (buttonKey == FSKEY_NULL) {
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

		// toggle connections between cells with right click (PS08 Task 2)
		if (mouseEvent == FSMOUSEEVENT_RBUTTONDOWN) {
			// Get the cell that was clicked on
			modelMouseLoc = getModelCoords(screenX, screenY);
			Cell currCell = theMaze->getCellFromMouse(modelMouseLoc.col, modelMouseLoc.row);
			Cell otherCell = currCell;

			// Get the center of the maze
			double centerX = ((double)currCell.col + 0.5) * (double)Maze::MAZE_BLOCK_SIZE;
			double centerY = ((double)currCell.row + 0.5) * (double)Maze::MAZE_BLOCK_SIZE;

			// Get the difference between the mouse position and the center of the maze
			int delX = modelMouseLoc.col - centerX;
			int delY = modelMouseLoc.row - centerY;

			// Toggle cell connections based on mouse position
			if (abs(delX) > 0.4 * Maze::MAZE_BLOCK_SIZE && abs(delY) < 0.4 * Maze::MAZE_BLOCK_SIZE) {
				otherCell.col += delX / abs(delX);
				MazeManager::toggleConnection(theMaze, currCell, otherCell);
			}

			if (abs(delY) > 0.4 * Maze::MAZE_BLOCK_SIZE && abs(delX) < 0.4 * Maze::MAZE_BLOCK_SIZE) {
				otherCell.row += delY / abs(delY);
				MazeManager::toggleConnection(theMaze, currCell, otherCell);
			}
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
			arial.setColorRGB(1, 1, 1);
			arial.drawText(message, screenX + 8, screenY + 35, 0.25);
		}
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
			"Enter file name of maze to load (use a, b as shortcuts).",
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
	std::cout << endl;
	std::cout << "Instructions: " << endl;
	std::cout << "    Arrows to move entity." << endl;
	std::cout << "    'R' to Reset entity to start" << endl;
	std::cout << "    'C' to Create a new maze" << endl;
	std::cout << "    'L' to Load a maze file" << endl;
	std::cout << "    'W' to write maze to file" << endl;
	std::cout << "    'H' to Hide/unHide map" << endl;
	std::cout << "       '+/-' to increase/decrease torch size" << endl;
	std::cout << "    'Q' to toggle sound Quiet/loud" << endl;
	std::cout << endl;
	std::cout << "    'Z' for zoom all." << endl;
	std::cout << "    Middle button or Ctrl drag to pan" << endl;
	std::cout << "    Mouse wheel turn or Shift drag to zoom" << endl;
	std::cout << endl;
	std::cout << "  Letter on a square to set start/end" << endl;
	std::cout << "     'S' for start" << endl;
	std::cout << "     'E' for end" << endl;
	std::cout << endl;
	std::cout << "  Path Finding (press letter again to hide)" << endl;
	std::cout << "    'G' use BFS to find shortest path from entity to end." << endl;
	std::cout << "    'V' show Visited squares in path-finding" << endl;
	std::cout << endl;
}

void MazeManager::zoomAll()
{
	originX = originY = 0.;
	scale = 1.0;

	if (theMaze->getWidth() > 0) {
		float scaleX, scaleY;
		int hei, wid;
		FsGetWindowSize(wid, hei);
		wid -= buttonWidth * 1.2;
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
	// ask for maze size
	bool continueAsking = true;
	string userInput;
	string errorString = " ";
	int wid, hei;

	glLoadIdentity();  // get rid of zoom and pan

	while (continueAsking) {
		userInput = DrawingUtilNG::getStringFromScreen(errorString,
			"Enter maze width and height to generate new random maze",
			"Two values, between 2 and 100 (ESC to cancel)");

		if (userInput.length() > 0) {
			// take two values, either space or comman separating them
			vector<string> splitInput = StringPlus::split(userInput, " ,xX");
			try {
				hei = stoi(splitInput.at(1));
				wid = stoi(splitInput.at(0));
				if (1 < wid && wid <= 100 && 1 < hei && hei <= 100) {
					continueAsking = false;
					theMaze->createRandom(wid, hei);
					zoomAll();
					theEntity->reset();
				}
				else
					errorString = "Values for width and height were not within bounds";
			}
			catch (exception e) {
				errorString = "ERROR: Unable to convert ->" + userInput
					+ "<- into numbers";;
			}
		}
		else // blank entry for input so cancel 
			continueAsking = false;

	}
}

void MazeManager::addButtons()
{
	int hei = 30;
	int spacing = 10;

	int currY = 10;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_L, "Load", &buttonFont,
		"Load a maze from a file");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_W, "Write", &buttonFont,
		"Save current maze to a file");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_H, "Hide", &buttonFont,
		"Hides map except for area of torch. Use +/- to control size of torch. Click again to change shape");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_Q, "Sound", &buttonFont,
		"Toggle sound feedback on/off");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_R, "Reset", &buttonFont,
		"Clear path travelled and put entity back at start");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_C, "Create", &buttonFont,
		"Erase current maze and start a random maze of given size");

	currY += hei + spacing * 3;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_T, "Rotate", &buttonFont,
		"Rotate maze 90deg counter-clockwise, switching width and height dimensions.");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_F, "Flip L/R", &buttonFont,
		"Mirror maze left/right (about vert centerline)");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_U, "Flip U/D", &buttonFont,
		"Mirror maze up/down (about horz centerline)");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_M, "Mirror", &buttonFont,
		"Mirror maze about diagonal line (both L/R and U/D), switching width and height dimensions.");

	currY += hei + spacing * 3;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_G, "Path BFS", &buttonFont,
		"Find shortest path using breadth-first search. Click again to hide.");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_A, "Path A*", &buttonFont,
		"Find shortest path using A* algorithm. Click again to hide.");

	currY += hei + spacing;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_V, "Searched", &buttonFont,
		"Show squares that were visited during search. Click again to hide");

	currY += hei + spacing * 10;
	myButtons.add(buttonX, currY, buttonWidth, hei, FSKEY_ESC, "Exit", &buttonFont,
		"Exit program");

	setButtonStates();

	// to disable a button (will gray out and won't return its value)
	//myButtons.disableButton(FSKEY_T);
	//myButtons.disableButton(FSKEY_M);
}

void MazeManager::setButtonStates()
{
	if (theMaze == nullptr || theMaze->getWidth() < 2) {
		myButtons.disableButton(FSKEY_W);
		myButtons.disableButton(FSKEY_H);
		myButtons.disableButton(FSKEY_R);
		myButtons.disableButton(FSKEY_G);
		myButtons.disableButton(FSKEY_V);
		myButtons.disableButton(FSKEY_F);
		myButtons.disableButton(FSKEY_U);
		myButtons.disableButton(FSKEY_T);
		myButtons.disableButton(FSKEY_M);
		myButtons.disableButton(FSKEY_T);
		myButtons.disableButton(FSKEY_M);

		myButtons.disableButton(FSKEY_A);  // A* algorithm
	}
	else {
		myButtons.enableButton(FSKEY_W);
		myButtons.enableButton(FSKEY_H);
		myButtons.enableButton(FSKEY_Q);
		myButtons.enableButton(FSKEY_R);
		myButtons.enableButton(FSKEY_G);
		myButtons.enableButton(FSKEY_V);
		myButtons.enableButton(FSKEY_F);
		myButtons.enableButton(FSKEY_U);
		myButtons.enableButton(FSKEY_T);
		myButtons.enableButton(FSKEY_M);

		myButtons.enableButton(FSKEY_A);
	}
}

//// Function for toggling connections between cells
void MazeManager::toggleConnection(Maze* maze, const Cell& cell, const Cell& otherCell)
{
	if (maze == nullptr)
	{
		return;
	}

	// Check if the cells are valid
	if (cell.row < 0 || cell.row >= maze->getHeight() || cell.col < 0 || cell.col >= maze->getWidth() ||
		otherCell.row < 0 || otherCell.row >= maze->getHeight() || otherCell.col < 0 || otherCell.col >= maze->getWidth())
	{
		return;
	}

	// Check if the cells are the same
	if (cell.row == otherCell.row && cell.col == otherCell.col)
	{
		return;
	}

	// Toggle the connection between the two cells
	if (maze->areConnected(cell, otherCell))
	{
		maze->disconnect(cell, otherCell);
		maze->disconnect(otherCell, cell);
	}
	else
	{
		maze->connect(cell, otherCell);
		maze->connect(otherCell, cell);
	}

	// Reset the entity
	if (theEntity != nullptr)
	{
		theEntity->reset();
	}
}


