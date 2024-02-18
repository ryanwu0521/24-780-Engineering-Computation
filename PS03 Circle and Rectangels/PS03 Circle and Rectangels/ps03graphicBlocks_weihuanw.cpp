// Ryan Wu
// ID: weihuanw
// Carnegie Mellon University
// Eng. Computation 24-780-B
// Prob. Set 3 Circles and Rectangles
// Due Tues. Sept 19, 2023

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "fssimplewindow.h"
#define GL_SILENCE_DEPRECATION


using namespace std;

// Declearing graphical item type
enum itemDeclaration {
    CIRCLE, RECT
};

// Creating a struct type to hold the information for each graphical item
struct GraphicalItem {
    itemDeclaration type;
    int centerX;
    int centerY;
    int height;
    int width;
    float red;
    float green;
    float blue;
};

// Function that takes a string and returns a GaphicalItem stuct with the information in the string
GraphicalItem createItem(const std::string &input) {
    GraphicalItem item;
    std::istringstream iss(input);
    std::string inputType;
    iss >> inputType;
    
    if (inputType.find("CIRCLE")!= string::npos)
        item.type = CIRCLE;
    else if (inputType.find("RECT")!= string::npos)
        item.type = RECT;
        
    iss >> item.centerX >> item.centerY >> item.height >> item.width >> item.red >> item.green >> item.blue;
    
    return item;
}

// Function that prints out the data stored in a graphical item
void printItem(const GraphicalItem &anItem, std::ostream &output = std::cout) {
    output << (anItem.type == itemDeclaration::CIRCLE ? "CIRCLE" : "RECT") << ": "
    << anItem.centerX << " " << anItem.centerY << " "
    << anItem.height << " " << anItem.width << " "
    << anItem.red << " " << anItem.green << " " << anItem.blue << std::endl;
}

// Function that reads user input files (Given)
void readItems(std::vector<GraphicalItem>& theItems, std::ifstream& inFile)
{
    string longInput;
    
    while (!inFile.eof()) {  // loop through whole file
        getline(inFile, longInput);   // get a whole line and put it in string
        if (longInput.find(":") != string::npos)// if there is a colon in the string
            theItems.push_back(createItem(longInput));
    }
}

// Function that writes user output files
void writeItems(const std::vector<GraphicalItem>& theItems, std::ofstream& outFile)
{
    for (const GraphicalItem& item : theItems) {
        printItem(item, outFile);
    }
}

