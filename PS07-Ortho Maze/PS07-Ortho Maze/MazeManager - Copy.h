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