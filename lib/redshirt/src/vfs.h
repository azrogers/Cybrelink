#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <span>
#include <memory>

enum class RsVfsSource { Filesystem, Archive };

class RsBaseVfsNodeContents
{
public:
	virtual size_t size() const = 0;
	virtual std::span<std::byte const> buffer() const = 0;
};

struct RsVfsNode
{
	std::string name;
	bool isFile = true;
	RsVfsSource source;
	std::unique_ptr<RsBaseVfsNodeContents> contents;

	std::map<std::string, RsVfsNode> children;
};

/**
 * A virtual filesystem containing files from both archives and disk.
 */
class RsVirtualFilesystem
{
public:
	RsVirtualFilesystem();

	bool AddArchive(std::string archiveFilename);
private:
	RsVfsNode _root;
};