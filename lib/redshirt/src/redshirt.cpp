#include <iostream>
#include <fstream>
#include <format>

#include <spdlog/spdlog.h>
#include <cppfs/cppfs.h>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FilePath.h>

#include "bungle.h"
#include "hash.h"
#include "redshirt.h"
#include "tosser.h"
#include "membuf.h"

const char RS_MARKER[] = "REDSHIRT\x0";
const char RS2_MARKER[] = "REDSHRT2\x0";
const int SIZE_MARKER = 9;

const int SIZE_RSFILENAME = 256;

static char tempfilename[SIZE_RSFILENAME] = ""; // Returned by RsArchiveFileOpen
static char rsapppath[SIZE_RSFILENAME] = "";
static char tempdir[SIZE_RSFILENAME] = "";
static bool rsInitialised = false;

#define BUFFER_SIZE 16384

static char HashBuffer[BUFFER_SIZE];
static char FilterBuffer[BUFFER_SIZE];

typedef void filterFunc(char*, unsigned);
typedef bool headerFunc(std::iostream& stream);

static std::fstream::openmode ModeStrToEnum(const std::string& str)
{
	if (str == "r") {
		return std::fstream::in;
	}
	if (str == "rb") {
		return std::fstream::in | std::fstream::binary;
	}
	if (str == "w") {
		return std::fstream::out;
	}
	if (str == "wb") {
		return std::fstream::out | std::fstream::binary;
	}

	throw std::exception("unknown file mode");
}

static void _RsRemove(const std::string& filename)
{
	cppfs::FileHandle file = cppfs::fs::open(filename);
	if (!file.exists() || !file.remove()) {
		spdlog::error("failed to remove %s", filename);
	}
}

static void _RsRename(const std::string& oldfilename, const std::string& newfilename)
{
	cppfs::FileHandle file = cppfs::fs::open(oldfilename);
	if (!file.exists() || !file.rename(newfilename)) { 
		spdlog::error("failed to rename %s to %s", oldfilename, newfilename);
	}
}

// filterStream:
// transfers all the data from input to output via filterFunc(buffer, length)
static bool filterStream(std::istream& input, std::ostream& output, filterFunc* filterFunc)
{
	do {
		std::streamsize bytesread = input.readsome(FilterBuffer, BUFFER_SIZE);
		if (bytesread <= 0) {
			break;
		}

		filterFunc(FilterBuffer, (unsigned)bytesread);

		if (!output.write(FilterBuffer, bytesread).good()) {
			return false;
		}

	} while (true);

	return true;
};

static unsigned int RsFileCheckSum(std::iostream& input, unsigned char* hashbuffer, unsigned int hashsize)
{
	void* context = HashInitial();

	std::streamsize bytesread;
	while ((bytesread = input.readsome(HashBuffer, BUFFER_SIZE)) > 0) {
		HashData(context, reinterpret_cast<unsigned char*>(HashBuffer), (unsigned int)bytesread);
	}

	return HashFinal(context, hashbuffer, hashsize);
}

bool writeRsEncryptedCheckSum(std::iostream& output)
{

	bool result = false;

	unsigned int hashsize = HashResultSize();
	unsigned char* hashbuffer = new unsigned char[hashsize];

	// calculate the checksum of the file minus the header
	output.seekg(SIZE_MARKER + hashsize);

	if (RsFileCheckSum(output, hashbuffer, hashsize) == hashsize) {
		// seek back to write the header
		output.seekp(SIZE_MARKER);
		result = output.write(reinterpret_cast<char*>(hashbuffer), hashsize).good();
	}

	delete[] hashbuffer;

	return result;
}

void encryptBuffer(char* buffer, unsigned length)
{
	// Decrypt each byte in the buffer.

	for (unsigned i = 0; i < length; i++) {
		buffer[i] += 128;
	}
}

void decryptBuffer(char* buffer, unsigned int length)
{
	// Decrypt each byte in the buffer.

	for (unsigned i = 0; i < length; i++) {
		buffer[i] -= 128;
	}
}

bool RsFileExists(const std::string& filename) { return std::fstream(filename).good(); }

bool readRsEncryptedHeader(std::iostream& input)
{

	bool result = false;

	char newmarker[SIZE_MARKER];

	if (input.read(newmarker, SIZE_MARKER).good()) {
		if (strncmp(newmarker, RS2_MARKER, SIZE_MARKER) == 0) {
			unsigned int hashsize = HashResultSize();
			char* hashbuffer = new char[hashsize];
			result = input.read(hashbuffer, hashsize).good();
			delete[] hashbuffer;
		} else if (strcmp(newmarker, RS_MARKER) == 0) {
			result = true;
		}
	}

	return result;
}

