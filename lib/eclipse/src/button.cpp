#include "button.h"

#define FUDGE 0

// Source file for button class
// Part of the Eclipse interface library
// By Christopher Delay

#include <spdlog/spdlog.h>

#include "button.h"
#include "eclipse.h"

Button::Button()
{
	x = y = width = height = 0;
	draw = NULL;
	image_standard = image_highlighted = image_clicked = NULL;
	mouseup = mousedown = mousemove = middleclick = NULL;
	SetTooltip(" ");
	userinfo = 0;
}

Button::Button(int newx, int newy, int newwidth, int newheight, std::string newcaption, std::string newname)
{
	SetProperties(newx, newy, newwidth, newheight + FUDGE, newcaption, newname);
	draw = NULL;
	image_standard = image_highlighted = image_clicked = NULL;
	mouseup = mousedown = mousemove = middleclick = NULL;
	SetTooltip(" ");
	userinfo = 0;
}

Button::~Button()
{
	// There is a possibility that this button may use the
	// same image for more than one of these pointers
	// So don't neccisarily delete all three

	bool delete_standard = (image_standard != NULL);
	bool delete_highlighted = (image_highlighted != NULL && image_highlighted != image_standard);
	bool delete_clicked =
		(image_clicked != NULL && image_clicked != image_standard && image_clicked != image_highlighted);

	if (delete_standard) {
		delete image_standard;
	}
	if (delete_highlighted) {
		delete image_highlighted;
	}
	if (delete_clicked) {
		delete image_clicked;
	}
}

void Button::SetProperties(
	int newx, int newy, int newwidth, int newheight, std::string newcaption, std::string newname)
{
	x = newx;
	y = newy;
	width = newwidth;
	height = newheight;
	name = newname;
	SetCaption(newcaption);
}

void Button::SetCaption(std::string newcaption) { caption = newcaption; }

void Button::SetTooltip(std::string newtooltip) { tooltip = newtooltip; }

void Button::SetStandardImage(Image* newimage)
{
	if (image_standard) {
		delete image_standard;
	}
	image_standard = newimage;
}

void Button::SetImages(Image* newstandard, Image* newhighlighted, Image* newclicked)
{
	if (image_standard) {
		delete image_standard;
	}
	if (image_highlighted) {
		delete image_highlighted;
	}
	if (image_clicked) {
		delete image_clicked;
	}

	image_standard = newstandard;
	image_highlighted = newhighlighted;
	image_clicked = newclicked;
}

void Button::RegisterDrawFunction(void (*newdraw)(Button*, bool, bool)) { draw = newdraw; }

void Button::RegisterMouseUpFunction(std::function<void(Button*)> newmouseup) { mouseup = newmouseup; }

void Button::RegisterMouseDownFunction(std::function<void(Button*)> newmousedown)
{

	mousedown = newmousedown;
}

void Button::RegisterMouseMoveFunction(std::function<void(Button*)> newmousemove)
{

	mousemove = newmousemove;
}

void Button::RegisterMiddleClickFunction(std::function<void(Button*)> newmiddleclick)
{
	middleclick = newmiddleclick;
}

void Button::Dirty() { dirty = true; }

void Button::Draw(bool highlighted, bool clicked)
{

	if (draw) {
		draw(this, highlighted, clicked);
	}
}

void Button::MouseUp()
{

	if (mouseup) {
		mouseup(this);
	}
}

void Button::MouseDown()
{

	if (mousedown) {
		mousedown(this);
	}
}

void Button::MouseMove()
{

	if (mousemove) {
		mousemove(this);
	}
}

void Button::MiddleClick()
{
	if (middleclick) {
		middleclick(this);
	}
}

void Button::DebugPrint()
{

	spdlog::info("BUTTON : name:'%s', x:%d, y:%d, width:%d, height:%d\n", name.c_str(), x, y, width, height);
	spdlog::info("         caption:'%s', tooltip:'%s'\n", caption.c_str(), tooltip.c_str());
}
