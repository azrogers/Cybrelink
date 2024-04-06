
#ifndef _included_bankcomputer_h
#define _included_bankcomputer_h

#include "world/computer/bankaccount.h"
#include "world/computer/computer.h"

class BankComputer : public Computer {

protected:
	int previousaccno;

public:
	BTree<BankAccount*> accounts;

public:
	BankComputer();
	~BankComputer();

	int GenerateUniqueAccountNumber();

	int CreateBankAccount(BankAccount* newaccount); //  These return the account
	int CreateBankAccount(const char* name,
						  const char* password,
						  int security, //	number assigned
						  int balance,
						  int loan);

	void CloseAccount(int accno);

	BankAccount* GetRandomAccount();

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();
	void Update();

	std::string GetID();
	int GetOBJECTID();
};

#endif
