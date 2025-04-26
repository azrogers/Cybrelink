#include "membuf.h"
#include "membuf.h"
#include "membuf.h"
#include "membuf.h"
#include "membuf.h"
#include "membuf.h"
#include "membuf.h"
#include "membuf.h"

MemoryBufferStreambuf::MemoryBufferStreambuf() { }

MemoryBufferStreambuf::MemoryBufferStreambuf(std::span<const std::byte>& bytes) :
	_contents(bytes.size())
{
	memcpy(_contents.data(), bytes.data(), bytes.size());
	setp(_contents.data(), _contents.data() + _contents.size());
	setg(_contents.data(), _contents.data(), _contents.data() + _contents.size());
}

std::streambuf::int_type MemoryBufferStreambuf::overflow(std::streambuf::int_type value)
{
	if (traits_type::eq_int_type(traits_type::eof(), value)) {
		return value;
	}

	_contents.push_back(value);
	setp(_contents.data(), _contents.data() + _contents.size());
	return value;
}

std::streamsize MemoryBufferStreambuf::xsputn(const std::streambuf::char_type* buf, std::streamsize length)
{
	const int size = _contents.size();
	_contents.resize(size + length);
	memcpy(_contents.data() + size, buf, length);
	return length;
}

std::span<const std::byte> MemoryBufferStreambuf::span() const
{
	return std::span<const std::byte>(reinterpret_cast<const std::byte*>(_contents.data()), _contents.size());
}

MemoryBufferStream::MemoryBufferStream() :
	_streambuf(new MemoryBufferStreambuf()),
	std::iostream(_streambuf)
{
}

MemoryBufferStream::MemoryBufferStream(std::span<const std::byte>& bytes) :
	_streambuf(new MemoryBufferStreambuf(bytes)),
	std::iostream(_streambuf)
{
}

MemoryBufferStream::~MemoryBufferStream() { delete _streambuf; }

std::span<const std::byte> MemoryBufferStream::span() const { return this->_streambuf->span(); }
