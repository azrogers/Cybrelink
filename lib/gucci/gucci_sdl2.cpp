#ifdef USE_SDL2

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <list>
#include <chrono>
#include <iostream>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gucci.h"
#include "gucci_internal.h"

using namespace std;

#include "mmgr.h"

#define GciAbs(num) (((num)>0)?(num):-(num))


static SDL_Surface* GlobalSurface = NULL;
static SDL_Window* GlobalWindow = NULL;
static SDL_GLContext GlobalContext;
static int GraphicsFlags = 0;

static unsigned _GciGetAccurateTime();

static bool _GciIsInitGraphicsLibrary = false;

static int sdlKeyToGucci(int key)
{
	switch (key)
	{
	case SDLK_F1: return GCI_KEY_F1;
	case SDLK_F2: return GCI_KEY_F2;
	case SDLK_F3: return GCI_KEY_F3;
	case SDLK_F4: return GCI_KEY_F4;
	case SDLK_F5: return GCI_KEY_F5;
	case SDLK_F6: return GCI_KEY_F6;
	case SDLK_F7: return GCI_KEY_F7;
	case SDLK_F8: return GCI_KEY_F8;
	case SDLK_F9: return GCI_KEY_F9;
	case SDLK_F10: return GCI_KEY_F10;
	case SDLK_F11: return GCI_KEY_F11;
	case SDLK_F12: return GCI_KEY_F12;
	default: return 1000 + key;
	}
}

static int sdlButtonToGucci(int button)
{
	switch (button)
	{
	case SDL_BUTTON_LEFT: return GCI_LEFT_BUTTON;
	case SDL_BUTTON_MIDDLE: return GCI_MIDDLE_BUTTON;
	case SDL_BUTTON_RIGHT: return GCI_RIGHT_BUTTON;
	default: return GCI_UNKNOWN;
	}
}

char* GciInitGraphicsLibrary(int graphics_flags)
{
	bool debugging = (graphics_flags & GCI_DEBUGSTART) != 0;

	if (debugging) printf("Initialising SDL...");
	//if ((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==-1)) { 
#ifdef _DEBUG
	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) == -1))
	{
#else
	if ((SDL_Init(SDL_INIT_VIDEO) == -1))
	{
#endif
		//printf("Could not initialize SDL: %s.\n", SDL_GetError());
		//exit(-1);
		char message[] = "Could not initialize SDL: %s.";
		char* errorMessage = NULL;
		const char* sdlErrorMessage = SDL_GetError();
		size_t sizeErrorMessage = sizeof(message) + strlen(sdlErrorMessage) + 1;
		errorMessage = new char[sizeErrorMessage];
#ifdef WIN32
		_snprintf(errorMessage, sizeErrorMessage, message, sdlErrorMessage);
#else
		snprintf(errorMessage, sizeErrorMessage, message, sdlErrorMessage);
#endif
		errorMessage[sizeErrorMessage - 1] = '\0';
		return errorMessage;
	}
	if (debugging) printf("done\n ");

	_GciIsInitGraphicsLibrary = true;
	return NULL;
}

