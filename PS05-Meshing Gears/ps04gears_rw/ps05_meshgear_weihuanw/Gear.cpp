/*
			 _   ___
			|_.'_____`._
		__ .'.-'     `-.`.		   Ryan Wu (based on Dr. Nestor Gomez)
	   \  /,'           `.\		   Carnegie Mellon University
		\//               \\	   Eng. Computation, 24-780-B
		;;                 ::	   Prob Set 5
		||        O        ||	   Due Friday October 6, 2023
	   /,:                 ;;
	  /__\\               //	   A Gear script that executes all 
		  \`.           ,'/		   functions in Gear class
		  /_.`-._____.-'.'		   
			 `-._____.-'

*/

#include <sstream>
#include <algorithm>    // std::min
#include <math.h>
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "StringPlus.h"
#include "Gear.h"
#include <vector>
#include <cmath>
#include <iostream>
#define _USE_MATH_DEFINES

using namespace std;

// load gears from file
void Gear::load(std::ifstream& inFile)
{
	string wholeLineString;
	stringstream wholeLineStream;
	bool continueReading = true;
	int colonLocation;

	// go through file
	while (!inFile.eof() && continueReading) {
		// read the whole line
		getline(inFile, wholeLineString);

		// find the colon and prepare to read from stringstream after the colon
		if ((colonLocation = wholeLineString.find(":")) != string::npos)
			wholeLineStream.str(wholeLineString.substr(colonLocation + 1));

		// when the find() function doesn't find it, string::npos is returned
		if (wholeLineString.find("partID") != string::npos) {
			partID = StringPlus::trim(
				wholeLineString.substr(colonLocation + 1));
		}
		else if (wholeLineString.find("pitch") != string::npos) {
			wholeLineStream >> pitch;
		}
		else if (wholeLineString.find("numbTeeth") != string::npos) {
			wholeLineStream >> numbTeeth;
		}
		else if (wholeLineString.find("startX") != string::npos) {
			wholeLineStream >> loc.x;
		}
		else if (wholeLineString.find("startY") != string::npos) {
			wholeLineStream >> loc.y;
		}
		else if (wholeLineString.find("startAngle") != string::npos) {
			wholeLineStream >> angle;
		}

		else if (wholeLineString.find("Gear End") != string::npos) {
			continueReading = false;
		}

		wholeLineStream.clear(); // get ready for next line
	}
}

// print gear information in terminal
void Gear::print(std::ostream& output)
{
	output << "Gear: " << endl;
	output << "\tpartID: " << partID << endl;
	output << "\tpitch: " << pitch << endl;
	output << "\tnumbTeeth: " << numbTeeth << endl;
	output << "\tstartX: " << loc.x << endl;
	output << "\tstartY: " << loc.y << endl;
	output << "\tstartAngle: " << angle << endl;
	output << "Gear End: " << endl;
}

