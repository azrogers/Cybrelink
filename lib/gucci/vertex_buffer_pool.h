#pragma once

#include <freetype-gl/freetype-gl.h>
#include <freetype-gl/vertex-buffer.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <xxhash.h>
#include <chrono>

#include "geom_types.h"

typedef unsigned int VertexBufferId;

// old buffers will be cleaned up after 5 minutes of not being touched
constexpr unsigned int BUFFER_MAX_AGE = 5 * 60;

class VertexBufferContent
{
public:
	std::string Text;
	UPoint EndPos;
	std::chrono::steady_clock::time_point LastUsed;

	VertexBufferContent() : Text(""), EndPos(0, 0) { }
};

class VertexBufferTextRenderingOptions
{
public:
	texture_font_t* Font;
	UColor Color = UColor(1.0f, 1.0f, 1.0f, 1.0f);
	int MaxWidth;
};

// class for buffering freetype-gl vertex buffer objects for reuse
class VertexBufferPool
{
public:
	VertexBufferPool();
	~VertexBufferPool();

	/// <summary>
	/// Get a text buffer with the given text. Uses an existing one or creates a new one if necessary.
	/// </summary>
	/// <param name="text">The text that should be in the buffer.</param>
	/// <param name="options">Rendering options</param>
	/// <returns>The id of the buffer to use</returns>
	VertexBufferId GetTextBuffer(const char* text, const VertexBufferTextRenderingOptions& options);
	/// <summary>
	/// Updates an existing buffer with new text.
	/// If the text can be updated by appending the previously created text, it will be, otherwise
	/// the buffer will be cleared and recreated.
	/// </summary>
	/// <param name="id">The buffer to use.</param>
	/// <param name="newText">The buffer's new text.</param>
	/// <param name="options">Rendering options</param>
	void UpdateBufferText(VertexBufferId id, const char* newText, const VertexBufferTextRenderingOptions& options);

	void RenderBuffer(VertexBufferId id, UPoint position, const VertexBufferTextRenderingOptions& options);

	/// <summary>
	/// Cleans up old unused buffers. Should be called regularly.
	/// </summary>
	void GarbageCollectTick();
private:
	unsigned int CompileShader(const char* shaderSource, unsigned int type);

	void AddTextToBuffer(
		VertexBufferId bufferId,
		const char* text,
		const VertexBufferTextRenderingOptions& options,
		const VertexBufferContent* existingContent = nullptr);

	std::map<XXH64_hash_t, VertexBufferId> contentHashToBufferId;
	std::map<VertexBufferId, XXH64_hash_t> bufferIdToContentHash;
	std::map<VertexBufferId, VertexBufferContent> bufferContent;
	std::vector<vertex_buffer_t*> buffers;
	std::set<VertexBufferId> unusedBufferPool;

	unsigned int vertexShaderId;
	unsigned int fragShaderId;
	unsigned int shaderProgram;

	unsigned int textureParamId;
	unsigned int modelParamId;
	unsigned int viewParamId;
	unsigned int projectionParamId;
	unsigned int tintParamId;
};