
/*

  Bank Account class object

	A specific version of Company
	Part of the World Sub-system

  */

#ifndef _included_bankaccount_h
#define _included_bankaccount_h

#include "app/serialise.h"
#include "app/uplinkobject.h"

#include "world/computer/logbank.h"
#include "world/person.h"

class Person;

class BankAccount : public UplinkObject {

public:
	char name[SIZE_PERSON_NAME];
	char password[64];
	int security;
	int accountnumber;
	int balance;
	int loan;

	LogBank log;

public:
	BankAccount();
	~BankAccount();

	void SetOwner(const char* newname);
	void SetSecurity(const char* newpassword, int newsecurity);
	void SetAccNumber(int newaccnumber);
	void SetBalance(int newbalance, int newloan);

	void ChangeBalance(int amount, const char* description);

	// Has this account sent money to target account
	// If partial is true,  it return true if there is a log of the transaction on the target _or_ source
	// account If partial is false, it return true if there is a log of the transaction on the target _and_
	// source account
	bool
	HasTransferOccured(const char* s_ip, const char* t_ip, int t_accno, int amount, bool partial = false);
	bool HasTransferOccured(const char* person, int amount); // Has this account received payment from person

	Person* GetPerson();

	static BankAccount* GetAccount(const char* bankip, const char* accno);
	static bool TransferMoney(const char* source_ip,
							  const char* source_accno,
							  const char* target_ip,
							  const char* target_accno,
							  int amount,
							  Person* person);

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();

	std::string GetID();
	int GetOBJECTID();
};

class BankAccountId : public IUplinkSerializeable {
	char IpAddress[SIZE_VLOCATION_IP];
	char AccountNum[16];

public:
	const char* GetIpAddress() { return IpAddress; }
	const char* GetAccountNum() { return AccountNum; }

	void SetIpAddress(const char* addr) { strncpy(IpAddress, addr, SIZE_VLOCATION_IP); }
	void SetAccountNum(const char* num) { strncpy(AccountNum, num, 16); }

	virtual bool Read(FILE* file) override
	{
		fread(IpAddress, sizeof(char), SIZE_VLOCATION_IP, file);
		fread(AccountNum, sizeof(char), 16, file);
		return true;
	}
	virtual bool Write(FILE* file) override
	{
		fwrite(IpAddress, sizeof(char), SIZE_VLOCATION_IP, file);
		fwrite(AccountNum, sizeof(char), 16, file);
		return true;
	}

	operator BankAccount*() const
	{
		return BankAccount::GetAccount(const_cast<char*>(IpAddress), const_cast<char*>(AccountNum));
	}

	operator int() const { return atoi(AccountNum); }

	BankAccountId() { }

	BankAccountId(char* addr, char* num)
	{
		strncpy(IpAddress, addr, SIZE_VLOCATION_IP);
		strncpy(AccountNum, num, 16);
	}

	BankAccountId(char* addr, int num)
	{
		strncpy(IpAddress, addr, SIZE_VLOCATION_IP);
		sprintf(AccountNum, "%d", num);
	}
};

#endif
