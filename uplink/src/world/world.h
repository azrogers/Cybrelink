
/*
	World class object

		Stores a database of information representing
		the game world, including companies, people,
		computers, missions etc.

  */

#ifndef _included_world_h
#define _included_world_h

// ============================================================================

#include <stdio.h>

#include "tosser.h"

#include "app/uplinkobject.h"
#include "world/date.h"
#include "world/generator/demoplotgenerator.h"
#include "world/generator/plotgenerator.h"
#include "world/scheduler/eventscheduler.h"

class VLocation;
class Company;
class Computer;
class Person;
class Mission;
class Player;
class GatewayDef;

// ============================================================================

class World : public UplinkObject {

protected:
	Date nextupdate;

public:
	Date date;
	EventScheduler scheduler;
	PlotGenerator plotgenerator;
	DemoPlotGenerator demoplotgenerator;

	BTree<VLocation*> locations;
	BTree<Company*> companies;
	BTree<Computer*> computers;
	BTree<Person*> people;

	DArray<char*> passwords; // No need to serialise
	DArray<GatewayDef*> gatewaydefs; // No need to serialise

public:
	World();
	virtual ~World();

	// Database management functions

	VLocation* CreateVLocation(const char* ip, int phys_x, int phys_y);
	bool VerifyVLocation(const char* ip, int phys_x, int phys_y);
	Company* CreateCompany(const char* name);
	Computer* CreateComputer(const char* name, const char* companyname, const char* ip);
	Person* CreatePerson(const char* name, const char* localhost);
	void CreatePassword(const char* password);

	void CreateVLocation(VLocation* vlocation);
	void CreateCompany(Company* company);
	void CreateComputer(Computer* computer);
	void CreatePerson(Person* person);
	void CreateGatewayDef(GatewayDef* newdef);

	VLocation* GetVLocation(const char* ip); //  These all return NULL
	Company* GetCompany(const char* name); //  if the specified object
	Computer* GetComputer(const char* name); //  is not found in the database
	Person* GetPerson(const char* name); //  (check before dereferencing)
	char* GetPassword(int index);
	GatewayDef* GetGatewayDef(const char* name);

	Player* GetPlayer(); //  Asserts that player exists

	// Common functions

	bool Load(FILE* file);
	void Save(FILE* file);
	void Print();
	void Update();

	std::string GetID();
};

#endif
