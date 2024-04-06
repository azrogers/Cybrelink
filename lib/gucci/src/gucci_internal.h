#if !defined __GUCCI_INTERNAL_H
	#define __GUCCI_INTERNAL_H

void GciFallbackDrawText(int x, int y, const char* text, int STYLE);
int GciFallbackTextWidth(const char* text, int STYLE);

	#include "vertex_buffer_pool.h"

void GciInitializePostGl();
void GciCleanup();
URect GciGetScreenRect();

	#ifdef USE_FREETYPEGL
void GciGarbageCollectTick();
	#endif

#endif
