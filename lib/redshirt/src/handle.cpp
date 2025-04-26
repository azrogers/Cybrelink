#include "handle.h"
#include "handle.h"

RsFileHandle::RsFileHandle(const std::string& _filename, std::fstream::openmode mode) :
	RsHandle(_filename)
{
	this->_stream = std::fstream(_filename, mode);
}

RsBufferHandle::RsBufferHandle(const std::string& _filename, std::span<const std::byte>& buffer) :
	RsHandle(filename),
	_stream(buffer)
{
}

RsBufferHandle::RsBufferHandle(const std::string& _filename, MemoryBufferStream& stream) : RsHandle(_filename) {
	 std::span<const std::byte> span = stream.span();
	_stream = MemoryBufferStream(span);
}
