/*
#include "oddraw.h"
#include "hook/etc.h"
#include "hook/hook.h"
#include "tamem.h"
#include "tafunctions.h"

#include "MixingBuffers.h"
#include "Switchalt.h"
#include "hook\etc.h"



IncreaseMixingBuffers::IncreaseMixingBuffers (DWORD num)
{
	RegWriteDword ( HKEY_CURRENT_USER, "Software\\Cavedog Entertainment\\Total Annihilation", "MixingBuffers", num);
}

Switchalt::Switchalt (BOOL IsSet)
{
	if (IsSet)
	{
		RegWriteDword ( HKEY_CURRENT_USER, "Software\\Cavedog Entertainment\\Total Annihilation", "SwitchAlt", 1);
	}
	else if (IsSet)
	{
		RegWriteDword ( HKEY_CURRENT_USER, "Software\\Cavedog Entertainment\\Total Annihilation", "SwitchAlt", 0);
	}
}*/