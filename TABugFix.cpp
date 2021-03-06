
#include "iddraw.h"
#include "iddrawsurface.h"


#include "hook/etc.h"
#include "hook/hook.h"

#include "tamem.h"
#include "tafunctions.h"
#include "TAbugfix.h"


#include "ddraw.h"


TABugFixing * FixTABug;
///////---------------------
/*
.text:004866E8 078 75 04                                                           jnz     short loc_4866EE
	.text:004866EA 078 33 F6                                                           xor     esi, esi
	.text:004866EC 078 EB 18                                                           jmp     short loc_486706
	-> if it's null, straight jmp to across the routine that used esi as unit ptr
	*/

unsigned int NullUnitDeathVictimAddr= 0x04866E8;
BYTE NullUnitDeathVictimBits[]={0x0F, 0x84, 0x6B, 0x07, 0x00, 0x00};

//.text:00438EDE 03C 0F 8C 69 01 00 00                                               jl      loc_43904D
//->    jle      loc_43904D  Radius most bigger than 0
unsigned int CircleRadiusAddr= 0x00438EDE;
BYTE CircleRadiusBits[]= {0x0F, 0x8E, 0x69, 0x01, 0x00, 0x00};

unsigned int CrackCdAddr= 0x0041D6B0;

BYTE CrackCDBits[]= {0x74, 0x0E, 0xB0, 0x2E};
unsigned int GUIErrorLengthAry[GUIERRORCOUNT]=
{
	0x04AEBBE,
	0x04AEBCA,
	0x04AEC2C,
	0x04AEC87
};
BYTE GUIErrorLengthBits[]= {0x80};

unsigned int CDMusic_TABAddr= 0x00460E0D;
BYTE CDMusic_TABBits[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};

unsigned int CDMusic_Menu_PauseAddr= 0x00490B30;
unsigned int CDMusic_Victory_PauseAddr= 0x4996DF;


unsigned int CDMusic_StopAddr= 0x4CED4b;
BYTE CDMusic_StopBits[]= {0x44, 0xb6, 0x50};


TABugFixing::TABugFixing ()
{
	NullUnitDeathVictim= new SingleHook ( NullUnitDeathVictimAddr, sizeof(NullUnitDeathVictimBits), INLINE_UNPROTECTEVINMENT, NullUnitDeathVictimBits);

	CircleRadius=  new SingleHook ( CircleRadiusAddr, sizeof(CircleRadiusBits), INLINE_UNPROTECTEVINMENT, CircleRadiusBits);

	BadModelHunter_ISH= new InlineSingleHook ( BadModelHunterAddr, 5, INLINE_5BYTESLAGGERJMP, BadModelHunter);

	CrackCd= new SingleHook ( CrackCdAddr, sizeof(CrackCDBits), INLINE_UNPROTECTEVINMENT, CrackCDBits);
	

	for (int i= 0; i<GUIERRORCOUNT; i++)
	{
		GUIErrorLengthHookAry[i]= new SingleHook ( GUIErrorLengthAry[i], sizeof(GUIErrorLengthBits), INLINE_UNPROTECTEVINMENT, GUIErrorLengthBits);
	}

	HMODULE Audiere_hm= GetModuleHandleA ( "audiere.dll");
	CDMusic_TAB= NULL;
	CDMusic_Menu_Pause= NULL;
	CDMusic_Victory_Pause= NULL;
	CDMusic_StopButton= NULL;
	if (NULL!=Audiere_hm)
	{// install music cd hook

		CDMusic_TAB= new SingleHook ( CDMusic_TABAddr, sizeof(CDMusic_TABBits), INLINE_UNPROTECTEVINMENT, CDMusic_TABBits);
		CDMusic_Menu_Pause= new InlineSingleHook ( CDMusic_Menu_PauseAddr, 5, INLINE_5BYTESLAGGERJMP, CDMusic_MenuProc);
		CDMusic_Victory_Pause= new InlineSingleHook ( CDMusic_Victory_PauseAddr, 5, INLINE_5BYTESLAGGERJMP, CDMusic_VictoryProc);
		//CDMusic_StopButton= new  SingleHook ( CDMusic_StopAddr  , sizeof(CDMusic_StopBits), INLINE_UNPROTECTEVINMENT, CDMusic_StopBits); 
		
	}
}