char* GciInitGraphics(const char* caption, int graphics_flags, int screenWidth, int screenHeight,
	int screenDepth, int screenRefresh, int argc, char* argv[])
{
	bool debugging = (graphics_flags & GCI_DEBUGSTART) != 0;
	bool runFullScreen = (graphics_flags & GCI_FULLSCREEN) != 0;

	int sdlFlags = 0;
	if (graphics_flags & GCI_DOUBLE)
	{
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		//sdlFlags |= SDL_DOUBLEBUF;
	}
	
	if (graphics_flags & GCI_BORDERLESS)
	{
		sdlFlags |= SDL_WINDOW_BORDERLESS;
	}
	else if (graphics_flags & GCI_FULLSCREEN)
	{
		sdlFlags |= SDL_WINDOW_FULLSCREEN;
	}

	SDL_DisplayMode target, closest;
	target.w = (graphics_flags & GCI_FULLSCREEN ? screenWidth : 0);
	target.h = (graphics_flags & GCI_FULLSCREEN ? screenHeight : 0);
	target.refresh_rate = screenRefresh > 0 ? screenRefresh : 0;

	if (screenDepth == 16)
	{
		target.format = SDL_PIXELFORMAT_RGB565;
	}
	else if (screenDepth == 24)
	{
		target.format = SDL_PIXELFORMAT_RGB24;
	}
	else if (screenDepth == 32)
	{
		target.format = SDL_PIXELFORMAT_ARGB32;
	}
	else if (screenDepth == -1)
	{
		target.format = 0;
	}
	else
	{
		printf("unsupported number of bits!");
		return nullptr;
	}

	if (SDL_GetClosestDisplayMode(0, &target, &closest) == nullptr)
	{
		printf("SDL_GetClosestDisplayMode returned nullptr!");
		return nullptr;
	}

	int closestBpp = SDL_BITSPERPIXEL(closest.format);
	switch (closestBpp)
	{
	case 16:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		break;
	case 24:
	case 32:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		break;
	default:
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, closestBpp / 3);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, closestBpp - (closestBpp / 3) * 2);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, closestBpp / 3);
		break;
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	GlobalWindow = SDL_CreateWindow(
		caption,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		(graphics_flags & GCI_FULLSCREEN ? closest.w : screenWidth),
		(graphics_flags & GCI_FULLSCREEN ? closest.h : screenHeight),
		sdlFlags | SDL_WINDOW_OPENGL
	);

	SDL_SetWindowDisplayMode(GlobalWindow, &closest);

	GlobalContext = SDL_GL_CreateContext(GlobalWindow);

	GraphicsFlags = graphics_flags;

	if (debugging) printf("SDL is now opening a %dx%d window in %d depth ...", screenWidth, screenHeight, screenDepth);
	GlobalSurface = SDL_GetWindowSurface(GlobalWindow);
	if (GlobalSurface == NULL)
	{
		//printf("Could not initialize SDL Video: %s.\n", SDL_GetError());
		//exit(-1);
		char message[] = "Could not initialize SDL Video: %s.";
		char* errorMessage = NULL;
		const char* sdlErrorMessage = SDL_GetError();
		size_t sizeErrorMessage = sizeof(message) + strlen(sdlErrorMessage) + 1;
		errorMessage = new char[sizeErrorMessage];
#ifdef WIN32
		_snprintf(errorMessage, sizeErrorMessage, message, sdlErrorMessage);
#else
		snprintf(errorMessage, sizeErrorMessage, message, sdlErrorMessage);
#endif
		errorMessage[sizeErrorMessage - 1] = '\0';
		return errorMessage;
	}
	if (debugging) printf("done\n ");

	return NULL;
}

void GciCleanupGraphics()
{
	if (GlobalWindow != nullptr)
	{
		SDL_GL_DeleteContext(GlobalContext);
	}
}

extern int gci_defaultfont; // The default Gucci style

void GciFallbackDrawText(int x, int y, char* text, int STYLE)
{
	static bool inside = false;

	if (!inside)
	{
		inside = true;
		GciDrawText(x, y, text, gci_defaultfont);
		inside = false;
	}
	else
	{
		// We recursed back here, not even the
		// default font is supported!
		abort();
	}
}

int GciFallbackTextWidth(char* text, int STYLE)
{
	static bool inside = false;

	if (!inside)
	{
		inside = true;
		int result = GciTextWidth(text, gci_defaultfont);
		inside = false;
		return inside;
	}
	else
	{
		// We recursed back here, not even the 
		// default font is supported!
		abort();
		return 0;
	}
}

#define GUCCI_FUNC(Function) \
static Gci##Function##FuncT *gci##Function##HandlerP = 0; \
void Gci##Function##Func( Gci##Function##FuncT * f) \
{ \
    gci##Function##HandlerP = f; \
}

