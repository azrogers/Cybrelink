
#ifndef WIN32

	#include <unistd.h>
	#include <winsock.h>

int WSAStartup(WORD versionRequested, WSADATA* w) { return 1; }

void WSACleanup() { }

int closesocket(int socket) { return close(socket); }

#endif
