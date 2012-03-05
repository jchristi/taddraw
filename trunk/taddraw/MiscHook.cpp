#include "oddraw.h"
#include "hook/etc.h"
#include "hook/hook.h"
#include "tamem.h"
#include "tafunctions.h"

#include "MixingBuffers.h"
#include "Switchalt.h"

LONG RegWriteDword ( HKEY RootKey, LPTSTR lpSubKey, LPTSTR lpValueName, DWORD Data)
{
	HKEY hKey;
	DWORD dwDisposition;
	DWORD Size = sizeof(int);

	RegCreateKeyEx ( RootKey, lpSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	RegSetValueEx ( hKey, lpValueName, 0, REG_DWORD, (CONST BYTE *)&Data, 4);
	return RegCloseKey(hKey);
}

IncreaseMixingBuffers::IncreaseMixingBuffers (DWORD num)
{
	RegWriteDword ( HKEY_CURRENT_USER, "Software\\Cavedog Entertainment\\Total Annihilation", "MixingBuffers", num);
}

Switchalt::Switchalt (int IsSet)
{
	if (1==IsSet)
	{
		RegWriteDword ( HKEY_CURRENT_USER, "Software\\Cavedog Entertainment\\Total Annihilation", "SwitchAlt", 1);
	}
	else if (0==IsSet)
	{
		RegWriteDword ( HKEY_CURRENT_USER, "Software\\Cavedog Entertainment\\Total Annihilation", "SwitchAlt", 0);
	}
}