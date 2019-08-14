#include <thread>
#include "Hooks.h"
#include "Utils\Utils.h"
#include "Utils\GlobalVars.h"

HINSTANCE HThisModule;
UCHAR szFileSys[255], szVolNameBuff[255];
DWORD dwMFL, dwSysFlags;
DWORD dwSerial;
LPCTSTR szHD = "C:\\";
int OnDllAttach()
{
    Hooks::Init();

	while (true)
		Sleep(10000000);

	Hooks::Restore();

	FreeLibraryAndExitThread(HThisModule, 0);
}


#define rias 1047255890
#define stapz 572397450
#define gumse -186730217
#define crack 353950379 
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH && GetModuleHandleA("csgo.exe"))
	{
		GetVolumeInformation(szHD, (LPTSTR)szVolNameBuff, 255, &dwSerial, &dwMFL, &dwSysFlags, (LPTSTR)szFileSys, 255);

		if (dwSerial == rias ||
			dwSerial == stapz ||
			dwSerial == gumse ||
			dwSerial == crack)
		{
			Sleep(100);
		}
		else
		{
			// when HWID rejected
		MessageBox(NULL, "HWID DOES NOT MATCH", "riashwidsystem", MB_OK);
			exit(0);
			return TRUE;
		}

		{
			{
				DisableThreadLibraryCalls(hModule);

				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)OnDllAttach, NULL, NULL, NULL);

				return TRUE;
			}
		}
		return FALSE;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	}
	return TRUE;
}

