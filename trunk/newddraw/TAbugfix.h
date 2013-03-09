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
public:
	TABugFixing ();
	~TABugFixing ();
	BOOL AntiCheat (void);
};

extern TABugFixing * FixTABug;;

int __stdcall BadModelHunter (PInlineX86StackBuffer X86StrackBuffer);