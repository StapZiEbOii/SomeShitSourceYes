#include "Menu.h"
#include "TGFCfg.h"
#include "..\SDK\Vector.h"
#include "..\SDK\ISurface.h"
#include "..\Utils\Color.h"
#include "..\Utils\GlobalVars.h"
#include "..\Utils\XorStr.h"

Menu g_Menu;


static int save_slot = 0;
#include <ShlObj.h>



void Menu::Render()
{
	int w, h;
	g_pEngine->GetScreenSize(w, h);

	time_t now = time(0);
	char* dt = ctime(&now);

	// Logo

	g_pSurface->DrawSetColor(0, 0, 0, 145);
	g_pSurface->DrawFilledRect(w - 128 /* Width Left */, 27 /* Height Top */, 220 /* Right Width */ + w - 275 /* Negative Left (Right Side) */, 101 /* Bottom Hight */);


	g_pSurface->DrawSetColor(0, 0, 0, 255);
	g_pSurface->DrawOutlinedRect(w - 128 /* Width Left */, 27 /* Height Top */, 220 /* Right Width */ + w - 275 /* Negative Left (Right Side) */, 101 /* Bottom Hight */);

	g_pSurface->DrawSetColor(255, 8, 127, 255);
	g_pSurface->DrawFilledRect(w - 122 /* Width Left */, 36 /* Height Top */, 299 /* Right Width */ + w - 360 /* Negative Left (Right Side) */, 94 /* Bottom Hight */);

	g_pSurface->DrawSetColor(0, 0, 0, 255);
	g_pSurface->DrawFilledRect(w - 122 /* Width Left */, 39 /* Height Top */, 168 /* Right Width */ + w - 229 /* Negative Left (Right Side) */, 95 /* Bottom Hight */);

	g_pSurface->DrawT(w - 100, 45, Color(255, 8, 127, 255), g::WatermarkFont, false, "C");


	// Date

	g_pSurface->DrawSetColor(0, 0, 0, 145);
	g_pSurface->DrawFilledRect(w - 366 /* Width Left */, 27 /* Height Top */, 36 /* Right Width */ + w - 176 /* Negative Left (Right Side) */, 100 /* Bottom Hight */);

	g_pSurface->DrawSetColor(0, 0, 0, 255);
	g_pSurface->DrawOutlinedRect(w - 366 /* Width Left */, 27 /* Height Top */, 36 /* Right Width */ + w - 176 /* Negative Left (Right Side) */, 100 /* Bottom Hight */);

	g_pSurface->DrawSetColor(255, 8, 127, 255);
	g_pSurface->DrawFilledRect(w - 360 /* Width Left */, 36 /* Height Top */, 265 /* Right Width */ + w - 409 /* Negative Left (Right Side) */, 91 /* Bottom Hight */);

	g_pSurface->DrawSetColor(0, 0, 0, 255);
	g_pSurface->DrawFilledRect(w - 360 /* Width Left */, 39 /* Height Top */, 241 /* Right Width */ + w - 385 /* Negative Left (Right Side) */, 95 /* Bottom Hight */);

	g_pSurface->DrawT(w - 262, 45, Color(255, 255, 255, 255), g::WatermarkDate, false, "DATE");
	g_pSurface->DrawT(w - 326, 61, Color(255, 8, 127, 255), g::WatermarkDate, false, dt);

	static bool Pressed = false;

	if (!Pressed && GetAsyncKeyState(VK_INSERT))
		Pressed = true;
	else if (Pressed && !GetAsyncKeyState(VK_INSERT))
	{
		Pressed = false;
		menuOpened = !menuOpened;
	}

	static Vector2D oldPos;
	static Vector2D mousePos;

	static int dragX = 0;
	static int dragY = 0;
	static int Width = 400;
	static int Height = 340;

	static int iScreenWidth, iScreenHeight;

	static bool Dragging = false;
	bool click = false;

	if (menuOpened)
	{
		if (GetAsyncKeyState(VK_LBUTTON))
			click = true;

		g_pEngine->GetScreenSize(iScreenWidth, iScreenHeight);
		Vector2D MousePos = g_pSurface->GetMousePosition();

		if (Dragging && !click)
		{
			Dragging = false;
		}

		if (Dragging && click)
		{
			Pos.x = MousePos.x - dragX;
			Pos.y = MousePos.y - dragY;
		}

		if (g_pSurface->MouseInRegion(Pos.x, Pos.y, Width, 20))
		{
			Dragging = true;
			dragX = MousePos.x - Pos.x;
			dragY = MousePos.y - Pos.y;
		}

		if (Pos.x < 0)
			Pos.x = 0;
		if (Pos.y < 0)
			Pos.y = 0;
		if ((Pos.x + Width) > iScreenWidth)
			Pos.x = iScreenWidth - Width;
		if ((Pos.y + Height) > iScreenHeight)
			Pos.y = iScreenHeight - Height;

		g_pSurface->FilledRect(Pos.x - 8, Pos.y - 26, 421, 377, Color(0, 0, 0, 155));
		g_pSurface->FilledRect(Pos.x - 10, Pos.y - 26, 421, 377, Color(0, 0, 0, 155));
		g_pSurface->FilledRect(Pos.x, Pos.y, Width, Height - 4, Color(247, 20, 122, 255));
		g_pSurface->OutlinedRect(Pos.x - 10, Pos.y - 26, 421, 377, Color(0, 0, 0, 255));
		g_pSurface->FilledRect(Pos.x, Pos.y, Width, Height, Color(0, 0, 0, 255));
		g_pSurface->DrawT(Pos.x - 0, Pos.y - 17, Color(255, 255, 255, 255), g::Tahoma, false, "CHAINSNATCH.IN Recode // ");

		GroupTabPos[0] = Pos.x + 85;
		GroupTabPos[1] = Pos.y + 25;
		GroupTabPos[2] = Width - 91;
		GroupTabPos[3] = Height - 50;

		ControlsX = GroupTabPos[0];
		GroupTabBottom = GroupTabPos[1] + GroupTabPos[3];

		OffsetY = GroupTabPos[1] + 7;

		static bool CfgInitLoad = true;
		static bool CfgInitSave = true;

		static int SaveTab = 0;

		if (g_pSurface->MouseInRegion(Pos.x + 307, Pos.y + 315, 0, 0))
		{
			if (CfgInitLoad && click)
			{
				SaveTab = 0;

				g_Config->Load();
				CfgInitLoad = false;
			}
		}
		else
			CfgInitLoad = true;

		if (g_pSurface->MouseInRegion(Pos.x + 307, Pos.y + 289, 0, 0))
		{
			if (CfgInitSave && click)
			{
				SaveTab = 1;

				g_Config->Save();
				CfgInitSave = false;
			}
		}
		else
			CfgInitSave = true;


		TabOffset = 0;
		SubTabOffset = 0;
		PreviousControl = -1;
		OldOffsetY = 0;

		Tab("A");
		{
			std::string aimbotname = "Enabled";
			CheckBox(aimbotname, &Config.Aimbot);
			ComboBox("Hitchance", { "off", "Bonus", "Normal" }, & Config.Hitchance);
			if (Config.Hitchance != 0)
				Slider(100, "HC Value", &Config.HitchanceValue);
			Slider(100, "Weapon Min Dmg.", &Config.Mindmg);
			CheckBox("Auto-Stop", &Config.Autostop);
			CheckBox("Faster Shot Flick Fix", &Config.FixShotPitch);
			CheckBox("Resolver", &Config.ResolverEnable);
			if (Config.ResolverEnable) ComboBox("", { "Perfection", "Experimental", "Beta" }, &Config.Resolver);
			CheckBox("Delay Shot", &Config.DelayShot);
			CheckBox("Ignore Limbs", &Config.IgnoreLimbs);
			CheckBox("Auto Revolver", &Config.AutoRevolver);

		}

		Tab("B");
		{
			CheckBox("Enabled", &Config.Esp);
			CheckBox("hitbox points", &Config.HitboxPoints);
			CheckBox("Chams", &Config.Chams);
			ColorPicker("Chams Color", Config.ChamsColor);
			CheckBox("Remove Zoom", &Config.NoZoom);
			CheckBox("No Scope", &Config.NoScope);
			CheckBox("Remove Recoil", &Config.NoRecoil);
			Slider(180, "Field Of View", &Config.Fov);
			CheckBox("Crosshair", &Config.Crosshair);


			ColorPicker("Text Color", Config.FontColor);
			ComboBox("Show Player Names", { "off", "top", "right" }, & Config.Name);
			ComboBox("Show Player Health", { "off", "right" }, & Config.HealthVal);
			ComboBox("Show Player Weapons", { "off", "bottom", "right" }, & Config.Weapon);
			CheckBox("Box ESP", &Config.Box);
			ColorPicker("Box Color", Config.BoxColor);
			CheckBox("Health", &Config.HealthBar);

		}

		Tab("C");
		{
			std::string antiaimname = "Enabled";
			CheckBox(antiaimname, &Config.Antiaim);
			CheckBox("Desync Angle", &Config.DesyncAngle);
			CheckBox("Random Jitter", &Config.RandJitterInRange);
			Slider(360, "Jitter Range", &Config.JitterRange);
			Slider(14, "Fakelag", &Config.Fakelag);
			CheckBox("On Peek", &Config.FakeLagOnPeek);
			if (Config.FakeLagOnPeek)
				CheckBox("Choke shot", &Config.ChokeShotOnPeek);

			MultiComboBox("Skeleton", { "Normal", "Backtrack Bones" }, Config.Skeleton);
			ColorPicker("Bones Color", Config.SkeletonColor);
			Slider(1000, "gay", &Config.Test1);
			Slider(1000, "gay", &Config.Test2);
			Slider(1000, "gay", &Config.Test3);
			Slider(1000, "gay", &Config.Test4);

		}

		Tab("D");
		{

			CheckBox("Shot Backtrack", &Config.ShotBacktrack);
			CheckBox("Position Backtrack", &Config.PosBacktrack);
			CheckBox("B-Aim If Lethal", &Config.BaimLethal);
			CheckBox("B-Aim If High inaccuracy", &Config.BaimPitch);
			CheckBox("B-Aim In Air", &Config.BaimInAir);
			CheckBox("Auto Bunnyhop", &Config.Bhop);
			CheckBox("Auto Strafe", &Config.AutoStrafe);
			CheckBox("Animated Clantag", &Config.ClanTag);
			CheckBox("Fake Walk", &Config.SlowWalk);
			if (Config.SlowWalk) Slider(100, "Speed", &Config.WalkSpeed);
			g_pSurface->DrawT(Pos.x + 307, Pos.y + 289, Color(255, 255, 255, 255), g::CourierNew, false, "Save Config");
			g_pSurface->DrawT(Pos.x + 307, Pos.y + 315, Color(255, 255, 255, 255), g::CourierNew, false, "Load Config");
		}

		TabSize = TabOffset;
		SubTabSize = SubTabOffset;
	}
}

