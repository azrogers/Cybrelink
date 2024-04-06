
#define FUDGE 10

#include <algorithm>
#include <chrono>
#include <format>
#include <iterator>
#include <list>
#include <set>
#include <vector>

#include <spdlog/spdlog.h>

#include "eclipse.h"
#include "tosser.h"

using namespace std;

#include <assert.h>

// Protected members ==========================================================

static list<Button> buttons;
static string currenthighlight;
static string currentclick;

static list<Animation> anims;
static bool animsenabled = true;
static bool animsfasterenabled = false;
static double animsfasterspeed = ECL_FASTER_ANIMATION_SPEED;

static std::set<std::string> superhighlightedbuttons;
static int superhighlight_borderwidth;

class dirtyrect {

public:
	dirtyrect() { x = y = width = height = 0; }
	dirtyrect(int newx, int newy, int newwidth, int newheight) :
		x(newx),
		y(newy),
		width(newwidth),
		height(newheight)
	{
	}
	~dirtyrect() { }

	int x;
	int y;
	int width;
	int height;
};

static vector<dirtyrect> dirtyrectangles;
static list<std::string> editablebuttons;

// Default button callbacks

static void (*default_draw)(Button*, bool, bool) = NULL;
static std::function<void(Button*)> default_mouseup = NULL;
static std::function<void(Button*)> default_mousedown = NULL;
static std::function<void(Button*)> default_mousemove = NULL;

static std::function<void(int, int, int, int)> clear_draw = NULL;

static void (*superhighlight_draw)(Button*, bool, bool) = NULL;

// ============================================================================

void EclReset(int width, int height)
{
	while (!buttons.empty()) {
		Button& b = buttons.front();
		EclRemoveButton(b.name);
	}

	// Also delete the animations
	// TODO : Doing this stops the animation of the caption on the log in screen
	// After you load a game, then exit back to the login screen
	/*
		for ( int j = 0; j < anims.Size (); ++j )
			if ( anims.ValidIndex (j) )
				if ( anims.GetData (j) )
					anims.GetData (j)->DebugPrint ();
	*/

	editablebuttons.clear();
	buttons.clear();

	superhighlight_borderwidth = 0;

	// TODO sort this out sometime
	EclDirtyClear();
	EclDirtyRectangle(0, 0, width, height);
}

void EclRegisterButton(int x, int y, int width, int height, std::string caption, std::string name)
{
	if (EclGetButton(name)) {
		spdlog::warn("ECL WARNING : EclRegisterButton called, Button name not unique : %s\n", name);
		return;
	}

	buttons.push_back(Button(x, y, width, height, caption, name));

	EclRegisterButtonCallbacks(name, default_draw, default_mouseup, default_mousedown, default_mousemove);

	EclDirtyButton(name);
}

void EclRegisterButton(
	int x, int y, int width, int height, std::string caption, std::string tooltip, std::string name)
{
	EclRegisterButton(x, y, width, height, caption, name);
	EclGetButton(name)->SetTooltip(tooltip);
}

void EclRegisterImageButton(
	int x, int y, int width, int height, std::string caption, std::string tooltip, std::string name)
{
	if (EclGetButton(name)) {
		spdlog::warn("ECL WARNING : EclRegisterImageButton called, Button name not unique : %s\n", name);
		return;
	}

	buttons.push_back(Button(x, y, width, height, caption, name));

	EclRegisterButtonCallbacks(name, default_draw, default_mouseup, default_mousedown, default_mousemove);

	EclDirtyButton(name);
}

void EclRemoveButton(std::string name)
{
	Button* button = EclGetButton(name);
	if (button == nullptr) {
		spdlog::warn("ECL WARNING : EclRemoveButton called, button does not exist : %s\n", name);
		return;
	}

	if (EclIsHighlighted(name)) {
		currenthighlight = "";
	}

	if (EclIsClicked(name)) {
		currentclick = name;
	}

	if (EclIsSuperHighlighted(name)) {
		EclSuperUnHighlight(name);
	}

	if (EclIsButtonEditable(name)) {
		EclMakeButtonUnEditable(name);
	}

	EclDirtyRectangle(button->x, button->y, button->width, button->height);

	anims.remove_if([name](Animation& anim) { return anim.buttonname == name; });
	buttons.remove_if([name](Button& button) { return button.name == name; });
}