bool writeRsEncryptedHeader(std::iostream& output)
{
	bool result = false;
	if (output.write(RS2_MARKER, SIZE_MARKER).good()) {
		unsigned int hashsize = HashResultSize();
		char* hashbuffer = new char[hashsize];
		memset(hashbuffer, 0, hashsize);
		result = (output.write(hashbuffer, hashsize).good());
		delete[] hashbuffer;
	}

	return result;
}

bool noHeader(std::iostream&)
{
	// Useful for a no-operation
	return true;
}

bool RsFileEncryptedNoVerify(const std::string& filename)
{
	std::fstream file;
	file.open(filename, std::fstream::in | std::fstream::binary);
	if (!file.is_open()) {
		spdlog::error("failed to open file %s", filename);
		return false;
	}

	bool result = readRsEncryptedHeader(file);

	return result;
}

bool RsFileEncrypted(const std::string& filename)
{
	std::fstream file;
	file.open(filename, std::fstream::in | std::fstream::binary);
	if (!file.is_open()) {
		spdlog::error("failed to open file %s", filename);
		return false;
	}

	bool result = false;

	char newmarker[SIZE_MARKER];
	if (file.read(newmarker, SIZE_MARKER).good()) {
		if (strncmp(newmarker, RS2_MARKER, SIZE_MARKER) == 0) {
			unsigned int hashsize = HashResultSize();
			char* hashbuffer = new char[hashsize];

			if (file.read(hashbuffer, hashsize).good()) {
				unsigned char* hashbuffer2 = new unsigned char[hashsize];
				unsigned int retsize = RsFileCheckSum(file, hashbuffer2, hashsize);
				if (retsize > 0 && memcmp(hashbuffer, hashbuffer2, retsize) == 0) {
					result = true;
				}

				delete[] hashbuffer2;
			}

			delete[] hashbuffer;
		} else if (strcmp(newmarker, RS_MARKER) == 0) {
			result = true;
		}
	}

	return result;
}

// filterFile:
//   takes input file (infile) and produces outfile
//
//   readHeader is a function which attempts to read the header
//   and returns true on success.
//
//   writeHeader is a function which attemps to write the header
//   and returns true on success.
//
//   filterFunc is a function which is used to transform the
//   bytes in the file.
//
//   returns true on success. Deletes outfile on failure.

bool filterFile(const std::string& infile,
				std::iostream& outfile,
				headerFunc* readHeader,
				headerFunc* writeHeader,
				headerFunc* writeChecksum,
				filterFunc* filter)
{
	std::fstream file;

	file.open(infile, std::fstream::in | std::fstream::binary);
	if (!file.is_open()) {
		spdlog::error("Failed to open %s in filterFile", infile);
		return false;
	}

	// Read header from input file
	if (!readHeader(file)) {
		spdlog::error("failed to read header from %s!", infile);
		return false;
	}

	// Set completely unbuffered (we do our own)
	// setvbuf(input, NULL, _IONBF, 0);
	// setvbuf(output, NULL, _IONBF, 0);

	// Write header into output file
	if (!writeHeader(outfile)) {
		spdlog::error("failed to write header!");
		return false;
	}

	if (!filterStream(file, outfile, filter)) {
		spdlog::error("failed to write containing bytes!");
		return false;
	}

	// Write checksum into output file
	if (!writeChecksum(outfile)) {
		spdlog::error("failed to write checksum!");
		return false;
	}

	return true;
}

// filterFileInPlace
// process a file in place, temporary file = filename++ext
// see filterFile for description of readHeader, writeHeader and filterFunc

bool filterFileInPlace(const std::string& filename,
					   const char* ext,
					   headerFunc* readHeader,
					   headerFunc* writeHeader,
					   headerFunc* writeChecksum,
					   filterFunc* filterFunc)
{
	std::string tempfilename = std::format("%s%s", filename, ext);

	std::fstream outfile;
	outfile.open(tempfilename, std::fstream::out | std::fstream::binary);

	if (filterFile(filename, outfile, readHeader, writeHeader, writeChecksum, filterFunc)) {
		_RsRemove(filename);
		_RsRename(tempfilename, filename);
		return true;
	} else {
		spdlog::error("Failed to write output file %s", tempfilename);
		return false;
	}
}

