#include "oddraw.h"
#include "idlevillager.h"
#include "iddrawsurface.h"
#include "tahook.h"
#include "tamem.h"
#include "tafunctions.h"
#include <stdio.h>

CIdleUnits::CIdleUnits()
{
	//LocalShare->IdleUnits = this;
	Semaphore_IdleCons= CreateSemaphore ( NULL, 1, 1, NULL);
	IDDrawSurface::OutptTxt ( "New CIdleUnits");
}

CIdleUnits::~CIdleUnits()
{
	if (NULL!=Semaphore_IdleCons)
	{
		CloseHandle ( Semaphore_IdleCons);
	}
}

void CIdleUnits::Blit(LPDIRECTDRAWSURFACE DestSurf)
{
	/*  int *BasePTR = (int*)0x511de8;
	int *UnitPTR = (int*)((*BasePTR)+0x1b8e+0x3c);

	for(int i=0; i<30; i++)
	{
	int Data;
	char *BytePTR = (char*)((*UnitPTR)+i + (0x118*2));
	Data = *BytePTR;

	char Outstr[20];
	wsprintf(Outstr, "%1i : %1i", i, Data);

	HDC hdc;
	DestSurf->GetDC(&hdc);

	TextOut(hdc, 140, 35 + i*15, Outstr, strlen(Outstr));

	DestSurf->ReleaseDC(hdc);
	}*/


/*
	int UnitOffset = 0x118;

	//find TA windows
	HWND TAhWnd = FindWindow("Total Annihilation Class", "Total Annihilation");
	//get PID
	DWORD PID;
	GetWindowThreadProcessId(TAhWnd, &PID);
	//enable all acess for ta
	HANDLE TAProc = OpenProcess(PROCESS_ALL_ACCESS, false, PID);

	int Adress = 0x511de8;
	int BasePTR;

	ReadProcessMemory(TAProc, (void*)Adress, (void*)&BasePTR, sizeof(int), NULL);

	int UnitPTR;
	ReadProcessMemory(TAProc, (void*)(BasePTR+0x1b8e+0x3c), (void*)&UnitPTR, sizeof(int), NULL);


	int StartAt = 0;
	for(int i=StartAt; i<StartAt+40; i++)
	{
		int Data = 0;
		ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

		char Outstr[20];
		wsprintf(Outstr, "%1i : %1i", i, Data);

		HDC hdc;
		DestSurf->GetDC(&hdc);

		TextOut(hdc, 200, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

		DestSurf->ReleaseDC(hdc);
	}
	StartAt += 40;
	for(int i=StartAt; i<StartAt+40; i++)
	{
		int Data = 0;
		ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

		char Outstr[20];
		wsprintf(Outstr, "%1i : %1i", i, Data);

		HDC hdc;
		DestSurf->GetDC(&hdc);

		TextOut(hdc, 280, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

		DestSurf->ReleaseDC(hdc);
	}
	StartAt += 40;
	for(int i=StartAt; i<StartAt+40; i++)
	{
		int Data = 0;
		ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

		char Outstr[20];
		wsprintf(Outstr, "%1i : %1i", i, Data);

		HDC hdc;
		DestSurf->GetDC(&hdc);

		TextOut(hdc, 340, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

		DestSurf->ReleaseDC(hdc);
	}
	StartAt += 40;
	for(int i=StartAt; i<StartAt+40; i++)
	{
		int Data = 0;
		ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

		char Outstr[20];
		wsprintf(Outstr, "%1i : %1i", i, Data);

		HDC hdc;
		DestSurf->GetDC(&hdc);

		TextOut(hdc, 420, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

		DestSurf->ReleaseDC(hdc);
	}
	StartAt += 40;
	for(int i=StartAt; i<StartAt+40; i++)
	{
		int Data = 0;
		ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

		char Outstr[20];
		wsprintf(Outstr, "%1i : %1i", i, Data);

		HDC hdc;
		DestSurf->GetDC(&hdc);

		TextOut(hdc, 500, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

		DestSurf->ReleaseDC(hdc);
	}
	StartAt += 40;
	for(int i=StartAt; i<StartAt+40; i++)
	{
		int Data = 0;
		ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

		char Outstr[20];
		wsprintf(Outstr, "%1i : %1i", i, Data);

		HDC hdc;
		DestSurf->GetDC(&hdc);

		TextOut(hdc, 580, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

		DestSurf->ReleaseDC(hdc);
	}
	StartAt += 40;
	for(int i=StartAt; i<StartAt+40; i++)
	{
		int Data = 0;
		ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

		char Outstr[20];
		wsprintf(Outstr, "%1i : %1i", i, Data);

		HDC hdc;
		DestSurf->GetDC(&hdc);

		TextOut(hdc, 660, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

		DestSurf->ReleaseDC(hdc);
	}

	char Outstr[20];
	wsprintf(Outstr, "%1i", UnitPTR + (2*UnitOffset));
	HDC hdc;
	DestSurf->GetDC(&hdc);

	TextOut(hdc, 490, 35, Outstr, strlen(Outstr));

	DestSurf->ReleaseDC(hdc);

	/ *int *PTR1 = (int*)0x511de8;
	int *Commander = (int*)((*PTR1)+0x1b8e+0x3c);
	unsigned short *XPos = (unsigned short*)((*Commander) + 0x6c);
	unsigned short *YPos = (unsigned short*)(*Commander + 0x74); * /

	CloseHandle(TAProc);*/
}

