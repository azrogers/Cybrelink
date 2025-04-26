
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

void EclRegisterButton(int x, int y, int width, int height, const std::string& caption, const std::string& name);

void EclRegisterButton(
	int x, int y, int width, int height, const std::string& caption, const std::string& tooltip, const std::string& name);

void EclRegisterImageButton(
	int x, int y, int width, int height, const std::string& caption, const std::string& tooltip, const std::string& name);

void EclRemoveButton(const std::string& name);

void EclButtonBringToFront(const std::string& name);
void EclButtonSendToBack(const std::string& name);

template <typename F> std::function<F> make_button_function(F* f)
{
	return static_cast<void (*)(Button*, bool, bool)>(f);
}

void EclRegisterDefaultButtonCallbacks(void (*draw)(Button*, bool, bool),
									   std::function<void(Button*)> mouseup,
									   std::function<void(Button*)> mousedown,
									   std::function<void(Button*)> mousemove);

void EclRegisterButtonCallbacks(const std::string& name,
								void (*draw)(Button*, bool, bool),
								std::function<void(Button*)> mouseup,
								std::function<void(Button*)> mousedown,
								std::function<void(Button*)> mousemove);

void EclRegisterButtonCallback(const std::string& name, std::function<void(Button*)> mouseup);

void EclRegisterMiddleClickCallback(const std::string& name, std::function<void(Button*)> middleclick);

void EclMakeButtonEditable(const std::string& name);
void EclMakeButtonUnEditable(const std::string& name);
bool EclIsButtonEditable(const std::string& name);
void EclHighlightNextEditableButton();

// Graphical functions ========================================================

void EclDirtyButton(const std::string& name); // Tells eclipse that this needs re-drawing
void EclDirtyRectangle(int x, int y, int w, int h); // Tells eclipse to blank this area
void EclDirtyClear(); // Clears all dirty buttons / rectangles
void EclClearRectangle(int x, int y, int w, int h); // Tells eclipse to blank this area (now)

bool EclIsOccupied(int x, int y, int w, int h); // True if there is a button here

void EclDrawAllButtons();
void EclDrawButton(const std::string& name);
void EclDrawButton(Button* button);

void EclHighlightButton(const std::string& name);
bool EclIsHighlighted(const std::string& name);
void EclUnHighlightButton();

void EclClickButton(const std::string& name);
bool EclIsClicked(const std::string& name);
void EclUnClickButton();

void EclSuperHighlight(const std::string& name);
bool EclIsSuperHighlighted(const std::string& name);
void EclSuperUnHighlight(const std::string& name);
void EclSuperUnHighlightAll();
void EclUpdateSuperHighlights(const std::string& name);

void EclRegisterClearDrawFunction(std::function<void(int, int, int, int)> draw);
void EclRegisterSuperHighlightFunction(int borderwidth, void (*draw)(Button*, bool, bool));

// Lookup functions ===========================================================

int EclLookupIndex(const std::string& name); // Can change
Button* EclGetButtonAtCoord(int x, int y);
Button* EclGetHighlightedButton();
Button* EclGetButton(const std::string& name);

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
	const std::string& bname, int targetX, int targetY, int time_ms, std::function<void()> callback = NULL);
int EclRegisterMovement(const std::string& bname,
						int targetX,
						int targetY,
						int time_ms,
						int MOVETYPE,
						std::function<void()> callback = NULL);
int EclRegisterResize(
	const std::string& bname, int targetW, int targetH, int time_ms, std::function<void()> callback = NULL);
int EclRegisterAnimation(const std::string& bname,
						 int targetX,
						 int targetY,
						 int targetW,
						 int targetH,
						 int time_ms,
						 std::function<void()> callback = NULL);

int EclRegisterCaptionChange(const std::string& bname,
							 const std::string& targetC,
							 int time_ms,
							 std::function<void()> callback = NULL);
int EclRegisterCaptionChange(const std::string& bname,
							 const std::string& targetC, // Determines time_ms from
							 std::function<void()> callback = NULL); // length of caption

int EclRegisterAnimation(const std::string& bname,
						 int targetX,
						 int targetY,
						 int MOVETYPE,
						 int targetW,
						 int targetH,
						 const std::string& targetC,
						 int time_ms,
						 std::function<void()> callback = NULL);

int EclIsAnimationActive(const std::string& bname); // Returns ID number of anim, or -1
int EclIsCaptionChangeActive(const std::string& bname);
int EclIsNoCaptionChangeActive(const std::string& bname);

void EclRemoveAnimation(int idnumber);

int EclAnimationsRunning();
void EclUpdateAllAnimations();

double EclGetAccurateTime(); // Returns the time in millisecond accuracy

// ============================================================================

void EclDebugPrint(); // Prints out all state information

#endif
