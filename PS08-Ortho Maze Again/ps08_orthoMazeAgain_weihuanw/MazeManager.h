/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 8: Ortho Maze Again
Due: Tues. Nov. 14, 2023
Program overall: Maze navigation using keyboard to create simple interactive game
This is a header file for the MazeManager class.
Tasked feature: created the A* algorithm to find the shortest path and added full editable maze feature
*/

#pragma once
#include "yssimplesound.h"
#include "GraphicFont.h"
#include "Maze.h"
#include "Entity.h"
#include "ButtonCollection.h"


class MazeManager {
protected:
	Maze* theMaze;
	Entity* theEntity;

	bool showVisited = false;
	bool pathIsFound = false;
	bool pathIsPossible = false;

	YsSoundPlayer theSoundPlayer;

	YsSoundPlayer::SoundData yesMoveSound;
	YsSoundPlayer::SoundData cantMoveSound;

	int key = FSKEY_NULL;

	// for handling mouse events
	int mouseEvent;
	int leftButton, middleButton, rightButton;
	int prevScreenX, prevScreenY, screenX, screenY;

	// for zooming and panning
	float originX = 0., originY = 0., scale = 1.;
	float scaleFactor = 1.05;

	bool soundIsOn = true;

	ArialFont arial;

	// stuff for buttons
	int buttonKey = FSKEY_NULL;
	ButtonCollection myButtons;
	ComicSansFont buttonFont;
	int buttonX = 830;
	int buttonWidth = 100;


public:
	// PS07  PS07  PS07  PS07  PS07  
	// asks user for width and height and asks Maze object 
	// to regenerate a random maze
	void createNewMaze();

	MazeManager(Maze* aMaze);

	bool manage();

	void loadFile();
	void saveFile();
	void showMenu();
	void toggleConnection(Maze* aMaze, const Cell& cell, const Cell& adjacentCell);

	~MazeManager() {
		delete theEntity;
	}

	void zoomAll();

	Cell getModelCoords(int screenX, int screenY);

protected:
	void addButtons();
	void setButtonStates();
};