void FixAck()
{
	/*  HANDLE TAProc = GetCurrentProcess();

	int Adress = 0x511de8;
	int BasePTR = 0;
	int BasePTR2;
	int UnitOffset = 0x249;

	ReadProcessMemory(TAProc, (void*)Adress, (void*)&BasePTR, sizeof(int), NULL);

	//Edit1->Text = BasePTR + 0x1b8e+0x3c;

	//int UnitPTR;
	ReadProcessMemory(TAProc, (void*)(BasePTR+0x1439b), (void*)&BasePTR2, sizeof(int), NULL);

	int UnitPTR;
	int UnitArrayOffset = 0x118;
	ReadProcessMemory(TAProc, (void*)(BasePTR+0x1b8e+0x3c), (void*)&UnitPTR, sizeof(int), NULL);

	char Buff[30];
	for(int i=0; i<300; i++)
	{
	ReadProcessMemory(TAProc, (void*)(BasePTR2 + UnitOffset*i), (void*)Buff, 30, NULL);

	if(strcmp(Buff, "Adv. Construction Kbot") == 0)
	{
	static float BreakRate = 0.001f;
	//WriteProcessMemory(TAProc, (void*)(BasePTR2 + UnitOffset*i + 458), (void*)&BreakRate, sizeof(float), NULL);
	float *BPTR = (float*)(BasePTR2 + UnitOffset*i + 458);
	*BPTR = 0.01;
	BreakRate += 0.4;
	}
	}  */
}

void CIdleUnits::FindIdleConst()
{
	//FixAck();
	//

	IDDrawSurface::OutptTxt ( "Search Idle Const");

	DWORD Wait_rtn= WaitForSingleObject ( Semaphore_IdleCons, 1000);
	if (WAIT_FAILED==Wait_rtn)
	{
		return ;
	}
		
	if (WAIT_TIMEOUT==Wait_rtn)
	{
		goto ReleaseIdleConsSemaphore;
	}

	static int LastNum = 0;

	TAdynmemStruct *PTR = *(TAdynmemStruct* *)0x00511de8;
	UnitStruct * Start;// = PTR->OwnUnitBegin;//*(UnitStruct * *)((*PTR)+0x14357); 
	//UnitStruct * End= (UnitStruct *)((*PTR)+0x1435B);
	int i= 0;
	int MyMaxUnit= PTR->Players[PTR->LocalHumanPlayer_PlayerID].UnitsNumber;

	if (LocalShare->OrgLocalPlayerID!=PTR->LocalHumanPlayer_PlayerID)
	{// do not support this on 
		return ;
	}

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

				if(*WorkerTime>0)//有生产能力
				{
					char *UnitState = (char*)(*UnitOrderPTR + 4);

					if((*UnitState==41 || *UnitState==64) && i>LastNum) //idle
					{
						if (0!=(0x20& Start->UnitSelected))
						{
							LastNum = i;
							

							DeselectUnits ( );
							*UnitSelected=  *UnitSelected| UnitSelected_State;
							UpdateSelectUnitEffect ( ) ;
							ApplySelectUnitMenu_Wapper ( );

							ScrollToCenter(*XPos, *YPos);
							goto ReleaseIdleConsSemaphore;
						}

						//
						IDDrawSurface::OutptTxt ( "Select One");
						IDDrawSurface::OutptInt ( LastNum);
					}
				}
			}
		}
		++i;

		if(i>= MyMaxUnit)
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
	DeselectUnits ();

	ReleaseSemaphore ( Semaphore_IdleCons, 1, NULL);
	FindIdleConst();
	return;
ReleaseIdleConsSemaphore:
	ReleaseSemaphore ( Semaphore_IdleCons, 1, NULL);
	return;
}

void CIdleUnits::ScrollToCenter(int x, int y)
{
	int *PTR = (int*)0x00511de8;
	int *XPointer = (int*)(*PTR + 0x1431f);
	int *YPointer = (int*)(*PTR + 0x14323);

	x -= ((*(DWORD *)(0x051F320+ 0x0D4))-128)/2;
	y -= ((*(DWORD *)(0x051F320+ 0x0D8))-64)/2;

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

bool CIdleUnits::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	__try
	{
		if(DataShare->TAProgress != TAInGame)
			return false;

		switch(Msg)
		{
		case WM_KEYDOWN:
			if(wParam == 66 && (GetAsyncKeyState(17)&0x8000)>0 && (GetAsyncKeyState(16)&0x8000)==0) // ctrl + b
			{
				FindIdleConst();
				return true;
			}
			break;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		;// return LocalShare->TAWndProc(WinProcWnd, Msg, wParam, lParam);
	}

	return false;
}
/*

void CIdleUnits::DeselectUnits()
{
// 	int UnitOffset = 0x118;
// 
// 	int *PTR1 = (int*)0x511de8;
// 	int *UnitPTR = (int*)((*PTR1)+0x1b8e+0x3c);
// 
// 	int i=0;
// 	int OwnUnitBegin = 0;
// 	int NumUnits = *((int*)(*PTR1+0x1ca7))& 0x0000ffff;
// 
// 	//	   deselects all units
// 	while(OwnUnitBegin<NumUnits)
// 	{
// 		unsigned char  *UnitDead = (unsigned char *)(*UnitPTR + 247 + i*UnitOffset);
// 		unsigned int  *UnitSelected = (unsigned int *)(*UnitPTR + 272 + i*UnitOffset);
// 
// 
// 		*UnitSelected = (*UnitSelected) & 0xffffffFD;//;
// 
// 		if(*UnitDead!=0 && *UnitDead!=1)
// 		{
// 			OwnUnitBegin++;
// 		}
// 
// 		i++;
// 	}
	//clean selected units
	TAdynmemStruct *PTR = *(TAdynmemStruct* *)0x00511de8;
	UnitStruct * Start= PTR->OwnUnitBegin;// *(UnitStruct * *)((*PTR)+0x14357); 
	//UnitStruct * End= (UnitStruct *)((*PTR)+0x1435B);

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
*/


///-------extern circle select
