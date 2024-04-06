
/*

  RedShirt library

	Designed to encrypt and decrypt sensitive files,
	such as save games or data files

  */

#ifndef _included_redshirt_h
#define _included_redshirt_h

#include <stdio.h>

#include "tosser.h"
#include "vfs.h"

// #ifdef _DEBUG
// #include "slasher.h"
// #endif

// ============================================================
// Basic routines =============================================

void RsInitialise(const char* newapppath); // Creates a temp directory
void RsCleanUp(); // Deletes all created files

// ============================================================
// Encryption routines ========================================

bool RsFileExists(const char* filename);
bool RsFileEncrypted(const char* filename);
bool RsFileEncryptedNoVerify(const char* filename);

bool RsEncryptFile(const char* filename); // Overwrites origional with encrypted
bool RsDecryptFile(const char* filename); // Overwrites origional with decrypted

FILE* RsFileOpen(const char* filename, const char* mode); // preserves origional
void RsFileClose(const char* filename, FILE* file);

// ============================================================
// Archive file routines ======================================

bool RsLoadArchive(const char* filename);

FILE* RsArchiveFileOpen(const char* filename, const char* mode); // Looks for file apppath/filename
std::string RsArchiveFileOpen(std::string filename);
char* RsArchiveFileOpen(const char* filename); // Opens from filename first, then from zip file
bool RsArchiveFileLoaded(const char* filename);

void RsArchiveFileClose(const char* filename, FILE* file = NULL);

void RsCloseArchive(const char* filename); // Frees all memory (how nice)

DArray<char*>* RsListArchive(const char* path = NULL, const char* filter = NULL);

#endif
