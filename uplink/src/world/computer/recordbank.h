

#ifndef _included_recordbank_h
#define _included_recordbank_h

/*

	Record Bank class

	Represents a bank of computer records
	ie a Database

  */

#include "app/uplinkobject.h"

class Record;

/*

  OP CODES

	=			:		Equals
	!			:		Not Equal

	+			:		Contains
	-			:		Does not contain


  */

#define RECORDBANK_NAME "Name"
#define RECORDBANK_PASSWORD "Password"
#define RECORDBANK_ACCNO "Account Number"
#define RECORDBANK_SECURITY "Security"
#define RECORDBANK_ADMIN "admin"
#define RECORDBANK_READWRITE "readwrite"
#define RECORDBANK_READONLY "readonly"

class RecordBank : public UplinkObject {

public:
	LList<Record*> records;

public:
	RecordBank();
	~RecordBank();

	// Data access functions

	void AddRecord(Record* newrecord);
	void AddRecordSorted(Record* newrecord, const char* sortfield = RECORDBANK_NAME);

	Record* GetRecord(int index); // Returns NULL if not found
	Record* GetRecord(const char* query); // Assumes there is only 1 match
	LList<Record*>* GetRecords(const char* query);
	Record* GetRandomRecord(const char* query);

	Record* GetRecordFromName(const char* name);
	Record* GetRecordFromNamePassword(const char* name, const char* password);
	Record* GetRecordFromAccountNumber(const char* accNo);

	int FindNextRecordIndexNameNotSystemAccount(int curindex = -1);

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();
	void Update();

	std::string GetID();
	int GetOBJECTID();

private:
	char* MakeSafeField(const char* fieldval);
};

// ============================================================================

/*
	Computer Record class
	Eg. A criminal record, a bank account, an uplink account etc

	  */

class Record : public UplinkObject {

public:
	BTree<char*> fields;

public:
	Record();
	virtual ~Record();

	void AddField(const char* name, const char* value);
	void AddField(const char* name, const int value);
	void AddField(const char* name, std::string newvalue) { AddField(name, newvalue.c_str()); }
	void AddField(std::string name, std::string newvalue) { AddField(name.c_str(), newvalue.c_str()); }

	void ChangeField(const char* name, const char* newvalue);
	void ChangeField(const char* name, std::string newvalue) { ChangeField(name, newvalue.c_str()); }
	void ChangeField(std::string name, std::string newvalue) { ChangeField(name.c_str(), newvalue.c_str()); }
	void ChangeField(const char* name, int newvalue);

	const char* GetField(const char* name);
	void DeleteField(const char* name);

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();
	void Update();

	std::string GetID();
	int GetOBJECTID();
};

#endif