// RsEncryptFile: encrypts a file in-place
bool RsEncryptFile(const std::string& filename)
{
	return filterFileInPlace(
		filename, ".e", noHeader, writeRsEncryptedHeader, writeRsEncryptedCheckSum, encryptBuffer);
}

// RsDecryptFile: decrypt a file in-place
bool RsDecryptFile(const std::string& filename)
{
	if (!RsFileEncrypted(filename)) {
		// Not encrypted, so nothing to do
		return true;
	}

	return filterFileInPlace(filename, ".d", readRsEncryptedHeader, noHeader, noHeader, decryptBuffer);
};

std::string RsBasename(const std::string& filename)
{
	return cppfs::FilePath(filename).baseName();
}

std::unique_ptr<RsFileHandle> RsFileOpen(const std::string& filename, const std::string& modeStr)
{
	if (!RsFileExists(filename)) {
		return nullptr;
	}

	const std::fstream::openmode mode = ModeStrToEnum(modeStr);

	if (!RsFileEncrypted(filename)) {
		return std::make_unique<RsFileHandle>(filename, mode);
	} else {
		MemoryBufferStream outstream;

		if (filterFile(filename, outstream, readRsEncryptedHeader, noHeader, noHeader, decryptBuffer)) {
			return std::make_unique<RsBufferHandle>(filename, outstream);
		} else {
			printf("Redshirt ERROR : Failed to write to output file\n");
			return NULL;
		}
	}
}

void RsFileClose(const std::string& filename, std::fstream& file)
{

	fclose(file);

	// Delete any decrypted versions

	char dfilename[SIZE_RSFILENAME];
	sprintf(dfilename, "%s.d", filename);

	int result = remove(dfilename);
}

bool RsLoadArchive(const std::string& filename)
{

	char fullfilename[SIZE_RSFILENAME];
	sprintf(fullfilename, "%s%s", rsapppath, filename);

	FILE* archive = RsFileOpen(fullfilename, "rb");

	if (!archive) {
		int len = (int)strlen(rsapppath);
		if (len >= 5) {
			char c1 = rsapppath[len - 5];
			char c2 = rsapppath[len - 4];
			char c3 = rsapppath[len - 3];
			char c4 = rsapppath[len - 2];
			char c5 = rsapppath[len - 1];

			if ((c1 == '\\' || c1 == '/') && (c2 == 'l' || c2 == 'L') && (c3 == 'i' || c3 == 'I')
				&& (c4 == 'b' || c4 == 'B') && (c5 == '\\' || c5 == '/')) {

				fullfilename[len - 4] = '\0';
				strcat(fullfilename, filename);
				archive = RsFileOpen(fullfilename, "rb");
			}
		}

		if (!archive) {
			return false;
		}
	}

	bool result =
		BglOpenZipFile(archive, rsapppath, const_cast<char*>(filename)); // use the short filename as the id

	RsFileClose(filename, archive);

	if (result) {
		printf("Successfully loaded data archive %s\n", filename);
	} else {
		printf("Failed to load data archive %s\n", filename);
	}

	return result;
}

FILE* RsArchiveFileOpen(const std::string& filename, const char* mode)
{

	FILE* file = NULL;
	char* fname = RsArchiveFileOpen(filename);

	if (fname) {
		// printf( "Opening file %s\n", fname );
		file = fopen(fname, mode);
	}

	return file;
}

std::string RsArchiveFileOpen(std::string filename)
{
	return std::string(RsArchiveFileOpen(filename.c_str()));
}

char* RsArchiveFileOpen(const std::string& filename)
{

	//
	// WARNING
	// This function returns tempfilename - a static string
	// belonging to this library, which changes at every call
	//

	char fullfilename[SIZE_RSFILENAME];
	sprintf(fullfilename, "%s%s", rsapppath, filename);

	//
	// Look to see if the file exists
	// If it does, just return its filename for use
	//

	// printf( "Trying file %s: ", fullfilename );

	if (RsFileExists(fullfilename)) {

		// printf( "Found.\n" );
		strcpy(tempfilename, fullfilename);
		return tempfilename;
	}

	//
	// Now look in our data files for the file
	// Extract it to a temporary file in the same direcory if we find it
	//

	if (BglFileLoaded(fullfilename)) {

		char* extension = strrchr(fullfilename, '.');
		assert(extension);

		int attempt = 0;
		bool success = false;
		char targetfilename[SIZE_RSFILENAME];

		while (!success && attempt < 3) {
			sprintf(targetfilename, "%stemp%d%s", tempdir, attempt, extension);
			success = BglExtractFile(fullfilename, targetfilename);
			++attempt;
		}

		if (success) {
			strcpy(tempfilename, targetfilename);
			// printf( "Found as %s.\n", targetfilename );
			return tempfilename;
		}

		// printf( "Loaded, but no success.\n" );

	} else {
		// printf( "Not loaded.\n" );
	}

	//
	// Ooops - the file is nowhere to be found
	//

	printf("REDSHIRT : Failed to load file : %s\n", fullfilename);
	return NULL;
}

