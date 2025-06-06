// FinanceInterface.cpp: implementation of the FinanceInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include "eclipse.h"
#include "redshirt.h"
#include "soundgarden.h"

#include "game/game.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "interface/interface.h"
#include "interface/localinterface/finance_interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/phonedialler.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/company/mission.h"
#include "world/computer/bankaccount.h"
#include "world/computer/computer.h"
#include "world/computer/recordbank.h"
#include "world/generator/missiongenerator.h"
#include "world/message.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/world.h"

FinanceInterface::FinanceInterface()
{

	oldbalance = 0;
	previousnumaccounts = 0;
	lastupdate = 0;
}

FinanceInterface::~FinanceInterface() { }

void FinanceInterface::TitleClick(Button* button)
{

	game->GetInterface()->GetLocalInterface()->RunScreen(SCREEN_NONE);
}

static void FinanceInterfaceTextDrawSplit(Button* button, bool highlighted, bool clicked)
{

	UplinkAssert(button);

	if (button->caption.empty()) {
		return;
	}

	SetColour("DefaultText");

	int xdistance[] = { 0, 55, 150, 220 };
	char splitString[256];
	size_t index = 0;
	size_t len = button->caption.length();

	for (int i = 0; i < (sizeof(xdistance) / sizeof(int)); i++) {
		size_t curIndex = 0;
		for (; index < len && index < (sizeof(splitString) - 1); index++) {
			char chr = button->caption[index];
			if (chr != ' ') {
				splitString[curIndex] = chr;
				curIndex++;
			} else if (curIndex > 0) {
				break;
			}
		}

		if (curIndex > 0) {
			splitString[curIndex] = '\0';
			text_draw(button->x + 10 + xdistance[i], button->y + 10, splitString, 256);
		}
	}
}

void FinanceInterface::DrawAccountsTitle(Button* button, bool highlighted, bool clicked)
{

	FinanceInterfaceTextDrawSplit(button, highlighted, clicked);

	SetColour("TitleText");

	glBegin(GL_LINES);
	glVertex2i(button->x, button->y + button->height);
	glVertex2i(button->x + button->width, button->y + button->height);
	glEnd();
}

void FinanceInterface::DrawAccountButton(Button* button, bool highlighted, bool clicked)
{

	UplinkAssert(button);

	int index;
	sscanf(button->name.c_str(), "finance_account %d", &index);

	// Draw a gray background if this is the player's current account

	if (index == game->GetWorld()->GetPlayer()->currentaccount) {
		glBegin(GL_QUADS);
		glColor4f(0.6f, 0.6f, 0.6f, ALPHA);
		glVertex2i(button->x, button->y);
		glColor4f(0.4f, 0.4f, 0.4f, ALPHA);
		glVertex2i(button->x + button->width, button->y);
		glColor4f(0.6f, 0.6f, 0.6f, ALPHA);
		glVertex2i(button->x + button->width, button->y + button->height);
		glColor4f(0.4f, 0.4f, 0.4f, ALPHA);
		glVertex2i(button->x, button->y + button->height);
		glEnd();
	} else {

		clear_draw(button->x, button->y, button->width, button->height);
	}

	FinanceInterfaceTextDrawSplit(button, highlighted, clicked);

	// Draw a box around the text if highlighted

	if (highlighted || clicked) {

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		border_draw(button);
	}
}

void FinanceInterface::ClickAccountButton(Button* button)
{

	UplinkAssert(button);

	int index;
	sscanf(button->name.c_str(), "finance_account %d", &index);

	if (index == game->GetWorld()->GetPlayer()->currentaccount) {

		// Get the account details

		char ip[SIZE_VLOCATION_IP];
		char accno[16];
		sscanf(game->GetWorld()->GetPlayer()->accounts.GetData(game->GetWorld()->GetPlayer()->currentaccount),
			   "%s %s",
			   ip,
			   accno);

		// Connect to this account computer

		game->GetWorld()->GetPlayer()->GetConnection()->Disconnect();
		game->GetWorld()->GetPlayer()->GetConnection()->Reset();

		PhoneDialler* pd = new PhoneDialler();
		pd->DialNumber(100, 100, ip, 4, accno);

	} else {

		// Set this as the current account

		int oldaccount = game->GetWorld()->GetPlayer()->currentaccount;
		game->GetWorld()->GetPlayer()->SetCurrentAccount(index);

		char oldaccount_buttonname[32];
		UplinkSnprintf(
			oldaccount_buttonname, sizeof(oldaccount_buttonname), "finance_account %d", oldaccount);
		EclDirtyButton(oldaccount_buttonname);
	}
}