void Menu::Tab(std::string name)
{
	int TabArea[4] = { Pos.x, Pos.y + 35 + (TabOffset * 78), 80, 26 };

	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(TabArea[0], TabArea[1], TabArea[2], TabArea[3]))
		TabNum = TabOffset;

	if (TabOffset == 0)
		g_pSurface->FilledRect(TabArea[0], TabArea[1], 70, (TabSize * 25), Color(0, 0, 0, 255));

	if (TabOffset == TabNum)
	{
		g_pSurface->DrawT(TabArea[0] + 17, TabArea[1] + 0, Color(255, 0, 76, 255), g::Tab, false, name.c_str());
		g_pSurface->FilledRect(TabArea[0], TabArea[1], 0, TabArea[3],Color(0, 0, 0, 255));
		g_pSurface->FilledRect(TabArea[0], TabArea[1], 0, TabArea[3],Color(0, 0, 0, 255));
	}

	g_pSurface->DrawT(TabArea[0] + 17, TabArea[1] + 0, Color(255, 255, 255, 255), g::Tab, false, name.c_str());

	TabOffset += 1;
	PreviousControl = -1;
}

void Menu::SubTab(std::string name)
{
	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	RECT TextSize = g_pSurface->GetTextSizeRect(g::CourierNew, name.c_str());

	static int TabSkip = 0;

	if (SubTabOffset == 0)
		g_pSurface->FilledRect(GroupTabPos[0], GroupTabPos[1], GroupTabPos[2], 21, Color(0, 0, 0, 255));

	if (SubTabSize != 0 && TabSkip == TabNum)
	{
		if (TabNum >= SubTabSize)
			TabNum = 0;

		int TabLength = (GroupTabPos[2] / SubTabSize);

		int GroupTabArea[4] = { (GroupTabPos[0]) + (TabLength * SubTabOffset), GroupTabPos[1], TabLength, 21 };

		if ((GroupTabArea[0] + GroupTabArea[3]) <= (GroupTabPos[0] + GroupTabPos[2]))
		{
			int TextPosition[2] = { GroupTabArea[0] + (TabLength / 2) - (TextSize.right / 2), (GroupTabArea[1] + 10) - (TextSize.bottom / 2) };

			if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(GroupTabArea[0], GroupTabArea[1], GroupTabArea[2], GroupTabArea[3]))
				SubTabNum = SubTabOffset;

			int Offset = ((SubTabSize - 1) == SubTabOffset) ? 0 : 1;

			if (((SubTabSize - 1) == SubTabOffset) && (((TabLength * SubTabSize) > GroupTabPos[2]) || ((TabLength * SubTabSize) < GroupTabPos[2])))
				Offset = (GroupTabPos[2] - (TabLength * SubTabSize));

			if (SubTabNum == SubTabOffset)
				g_pSurface->FilledRect(GroupTabArea[0], GroupTabArea[1], GroupTabArea[2] + Offset, GroupTabArea[3],Color(0, 0, 0, 255));

			g_pSurface->DrawT(TextPosition[0], TextPosition[1], Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());
		}
	}

	if (TabSkip != TabNum) // frame skip for drawing
		TabSkip = TabNum;

	if (SubTabOffset == SubTabNum)
		OffsetY += 20;

	SubTabOffset += 1;
	PreviousControl = -1;
}

