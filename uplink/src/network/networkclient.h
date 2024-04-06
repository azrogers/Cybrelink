
/*

  Network client runs on a client computer
  Handles incoming data from the server
  Deals with the interface

  */

#ifndef _included_networkclient_h
#define _included_networkclient_h

#if ENABLE_NETWORK
	#include <tcp4u.h>
#endif

#include "app/uplinkobject.h"

class NetworkScreen;

// Types of client ============================================================

#define CLIENT_NONE 0
#define CLIENT_COMMS 1
#define CLIENT_STATUS 2

// ============================================================================

class NetworkClient : public UplinkObject {

protected:
#if ENABLE_NETWORK
	SOCKET socket;
#endif

	int clienttype;

	int currentscreencode;
	NetworkScreen* screen;

protected:
	void Handle_ClientCommsData(char* buffer);
	void Handle_ClientStatusData(char* buffer);

public:
	NetworkClient();
	virtual ~NetworkClient();

	bool StartClient(const char* ip);
	bool StopClient();

	void SetClientType(int newtype);

	int InScreen(); // Returns id code of current screen
	void RunScreen(int SCREENCODE);
	NetworkScreen* GetNetworkScreen(); // Asserts screen

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();
	void Update();
	std::string GetID();
};

#endif
