
/*
	Base class for all Client Connections

	These exist on the Server and are used to send the 
	right data to the right client

  */


#ifndef _included_clientconnection_h
#define _included_clientconnection_h

#if ENABLE_NETWORK
#include <tcp4u.h>
#endif

#include "app/uplinkobject.h"

#include "world/rating.h"
#include "world/date.h"

#ifndef ENABLE_NETWORK
#define SOCKET uint32_t
#endif


class ClientConnection : public UplinkObject  
{

protected:

	int connectionsize;					// Specific to CLIENT_COMMS
        int traceprogress;					// Specific to CLIENT_COMMS
    int numlinks; // Specific to CLIENT_COMMS
    bool sentgateway; // Specific to CLIENT_COMMS
    
	Date lastnewsdate;					// Specific to CLIENT_STATUS
	Rating rating;						// Specific to CLIENT_STATUS
	int balance;						// Specific to CLIENT_STATUS
	int numaccounts;					// Specific to CLIENT_STATUS
	char convictions [256];				// Specific to CLIENT_STATUS
	char hudupgrades;					// Specific to CLIENT_STATUS
	int cpumodemmemory;					// Specific to CLIENT_STATUS
	int numhwitems;						// Specific to CLIENT_STATUS
	char ip [128];						// Specific to CLIENT_STATUS

public:

	SOCKET socket;

	int clienttype;
	int connectiontime;
	int previousupdate;				// Time of previous data burst to client

	int index;							// Into app->network->clients

protected:

	void Handle_ClientCommsInterface ();
	void Handle_ClientStatusInterface ();

public:

	ClientConnection();
	virtual ~ClientConnection();

	void SetSocket ( SOCKET socket );	
	void SetClientType ( int newclienttype );
	void SetConnectionTime ( int newtime );

	void SetIndex ( int newindex );

	int TimeActive ();					// Returns seconds active

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();
	char *GetID ();

};

#endif 