bool RsArchiveFileLoaded(const std::string& filename)
{

	char fullfilename[SIZE_RSFILENAME];
	sprintf(fullfilename, "%s%s", rsapppath, filename);

	if (RsFileExists(fullfilename)) {
		return true;
	}
	if (BglFileLoaded(fullfilename)) {
		return true;
	}

	return false;
}

void RsArchiveFileClose(const std::string& filename, FILE* file)
{

	if (file) {
		fclose(file);
	}

	const char* extension = strrchr(filename, '.');
	assert(extension);

	char targetfilename[SIZE_RSFILENAME];
	int attempt = 0;

	while (attempt < 3) {
		sprintf(targetfilename, "%stemp%d%s", tempdir, attempt, extension);
		int result = remove(targetfilename);
		++attempt;
	}
}

void RsCloseArchive(const std::string& filename) { BglCloseZipFile(const_cast<char*>(filename)); }

bool RsMakeDirectory(const char* dirname)
{
#ifdef WIN32
	return _mkdir(dirname) == 0;
#else
	return mkdir(dirname, 0700) == 0;
#endif
}

void RsDeleteDirectory(const char* dirname)
{

#ifdef WIN32
	_rmdir(dirname);
#else
	rmdir(dirname);
#endif
}

void RsInitialise(const char* newapppath)
{
	sprintf(rsapppath, "%s", newapppath);

	// create a temp directory

#ifdef WIN32
	sprintf(tempdir, "%stemp/", rsapppath);
	RsMakeDirectory(tempdir);
#else
	// Try in the current directory

	sprintf(tempdir, "%stemp/", rsapppath);
	if (!RsMakeDirectory(tempdir)) {
		// Try in /tmp

		strcpy(tempdir, "/tmp/uplink-XXXXXX");

		if (mkdtemp(tempdir) == NULL) {
			printf("Failed to make temporary directory\n");
			abort();
		}

		strcat(tempdir, "/");
	}
#endif

	rsInitialised = true;

	// clean up, even on aborts
	atexit(RsCleanUp);
}

void RsCleanUp()
{
	if (!rsInitialised) {
		return;
	}

	rsInitialised = false;

	// Delete all files in the temp directory

#ifdef WIN32

	char searchstring[SIZE_RSFILENAME];
	sprintf(searchstring, "%s*.*", tempdir);

	_finddata_t thisfile;
	intptr_t fileindex = _findfirst(searchstring, &thisfile);

	int exitmeplease = 0;

	while (fileindex != -1 && !exitmeplease) {

		if (thisfile.attrib & _A_NORMAL || thisfile.attrib & _A_ARCH) {

			char fullfilename[SIZE_RSFILENAME];
			sprintf(fullfilename, "%s%s", tempdir, thisfile.name);
			int result = remove(fullfilename);
		}

		exitmeplease = _findnext(fileindex, &thisfile);
	}

	if (fileindex != -1) {
		_findclose(fileindex);
	}

#else
	{
		DIR* dir = opendir(tempdir);
		if (dir != NULL) {
			struct dirent* entry = readdir(dir);

			while (entry != NULL) {

				char fullfilename[SIZE_RSFILENAME];
				sprintf(fullfilename, "%s%s", tempdir, entry->d_name);

				remove(fullfilename);

				entry = readdir(dir);
			}

			closedir(dir);
		}
	}
#endif

	// Delete the temp directory

	RsDeleteDirectory(tempdir);

	BglCloseAllFiles();
}

DArray<char*>* RsListArchive(const char* path, const char* filter)
{

	DArray<char*>* result = BglListFiles(rsapppath, const_cast<char*>(path), const_cast<char*>(filter));

	// Strip rsapppath from every result

	for (int i = 0; i < result->Size(); ++i) {
		if (result->ValidIndex(i)) {

			char* thisResult = result->GetData(i);
			char* newResult = new char[strlen(thisResult) - strlen(rsapppath) + 1];
			strcpy(newResult, thisResult + strlen(rsapppath));
			result->PutData(newResult, i);
		}
	}

	return result;
}
