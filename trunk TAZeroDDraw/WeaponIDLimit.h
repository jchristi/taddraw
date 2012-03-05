#pragma once

#define PTRARYNUM (15)
#define LENARYNUM (3)
class IncreaseWeaponTypeLimit
{
public:
	LPVOID CurtPtr;
	DWORD CurtLen;

	SingleHook * PtrHookAry[PTRARYNUM];
	SingleHook * LenHookAry[LENARYNUM];

private:
	LPVOID OrgPtr;
	DWORD OrglLen;

public:
	IncreaseWeaponTypeLimit ();
	IncreaseWeaponTypeLimit (DWORD NewLen);
	~IncreaseWeaponTypeLimit ();

private:
	LPVOID WriteWeaponAryPtr ( LPVOID * NewPtr);
	DWORD WriteWeaponAryLen ( DWORD Newlen);
	void NewLimit (DWORD NewLen);
};

extern IncreaseWeaponTypeLimit * NowIncreaseWeaponTypeLimit;