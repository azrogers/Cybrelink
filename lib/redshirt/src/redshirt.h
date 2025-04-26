
/*

  RedShirt library

	Designed to encrypt and decrypt sensitive files,
	such as save games or data files

  */

#ifndef _included_redshirt_h
#define _included_redshirt_h

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memory>

#include "tosser.h"
#include "handle.h"
#include "vfs.h"

// #ifdef _DEBUG
// #include "slasher.h"
// #endif

// ============================================================
// Basic routines =============================================

void RsInitialise(const std::string& newapppath); // Creates a temp directory
void RsCleanUp(); // Deletes all created files

// ============================================================
// Encryption routines ========================================

bool RsFileExists(const std::string& filename);
bool RsFileEncrypted(const std::string& filename);
bool RsFileEncryptedNoVerify(const std::string& filename);

bool RsEncryptFile(const std::string& filename); // Overwrites origional with encrypted
bool RsDecryptFile(const std::string& filename); // Overwrites origional with decrypted

std::unique_ptr<RsHandle> RsFileOpen(const std::string& filename,
										 const std::string& mode); // preserves origional
void RsFileClose(std::unique_ptr<RsHandle>& file);

// ============================================================
// Archive file routines ======================================

bool RsLoadArchive(const std::string& filename);

std::unique_ptr<RsHandle> RsArchiveFileOpen(const std::string& filename,
												const std::string& mode); // Looks for file apppath/filename
std::string RsArchiveFileOpen(const std::string& filename);
bool RsArchiveFileLoaded(const std::string& filename);

void RsArchiveFileClose(const std::string& filename);
void RsArchiveFileClose(std::unique_ptr<RsHandle>& file);

void RsCloseArchive(const std::string& filename); // Frees all memory (how nice)

DArray<char*>* RsListArchive(const std::string& path = NULL, const std::string& filter = "");

#endif
