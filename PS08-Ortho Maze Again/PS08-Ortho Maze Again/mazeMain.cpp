#include "fssimplewindow.h"
#include "MazeManager.h"

int main()
{
	FsOpenWindow(16, 16, 950, 800, 1, "Maze Game (NG 2023)");

	Maze aMaze;
	MazeManager theManager(&aMaze);
	theManager.showMenu();

	while (theManager.manage()) {
		FsSwapBuffers();
		FsSleep(10);
	}

}