GUCCI_FUNC(Display);
GUCCI_FUNC(Motion);
GUCCI_FUNC(PassiveMotion);
GUCCI_FUNC(Keyboard);
GUCCI_FUNC(Idle);
GUCCI_FUNC(Reshape);
GUCCI_FUNC(Mouse);
GUCCI_FUNC(Special);
GUCCI_FUNC(Scrollwheel);

static bool gciRedisplay = true;
static bool displayDamaged = false;
static bool finished = false;

bool GciLayerDamaged()
{
	return displayDamaged;
}

void GciSwapBuffers()
{
	SDL_GL_SwapWindow(GlobalWindow);
	displayDamaged = false;
}

void GciPostRedisplay()
{
	gciRedisplay = displayDamaged = true;
}

class Callback
{
public:
	Callback(unsigned duration, GciCallbackT* callback, int value)
		: callback(callback), value(value), duration(duration)
	{
		callbackStart = std::chrono::steady_clock::now();
	};

	bool expired()
	{
		int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - callbackStart).count();
		return elapsed > duration;
	};

	void invoke();

private:
	GciCallbackT* callback;
	int value;
	unsigned duration;
	std::chrono::steady_clock::time_point callbackStart;
};

typedef list<Callback*> TimerList;
static TimerList timerEvents;

static void GciProcessTimerEvents()
{
	for (TimerList::iterator i = timerEvents.begin(); i != timerEvents.end(); )
	{
		Callback* c = *i;
		if (c->expired())
		{
			i = timerEvents.erase(i);
			c->invoke();
			delete c;
		}
		else
			i++;
	}
}

void Callback::invoke()
{
	(*callback)(value);
};

void GciTimerFunc(unsigned int millis, GciCallbackT * callback, int value)
{
	timerEvents.push_back(new Callback(millis, callback, value));
}

void GciMainLoop()
{
	finished = false;

	unsigned lastFrameTime = _GciGetAccurateTime();
	unsigned frameRate = 35;
	unsigned framePeriod = 1000 / frameRate;

	while (!finished)
	{
		//     unsigned now = _GciGetAccurateTime();
		//     unsigned frameDuration = now - lastFrameTime;

		//     if (frameDuration > framePeriod) {
		//       lastFrameTime = now;
		if (gciDisplayHandlerP)
			(*gciDisplayHandlerP)();

		SDL_Event event;

		/* Check for events */
		while (SDL_PollEvent(&event) && !finished)
		{  /* Loop until there are no events left on the queue */
			switch (event.type)
			{  /* Process the appropiate event type */
			case SDL_KEYDOWN:
			{
				int x, y;
				SDL_GetMouseState(&x, &y);

				SDL_Keycode keycode = event.key.keysym.sym;
				if ((event.key.keysym.mod & KMOD_NUM) == KMOD_NUM)
				{
					if (SDLK_KP_0 <= event.key.keysym.sym && event.key.keysym.sym <= SDLK_KP_9)
						keycode = SDLK_0 + (event.key.keysym.sym - SDLK_KP_0);
					else if (event.key.keysym.sym == SDLK_KP_PERIOD)
						keycode = SDLK_PERIOD;
					else if (event.key.keysym.sym == SDLK_KP_ENTER)
						keycode = SDLK_RETURN;
					else if (event.key.keysym.sym == SDLK_KP_PLUS)
						keycode = SDLK_PLUS;
					else if (event.key.keysym.sym == SDLK_KP_MINUS)
						keycode = SDLK_MINUS;
					else if (event.key.keysym.sym == SDLK_KP_MULTIPLY)
						keycode = SDLK_ASTERISK;
					else if (event.key.keysym.sym == SDLK_KP_DIVIDE)
						keycode = SDLK_SLASH;
					else if (event.key.keysym.sym == SDLK_KP_EQUALS)
						keycode = SDLK_EQUALS;
				}

				if (keycode == 0)
				{
					if (gciSpecialHandlerP)
						(*gciSpecialHandlerP)(sdlKeyToGucci(event.key.keysym.sym), x, y);
				}
				else
				{
					if (gciKeyboardHandlerP)
						(*gciKeyboardHandlerP)((unsigned char)keycode, x, y);
				}
			}
			break;
			case SDL_MOUSEMOTION:
				if (gciMotionHandlerP)
				{
					/* I have a feeling we should only call this if
					   a mouse button is depressed */
					(*gciMotionHandlerP)(event.motion.x, event.motion.y);
				}
				if (gciPassiveMotionHandlerP)
				{
					(*gciPassiveMotionHandlerP)(event.motion.x, event.motion.y);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if (gciMouseHandlerP)
				{
					(*gciMouseHandlerP)(sdlButtonToGucci(event.button.button),
						event.type == SDL_MOUSEBUTTONUP ? GCI_UP : GCI_DOWN,
						event.button.x,
						event.button.y);
				}
				break;
			case SDL_MOUSEWHEEL:
				if (gciMouseHandlerP && event.wheel.y != 0)
				{
					int button = (event.wheel.y > 0 ? GCI_WHEELUP : GCI_WHEELDOWN);
					int type = (event.wheel.y > 0 ? GCI_UP : GCI_DOWN);
					(*gciMouseHandlerP)(button, type, event.button.x, event.button.y);
				}
			case SDL_QUIT:
				finished = true;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_EXPOSED:
					displayDamaged = true;
					break;
				}
				break;
			default: /* Report an unhandled event */
				break;
			}
		}

		if (!finished)
		{
			/* Let other processes run */
			// SDL_Delay(1);

			GciProcessTimerEvents();

			if (gciIdleHandlerP)
				(*gciIdleHandlerP)();

			//     if (!gciRedisplay) {
			// 	    SDL_WaitEvent(&event);
			// 	    SDL_PushEvent(&event);
			//     }
		}
	}

	SDL_Quit();
}