// draw gear
void Gear::draw(bool fill, bool showID)
{
	float radius = numbTeeth / pitch / 2.;
	float module = radius * 2 / numbTeeth;
	float angleStepRad = 360. / numbTeeth * atan(1.) / 45.;

	glTranslated(loc.x, loc.y, 0.);
	glRotated(angle, 0., 0., 1.);
	float topWidth = 1.0 * module;
	float botWidth = 1.8 * module;
	float radiusToToothSide = sqrt(pow(radius - 1.25 * module, 2.) + pow(botWidth / 2., 2.));
	float theta = angleStepRad - atan(botWidth / 2. / (radius - 1.25 * module));
	Point2D extraSegment = { radiusToToothSide * cos(theta), radiusToToothSide * sin(theta) };
	// draw the teeth
	for (int i = 0; i < numbTeeth; i++) {

		if (!fill) {
			glBegin(GL_LINE_STRIP);
			glVertex2d(extraSegment.x, extraSegment.y);
			glVertex2d(radius - 1.25 * module, botWidth / 2.);
			glVertex2d(radius + 1.00 * module, topWidth / 2.);
			glVertex2d(radius + 1.00 * module, -topWidth / 2.);
			glVertex2d(radius - 1.25 * module, -botWidth / 2.);
			glEnd();
		}
		else {
			// filled in (two convex shapes)
			glBegin(GL_POLYGON);
			glVertex2d(0., 0.);
			glVertex2d(radius - 1.25 * module, botWidth / 2.);
			glVertex2d(radius + 1.00 * module, topWidth / 2.);
			glVertex2d(radius + 1.00 * module, -topWidth / 2.);
			glVertex2d(radius - 1.25 * module, -botWidth / 2.);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex2d(0., 0.);
			glVertex2d(extraSegment.x, extraSegment.y);
			glVertex2d(radius - 1.25 * module, botWidth / 2.);
			glEnd();
		}
		glRotated(360. / numbTeeth, 0., 0., 1.);
	}

	// show partID (not the way I wanted it, but OK for now)
	if (showID) {
		glColor3ub(255, 255, 0);  // yellow
		glRasterPos2i(0, 0);  // sets position
		YsGlDrawFontBitmap8x12(partID.c_str());

		// used the following code to prove that YsGLDrawFont never rotates :-(
		//glBegin(GL_LINES);
		//glVertex2f(0, 0);
		//glVertex2f(radius, 0);
		//glEnd();

		glColor3ub(0, 0, 0); // black

	}

	// draw pitch circle (useful for debugging)
	//float PI = atan(1.) * 4.;
	//glBegin(GL_LINE_LOOP);
	//for (float angle = 0; angle < 2* PI; angle += PI/32) // 64 points
	//	glVertex2d(loc.x + radius* cos(angle), loc.y + radius * sin(angle));
	//glEnd();

	glRotated(-angle, 0., 0., 1.);
	glTranslated(-loc.x, -loc.y, 0.);

}

// edit current gear 
void Gear::edit()
{
	string tempString;

	cout << "\nFor each of the following, enter \".\" to keep original value:" << endl;

	cout << "\tpartID (" << partID << ") new value >> ";
	cin >> tempString;
	if (tempString != ".")
		partID = StringPlus::trim(tempString);

	cout << "\tpitch (" << pitch << ") new value >> ";
	cin >> tempString;
	StringPlus::trim(tempString);
	if (tempString != ".")
		pitch = stof(tempString);

	cout << "\tnumbTeeth (" << numbTeeth << ") new value >> ";
	cin >> tempString;
	StringPlus::trim(tempString);
	if (tempString != ".")
		numbTeeth = stoi(tempString);

	cout << "\tstartX (" << loc.x << ") new value >> ";
	cin >> tempString;
	StringPlus::trim(tempString);
	if (tempString != ".")
		loc.x = stof(tempString);

	cout << "\tstartY (" << loc.y << ") new value >> ";
	cin >> tempString;
	if (tempString != ".")
		loc.y = stof(tempString);

	cout << "\tangle (" << angle << ") new value >> ";
	cin >> tempString;
	if (tempString != ".")
		angle = stof(tempString);
}

// rotate gear
void Gear::rotate(float rotAngle, Gear* requestingGear)
{
	// rotate this gear
	angle += rotAngle;
	angle = fmod(angle, 360.);

	// rotate attached gears
	for (auto currGear : this->attachedGears) {
		if (currGear != requestingGear)
			currGear->rotate(rotAngle, this);
	}
	// mesh gears
	for (auto currGear : this->meshedGears) {
		if (currGear != requestingGear) {
			float otherAngle = -(rotAngle * this->numbTeeth) / (float)currGear->numbTeeth;
			currGear->rotate(otherAngle, this);
		}
	}
}

// move gear
void Gear::move(Point2D delta, Gear* requestingGear)
{
	// move this gear
	loc.x += delta.x;
	loc.y += delta.y;

	// move attached gears
	for (auto currGear : attachedGears) {
		if (currGear != requestingGear)
			currGear->move(delta, this);
	}

	// move meshed gears
	for (auto currGear : meshedGears) {
		if (currGear != requestingGear)
			currGear->move(delta, this);
	}
}

