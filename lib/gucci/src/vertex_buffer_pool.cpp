#if USE_FREETYPEGL
	#include "vertex_buffer_pool.h"
	#include "gucci_internal.h"
	#include <freetype-gl/text-buffer.h>
	#include <gl/GL.h>
	#include <gl/glew.h>
	#include <glm/glm.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtc/type_ptr.hpp>

typedef struct {
	float x, y, z;
	float s, t;
	float r, g, b, a;
} vertex_t;

const char* vertexShaderSource =
	#include "text_basic_vert.txt"
	;

/*const char* fragShaderSource = "uniform sampler2D texture;\n"
	"uniform vec4 tint;\n"
	"void main()\n"
	"{\n"
	"	float a = texture2D(texture, gl_TexCoord[0].xy).r; \n"
	"	gl_FragColor = vec4(tint.rgb, a * tint.a); \n"
	"}\n";*/

const char* fragShaderSource =
	#include "text_basic_frag.txt"
	;

VertexBufferPool::VertexBufferPool()
{
	vertexShaderId = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	fragShaderId = CompileShader(fragShaderSource, GL_FRAGMENT_SHADER);
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderId);
	glAttachShader(shaderProgram, fragShaderId);
	glLinkProgram(shaderProgram);

	textureParamId = glGetUniformLocation(shaderProgram, "texture");
	modelParamId = glGetUniformLocation(shaderProgram, "model");
	viewParamId = glGetUniformLocation(shaderProgram, "view");
	projectionParamId = glGetUniformLocation(shaderProgram, "projection");
	tintParamId = glGetUniformLocation(shaderProgram, "tint");
}

VertexBufferPool::~VertexBufferPool()
{
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragShaderId);
}

unsigned int VertexBufferPool::CompileShader(const char* source, unsigned int type)
{
	unsigned int shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, &source, NULL);
	glCompileShader(shaderId);

	int success;
	char infoLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		printf("shader compilation failed! %s", infoLog);
	}

	return shaderId;
}

