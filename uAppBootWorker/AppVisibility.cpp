#include "stdafx.h"
#include "..\..\common\pacmanclient.h"

void ApplyAppVisibility()
{
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"Software\\OEM\\FirstBoot\\HideApps",
		0,
		KEY_READ,
		&hKey) == ERROR_SUCCESS)
	{
		wchar_t keyName[500];
		DWORD keyNameSize = 500;

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
				GUID guid;
				Shell::Applications::String2GUID(keyName, &guid);
				PMSetApplicationVisibility(guid, 0);
			}
			keyNameSize = 500;
		}
		RegCloseKey(hKey);
	}

}