void FinanceInterface::AfterPhoneDialler(char* ip, char* info)
{

	UplinkAssert(ip);
	UplinkAssert(info);

	game->GetWorld()->GetPlayer()->GetConnection()->Disconnect();
	game->GetWorld()->GetPlayer()->GetConnection()->Reset();

	game->GetWorld()->GetPlayer()->GetConnection()->AddVLocation(ip);
	game->GetWorld()->GetPlayer()->GetConnection()->Connect();

	game->GetInterface()->GetRemoteInterface()->RunNewLocation();

	// Lookup the user name and security level

	Computer* bank = game->GetWorld()->GetVLocation(ip)->GetComputer();
	UplinkAssert(bank);

	Record* rec = bank->recordbank.GetRecordFromAccountNumber(info);
	UplinkAssert(rec);

	const char* name = rec->GetField(RECORDBANK_NAME);
	UplinkAssert(name);

	const char* security_string = rec->GetField(RECORDBANK_SECURITY);
	UplinkAssert(security_string);
	int security;
	sscanf(security_string, "%d", &security);

	// Set security name and level
	// Run the accounts screen

	game->GetInterface()->GetRemoteInterface()->SetSecurity(name, security);
	game->GetInterface()->GetRemoteInterface()->RunScreen(3, bank);
}

void FinanceInterface::Create()
{

	if (!IsVisible()) {

		LocalInterfaceScreen::Create();

		int screenw = app->GetOptions()->GetOptionValue("graphics_screenwidth");
		int paneltop = (int)(100.0 * ((screenw * PANELSIZE) / 188.0) + 30);
		int panelwidth = (int)(screenw * PANELSIZE);

		EclRegisterButton(screenw - panelwidth,
						  paneltop + 3,
						  panelwidth - 7,
						  15,
						  "FINANCIAL",
						  "Remove the finance screen",
						  "finance_title");
		EclRegisterButtonCallback("finance_title", TitleClick);

		EclRegisterButton(screenw - panelwidth, paneltop + 50, panelwidth - 7, 15, "", "", "finance_balance");
		EclRegisterButtonCallbacks("finance_balance", text_draw, NULL, NULL, NULL);

		EclRegisterButton(screenw - panelwidth,
						  paneltop + 80,
						  panelwidth - 7,
						  15,
						  "AccNo     Bank            Balance",
						  "",
						  "finance_acctitle");
		EclRegisterButtonCallbacks("finance_acctitle", DrawAccountsTitle, NULL, NULL, NULL);

		// Create a new button for each open account

		for (int i = 0; i < game->GetWorld()->GetPlayer()->accounts.Size(); ++i) {

			char* accountdetails = game->GetWorld()->GetPlayer()->accounts.GetData(i);
			char ip[SIZE_VLOCATION_IP];
			char accno[16];
			sscanf(accountdetails, "%s %s", ip, accno);

			char accounttext[128];
			UplinkStrncpy(accounttext, "Logging on...", sizeof(accounttext));

			char name[32];
			UplinkSnprintf(name, sizeof(name), "finance_account %d", i);
			EclRegisterButton(screenw - panelwidth,
							  paneltop + 100 + i * 20,
							  panelwidth - 7,
							  15,
							  accounttext,
							  "Set this to be your current account",
							  name);
			EclRegisterButtonCallbacks(
				name, DrawAccountButton, ClickAccountButton, button_click, button_highlight);
			EclRegisterMiddleClickCallback(name, MiddleClickAccountButton);
		}

		previousnumaccounts = game->GetWorld()->GetPlayer()->accounts.Size();
		lastupdate = 0;

		EclRegisterButton(
			screenw - panelwidth + 150, paneltop + 50, 80, 15, "Pay All Fines", "finance_pay_fines");
		EclRegisterButtonCallbacks(
			"finance_pay_fines", button_draw, ClickPayFinesButton, button_click, button_highlight);
	}
}

void FinanceInterface::MiddleClickAccountButton(Button* button)
{
	Button* sourceButton = EclGetButton("s_accno_value -1 -1");
	if (!sourceButton) {
		return;
	}

	int index;
	sscanf(button->name.c_str(), "finance_account %d", &index);

	auto account = game->GetWorld()->GetPlayer()->accounts.GetData(index);
	char ip[SIZE_VLOCATION_IP];
	char accno[16];
	sscanf(account, "%s %s", ip, accno);

	Button* targetIpButton = EclGetButton("t_ip_value 0 0");
	Button* targetAccountNumButton = EclGetButton("t_accno_value 0 0");
	if (!targetIpButton || !targetAccountNumButton) {
		return;
	}

	targetIpButton->SetCaption(ip);
	targetAccountNumButton->SetCaption(accno);
}

