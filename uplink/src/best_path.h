#pragma once
#include "game/game.h"
#include "tosser.h"
#include "world/vlocation.h"

class BestPathGenerator {
public:
	static void CreatePath(Game* game, BTree<VLocation*>);
};