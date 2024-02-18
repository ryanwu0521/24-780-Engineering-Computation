/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 10: Shape
Due: Tues. Nov. 28, 2023
Program overall: create a shape class and its derived classes, and use them to create a GUI with interesting features.
This is the cpp file of ShapeManager with all it's functions and features.
Tasked feature: create the reccalcShape function to determine the shape's properties (the function is called using the w key).
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include "ShapeManager.h"
#include "ysglfontdata.h"
#include "yssimplesound.h"

using namespace std;

ShapeManager::ShapeManager(int width, int height)
{
	// for testing, just load 3 shapes (comment out when done)
	ifstream inFile;
	inFile.open("spur01.shape");
	theShapes.push_back(Shape2D(inFile));
	inFile.close();
	inFile.open("sample01.shape");
	theShapes.push_back(Shape2D(inFile));
	inFile.close();
	inFile.open("rectangle01.shape");
	theShapes.push_back(Shape2D(inFile));
	inFile.close();
	resetView();

	winWidth = width;
	winHeight = height;

	// added 300 pixels on right of window for the buttons
	FsOpenWindow(16, 16, winWidth + 160, winHeight, 1, "Shape Manager (RW 2023)");

	// set color and width
	glColor3b(255, 0, 0);  // red
	//glLineWidth(3);

		// load sounds and start player
	if (YSOK != insideSound.LoadWav("cash_register.wav"))
		cout << "   ERROR: Unable to load cash_register.wav " << endl;
	if (YSOK != outsideSound.LoadWav("buzzer.wav"))
		cout << "   ERROR: Unable to load cash_register.wav " << endl;
	theSoundPlayer.Start();

	// create the buttons
	buttonFont = new ComicSansFont();
	buttonX = winWidth + 30;
	addButtons();
}

void ShapeManager::addButtons()
{
	int hei = 30;
	int spacing = 8;

	int currY = 10;
	myControls.add(buttonX, currY, buttonWidth, hei, FSKEY_L, "Load", buttonFont,
		"Load shape from file and add it to the manager");

	currY += hei + spacing;
	myControls.add(buttonX, currY, buttonWidth, hei, FSKEY_R, "Zoom All", buttonFont,
		"Reset the view so all shapes fit");

	currY += hei + spacing;
	myControls.add(buttonX, currY, buttonWidth / 2 - 2, hei, FSKEY_F, "Fill", buttonFont,
		"Show all shapes Filled");

	//currY += hei + spacing;
	myControls.add(buttonX + buttonWidth / 2 + 2, currY, buttonWidth / 2 - 2, hei, FSKEY_C, "Clear", buttonFont,
		"Clear all shapes");

	currY += hei + spacing * 3;
	myControls.add(buttonX, currY, buttonWidth, hei, FSKEY_E, "Edit", buttonFont,
		"Toggle edit mode on/off");

	currY += hei + spacing;
	myControls.add(buttonX, currY, buttonWidth, hei, FSKEY_P, "Show Pnts", buttonFont,
		"Toggle showing of Points");

	currY += hei + spacing;
	myControls.add(buttonX, currY, buttonWidth / 2 - 2, hei, FSKEY_COMMA, "<<", buttonFont,
		"Cycle through shapes");
	myControls.add(buttonX + buttonWidth / 2 + 2, currY, buttonWidth / 2 - 2, hei, FSKEY_DOT, ">>", buttonFont,
		"Cycle through shapes");

	currY += hei + spacing * 1;
	myControls.add(buttonX, currY, buttonWidth, hei, FSKEY_Z, "Zoom", buttonFont,
		"Zoom into just the current shape");

	currY += hei + spacing;
	myControls.add(buttonX, currY, buttonWidth, hei, FSKEY_S, "Save", buttonFont,
		"Save the current shape");

	currY += hei + spacing;
	myControls.add(buttonX, currY, buttonWidth, hei, FSKEY_W, "Props", buttonFont,
		"Show all calculated properties for current shape");

	currY += hei + spacing;
	myControls.add(buttonX, currY, buttonWidth, hei, FSKEY_H, "Color", buttonFont,
		"Change Hue angle of shape by +10 deg");


	currY += hei + spacing;


	// Creating a combox box

	// initialize like a button with a value (4th param) that is a multiple of 1000

	ComboBox* newBox = new ComboBox(buttonX - 50, currY, buttonWidth + 50, hei,
		1000, "ColorBox", buttonFont, "Use this to select color for current shape");

	// add items in the order they should appear
	newBox->addItem("Red");
	newBox->addItem("Yellow");
	newBox->addItem("Green");
	newBox->addItem("Cyan");
	newBox->addItem("Blue");
	newBox->addItem("Magenta");

	// add the combo box to the collection
	myControls.add(newBox);

	setButtonStates();

}