void FinanceInterface::ClickPayFinesButton(Button* button)
{
	std::vector<Mission*> missionsToRemove;
	// find total amount of money the player has
	int totalMoney = 0;
	for (int i = 0; i < game->GetWorld()->GetPlayer()->accounts.Size(); i++) {
		char* accountdetails = game->GetWorld()->GetPlayer()->accounts.GetData(i);
		char ip[SIZE_VLOCATION_IP];
		char accno[16];
		sscanf(accountdetails, "%s %s", ip, accno);

		BankAccount* account = BankAccount::GetAccount(ip, accno);
		totalMoney += account->balance;
	}

	// find every payfine mission and force it to complete
	for (int i = 0; i < game->GetWorld()->GetPlayer()->missions.Size(); i++) {
		Mission* mission = game->GetWorld()->GetPlayer()->missions.GetData(i);
		if (mission->TYPE == MISSION_PAYFINE) {
			char targetIp[SIZE_VLOCATION_IP];
			char targetAccno[16];
			sscanf(mission->completionB, "%s %s", targetIp, targetAccno);

			Person* person = game->GetWorld()->GetPerson(mission->completionA);

			int amountToTransfer = atoi(mission->completionC);
			if (amountToTransfer > totalMoney) {
				// not enough to pay for the fine
				break;
			}

			for (int j = 0; j < game->GetWorld()->GetPlayer()->accounts.Size(); j++) {
				char* accountdetails = game->GetWorld()->GetPlayer()->accounts.GetData(j);
				char ip[SIZE_VLOCATION_IP];
				char accno[16];
				sscanf(accountdetails, "%s %s", ip, accno);
				BankAccount* account = BankAccount::GetAccount(ip, accno);

				if (account->balance >= amountToTransfer) {
					UplinkAssert(
						account->TransferMoney(ip, accno, targetIp, targetAccno, amountToTransfer, person));
					Message* m = new Message();
					m->SetFrom("PLAYER");
					m->SetTo(mission->contact);
					m->SetSubject("Mission completed");

					Person* person = game->GetWorld()->GetPerson(mission->contact);

					// force mission to complete
					bool isCompleted = MissionGenerator::IsMissionComplete(mission, person, m);
					UplinkAssert(isCompleted);
					totalMoney -= amountToTransfer;
					missionsToRemove.push_back(mission);

					delete m;
					break;
				}
			}
		}
	}

	// remove after for loop to avoid modifying the collection while iterating
	for (Mission* mission : missionsToRemove) {
		int idx = game->GetWorld()->GetPlayer()->missions.FindData(mission);
		game->GetWorld()->GetPlayer()->missions.RemoveData(idx);
		delete mission;
	}

	// remove messages generated when fine paid
	std::vector<Message*> messagesToRemove;
	for (int i = 0; i < game->GetWorld()->GetPlayer()->messages.Size(); i++) {
		Message* message = game->GetWorld()->GetPlayer()->messages.GetData(i);
		if (strcmp(message->GetSubject(), "Fine paid") == 0
			|| strcmp(message->GetSubject(), "Outstanding fine still not paid") == 0) {
			messagesToRemove.push_back(message);
		}
	}

	for (Message* message : messagesToRemove) {
		int idx = game->GetWorld()->GetPlayer()->messages.FindData(message);
		game->GetWorld()->GetPlayer()->messages.RemoveData(idx);
		delete message;
	}

	game->GetInterface()->GetLocalInterface()->Update();
}

void FinanceInterface::Remove()
{

	if (IsVisible()) {

		LocalInterfaceScreen::Remove();

		EclRemoveButton("finance_title");
		EclRemoveButton("finance_balance");
		EclRemoveButton("finance_acctitle");
		EclRemoveButton("finance_pay_fines");

		// Remove each account button

		int i = 0;
		char name[32];
		UplinkSnprintf(name, sizeof(name), "finance_account %d", i);

		while (EclGetButton(name)) {

			EclRemoveButton(name);
			++i;
			UplinkSnprintf(name, sizeof(name), "finance_account %d", i);
		}
	}
}

