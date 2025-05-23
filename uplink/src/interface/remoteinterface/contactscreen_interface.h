
/*

  Contact Screen Interface

	Allows the user to talk to another person through
	an IRC style interface

  */

#ifndef _included_contactscreeninterface_h
	#define _included_contactscreeninterface_H

	#include "eclipse.h"
	#include "tosser.h"

	#include "interface/remoteinterface/remoteinterfacescreen.h"

// ============================================================================

class Mission;
class GenericScreen;

	#define CONTACT_TYPE_NONE 0
	#define CONTACT_TYPE_MISSION 1

// ============================================================================

class ContactScreenInterface : public RemoteInterfaceScreen {

protected:
	int TYPE; // What type of conference is this?
	LList<char*> users; // 0 is system, 1 is player, 2+ are NPC's

	LList<char*> messagequeue; // Messages NOT YET POSTED
	bool waiting; // True if waiting for message to "scroll" on
	int numquestions; // Number of clickable questions

	Mission* mission; // Specific to TYPE mission
	int origionalpayment; //
	int origionalpaymentmethod; //
	bool askedpayment; // Stops you from asking more than once
	bool askedpaymentmethod; // Stops you from asking more than once

protected:
	static void PutMessage(int userid, const char* message);
	static void PutMessage(int userid, std::string message) { PutMessage(userid, message.c_str()); }
	static void AskQuestion(int index);
	static void WaitingCallback(); // Called when messagehas finished "scrolling" on

	static void TypeHereDraw(Button* button, bool highlighted, bool clicked);
	static void PostClick(Button* button);

	static void BorderDraw(Button* button, bool highlighted, bool clicked);
	static void MessageDraw(Button* button, bool highlighted, bool clicked);

	static void QuestionDraw(Button* button, bool highlighted, bool clicked);
	static void QuestionClick(Button* button);

public:
	ContactScreenInterface();
	~ContactScreenInterface();

	void SetTYPE(int newTYPE);
	void SetMission(Mission* newmission);

	void AddUser(const char* name);
	void AddQuestion(const char* question, int index = -1);

	bool ReturnKeyPressed();

	void Create(ComputerScreen* cs);
	void Remove();
	void Update();
	bool IsVisible();

	int ScreenID();

	GenericScreen* GetComputerScreen();
};

#endif
