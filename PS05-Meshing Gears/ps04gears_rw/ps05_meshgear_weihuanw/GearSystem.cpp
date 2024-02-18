/*
			 _   ___
			|_.'_____`._
		__ .'.-'     `-.`.		   Ryan Wu (Based on Dr. Nestor Gomez)
	   \  /,'           `.\		   Carnegie Mellon University
		\//               \\	   Eng. Computation, 24-780-B
		;;                 ::	   Prob Set 5
		||        O        ||	   Due Friday October 6, 2023
	   /,:                 ;;
	  /__\\               //	   A gear system that provides simple
		  \`.           ,'/		   user input functions
		  /_.`-._____.-'.'
			 `-._____.-'
*/

#include "Gear.h"
#include "GearSystem.h"

// draw and highlight current gear
void GearSystem::draw(bool highlightOther)
{
	// highlight current Gear by drawing thick blue line
	glColor3ub(0, 0, 255);
	glLineWidth(3);
	if (currGearIndex >= 0 && currGearIndex < allGears.size())
	{// extra protection
		allGears.at(currGearIndex)->draw();
	}

	// highlight other Gear by drawing thick red line
	glColor3ub(255, 0, 0);
	glLineWidth(3);
	if (otherGearIndex >= 0 && otherGearIndex < allGears.size())
	{// extra protection
		allGears.at(otherGearIndex)->draw();
	}

	// draw all the gears
	glColor3ub(0, 0, 0);
	glLineWidth(1);
	for (int i = 0; i < allGears.size(); i++)
	{
		allGears.at(i)->draw();
	}
}

void GearSystem::inputRequiredMessage()
{
	// display message on graphics window directing user to check console

	std::string message = "Input required on console . . .";
	// black color
	glColor3f(0, 0, 0);

	//center horizontally and vertically for 20-pixel-wide by 28-pixel-height letters
	int wid, hei;
	FsGetWindowSize(wid, hei);
	glRasterPos2d(wid / 2 - message.length() * 20 / 2, hei / 2 + 28 / 2);

	YsGlDrawFontBitmap20x28(message.c_str()); // function requires a C-language string
	FsSwapBuffers();

}

// rotate current gear
void GearSystem::rotateCurrent(float deg)
{
	allGears.at(currGearIndex)->rotate(deg);
}

// delete current gear
void GearSystem::deleteCurrent()
{
	char yesNo;
	if (currGearIndex >= 0) {
		// redirect to console
		inputRequiredMessage();

		// ask for verification
		std::cout << "\n\nAre you sure you want to delete this gear ("
			<< allGears.at(currGearIndex)->getID() << ")? (Y/N) >> ";
		std::cin >> yesNo;

		// delete/erase the current gear and make the previous gear current
		if (yesNo == 'Y' || yesNo == 'y') {
			delete allGears.at(currGearIndex); // release memory
			allGears.erase(allGears.begin() + currGearIndex);
			currGearIndex--;
			if (currGearIndex < 0)
				currGearIndex = allGears.size() - 1;
		}

		while (getchar() != '\n'); // this will clear the newline associated with last input
	}

}

// edit current gear
void GearSystem::editCurrent()
{
	allGears.at(currGearIndex)->edit();
}

// print current gear information
void GearSystem::printCurrent()
{
	allGears.at(currGearIndex)->print();
}

// cycle through current gear
void GearSystem::changeCurrent(int change)
{
	currGearIndex += change;
	if (currGearIndex >= allGears.size()) {
		currGearIndex = 0;
	}
	if (currGearIndex < 0) {
		currGearIndex = allGears.size() - 1;
	}
}

// cycle through gear 
void GearSystem::changeOther(int change)
{
	otherGearIndex += change;
	if (otherGearIndex >= allGears.size()) {
		otherGearIndex = 0;
	}
	if (otherGearIndex < 0) {
		otherGearIndex = allGears.size() - 1;
	}
}

//void GearSystem::changeCurrent(Point2D givenCoords)
//{
//
//}

//void GearSystem::changeOther(Point2D givenCoords)
//{
//
//}

