#pragma once

#include <string>
#include <span>
#include <fstream>

#include "membuf.h"

struct RsHandle {
	std::string filename;

	RsHandle(const std::string& _filename) :
		filename(_filename)
	{
	}
};

struct RsFileHandle : public RsHandle {
	RsFileHandle(const std::string& _filename, std::fstream::openmode mode);

private:
	std::fstream _stream;
};

struct RsBufferHandle : public RsHandle {
	RsBufferHandle(const std::string& _filename, std::span<const std::byte>& buffer);
	RsBufferHandle(const std::string& _filename, MemoryBufferStream& _stream);

private:
	MemoryBufferStream _stream;
};