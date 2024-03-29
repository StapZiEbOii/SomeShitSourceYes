#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\IClientMode.h"
#include <deque>

class ShonaxResolver
{
public:
	bool UseFreestandAngle[65];
	float FreestandAngle[65];
	Vector absOriginBackup;
	float pitchHit[65];

	void OnCreateMove();
	void FrameStage(ClientFrameStage_t stage);
private:
	void AnimationFix(C_BaseEntity* pEnt);
};
extern ShonaxResolver g_Resolver;