void Menu::CheckBox(std::string name, bool* item)
{
	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	static bool pressed = false;

	if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 0, OffsetY, 16, 16))
	{
		if (pressed)
			* item = !*item;
		pressed = false;
	}

	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 0, OffsetY, 16, 16) && !pressed)
		pressed = true;
	
	if (*item == true)

		g_pSurface->FilledRect(ControlsX + 0, OffsetY, 16, 16, Color(255, 0, 115, 255));
	else
		g_pSurface->FilledRect(ControlsX + 0, OffsetY, 16, 16, Color(26, 26, 26, 255));

	g_pSurface->OutlinedRect(ControlsX + 0, OffsetY, 16, 16, Color(0, 0, 0, 255));

	g_pSurface->DrawT(ControlsX + 26, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());

	OldOffsetY = OffsetY;
	OffsetY += 26;
	PreviousControl = check_box;
}

void Menu::Slider(int max, std::string name, int* item)
{
	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	float pixelValue = max / 114.f;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 0, OffsetY + 5, 120, 8))
		*item = (g_pSurface->GetMousePosition().x - (ControlsX + 2)) * pixelValue;

	if (*item > max)
		*item = max;
	if (*item < 0)
		*item = 0;

	g_pSurface->FilledRect(ControlsX + 0, OffsetY + 5, 120, 8, Color(0, 0, 0, 255));
	g_pSurface->OutlinedRect(ControlsX + 0, OffsetY + 5, 120, 8, Color(245, 66, 123, 255));
	g_pSurface->FilledRect(ControlsX + 0 + (*item / pixelValue), OffsetY + 5, 5, 8, Color(245, 66, 123, 255));
	g_pSurface->DrawT(ControlsX + 51, OffsetY - 8, Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());

	g_pSurface->DrawT(ControlsX + 61, OffsetY + 1, Color(255, 255, 255, 255), g::Tahoma, true, std::to_string(*item).c_str());

	OldOffsetY = OffsetY;
	OffsetY += 26;
	PreviousControl = slider;
}

