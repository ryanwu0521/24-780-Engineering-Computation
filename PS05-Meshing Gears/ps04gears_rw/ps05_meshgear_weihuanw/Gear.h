/*
			 _   ___
			|_.'_____`._
		__ .'.-'     `-.`.		   Ryan Wu (based on Dr. Nestor Gomez)
	   \  /,'           `.\		   Carnegie Mellon University
		\//               \\	   Eng. Computation, 24-780-B
		;;                 ::	   Prob Set 5
		||        O        ||	   Due Friday October 6, 2023
	   /,:                 ;;
	  /__\\               //	   Implement a Gear class to maintain
		  \`.           ,'/		   properties of gears. Also allow for
		  /_.`-._____.-'.'		   reading/writing files and display
			 `-._____.-'

*/
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>

// useful for managing coordinates
struct Point2D {
	float x, y;
};

class Gear {
private:
	float pitch;        // given in teeth per inch
	int numbTeeth;
	Point2D loc;        // current location of gear {x, y}
	float angle;        // current rotation of gear in degrees

public:
	std::string partID; // used for identification
	int gearIndex;
	std::vector<Gear *> attachedGears; 
	std::vector<Gear *> meshedGears;

public:
	// reads data from a file over-riding any existing data
	void load(std::ifstream& inFile);

	// outputs all the data for a gear in format matching attached example
	void print(std::ostream& output = std::cout);

	// calculates and return the pitch diameter
	float pitchDiam() { return numbTeeth / pitch; };

	// draws the gear on graphics window
	void draw(bool fill = false, bool showID = false);

	// get gear ID information
	std::string getID() { return partID; }

	// get gear Index information
	int getIndex() { return gearIndex; }
	

	// rotates the gear by the given angle (positive is CCW)
	// void rotate(float rotAngle) { angle = std::fmod(angle+rotAngle, 360.); }
	// void rotate(float rotAngle) { angle += rotAngle; }

	// rotate this gear by given amount // and possibly cause the rotation of connected gears (attached and meshed) // except do not rotate the gear that is requesting the rotation
	void rotate(float rotAngle, Gear* requestingGear = nullptr);
	// goes through each property, showing existing values // and asking user to make changes
	void edit();

	void move(Point2D delta, Gear* requestingGear = nullptr);

	void save(std::vector<Gear*> allGears, std::ofstream& outFile);

	// create a bi-directional attach relationship between this gear and r
	// otherGear, unless bi-directional is not appropriate a
	// may require this gear’s location to change to match otherGear’s G
	// returns false if connection cannot be done
	bool attachToGear(Gear* otherGear, bool biDirectional = true);
	// create a bi-directional meshed relationship between this gear and // otherGear, unless bi-directional is not appropriate // may require this gear to translate (along line connecting centers) // and rotate to allow meshing, but do not move or rotate otherGear // returns false if connection cannot be done (unmatched pitches)
	bool meshInto(Gear* otherGear, bool biDirectional = true);

	// returns bottom left corner of gear bounding box
	Point2D minBound();
	// { return anyBound(-1); }
	// returns top right corner of gear bounding box
	Point2D maxBound();

	// detaching 
	bool deAttach(Gear* otherGear) { return dissociate(otherGear, &attachedGears); }
	// demeshing
	bool deMesh(Gear* otherGear) { return dissociate(otherGear, &meshedGears); }

	// destructor
	~Gear();

	bool pointIsInGear(Point2D aPoint) {
		return ((aPoint.x - loc.x) * (aPoint.x - loc.x)
			+ (aPoint.y - loc.y) * (aPoint.y - loc.y))
			< (pitchDiam() * pitchDiam())/4.;

	}
protected:
	// since the detaching and demeshing functions are so similar, just use
	// this adaptive function.
	// 2nd parameter is a pointer to a vector of pointers to gears
	bool dissociate(Gear* otherGear, std::vector<Gear*>* collection);

	// again min and max bounds are very similar
	Point2D anyBound(int factor);
};