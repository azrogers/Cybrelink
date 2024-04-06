// UplinkTask.cpp: implementation of the UplinkTask class.
//
//////////////////////////////////////////////////////////////////////

#include "game/game.h"

#include "interface/interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/uplinktask.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UplinkTask::UplinkTask() :
	Task()
{

	targetobject = NULL;
	targetstring = "";
	targetint = -1;
	version = 1.0;
	followmouse = false;
}

UplinkTask::~UplinkTask() { }

void UplinkTask::SetVersion(float newversion) { version = newversion; }

void UplinkTask::SetFollowMouse(bool newfollowmouse) { followmouse = newfollowmouse; }

void UplinkTask::SetTarget(UplinkObject* newtargetobject, std::string& newtargetstring, int targetint) { }

void UplinkTask::MoveTo(int x, int y, int time_ms) { }