void Menu::ComboBox(std::string name, std::vector< std::string > itemname, int* item)
{
	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	bool pressed = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 153, OffsetY, 140, 16) && !clickRest)
	{
		nameSelected = name;
		pressed = true;
		clickRest = true;
	}
	else if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 153, OffsetY, 140, 16))
		clickRest = false;

	if (pressed)
	{
		if (!rest)
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if (nameSelected == name)
		open = selectedOpened;

	g_pSurface->DrawT(ControlsX + 6, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());
	g_pSurface->FilledRect(ControlsX + 153, OffsetY, 140, 16,Color(0, 0, 0, 255));
	g_pSurface->OutlinedRect(ControlsX + 153, OffsetY, 140, 16, Color(255, 0, 76, 255));

	if (open)
	{
		g_pSurface->FilledRect(ControlsX + 153, OffsetY, 140, 17 + (itemname.size() * 16),Color(0, 0, 0, 255));
		g_pSurface->OutlinedRect(ControlsX + 153, OffsetY, 140, 17 + (itemname.size() * 16), Color(245, 66, 123, 255));

		for (int i = 0; i < itemname.size(); i++)
		{
			if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 153, OffsetY + 16 + i * 16, 140, 16))
				*item = i;

			if (*item == i)
				g_pSurface->FilledRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16,Color(0, 0, 0, 255));
			g_pSurface->OutlinedRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16, Color(245, 66, 123, 255));

			g_pSurface->DrawT(ControlsX + 159, OffsetY + 16 + (i * 16), Color(255, 255, 255, 255), g::CourierNew, false, itemname.at(i).c_str());
		}
	}

	g_pSurface->DrawT(ControlsX + 159, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, itemname.at(*item).c_str());

	OldOffsetY = OffsetY;

	if (open)
		OffsetY += 26 + (itemname.size() * 16);
	else
		OffsetY += 26;

	PreviousControl = combo_box;
}

