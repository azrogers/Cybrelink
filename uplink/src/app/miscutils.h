

/*
	Misc string/file utils

  */


#ifndef _included_miscutils_h
#define _included_miscutils_h

#include "tosser.h"
#include <vector>
#include <string>

char* LowerCaseString(const char* thestring);
char* StripCarriageReturns(const char* thestring);               // Replaces first cr with \x0
char* TrimSpaces(const char* thestring);

char* GetFilePath(const char* filename);
void MakeDirectory(const char* directory);
void MakeDirectory(std::string directory);
bool DoesFileExist(const char* filename);
bool DoesFileExist(std::string filename);
void EmptyDirectory(const char* directory);
void EmptyDirectory(std::string directory);
bool CopyFilePlain(const char* oldfilename, const char* newfilename);
bool CopyFilePlain(std::string oldFilename, std::string newFilename);
bool CopyFileUplink(const char* oldfilename, const char* newfilename);
bool CopyFileUplink(std::string oldFilename, std::string newFilename);
bool RemoveFile(const char* filename);
bool RemoveFile(std::string filename);

DArray <char*>* ListDirectory(char* directory, char* filter);
std::vector<std::string> ListDirectoryVector(std::string directory, std::string filter);
std::vector<std::string> ListSubdirs(std::string directory);

void SetColour(char* colourName);                    // calls glColour3f

void PrintStackTrace();

/**
 * Creates a new string vector from the given DArray<char*>, freeing the array in the process.
 */
std::vector<std::string> ConsumeToStringVector(DArray<char*>* sourceArray);

#endif

