// StatusInterface.cpp: implementation of the StatusInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>


#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"
#include "world/vlocation.h"

#include "options/options.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/mail_view_interface.h"
#include "interface/localinterface/mission_interface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/recordbank.h"
#include "world/generator/recordgenerator.h"
#include "world/company/mission.h"
#include "world/message.h"

#include "mmgr.h"

Image *MailViewInterface::iclose_tif = NULL;
Image *MailViewInterface::iclose_h_tif = NULL;
Image *MailViewInterface::iclose_c_tif = NULL;

MailViewInterface::MailViewInterface()
{
	iclose_tif = get_assignbitmap("close.tif");
	iclose_h_tif = get_assignbitmap("close_h.tif");
	iclose_c_tif = get_assignbitmap("close_c.tif");
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void MailViewInterface::TitleClick(Button *button)
{

	game->GetInterface()->GetLocalInterface()->RunScreen(SCREEN_NONE);

}

void MailViewInterface::MiniTitleDraw(Button *button, bool highlighted, bool clicked)
{

	SetColour("TitleText");
	GciDrawText(button->x + 10, button->y + 10, button->caption, HELVETICA_18);

}

void MailViewInterface::DrawMissionsTitle(Button *button, bool highlighted, bool clicked)
{
	text_draw(button, highlighted, clicked);

	SetColour("TitleText");

	glBegin(GL_LINES);
	glVertex2i(button->x, button->y + button->height);
	glVertex2i(button->x + button->width, button->y + button->height);
	glEnd();
}

void MailViewInterface::SelectMission(Button* button)
{
	int missionIndex;
	sscanf(button->name, "mailview_selectmission %d", &missionIndex);

	if(game->GetWorld()->GetPlayer()->missions.ValidIndex(missionIndex))
	{
		game->GetInterface()->GetLocalInterface()->RunScreen(SCREEN_MISSION, missionIndex);
		//((MissionInterface *)game->GetInterface()->GetLocalInterface())->SetMission(missionIndex);
	}
}

void MailViewInterface::RemoveMail(Button* button)
{
	int mailIndex;
	sscanf(button->name, "mailview_removemail %d", &mailIndex);

	if(game->GetWorld()->GetPlayer()->messages.ValidIndex(mailIndex))
	{
		game->GetWorld()->GetPlayer()->messages.RemoveData(mailIndex);
	}
}

void MailViewInterface::ViewMail(Button* button)
{
	int mailIndex;
	sscanf(button->name, "mailview_viewmail %d", &mailIndex);

	if(game->GetWorld()->GetPlayer()->messages.ValidIndex(mailIndex))
	{
		game->GetInterface()->GetLocalInterface()->RunScreen(SCREEN_EMAIL, mailIndex);
	}
}

void MailViewInterface::MailMouseDown(Button *button)
{
	UplinkAssert(button);

	int index;
	sscanf(button->name, "mailview_viewmail %d", &index);

	if(game->GetWorld()->GetPlayer()->messages.ValidIndex(index))
	{
		button_click(button);
	}
}

void MailViewInterface::MailMouseMove(Button *button)
{
	UplinkAssert(button);

	int index;
	sscanf(button->name, "mailview_viewmail %d", &index);

	if(game->GetWorld()->GetPlayer()->messages.ValidIndex(index))
	{
		button_highlight(button);
	}
}

void MailViewInterface::DrawMailButton(Button *button, bool highlighted, bool clicked)
{
	UplinkAssert(button);

	clear_draw(button->x, button->y, button->width, button->height);

	int index;
	sscanf(button->name, "mailview_viewmail %d", &index);

	char* aColor = (highlighted || clicked ? "DarkPanelB" : "DarkPanelA");
	char* bColor = (highlighted || clicked ? "DarkPanelA" : "DarkPanelB");

	glBegin(GL_QUADS);
	SetColour(bColor);   glVertex2i(button->x, button->y);
	SetColour(aColor);   glVertex2i(button->x + button->width, button->y);
	SetColour(bColor);   glVertex2i(button->x + button->width, button->y + button->height);
	SetColour(aColor);   glVertex2i(button->x, button->y + button->height);
	glEnd();

	SetColour("DefaultText");

	// Draw the text

	GciDrawText(button->x + 10, button->y + 10, button->caption);

	// Draw a box around the text if highlighted

	if(highlighted || clicked)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		border_draw(button);
	}
}

