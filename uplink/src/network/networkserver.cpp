// NetworkServer.cpp: implementation of the NetworkServer class.
//
//////////////////////////////////////////////////////////////////////

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "network/clientconnection.h"
#include "network/networkclient.h"
#include "network/networkserver.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NetworkServer::NetworkServer()
{
#if ENABLE_NETWORK
	listensocket = -1;
#endif
	listening = false;
}

NetworkServer::~NetworkServer() { DeleteDArrayData((DArray<UplinkObject*>*)&clients); }

bool NetworkServer::StartServer()
{

#if ENABLE_NETWORK
	// Establish a listen socket

	unsigned short portnum = 31337;
	int result = TcpGetListenSocket(&listensocket, NULL, &portnum, 5);
	lastlisten = time(NULL);

	if (result != TCP4U_SUCCESS) {
		return false;
	}

	else {
		return true;
	}
#else
	return false;
#endif
}

void NetworkServer::StopServer()
{

#if ENABLE_NETWORK
	// Close all open sockets

	for (int i = 0; i < clients.Size(); ++i) {
		if (clients.ValidIndex(i)) {

			int result = TcpClose(&(clients.GetData(i)->socket));

			if (result != TCP4U_SUCCESS) {
				printf("NetworkServer::StopServer, failed to close connection %d\n", i);
			}
		}
	}

	DeleteDArrayData((DArray<UplinkObject*>*)&clients);
	clients.Empty();
#endif
}

char* NetworkServer::GetRemoteHost(int socketindex)
{
#if ENABLE_NETWORK
	UplinkAssert(clients.ValidIndex(socketindex));

	DWORD ip;
	char* host = new char[128];
	int result = TcpGetRemoteID(clients.GetData(socketindex)->socket, host, 128, &ip);

	if (result == TCP4U_SUCCESS) {
		return host;
	}

	else {
		printf("NetworkServer::GetRemoteHost, failed to get remote host\n");
		return NULL;
	}
#else
	return NULL;
#endif
}

char* NetworkServer::GetRemoteIP(int socketindex)
{
#if ENABLE_NETWORK
	UplinkAssert(clients.ValidIndex(socketindex));

	DWORD ip;
	char* sip = (char*)&ip;
	char* host = new char[128];
	int result = TcpGetRemoteID(clients.GetData(socketindex)->socket, host, 128, &ip);

	size_t fullipsize = 64;
	char* fullip = new char[fullipsize];
	UplinkSnprintf(fullip, fullipsize, "%u.%u.%u.%u", sip[0], sip[1], sip[2], sip[3]);

	delete[] host;

	if (result == TCP4U_SUCCESS) {
		return fullip;
	}

	else {
		printf("NetworkServer::GetRemoteIP, failed to get remote ip\n");
		return NULL;
	}
#else
	return NULL;
#endif
}

void NetworkServer::Listen() { listening = true; }

void NetworkServer::StopListening() { listening = false; }

bool NetworkServer::Load(FILE* file)
{
	// not needed
	return true;
}

void NetworkServer::Save(FILE* file)
{
	// not needed
}

void NetworkServer::Print()
{
#if ENABLE_NETWORK
	printf("NetworkServer : listensocket:%d, lastlisten:%d\n", listensocket, lastlisten);

	PrintDArray((DArray<UplinkObject*>*)&clients);
#endif
}

void NetworkServer::Update()
{

#if ENABLE_NEWORK
	// Check the listen socket for new connections every 5 seconds

	if (listening && listensocket != -1) {

		if (time(NULL) > lastlisten + 5) {

			SOCKET thesocket;
			int result = TcpAccept(&thesocket, listensocket, 1);

			if (result == TCP4U_SUCCESS) {

				// New connection requested - deal with it

				ClientConnection* cc = new ClientConnection();
				cc->SetSocket(thesocket);
				cc->SetClientType(CLIENT_NONE);
				cc->SetConnectionTime((int)EclGetAccurateTime());

				int index = clients.PutData(cc);
				cc->SetIndex(index);
			}

			lastlisten = time(NULL);
		}
	}

	// Read any input from the open sockets
	// And send out the neccisary data

	UpdateDArray((DArray<UplinkObject*>*)&clients);
#endif
}

std::string NetworkServer::GetID() { return "SERVER"; }