void ShapeManager::setButtonStates()
{
	if (!inEditMode) {
		myControls.disableButton(FSKEY_W);
		myControls.disableButton(FSKEY_P);
		myControls.disableButton(FSKEY_COMMA);
		myControls.disableButton(FSKEY_DOT);
		myControls.disableButton(FSKEY_Z);
		myControls.disableButton(FSKEY_H);
		myControls.disableButton(FSKEY_W);
		myControls.disableButton(FSKEY_S);
		myControls.disableButton(1000);
	}
	else {
		myControls.enableButton(FSKEY_W);
		myControls.enableButton(FSKEY_P);
		myControls.enableButton(FSKEY_COMMA);
		myControls.enableButton(FSKEY_DOT);
		myControls.enableButton(FSKEY_Z);
		myControls.enableButton(FSKEY_H);
		myControls.enableButton(FSKEY_W);
		myControls.enableButton(FSKEY_S);
		myControls.enableButton(1000);
	}

}

void ShapeManager::showMenu()
{
	cout << "\n\n";
	cout << "Use these keys on the screen:" << endl;
	cout << " Arws : pan in all directions (also Ctrl+Drag)" << endl;
	cout << "  +/- : zoom in and out (also Shft+Drag)" << endl;
	cout << "    R : Reset the view so all shapes fit" << endl;
	cout << "    F : show all shapes Filled" << endl;

	cout << endl;
	cout << "    L : Load shape from file" << endl;
	cout << "    C : Clear all shapes" << endl;
	cout << endl;

	cout << "    E : toggle Edit mode" << endl;
	cout << "      \< \> : cycle through shapes to select current shape" << endl;	// (use FSKEY_COMMA and FSKEY_DOT)
	cout << "        P : toggle showing of Points" << endl;
	cout << "        H : change Hue angle of shape by +10 deg" << endl;
	cout << "        Z : Zoom into just the current shape" << endl;
	cout << "        S : Save the current shape" << endl;
	cout << "        W : Show all calculated properties for current shape" << endl;
	cout << endl;

	cout << "  ESC : exit program" << endl;
}

void ShapeManager::resetView()
{
	if (theShapes.size() > 0) {
		Point2D currLow, currUpper;
		Point2D overallLow = theShapes[0].getLowerBound();
		Point2D overallUpper = theShapes[0].getUpperBound();

		for (auto& aShape : theShapes) {
			currLow = aShape.getLowerBound();
			currUpper = aShape.getUpperBound();
			overallLow.x = min(overallLow.x, currLow.x);
			overallLow.y = min(overallLow.y, currLow.y);
			overallUpper.x = max(overallUpper.x, currUpper.x);
			overallUpper.y = max(overallUpper.y, currUpper.y);
		}
		resetView(overallLow, overallUpper);
	}
}

void ShapeManager::resetView(Point2D lowerB, Point2D upperB)
{
	int extraBorder = 24; // allows for some space around all shape(s)

	// Determine needed scale to completely fill in the screen.
	// We do it for each direction and then choose smallest as overall 
	// new scale, leaving blank space for other direction 

	float horzScale = (winWidth - extraBorder) / (upperB.x - lowerB.x);
	float vertScale = (winHeight - extraBorder) / (upperB.y - lowerB.y);
	scale = min(horzScale, vertScale);

	// To adjust panX and panY, we need to put center of all shapes
	// at the center of screen. Thus, we first pan to center of screen
	// and then pan from center of shapes (average of shape bounds)
	// to world 0,0  , converted to pixels. Note minus for x (moving left)
	// and plus for y (moving down) since we are using pixel dimensions

	panX = winWidth / 2. - (upperB.x + lowerB.x) / 2. * scale;
	panY = winHeight / 2. + (upperB.y + lowerB.y) / 2. * scale;
}

