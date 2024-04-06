
/*

	Main Menu Screen base class

	All screens accessable via the main menu should be derived from this

  */

#ifndef _included_mainmenuscreen_h
#define _included_mainmenuscreen_h

#include "tosser.h"

class MainMenuScreen {

protected:
	DArray<char*>* interface_buttons;
	virtual void RegisterButton(int x, int y, int width, int height, const char* caption, const char* name);
	virtual void RegisterButton(
		int x, int y, int width, int height, const char* caption, const char* tooltip, const char* name);

public:
	MainMenuScreen();
	virtual ~MainMenuScreen();

	virtual void Create();
	virtual void Remove();
	virtual void Update();
	virtual bool IsVisible();

	virtual int ScreenID();

	virtual bool ReturnKeyPressed(); // Return true to block further usage of this return
};

#endif
