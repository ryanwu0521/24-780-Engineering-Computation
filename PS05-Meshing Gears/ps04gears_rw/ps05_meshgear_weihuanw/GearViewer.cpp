/*
			 _   ___
			|_.'_____`._
		__ .'.-'     `-.`.		   Ryan Wu (Based on Dr. Nestor Gomez)
	   \  /,'           `.\		   Carnegie Mellon University
		\//               \\	   Eng. Computation, 24-780-B
		;;                 ::	   Prob Set 5
		||        O        ||	   Due Friday October 6, 2023
	   /,:                 ;;
	  /__\\               //	   A gear viewer that loads and
		  \`.           ,'/		   displays several simple gears and
		  /_.`-._____.-'.'         provides user input menu
			 `-._____.-'

Extra features:
1) Fill-in gear area when highlighting current gear
2) Toggle continuous rotation of current gear in direction of last rotation
3) Click on a gear to make it current
4) Display model coordinates when clicking and holding the left mouse button
   anywhere on window.
5) Ticking sound when gears are manually rotated

*/

#include <string.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "yssimplesound.h"

#include "GearSystem.h"

using namespace std;

int main(void);

// Displaing menu
void showMenu();

Point2D getModelCoords(int screenX, int screenY,
	float originX, float originY, float scale);

int main(void)
{
	const int width = 800, height = 600;
	float originX = 0., originY = 0.;
	float scale = 10.;
	int mouseEvent, leftButton, middleButton, rightButton;
	int screenX, screenY;
	stringstream coordStream; // for displaying coordinates on screen
	Point2D modelCoord;

	bool terminate = false;
	bool rotateIsOn = false;
	int rotateDir = 1;
	float rotateMicro = 1.f;
	char yesNo;
	GearSystem mySystem;

	//int currGearIndex = -1;
	//Gear newGear;
	//vector<Gear> allGears;



	//string datafilename;
	int key;
	//ifstream inFile;
	bool canPlaySound = false;

	FsOpenWindow(16, 16, width, height, 1, "Gear Viewer (NG2021)"); // use double buffer

	YsSoundPlayer player1;
	YsSoundPlayer::SoundData clickSound;
	if (YSOK == clickSound.LoadWav("click.wav")) {
		canPlaySound = true;
		// start the player after opening window
		player1.Start();
	}
	else {
		cout << "Failed to read " << "click.wav" << endl;
		return 1;
	}

	showMenu();

	//// for UI testing, just load 3 gears, commment/uncomment as needed
	mySystem.addGear("b1to5");
	mySystem.getOriginAndScale(originX, originY, scale);

	while (!terminate) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		FsPollDevice();
		key = FsInkey();

		switch (key) {
		case FSKEY_ESC:
		case FSKEY_X:   // exit program
			terminate = true;
			break;
		case FSKEY_UP: // prev current gear
			mySystem.changeCurrent(-1);
			break;
		case FSKEY_DOWN: // next current gear
			mySystem.changeCurrent(+1);
			break;
		case FSKEY_LEFT: // rotate CCW
			mySystem.rotateCurrent(rotateMicro);
			break;
		case FSKEY_RIGHT: // rotate CW
			mySystem.rotateCurrent(-rotateMicro);
			break;
		case FSKEY_L: // list all data and redisplay menu
			mySystem.printCurrent();
			break;
		case FSKEY_R:
			mySystem.changeOther(+1); // prev other gear
			break;
		case FSKEY_F:
			mySystem.changeOther(-1); // next other gear
			break;
		/*case FSKEY_G:
			mySystem.getGear();
			break;*/
			//case FSKEY_R: // toggle continuous rotation
			//	rotateIsOn = !rotateIsOn;
			//	break;
		case FSKEY_A: // add a gear
			mySystem.addGear(""); // add gears from different files
			break;
		case FSKEY_D: // delete a gear, if there are any in the collection
			mySystem.deleteCurrent();
			break;
		case FSKEY_E: // edit current gear
			mySystem.editCurrent();
			break;
		case FSKEY_M: // mesh gears
			mySystem.meshGears();
			break;
		case FSKEY_T: // attach gears
			mySystem.attachGears();
			break;
		case FSKEY_S: // save all gear information
			mySystem.saveAll();
			break;
		}

		//adjust the graphics plane
		glTranslated(0, height, 0); // make bottom-left the origin
		glScaled(scale, -scale, 1.);  // scale everything, with positive y going up
		glTranslated(-originX, -originY, 0);

		
		//highlight current Gear
		mySystem.draw();

		//glColor3ub(255, 0, 0);
		//glLineWidth(3);
		//if (currGearIndex >= 0 && currGearIndex < allGears.size()) {// extra protection
		//	if (rotateIsOn)		// manage constant rotation
		//		allGears.at(currGearIndex).rotate(rotateDir * rotateMicro);
		//	
		//	// draw gear solid color and show partID
		//	allGears.at(currGearIndex).draw(true, true); 
		//}

		// reset transformation matrix completely
		glLoadIdentity();

		// mouse event
		mouseEvent = FsGetMouseEvent(leftButton, middleButton,
			rightButton, screenX, screenY);

		//if (mouseEvent == FSMOUSEEVENT_LBUTTONDOWN /*&& FsGetKeyState(FSKEY_SHIFT)*/) {
		//	// make gear under mouse the current gear
		//	modelCoord = getModelCoords(screenX, screenY,
		//		originX, originY, scale);
		//	int i = 0;
		//	while (i < allGears.size()
		//		&& !(allGears[i].pointIsInGear(modelCoord)))
		//		i++;

		//	if (i < allGears.size()) // found a gear under mouse
		//		currGearIndex = i;
		//}
		if (leftButton) {
			// write coords on screen if left button is held down
			modelCoord = getModelCoords(screenX, screenY,
				originX, originY, scale);
			coordStream.str("");  // reset stream
			coordStream.precision(4);
			coordStream << modelCoord.x << ", " << modelCoord.y;
			glColor3ub(60, 230, 60);
			glRasterPos2i(screenX, screenY - 15);  // set position
			YsGlDrawFontBitmap7x10(coordStream.str().c_str());
		}

		FsSwapBuffers();
		FsSleep(20);
	}

	if (canPlaySound)
		player1.End();

}

