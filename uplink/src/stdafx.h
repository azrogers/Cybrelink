
#ifndef __STDAFX_H
#define __STDAFX_H

#ifdef WIN32
	#define VC_EXTRALEAN
	#include <tchar.h>
	#include <windows.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef OLD_HEADERS
	#include <iostream>
	#include <strstream>
using namespace std;
#else
	#include <iostream.h>
	#include <strstream.h>
#endif

#ifdef _DEBUG
	#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
	#define DEBUG_PRINTF(...)
#endif

#endif /* __STDAFX_H */