void ShapeManager::paintEditIndicator()
{
	bool useAwful = false;

	if (!useAwful) {
		// draw yellow square all around, with the word "EDIT" at top right and
		// bottom left corners

		// box around (big yellow rectangle with slightly smaller white rectangle)
		glColor3ub(255, 255, 0); // yellow
		glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(winWidth, 0);
		glVertex2i(winWidth, winHeight);
		glVertex2i(0, winHeight);
		glEnd();

		glColor3ub(255, 255, 255); // white
		int frameThickness = 5;
		glBegin(GL_QUADS);
		glVertex2i(frameThickness, frameThickness);
		glVertex2i(winWidth - frameThickness - 1, frameThickness);
		glVertex2i(winWidth - frameThickness - 1, winHeight - frameThickness - 1);
		glVertex2i(frameThickness, winHeight - frameThickness - 1);
		glEnd();

		// little boxes for EDIT label
		glColor3ub(255, 255, 0); // yellow
		int boxW = 40, boxH = 18;
		glBegin(GL_QUADS);
		glVertex2i(winWidth - boxW, 0);
		glVertex2i(winWidth, 0);
		glVertex2i(winWidth, boxH);
		glVertex2i(winWidth - boxW, boxH);
		glEnd();

		glBegin(GL_QUADS);
		glVertex2i(0, winHeight - boxH);
		glVertex2i(boxW, winHeight - boxH);
		glVertex2i(boxW, winHeight);
		glVertex2i(0, winHeight);
		glEnd();

		// label "EDIT"
		glColor3ub(0, 0, 0); // black
		glRasterPos2i(winWidth - boxW + 3, boxH - 3);  // top right
		YsGlDrawFontBitmap8x12("EDIT");
		glRasterPos2i(3, winHeight - 3);  // bottom left
		YsGlDrawFontBitmap8x12("EDIT");
	}
	else {
		// awful edit indicator (but simple to code :-)
		glColor3ub(0, 0, 0); // black
		glRasterPos2i(0, 30);  // top right
		YsGlDrawFontBitmap20x28("YOU ARE NOW IN EDIT MODE");
	}
}

Point2D ShapeManager::getScreenCoords(Point2D worldCoords)
{
	float screenX = worldCoords.x * scale + panX;
	float screenY = worldCoords.y * -scale + panY;
	return { screenX, screenY };
}

Point2D ShapeManager::getWorldCoords(Point2D screenCoords)
{
	float worldX = (screenCoords.x - panX) / scale;
	float worldY = (screenCoords.y - panY) / -scale;
	return { worldX, worldY };
}

