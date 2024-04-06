
#ifndef _included_animation_h
#define _included_animation_h

#include "button.h"

class Animation {

public:
	Animation();

	Animation& operator=(Animation& other)
	{
		this->id = other.id;
		this->buttonname = other.buttonname;
		this->button = other.button;
		this->sourceX = other.sourceX;
		this->sourceY = other.sourceY;
		this->targetX = other.targetX;
		this->targetY = other.targetY;
		this->dX = other.dX;
		this->dY = other.dY;

		this->sourceW = other.sourceW;
		this->sourceH = other.sourceH;
		this->targetW = other.targetW;
		this->targetH = other.targetH;
		this->dW = other.dW;
		this->dH = other.dH;
		this->targetC = other.targetC;
		this->dC = other.dC;
		this->time = other.time;
		this->starttime = other.starttime;
		this->finishtime = other.finishtime;
		this->MOVETYPE = other.MOVETYPE;
		this->callback = other.callback;
		return *this;
	}

	int id;
	std::string buttonname;
	Button* button;
	int sourceX, sourceY; // Where the button started
	int targetX, targetY; // Where the button should end up
	float dX, dY; // Amount to move for every time interval

	int sourceW, sourceH; // Origional size of button
	int targetW, targetH; // Target size of button
	float dW, dH; // Amount to resize for every time interval

	std::string targetC; // Target caption
	float dC; // How much of caption to add on every TI

	int time; // How long it should take
	int starttime; // Time animation was started
	int finishtime; // Time when animation should be finished
	int MOVETYPE; // Numeric constant - eg MOVE_XFIRST

	std::function<void()> callback; // Called when animation has finished

	void DebugPrint(); // Print out all state information
};

#endif
