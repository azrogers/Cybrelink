// StatusInterface.h: interface for the StatusInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_mailviewinterface_h
#define _included_mailviewinterface_h


#include "interface/localinterface/localinterfacescreen.h"


class MailViewInterface : public LocalInterfaceScreen
{
private:
	int numMissions;
	int numMails;
	static Image* iclose_tif;
	static Image* iclose_h_tif;
	static Image* iclose_c_tif;

protected:

	static void MiniTitleDraw(Button *button, bool highlighted, bool clicked);

	static void TitleClick(Button *button);
	static void DrawMissionsTitle(Button *button, bool highlighted, bool clicked);
	static void SelectMission(Button *button);
	static void ViewMail(Button *button);
	static void DrawMailButton(Button* button, bool highlighted, bool clicked);
	static void MailMouseDown(Button* button);
	static void MailMouseMove(Button* button);

	static void RemoveMail(Button* button);
public:
	MailViewInterface();

	void Create();
	void Remove();
	void Update();
	bool IsVisible();

	int ScreenID();

};

#endif 
