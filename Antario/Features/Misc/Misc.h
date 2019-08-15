#pragma once

#include "..\Aimbot\Autowall.h"
#include "..\Aimbot\Aimbot.h"
#include "..\Aimbot\LagComp.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK\ICvar.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\Menu\Menu.h"
#include <iostream>
#include <algorithm>

// kinda just throw shit in here

#define _SOLVEY(a, b, c, d, e, f) ((c * b - d * a) / (c * f - d * e))
#define SOLVEY(...) _SOLVEY(?, ?, ?, ?, ?, ?)
#define SOLVEX(y, world, forward, right) ((world.x - right.x * y) / forward.x)
auto LoadNamedSky = reinterpret_cast<void(__fastcall*)(const char*)>(Utils::FindSignature("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
class Misc
{
public:
    void OnCreateMove()
    {
        this->pCmd   = g::pCmd;
        this->pLocal = g::pLocalEntity;

		this->DoSlowWalk();
		this->DoAutostrafe();
		this->DoBhop();
		this->DoFakeLag();
		this->AutoRevolver();
		this->AnimClantag();
		this->NightMode();
    };

	void NightMode() {
		static std::string old_Skyname = "";
		static bool OldNightmode;
		static int OldSky;
		if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame() || !g::pLocalEntity || !g::pLocalEntity->IsAlive())
		{
			old_Skyname = "";
			OldNightmode = false;
			OldSky = 0;
			return;
		}

		static ConVar* r_DrawSpecificStaticProp;
		if (OldNightmode != g_Menu.Config.Nightmode)
		{

			r_DrawSpecificStaticProp = g_pCvar->FindVar("r_DrawSpecificStaticProp");
			r_DrawSpecificStaticProp->SetValue(0);

			for (MaterialHandle_t i = g_pMaterialSys->FirstMaterial(); i != g_pMaterialSys->InvalidMaterial(); i = g_pMaterialSys->NextMaterial(i))
			{
				IMaterial* pMaterial = g_pMaterialSys->GetMaterial(i);
				if (!pMaterial)
					continue;
				if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
				{
					if (g_Menu.Config.Nightmode) {
						LoadNamedSky("sky_csgo_night02");

						if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
							pMaterial->ColorModulate(0.11f, 0.11f, 0.11f);
						else
							pMaterial->ColorModulate(0.05f, 0.05f, 0.05f);
					}
					else {
						LoadNamedSky("sky_cs15_daylight04_hdr");
						pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
					}
				}
			}
			OldNightmode = g_Menu.Config.Nightmode;
		}
	}

	void MovementFix(Vector& oldang) // i think osmium
	{
		Vector vMovements(g::pCmd->forwardmove, g::pCmd->sidemove, 0.f);

		if (vMovements.Length2D() == 0)
			return;

		Vector vRealF, vRealR;
		Vector aRealDir = g::pCmd->viewangles;
		aRealDir.Clamp();

		g_Math.AngleVectors(aRealDir, &vRealF, &vRealR, nullptr);
		vRealF[2] = 0;
		vRealR[2] = 0;

		VectorNormalize(vRealF);
		VectorNormalize(vRealR);

		Vector aWishDir = oldang;
		aWishDir.Clamp();

		Vector vWishF, vWishR;
		g_Math.AngleVectors(aWishDir, &vWishF, &vWishR, nullptr);

		vWishF[2] = 0;
		vWishR[2] = 0;

		VectorNormalize(vWishF);
		VectorNormalize(vWishR);

		Vector vWishVel;
		vWishVel[0] = vWishF[0] * g::pCmd->forwardmove + vWishR[0] * g::pCmd->sidemove;
		vWishVel[1] = vWishF[1] * g::pCmd->forwardmove + vWishR[1] * g::pCmd->sidemove;
		vWishVel[2] = 0;

		float a = vRealF[0], b = vRealR[0], c = vRealF[1], d = vRealR[1];
		float v = vWishVel[0], w = vWishVel[1];

		float flDivide = (a * d - b * c);
		float x = (d * v - b * w) / flDivide;
		float y = (a * w - c * v) / flDivide;

		g::pCmd->forwardmove = x;
		g::pCmd->sidemove = y;
	}

	void ThirdPerson(ClientFrameStage_t curStage)
	{
		if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected() || !g::pLocalEntity)
			return;
			
		static bool init = false;

		if (GetKeyState(VK_MBUTTON) && g::pLocalEntity->IsAlive())
		{
			if (init)
			{
				ConVar* sv_cheats = g_pCvar->FindVar("sv_cheats");
				*(int*)((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0; // ew
				sv_cheats->SetValue(1);
				g_pEngine->ExecuteClientCmd("thirdperson");
			}
			init = false;
		}
		else
		{
			if (!init)
			{
				ConVar* sv_cheats = g_pCvar->FindVar("sv_cheats");
				*(int*)((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0; // ew
				sv_cheats->SetValue(1);
				g_pEngine->ExecuteClientCmd("firstperson");
			}
			init = true;
		}

		if (curStage == FRAME_RENDER_START && GetKeyState(VK_MBUTTON) && g::pLocalEntity->IsAlive())
		{
			g_pPrediction->SetLocalViewAngles(Vector(g::RealAngle.x, g::RealAngle.y, 0)); // lol
		}
	}

	void Crosshair()
	{
		if (!g::pLocalEntity)
			return;

		if (!g::pLocalEntity->IsAlive())
			return;
		
		if (g::pLocalEntity->IsScoped() && g_Menu.Config.NoScope)
		{
			int Height, Width;
			g_pEngine->GetScreenSize(Width, Height);

			Vector punchAngle = g::pLocalEntity->GetAimPunchAngle();

			float x = Width / 2;
			float y = Height / 2;
			int dy = Height / 90;
			int dx = Width / 90;
			x -= (dx*(punchAngle.y));
			y += (dy*(punchAngle.x));

			Vector2D screenPunch = { x, y };

			g_pSurface->Line(0, screenPunch.y, Width, screenPunch.y, Color(0, 0, 0, 255));
			g_pSurface->Line(screenPunch.x, 0, screenPunch.x, Height, Color(0, 0, 0, 255));
		}

		static bool init = false;
		static bool init2 = false;

		if (g_Menu.Config.Crosshair)
		{
			if (g::pLocalEntity->IsScoped())
			{
				if (init2)
				{
					ConVar* sv_cheats = g_pCvar->FindVar("sv_cheats");
					*(int*)((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0; // ew
					sv_cheats->SetValue(1);

					g_pEngine->ExecuteClientCmd("weapon_debug_spread_show 0");
					g_pEngine->ExecuteClientCmd("crosshair 0");
				}
				init2 = false;
			}
			else
			{
				if (!init2)
				{
					ConVar* sv_cheats = g_pCvar->FindVar("sv_cheats");
					*(int*)((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0; // ew
					sv_cheats->SetValue(1);

					g_pEngine->ExecuteClientCmd("weapon_debug_spread_show 3");
					g_pEngine->ExecuteClientCmd("crosshair 1");
				}
				init2 = true;
			}

			init = false;
		}
		else
		{
			if (!init)
			{
				ConVar* sv_cheats = g_pCvar->FindVar("sv_cheats");
				*(int*)((DWORD)&sv_cheats->fnChangeCallback + 0xC) = 0; // ew
				sv_cheats->SetValue(1);

				g_pEngine->ExecuteClientCmd("weapon_debug_spread_show 0");
				g_pEngine->ExecuteClientCmd("crosshair 1");
			}
			init = true;
		}
	}

	void NormalWalk() // heh
	{
		g::pCmd->buttons &= ~IN_MOVERIGHT;
		g::pCmd->buttons &= ~IN_MOVELEFT;
		g::pCmd->buttons &= ~IN_FORWARD;
		g::pCmd->buttons &= ~IN_BACK;

		if (g::pCmd->forwardmove > 0.f)
			g::pCmd->buttons |= IN_FORWARD;
		else if (g::pCmd->forwardmove < 0.f)
			g::pCmd->buttons |= IN_BACK;
		if (g::pCmd->sidemove > 0.f)
		{
			g::pCmd->buttons |= IN_MOVERIGHT;
		}
		else if (g::pCmd->sidemove < 0.f)
		{
			g::pCmd->buttons |= IN_MOVELEFT;
		}
	}

private:
    CUserCmd*     pCmd;
    C_BaseEntity* pLocal;

    void DoBhop() const
    {
        if (!g_Menu.Config.Bhop)
            return;

		if (!g::pLocalEntity->IsAlive())
			return;

        static bool bLastJumped = false;
        static bool bShouldFake = false;

        if (!bLastJumped && bShouldFake)
        {
            bShouldFake = false;
            pCmd->buttons |= IN_JUMP;
        }
        else if (pCmd->buttons & IN_JUMP)
        {
            if (pLocal->GetFlags() & FL_ONGROUND)
                bShouldFake = bLastJumped = true;
            else 
            {
                pCmd->buttons &= ~IN_JUMP;
                bLastJumped = false;
            }
        }
        else
            bShouldFake = bLastJumped = false;
    }

	void DoAutostrafe() const
	{
		if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame() || !g_Menu.Config.AutoStrafe)
			return;

		if (!g::pLocalEntity->IsAlive())
			return;

		if (!(g::pLocalEntity->GetFlags() & FL_ONGROUND) && GetAsyncKeyState(VK_SPACE))
		{
			pCmd->forwardmove = (10000.f / g::pLocalEntity->GetVelocity().Length2D() > 450.f) ? 450.f : 10000.f / g::pLocalEntity->GetVelocity().Length2D();
			pCmd->sidemove = (pCmd->mousedx != 0) ? (pCmd->mousedx < 0.0f) ? -450.f : 450.f : (pCmd->command_number % 2) == 0 ? -450.f : 450.f;	
		}
	}

	void slow_walk(CUserCmd* cmd)
	{
		if (!GetAsyncKeyState(VK_SHIFT))
			return;

		if (!g::pLocalEntity)
			return;

		auto weapon_handle = g::pLocalEntity->GetActiveWeapon();

		if (!weapon_handle)
			return;

		Vector velocity = g::pLocalEntity->GetVelocity();
		Vector direction = velocity.Angle();
		float speed = velocity.Length();

		direction.y = cmd->viewangles.y - direction.y;

		Vector negated_direction = direction * -speed;
		if (velocity.Length() >= (weapon_handle->GetCSWpnData()->max_speed * .34f))
		{
			cmd->forwardmove = negated_direction.x;
			cmd->sidemove = negated_direction.y;
		}
	}

	#define Square(x) ((x)*(x))
	void MinWalk(CUserCmd* get_cmd, float get_speed) const
	{
		if (get_speed <= 0.f)
			return;

		float min_speed = (float)(FastSqrt(Square(get_cmd->forwardmove) + Square(get_cmd->sidemove) + Square(get_cmd->upmove)));
		if (min_speed <= 0.f)
			return;

		if (get_cmd->buttons & IN_DUCK)
			get_speed *= 2.94117647f;

		if (min_speed <= get_speed)
			return;

		float kys = get_speed / min_speed;

		get_cmd->forwardmove *= kys;
		get_cmd->sidemove *= kys;
		get_cmd->upmove *= kys;
	}
	void DoSlowWalk() const
	{
		if (VK_SHIFT > 0 && !GetAsyncKeyState(VK_SHIFT) || VK_SHIFT <= 0)
		{
			return;
		}

		MinWalk(pCmd, g_Menu.Config.WalkSpeed);
	}


	void DoFakeLag() const
	{
		if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame() || g_Menu.Config.Fakelag == 0 || g_Menu.Config.LegitBacktrack)
			return;
		
		if (!g::pLocalEntity->IsAlive())
			return;

		if (g::pLocalEntity->IsKnifeorNade())
			return;

		auto NetChannel = g_pEngine->GetNetChannel();

		if (!NetChannel)
			return;
		
		static float maxSpeed = 320.f;
		static float Acceleration = 5.5f;
		float maxAccelspeed = Acceleration * maxSpeed * g_pGlobalVars->intervalPerTick;

		float TicksToStop = g::pLocalEntity->GetVelocity().Length() / maxAccelspeed;

		bool canHit = false;
		static bool init = false;
		static bool stop = true;
		static bool stop2 = true;
		bool skip = false;

		if (g_Menu.Config.FakeLagOnPeek)
		{
			for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
			{
				C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

				if (!pPlayerEntity
					|| !pPlayerEntity->IsAlive()
					|| pPlayerEntity->IsDormant()
					|| pPlayerEntity == g::pLocalEntity
					|| pPlayerEntity->GetTeam() == g::pLocalEntity->GetTeam())
					continue;

				Vector EnemyHead = { pPlayerEntity->GetOrigin().x, pPlayerEntity->GetOrigin().y, (pPlayerEntity->GetHitboxPosition(0, g_Aimbot.Matrix[pPlayerEntity->EntIndex()]).z + 10.f) };

				Vector Head = { g::pLocalEntity->GetOrigin().x, g::pLocalEntity->GetOrigin().y, (g::pLocalEntity->GetHitboxPosition(0, g_Aimbot.Matrix[pPlayerEntity->EntIndex()]).z + 10.f) };
				Vector HeadExtr = (Head + (g::pLocalEntity->GetVelocity() * 0.203125f));
				Vector OriginExtr = ((g::pLocalEntity->GetOrigin() + (g::pLocalEntity->GetVelocity() * 0.21875f)) + Vector(0, 0, 8));

				float dmg;

				if (abs(g::pLocalEntity->GetVelocity().Length2D()) > 50.f && (g_Autowall.CanHitFloatingPoint(HeadExtr, EnemyHead) || g_Autowall.CanHitFloatingPoint(OriginExtr, EnemyHead)))
				{
					canHit = true;
				}
			}

			if (canHit)
			{
				if (stop2)
				{
					init = true;
					stop2 = false;
				}
			}
			else
				stop2 = true;

			if (init)
			{
				if (!stop)
				{
					g::bSendPacket = true;
					g::LagPeek = true;
					stop = true;
					skip = true;
				}
			}

			if (!skip)
			{
				if (g::LagPeek)
				{
					if (NetChannel->m_nChokedPackets < 13)
						g::bSendPacket = false;
					else
						g::LagPeek = false;
				}
				else
				{
					g::bSendPacket = (NetChannel->m_nChokedPackets >= g_Menu.Config.Fakelag);
					stop = false;
					init = false;
				}
			}
		}
		else
		{
			init = false;
			stop = true;
			stop2 = true;
			g::LagPeek = false;

			g::bSendPacket = (NetChannel->m_nChokedPackets >= g_Menu.Config.Fakelag);

			if (GetAsyncKeyState(VK_SHIFT))
				g::bSendPacket = (NetChannel->m_nChokedPackets >= 13);
		}

	}

	static void SetClanTag(std::string text, std::string text2)
	{
		if (!g_pEngine->IsInGame() || !g_pEngine->IsConnected())
			return;
		Hooks::SetClanTag(text.c_str(), text.c_str());
	}

	static void Marquee(std::string& clantag)
	{
		std::string temp = clantag;
		clantag.erase(0, 1);
		clantag += temp[0];
	}

	static void AnimClantag()
	{
		if (!g_pEngine->IsConnected() && g_pEngine->IsInGame())
		{
			g_Menu.Config.ClanTagMode = 0;
			SetClanTag(" ", " ");
		}

		if (g_Menu.Config.ClanTagMode == 1)
		{
			static float oldTime;
			if (g_pEngine->IsConnected())
			{
				if (g_pGlobalVars->curtime - oldTime >= 0.25f)
				{
					SetClanTag(g_Menu.Config.ClantagText, g_Menu.Config.ClantagText);
					oldTime = g_pGlobalVars->curtime;
				}
			}
		}
		else if (g_Menu.Config.ClanTagMode == 2)
		{
			// \u0020
			static std::string cur_clantag = " " + g_Menu.Config.ClantagText + " ";
			static float oldTime;

			if (g_pEngine->IsConnected())
			{
				if (g_pGlobalVars->curtime - oldTime >= 0.25f)
				{
					Marquee(cur_clantag);
					SetClanTag(cur_clantag.c_str(), g_Menu.Config.ClantagText);
					oldTime = g_pGlobalVars->curtime;
				}
			}
		}
	}

	void AutoRevolver()
	{
		auto me = g::pLocalEntity;
		auto cmd = g::pCmd;
		auto weapon = me->GetActiveWeapon();

		if (!g_Menu.Config.AutoRevolver)
			return;

		if (!me || !me->IsAlive() || !weapon)
			return;

		if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		{
			static int delay = 0; /// pasted delay meme from uc so we'll stop shooting on high ping
			delay++;

			if (delay <= 28)
				g::pCmd->buttons |= IN_ATTACK;
			else
				delay = 0;
		}
	}
};

extern Misc g_Misc;