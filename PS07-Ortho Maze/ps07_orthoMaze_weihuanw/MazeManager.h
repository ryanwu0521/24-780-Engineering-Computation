/*
Ryan Wu (ID:weihuanw) based on Dr.Nestor Gomez's code
Carnegie Mellon University
Eng. Computation, 24-780-B
Problem Set 7: Ortho Maze
Due: Tues. Nov. 7, 2023
Program overall: Maze navigation using keyboard to create simple interactive game
This is a header file for the MazeManager class.
Tasked feature: created a new random maze using Aldous-Broder algorithm and user input
*/

#pragma once
#include "yssimplesound.h"
#include "GraphicFont.h"
#include "Maze.h"
#include "Entity.h"


class MazeManager {
protected:
	Maze* theMaze;
	Entity* theEntity;

	bool showVisited = false;
	bool pathIsFound = false;

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

	~MazeManager() {
		delete theEntity;
	}

	void zoomAll();

	Cell getModelCoords(int screenX, int screenY);

};