void MailViewInterface::Create()
{

	if(!IsVisible())
	{

		LocalInterfaceScreen::Create();

		int screenw = app->GetOptions()->GetOptionValue("graphics_screenwidth");
		int screenh = app->GetOptions()->GetOptionValue("graphics_screenheight");
		int paneltop = (int)(100.0 * ((screenw * PANELSIZE) / 188.0) + 30);
		int panelwidth = (int)(screenw * PANELSIZE);

		int cwidth = 95;

		EclRegisterButton(screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "MAIL", "Remove the mail screen", "mailview_title");
		EclRegisterButtonCallback("mailview_title", TitleClick);

		/*EclRegisterButton(screenw - panelwidth, paneltop + 30, panelwidth - 7, 15, "Missions", "", "mailview_missiontitle");
		EclRegisterButtonCallbacks("mailview_missiontitle", DrawMissionsTitle, NULL, NULL, NULL);

		// draw all missions
		char name[128];
		char caption[128];
		char caption2[256];
		int nextY = paneltop + 50;
		for(int i = 0; i < game->GetWorld()->GetPlayer()->missions.Size(); i++)
		{
			UplinkSnprintf(name, sizeof(name), "mailview_selectmission %d", i);
			char *details = game->GetWorld()->GetPlayer()->missions.GetData(i)->description;
			int captionIndex = 0;
			for(int j = 0; j < sizeof(caption); j++)
			{
				if(details[j] != '\n')
				{
					caption[captionIndex++] = details[j];
				}

				if(details[j] == 0x00)
				{
					break;
				}
			}

			UplinkSnprintf(caption2, sizeof(caption2), "%s    %s",
				game->GetWorld()->GetPlayer()->missions.GetData(i)->employer,
				caption);

			EclRegisterButton(screenw - panelwidth, nextY, panelwidth - 7, 15, caption2, name);
			EclRegisterButtonCallback(name, SelectMission);

			nextY += 15 + 2;
		}

		numMissions = game->GetWorld()->GetPlayer()->missions.Size();*/
		
		EclRegisterButton(screenw - panelwidth, paneltop + 30, panelwidth - 7, 15, "Messages", "", "mailview_messagestitle");
		EclRegisterButtonCallbacks("mailview_messagestitle", DrawMissionsTitle, NULL, NULL, NULL);

		char name[128];
		char caption[128];
		int nextY = paneltop + 50;
		for(int i = 0; i < game->GetWorld()->GetPlayer()->messages.Size(); i++)
		{
			UplinkSnprintf(name, sizeof(name), "mailview_removemail %d", i);

			EclRegisterButton(screenw - panelwidth + 1, nextY + 1, 13, 13, "", "Remove mail", name);
			button_assignbitmaps(name, iclose_tif, iclose_h_tif, iclose_c_tif);
			EclRegisterButtonCallback(name, RemoveMail);

			UplinkSnprintf(name, sizeof(name), "mailview_viewmail %d", i);
			auto message = game->GetWorld()->GetPlayer()->messages.GetData(i);
			UplinkSnprintf(caption, sizeof(caption), "%s", message->GetSubject());
			EclRegisterButton(screenw - panelwidth + 15 + 2, nextY, panelwidth - 7 - 15 - 2, 15, caption, name);
			EclRegisterButtonCallbacks(name, DrawMailButton, ViewMail, MailMouseDown, MailMouseMove);

			nextY += 15 + 2;
		}

		numMails = game->GetWorld()->GetPlayer()->messages.Size();
	}
}

void MailViewInterface::Remove()
{

	if(IsVisible())
	{

		LocalInterfaceScreen::Remove();

		EclRemoveButton("mailview_title");
		EclRemoveButton("mailview_messagestitle");
		/*EclRemoveButton("mailview_missiontitle");

		char name[128];
		for(int i = 0; i < numMissions; i++)
		{
			UplinkSnprintf(name, sizeof(name), "mailview_selectmission %d", i);
			EclRemoveButton(name);
		}*/

		char name[128];
		for(int i = 0; i < numMails; i++)
		{
			UplinkSnprintf(name, sizeof(name), "mailview_removemail %d", i);
			EclRemoveButton(name);
			UplinkSnprintf(name, sizeof(name), "mailview_viewmail %d", i);
			EclRemoveButton(name);
		}
	}
}

void MailViewInterface::Update()
{
	/*auto currentNumMissions = game->GetWorld()->GetPlayer()->missions.Size();

	// recreate
	if(currentNumMissions != numMissions)
	{
		Remove();
		Create();
	}*/

	auto currentNumMails = game->GetWorld()->GetPlayer()->messages.Size();
	if(currentNumMails != numMails)
	{
		Remove();
		Create();
	}
}

bool MailViewInterface::IsVisible()
{

	return (EclGetButton("mailview_title") != NULL);

}

int MailViewInterface::ScreenID()
{

	return SCREEN_MAILVIEW;

}