void FinanceInterface::Update()
{

	if (time(NULL) > lastupdate + 5 && IsVisible()) {

		// Create/Destroy new account buttons if neccisary

		if (previousnumaccounts < game->GetWorld()->GetPlayer()->accounts.Size()) {

			int screenw = app->GetOptions()->GetOptionValue("graphics_screenwidth");
			int paneltop = (int)(100.0 * ((screenw * PANELSIZE) / 188.0) + 30);
			int panelwidth = (int)(screenw * PANELSIZE);

			// Create a new button for each new open account

			for (int i = previousnumaccounts; i < game->GetWorld()->GetPlayer()->accounts.Size(); ++i) {

				char* accountdetails = game->GetWorld()->GetPlayer()->accounts.GetData(i);
				char ip[SIZE_VLOCATION_IP];
				char accno[16];
				sscanf(accountdetails, "%s %s", ip, accno);

				char accounttext[128];
				UplinkStrncpy(accounttext, "Logging on...", sizeof(accounttext));

				char name[32];
				UplinkSnprintf(name, sizeof(name), "finance_account %d", i);
				EclRegisterButton(screenw - panelwidth,
								  paneltop + 100 + i * 20,
								  panelwidth - 7,
								  15,
								  accounttext,
								  "Open this account",
								  name);
				EclRegisterButtonCallbacks(
					name, DrawAccountButton, ClickAccountButton, button_click, button_highlight);
				EclRegisterMiddleClickCallback(name, MiddleClickAccountButton);
			}

			previousnumaccounts = game->GetWorld()->GetPlayer()->accounts.Size();

		} else if (previousnumaccounts > game->GetWorld()->GetPlayer()->accounts.Size()) {

			for (int i = game->GetWorld()->GetPlayer()->accounts.Size(); i < previousnumaccounts; ++i) {

				char name[32];
				UplinkSnprintf(name, sizeof(name), "finance_account %d", i);
				EclRemoveButton(name);
			}

			previousnumaccounts = game->GetWorld()->GetPlayer()->accounts.Size();
		}

		// Update each account button with the new balance
		// Also work out the new balance (quicker to do it here than to call player->GetBalance )

		int balance = 0;

		for (int i = 0; i < game->GetWorld()->GetPlayer()->accounts.Size(); ++i) {

			char name[32];
			UplinkSnprintf(name, sizeof(name), "finance_account %d", i);
			UplinkAssert(EclGetButton(name));

			// Try to get the old balance

			int oldbalance = -1;

			if (strcmp(EclGetButton(name)->caption.c_str(), "Logging on...") != 0
				&& strcmp(EclGetButton(name)->caption.c_str(), "Failed to log on") != 0) {

				char accno[16];
				char tempip[SIZE_VLOCATION_IP];
				sscanf(EclGetButton(name)->caption.c_str(), "%s    %s    %d", accno, tempip, &oldbalance);
			}

			char ip[SIZE_VLOCATION_IP];
			char accno[16];
			sscanf(game->GetWorld()->GetPlayer()->accounts.GetData(i), "%s %s", ip, accno);

			// Lookup the balance

			BankAccount* ba = BankAccount::GetAccount(ip, accno);
			int newbalance = ba->balance;
			if (oldbalance == -1) {
				oldbalance = newbalance;
			}
			balance += newbalance;

			char newcaption[256];
			if (newbalance > oldbalance) {
				UplinkSnprintf(newcaption,
							   sizeof(newcaption),
							   "%s    %s    %dc     (+%dc)",
							   accno,
							   ip,
							   newbalance,
							   newbalance - oldbalance);
			} else if (newbalance < oldbalance) {
				UplinkSnprintf(newcaption,
							   sizeof(newcaption),
							   "%s    %s    %dc     (%dc)",
							   accno,
							   ip,
							   newbalance,
							   newbalance - oldbalance);
			} else {
				UplinkSnprintf(newcaption, sizeof(newcaption), "%s    %s    %dc", accno, ip, newbalance);
			}

			EclGetButton(name)->SetCaption(newcaption);

			if (newbalance != oldbalance) {
				EclDirtyButton("localint_background");
			}
		}

		// Update the overall balance

		char caption[128];
		UplinkSnprintf(caption, sizeof(caption), "Balance : %d credits", balance);
		EclGetButton("finance_balance")->SetCaption(caption);

		lastupdate = time(NULL);
	}
}

bool FinanceInterface::IsVisible() { return (EclGetButton("finance_title") != NULL); }

int FinanceInterface::ScreenID() { return SCREEN_FINANCE; }
