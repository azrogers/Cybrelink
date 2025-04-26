// -*- tab-width:4 c-file-style:"cc-mode" -*-

#include <algorithm>
#include <cctype>
#include <string>

#include "gl.h"

#include "stdafx.h"

#include "redshirt.h"

#include "gucci.h"

#include <cppfs/FileHandle.h>
#include <cppfs/FileIterator.h>
#include <cppfs/FilePath.h>
#include <cppfs/fs.h>
#include <cpptrace/cpptrace.hpp>

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"

#include "options/options.h"

char* GetFilePath(const char* filename)
{

	UplinkAssert(filename);

	size_t newstringsize = strlen(filename) + 1;
	char* newstring = new char[newstringsize];
	UplinkStrncpy(newstring, filename, newstringsize);
	/*
		char *p = newstring;

		while ( strchr ( p, '/' ) )
			p = strchr ( p, '/' ) + 1;

		if (p) *p = '\x0';
	  */

	char* p = strrchr(newstring, '/');
	if (!p) {
		p = strrchr(newstring, '\\');
	}

	if (p) {
		*(p + 1) = '\x0';
	} else {
		UplinkStrncpy(newstring, "./", newstringsize);
	}

	return newstring;
}

char* LowerCaseString(const char* thestring)
{

	UplinkAssert(thestring);

	char* thecopy = new char[strlen(thestring) + 1];
	UplinkSafeStrcpy(thecopy, thestring);

	for (char* p = thecopy; *p != '\x0'; ++p) {
		if (*p >= 'A' && *p <= 'Z') {
			*p += 'a' - 'A';
		}
	}

	return thecopy;
}

