#include "stdafx.h"
#include <vector>

typedef std::vector<wchar_t *> STRINGVECTOR;

static void LoadWallpaperRepository(STRINGVECTOR *wallpaperRepository)
{
	__try
	{
		HKEY hKey;

		if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
			L"Software\\OEM\\Wallpapers",
			0,
			KEY_READ,
			&hKey) == ERROR_SUCCESS)
		{
			wchar_t keyName[500];
			DWORD keyNameSize = 50;
			DWORD dwIndex = 0;

			DWORD dwType = REG_NONE;
			while (RegEnumValue(hKey, 
				dwIndex++, 
				keyName, 
				&keyNameSize, 
				NULL, 
				&dwType, 
				NULL, 
				NULL) == ERROR_SUCCESS)
			{
				if (dwType != REG_NONE)
				{
					wchar_t *text = new wchar_t[wcslen(keyName) + 1];
					wcscpy(text, keyName);
					wallpaperRepository->push_back(text);
				}
				keyNameSize = 500;
			}
			RegCloseKey(hKey);
		}
		
	}
	__except (1)
	{

	}
}



void TransferWallpapers()
{
	STRINGVECTOR wallpaperRepository;

	LoadWallpaperRepository(&wallpaperRepository);
	int paramLength = 1;
	int size = wallpaperRepository.size();
	for (int i = 0; i < size; ++i)
	{
		wchar_t *str = wallpaperRepository[i];
		paramLength += wcslen(str) + 1;
	}

	if (paramLength > 1)
	{
		wchar_t *param = new wchar_t[paramLength];
		memset(param, 0, paramLength * sizeof(wchar_t));
		wchar_t *tmp = param;
		for (int i = 0; i < size; ++i)
		{
			wchar_t *str = wallpaperRepository[i];
			memcpy(tmp, str, (wcslen(str) + 1) * sizeof(wchar_t));	
			tmp += wcslen(str) + 1;
		}

		HKEY hKey = NULL;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Shell\\WallpaperSets", 0, 0, &hKey);
		if (hKey)
		{
			RegSetValueEx(hKey, L"OEMWallpapers", 0, REG_MULTI_SZ, (LPBYTE)param, paramLength * sizeof(wchar_t));
			RegCloseKey(hKey);
		}
		delete[] param;
	}
	for (STRINGVECTOR::iterator iterator = wallpaperRepository.begin(); iterator != wallpaperRepository.end(); ++iterator)
	{
		delete(*iterator);
	}
	wallpaperRepository.erase(wallpaperRepository.begin(), wallpaperRepository.end());
}