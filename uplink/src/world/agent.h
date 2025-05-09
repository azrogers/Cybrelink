
/*

  Agent class object

	Part of the World sub-system

  */

#ifndef _included_agent_h
#define _included_agent_h

// ============================================================================

#include <stdio.h>

#include "world/computer/gateway.h"
#include "world/person.h"

class Message;
class Mission;

#define SIZE_AGENT_HANDLE 64

// ============================================================================

class Agent : public Person {

public:
	LList<char*> links; // The agents' "favourites" (IP)
	BTree<char*> codes; // All known access codes, indexed on ip
	LList<Mission*> missions; // The agents' missions

	char handle[SIZE_AGENT_HANDLE];

public:
	Agent();
	~Agent();

	void SetHandle(const char* newhandle);

	void GiveMission(Mission* mission);
	void CheckMissionDueDates(); // Have any missions expired?

	void GiveLink(const char* newip);
	bool HasLink(const char* newip);
	void RemoveLink(char* ip);

	void GiveCode(const char* newip, const char* newcode);
	void GiveMessage(Message* message);
	int HasAccount(char* ip); // Returns access level or -1

	int CreateNewAccount(const char* bankip,
						 const char* accname,
						 const char* password,
						 int balance,
						 int loan); // Returns account number

	bool HasMissionLink(const char* ip);

	void AttemptMission(); // Attempts mission 0
	void AttemptMission_StealFile();
	void AttemptMission_DeleteFile();
	void AttemptMission_ChangeAccount();
	void AttemptMission_TraceUser();
	void AttemptMission_RemoveComputer();

	void EstablishConnection(char* ip); // Complexity based on skill

	static bool ParseAccessCode(
		const char* code, char* username, size_t usernamesize, char* password, size_t passwordsize);

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();
	void Update();

	std::string GetID();
	int GetOBJECTID();
};

#endif
