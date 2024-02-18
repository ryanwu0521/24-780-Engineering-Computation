#pragma once
#include <string>
#include <vector>
#include "fssimplewindow.h"

//#include "GraphicFont.h"
class GraphicFont;

class Control {
protected:
	int x = -1, y = -1;
	int xSize = -1, ySize = -1;
	int value = FSKEY_NULL;
	std::string label = "Button";
	std::string hover = "I do something";

	bool enabled = true;

	GraphicFont* theFont = nullptr;  // using pointer to avoid creating copies of fonts

public:
	Control() {};
	Control(int locX, int locY, int wid, int hei, int val, std::string text,
		GraphicFont* aFont, std::string description);
	void disable() { enabled = false; };
	void enable() { enabled = true; };
	void addHover(std::string newHover) { hover = newHover; }
	int getWidth() { return xSize; }

	virtual void paint() {};
	virtual int getVal(int locX, int locY) { return value; };
	virtual int getVal() { return value; }
	virtual void paintHover(int locX, int locY) {};
};

class ComboBox : public Control {
protected:
	std::vector<std::string> items;
	int currSelection = -1;
	int itemYsize;

	bool isExpanded = false;

public:
	ComboBox(int locX, int locY, int wid, int hei, int val, std::string label,
		GraphicFont* aFont, std::string description) :
		Control(locX, locY, wid, hei, val, label, aFont, description) {
		itemYsize = ySize * 0.8;
	};

	//virtual int getVal() {
	//	return value + currSelection;
	//};
	virtual int getVal(int locX, int locY);
	virtual void paint();
	virtual void paintHover(int locX, int locY);

	void addItem(std::string newItem, int location = 100);
};

class Button : public Control {

public:
	Button() {};
	Button(int locX, int locY, int wid, int hei, int val, std::string text,
		GraphicFont* aFont, std::string description) :
		Control(locX, locY, wid, hei, val, text, aFont, description) {};

	virtual int getVal(int locX, int locY);
	virtual int getVal() { return value; }
	virtual void paint();
	virtual void paintHover(int locX, int locY);

};


class ControlCollection {
protected:
	std::vector<Control*> theControls;

public:
	void add(int locX, int locY, int wid, int hei, int val, std::string text,
		GraphicFont* aFont, std::string description = "");

	void add(Control* newItem) {
		theControls.push_back(newItem);
	}
	void remove(int val);
	void disableButton(int val);
	void enableButton(int val);

	int checkClick(int locX, int locY);
	void checkHover(int locX, int locY);

	void paint();

	~ControlCollection();

private:
	int getIndex(int val);
};