void ShapeManager::manageMouse()
{
	// handle mouse input (OpenGL is still in screen coords)
	stringstream coordStream;     // for displaying coordinates on screen

	//mouseEvent = FsGetMouseEvent(leftButton, middleButton,
	//	rightButton, screenX, screenY);

	Point2D worldPnt = getWorldCoords({ screenX * 1.f, screenY * 1.f });

	if (leftButton) { // write coords on screen if left button is held down
		coordStream.str("");  // reset stream
		coordStream.precision(4);
		coordStream << worldPnt.x << ", " << worldPnt.y
			<< " (" << screenX << ", " << screenY << ")";
		glColor3ub(60, 230, 60);
		glRasterPos2i(screenX, screenY - 3);  // set position 3 pix above
		YsGlDrawFontBitmap7x10(coordStream.str().c_str());
	}

	if (inEditMode) {
		float hoverDistance = 3.f / scale; // 3 pixels

		if (mouseEvent == FSMOUSEEVENT_RBUTTONDOWN) {
			if (theShapes.at(currShape).isContained(worldPnt)) {
				theSoundPlayer.PlayOneShot(insideSound);
			}
			else
				theSoundPlayer.PlayOneShot(outsideSound);
		}

		// figure out if there's a point near mouse location
		if (!vertexIsMoving) { // avoid changing currVertex while moving a vertex
			currVertex = theShapes.at(currShape).getIndex(worldPnt, hoverDistance);
		}
		if (mouseEvent == FSMOUSEEVENT_LBUTTONUP) {
			vertexIsMoving = false;
		}
		else if (leftButton && mouseEvent == FSMOUSEEVENT_MOVE && currVertex > 0) {
			vertexIsMoving = true;
			theShapes.at(currShape).movePoint(worldPnt, currVertex);
		}
	}

	// capture location of first button press (needed for panning and zooming)
	if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN || mouseEvent == FSMOUSEEVENT_MBUTTONDOWN) {
		prevScreenX = screenX; prevScreenY = screenY;
	}

	// disallow panning and zooming with mouse when a vertex is moving
	if (!vertexIsMoving) {

		// pan in x and y axes when Ctrl key is held down and left button is down
		// note: I added middle button (wheel) drag for panning
		if (middleButton || (FsGetKeyState(FSKEY_CTRL) && leftButton)) {
			// no need for scale since the screen-to-model ratio is not applicable
			panX += (screenX - prevScreenX);
			panY += (screenY - prevScreenY);
			prevScreenX = screenX; prevScreenY = screenY; // reset previous values to continue move
		}

		// zoom in and out when Shft key is held down and left button is down
		// note: I added wheel rolling for zoomimg, which accidentally is also
		//       triggered by touchpad pinch and two finger scroll
		else if (key == FSKEY_WHEELUP || key == FSKEY_WHEELDOWN
			|| (FsGetKeyState(FSKEY_SHIFT) && leftButton)) {
			double oldScale = scale;
			if (key == FSKEY_WHEELUP)
				scale *= 1.03; // less jumpy than zooming with +/- keys
			else if (key == FSKEY_WHEELDOWN)
				scale /= 1.02;
			else if (screenY < prevScreenY)
				scale *= max(1.02, (prevScreenY - screenY) * 0.1);
			else if (screenY > prevScreenY)
				scale /= max(1.02, (prevScreenY - screenY) * -0.1);

			// adjust panX and panY so point under mouse does not move
			// i.e., we can zoom in/out on a specific point
			// a bit complicated since you have to convert old origin to screen coords
			// then adjust pan, then convert to model coords. 
			// what you see below is the simplified equation after all substitutions
			// rounding reduces "shifting"
			if (key == FSKEY_WHEELUP || key == FSKEY_WHEELDOWN) {
				panX = (int)round((screenX * (oldScale - scale)
					+ panX * scale) / oldScale);
				panY = (int)round((screenY * (oldScale - scale)
					+ panY * scale) / oldScale);
			}
			prevScreenX = screenX; prevScreenY = screenY; // reset previous values to continue move
		}
	}
}

