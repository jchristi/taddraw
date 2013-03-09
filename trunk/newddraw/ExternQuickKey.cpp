//扩展的快捷键

#include "oddraw.h"
#include "iddrawsurface.h"
#include "taunitdrawer.h"
#include "tamem.h"
#include "tafunctions.h"
#include "hook\hook.h"
#include "hook\etc.h"
#include "ExternQuickKey.h"
#include "WeaponIDLimit.h"
#include "tahook.h"
#include <vector>
using namespace std;
#include "TAConfig.h"





ExternQuickKey * myExternQuickKey;

///////--------------

ExternQuickKey::ExternQuickKey ()
{
	TAMainStruct_Ptr= * TAmainStruct_PtrPtr;

	//AddtionInit ( );
	Semaphore_OnlyInScreenSameType= CreateSemaphore ( NULL, 1, 1, NULL);
	Semaphore_FilterSelect= CreateSemaphore ( NULL, 1, 1, NULL);
	Semaphore_OnlyInScreenWeapon= CreateSemaphore ( NULL, 1, 1, NULL);
	Semaphore_IdleCons= CreateSemaphore ( NULL, 1, 1, NULL);
	Semaphore_IdleFac=  CreateSemaphore ( NULL, 1, 1, NULL);

	DoubleClick= MyConfig->GetIniBool ( "DoubleClick", TRUE);
	//TAMainStruct_Ptr->inter
	HookInCircleSelect= new InlineSingleHook ( (unsigned int)AddrAboutCircleSelect, 5, 
		INLINE_5BYTESLAGGERJMP, AddtionRoutine_CircleSelect);

	HKEY hKey;
	HKEY hKey1;
	DWORD dwDisposition;
	DWORD Size;

	RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\TA Patch", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

	RegCreateKeyEx(hKey, "Eye", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey1, &dwDisposition);

	Size = sizeof(int);
	if(RegQueryValueEx(hKey1, "KeyCode", NULL, NULL, (unsigned char*)&VirtualKeyCode, &Size) != ERROR_SUCCESS)
	{
		VirtualKeyCode = 88;
	}

	RegCloseKey ( hKey);
	RegCloseKey ( hKey1);

	Add = (char*)0x41ac14;
	Sub = (char*)0x41ac18;

	OldAdd = *Add;
	OldSub = *Sub;
	NumAdd = 100;
	NumSub = -100;

	IDDrawSurface::OutptTxt ( "New ExternQuickKey");
}


ExternQuickKey::~ExternQuickKey ()
{
	//AddtionRelease ( );
	if (NULL!=Semaphore_OnlyInScreenSameType)
	{
		CloseHandle ( Semaphore_OnlyInScreenSameType);
	}
	if (NULL!=Semaphore_FilterSelect)
	{
		CloseHandle ( Semaphore_FilterSelect);
	}
	if (NULL!=Semaphore_OnlyInScreenWeapon)
	{
		CloseHandle ( Semaphore_OnlyInScreenWeapon);
	}

	if (NULL!=Semaphore_IdleCons)
	{
		CloseHandle ( Semaphore_IdleCons);
	}
	if (NULL!=Semaphore_IdleFac)
	{
		CloseHandle ( Semaphore_IdleFac);
	}

	
	if (NULL!=HookInCircleSelect)
	{
		delete HookInCircleSelect;
	}
	
	WriteProcessMemory ( GetCurrentProcess(), (void*)Add, &OldAdd, 1, NULL);
	WriteProcessMemory ( GetCurrentProcess(), (void*)Sub, &OldSub, 1, NULL);
}