// Function that calls for user inputs (Given)
bool getUserInput(std::vector<GraphicalItem>& theItems) {
    // guides user through data input and makes changes to
    // the collection of graphical items
    
    char userChoice;
    int inputInt;
    std::string longInput, shapeDescription;
    bool validResponse = false, stayInProgram = true;
    
    while (!validResponse) {
        std::cout << std::endl;
        std::cout << "Make a selection: add (C)ircle, add (R)ectangle, " << std::endl;
        std::cout << "                  (D)elete item, clear (A)ll, " << std::endl;
        std::cout << "                  (L)oad file, (S)ave file, e(X)it >> ";
        std::cin >> userChoice;
        
        std::cin.clear(); std::cin.ignore(); // to clear up the input buffer
        
        if (userChoice == 'C' || userChoice == 'c'
            || userChoice == 'R' || userChoice == 'r') { // add a new shape
            
            // initialize description string
            if (userChoice == 'C' || userChoice == 'c')
                shapeDescription = "CIRCLE: ";
            else
                shapeDescription = "RECT: ";
            
            // ask for parameters
            std::cout << "                  Center of shape (x y) >> ";
            getline(std::cin, longInput);
            shapeDescription += longInput + " ";
            
            std::cout << "                  Size of shape (height width) >> ";
            getline(std::cin, longInput);
            shapeDescription += longInput + " ";
            
            std::cout << "                  Color of shape (red green blue) >> ";
            getline(std::cin, longInput);
            shapeDescription += longInput;
            
            // create and add the item to collection
            theItems.push_back(createItem(shapeDescription));
            validResponse = true;
        }
        else if (userChoice == 'D' || userChoice == 'd') { // delete single item
            if (theItems.size() < 1)
                std::cout << std::endl << "                 No items to delete" << std::endl;
            else {         std::cout << "                      Select index of item to delete (1 to "
                << theItems.size() << ") >> ";
                std::cin >> inputInt;
                if (0 < inputInt && inputInt <= theItems.size()) {
                    // confirm first
                    std::cout << "Are you sure you want to delete the item:" << std::endl;
                    std::cout << "     ";
                    printItem(theItems.at(inputInt - 1));
                    std::cout << "   (Y/N) >> ";
                    std::cin >> userChoice;
                    if (userChoice == 'Y' || userChoice == 'y') {
                        theItems.erase(theItems.begin() + inputInt - 1);
                        validResponse = true;
                    }
                }
            }
        }
        else if (userChoice == 'A' || userChoice == 'a') { // clear all items
            if (theItems.size() < 1)
                std::cout << std::endl << "                 No items to delete" << std::endl;
            else {
                // confirm first
                std::cout << "Are you sure you want to clear all items? (Y/N) >> ";
                std::cin >> userChoice;
                if (userChoice == 'Y' || userChoice == 'y') {
                    theItems.clear();
                    validResponse = true;
                }
                else
                    validResponse = false;
            }
        }
        else if (userChoice == 'X' || userChoice == 'x') {
            stayInProgram = false;
            validResponse = true;
        }
        else if (userChoice == 'L' || userChoice == 'l') {
                
            std::string (inputFile);
            std::cout << "Please enter your input file's name: ";
            std::cin >> inputFile;
            
            std::ifstream inFile(inputFile);
            if (inFile.is_open()) {
                readItems(theItems, inFile);
                std::cout << "Input file loaded." << std::endl;
                validResponse = true;
            } else {
                std::cout << "Failed to load input file." << std::endl;
            }
        }
        else if (userChoice == 'S' || userChoice == 's') {
            
            std::string (outputFile);
            std::cout << "Please enter your output file's name: ";
            std::cin >> outputFile;
            
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                writeItems(theItems, outFile);
                outFile.close();
                std::cout << "Your output file has been saved." << std::endl;
            } else {
                std::cout << "Failed to write to output file." << std::endl;
                
            validResponse = true;
            }
        }
        if (!validResponse)
            std::cout << std::endl << "No changes were recorded " << std::endl;
    }
    
    // return true if program should continue
    return stayInProgram;
}

// Function for drawing circles
void drawCircle(int centerX, int centerY, int height, int width, float red, float green, float blue, bool fill) {
    const double YS_PI = 3.1415927;
    
    glColor3f(red, green, blue);
    
    if (fill)
        glBegin(GL_POLYGON);
    else
        glBegin(GL_LINE_LOOP);
    
    int midWidth = width / 2;
    int midHeight = height / 2;
    
    for (int i = 0; i < 64; i++)
        {
            double angle = (double)i * YS_PI / 32.0;
            double x = (double)centerX + cos(angle) * (double)midWidth;
            double y = 600 - ((double)centerY + sin(angle) * (double)midHeight);
            glVertex2d(x, y);
            }
    glEnd();
}

// Function for drawing rectangles
void drawRect(int centerX, int centerY, int height, int width, float red, float green, float blue, bool fill) {
    
    glColor3f(red, green, blue);
    
    if(fill)
        glBegin(GL_QUADS);
    else
        glBegin(GL_LINE_LOOP);
    
    int midWidth = width / 2;
    int midHeight = height / 2;
    
    glVertex2i(centerX - midWidth, 600 - (centerY - midHeight));
    glVertex2i(centerX + midWidth, 600 - (centerY - midHeight));
    glVertex2i(centerX + midWidth, 600 - (centerY + midHeight));
    glVertex2i(centerX - midWidth, 600 - (centerY + midHeight));
    
    glEnd();
}

//Function to draw Task 2 and 3 outputs
void drawItem(const GraphicalItem &anItem) {
    if (anItem.type == RECT) {
        drawRect(anItem.centerX, anItem.centerY, anItem.height, anItem.width, anItem.red, anItem.green, anItem.blue, true);
    } else if (anItem.type == CIRCLE) {
        drawCircle(anItem.centerX, anItem.centerY, anItem.height, anItem.width, anItem.red, anItem.green, anItem.blue, true);
    }
}

// My main code
int main() 
{
    std::vector <GraphicalItem> itemColl;
    
    FsOpenWindow(16,16,800,600,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    bool stayInProgram = true;
    while (stayInProgram) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        for (const GraphicalItem &anItem : itemColl)
            drawItem(anItem);
        
        glFlush();
        FsPollDevice();
        FsSleep(10);
        stayInProgram = getUserInput(itemColl);
    }
    return 0;
}