bool ShapeManager::manage()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// reset all transformations
	glLoadIdentity();

	FsPollDevice();
	key = FsInkey();

	// take care of buttons and combo boxes
	mouseEvent = FsGetMouseEvent(leftButton, middleButton,
		rightButton, screenX, screenY);

	// just to save on processing, make it so that when a vertex is being edited
	// don't check for any other event (click on button control, letter pressing, etc.)

	if (!vertexIsMoving) {
		buttonKey = FSKEY_NULL;
		if (key == FSKEY_NULL && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
			buttonKey = myControls.checkClick(screenX, screenY);

			if (buttonKey != FSKEY_NULL)
				key = buttonKey;  // pretend the user pressed a key 
		}


		switch (key) {
		case FSKEY_L: // load a file and add to shapes
			cout << endl << "   Enter name of shape file to load > ";
			cin >> fileName;
			if (fileName.length() > 0) {
				if (fileName.find(".shape") == string::npos)
					fileName += ".shape";

				inFile.open(fileName);
				if (inFile.is_open()) {
					theShapes.push_back(Shape2D(inFile));
					inFile.close();
					cout << "       A new shape was added from file " << fileName << endl;
					cout << "       There are now " << theShapes.size() << " shapes." << endl;

					// change zoom to show newly added shape
					resetView(theShapes.back().getLowerBound(),
						theShapes.back().getUpperBound());
				}
				else
					cout << "       ERROR: Was not able to open file " << fileName << endl;
			}
			showMenu(); // So that it is "fresh"
			break;
		case FSKEY_C: // clear all shapes (with confirmation)
			cout << "Are you sure you want to clear all shapes (Y/N) >> ";
			cin >> userChoice;
			if (userChoice == 'Y' || userChoice == 'y') {
				theShapes.clear(); // empties all members of a vector (kills memory too)
			}
			showMenu(); // So that it is "fresh"
			break;
		case FSKEY_RIGHT: panX += 5;
			break;
		case FSKEY_LEFT: panX -= 5;
			break;
		case FSKEY_UP: panY -= 5;
			break;
		case FSKEY_DOWN: panY += 5;
			break;
		case FSKEY_PLUS: scale *= 1.05;
			break;
		case FSKEY_MINUS: scale /= 1.05;
			break;

			// new stuff for PS04
		case FSKEY_R:   // reset Zoom to fit all shapes
			resetView();
			break;
		case FSKEY_E:
			if (!inEditMode && theShapes.size() < 1)
				cout << "Nothing to Edit." << endl;
			else {
				inEditMode = !inEditMode;
				setButtonStates();
				if (inEditMode && currShape < 0) // take care of first edit mode
					currShape = 0;
			}
			break;
		case FSKEY_F:   // show shapes filled
			useFillMode = !useFillMode;
			break;
		}

		if (inEditMode) {
			paintEditIndicator();
			ofstream outFile;
			switch (key) {
			case FSKEY_W: // show all calculated properties for current shape
				if (0 <= currShape && currShape < theShapes.size()) {
					cout << endl << "Properties for current shape:" << endl;
					
					// recalculate and print properties
					theShapes.at(currShape).recalcShape();
					string props = theShapes.at(currShape).outputProps();
					cout << props << endl;
				}
				break;
			case FSKEY_S: // save current shape (similar to load shape above)
				cout << "Filename for saving current shape >> ";
				cin >> fileName;

				if (fileName.find(".shape") == string::npos)
					fileName += ".shape";

				outFile.open(fileName);
				if (outFile.is_open()) {
					outFile << theShapes.at(currShape);
					outFile.close();
					cout << "Just saved " << fileName << " to current folder." << endl;

				}
				else {
					cout << "Cannot open file " << fileName << " for outFile." << endl;
				}
				showMenu();
				break;
			case FSKEY_COMMA:   // also the '<' key, change currShape
				if (theShapes.size() > 0) {
					currShape--;
					if (currShape < 0)
						currShape = theShapes.size() - 1;
				}
				break;
			case FSKEY_DOT:   // also the '>' key, change currShape
				if (theShapes.size() > 0) {
					currShape++;
					if (currShape >= theShapes.size())
						currShape = 0;
				}
				break;
			case FSKEY_P:  // toggle showing points for current shape
				showThePoints = !showThePoints;
			case FSKEY_H:
				if (0 <= currShape && currShape < theShapes.size())
					theShapes.at(currShape).changeColor(10.f);
				break;
			case FSKEY_Z:  // zoom onto current shape
				if (0 <= currShape && currShape < theShapes.size())
					resetView(theShapes.at(currShape).getLowerBound(),
						theShapes.at(currShape).getUpperBound());
				break;
			default:
				if (key >= 1000) {
					// change color to selected color from combobox
					// note: key value is 1000 * combobox value + 0-100 for item selected
					theShapes.at(currShape).setColor((key % 1000) * 60.);

				}

			}

			if (currVertex > 0) {
				theShapes.at(currShape).paintPoint(currVertex, 5. / scale);
			}
		}
	}

	// draw edit mode indicator
	if (inEditMode)
		paintEditIndicator();

	// handle mouse input (OpenGL is still in screen coords)
	manageMouse();

	// display buttons
	myControls.paint();
	if (!vertexIsMoving)
		myControls.checkHover(screenX, screenY); // remove hover feedback for better performance ?

	// set up axes to "math normal" (origin at lower left, y-axis going up)
	// and pan and scale
	glTranslatef(panX, panY, 0);
	glScalef(scale, -scale, 1);

	//// highlight point that could be selected
	//if (inEditMode && currVertex > 0) {
	//	theShapes.at(currShape).paintPoint(currVertex, 6. / scale);
	//}

	// draw all the shapes, carefully setting parameters
	int i = 0;
	for (auto& aShape : theShapes) {
		if (inEditMode) {
			if (currShape == i) {
				glLineWidth(3);
				aShape.paint(true, useFillMode, showThePoints); // show the points maybe
				glLineWidth(1);
			}
			else
				aShape.paint(true, useFillMode, false, 60); // yellow
		}
		else
			aShape.paint(true, useFillMode, false); // use shape-maintained colors
		i++;
	}

	// highlight point that could be selected
	if (inEditMode && currVertex > 0) {
		glColor3ub(215, 215, 0);
		theShapes.at(currShape).paintPoint(currVertex, 6. / scale);
	}

	return key != FSKEY_ESC;
}

