
/*

  Arrest event

	Occurs when a person is arrested by federal agents
	(usually for hacking)

  */

#ifndef _included_arrestevent_h
#define _included_arrestevent_h

#include "world/person.h"
#include "world/scheduler/uplinkevent.h"

class ArrestEvent : public UplinkEvent {

protected:
	char* reason;
	char* ip;

public:
	char name[SIZE_PERSON_NAME];

public:
	ArrestEvent();
	~ArrestEvent();

	void SetName(const char* newname);
	void SetReason(const char* newreason);
	void SetIP(char* newip);

	void Run();

	char* GetShortString();
	char* GetLongString();

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();

	std::string GetID();
	int GetOBJECTID();
};

#endif
