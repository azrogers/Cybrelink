#include "best_path.h"
#include "game/game.h"
#include "game/data/data.h"
#include "world/world.h"
#include "world/player.h"

struct path_point
{
	int index;
	int x;
	int y;

	path_point(int index, int x, int y) : index(index), x(x), y(y) { }
};

void BestPathGenerator::CreatePath(Game* game, BTree<VLocation *> location)
{
	auto arr = location.ConvertToDArray();
	int firstIndex = -1;
	LList<path_point *> points;

	for(int i = 0; i < arr->Size(); i++)
	{
		// first point
		if(strcmp(arr->GetData(i)->ip, IP_INTERNIC) == 0)
		{
			firstIndex = i;
			continue;
		}

		if(!arr->GetData(i)->displayed || !game->GetWorld()->GetPlayer()->HasLink(arr->GetData(i)->ip))
		{
			continue;
		}

		points.PutData(new path_point(i, arr->GetData(i)->x, arr->GetData(i)->y));
	}

	// internic not found
	if(firstIndex == -1)
	{
		return;
	}

	path_point currentPoint(firstIndex, arr->GetData(firstIndex)->x, arr->GetData(firstIndex)->y);
	game->GetWorld()->GetPlayer()->connection.AddVLocation(IP_INTERNIC);

	int numPoints = points.Size();
	// for the number of points we have...
	for(int i = 0; i < numPoints; i++)
	{
		// iterate through all remaining points to find the closest to the current point
		int lastDistance = 0;
		int closestIndex = -1;
		int closestPointsIndex;
		for(int j = 0; j < points.Size(); j++)
		{
			auto point = points.GetData(j);
			int distance = sqrt(pow(point->x - currentPoint.x, 2) + pow(point->y - currentPoint.y, 2));
			if(closestIndex == -1 || distance < lastDistance)
			{
				closestIndex = point->index;
				closestPointsIndex = j;
				lastDistance = distance;
				continue;
			}
		}

		char ip[SIZE_VLOCATION_IP];
		UplinkStrncpy(ip, arr->GetData(closestIndex)->ip, SIZE_VLOCATION_IP);
		game->GetWorld()->GetPlayer()->connection.AddVLocation(ip);
		points.RemoveData(closestPointsIndex);
	}
}