std::string LowerCaseString(std::string thestring)
{
	std::string result = thestring;
	std::transform(
		result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
	return result;
}

char* StripCarriageReturns(const char* thestring)
{

	UplinkAssert(thestring);

	// Is there a cr?

	const char* firstcr = strchr(thestring, '\n');

	if (!firstcr) {

		// No cr found - copy string and return it

		char* result = new char[strlen(thestring) + 1];
		UplinkSafeStrcpy(result, thestring);
		return result;

	} else {

		// Found a cr - so shorten string to that point

		size_t newlength = firstcr - thestring;
		char* result = new char[newlength + 1];
		strncpy(result, thestring, newlength);
		result[newlength] = '\x0';
		return result;
	}
}

char* TrimSpaces(const char* thestring)
{

	UplinkAssert(thestring);

	char* retstring;

	int indexfirstnospace = -1;
	int indexlastnospace = -1;

	for (int i = 0; thestring[i] != '\0'; i++) {
		if (indexlastnospace == -1) {
			indexfirstnospace = i;
		}
		if (thestring[i] != ' ') {
			indexlastnospace = i;
		}
	}

	int lenretstring;
	if (indexfirstnospace == -1 || indexlastnospace == -1) {
		lenretstring = 0;
	} else {
		lenretstring = (indexlastnospace - indexfirstnospace) + 1;
	}

	retstring = new char[lenretstring + 1];
	if (lenretstring > 0) {
		memcpy(retstring, thestring + indexfirstnospace, lenretstring);
	}
	retstring[lenretstring] = '\0';

	return retstring;
}

void MakeDirectory(const char* dirname) { MakeDirectory(std::string(dirname)); }

void MakeDirectory(std::string directory)
{
	cppfs::FileHandle handle = cppfs::fs::open(directory);
	if (!handle.isDirectory()) {
		handle.createDirectory();
		handle.setPermissions(0700);
	}
}

bool DoesFileExist(const char* filename) { return DoesFileExist(std::string(filename)); }

bool DoesFileExist(std::string filename) { return cppfs::fs::open(filename).exists(); }

void EmptyDirectory(const char* directory) { EmptyDirectory(std::string(directory)); }

void EmptyDirectory(std::string directory)
{
	cppfs::FileHandle dir = cppfs::fs::open(directory);
	if (dir.isDirectory()) {
		for (cppfs::FileIterator it = dir.begin(); it != dir.end(); ++it) {
			RemoveFile(*it);
		}
	}
}

bool CopyFilePlain(const char* oldfilename, const char* newfilename)
{
	return CopyFilePlain(std::string(oldfilename), std::string(newfilename));
}

bool CopyFilePlain(std::string oldFilename, std::string newFilename)
{
	cppfs::FileHandle oldFile = cppfs::fs::open(oldFilename);
	if (!oldFile.isFile()) {
		return false;
	}

	cppfs::FileHandle newFile = cppfs::fs::open(newFilename);
	return oldFile.copy(newFile);
}

bool CopyFileUplink(const char* oldfilename, const char* newfilename)
{

	bool success = false;
	FILE* fileread = RsFileOpen(oldfilename, "rb");
	FILE* filewrite = fopen(newfilename, "wb");

	if (fileread && filewrite) {
		char buffer[256];
		size_t sizeread;

		while ((sizeread = fread(buffer, 1, sizeof(buffer), fileread)) > 0) {
			fwrite(buffer, 1, sizeread, filewrite);
		}

		success = true;
	}

	if (filewrite) {
		fclose(filewrite);
	}
	if (fileread) {
		RsFileClose(const_cast<char*>(oldfilename), fileread);
	}

	return success;
}

bool CopyFileUplink(std::string oldFilename, std::string newFilename)
{
	return CopyFileUplink(oldFilename.c_str(), newFilename.c_str());
}

bool RemoveFile(const char* filename) { return RemoveFile(std::string(filename)); }

bool RemoveFile(std::string filename)
{
	cppfs::FileHandle handle = cppfs::fs::open(filename);
	if (!handle.exists()) {
		return true;
	}

	return handle.remove();
}

DArray<char*>* ListDirectory(const char* directory, const char* filter)
{
	//
	// Start with a listing from Redshirt archives

	DArray<char*>* result = RsListArchive(directory, filter);

	std::string path(app->path);
	path.append(directory);

	cppfs::FileHandle handle = cppfs::fs::open(path);

	if (!handle.isDirectory()) {
		return result;
	}

	for (cppfs::FileIterator it = handle.begin(); it != handle.end(); ++it) {
		std::string filename = *it;
		if (filename.find(filter) != std::string::npos) {
			char* data = new char[filename.length() + 1];
			UplinkSafeStrcpy(data, filename.c_str());
			result->PutData(data);
		}
	}

	return result;
}

std::vector<std::string> ListDirectoryVector(std::string directory, std::string filter)
{
	return ConsumeToStringVector(
		ListDirectory(const_cast<char*>(directory.c_str()), const_cast<char*>(filter.c_str())));
}

std::vector<std::string> ListSubdirs(std::string directory)
{
	std::vector<std::string> result;

	cppfs::FileHandle dir = cppfs::fs::open(directory);
	if (!dir.isDirectory()) {
		return result;
	}

	for (cppfs::FileIterator it = dir.begin(); it != dir.end(); ++it) {
		std::string filename = *it;
		cppfs::FileHandle handle = cppfs::fs::open(filename);
		if (handle.isDirectory()) {
			result.push_back(filename);
		}
	}

	return result;
}

void SetColour(const char* colourName)
{

	if (!app || !app->GetOptions() || !app->GetOptions()->GetColour(colourName)) {

		printf("SetColour WARNING : Failed to find colour %s\n", colourName);
		glColor3f(0.0f, 0.0f, 0.0f);
		return;
	}

	ColourOption* col = app->GetOptions()->GetColour(colourName);
	UplinkAssert(col);
	glColor3f(col->r, col->g, col->b);
}

// calls glColour3f
void SetColour(std::string colourName) { SetColour(colourName.c_str()); }

void PrintStackTrace() { cpptrace::generate_trace().print(); }

std::vector<std::string> ConsumeToStringVector(DArray<char*>* sourceArray)
{
	std::vector<std::string> result(sourceArray->NumUsed());
	for (int i = 0; i < sourceArray->NumUsed(); i++) {
		result[i] = std::string(sourceArray->GetData(i));
		delete sourceArray->GetData(i);
	}

	delete sourceArray;
	return result;
}