// uAppBootWorker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AppVisibility.h"
#include "Wallpapers.h"
#include "TilePosition.h"
#include "regext.h"
#include "..\..\common\Process.hpp"
#include "..\..\common\Tiles.hpp"

static ULONG thread(LPVOID pParam)
{
	__try
	{
		ProcessTilePositions();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return 0;
}

static void OemPrepinnedCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData)
{
	DWORD dwValue = 0, dwValue2 = 0;
	RegistryGetDWORD(HKEY_LOCAL_MACHINE, L"System\\OEMPrePinned", L"PrePinCompleted", &dwValue);
	RegistryGetDWORD(HKEY_LOCAL_MACHINE, L"System\\Shell\\OOBE", L"Done", &dwValue2);
	if (dwValue && dwValue2)
	{
		HANDLE hThread = CreateThread(NULL, 0, thread, 0, 0, NULL);
		WaitForSingleObject(hThread, 10000);
		Sleep(1000);
		hThread = CreateThread(NULL, 0, thread, 0, 0, NULL);
		WaitForSingleObject(hThread, 10000);

		Diagnostics::Process::Start(L"\\Windows\\HqTokens.exe", L"");

		keybd_event(VK_ESCAPE, 0, KEYEVENTF_SILENT, 0);
		Sleep(100);
		keybd_event(VK_ESCAPE, 0, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, 0);
		PostQuitMessage(0);
	}
}

class AutoCallbackRegistration
{
public:
	AutoCallbackRegistration()
	{
		hRegNotifies[0] = 0;
		hRegNotifies[1] = 0;
		RegistryNotifyCallback(HKEY_LOCAL_MACHINE, L"System\\OEMPrePinned", L"PrePinCompleted", OemPrepinnedCallback, 0, NULL, &hRegNotifies[0]);
		RegistryNotifyCallback(HKEY_LOCAL_MACHINE, L"System\\Shell\\OOBE", L"Done", OemPrepinnedCallback, 0, NULL, &hRegNotifies[1]);
	}

	~AutoCallbackRegistration()
	{
		RegistryCloseNotification(hRegNotifies[0]);
		RegistryCloseNotification(hRegNotifies[1]);
	}

private:
	
	HREGNOTIFY hRegNotifies[2];
};

static AutoCallbackRegistration _callbackRegistration;

int _tmain(int argc, _TCHAR* argv[])
{
	ApplyAppVisibility();
	TransferWallpapers();

	OemPrepinnedCallback(NULL, 0, NULL, 0);
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

