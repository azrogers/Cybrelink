#if !defined __DOS2UNIX_H

	#define __DOS2UNIX_H

	#include <fstream>
	#include <iostream>

using namespace std;

class dos2unixbuf : public streambuf {
public:
	typedef ios::openmode openmode;

	dos2unixbuf(const char* filename, openmode mode);
	virtual ~dos2unixbuf();

	void close();

protected:
	virtual int overflow(int c = EOF);
	virtual int underflow();
	virtual int uflow();
	virtual int pbackfail(int c = EOF);
	virtual int sync();

private:
	filebuf inner;
	char* buffer;
};

class idos2unixstream : public istream {
public:
	idos2unixstream(const char* filename) :
		istream(new dos2unixbuf(filename, ios::in))
	{
	}

	virtual ~idos2unixstream();

	void close();
};

#endif // __DOS2UNIX_H