// add gears from other files
void GearSystem::addGear(const std::string filename)
{
	std::ifstream inFile;
	std::string datafilename;
	//std::string filename;

	if (filename == "") {
		//ask for filename
		inputRequiredMessage();
		std::cout << "\n\nPlease enter the name of the file to read > ";
		std::cin >> datafilename;
	}
	else
		datafilename = filename;

	if (datafilename.find(".gear") == std::string::npos) // if missing extension, add it
		datafilename += ".gear"; // allows user to omit extension

	inFile.open(datafilename);

	if (inFile.is_open()) { // may not have found file
		std::string wholeLineString;

		while (!inFile.eof()) {
			// read the whole line
			getline(inFile, wholeLineString);
			if (wholeLineString.find("Gear:") != std::string::npos) {

				Gear* newGear = new Gear; // create a new gear (but not as a variable in this function)

				newGear->load(inFile); // load gear properties from ifstream
				   // note that   newGear->load   is the same as  (*newGear).load

				allGears.push_back(newGear); // add pointer to new gear to vector
			}

			// continue reading ifstream (perhaps there are more gears)
		}
		inFile.close();
		currGearIndex = allGears.size() - 1; // set last-added gear to be current gear
	}
	else
		std::cout << "\nError reading file " << datafilename
		<< "    Please check data and try again." << std::endl;

}

// get gear information (ID)
Gear* GearSystem::getGear(const std::string& gearID)
{
	for (auto& Gear : allGears) {
		if (Gear->getID() == gearID) {
			return Gear;
		}
	}
	return nullptr;
}

// get gear information (Index)
Gear* GearSystem::getGear(int gearIndex)
{
	for (auto& Gear : allGears) {
		if (Gear->getIndex() == gearIndex) {
		
			return Gear;
		}
	}
	return nullptr;
}

/*{
		if (0 <= gearIndex && gearIndex)
	}*/

// set orginial scale for GUI
void GearSystem::getOriginAndScale(float& originX, float& originY, float& scale)
{
	if (allGears.size() > 0) {
		Point2D minBnd, maxBnd, currMin, currMax;

		minBnd = allGears.at(0) -> minBound();
		maxBnd = allGears.at(0) -> maxBound();
		for (auto& currGear : allGears) {
			currMin = currGear -> minBound();
			currMax = currGear -> maxBound();
			if (minBnd.x > currMin.x)
				minBnd.x = currMin.x;
			if (minBnd.y > currMin.y)
				minBnd.y = currMin.y;
			if (maxBnd.x < currMax.x)
				maxBnd.x = currMax.x;
			if (maxBnd.y < currMax.y)
				maxBnd.y = currMax.y;
		}
		originX = minBnd.x; originY = minBnd.y;

		int hei, wid;
		FsGetWindowSize(wid, hei);

		float scaleX, scaleY;
		int extraBorder = 10; // allows for some space around all the gears
		scaleX = (wid - extraBorder) / (maxBnd.x - minBnd.x);
		scaleY = (hei - extraBorder) / (maxBnd.y - minBnd.y);
		scale = std::min(scaleX, scaleY);
		if (scale == scaleX) // adjust originY so as to center vertically
			originY -= 0.5 * (hei / scale - (maxBnd.y - minBnd.y));
		else // adjust originX so as to center horizontally
			originX -= 0.5 * (wid / scale - (maxBnd.x - minBnd.x));

		// adjust for half extraborder
		originX -= extraBorder / 2. / scale;
		originY -= extraBorder / 2. / scale;
	}
	else {
		originX = originY = 0.;
		scale = 1.0;
	}

}

// attach the current gear and other gear
bool GearSystem::attachGears()
{
	if (0 <= currGearIndex && currGearIndex < allGears.size()
		&& 0 <= otherGearIndex && otherGearIndex < allGears.size()
		&& currGearIndex != otherGearIndex) {// extra protection
		return allGears.at(currGearIndex)->attachToGear(allGears.at(otherGearIndex));
	}
	else
		return false;
}

// mesh the current gear and other gear
bool GearSystem::meshGears()
{
	if (0 <= currGearIndex && currGearIndex < allGears.size()
		&& 0 <= otherGearIndex && otherGearIndex < allGears.size()
		&& currGearIndex != otherGearIndex) {// extra protection
		return allGears.at(currGearIndex)->meshInto(allGears.at(otherGearIndex));
	}
	else
		return false;
}

// save all gear data to a file
void GearSystem::saveAll()
{
	std::string filename;
	std::cout << "Please provide the file name to save all gear data: ";
	std::cin >> filename;

	// Open the output file
	std::ofstream outFile(filename);
	if (outFile.is_open()) {
		
		for (int i = 0; i < allGears.size(); i++){
			allGears.at(i)->save(allGears, outFile); 
		}
		outFile.close();
		std::cout << "Gear information saved!" << std::endl;
	}
	else {
		std::cout << "Error: Unable to save gear information." << std::endl;
	}
}
