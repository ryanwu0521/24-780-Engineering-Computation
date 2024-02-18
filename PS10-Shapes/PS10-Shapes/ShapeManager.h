/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 10: Shape
Due: Tues. Nov. 28, 2023
Program overall: create a shape class and its derived classes, and use them to create a GUI with interesting features.
This is the header file of ShapeManager with all it's functions and features.
Tasked feature: create the reccalcShape function to determine the shape's properties (the function is called using the w key).
*/

#pragma once

#include <vector>
#include <fstream>

#include "fssimplewindow.h"
#include "yssimplesound.h"
#include "Shape2D.h"
#include "GraphicFont.h"
#include "ControlCollection.h"

class ShapeManager {
private: 
	std::vector<Shape2D> theShapes;
	int currShape = -1;

	int winWidth = 800, winHeight = 600;
	int panX = 0, panY = winHeight;
	float scale = 6.f;

	std::string fileName;
	std::ifstream inFile;
	char userChoice = 'N';
	int key = FSKEY_NULL; // just to start it up

	bool inEditMode = false;
	bool showThePoints = false;
	bool useFillMode = false;

	// for moving a vertex with the mouse
	int currVertex = -1;
	bool vertexIsMoving = false;

	// audio feedback on inside/outside check
	YsSoundPlayer theSoundPlayer;
	YsSoundPlayer::SoundData insideSound;
	YsSoundPlayer::SoundData outsideSound;

	// stuff for mouse
	int mouseEvent, leftButton, middleButton, rightButton;
	int screenX, screenY;
	int prevScreenX = -1000, prevScreenY = -1000;

	// stuff for buttons
	int buttonKey = FSKEY_NULL;
	ControlCollection myControls;
	GraphicFont *buttonFont; // need to create font AFTER FsOpenWindow
	int buttonX = -1;
	int buttonWidth = 100;

public:
	ShapeManager() {};

	ShapeManager(int width, int height);

	bool manage();

	// displays full menu of user options on console
	void showMenu();

private:
	// resets pan and zoom so that all shapes fit on the window
	void resetView();

	// do the calculations to set values of panx, pany, and zoom
	// such that given bounding box fills the screen
	void resetView(Point2D lowerB, Point2D upperB);

	// if manager is in edit mode, this paints some indication on the graphics
	// screen that this is the case. Otherwise does nothing
	void paintEditIndicator();

	// uses current pan and zoom to convert coordinates from world to screen
	Point2D getScreenCoords(Point2D worldCoords);
	
	// uses current pan and zoom to convert coordinates from screen to world
	Point2D getWorldCoords(Point2D screenCoords);

	// handle mouse input (OpenGL is still in screen coords)
	void manageMouse();

	void addButtons();
	void setButtonStates();

};