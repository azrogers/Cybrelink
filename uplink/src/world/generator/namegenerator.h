
#ifndef _included_namegenerator_h
#define _included_namegenerator_h

#define MAX_COMPUTERNAME 128
#define MAX_PERSONNAME 128

class NameGenerator {

protected:
	static LList<char*> fornames;
	static LList<char*> surnames;
	static LList<char*> agentaliases;
	static LList<char*> companynamesA;
	static LList<char*> companynamesB;

	static void LoadNames();

public:
	static void Initialise();
	static void Shutdown();

	static char* GenerateCompanyName();
	static char* GeneratePersonName();
	static char* GenerateAgentAlias();
	static char* GenerateBankName();

	static char* GeneratePassword();
	static const char* GenerateEasyPassword(); // Guessable
	static char* GenerateComplexPassword(); // Non dictionary hackable

	static char* GenerateDataName(const char* companyname, int DATATYPE);

	static char* GeneratePublicAccessServerName(const char* companyname);
	static char* GenerateAccessTerminalName(const char* companyname);
	static char* GenerateInternalServicesServerName(const char* companyname);
	static char* GenerateCentralMainframeName(const char* companyname);
	static char* GenerateInternationalBankName(const char* companyname);
	static char* GenerateFileServerName(const char* companyname);
	static char* GenerateLANName(const char* companyname);
	static char* GeneratePersonalComputerName(const char* personname);
	static char* GenerateVoicePhoneSystemName(const char* personname);
};

#endif
