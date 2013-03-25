#pragma once

class SingleHook;
#define GUIERRORCOUNT (4)

class TABugFixing
{
private:
	SingleHook * NullUnitDeathVictim;
	SingleHook * CircleRadius;
	SingleHook * CrackCd;
	InlineSingleHook * BadModelHunter_ISH;

	SingleHook * GUIErrorLengthHookAry[GUIERRORCOUNT];

	SingleHook * CDMusic_TAB;

	InlineSingleHook * CDMusic_Menu_Pause;
	InlineSingleHook * CDMusic_Victory_Pause;

public:
	TABugFixing ();
	~TABugFixing ();
	BOOL AntiCheat (void);
};

extern TABugFixing * FixTABug;;

int __stdcall BadModelHunter (PInlineX86StackBuffer X86StrackBuffer);

int __stdcall CDMusic_VictoryProc (PInlineX86StackBuffer X86StrackBuffer);
int __stdcall CDMusic_MenuProc (PInlineX86StackBuffer X86StrackBuffer);