#include "EnginePrediction.h"
#include "..\..\SDK\CInput.h"
#include "..\..\SDK\CEntity.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\Menu\Menu.h"

//outlassn https://www.unknowncheats.me/forum/1842735-post1.html and vrc https://www.unknowncheats.me/forum/2296204-post1.html



float flOldCurtime;
float flOldFrametime;
uintptr_t uRandomSeed = NULL;
void CEnginePred::run_prediction()
{
	static int flTickBase;
	static CUserCmd* pLastCmd;

	if (!g::pLocalEntity)
		return;
	// fix tickbase if game didnt render previous tick
	if (pLastCmd)
	{
		if (pLastCmd->hasbeenpredicted)
			flTickBase = g::pLocalEntity->GetTickBase();
		else
			++flTickBase;
	}

	// get random_seed as its 0 in clientmode->createmove
	const auto getRandomSeed = [&]()
	{
		using MD5_PseudoRandomFn = unsigned long(__cdecl*)(std::uintptr_t);
		static auto offset = Utils::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 70 6A 58");
		static auto MD5_PseudoRandom = reinterpret_cast<MD5_PseudoRandomFn>(offset);
		return MD5_PseudoRandom(g::pCmd->command_number) & 0x7FFFFFFF;
	};


	pLastCmd = g::pCmd;
	flOldCurtime = g_pGlobalVars->curtime;
	flOldFrametime = g_pGlobalVars->frametime;

	uRandomSeed = getRandomSeed();
	g_pGlobalVars->curtime = flTickBase * g_pGlobalVars->intervalPerTick;
	g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick;

	g_pMovement->StartTrackPredictionErrors(g::pLocalEntity);

	CMoveData data;
	memset(&data, 0, sizeof(CMoveData));

g_pMoveHelper->SetHost(g::pLocalEntity);
	//https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/gamemovement.cpp#L1135
	g_pPrediction->SetupMove(g::pLocalEntity, g::pCmd, g_pMoveHelper, &data);
	g_pMovement->ProcessMovement(g::pLocalEntity, &data);
	g_pPrediction->FinishMove(g::pLocalEntity, g::pCmd, &data);
}

void CEnginePred::end_prediction()
{


	if (!g::pLocalEntity)
		return;
	g_pMovement->FinishTrackPredictionErrors(g::pLocalEntity);
	g_pMoveHelper->SetHost(nullptr);

	g_pGlobalVars->curtime = flOldCurtime;
	g_pGlobalVars->frametime = flOldFrametime;
}

CEnginePred* prediction = new CEnginePred();