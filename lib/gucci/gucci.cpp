#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>

#include <gl/glew.h>
#include <GL/gl.h>

//#define USE_FTGL
//#define USE_GLTT

#include <map>

#include "gucci.h"
#include "gucci_internal.h"
#include "tosser.h"

#ifdef USE_FREETYPEGL
#include <freetype-gl/freetype-gl.h>
static std::map<int, texture_font_t*> fonts;
static std::map<int, texture_atlas_t*> atlases;

static VertexBufferPool *GlobalVertexBufferPool;
#endif

#ifdef USE_FTGL
#  ifdef WIN32
#include <FTFace.h>
#include <FTGLBitmapFont.h>
//#include <FTGLPixmapFont.h>
#  else
#include <ftgl/FTFace.h>
#include <ftgl/FTGLBitmapFont.h>
//#include <ftgl/FTGLPixmapFont.h>
#  endif

static std::map<int, FTGLBitmapFont*> fonts;
//static std::map<int, FTGLPixmapFont *> fonts;

static BTree<FTFace*> faces;
#endif // USE_FTGL

#ifdef USE_GLTT
#  ifdef WIN32
#include <FTFace.h>
#include <FTEngine.h>
#include <GLTTBitmapFont.h>
#  else
#include <gltt/FTFace.h>
#include <gltt/FTEngine.h>
#include <gltt/GLTTBitmapFont.h>
#  endif

static std::map<int, GLTTBitmapFont*> fonts;

static BTree<FTFace*> faces;
#endif // USE_GLTT

#include <math.h>

using namespace std;

#include "mmgr.h"

int  gci_defaultfont = HELVETICA_12;
bool gci_truetypeenabled = false;

void GciInitializePostGl()
{
#ifdef USE_FREETYPEGL
	GlobalVertexBufferPool = new VertexBufferPool();
#endif
}

void GciCleanup()
{
#ifdef USE_FREETYPEGL
	delete GlobalVertexBufferPool;
#endif
}

#ifdef USE_FREETYPEGL
void GciGarbageCollectTick()
{
	GlobalVertexBufferPool->GarbageCollectTick();
}
#endif

void GciSetDefaultFont(int STYLE)
{

	gci_defaultfont = STYLE;

}

int GciDrawText(int x, int y, char* text)
{
	return GciDrawText(x, y, text, gci_defaultfont);
}

int GciTextWidth(char* text)
{

	return GciTextWidth(text, gci_defaultfont);

}

void GciEnableTrueTypeSupport()
{

	gci_truetypeenabled = true;

}

void GciDisableTrueTypeSupport()
{

	gci_truetypeenabled = false;

}

bool GciRegisterTrueTypeFont(const char* filename)
{
	return true;
}

bool GciUnregisterTrueTypeFont(const char* filename)
{
	return true;
}

#ifndef USE_FREETYPEGL
static FTFace* RegisterFace(const char* familyname, const char* filename)
{
	if (faces.GetData(familyname))
		return faces.GetData(familyname);

	FTFace* face;
#ifdef USE_GLTT
	face = new FTFace();
	if (!face->open(filename)) {
		delete face;
		return NULL;
	}
#endif // USE_GLTT
#ifdef USE_FTGL
	face = new FTFace(filename);
	if (face->Error() != 0) {
		delete face;
		return NULL;
	}
#endif // USE_GLTT
	faces.PutData(familyname, face);
	return face;
}

static void UnregisterFace(const char* familyname)
{
	FTFace* face = faces.GetData(familyname);
	if (face) {
		faces.RemoveData(familyname);
		delete face;
	}
}
#endif // USE_FREETYPEGL


int GciDrawText(int x, int y, char* text, int STYLE, unsigned int bufferId)
{

	if (gci_truetypeenabled && fonts[STYLE]) {

		// Use true type fonts
#ifdef USE_GLTT        
		GLTTBitmapFont* font = fonts[STYLE];
		font->output(x, y, text);
#endif // USE_GLTT
#ifdef USE_FTGL
		FTGLBitmapFont* font = fonts[STYLE];
		//FTGLPixmapFont *font = fonts[STYLE];
		glRasterPos2i(x, y);
		font->Render(text);
#endif // USE_FTGL
#ifdef USE_FREETYPEGL
		float color[4];
		glGetFloatv(GL_CURRENT_COLOR, color);
		GucciTextDrawingOptions options;
		options.Color = UColor(color[0], color[1], color[2], color[3]);
		options.FontIndex = STYLE;
		return GciDrawText(UPoint(x, y), text, options, bufferId);
#endif
	}
	else {
		GciFallbackDrawText(x, y, text, STYLE);
	}
	return 0;
}

int GciDrawText(UPoint point, char* text, const GucciTextDrawingOptions& options, unsigned int bufferId)
{
#ifndef USE_FREETYPEGL
#pragma error "Not supported"
#else
	VertexBufferTextRenderingOptions vertexOptions;
	vertexOptions.Font = fonts[options.FontIndex];
	vertexOptions.Color = options.Color;
	vertexOptions.MaxWidth = options.MaxWidth;
	if (bufferId != -1)
	{
		GlobalVertexBufferPool->UpdateBufferText(bufferId, text, vertexOptions);
	}
	else
	{
		bufferId = GlobalVertexBufferPool->GetTextBuffer(text, vertexOptions);
	}

	GlobalVertexBufferPool->RenderBuffer(bufferId, point, vertexOptions);
	return bufferId;
#endif
	return 0;
}