void EclButtonBringToFront(std::string name)
{
	for (auto it = buttons.begin(); it != buttons.end(); ++it) {
		if (it->name == name) {
			buttons.splice(buttons.end(), buttons, it);
			EclDirtyButton(name);
			return;
		}
	}

	spdlog::warn("ECL WARNING : EclButtonBringToFront called, button does not exist : %s\n", name);
}

void EclButtonSendToBack(std::string name)
{
	for (auto it = buttons.begin(); it != buttons.end(); ++it) {
		if (it->name == name) {
			buttons.splice(buttons.begin(), buttons, it);
			EclDirtyButton(name);
			return;
		}
	}

	spdlog::warn("ECL WARNING : EclButtonBringToFront called, button does not exist : %s\n", name);
}

void EclRegisterDefaultButtonCallbacks(void (*draw)(Button*, bool, bool),
									   std::function<void(Button*)> mouseup,
									   std::function<void(Button*)> mousedown,
									   std::function<void(Button*)> mousemove)
{
	default_draw = draw;
	default_mouseup = mouseup;
	default_mousedown = mousedown;
	default_mousemove = mousemove;
}

void EclRegisterButtonCallbacks(std::string name,
								void (*draw)(Button*, bool, bool),
								std::function<void(Button*)> mouseup,
								std::function<void(Button*)> mousedown,
								std::function<void(Button*)> mousemove)
{

	Button* button = EclGetButton(name);

	if (button != nullptr) {
		button->RegisterDrawFunction(draw);
		button->RegisterMouseUpFunction(mouseup);
		button->RegisterMouseDownFunction(mousedown);
		button->RegisterMouseMoveFunction(mousemove);
	} else {
		spdlog::warn("ECL WARNING : EclRegisterButtonCallbacks called, button does not exist : %s\n", name);
	}
}

void EclRegisterButtonCallback(std::string name, std::function<void(Button*)> mouseup)
{
	Button* button = EclGetButton(name);

	if (button != nullptr) {
		button->RegisterMouseUpFunction(mouseup);
	} else {
		spdlog::warn("ECL WARNING : EclRegisterButtonCallback called, button does not exist : %s\n", name);
	}
}

void EclRegisterMiddleClickCallback(std::string name, std::function<void(Button*)> middleclick)
{
	Button* button = EclGetButton(name);

	if (button != nullptr) {
		button->RegisterMiddleClickFunction(middleclick);
	} else {
		spdlog::warn("ECL WARNING : EclRegisterButtonCallback called, button does not exist : %s\n", name);
	}
}

void EclMakeButtonEditable(std::string name)
{
	Button* button = EclGetButton(name);

	if (button != nullptr) {
		editablebuttons.push_back(name);
	} else {
		spdlog::warn("ECL WARNING : EclMakeButtonEditable called, invalid button name\n");
	}
}

void EclMakeButtonUnEditable(std::string name)
{
	editablebuttons.remove_if([name](std::string n) { return name == n; });
}

bool EclIsButtonEditable(std::string name)
{
	if (!EclGetButton(name)) {
		return false;
	}

	return std::find(editablebuttons.begin(), editablebuttons.end(), name) != editablebuttons.end();
}

void EclHighlightNextEditableButton()
{
	//
	// Look to see if an existing button is highlighted
	//

	if (currenthighlight.length() > 0) {
		for (auto it = editablebuttons.begin(); it != editablebuttons.end(); ++it) {
			if (*it == currenthighlight) {
				auto nextIt = std::next(it, 1);
				std::string& nextButton = nextIt == editablebuttons.end() ? editablebuttons.front() : *nextIt;
				EclHighlightButton(nextButton);
				return;
			}
		}
	}

	//
	// Nothing editable is currentlyhighlighted, so move to the first
	//

	if (!editablebuttons.empty()) {
		EclHighlightButton(editablebuttons.front());
	}
}

