#include "oddraw.h"
#include "iddrawsurface.h"
#include "tamem.h"
#include "tafunctions.h"
#include "stdio.h"

int ViewPlayerLos_Replay (int PlayerAryIndex, BOOL HaveControl)
{
	TAdynmemStruct * PTR1 = *(TAdynmemStruct * *)0x511de8;

	PTR1->EyeBallState= PTR1->EyeBallState& (~ FULLLOS);
	//PTR1->LOS_Sight_PlayerID= 0;
	char Curt_LOS_Sight_PlayerID= PTR1->LOS_Sight_PlayerID;

	if (PlayerAryIndex<10
		&&(0!=PTR1->Players[PlayerAryIndex].PlayerActive)
		&& (10!=PTR1->Players[PlayerAryIndex].PlayerAryIndex))
	{//valid player ID
		unsigned char CurtPlayerType= PTR1->Players[PlayerAryIndex].My_PlayerType;

		if ((Player_LocalHuman==CurtPlayerType)
			|| (Player_LocalAI==CurtPlayerType)
			|| (Player_RemoteHuman==CurtPlayerType)
			|| (Player_RemoteAI==CurtPlayerType))
		{
			if (0==PlayerAryIndex)
			{//back to my view
				
				PlayerAryIndex= LocalShare->OrgLocalPlayerID;
				PTR1->LocalHumanPlayer_PlayerID= PlayerAryIndex;
			}
			else
			{
				PTR1->EyeBallState|= FULLLOS;
			}

			Curt_LOS_Sight_PlayerID= PlayerAryIndex;
			//PTR1->LOS_Sight_PlayerID= Curt_LOS_Sight_PlayerID;
			char ChatBuff[0x100];
			memset ( ChatBuff, 0, 100);
			reinterpret_cast<char * *>(ChatBuff)[0]= &ChatBuff[0x10];
			reinterpret_cast<char * *>(ChatBuff)[1]= &ChatBuff[0x20];
			lstrcpyA ( &ChatBuff[0x10], "view");
			
			sprintf_s ( &ChatBuff[0x20], 0x100, "%d", Curt_LOS_Sight_PlayerID);
			ChatBuff[0xd0]= 2;
			ViewCommandProc ( ChatBuff);
			if (HaveControl)
			{
				PTR1->LocalHumanPlayer_PlayerID= Curt_LOS_Sight_PlayerID;
			}
		}
	}


	LoadTARegConfig ( );
	UpdateLosState ( 0);
	//PTR1->LOS_Sight_PlayerID= Curt_LOS_Sight_PlayerID;
	//UpdateLosState ( 0);
	return Curt_LOS_Sight_PlayerID;
}

int UpdateTAProcess (void)
{
	TAdynmemStruct * PTR1 = *(TAdynmemStruct * *)0x511de8;


	if (2==PTR1->State_GUI_CallBack)
	{
		DataShare->TAProgress= TALobby;
	}
	if (5==PTR1->State_GUI_CallBack)
	{
		DataShare->TAProgress= TALoading;
	}
	if (6==PTR1->State_GUI_CallBack)
	{
		DataShare->TAProgress= TAInGame;
	}
	

	return DataShare->TAProgress;
}


