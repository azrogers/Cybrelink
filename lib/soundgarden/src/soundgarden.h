
/*
		S O U N D G A R D E N

			Sound library
		 By Christopher Delay
		 and John Knottenbelt

  */

#ifndef _included_soundgarden_h
#define _included_soundgarden_h

#include <stdio.h>

// #ifdef _DEBUG
// #include "slasher.h"
// #endif

// ===========================================================================
// Core interface
// Must be implemented for each platform
//

void SgInitialise();
void SgShutdown();

void SgPlaySound(const char* filename,
				 const char* id = NULL,
				 bool synchronised = true); // id must be unique to each sample.
											// If NULL, filename is used.

void SgPlayMod(const char* fullfilename);
void SgSetModVolume(int newvolume); // 0 = silent, 100 = full
int SgGetModVolume();
bool SgModFinished();
void SgStopMod();

// ===========================================================================
// Platform independant extensions
// (soundgarden.cpp)
//

void SgPlaylist_Initialise();
void SgPlaylist_Shutdown();
void SgPlaylist_Create(const char* pname);
void SgPlaylist_AddSong(const char* pname, const char* songname);
void SgPlaylist_Play(const char* pname);
void SgPlaylist_Stop();
void SgPlaylist_NextSong();
void SgPlaylist_RandomSong();

void SgUpdate();

#endif