int EclLookupIndex(std::string name)
{
	auto it = buttons.begin();
	for (int i = 0; i < buttons.size(); i++) {
		if (it->name == name) {
			return i;
		}
		++it;
	}

	return -1;
}

static bool EclRectangleOverlap(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{

	int maxleft = x1 > x2 ? x1 : x2;
	int maxtop = y1 > y2 ? y1 : y2;

	int minright = x1 + w1 < x2 + w2 ? x1 + w1 : x2 + w2;
	int minbottom = y1 + h1 < y2 + h2 ? y1 + h1 : y2 + h2;

	if (maxtop < minbottom && maxleft < minright) {
		return true;
	}

	else {
		return false;
	}
}

bool EclIsOccupied(int x, int y, int w, int h)
{
	for (Button& b : buttons) {
		if (EclRectangleOverlap(b.x, b.y, b.width, b.height, x, y, w, h)) {
			return true;
		}
	}

	return false;
}

void EclDirtyButton(std::string name) { }

void EclDirtyRectangle(int x, int y, int w, int h) { }

void EclDrawAllButtons()
{
	// Draw all buttons

	for (Button& b : buttons) {
		if (b.x >= 0 && b.y >= 0) {
			EclDrawButton(&b);
		}
	}

	dirtyrectangles.clear();
}

void EclClearRectangle(int x, int y, int w, int h)
{
	if (clear_draw) {
		clear_draw(x, y, w, h);
	}
}

void EclDirtyClear()
{
	dirtyrectangles.clear();

	for (Button& b : buttons) {
		b.dirty = false;
	}
}

void EclDrawButton(std::string name) { EclDrawButton(EclGetButton(name)); }

void EclDrawButton(Button* button)
{
	if (button == nullptr) {
		spdlog::error("ECL ERROR : attempted to render null button");
		return;
	}

	if (EclIsClicked(button->name)) {
		button->Draw(false, true);
	} else if (EclIsHighlighted(button->name)) {
		button->Draw(true, false);
	} else {
		button->Draw(false, false);
	}
}

void EclHighlightButton(std::string name)
{
	if (!EclIsHighlighted(name)) {
		EclUnHighlightButton();
		currenthighlight = name;
		EclDirtyButton(name);
	}
}

void EclClickButton(std::string name)
{
	if (!EclIsClicked(name)) {

		EclUnClickButton();
		currentclick = name;
		EclDirtyButton(name);
	}
}

bool EclIsHighlighted(std::string name) { return currenthighlight == name; }

bool EclIsClicked(std::string name) { return currentclick == name; }

void EclUnHighlightButton()
{
	Button* button = EclGetButton(currenthighlight);
	if (button) {
		EclDirtyButton(button->name);
	}

	currenthighlight = "";
}

void EclUnClickButton()
{
	Button* button = EclGetButton(currentclick);
	if (button) {
		EclDirtyButton(button->name);
	}

	currentclick = "";
}

void EclSuperHighlight(std::string name)
{

	Button* button = EclGetButton(name);
	if (button != nullptr && !superhighlightedbuttons.contains(name)) {
		superhighlightedbuttons.insert(name);

		// Create the button itself
		int x = button->x - superhighlight_borderwidth;
		int y = button->y - superhighlight_borderwidth;
		int width = button->width + superhighlight_borderwidth * 2;
		int height = button->height + superhighlight_borderwidth * 2;

		std::string superhighlightname = std::format("Ecl_superhighlight %s", name);
		EclRegisterButton(x, y, width, height, "", "", superhighlightname);
		EclRegisterButtonCallbacks(superhighlightname, superhighlight_draw, NULL, NULL, NULL);

		EclButtonSendToBack(superhighlightname);
	}
}

void EclSuperUnHighlight(std::string name)
{
	if (superhighlightedbuttons.contains(name)) {
		superhighlightedbuttons.erase(name);
		std::string superhighlightname = std::format("Ecl_superhighlight %s", name);
		EclRemoveButton(name);

	} else {
		spdlog::warn("ECL WARNING : SuperUnHighlight called, button not found : %s\n", name);
	}
}

void EclSuperUnHighlightAll()
{
	for (std::string key : superhighlightedbuttons) {
		EclSuperUnHighlight(key);
	}
}

bool EclIsSuperHighlighted(std::string name) { return superhighlightedbuttons.contains(name); }

void EclRegisterClearDrawFunction(std::function<void(int, int, int, int)> draw) { clear_draw = draw; }

void EclRegisterSuperHighlightFunction(int borderwidth, void (*draw)(Button*, bool, bool))
{
	superhighlight_borderwidth = borderwidth;
	superhighlight_draw = draw;
}

void EclUpdateSuperHighlights(std::string name)
{
	Button* sourcebutton = EclGetButton(name);

	if (!sourcebutton) {

		EclSuperUnHighlight(name);
		return;
	}

	if (EclIsSuperHighlighted(name)) {
		std::string superhighlight_name = std::format("Ecl_superhighlight %s", name);
		Button* highlightbutton = EclGetButton(superhighlight_name);

		if (highlightbutton != nullptr) {
			// Update the position of this highlight button
			// to keep up with the actual button

			if (highlightbutton->x != sourcebutton->x || highlightbutton->y != sourcebutton->y) {

				EclDirtyRectangle(
					highlightbutton->x, highlightbutton->y, highlightbutton->width, highlightbutton->height);

				highlightbutton->x = sourcebutton->x - superhighlight_borderwidth;
				highlightbutton->y = sourcebutton->y - superhighlight_borderwidth;
			}

			EclDirtyButton(superhighlight_name);

		} else {
			spdlog::warn(
				"ECL WARNING : UpdateSuperHighlights called, button is SuperHighlighted : %s\nBut no "
				"SuperHighlight button exists\n",
				name);
		}

	} else {
		spdlog::warn("ECL WARNING : UpdateSuperHighlights called, button is not SuperHighlighted : %s\n",
					 name);
	}
}

Button* EclGetButtonAtCoord(int x, int y)
{
	for (Button& b : buttons) {
		if (x >= b.x && x <= b.x + b.width && y >= b.y && y <= b.y + b.height) {
			return &b;
		}
	}

	return nullptr;
}

Button* EclGetHighlightedButton()
{
	return currenthighlight.length() > 0 ? EclGetButton(currenthighlight) : nullptr;
}

Button* EclGetButton(std::string name)
{
	auto it = std::find_if(buttons.begin(), buttons.end(), [name](Button& b) { return b.name == name; });
	if (it == buttons.end()) {
		return nullptr;
	}

	return &*it;
}

void EclEnableAnimations() { animsenabled = true; }

void EclDisableAnimations() { animsenabled = false; }

void EclEnableFasterAnimations(double speed)
{
	animsfasterenabled = true;
	animsfasterspeed = speed;
}

void EclDisableFasterAnimations() { animsfasterenabled = false; }

int EclRegisterMovement(
	std::string bname, int targetX, int targetY, int time_ms, std::function<void()> callback)
{

	return EclRegisterMovement(bname, targetX, targetY, time_ms, MOVE_STRAIGHTLINE, callback);
}

int EclRegisterMovement(
	std::string bname, int targetX, int targetY, int time_ms, int MOVETYPE, std::function<void()> callback)
{

	Button* button = EclGetButton(bname);
	if (button) {
		return EclRegisterAnimation(
			bname, targetX, targetY, MOVETYPE, button->width, button->height, "", time_ms, callback);
	}

	else {
		return -1;
	}
}

int EclRegisterResize(
	std::string bname, int targetW, int targetH, int time_ms, std::function<void()> callback)
{

	Button* button = EclGetButton(bname);
	if (button) {
		return EclRegisterAnimation(bname, button->x, button->y, targetW, targetH, time_ms, callback);
	}

	else {
		return -1;
	}
}

int EclRegisterAnimation(std::string bname,
						 int targetX,
						 int targetY,
						 int targetW,
						 int targetH,
						 int time_ms,
						 std::function<void()> callback)
{

	return EclRegisterAnimation(
		bname, targetX, targetY, MOVE_STRAIGHTLINE, targetW, targetH, "", time_ms, callback);
}

int EclRegisterCaptionChange(std::string bname,
							 std::string targetC,
							 int time_ms,
							 std::function<void()> callback)
{

	Button* button = EclGetButton(bname);
	if (button) {
		return EclRegisterAnimation(bname,
									button->x,
									button->y,
									MOVE_STRAIGHTLINE,
									button->width,
									button->height,
									targetC,
									time_ms,
									callback);
	}

	else {
		spdlog::warn("ECL WARNING : Tried to register a caption change, could not find button %s\n", bname);
		return -1;
	}
}

int EclRegisterCaptionChange(std::string bname, std::string targetC, std::function<void()> callback)
{
	int time = (int)(targetC.length() * 20);
	return EclRegisterCaptionChange(bname, targetC, time, callback);
}

int EclAnimationsRunning() { return anims.size() > 0; }

int EclRegisterAnimation(std::string bname,
						 int targetX,
						 int targetY,
						 int MOVETYPE,
						 int targetW,
						 int targetH,
						 std::string targetC,
						 int time_ms,
						 std::function<void()> callback)
{
	Button* button = EclGetButton(bname);

	if (button) {
		Animation anim;
		anim.buttonname = bname;
		anim.button = button;
		anim.MOVETYPE = MOVETYPE;

		anim.sourceX = button->x;
		anim.sourceY = button->y;
		anim.targetX = targetX;
		anim.targetY = targetY;
		anim.sourceW = button->width;
		anim.sourceH = button->height;
		anim.targetW = targetW;
		anim.targetH = targetH;

		anim.callback = callback;

		anim.time = time_ms;
		if (animsfasterenabled) {
			anim.time = (int)(anim.time / animsfasterspeed);
		}

		anim.starttime = (int)EclGetAccurateTime();
		anim.finishtime = (int)EclGetAccurateTime() + anim.time;

		if (anim.finishtime - anim.starttime == 0) {
			anim.finishtime = anim.starttime + 1;
		}

		if (anim.targetX != anim.sourceX || anim.targetY != anim.sourceY) {

			// Button will move
			anim.dX = float(anim.targetX - anim.sourceX) / float(anim.finishtime - anim.starttime);
			anim.dY = float(anim.targetY - anim.sourceY) / float(anim.finishtime - anim.starttime);

		} else {

			// Button will not move
			anim.dX = 0.0;
			anim.dY = 0.0;
		}

		if (anim.targetW != anim.sourceW || anim.targetH != anim.sourceH) {

			// Button will resize
			anim.dW = float(anim.targetW - anim.sourceW) / float(anim.finishtime - anim.starttime);
			anim.dH = float(anim.targetH - anim.sourceH) / float(anim.finishtime - anim.starttime);

		} else {

			// Button will not resize
			anim.dW = 0.0;
			anim.dH = 0.0;
		}

		if (!targetC.empty()) {

			// Button caption will change
			// Remove ALL existing caption change animations

			int ca = EclIsCaptionChangeActive(bname);
			while (ca != -1) {
				EclRemoveAnimation(ca);
				ca = EclIsCaptionChangeActive(bname);
			}

			anim.targetC = targetC;
			anim.dC = float(targetC.length()) / float(anim.finishtime - anim.starttime);

		} else {

			// Button caption will not change
			anim.targetC = "";
			anim.dC = 0.0;
		}

		anims.push_back(std::move(anim));
		return anim.id;
	} else {
		// Could not find button
		return -1;
	}
}

int EclIsAnimationActive(std::string bname)
{
	auto it = anims.begin();
	for (int i = 0; i < anims.size(); i++) {
		Animation& anim = *it;
		if (anim.buttonname == bname) {
			return anim.id;
		}
		++it;
	}

	return -1;
}

int EclIsCaptionChangeActive(std::string bname)
{
	auto it = anims.begin();
	for (int i = 0; i < anims.size(); i++) {
		Animation& anim = *it;
		if (anim.buttonname == bname && !anim.targetC.empty()) {
			return anim.id;
		}
		++it;
	}

	return -1;
}

int EclIsNoCaptionChangeActive(std::string bname)
{
	auto it = anims.begin();
	for (int i = 0; i < anims.size(); i++) {
		Animation& anim = *it;
		if (anim.buttonname == bname && anim.targetC.empty()) {
			return anim.id;
		}
		++it;
	}

	return -1;
}

void EclRemoveAnimation(int idnumber)
{
	anims.remove_if([idnumber](Animation& a) { return a.id == idnumber; });
}

void EclUpdateAllAnimations()
{

	const double time = EclGetAccurateTime();

	//
	// Update all super-highlighted buttons
	//

	for (std::string h : superhighlightedbuttons) {
		EclUpdateSuperHighlights(h);
	}

	std::vector<int> animsToRemove;

	//
	// Update all animations
	//

	for (auto it = anims.begin(); it != anims.end(); ++it) {
		Animation* anim = &*it;
		assert(anim);

		// Button may have been deleted - attempt to re-aquire it here
		anim->button = EclGetButton(anim->buttonname);

		if (!anim->button) {

			// This button has been destroyed - remove the anim
			spdlog::warn("ECL WARNING : Animation defined on invalid button : %s\n", anim->buttonname);

			animsToRemove.push_back(anim->id);
			continue;
		}

		EclDirtyButton(anim->buttonname);

		if (time >= anim->finishtime || !animsenabled) {

			// Animation has finished - set to final position / size / caption
			// OR Animations are not enabled - set to final position as well

			if (anim->dX || anim->dY) {

				EclDirtyRectangle(
					anim->button->x, anim->button->y, anim->button->width, anim->button->height);

				anim->button->x = anim->targetX;
				anim->button->y = anim->targetY;
			}

			if (anim->dW || anim->dH) {

				EclDirtyRectangle(
					anim->button->x, anim->button->y, anim->button->width, anim->button->height);

				anim->button->width = anim->targetW;
				anim->button->height = anim->targetH;
			}

			if (anim->dC) {

				anim->button->SetCaption(anim->targetC);
			}

			// Update any SuperHighlights that exist on this button

			if (EclIsSuperHighlighted(anim->buttonname)) {
				EclUpdateSuperHighlights(anim->buttonname);
			}

			// Call the callback if neccesary
			if (anim->callback) {
				anim->callback();
			}

			// Remove the animation from the list
			animsToRemove.push_back(anim->id);
		} else {

			if (anim->MOVETYPE == MOVE_STRAIGHTLINE) {

				// Button moves at a constant rate in a straight line to target

				if (anim->dX || anim->dY) {

					int dX = (int)(anim->sourceX + (time - anim->starttime) * anim->dX) - anim->button->x;
					int dY = (int)(anim->sourceY + (time - anim->starttime) * anim->dY) - anim->button->y;

					// EclDirtyRectangle ( anim->button->x, anim->button->y, dX, anim->button->height
					// );
					EclDirtyRectangle(
						anim->button->x, anim->button->y, anim->button->width, anim->button->height);

					// Buttons location can change
					// Calculate location of button
					anim->button->x = anim->sourceX + (int)((time - anim->starttime) * anim->dX);
					anim->button->y = anim->sourceY + (int)((time - anim->starttime) * anim->dY);
					// 							double now = time;
					// 							double f = ( now - anim->starttime ) /
					// (anim->finishtime - anim->starttime);
					// anim->button->x = anim->sourceX + (anim->targetX - anim->sourceX) * f;
					// anim->button->y = anim->sourceY + (anim->targetY - anim->sourceY) * f;
				}

			} else if (anim->MOVETYPE == MOVE_XFIRST) {

				// Button moves in X direction, then in Y direction

				if (anim->dX || anim->dY) {

					EclDirtyRectangle(
						anim->button->x, anim->button->y, anim->button->width, anim->button->height);

					float ratio = (anim->dX / (anim->dX + anim->dY));
					int halftime = anim->starttime + (int)(ratio * (anim->finishtime - anim->starttime));

					if (time < halftime) {
						float firsthalf =
							(float)(time - anim->starttime) / (float)(halftime - anim->starttime);
						anim->button->x = anim->sourceX + (int)((anim->targetX - anim->sourceX) * firsthalf);
					} else {
						float secondhalf = (float)(time - halftime) / (float)(anim->finishtime - halftime);
						anim->button->x = anim->targetX;
						anim->button->y = anim->sourceY + (int)((anim->targetY - anim->sourceY) * secondhalf);
					}
				}

			} else if (anim->MOVETYPE == MOVE_YFIRST) {

				// Button moves in Y direction, then in X direction

				if (anim->dX || anim->dY) {

					EclDirtyRectangle(
						anim->button->x, anim->button->y, anim->button->width, anim->button->height);

					float ratio = (anim->dY / (anim->dX + anim->dY));
					int halftime = anim->starttime + (int)(ratio * (anim->finishtime - anim->starttime));

					if (time < halftime) {
						float firsthalf =
							(float)(time - anim->starttime) / (float)(halftime - anim->starttime);
						anim->button->y = anim->sourceY + (int)((anim->targetY - anim->sourceY) * firsthalf);
					} else {
						float secondhalf = (float)(time - halftime) / (float)(anim->finishtime - halftime);
						anim->button->y = anim->targetY;
						anim->button->x = anim->sourceX + (int)((anim->targetX - anim->sourceX) * secondhalf);
					}
				}
			}

			if (anim->dW || anim->dH) {

				// Buttons size can change
				// Calculate new size
				EclDirtyRectangle(
					anim->button->x, anim->button->y, anim->button->width, anim->button->height);

				anim->button->width = anim->sourceW + (int)((time - anim->starttime) * anim->dW);
				anim->button->height = anim->sourceH + (int)((time - anim->starttime) * anim->dH);
			}

			if (anim->dC) {
				// Buttons caption can change
				size_t showlength = (size_t)(((float)time - anim->starttime) * anim->dC);
				size_t maxlen = anim->targetC.length();
				size_t len = showlength < maxlen ? showlength : maxlen;
				std::string newCaption = std::string(anim->targetC.c_str(), len);
				anim->button->SetCaption(newCaption);
			}

			// Update any SuperHighlights that exist on this button

			if (EclIsSuperHighlighted(anim->buttonname)) {
				EclUpdateSuperHighlights(anim->buttonname);
			}
		}
	}

	for (int id : animsToRemove) {
		EclRemoveAnimation(id);
	}
}

double EclGetAccurateTime()
{
	static chrono::high_resolution_clock::time_point start_time;
	static bool has_prev_time = false;

	chrono::high_resolution_clock::time_point new_time = chrono::high_resolution_clock::now();

	if (!has_prev_time) {
		start_time = new_time;
		has_prev_time = true;
		return 0;
	}

	chrono::milliseconds diff = chrono::duration_cast<chrono::milliseconds>(new_time - start_time);
	return chrono::duration_cast<chrono::duration<double, std::milli>>(diff).count();
}

void EclDebugPrint()
{

	spdlog::info("============== E C L I P S E ===============================\n");

	spdlog::info("BUTTONS : \n");
	for (Button& b : buttons) {
		b.DebugPrint();
	}

	spdlog::info("Current Highlight = %s\n", currenthighlight);
	spdlog::info("Current Click     = %s\n", currentclick);

	spdlog::info("ANIMATIONS : \n");
	if (animsenabled) {
		spdlog::info("Animations are ENABLED\n");
	} else {
		spdlog::info("Animations are DISABLED\n");
	}

	if (animsfasterenabled) {
		spdlog::info("Faster Animations are ENABLED\n");
	} else {
		spdlog::info("Faster Animations are DISABLED\n");
	}

	for (Animation& anim : anims) {
		anim.DebugPrint();
	}

	printf("EDITABLE BUTTONS : \n");

	for (std::string e : editablebuttons) {
		spdlog::info(e);
	}

	spdlog::info("============== E N D  O F  E C L I P S E ===================\n");
}