void Menu::MultiComboBox(std::string name, std::vector< std::string > itemname, bool* item)
{
	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	static bool multiPressed = false;
	bool pressed = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;
	std::string itemsSelected = "";
	int lastItem = 0;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 153, OffsetY, 140, 16) && !clickRest)
	{
		nameSelected = name;
		pressed = true;
		clickRest = true;
	}
	else if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 153, OffsetY, 140, 16))
		clickRest = false;

	if (pressed)
	{
		if (!rest)
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if (nameSelected == name)
		open = selectedOpened;

	g_pSurface->DrawT(ControlsX + 6, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, name.c_str());
	g_pSurface->FilledRect(ControlsX + 153, OffsetY, 140, 16,Color(0, 0, 0, 255));
	g_pSurface->OutlinedRect(ControlsX + 153, OffsetY, 140, 16, Color(255, 0, 76, 255));

	if (open)
	{
		g_pSurface->FilledRect(ControlsX + 153, OffsetY, 140, 17 + (itemname.size() * 16),Color(0, 0, 0, 255));
		g_pSurface->OutlinedRect(ControlsX + 153, OffsetY, 140, 17 + (itemname.size() * 16), Color(255, 0, 76, 255));

		for (int i = 0; i < itemname.size(); i++)
		{
			if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 153, OffsetY + 16 + (i * 16), 140, 16))
			{
				if (multiPressed)
					item[i] = !item[i];
				multiPressed = false;
			}

			if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + 153, OffsetY + 16 + (i * 16), 140, 16) && !multiPressed)
				multiPressed = true;

			if (item[i]) {
				g_pSurface->FilledRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16, Color(0, 0, 0, 255));
				g_pSurface->OutlinedRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16, Color(255, 0, 76, 255));
			}


			else
			g_pSurface->FilledRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16,Color(0, 0, 0, 255));
			g_pSurface->OutlinedRect(ControlsX + 154, OffsetY + 16 + (i * 16), 138, 16, Color(255, 0, 76, 255));

			g_pSurface->DrawT(ControlsX + 159, OffsetY + 16 + (i * 16), Color(255, 255, 255, 255), g::CourierNew, false, itemname.at(i).c_str());
		}

	}

	bool items = false;

	// man look at all these for loops i sure am retarded

	for (int i = 0; i < itemname.size(); i++)
	{ 
		if (item[i]) 
		{
			if (lastItem < i)
				lastItem = i;
		}
	}

	for (int i = 0; i < itemname.size(); i++)
	{ 
		if (item[i]) 
		{
			items = true;
			RECT TextSize = g_pSurface->GetTextSizeRect(g::CourierNew, itemsSelected.c_str());
			RECT TextSizeGonaAdd = g_pSurface->GetTextSizeRect(g::CourierNew, itemname.at(i).c_str());
			if (TextSize.right + TextSizeGonaAdd.right < 130)
				itemsSelected += itemname.at(i) + ((lastItem == i) ? "" : ", ");
		}
	}

	if (!items)
		itemsSelected = "off";

	g_pSurface->DrawT(ControlsX + 159, OffsetY, Color(255, 255, 255, 255), g::CourierNew, false, itemsSelected.c_str());

	OldOffsetY = OffsetY;

	if (open)
		OffsetY += 26 + (itemname.size() * 16);
	else
		OffsetY += 26;

	PreviousControl = multi_box;
}

