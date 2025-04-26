#pragma once

#include <cstddef>
#include <streambuf>
#include <vector>
#include <span>
#include <iostream>

/**
 * Implementation of \ref std::streambuf for byte buffers.
 */
class MemoryBufferStreambuf : public std::streambuf {
public:
	MemoryBufferStreambuf();
	MemoryBufferStreambuf(std::span<const std::byte>& bytes);

	virtual std::streambuf::int_type overflow(std::streambuf::int_type value) override;
	virtual std::streamsize xsputn(const std::streambuf::char_type* buf, std::streamsize length) override;

	std::span<const std::byte> span() const;

private:
	std::vector<char> _contents;
};

class MemoryBufferStream : private MemoryBufferStreambuf, public std::iostream {
public:
	MemoryBufferStream();
	MemoryBufferStream(std::span<const std::byte>& bytes);

	~MemoryBufferStream();

	std::span<const std::byte> span() const;

private:
	MemoryBufferStreambuf* _streambuf;
};