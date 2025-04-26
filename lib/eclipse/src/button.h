

// Button class
// Part of the Eclipse interface library
// By Christopher Delay

#ifndef _included_button_h
#define _included_button_h

#include <functional>
#include <gucci.h>

class Button {

public:
	int x;
	int y;
	int width;
	int height;
	std::string caption;
	std::string name;
	std::string tooltip;
	int userinfo; // Used to represent other program data about the button.

	bool dirty;

	Image* image_standard; // Used when this button is
	Image* image_highlighted; // represented by an image
	Image* image_clicked;

protected:
	std::function<void(Button*, bool, bool)> draw;
	std::function<void(Button*)> mouseup;
	std::function<void(Button*)> mousedown;
	std::function<void(Button*)> mousemove;
	std::function<void(Button*)> middleclick;

public:
	Button();
	Button(int newx, int newy, int newwidth, int newheight, const std::string& newcaption, const std::string& newname);

	virtual ~Button();

	void SetProperties(
		int newx, int newy, int newwidth, int newheight, const std::string& newcaption, const std::string& newname);

	void SetCaption(const std::string& newcaption);
	void SetTooltip(const std::string& newtooltip);
	void SetStandardImage(Image* newimage);
	void SetImages(Image* newstandard, Image* newhighlighted, Image* newclicked);

	void RegisterDrawFunction(void (*newdraw)(Button*, bool, bool));
	void RegisterMouseUpFunction(std::function<void(Button*)> newmouseup);
	void RegisterMouseDownFunction(std::function<void(Button*)> newmousedown);
	void RegisterMouseMoveFunction(std::function<void(Button*)> newmousemove);
	void RegisterMiddleClickFunction(std::function<void(Button*)> newmiddleclick);

	void Dirty();

	void Draw(bool highlighted, bool clicked);
	void MouseUp();
	void MouseDown();
	void MouseMove();
	void MiddleClick();

	void DebugPrint();
};

#endif