void GciRestoreScreenSize()
{
	if (_GciIsInitGraphicsLibrary && SDL_WasInit(SDL_INIT_VIDEO))
	{
		// A try to remove some exit crashes
		int value = 0;
		if (SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value) == 0)
		{
			if (value != 0)
			{
				SDL_GL_SwapWindow(GlobalWindow);
			}
		}
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	_GciIsInitGraphicsLibrary = false;
	finished = true;
}

void GciStoreScreenSize()
{}

bool GciSetScreenSize(int width, int height,
	int bpp, int refresh)
{
	return true;
}

void GciResizeGlut(int width, int height)
{}

void GciRestoreGlut()
{}

static unsigned _GciGetAccurateTime()
{

#ifdef WIN32

	static bool supportsHighResTime = false;
	static bool initted = false;

	static LARGE_INTEGER frequency;
	static LARGE_INTEGER firstPerformanceCount;
	static DWORD firstTickCount = 0;

	if (!initted)
	{
		initted = true;

		if (QueryPerformanceFrequency(&frequency) &&
			QueryPerformanceCounter(&firstPerformanceCount))
		{

			supportsHighResTime = true;
		}
		else
		{
			firstTickCount = GetTickCount();
		}
	}

	if (supportsHighResTime)
	{
		LARGE_INTEGER performanceCount;
		QueryPerformanceCounter(&performanceCount);

		return (unsigned)(1000 * (performanceCount.QuadPart - firstPerformanceCount.QuadPart) / frequency.QuadPart);
	}
	else
	{
		return GetTickCount() - firstTickCount;
	}

	/*
	//return 1000 * ( (float) clock () / (float) CLOCKS_PER_SEC );

	LARGE_INTEGER lpFrequency;
	LARGE_INTEGER lpPerformanceCount;
	if ( QueryPerformanceFrequency ( &lpFrequency ) != 0 && QueryPerformanceCounter ( &lpPerformanceCount ) != 0 ) {

		return (unsigned) ( ( lpPerformanceCount.QuadPart * 1000 ) / lpFrequency.QuadPart );

	}

	return GetTickCount ();
	*/

#else

	// Linux version

	static bool initted = false;
	static struct timeval startTime;
	struct timeval tv;

	if (!initted)
	{
		initted = true;
		gettimeofday(&startTime, NULL);
		return 0;
	}

	gettimeofday(&tv, NULL);

	long diff_usec = tv.tv_usec - startTime.tv_usec;
	if (diff_usec < 0)
	{
		diff_usec += 1000000;
		tv.tv_sec--;
	}
	long diff_sec = tv.tv_sec - startTime.tv_sec;

	return 1000 * diff_sec + diff_usec / 1000;

#endif

}