VertexBufferId VertexBufferPool::GetTextBuffer(const char* text,
											   const VertexBufferTextRenderingOptions& options)
{
	XXH64_hash_t hash = XXH3_64bits(text, strlen(text));
	auto it = contentHashToBufferId.find(hash);
	if (it != contentHashToBufferId.end()) {
		// we already have a buffer for this!
		bufferContent[it->second].LastUsed = std::chrono::steady_clock::now();
		return it->second;
	} else {
		VertexBufferId bufferId;
		if (!unusedBufferPool.empty()) {
			// we can reuse an old buffer
			bufferId = *unusedBufferPool.begin();
			unusedBufferPool.erase(bufferId);
		} else {
			// we need to make a new buffer
			buffers.push_back(vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f"));
			bufferId = buffers.size() - 1;
		}

		AddTextToBuffer(bufferId, text, options);
		return bufferId;
	}
}

void VertexBufferPool::UpdateBufferText(VertexBufferId id,
										const char* newText,
										const VertexBufferTextRenderingOptions& options)
{
	VertexBufferContent& content = bufferContent.at(id);

	// it's the same, no need to update
	if (content.Text.compare(newText) == 0) {
		return;
	}

	// if this is the original text + some extra, we can update without rewriting
	const char* substr = strstr(newText, content.Text.c_str());
	// wasn't found or it was found somewhere other than the start (can't prepend)
	if (substr == nullptr || substr != newText) {
		vertex_buffer_clear(buffers.at(id));
		AddTextToBuffer(id, newText, options);
	} else // we're appending!
	{
		AddTextToBuffer(id, newText + content.Text.length(), options, &content);
	}
}

void VertexBufferPool::GarbageCollectTick()
{
	std::vector<VertexBufferId> buffersToErase;
	for (const std::pair<const VertexBufferId, VertexBufferContent>& pair : bufferContent) {
		long long secondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(
									   std::chrono::steady_clock::now() - pair.second.LastUsed)
									   .count();

		if (secondsElapsed > BUFFER_MAX_AGE) {
			buffersToErase.push_back(pair.first);
		}
	}

	// we don't actually delete buffers, we just mark them available to use again
	for (const VertexBufferId id : buffersToErase) {
		bufferContent.erase(id);
		XXH64_hash_t hash = bufferIdToContentHash.at(id);
		bufferIdToContentHash.erase(id);
		contentHashToBufferId.erase(hash);

		vertex_buffer_clear(buffers.at(id));
		unusedBufferPool.emplace(id);
	}
}

void VertexBufferPool::AddTextToBuffer(VertexBufferId bufferId,
									   const char* text,
									   const VertexBufferTextRenderingOptions& options,
									   const VertexBufferContent* existingContent)
{
	UPoint position(0, 0);

	if (existingContent != nullptr) {
		position = existingContent->EndPos;
	}

	vertex_buffer_t* buffer = buffers.at(bufferId);

	// make sure we have the glyphs we need
	texture_font_load_glyphs(options.Font, text);

	if (options.Font->atlas->id != 0) {
		texture_font_t* font = options.Font;
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RED,
					 font->atlas->width,
					 font->atlas->height,
					 0,
					 GL_RED,
					 GL_UNSIGNED_BYTE,
					 font->atlas->data);
	}

	for (int i = 0; i < strlen(text); i++) {
		// too far, new line
		if (options.MaxWidth > 0 && position.X >= options.MaxWidth) {
			position.X = 0;
			position.Y -= options.Font->height;
		}

		texture_glyph_t* glyph = texture_font_get_glyph(options.Font, text + i);
		if (glyph != nullptr) {
			float kerning = 0;
			if (i > 0) {
				kerning = texture_glyph_get_kerning(glyph, text + i - 1);
			}

			position.X += kerning;

			// create quad for glyph
			GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
			float x0 = position.X + glyph->offset_x;
			float y0 = position.Y + glyph->offset_y;
			float x1 = x0 + glyph->width;
			float y1 = y0 - glyph->height;
			// color is handled in the shader
			vertex_t vertices[] = { { x0, y0 - 0.5f, 0, glyph->s0, glyph->t0, 1.0f, 1.0f, 1.0f, 1.0f },
									{ x0, y1 - 0.5f, 0, glyph->s0, glyph->t1, 1.0f, 1.0f, 1.0f, 1.0f },
									{ x1, y1 - 0.5f, 0, glyph->s1, glyph->t1, 1.0f, 1.0f, 1.0f, 1.0f },
									{ x1, y0 - 0.5f, 0, glyph->s1, glyph->t0, 1.0f, 1.0f, 1.0f, 1.0f } };

			vertex_buffer_push_back(buffer, vertices, 4, indices, 6);

			position.X += glyph->advance_x;
		}
	}

	VertexBufferContent newContent;
	newContent.EndPos = position;
	newContent.Text = std::string(text);
	newContent.LastUsed = std::chrono::steady_clock::now();
	bufferContent.insert_or_assign(bufferId, newContent);

	// remove old hash if it exists
	auto oldContentHashIt = bufferIdToContentHash.find(bufferId);
	if (oldContentHashIt != bufferIdToContentHash.end()) {
		contentHashToBufferId.erase(oldContentHashIt->second);
	}
	XXH64_hash_t newHash = XXH3_64bits(text, strlen(text));
	bufferIdToContentHash.insert_or_assign(bufferId, newHash);
	contentHashToBufferId.insert_or_assign(newHash, bufferId);
}

void VertexBufferPool::RenderBuffer(VertexBufferId id,
									UPoint position,
									const VertexBufferTextRenderingOptions& options)
{
	URect screenRect = GciGetScreenRect();
	vertex_buffer_t* buffer = buffers.at(id);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	texture_font_t* font = options.Font;

	// ensure atlas texture is created and bound
	if (font->atlas->id == 0) {
		glGenTextures(1, &font->atlas->id);
		glBindTexture(GL_TEXTURE_2D, font->atlas->id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RED,
					 font->atlas->width,
					 font->atlas->height,
					 0,
					 GL_RED,
					 GL_UNSIGNED_BYTE,
					 font->atlas->data);
	} else {
		glBindTexture(GL_TEXTURE_2D, font->atlas->id);
	}

	glm::mat4 model, view, projection;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(position.X, screenRect.BottomRight.Y - position.Y, 0));
	view = glm::mat4(1.0f);
	projection = glm::ortho<float>(0, screenRect.BottomRight.X, 0, screenRect.BottomRight.Y, -1, 1);

	glUseProgram(shaderProgram);
	{
		glUniform1i(textureParamId, 0);
		glUniform2f(glGetUniformLocation(shaderProgram, "pixel"),
					1.0f / font->atlas->width,
					1.0f / font->atlas->height);
		glUniform4f(tintParamId, options.Color.R, options.Color.G, options.Color.B, options.Color.A);
		glUniformMatrix4fv(modelParamId, 1, 0, glm::value_ptr(model));
		glUniformMatrix4fv(viewParamId, 1, 0, glm::value_ptr(view));
		glUniformMatrix4fv(projectionParamId, 1, 0, glm::value_ptr(projection));
		vertex_buffer_render(buffer, GL_TRIANGLES);
	}

	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
}
#endif