#include "vfs.h"

#include <libzippp.h>
#include <spdlog/spdlog.h>

using namespace libzippp;

class RsVfsArchiveNodeContents : public RsBaseVfsNodeContents {
public:
	RsVfsArchiveNodeContents(ZipEntry& entry)
	{
		size_t size = entry.getSize();
		void* data = entry.readAsBinary();
		this->_buffer = std::span<std::byte const>(reinterpret_cast<std::byte*>(data), size);
	}

	virtual size_t size() const override {
		return this->_buffer.size();
	}

	virtual std::span<std::byte const> buffer() const override {
		return this->_buffer;
	}
private:
	std::span<std::byte const> _buffer;
};

RsVirtualFilesystem::RsVirtualFilesystem() {

}

bool RsVirtualFilesystem::AddArchive(std::string archiveFilename)
{
	ZipArchive archive(archiveFilename);
	if (!archive.open())
	{
		spdlog::error("Failed to open archive %s", archiveFilename);
		return false;
	}

	std::vector<ZipEntry> entries = archive.getEntries();
	for (auto it = entries.begin(); it != entries.end(); ++it)
	{
		ZipEntry& entry = *it;

		RsVfsNode node;
		node.isFile = entry.isFile();
		node.name = entry.getName();
		node.source = RsVfsSource::Archive;
		node.contents = std::make_unique<RsVfsArchiveNodeContents>(entry);
		
		this->_root.children[node.name] = std::move(node);
	}

	return true;
}