GciScreenModeList* GciListScreenModes()
{

	GciScreenModeList* resolutions = new DArray<GciScreenMode*>;

	/* Get available fullscreen/hardware modes */
	SDL_DisplayMode tempMode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };

	int numModes = SDL_GetNumDisplayModes(0);
	for (int i = 0; i < numModes; i++)
	{
		if (SDL_GetDisplayMode(0, i, &tempMode) != 0)
		{
			printf("SDL_GetDisplayMode failed: %s", SDL_GetError());
		}

		if (tempMode.w >= 640 && tempMode.h >= 480)
		{
			GciScreenMode* newMode = new GciScreenMode;
			newMode->w = tempMode.w;
			newMode->h = tempMode.h;
			newMode->bpp = SDL_BITSPERPIXEL(tempMode.format);
			newMode->refreshRate = tempMode.refresh_rate;
			resolutions->PutData(newMode);
		}
	}

	return resolutions;
}

GciScreenMode* GciGetClosestScreenMode(int width, int height)
{
	GciScreenMode* newMode = new GciScreenMode;

	SDL_DisplayMode target, closest;
	target.w = width;
	target.h = height;
	
	if (SDL_GetClosestDisplayMode(0, &target, &closest) == nullptr)
	{
		printf("SDL_GetClosestDisplayMode returned nullptr");
		return nullptr;
	}

	newMode->w = closest.w;
	newMode->h = closest.h;
	newMode->bpp = SDL_BITSPERPIXEL(closest.format);
	newMode->refreshRate = closest.refresh_rate;

	return newMode;
}

GciScreenMode* GciGetClosestBorderlessMode()
{
	SDL_DisplayMode current;
	SDL_GetDesktopDisplayMode(0, &current);
	return GciGetClosestScreenMode(current.w, current.h);
}

void GciDeleteScreenModeArrayData(GciScreenModeList * modes)
{

	if (modes == NULL) return;

	for (int i = 0; i < modes->Size(); ++i)
		if (modes->ValidIndex(i))
			if (modes->GetData(i))
				delete modes->GetData(i);
	modes->Empty();
}

void GciSaveScreenshot(const char* file)
{

	SDL_Surface* temp;
	unsigned char* pixels;
	int i;

	int w = GlobalSurface->w;
	int h = GlobalSurface->h;

	if (!(GraphicsFlags & GCI_FULLSCREEN))
	{
		SDL_SaveBMP(GlobalSurface, file);
		return;
	}

	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, GlobalSurface->w, GlobalSurface->h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
		0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
	);
	if (temp == NULL)
		return;

	pixels = (unsigned char*)malloc(3 * w * h);
	if (pixels == NULL)
	{
		SDL_FreeSurface(temp);
		return;
	}

	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	for (i = 0; i < h; i++)
		memcpy(((char*)temp->pixels) + temp->pitch * i, pixels + 3 * w * (h - i - 1), w * 3);
	free(pixels);

	SDL_SaveBMP(temp, file);
	SDL_FreeSurface(temp);
	return;
}

bool GciAppVisible()
{
	int flags = SDL_GetWindowFlags(GlobalWindow);

	if ((flags & SDL_WINDOW_INPUT_FOCUS) == SDL_WINDOW_INPUT_FOCUS)
		return true;

	return false;

}

#endif // USE_SDL
