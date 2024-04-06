
/*
	G U C C I
	Graphics library
	By Christopher Delay

  */

#ifndef _included_gucci_h
#define _included_gucci_h

#include "tosser.h"

#ifdef WIN32
	#include <Windows.h>
#endif

#include <string>
#include <string_view>

/*
#ifdef _DEBUG
#include "slasher.h"
#endif
*/

// Image library -----------------------------------

#include "geom_types.h"
#include "image.h"

// Screen resolution functions ---------------------

void GciStoreScreenSize();
bool GciSetScreenSize(int width,
					  int height,
					  int bpp = -1,
					  int refresh = -1); // Returns true (success) or false (failure)
void GciRestoreScreenSize();

void GciResizeGlut(int width, int height);
void GciRestoreGlut();

// Font functions ----------------------------------

#define BITMAP_13 1
#define BITMAP_15 2
#define TIMESROMAN_10 3
#define TIMESROMAN_24 4
#define HELVETICA_10 5
#define HELVETICA_12 6
#define HELVETICA_18 7

void GciSetDefaultFont(int STYLE);

class GucciTextDrawingOptions {
public:
	int FontIndex;
	UColor Color;
	int MaxWidth;

	GucciTextDrawingOptions() :
		FontIndex(-1),
		MaxWidth(0),
		Color(1.0f, 1.0f, 1.0f, 1.0f)
	{
	}
};

int GciDrawText(int x, int y, const char* text); // Uses default
int GciDrawText(int x, int y, std::string text); // Uses default
int GciDrawText(int x, int y, std::string_view& text);
int GciDrawText(int x,
				int y,
				const char* text,
				int STYLE,
				unsigned int bufferId = -1); // STYLE can be a ttf index if ttf is enabled
int GciDrawText(int x, int y, std::string_view& text, int STYLE, unsigned int bufferId = -1);
int GciDrawText(int x, int y, std::string text, int STYLE, unsigned int bufferId = -1);
int GciDrawText(UPoint point, char* text, const GucciTextDrawingOptions& options, unsigned int bufferId = -1);

int GciTextWidth(const char* text);
int GciTextWidth(std::string text);

int GciTextWidth(const char* text, int STYLE);
int GciTextWidth(std::string text, int STYLE);

bool GciRegisterTrueTypeFont(const char* filename);
bool GciUnregisterTrueTypeFont(const char* filename);

// Font functions for dealing with true types -------

void GciEnableTrueTypeSupport();
void GciDisableTrueTypeSupport();

bool GciLoadTrueTypeFont(int index, const char* fontname, const char* filename, int size);
void GciDeleteTrueTypeFont(int index);
void GciDeleteAllTrueTypeFonts();

/* Graphics flags */
#define GCI_DOUBLE (0x01 << 0)
#define GCI_RGB (0x01 << 1)
#define GCI_FULLSCREEN (0x01 << 2)
#define GCI_DEBUGSTART (0x01 << 3)
#define GCI_BORDERLESS (0x01 << 4)

char* GciInitGraphicsLibrary(int graphics_flags);
char* GciInitGraphics(const char* caption,
					  int graphics_flags,
					  int screenWidth,
					  int screenHeight,
					  int screenDepth,
					  int screenRefreshRate,
					  int argc,
					  char* argv[]);
void GciCleanupGraphics();

void GciMainLoop();

typedef void GciDisplayFuncT(void);
typedef void GciIdleFuncT(void);
typedef void GciKeyboardFuncT(unsigned char key, int x, int y);
typedef void GciMouseFuncT(int button, int state, int x, int y);
typedef void GciScrollwheelFuncT(int change);
typedef void GciMotionFuncT(int x, int y);
typedef void GciPassiveMotionFuncT(int x, int y);
typedef void GciReshapeFuncT(int newWidth, int oldWidth);
typedef void GciSpecialFuncT(int key, int x, int y);
typedef void GciCallbackT(int value);

bool GciLayerDamaged();
void GciSwapBuffers();
void GciPostRedisplay();

#define GCI_GLUT_FUNC(Function) void Gci##Function##Func(Gci##Function##FuncT*);

GCI_GLUT_FUNC(Display); /* void GciDisplayFunction( GciDisplayFunc * ); */
GCI_GLUT_FUNC(Mouse);
GCI_GLUT_FUNC(Motion);
GCI_GLUT_FUNC(PassiveMotion);
GCI_GLUT_FUNC(Keyboard);
GCI_GLUT_FUNC(Special);
GCI_GLUT_FUNC(Idle);
GCI_GLUT_FUNC(Reshape);

void GciTimerFunc(unsigned int millis, GciCallbackT* callback, int value);

/* Special Keys */

#define GCI_KEY_F1 1
#define GCI_KEY_F2 2
#define GCI_KEY_F3 3
#define GCI_KEY_F4 4
#define GCI_KEY_F5 5
#define GCI_KEY_F6 6
#define GCI_KEY_F7 7
#define GCI_KEY_F8 8
#define GCI_KEY_F9 9
#define GCI_KEY_F10 10
#define GCI_KEY_F11 11
#define GCI_KEY_F12 12

/* Mouse states */

#define GCI_UP 0
#define GCI_DOWN 1

/* Mouse buttons */

#define GCI_LEFT_BUTTON 1
#define GCI_RIGHT_BUTTON 2
#define GCI_MIDDLE_BUTTON 3
#define GCI_WHEELUP 4
#define GCI_WHEELDOWN 5

/* Define for unknown buttons */
#define GCI_UNKNOWN -1

struct GciScreenMode {
	unsigned w; // Width
	unsigned h; // Height
	unsigned bpp; // Bits per pixel
	unsigned refreshRate;
};

typedef DArray<GciScreenMode*> GciScreenModeList;

GciScreenModeList* GciListScreenModes();
void GciDeleteScreenModeArrayData(GciScreenModeList*);

GciScreenMode* GciGetClosestScreenMode(int width, int height);
GciScreenMode* GciGetClosestBorderlessMode();

void GciSaveScreenshot(const char* file);

bool GciAppVisible();

#endif