TABugFixing::~TABugFixing ()
{
	if (NULL!=NullUnitDeathVictim)
	{
		delete NullUnitDeathVictim;
	}
	if (NULL!=BadModelHunter_ISH)
	{
		delete BadModelHunter_ISH;
	}
	
	if (NULL!=CrackCd)
	{
		delete CrackCd;
	}
	if (NULL!=CircleRadius)
	{
		delete CircleRadius;
	}
	for (int i= 0; i<GUIERRORCOUNT; i++)
	{
		if (NULL!=GUIErrorLengthHookAry[i])
		{
			delete GUIErrorLengthHookAry[i];
		}
		
	}

	if (CDMusic_TAB)
	{
		delete CDMusic_TAB;
	}
	if (CDMusic_Menu_Pause)
	{
		delete CDMusic_Menu_Pause;
	}
	if (CDMusic_Victory_Pause)
	{
		delete CDMusic_Victory_Pause;
	}
	if (CDMusic_StopButton)
	{
		delete CDMusic_StopButton;
	}
	
}

BOOL TABugFixing::AntiCheat (void)
{
	// sync "+now Film Chris Include Reload Assert"  with cheating

	if (TRUE==*IsCheating)
	{
		(*TAmainStruct_PtrPtr)->SoftwareDebugMode|= 2;
	}
	else
	{
		(*TAmainStruct_PtrPtr)->SoftwareDebugMode= ((*TAmainStruct_PtrPtr)->SoftwareDebugMode)& (~ 2);
	}

	return TRUE;
}



void LogToErrorlog (LPSTR Str)
{
	HANDLE file = CreateFileA("ErrorLog.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS	, 0, NULL);
	DWORD tempWritten;
	SetFilePointer ( file, 0, 0, FILE_END);
	WriteFile ( file, Str, strlen(Str), &tempWritten, NULL);
	WriteFile ( file, "\r\n", 2, &tempWritten, NULL);

	CloseHandle ( file);
}
/*
.text:00458C5A 078 C1 E9 02                                                        shr     ecx, 2          ; let's add a check in here, if  ecx is bigger than 600*600
	.text:00458C5D 078 F3 AB                                                           rep stosd               ; init the CompositeBuffer as background*/

int __stdcall BadModelHunter (PInlineX86StackBuffer X86StrackBuffer)
{
	OFFSCREEN * Offscreen_p= (OFFSCREEN *)(X86StrackBuffer->Esp+ 0x48);
	if ((600<(Offscreen_p->Width))
		||(600<(Offscreen_p->Height)))
	{// record thsi shit.
		Object3doStruct * Obj_ptr=  *(Object3doStruct * *)(X86StrackBuffer->Esp+ 0x68+ 0x10+ 0x8);

		LogToErrorlog ( "Bad Model Which Crash Game In Random!!!!!");
		LogToErrorlog ( Obj_ptr->ThisUnit->UnitType->Name);
		LogToErrorlog ( Obj_ptr->ThisUnit->UnitType->UnitName);
		LogToErrorlog ( Obj_ptr->ThisUnit->UnitType->UnitDescription);
		LogToErrorlog ( Obj_ptr->ThisUnit->UnitType->ObjectName);
		SendText ( "Plz Send Errorlog.txt(In Your TA Path) And The Replay Tad To XPoy(In TAU Or In TAclub).\r\nMan, You Meet A Bad Crash Cause By Unit. \r\nOr If You Are A Modder, You Can Know Which Unit Had Model Problem In End Of The Errorlog.txt", 1);

		X86StrackBuffer->rtnAddr_Pvoid= (LPVOID)SafeModelAddr;

		return X86STRACKBUFFERCHANGE;
		//rtn to 	
	}
	return 0;
}

int __stdcall CDMusic_MenuProc (PInlineX86StackBuffer X86StrackBuffer)
{
	if (1==*(DWORD * )(X86StrackBuffer->Esp+ 4))
	{
		PauseCDMusic ( );
	}

	return 0;
}

int __stdcall CDMusic_VictoryProc (PInlineX86StackBuffer X86StrackBuffer)
{
	PauseCDMusic ( );
	return 0;
}