int GciTextWidth(char* text, int STYLE)
{
	if (fonts[STYLE]) {
#ifdef USE_GLTT
		return fonts[STYLE]->getWidth(text);
#endif
#ifdef USE_FTGL
		float llx, lly, llz, urx, ury, urz;
		fonts[STYLE]->BBox(text, llx, lly, llz, urx, ury, urz);
		return (int)(fabs(llx - urx) + 0.5);
#endif
#ifdef USE_FREETYPEGL
		// ensure that all the font's glyphs are loaded
		int missedGlyphs = texture_font_load_glyphs(fonts[STYLE], text);
		if (missedGlyphs > 0)
		{
			// double the atlas size and try again
			texture_font_enlarge_atlas(fonts[STYLE], atlases[STYLE]->width * 2, atlases[STYLE]->height * 2);
			texture_font_load_glyphs(fonts[STYLE], text);
		}

		float totalWidth = 0;
		for (int i = 0; i < strlen(text); i++)
		{
			texture_glyph_t* glyph = texture_font_get_glyph(fonts[STYLE], text + i);
			if (i > 0)
			{
				float kerning = texture_glyph_get_kerning(glyph, text + (i - 1));
				totalWidth += kerning;
			}

			totalWidth += glyph->advance_x;
		}

		return totalWidth;
#endif
	}
	else
	{
		return GciFallbackTextWidth(text, STYLE);
	}
}

bool GciLoadTrueTypeFont(int index, char* fontname, char* filename, int size)
{

	if (gci_truetypeenabled) {
#ifndef USE_FREETYPEGL
		int pointSize = int(size * 72.0 / 96.0 + 0.5);
		FTFace* face = RegisterFace(fontname, filename);

		if (!face)
			return false;

#ifdef USE_GLTT

		GLTTBitmapFont* font = new GLTTBitmapFont(face);

		if (!font->create(pointSize)) {
			delete font;
			return false;
		}
#endif // USE_GLTT
#ifdef USE_FTGL

		FTGLBitmapFont* font = new FTGLBitmapFont(filename);
		//FTGLPixmapFont *font = new FTGLPixmapFont(filename);
		if (font->Error() != 0 || !font->FaceSize(pointSize, 96)) {
			delete font;
			return false;
		}
#endif // USE_FTGL

		GciDeleteTrueTypeFont(index);
		fonts[index] = font;

#else  
		int pointSize = size;
		texture_atlas_t* atlas = texture_atlas_new(512, 512, 1);
		texture_font_t* font = texture_font_new_from_file(atlas, pointSize, filename);

		GciDeleteTrueTypeFont(index);
		fonts[index] = font;
		atlases[index] = atlas;

#endif // USE_FREETYPEGL

		return true;
	}
	else {
		printf("GciLoadTrueTypeFont called, but truetypes are not enabled\n");
		return false;
	}
}

void GciDeleteTrueTypeFont(int index)
{
#ifdef USE_FTGL
	if (fonts[index])
		delete fonts[index];
#endif

#ifdef USE_GLTT
	// FTGL's destructor hangs, for some reason!
	delete fonts[index];
#endif

#ifdef USE_FREETYPEGL
	if (fonts[index])
	{
		texture_font_delete(fonts[index]);
		texture_atlas_delete(atlases[index]);
	}

	atlases[index] = NULL;
#endif

	fonts[index] = NULL;
}

void GciDeleteAllTrueTypeFonts()
{
	// Delete all the frickin' fonts
#ifdef USE_GLTT    
	for (map<int, GLTTBitmapFont*>::iterator x = fonts.begin(); x != fonts.end(); x++)
		GciDeleteTrueTypeFont(x->first);
#elif USE_FTGL
	for (map<int, FTGLBitmapFont*>::iterator x = fonts.begin(); x != fonts.end(); x++)
		//for (map<int, FTGLPixmapFont *>::iterator x = fonts.begin(); x != fonts.end(); x++)
		GciDeleteTrueTypeFont(x->first);
#elif USE_FREETYPEGL
	for (std::pair<int, texture_font_t*> pair : fonts)
	{
		GciDeleteTrueTypeFont(pair.first);
	}
	atlases.clear();
#endif

#ifndef USE_FREETYPEGL
	// Delete all the frickin' faces

	DArray <FTFace*>* thefaces = faces.ConvertToDArray();

	for (int id = 0; id < thefaces->Size(); ++id) {
		if (thefaces->ValidIndex(id)) {

			FTFace* theface = thefaces->GetData(id);
			delete theface;

		}
	}

	// Delete the structures

	delete thefaces;
	faces.Empty();
#endif
	fonts.clear();

#ifdef USE_GLTT
	// Added to kill the remaining memory leaks, not needed, could be removed if a updated library come and this function is not in.
	//FTEngine::destroyStaticEngine();
#endif

}
