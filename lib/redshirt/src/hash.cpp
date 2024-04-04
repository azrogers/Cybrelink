#include "sha1.hpp"
#include <istream>

struct membuf : std::streambuf
{
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
	}
};


void* HashInitial()
{
	SHA1* checksum = new SHA1;
	return checksum;

}

void HashData(void* context, unsigned char* data, unsigned int lendata)
{
	SHA1* checksum = (SHA1*)context;
	membuf sbuf(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data + lendata));
	checksum->update(std::istream(&sbuf));
}

unsigned int HashResultSize()
{
	return (5 * sizeof(uint32_t));
}

unsigned int HashFinal(void* context, unsigned char* result, unsigned int lenresult)
{
	SHA1* checksum = (SHA1*)context;
	uint32_t hash[5];

	checksum->final_digest(hash);
	delete checksum;

	unsigned int lencopy = std::min(lenresult, (unsigned int)sizeof(hash));

	memcpy(result, hash, lencopy);
	return lencopy;
}