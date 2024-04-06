#include <spdlog/spdlog.h>

#include "animation.h"

static int NextAnimId = 0;

Animation::Animation() :
	buttonname(""),
	button(nullptr),
	targetC(""),
	id(NextAnimId++)
{
}
void Animation::DebugPrint()
{
	spdlog::info("ANIMATION : buttonname:'%s'\n", buttonname.c_str());
	spdlog::info("			  sX:%d, sY:%d, tX:%d, tY:%d, dX:%f, dY:%f\n",
				 sourceX,
				 sourceY,
				 targetX,
				 targetY,
				 dX,
				 dY);
	spdlog::info("		      sW:%d, sH:%d, tW:%d, tH:%d, dH:%f, dW:%f\n",
				 sourceW,
				 sourceH,
				 targetW,
				 targetH,
				 dH,
				 dW);
	spdlog::info("		      target caption : '%s', dC:%f\n", targetC.c_str(), dC);
	spdlog::info("			  time:%d, starttime:%d, finishtime:%d, MOVETYPE:%d\n",
				 time,
				 starttime,
				 finishtime,
				 MOVETYPE);
}