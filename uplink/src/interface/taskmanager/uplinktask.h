// UplinkTask.h: interface for the UplinkTask class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_uplinktask_h
#define _included_uplinktask_h

// ============================================================================

#include "app/uplinkobject.h"
#include "task.h"

// ============================================================================

class UplinkTask : public Task {

protected:
	UplinkObject* targetobject;
	std::string targetstring;
	int targetint;

public:
	float version;
	bool followmouse;

public:
	UplinkTask();
	virtual ~UplinkTask();

	void SetVersion(float newversion);
	void SetFollowMouse(bool newfollowmouse);

	virtual void SetTarget(UplinkObject* newtargetobject, std::string& newtargetstring, int targetint);
	virtual void MoveTo(int x, int y, int time_ms);
};

#endif
