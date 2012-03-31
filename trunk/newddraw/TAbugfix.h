#pragma once

class SingleHook;

class TABugFixing
{
private:
	SingleHook * NullUnitDeathVictim;
	SingleHook * CircleRadius;
	SingleHook * CrackCd;
	InlineSingleHook * BadModelHunter_ISH;
public:
	TABugFixing ();
	~TABugFixing ();
	BOOL AntiCheat (void);
};

extern TABugFixing * FixTABug;;

int __stdcall BadModelHunter (PInlineX86StackBuffer X86StrackBuffer);