BOOL AntiCheat (void)
{
	// sync "+now Film Chris Include Reload Assert"  with cheating

	if (TRUE==*IsCheating)
	{
		(*TAmainStruct_PtrPtr)->SoftwareDebugMode|= 2;
	}
	else
	{
		(*TAmainStruct_PtrPtr)->SoftwareDebugMode&= ! 2;
	}

	return TRUE;
}
LPDWORD GetUnitIDMaskAryByCategory (LPSTR CategoryName_cstrp)
{
	__try
	{// don't trust param and don't trust the way to call TA functions

		__GetUnitIDMaskAryByCategory _GetUnitIDMaskAryByCategory= (__GetUnitIDMaskAryByCategory)0x0488C50;
		return _GetUnitIDMaskAryByCategory ( CategoryName_cstrp);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return NULL;
}

void freeTAMem (LPVOID MemAddress)
{
	__try
	{// don't trust param and don't trust the way to call TA functions

		_free_SafeWay free_SafeWay=	(_free_SafeWay)0x004B4F20;

		free_SafeWay ( MemAddress);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
}

void UpdateSelectUnitEffect (void)
{
	int *PTR = (int*)0x00511de8;
	BYTE temp= *((BYTE *)(*PTR+ 0x37EBE));
	*((BYTE *)(*PTR+ 0x37EBE))= (BYTE)(temp| 0x10);

	//
/*

*/
}

void ApplySelectUnitMenu_Wapper (void)
{
	int *PTR = (int*)0x00511de8;
	*((unsigned short int *)(*PTR+ 0x37E9C))= 0;
	typedef void (__stdcall * _ApplySelectUnitMenu) (void);
	_ApplySelectUnitMenu ApplySelectUnitMenu= (_ApplySelectUnitMenu)0x00495860;
	ApplySelectUnitMenu ();
}
void DeselectUnits(void)
{
	//clean selected units
	TAdynmemStruct *PTR = *((TAdynmemStruct * *)0x00511de8);
	UnitStruct * Start= PTR->OwnUnitBegin;

	while (Start<=(PTR->OwnUnitEnd))
	{
		if (0!=(0x20& Start->UnitSelected))
		{
			if (0.0F==(Start->Nanoframe))
			{
				Start->UnitSelected= (Start->UnitSelected)& (0xFFFFFFEF);
			}
		}

		Start= &Start[1];
	}
	
}


//find OwnUnitBegin under mousepointer
_FindMouseUnit FindMouseUnit = (_FindMouseUnit)0x48CD80;

//fill TAdynmem->MouseMapPosX & TAdynmem->MouseMapPosY first

_TAMapClick TAMapClick = (_TAMapClick)0x498F70;

_TestBuildSpot TestBuildSpot = (_TestBuildSpot)0x4197D0;

//Type - 0 = chat, 1 = popup
_SendText SendText = (_SendText)0x46bc70;

_ShowText ShowText = (_ShowText)0x463E50;

_TADrawRect TADrawRect = (_TADrawRect)0x4BF8C0;

_TADrawLine TADrawLine = (_TADrawLine)0x4CC7AB;				

_GetContext GetContext = (_GetContext)0x4C5E70;

//CirclePointer = CirclePointer in tadynmemstruct
_TADrawCircle TADrawCircle = (_TADrawCircle)0x438EA0;				


_ApplySelectUnitMenu ApplySelectUnitMenu= (_ApplySelectUnitMenu)0x00495860;


_free_SafeWay free_SafeWay=	(_free_SafeWay)0x004B4F20;

_malloc_SafeWay malloc_SafeWay= (_malloc_SafeWay) 0x004B4F10;

_UpdateLOSState UpdateLosState= (_UpdateLOSState) 0x004816A0;
_LoadTARegConfig LoadTARegConfig= (_LoadTARegConfig) 0x00430F00;
_ViewCommandProc ViewCommandProc= (_ViewCommandProc) 0x00416B50;
//////////////////////////////////////////////////////////////////////////////////////////
/// Not working.
//////////////////////////////////////////////////////////////////////////////////////////
_TestGridSpot TestGridSpot = (_TestGridSpot)0x47D2E0;


///--------------------TAMainStruct
TAdynmemStruct * * TAmainStruct_PtrPtr= (TAdynmemStruct * *)0x0511DE8;

DWORD Sfx_mallocBufSizeAddr= 0x00471C87;


_GetIniFileInt GetIniFileInt= (_GetIniFileInt) 0x49F5A0;

LPDWORD AISearchMapEntriesLimit= (LPDWORD) 0x0040EAD6;

LPBYTE AddrAboutCircleSelect= (LPBYTE)0x48C599;
//
_ApplySelectUnitGUI ApplySelectUnitGUI= (_ApplySelectUnitGUI)0x00495860;

LPCSTR TAIniStr=  reinterpret_cast<LPCSTR> (0x5098A3);

LPCSTR CompanyName_CCSTR= (LPCSTR)0x50DDF4;
LPCSTR GameName_CCSTR= (LPCSTR) 0x5032E8;

unsigned int EnterOption_Address = 0x0460160;

unsigned int PressInOption_Address= 0x0045FC60;

unsigned int AddtionInitAddr= 0x0049E909;


unsigned int Blt_BottomState0_TextRtn= 0x004C16D9 ;
unsigned int Blt_BottomState0_TextAddr= 0x004C16D4 ;
unsigned int Blt_BottomState1_TextRtn= 0x004C1749 ;
unsigned int Blt_BottomState1_TextAddr= 0x004C1744 ;

unsigned int GetTextExtent_AssignCharLenAddr= 0x004C1480;
unsigned int GetStrExtentAddr= 0x004A5030;

unsigned int PopadStateAddr= 0x0468AC0;


unsigned int Addr_0049E91C= 0x0049E91C;
unsigned int Addr_0049E93B= 0x0049E93B;
unsigned int Addr_00491A75= 0x00491A75;
unsigned int Addr_00491B01= 0x00491B01;
unsigned int Addr_0049802B= 0x0049802B;
unsigned int Addr_004980AD= 0x004980AD;


unsigned int TADontInit_Addr= 0x4B5AB6;

unsigned int MPUnitLimitAddr= 0x0044CAFE;
unsigned int UnitLimit0Addr= 0x491640;
unsigned int UnitLimit1Addr=0x491659;
unsigned int UnitLimit2Addr=0x491666;

BOOL * IsCheating= reinterpret_cast<BOOL *>(0x5091CC);

WNDPROC TAWndProc_Addr= (WNDPROC )0x4B5CC0;

unsigned int TAWndProcSH_Addr=0x4B5B82;