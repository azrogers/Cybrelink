
/*
   ============================================================================

			E C L I P S E

		  Interface Library
		By Christopher Delay

		Started 23rd May '99

   ============================================================================
*/

#ifndef _included_eclipse_h
#define _included_eclipse_h

/*
#ifdef DMALLOC
#include "dmalloc.h"
#endif
*/

/*
#ifdef _DEBUG
#include "slasher.h"
#endif
*/

#include <functional>

#include "animation.h"
#include "button.h"

#include "tosser.h"

// Initialisation functions ===================================================

void EclReset(int width, int height);

void EclRegisterButton(int x, int y, int width, int height, std::string caption, std::string name);

void EclRegisterButton(
	int x, int y, int width, int height, std::string caption, std::string tooltip, std::string name);

void EclRegisterImageButton(
	int x, int y, int width, int height, std::string caption, std::string tooltip, std::string name);

void EclRemoveButton(std::string name);

void EclButtonBringToFront(std::string name);
void EclButtonSendToBack(std::string name);

template <typename F> std::function<F> make_button_function(F* f)
{
	return static_cast<void (*)(Button*, bool, bool)>(f);
}

void EclRegisterDefaultButtonCallbacks(void (*draw)(Button*, bool, bool),
									   std::function<void(Button*)> mouseup,
									   std::function<void(Button*)> mousedown,
									   std::function<void(Button*)> mousemove);

void EclRegisterButtonCallbacks(std::string name,
								void (*draw)(Button*, bool, bool),
								std::function<void(Button*)> mouseup,
								std::function<void(Button*)> mousedown,
								std::function<void(Button*)> mousemove);

void EclRegisterButtonCallback(std::string name, std::function<void(Button*)> mouseup);

void EclRegisterMiddleClickCallback(std::string name, std::function<void(Button*)> middleclick);

void EclMakeButtonEditable(std::string name);
void EclMakeButtonUnEditable(std::string name);
bool EclIsButtonEditable(std::string name);
void EclHighlightNextEditableButton();

// Graphical functions ========================================================

void EclDirtyButton(std::string name); // Tells eclipse that this needs re-drawing
void EclDirtyRectangle(int x, int y, int w, int h); // Tells eclipse to blank this area
void EclDirtyClear(); // Clears all dirty buttons / rectangles
void EclClearRectangle(int x, int y, int w, int h); // Tells eclipse to blank this area (now)

bool EclIsOccupied(int x, int y, int w, int h); // True if there is a button here

void EclDrawAllButtons();
void EclDrawButton(std::string name);
void EclDrawButton(Button* button);

void EclHighlightButton(std::string name);
bool EclIsHighlighted(std::string name);
void EclUnHighlightButton();

void EclClickButton(std::string name);
bool EclIsClicked(std::string name);
void EclUnClickButton();

void EclSuperHighlight(std::string name);
bool EclIsSuperHighlighted(std::string name);
void EclSuperUnHighlight(std::string name);
void EclSuperUnHighlightAll();
void EclUpdateSuperHighlights(std::string name);

void EclRegisterClearDrawFunction(std::function<void(int, int, int, int)> draw);
void EclRegisterSuperHighlightFunction(int borderwidth, void (*draw)(Button*, bool, bool));

// Lookup functions ===========================================================

int EclLookupIndex(std::string name); // Can change
Button* EclGetButtonAtCoord(int x, int y);
Button* EclGetHighlightedButton();
Button* EclGetButton(std::string name);

// Animation functions ========================================================

#define MOVE_STRAIGHTLINE 1
#define MOVE_XFIRST 2
#define MOVE_YFIRST 3

void EclEnableAnimations(); // Default
void EclDisableAnimations();

#define ECL_FASTER_ANIMATION_SPEED 2.0

void EclEnableFasterAnimations(double speed = ECL_FASTER_ANIMATION_SPEED);
void EclDisableFasterAnimations(); // Default

int EclRegisterMovement(
	std::string bname, int targetX, int targetY, int time_ms, std::function<void()> callback = NULL);
int EclRegisterMovement(std::string bname,
						int targetX,
						int targetY,
						int time_ms,
						int MOVETYPE,
						std::function<void()> callback = NULL);
int EclRegisterResize(
	std::string bname, int targetW, int targetH, int time_ms, std::function<void()> callback = NULL);
int EclRegisterAnimation(std::string bname,
						 int targetX,
						 int targetY,
						 int targetW,
						 int targetH,
						 int time_ms,
						 std::function<void()> callback = NULL);

int EclRegisterCaptionChange(std::string bname,
							 std::string targetC,
							 int time_ms,
							 std::function<void()> callback = NULL);
int EclRegisterCaptionChange(std::string bname,
							 std::string targetC, // Determines time_ms from
							 std::function<void()> callback = NULL); // length of caption

int EclRegisterAnimation(std::string bname,
						 int targetX,
						 int targetY,
						 int MOVETYPE,
						 int targetW,
						 int targetH,
						 std::string targetC,
						 int time_ms,
						 std::function<void()> callback = NULL);

int EclIsAnimationActive(std::string bname); // Returns ID number of anim, or -1
int EclIsCaptionChangeActive(std::string bname);
int EclIsNoCaptionChangeActive(std::string bname);

void EclRemoveAnimation(int idnumber);

int EclAnimationsRunning();
void EclUpdateAllAnimations();

double EclGetAccurateTime(); // Returns the time in millisecond accuracy

// ============================================================================

void EclDebugPrint(); // Prints out all state information

#endif
