// NetworkServer.h: interface for the NetworkServer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_networkserver_h
#define _included_networkserver_h

// ============================================================================

#if ENABLE_NETWORK
#include <tcp4u.h>
#endif

#include <time.h>

#include "tosser.h"

#include "app/uplinkobject.h"

class ClientConnection;

// ============================================================================

class NetworkServer : public UplinkObject
{

protected:

#if ENABLE_NETWORK
	SOCKET listensocket;
#endif
	time_t lastlisten;

	bool listening;

public:

	DArray <ClientConnection*> clients;

public:

	NetworkServer();
	virtual ~NetworkServer();

	bool StartServer();
	void StopServer();

	void Listen();
	void StopListening();

	char* GetRemoteHost(int socketindex);
	char* GetRemoteIP(int socketindex);

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();
	void Update();
	char* GetID();

};

#endif 