void showMenu() // menu for user input key events
{
	cout << "\n\n";
	cout << "Use these keys on the graphics window:" << endl << endl;
	cout << "\tA : Add a gear from input file" << endl;
	cout << "\tD : Delete the current selected gear from the collection" << endl;
	cout << "\tE : Edit the selected gear" << endl;
	cout << "\tL : List all the data for the currently selected gear on the console window" << endl;
	cout << "\tM : Mesh the current gear to another user-selected gear" << endl;
	cout << "\tT : Attach the current gear to another user-selected gear" << endl;
	cout << "\tS : Save all the gear information into a single file" << endl;

	cout << endl;
	cout << "\tUp/Down arrows: cycle through current gears in the collection" << endl;
	cout << "\tR: cycle through other gears in the collection" << endl;
	cout << "\tF: cycle through other gears in the collection" << endl;
	cout << "\tLeft/Right arrows: rotate the currently selected gear" << endl;

	//cout << "\tR : Keep rotating currently selected gear (in previous direction)" << endl;
	cout << endl;
	cout << "\tLeft click on gear to make current" << endl;
	cout << "\tLeft click and hold anywhere to see coordinates" << endl;
	cout << endl;
	cout << "\tX/esc : exit the program" << endl;
}


// GUI display settings
Point2D getModelCoords(int screenX, int screenY, float originX, float originY, float scale) {
	int wei, hei;
	FsGetWindowSize(wei, hei);
	return { screenX / scale + originX, (hei - screenY) / scale + originY };
}