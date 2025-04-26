#include "vfs.h"

#include <iostream>
#include <memory>
#include <fstream>

#include <cppfs/cppfs.h>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <libzippp.h>
#include <spdlog/spdlog.h>

using namespace cppfs;
using namespace libzippp;

class RsVfsArchiveNodeContents : public RsBaseVfsNodeContents {
public:
	RsVfsArchiveNodeContents(ZipEntry& entry)
	{
		size_t size = entry.getSize();
		void* data = entry.readAsBinary();
		this->_buffer = std::span<const std::byte>(reinterpret_cast<std::byte*>(data), size);
	}

	virtual size_t size() const override { return this->_buffer.size(); }

	virtual std::span<const std::byte> buffer() const override { return this->_buffer; }

private:
	std::span<const std::byte> _buffer;
};

RsVirtualFilesystem::RsVirtualFilesystem() { }

bool RsVirtualFilesystem::AddArchive(std::string archiveFilename)
{
	std::ifstream stream(archiveFilename, std::ios::binary | std::ios::ate);

	std::streamsize size = stream.tellg();
	stream.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	stream.read(buffer.data(), size);

	ZipArchive* archive = ZipArchive::fromBuffer(buffer.data(), size);
	if (!archive->open()) {
		ZipArchive::free(archive);
		spdlog::error("Failed to open archive %s", archiveFilename);
		return false;
	}

	std::vector<ZipEntry> entries = archive->getEntries();
	for (auto it = entries.begin(); it != entries.end(); ++it) {
		ZipEntry& entry = *it;

		RsVfsNode node;
		node.isFile = entry.isFile();
		node.name = entry.getName();
		node.source = RsVfsSource::Archive;
		node.contents = std::make_unique<RsVfsArchiveNodeContents>(entry);

		this->_root.children[node.name] = std::move(node);
	}

	ZipArchive::free(archive);

	return true;
}
