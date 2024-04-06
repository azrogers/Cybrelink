
#ifndef WIN32

	#include "missing.h"
	#include <ctype.h>
	#include <string.h>

char* _strupr(char* p)
{
	for (char* s = p; *s; s++) {
		*s = toupper(*s);
	}
	return p;
}

#endif
