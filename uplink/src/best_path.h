#pragma once
#include "tosser.h"
#include "game/game.h"
#include "world/vlocation.h"

class BestPathGenerator
{
public:
	static void CreatePath(Game* game, BTree<VLocation *>);
};