bool ExternQuickKey::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (TAInGame!=DataShare->TAProgress)
	{
		return false;
	}
	__try
	{
		switch (Msg)
		{
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:

			if (! DoubleClick)
			{
				break;
			}
/*
			if (0==(TAMainStruct_Ptr->InterfaceType))
			{
				if (Msg==WM_RBUTTONDBLCLK)
				{
					break;
				}
			}
			else
			{
				if (Msg==WM_LBUTTONDBLCLK)
				{
					break;
				}
			}*/

			if ((GetAsyncKeyState(VirtualKeyCode)&0x8000)==0)
			{// don't catch the msg when whiteboard key down.
				int xPos;
				xPos= LOWORD(lParam);
				int yPos;
				yPos = HIWORD(lParam);
				if ((xPos>(TAMainStruct_Ptr->GameSreen_Rect.left))&&(xPos<(TAMainStruct_Ptr->GameSreen_Rect.right))&&(yPos>(TAMainStruct_Ptr->GameSreen_Rect.top))&&(yPos<(TAMainStruct_Ptr->GameSreen_Rect.bottom)))
				{
					if (0!=TAMainStruct_Ptr->MouseOverUnit)
					{
						if (TAMainStruct_Ptr->Players[TAMainStruct_Ptr->LocalHumanPlayer_PlayerID].PlayerAryIndex==(TAMainStruct_Ptr->OwnUnitBegin[TAMainStruct_Ptr->MouseOverUnit].Owner_PlayerPtr0->PlayerAryIndex))
						{
							SelectOnlyInScreenSameTypeUnit ( FALSE);
							UpdateSelectUnitEffect ( ) ;
							ApplySelectUnitMenu_Wapper ( );
							return true;
						}
					}
				}
			}
			break;
		case WM_KEYDOWN:
			if ((int)wParam==17)
			{
				WriteProcessMemory(GetCurrentProcess(), (void*)Add, &NumAdd, 1, NULL);
				WriteProcessMemory(GetCurrentProcess(), (void*)Sub, &NumSub, 1, NULL);
				//break;
			}

			if (0x53==(int)wParam)
			{//ctrl+s
				if ((GetAsyncKeyState(VK_CONTROL)&0x8000)>0 && (GetAsyncKeyState(VK_SHIFT)&0x8000)==0)
				{

					SelectOnlyInScreenWeaponUnit ( MOVEUNITSELECTABLE);
					UpdateSelectUnitEffect ( ) ;
					ApplySelectUnitMenu_Wapper ( );
					return true;
				}
			}
			if(wParam == 66 && (GetAsyncKeyState(17)&0x8000)>0 && (GetAsyncKeyState(16)&0x8000)==0) 
			{// ctrl + b
				DeselectUnits ();
				FindIdleConst();
				UpdateSelectUnitEffect ( ) ;
				ApplySelectUnitMenu_Wapper ( );
				return true;
			}
			if(wParam == 70  && (GetAsyncKeyState(17)&0x8000)>0 && (GetAsyncKeyState(16)&0x8000)==0) 
			{// ctrl + f
				DeselectUnits ();
				FindIdelFactory ();
				UpdateSelectUnitEffect ( ) ;
				ApplySelectUnitMenu_Wapper ( );
				return true;
			}
			
			break;
		case WM_KEYUP:
			switch((int)wParam)
			{
			case 17:
				WriteProcessMemory(GetCurrentProcess(), (void*)Add, &OldAdd, 1, NULL);
				WriteProcessMemory(GetCurrentProcess(), (void*)Sub, &OldSub, 1, NULL);
				//break;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}

	return false;
}

void ExternQuickKey::FindIdelFactory ()
{
	DWORD Wait_rtn= WaitForSingleObject ( Semaphore_IdleFac, INFINITE);
	if (WAIT_FAILED==Wait_rtn)
	{
		return ;
	}

	if (WAIT_TIMEOUT==Wait_rtn)
	{
		ReleaseSemaphore ( Semaphore_IdleFac, 1, NULL);
		return ;
	}

	static int LastNum = 0;
reTry:
	TAdynmemStruct *PTR = TAMainStruct_Ptr;

	UnitStruct *  Begin=  PTR->OwnUnitBegin;
/*	UnitStruct *  End= PTR->Players[PTR->LocalHumanPlayer_PlayerID].UnitsAry_End;*/
	UnitStruct *  Current= Begin;


	int j= LastNum;
	int MyMaxUnit= PTR->Players[PTR->LocalHumanPlayer_PlayerID].UnitsNumber;

	LPDWORD FactoryAry= NULL;

	FactoryAry= GetUnitIDMaskAryByCategory ( "CTRL_F");

	while (j<=MyMaxUnit)
	{

		Current= &(PTR->Players[PTR->LocalHumanPlayer_PlayerID].Units[j]);

		if ((0!=((UnitValid2_State)& Current->UnitSelected)))
		{
			if (0.0F==(Current->Nanoframe))
			{
				//ID= ;
				if ((NULL!=Current->Owner_PlayerPtr1))//&&(Player_LocalHuman==Current->ValidOwner_PlayerPtr->My_PlayerType))
				{
					if (MatchInTypeAry ( Current->UnitID, FactoryAry))
					{
						if (NULL!=Current->UnitOrders)
						{
							char UnitState= Current->UnitOrders->COBHandler_index;
							if((UnitState==0xc)) //not idle
							{
								++j;
								continue;
							}
						}
						//i= i+ 1;
						if (LastNum<j)
						{
							LastNum = j;
							break;
						}
					}
				}
			}
		}
		//Current= &Current[1];
		++j;
	}
	if (j<=MyMaxUnit)
	{
		//founded!
		Current->UnitSelected|=  UnitSelected_State;

		ScrollToCenter( Current->XPos,  Current->YPos);
	}
	else
	{
		// not found once
		if (0!=LastNum)
		{
			LastNum= 0;
			goto reTry;
		}
	}
	//really not found 


	ReleaseSemaphore ( Semaphore_IdleFac, 1, NULL);
	return ;
}

void ExternQuickKey::FindIdleConst()
{
	//FixAck();
	//

	IDDrawSurface::OutptTxt ( "Search Idle Const");

	DWORD Wait_rtn= WaitForSingleObject ( Semaphore_IdleCons, INFINITE);
	if (WAIT_FAILED==Wait_rtn)
	{
		return ;
	}
		
	if (WAIT_TIMEOUT==Wait_rtn)
	{
		goto ReleaseIdleConsSemaphore;
	}

	static int LastNum = 0;

	TAdynmemStruct *PTR = TAMainStruct_Ptr;
	UnitStruct * Start;// = PTR->OwnUnitBegin;//*(UnitStruct * *)((*PTR)+0x14357); 
	//UnitStruct * End= (UnitStruct *)((*PTR)+0x1435B);
	int i;
	int MyMaxUnit= PTR->Players[PTR->LocalHumanPlayer_PlayerID].UnitsNumber;
	LPDWORD CommandMaskAry= NULL;

	CommandMaskAry= GetUnitIDMaskAryByCategory ( "Commander");
/*
	if (LocalShare->OrgLocalPlayerID!=PTR->LocalHumanPlayer_PlayerID)
	{// do not support this on 

		return ;
	}
	*/

	if (MyMaxUnit<LastNum)
	{
		LastNum= 0;
	}
	i= LastNum;

	while (i<=MyMaxUnit)
	{
		Start= &(PTR->Players[PTR->LocalHumanPlayer_PlayerID].Units[i]);

		char *UnitDead = (char*)(&Start->HealthPerB);
		char *Builder = (char*)(&Start->Builder);  //16 == no weapon
		short *XPos = (short*)(&Start->XPos);
		short *YPos = (short*)(&Start->YPos);
		int *IsUnit = (int*)(&Start->IsUnit);
		char *UnitSelected = (char*)(&Start->UnitSelected);

		int *UnitOrderPTR = (int*)(&Start->UnitOrders);

		if(*UnitDead!=0 && *UnitDead!=1)
		{
			if(*IsUnit)
			{
				//check if workertime > 0
				UnitDefStruct *DefiPTR = (Start->UnitType);
				unsigned short *WorkerTime = &(DefiPTR->nWorkerTime);

				if ((NULL!=CommandMaskAry)
					&&(!(MatchInTypeAry ( Start->UnitID, CommandMaskAry))))
				{// not comm
					if(*WorkerTime>0)//有生产能力
					{
						char *UnitState = (char*)(*UnitOrderPTR + 4);
					
						if((NULL==*UnitOrderPTR)
							||(*UnitState==41 || *UnitState==64)) //idle
						{
							if  (LastNum<i)
							{
								LastNum = i;

								*UnitSelected=  *UnitSelected| UnitSelected_State;

								ScrollToCenter(*XPos, *YPos);
								goto ReleaseIdleConsSemaphore;
							}

						}
					}
				}
			}
		}
		++i;

		if(MyMaxUnit<i)
		{
			if(LastNum == 0) //no units found and all units searched. this is 2rd time enter FindIdleConst;
			{
				goto ReleaseIdleConsSemaphore;
			}
			break;
		}
		//Start= &Start[1];
	}

	//search from the beginning, cause last num be reset at this case
	LastNum = 0;

	ReleaseSemaphore ( Semaphore_IdleCons, 1, NULL);
	FindIdleConst();
	return;
ReleaseIdleConsSemaphore:
	ReleaseSemaphore ( Semaphore_IdleCons, 1, NULL);
	return;
}

void ExternQuickKey::ScrollToCenter(int x, int y)
{
	int *PTR = (int*)TAmainStruct_PtrPtr;
	int *XPointer = (int*)(*PTR + 0x1431f);
	int *YPointer = (int*)(*PTR + 0x14323);

	x -= (((*TAProgramStruct_PtrPtr)->ScreenWidth)-128)/2;
	y -= (((*TAProgramStruct_PtrPtr)->ScreenHeight)-64)/2;

	if(x<0)
		x = 0;
	if(y<0)
		y = 0;
	if(x>CTAHook::GetMaxScrollX())
		x = CTAHook::GetMaxScrollX();
	if(y>CTAHook::GetMaxScrollY())
		y = CTAHook::GetMaxScrollY();

	//*XPointer = x;
	*(XPointer + 2) = x;
	//*YPointer = y;
	*(YPointer + 2)= y;
}

int ExternQuickKey::SelectOnlyInScreenSameTypeUnit (BOOL FirstSelect_Flag)
{// FirstSelect mean if already selected  in screen same type units, then will select all same units ever not in screen
//  return Selected units number
	WaitForSingleObject ( Semaphore_OnlyInScreenSameType, INFINITE);
	DWORD SelectedUnitTypeIDAry_Dw[0x100];
	memset ( SelectedUnitTypeIDAry_Dw, 0, sizeof(SelectedUnitTypeIDAry_Dw));

	WORD ID= 0;
	TAdynmemStruct *PTR = TAMainStruct_Ptr;
	UnitStruct * Begin= PTR->OwnUnitBegin;
	UnitStruct * End= PTR->OwnUnitEnd;

	while (Begin<=End)
	{
		if (0!=((Begin->UnitSelected)& 0x10))
		{//this one are Selected OwnUnitBegin
			SetIDMaskInTypeAry ( Begin->UnitID, SelectedUnitTypeIDAry_Dw);
		}
		Begin= &Begin[1];
	}
	DeselectUnits();
	// now we got all selected OwnUnitBegin type
	Begin= PTR->OwnUnitBegin;
	UnitStruct *  Current;
	int MaxHotUnitCount_I= PTR->NumHotUnits;
	int Counter= 0;
	short int * HotUnitAry_Ptr= PTR->HotUnits;
	int SelectedUnits= 0;
	while (Counter<MaxHotUnitCount_I)
	{
		Current= &(Begin[HotUnitAry_Ptr[Counter]]);

		if (0!=(0x20& Current->UnitSelected))
		{
			if (0.0F==(Current->Nanoframe))
			{
				//ID= ;
				if (MatchInTypeAry ( Current->UnitID, SelectedUnitTypeIDAry_Dw))
				{
					if ((NULL!=Current->Owner_PlayerPtr1))
					{
						if (TAMainStruct_Ptr->LocalHumanPlayer_PlayerID==Current->Owner_PlayerPtr1->PlayerAryIndex)
						{
							Current->UnitSelected= (Current->UnitSelected)| (0x10);
							++SelectedUnits;
						}

					}
				}
			}
		}
		++Counter;
	}

	UpdateSelectUnitEffect ( ) ;
	ApplySelectUnitMenu_Wapper ( );

	ReleaseSemaphore ( Semaphore_OnlyInScreenSameType, 1, NULL);
	return SelectedUnits;
}


int ExternQuickKey::SelectOnlyInScreenWeaponUnit (unsigned int SelectWay_Mask)
{//设计成mask 1==选中带武器可移动单位，设计SelectOnlyInScreenWeaponUnit成Mask 2==炮塔是否被选中, mask 4==是否选中Commander, mask 8==是否选中空军
	WaitForSingleObject ( Semaphore_OnlyInScreenWeapon, INFINITE);

	int SelectedUnits= 0;
	bool DoSelect_b= false;
	if (MOVEUNITSELECTABLE==SelectWay_Mask)
	{
		DeselectUnits ( );

		TAdynmemStruct *PTR = TAMainStruct_Ptr;

		LPDWORD CommandMaskAry= NULL;
		CommandMaskAry= GetUnitIDMaskAryByCategory ( "Commander");
		unsigned long NoWeaponPtr= reinterpret_cast<unsigned long> (&(PTR->Weapons[0]));
		//LPDWORD WeaponUnitAry= GetUnitIDMaskAryByCategory ( "CTRL_S");
		
		UnitStruct * Begin= PTR->OwnUnitBegin;
		UnitStruct *  Current;
		int MaxHotUnitCount_I= PTR->NumHotUnits;
		int Counter= 0;
		short int * HotUnitAry_Ptr= PTR->HotUnits;
		
		while (Counter<MaxHotUnitCount_I)
		{
			Current= &(Begin[HotUnitAry_Ptr[Counter]]);

			if (0!=(UnitValid2_State& Current->UnitSelected))
			{
				if (0.0F==(Current->Nanoframe))
				{
					if ((NULL!=Current->Owner_PlayerPtr1))
					{
						if (TAMainStruct_Ptr->LocalHumanPlayer_PlayerID==Current->Owner_PlayerPtr1->PlayerAryIndex)
						{
							Current->UnitSelected= (Current->UnitSelected)| (0x10);
							++SelectedUnits;
						}
					}
				}
			}
			++Counter;
		}
		FilterSelectedUnit ( WEAPONUNITS);
		UpdateSelectUnitEffect ( ) ;
		ApplySelectUnitMenu_Wapper ( );
		//freeTAMem ( WeaponUnitAry);
	}

	ReleaseSemaphore ( Semaphore_OnlyInScreenWeapon, 1, NULL);
	return SelectedUnits;
}

bool ExternQuickKey::SetIDMaskInTypeAry (WORD ID, DWORD SelectedUnitTypeIDAry_Dw[])
{
	SelectedUnitTypeIDAry_Dw[ID/ 32]|= (1<< (ID& 0x1f));
	return true;
}

bool ExternQuickKey::MatchInTypeAry (WORD ID, DWORD SelectedUnitTypeIDAry_Dw[])
{
	if (0!=(SelectedUnitTypeIDAry_Dw[ID/ 32]& (1<< (ID& 0x1f))))
	{
		return true;
	}
	return false;
}

int ExternQuickKey::FilterSelectedUnit (TAUnitType NeededType) //只会在已选中的单位中选择!!!!速度很慢，全部单位都枚举一遍!
{
	if (INVALIDTYPE==NeededType)
	{
		return 0;
	}

	WaitForSingleObject ( Semaphore_FilterSelect, INFINITE);

	LPDWORD CommandMaskAry= NULL;
	//LPDWORD ENGINEER
	//FACTORYS
	LPDWORD WeaponUnitsMaskAry= NULL;

	CommandMaskAry= GetUnitIDMaskAryByCategory ( "Commander");
	//WeaponUnitsMaskAry= GetUnitIDMaskAryByCategory ( "CTRL_W");

	TAdynmemStruct *PTR = TAMainStruct_Ptr;
	
	UnitStruct *  Begin= PTR->OwnUnitBegin;
	UnitStruct *  End= PTR->OwnUnitEnd;
	UnitStruct *  Current= Begin;
	int SelectedCounter= 0;

	bool DoSelect_b;

	unsigned long NoWeaponPtr;

	//"NoWeapon"
	PTR = *reinterpret_cast<TAdynmemStruct * *>(*(WeaponRelatedPtr[0]));
	NoWeaponPtr= reinterpret_cast<unsigned long> (&(PTR->Weapons[0]));
	
	PTR = TAMainStruct_Ptr;
	while (Current<=End)
	{
		if ((0!=((UnitSelected_State)& Current->UnitSelected))&&(0!=((UnitValid2_State)& Current->UnitSelected)))
		{
			if (0.0F==(Current->Nanoframe))
			{
				//ID= ;
				if ((NULL!=Current->Owner_PlayerPtr1))//&&(Player_LocalHuman==Current->ValidOwner_PlayerPtr->My_PlayerType))
				{
					//这儿是过滤  filter:
					DoSelect_b= false;

					if (0!=(COMMANDER& NeededType)&&
						(NULL!=CommandMaskAry)&&
						(MatchInTypeAry ( Current->UnitID, CommandMaskAry)))
					{
						DoSelect_b= true;
					}
					
					if (0!=(WEAPONUNITS& NeededType)&&
							((NoWeaponPtr!=reinterpret_cast<unsigned long>(Current->UnitType->weapon1))|| 
							(NoWeaponPtr!=reinterpret_cast<unsigned long>(Current->UnitType->weapon2))||
							(NoWeaponPtr!=reinterpret_cast<unsigned long>(Current->UnitType->weapon3)))
						)
					{
						DoSelect_b= true;
						if ((NULL!=CommandMaskAry)&&
							(MatchInTypeAry ( Current->UnitID, CommandMaskAry)))
						{//don't select commander in here
							DoSelect_b= false;
						}
						if((0!=Current->UnitType->nWorkerTime))
						{
							DoSelect_b= false;
						}
						
						if (NULL!=Current->UnitType->YardMap)
						{//building
							DoSelect_b= false;
						}

						if ((0!=(stockpile_mask&(Current->UnitType->weapon1->WeaponTypeMask)))
							||(0!=(stockpile_mask&(Current->UnitType->weapon2->WeaponTypeMask)))
							||(0!=(stockpile_mask&(Current->UnitType->weapon3->WeaponTypeMask))))
						{
							DoSelect_b= false;
						}
					}

					if (0!=(ENGINEER& NeededType)
						&&(0!=Current->UnitType->nWorkerTime)
						&&(NULL==Current->UnitType->YardMap)//building
						&&(! MatchInTypeAry ( Current->UnitID, CommandMaskAry)))
					{
						DoSelect_b= true;
					}
					if (0!=(FACTORYS& NeededType)
						&&(0!=Current->UnitType->nWorkerTime)
						&&(NULL!=Current->UnitType->YardMap)
						&&(! MatchInTypeAry ( Current->UnitID, CommandMaskAry)))
					{
						DoSelect_b= true;
					}

					if (DoSelect_b)
					{
						Current->UnitSelected=  Current->UnitSelected| UnitSelected_State;
						++SelectedCounter;
					}
					else
					{
						Current->UnitSelected= Current->UnitSelected& (~ UnitSelected_State);
					}
				}
			}
		}

		Current= &Current[1];
		
	}
	if (0!=SelectedCounter)
	{
		UpdateSelectUnitEffect ( );
		//ApplySelectUnitMenu_Wapper ( );
	}

	ReleaseSemaphore ( Semaphore_FilterSelect, 1, NULL);

	return SelectedCounter;
}

int __stdcall AddtionRoutine_CircleSelect (PInlineX86StackBuffer X86StrackBuffer)
{
	//int Selected= X86StrackBuffer->Ebp;
	__try	
	{
		if(DataShare->ehaOff == 1 && !DataShare->PlayingDemo)
		{
			__leave;
		}

		if (TAInGame==DataShare->TAProgress)
		{
			if (0<(0x8000&GetKeyState ( 0x57)))
			{
				X86StrackBuffer->Ebp= ((ExternQuickKey *)myExternQuickKey)->FilterSelectedUnit ( WEAPONUNITS);
				return X86STRACKBUFFERCHANGE;
			}
			else if (0<(0x8000&GetKeyState ( 0x42)))
			{
				X86StrackBuffer->Ebp= ((ExternQuickKey *)myExternQuickKey)->FilterSelectedUnit ( ENGINEER);
				return X86STRACKBUFFERCHANGE;
			}
			else if (0<(0x8000&GetKeyState ( 0x59)))
			{
				X86StrackBuffer->Ebp= ((ExternQuickKey *)myExternQuickKey)->FilterSelectedUnit ( FACTORYS);
				return X86STRACKBUFFERCHANGE;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	
	return 0;
}



