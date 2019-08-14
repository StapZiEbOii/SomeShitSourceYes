#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\IClientMode.h"
#include <deque>

class StickResolver
{
public:
	bool StickUseFreestandAngle[65];
	float StickFreestandAngle[65];
	Vector StickabsOriginBackup;
	float StickpitchHit[65];

	void StickOnCreateMove();
	void StickFrameStage(ClientFrameStage_t stage);
private:
	void StickAnimationFix(C_BaseEntity* pEnt);
};
extern StickResolver g_StickResolver;