// saving gear information
void Gear::save(std::vector<Gear*> allGears, std::ofstream& outFile)
{
	outFile << "Gear:" << endl;
	outFile << "\tpartID: " << partID << endl;
	outFile << "\tpitch: " << pitch << endl;
	outFile << "\tnumbTeeth: " << numbTeeth << endl;
	outFile << "\tstartX: " << loc.x << endl;
	outFile << "\tstartY: " << loc.y << endl;
	outFile << "\tstartAngle: " << angle << endl;
	outFile << "Gear End:" << endl;
}

// dissociate gears (optional)
bool Gear::dissociate(Gear* otherGear, std::vector<Gear*>* collection)
{
	for (int i = 0; i < collection->size(); i++) {
		if (collection->at(i) == otherGear) {
			collection->erase(collection->begin() + i);
			return true; // <<<<<<<<<<<  possible exit from function
		}
	}
	return false; // other gear not found
}

// attach gears
bool Gear::attachToGear(Gear* otherGear, bool biDirectional) {
	for (auto currGear : attachedGears) {
		if (currGear == otherGear)

			return false;
	}
	for (auto currGear : meshedGears) {
		if (currGear == otherGear)

			return false;
	}

	Point2D otherLoc = otherGear->loc;
	Point2D delta;
	delta.x = otherLoc.x - loc.x;
	delta.y = otherLoc.y - loc.y;
	move(delta);

	attachedGears.push_back(otherGear);
	otherGear->attachToGear(this);
}

// mesh gears
bool Gear::meshInto(Gear* otherGear, bool biDirectional)
{
	for (auto currGear : meshedGears) {
		if (currGear == otherGear)

			return false;
	}
	for (auto currGear : attachedGears) {
		if (currGear == otherGear)

			return false;
	}

	Point2D otherLoc = otherGear->loc;
	Point2D delta;
	double newDist = (otherGear->pitchDiam() + this->pitchDiam()) / 2;
	double currDist = sqrt(pow((otherLoc.x - loc.x), 2) + pow((otherLoc.y - loc.y), 2));

	float angleLine = atan2(otherLoc.y - loc.y, otherLoc.x - loc.x) * 180 / 3.1415926;
	double combinedRotate = -(angle - angleLine) + 180. / numbTeeth;
	double psudoRotate = 180. - otherGear -> angle + angleLine;
	combinedRotate += psudoRotate * ((otherGear->numbTeeth) / (float)numbTeeth);
	
	delta.x = (otherLoc.x - loc.x) * ((currDist - newDist) / currDist);
	delta.y = (otherLoc.y - loc.y) * ((currDist - newDist) / currDist);
	move(delta);
	rotate(combinedRotate);
	meshedGears.push_back(otherGear);
	otherGear->meshInto(this);
	
}

// setting up the GUI boundaries
Point2D Gear::minBound()
{
	// use addendum dimension
	float radius = pitchDiam() / 2.;
	float module = pitchDiam() / numbTeeth;
	return { loc.x - radius - module, loc.y - radius - module };
}

// setting up the GUI boundaries
Point2D Gear::maxBound()
{
	// use addendum dimension
	float radius = pitchDiam() / pitch / 2.;
	float module = pitchDiam() / numbTeeth;
	return { loc.x + radius + module, loc.y + radius + module };
}

// setting up the GUI boundaries
Point2D Gear::anyBound(int factor)
{
	// use addendum dimension (= 1.0 * module)
	float radius = numbTeeth / pitch / 2.;
	float module = 1. / pitch;
	return { loc.x + factor * (radius + module), loc.y + factor * (radius + module) };
}

// detach and un-mesh all gears (optional)
Gear::~Gear()
{
	// tell all my attached gears to let go of me
	for (auto currGear : attachedGears) {
		currGear->deAttach(this);
	}

	// tell all my meshed gears to let go of me
	for (auto currGear : meshedGears) {
		currGear->deMesh(this);
	}
}