void Menu::ColorPicker(std::string name, ColorV2& item) // best coder in the universe
{
	if (GroupTabBottom <= OffsetY + 16)
		return;

	if (TabOffset - 1 != TabNum || TabOffset == 0)
		return;

	if (SubTabOffset != 0)
		if (SubTabOffset - 1 != SubTabNum)
			return;

	if (PreviousControl == slider || PreviousControl == -1)
		return;

	int CtrXoffset = 0;

	if (PreviousControl != check_box)
		CtrXoffset = 132;
	else
		CtrXoffset = 256;

	int yoffset = OldOffsetY + 10;
	int xoffset = ControlsX + 330;

	Color rainbow;

	bool pressed = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;

	if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + CtrXoffset, OldOffsetY, 16, 16) && !clickRest)
	{
		nameSelected = name;
		pressed = true;
		clickRest = true;
	}
	else if (!GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(ControlsX + CtrXoffset, OldOffsetY, 16, 16))
		clickRest = false;

	if (pressed)
	{
		if (!rest)
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if (nameSelected == name)
		open = selectedOpened;
	
	if (open)
	{
		g_pSurface->FilledRect(xoffset, OldOffsetY, 100, 20,Color(0, 0, 0, 255));
		g_pSurface->FilledRect(xoffset, OldOffsetY + 100, 100, 20,Color(255, 255, 255, 255));

		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(xoffset, OldOffsetY, 100, 10))
		{
			item.red = 0;
			item.green = 0;
			item.blue = 0;
			item.alpha = 255;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(xoffset, OldOffsetY + 110, 100, 10))
		{
			item.red = 255;
			item.green = 255;
			item.blue = 255;
			item.alpha = 255;
		}

		for (int i = 0; i < 100; i++)
		{
			if (xoffset >= ControlsX + 430)
			{
				xoffset -= 100;
				yoffset += 10;
			}

			float hue = (i * .01f);

			rainbow.FromHSV(hue, 1.f, 1.f);

			g_pSurface->FilledRect(xoffset , yoffset, 10, 10, rainbow);

			if (GetAsyncKeyState(VK_LBUTTON) && g_pSurface->MouseInRegion(xoffset, yoffset, 10, 10))
			{
				item.red = rainbow.red;
				item.green = rainbow.green;
				item.blue = rainbow.blue;
				item.alpha = 255.f;
			}			

			xoffset += 10;
		}
	}

	rainbow.red = item.red;
	rainbow.green = item.green;
	rainbow.blue = item.blue;
	rainbow.alpha = 255;

	g_pSurface->FilledRect(ControlsX + CtrXoffset, OldOffsetY, 16, 16,rainbow);
}