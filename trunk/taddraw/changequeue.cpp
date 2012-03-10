#include "oddraw.h"
#include "changequeue.h"
#include "iddrawsurface.h"
#include <stdio.h>
#include "tamem.h"

TAdynmemStruct *TAdynmem;

bool pressed = false;
HWND GWinProchWnd;

CChangeQueue::CChangeQueue()
{
	LocalShare->ChangeQueue = this;

	int *PTR = (int*)0x00511de8;
	TAdynmem = (TAdynmemStruct*)(*PTR);

	IDDrawSurface::OutptTxt ( "New CChangeQueue");
}

CChangeQueue::~CChangeQueue()
{

}

void Write(HWND WinProchWnd)
{
	/*char Mtmp[100];
	wsprintf(Mtmp, "IsUnit: %i", &TAdynmem->OwnUnitBegin[0].IsUnit);
	OutptTxt(Mtmp);
	wsprintf(Mtmp, "Weapon1: %i", &TAdynmem->OwnUnitBegin[0].Weapon1);
	OutptTxt(Mtmp);
	wsprintf(Mtmp, "Weapon2: %i", &TAdynmem->OwnUnitBegin[0].Weapon2);
	OutptTxt(Mtmp);
	wsprintf(Mtmp, "Weapon3: %i", &TAdynmem->OwnUnitBegin[0].Weapon3);
	OutptTxt(Mtmp);

	wsprintf(Mtmp, "Xpos: %i", &TAdynmem->OwnUnitBegin[0].XPos);
	OutptTxt(Mtmp);

	wsprintf(Mtmp, "Orders: %i", TAdynmem->OwnUnitBegin[0].UnitOrders);
	OutptTxt(Mtmp);
	wsprintf(Mtmp, "UnitSelected: %i", TAdynmem->OwnUnitBegin[0].UnitSelected);
	OutptTxt(Mtmp);*/


	UnitOrdersStruct *LastOrder = TAdynmem->OwnUnitBegin[0].UnitOrders;
	UnitOrdersStruct *NewLastOrder;
	while(LastOrder->NextOrder)
	{
		NewLastOrder = LastOrder;
		LastOrder = LastOrder->NextOrder;

	}

	LastOrder->NextOrder = TAdynmem->OwnUnitBegin[0].UnitOrders;
	TAdynmem->OwnUnitBegin[0].UnitOrders = LastOrder;

	NewLastOrder->NextOrder = NULL; 


}

void CChangeQueue::Blit(LPDIRECTDRAWSURFACE DestSurf)
{

}

bool CChangeQueue::Message(HWND WinProchWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_KEYDOWN:
		if(wParam == 67) //c
		{
			TAdynmem->OwnUnitBegin[0].UnitOrders->ThisStart.ThisStart = 1;
			Write(WinProchWnd);
			//*((int*)NULL) = 1;
			return true;
		}
		break;
	case WM_KEYUP:
		if(wParam == 67) //c
		{
			TAdynmem->OwnUnitBegin[0].UnitOrders->ThisStart.ThisStart = 0;
			return true;
		}
		break;
	}

	return false;
}

