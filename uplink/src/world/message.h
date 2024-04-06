
/*

  Message class object

	Stores the data representing a message (system message, email)

  */

#ifndef _included_message_h
#define _included_message_h

// ============================================================================

#include "app/uplinkobject.h"

#include "world/date.h"
#include "world/person.h"

class Data;

// ============================================================================

class Message : public UplinkObject {

protected:
	char* subject;
	char* body;

	Data* data; // An attached file/program

public:
	char to[SIZE_PERSON_NAME];
	char from[SIZE_PERSON_NAME];

	LList<char*> links; // All useful ip addresses (to be added to links)
	BTree<char*> codes; // Access codes, indexed on ip

	Date date; // Date of send

public:
	Message();
	virtual ~Message();

	void SetTo(const char* newto);
	void SetTo(std::string newto) { SetTo(newto.c_str()); }
	void SetFrom(const char* newfrom);
	void SetFrom(std::string newfrom) { SetFrom(newfrom.c_str()); }
	void SetSubject(const char* newsubject);
	void SetSubject(std::string newsubject) { SetSubject(newsubject.c_str()); }
	void SetBody(const char* newbody);
	void SetBody(std::string newbody) { SetBody(newbody.c_str()); }
	void SetDate(Date* newdate);

	void GiveLink(const char* ip);
	void GiveCode(const char* ip, const char* code);

	void AttachData(Data* newdata);
	Data* GetData();

	const char* GetSubject() const;
	const char* GetBody() const;

	void Send();

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();

	std::string GetID();
	int GetOBJECTID();
};

#endif
