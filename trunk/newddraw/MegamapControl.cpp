#include "oddraw.h"

#include "hook/etc.h"
#include "hook/hook.h"
#include "dialog.h"
#include "tamem.h"
#include "tafunctions.h"
#include "PCX.H"
#include "gameredrawer.h"
#include "UnitDrawer.h"
#include "MegamapControl.h"
#include "fullscreenminimap.h"
#include "dialog.h"
#include "gaf.h"
#include "iddrawsurface.h"
#include "ExternQuickKey.h "

#include <math.h>

#ifdef USEMEGAMAP

using namespace ordertype;

int  __stdcall FindMouseUnitRounte (PInlineX86StackBuffer X86StrackBuffer)
{
	if (((MegaMapControl *)(X86StrackBuffer->myInlineHookClass_Pish->ParamOfHook))->IsInControl ())
	{
		X86StrackBuffer->Eax= (*TAmainStruct_PtrPtr)->MouseOverUnit;
		X86StrackBuffer->rtnAddr_Pvoid= (LPVOID)0x48CDDC ;
		return X86STRACKBUFFERCHANGE;
	}

	return 0;
}

int  __stdcall  GetPosition_DwordRounte (PInlineX86StackBuffer X86StrackBuffer)
{
	if (((MegaMapControl *)(X86StrackBuffer->myInlineHookClass_Pish->ParamOfHook))->IsInControl ())
	{
		
		X86StrackBuffer->rtnAddr_Pvoid= (LPVOID)0x484CD4 ;
		return X86STRACKBUFFERCHANGE;
	}

	return 0;
}

int  __stdcall  GetGridPosFeatureRounte (PInlineX86StackBuffer X86StrackBuffer)
{
	if (((MegaMapControl *)(X86StrackBuffer->myInlineHookClass_Pish->ParamOfHook))->IsInControl ())
	{
		X86StrackBuffer->Eax= 0xffff;
		X86StrackBuffer->rtnAddr_Pvoid= (LPVOID)0x421EA0 ;
		return X86STRACKBUFFERCHANGE;
	}

	return 0;
}

MegaMapControl::MegaMapControl (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p, RECT * GameScreen_p,
	int MaxIconWidth, int MaxIconHeight, int MegaMapVirtulKey_arg, BOOL WheelMoveMegaMap_v, BOOL DoubleClickMoveMegamap_v, BOOL WheelZoom_v)
{
	FindMouseUnitHook= new InlineSingleHook ( (unsigned int)FindMouseUnit, 5, INLINE_5BYTESLAGGERJMP, FindMouseUnitRounte);
	FindMouseUnitHook->SetParamOfHook ( (LPVOID)this);


	GetPosition_DwordHook= new InlineSingleHook ( (unsigned int)GetPosition_Dword, 5, INLINE_5BYTESLAGGERJMP, GetPosition_DwordRounte);
	GetPosition_DwordHook->SetParamOfHook ( (LPVOID)this);


	GetGridPosFeatureHook= new InlineSingleHook ( (unsigned int)GetGridPosFeature, 5, INLINE_5BYTESLAGGERJMP, GetGridPosFeatureRounte);
	GetGridPosFeatureHook->SetParamOfHook ( (LPVOID)this);

	
	for (int i= 0; i<0x15; ++i)
	{
		Cursor_Surfc= NULL;
	}

	Init ( parent_p, MegaMapScreen_p, TAMap_p, GameScreen_p, MaxIconWidth, MaxIconHeight, MegaMapVirtulKey_arg, WheelMoveMegaMap_v, DoubleClickMoveMegamap_v, WheelZoom_v);
}


MegaMapControl::~MegaMapControl()
{
	if (FindMouseUnitHook)
	{
		delete FindMouseUnitHook;
	}
	if (GetPosition_DwordHook)
	{
		delete GetPosition_DwordHook;
	}

	if (GetGridPosFeatureHook)
	{
		delete GetGridPosFeatureHook;
	}
	return ;
}

void MegaMapControl::Init (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p, RECT * GameScreen_p,
	int MaxIconWidth, int MaxIconHeight, int MegaMapVirtulKey_arg, BOOL WheelMoveMegaMap_v, BOOL DoubleClickMoveMegamap_v, BOOL WheelZoom_v)
{
	parent= parent_p;
	
	MegaMapVirtulKey= MegaMapVirtulKey_arg;

	TAmainStruct_Ptr= *TAmainStruct_PtrPtr;

	SelectedCount= 0;

	WheelMoveMegaMap= WheelMoveMegaMap_v;
	DoubleClickMoveMegamap= DoubleClickMoveMegamap_v;
	WheelZoom= WheelZoom_v;
	//OrderType= 
	memcpy ( &MegaMapScreen, MegaMapScreen_p, sizeof(RECT));
	MegaMapWidth= MegaMapScreen.right- MegaMapScreen.left;
	MegaMapHeight= MegaMapScreen.bottom- MegaMapScreen.top;

	memcpy ( &TAMap, TAMap_p, sizeof(RECT));
	TAMapWidth= TAMap.right- TAMap.left;
	TAMapHeight= TAMap.bottom- TAMap.top;

	Screen2MapWidthScale= static_cast<float>(MegaMapWidth)/ static_cast<float>(TAMapWidth);
	Screen2MapHeightScale= static_cast<float>(MegaMapHeight)/ static_cast<float>(TAMapHeight);


	memcpy ( &TAGameScreen, GameScreen_p, sizeof(RECT));



	Position_Dword temp;

	ScreenPos2TAPos ( &temp, MaxIconWidth, MaxIconHeight);

	HalfMaxIconWidth_TAPos= temp.X/ 2;
	HalfMaxIconHeight_TAPos= temp.Y/ 2;

	InControl= FALSE;
	InMap= FALSE;

	LastDblXPos= -1;
	LastDblYpos= -1;
}


void MegaMapControl::InitSurface ( LPDIRECTDRAW TADD)
{
	if (!TADD)
	{
		return;// invalid
	}
	ReleaseSurface ( );

	LastCursor_GAFp= (*TAProgramStruct_PtrPtr)->Cursor;

	if (NULL==LastCursor_GAFp)
	{
		LastCursor_GAFp= TAmainStruct_Ptr->cursor_ary[cursornormal]->PtrFrameAry[0].PtrFrame;
	}

	if (LastCursor_GAFp)
	{

		POINT GafSize={0, 0};
		int Width= 0x30;
		int Height= 0x30;

		DDSURFACEDESC ddsd;
		DDRAW_INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;

		ddsd.dwWidth = Width;
		ddsd.dwHeight = Height;

		TADD->CreateSurface ( &ddsd, &Cursor_Surfc, NULL);

		DDRAW_INIT_STRUCT(ddsd);

		Cursor_Surfc->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);


		GafSize.x= ddsd.lPitch;
		GafSize.y= ddsd.dwHeight;


		memset ( (ddsd.lpSurface), LastCursor_GAFp->Background, ddsd.lPitch* ddsd.dwHeight);
		CopyGafToBits ( (LPBYTE)(ddsd.lpSurface), &GafSize, 0, 0, LastCursor_GAFp);

		Cursor_Surfc->Unlock(NULL);

	}

}

void MegaMapControl::ReleaseSurface (void) 
{

	if (NULL!=Cursor_Surfc)
	{
		Cursor_Surfc->Release ( );
		Cursor_Surfc= NULL;

	}
	
}

/*
DrawSelectRect ()
{

}*/

void MegaMapControl::DrawCursor(LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch
	, unsigned int X, unsigned int Y)
{
	GAFFrame * Cursor_GafP= (*TAProgramStruct_PtrPtr)->Cursor;


	X= (*TAProgramStruct_PtrPtr)->CursorX+ Cursor_GafP->xPosition;//= X;
	Y= (*TAProgramStruct_PtrPtr)->CursorY+ Cursor_GafP->yPosition;//= Y;

	POINT Aspect;
	Aspect.x= dwWidth;
	Aspect.y= dwHeight;

	CopyGafToBits ( (LPBYTE)lpSurfaceMem, &Aspect, X, Y, Cursor_GafP);
// 
// 	OFFSCREEN OffScreen;
// 	memset ( &OffScreen, 0, sizeof(OFFSCREEN));
// 	OffScreen.Height= dwHeight;
// 	OffScreen.Width= dwWidth;
// 	OffScreen.lPitch= lPitch;
// 	OffScreen.lpSurface= lpSurfaceMem;
// 
// 	OffScreen.ScreenRect.left= 0;
// 	OffScreen.ScreenRect.right=  dwWidth;;
// 
// 	OffScreen.ScreenRect.top= 0;
// 	OffScreen.ScreenRect.bottom= dwHeight;

// 	GafFrame2OFFScreen ( &OffScreen, Cursor_GafP, 
// 		X, Y);
}

POINT * MegaMapControl::TAPos2ScreenPos (POINT * ScreenPos, unsigned int TAX, unsigned int TAY, unsigned int TAZ)
{
	if (NULL==ScreenPos)
	{
		return NULL;
	}
	int TAx= TAX; 
	int TAy= TAY- TAZ/ 2;


	ScreenPos->x= static_cast<int>(static_cast<float>(TAx)* Screen2MapWidthScale);
	ScreenPos->y= static_cast<int>(static_cast<float>(TAy)* Screen2MapHeightScale);

	return ScreenPos;
}

Position_Dword * MegaMapControl::ScreenPos2TAPos (Position_Dword * TAPos, int x, int y, BOOL UseTAHeight)
{
	if (NULL==TAPos)
	{
		return NULL;
	}
	TAPos->X= static_cast<int>(static_cast<float>(x)/ Screen2MapWidthScale);
	TAPos->Y= static_cast<int>(static_cast<float>(y)/ Screen2MapHeightScale);
	if (UseTAHeight&&
		TAmainStruct_Ptr->Features)
	{
		TAPos->Z= GetPosHeight ( TAPos);
	}
	else
	{
		TAPos->Z= TAmainStruct_Ptr->SeaLevel;
	}


	//GetPosition_Dword ( TAPos->X, TAPos->Y, TAPos);
	return TAPos;
}


void MegaMapControl::EnterMegaMap ()
{
	PlaySound_Effect ( "Options", 0);

	parent->EnterMegaMap ( );

	// init those megamap control's data
	InControl= FALSE;
	InMap= FALSE;

	SelectState= selectbuttom::none;
	SelectedCount= CountSelectedUnits ( );
	TAmainStruct_Ptr->MouseOverUnit= 0;
	LastDblXPos= 0;
	LastDblYpos= 0;
	TAmainStruct_Ptr->BuildSpotState&= !CIRCLESELECTING;


}
                                    
void MegaMapControl::QuitMegaMap ( )
{
	PlaySound_Effect ( "Previous", 0);
	parent->QuitMegaMap ( );
	InControl= FALSE;
	InMap= FALSE;
	SelectState= selectbuttom::none;
	TAmainStruct_Ptr->MouseOverUnit= 0;
}

void MegaMapControl::Set (int VirtualKey)
{
	MegaMapVirtulKey= VirtualKey;
}

bool MegaMapControl::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	IDDrawSurface::OutptTxt( "MapControl Message");

	bool LBMD;
	int zDelta;
	int xPos;
	int yPos;
	bool shift;
	switch(Msg)
	{
	case WM_KEYUP:
	case WM_KEYDOWN:
		if(wParam == MegaMapVirtulKey)
		{
			if (WM_KEYUP==Msg)
			{
				if (IsBliting())
				{

					QuitMegaMap ( );
				}
				else
				{
					EnterMegaMap ();
				}
			}
			return true;
		}

		break;
	case WM_MOUSEWHEEL:
		
		zDelta= (short) HIWORD(wParam);    // wheel rotation

		xPos= LOWORD(lParam);  // horizontal position of cursor 
		
		yPos= HIWORD(lParam);  // vertical position of cursor 
		if (zDelta<0)
		{
			WheelBack ( xPos, yPos);
		}
		else if (0<zDelta)
		{//
			WheelFont ( xPos, yPos);
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK: 
	case WM_MOUSEMOVE:
		if ( IsBliting ())
		{
			 xPos = LOWORD(lParam);  // horizontal position of cursor 
			 yPos = HIWORD(lParam);  // vertical position of cursor 

			 LBMD= MK_LBUTTON==(MK_LBUTTON& wParam);
			 shift= MK_SHIFT==(MK_SHIFT& wParam);

			if ((xPos<TAGameScreen.right)
				&&(TAGameScreen.left<xPos)
				&&(yPos<TAGameScreen.bottom)
				&&(TAGameScreen.top<yPos))
			{
				InControl= TRUE;
			}
			else
			{
				InControl= FALSE;
			}

			if ((xPos<(MegaMapScreen.right))
				&&(MegaMapScreen.left<xPos)
				&&(yPos<(MegaMapScreen.bottom))
				&&(MegaMapScreen.top<yPos))
			{// in map
				
				InMap= TRUE;
				
				
				xPos= xPos- MegaMapScreen.left;
				yPos= yPos- MegaMapScreen.top;

				switch (Msg)
				{
				case WM_LBUTTONDOWN:
					if (! SelectDown ( xPos, yPos, false))
					{
						LeftDown ( xPos, yPos, shift);
					}
						
						
					break;
				case WM_LBUTTONDBLCLK:
					DoubleClick ( xPos, yPos, shift);
					break;
				case WM_LBUTTONUP:
					SelectedCount= CountSelectedUnits ( );
					if (!SelectUp ( xPos, yPos, false, shift))
					{
						LeftUp ( xPos, yPos, shift);
					}
						
					break;
				case WM_RBUTTONDOWN:
					RightDown( xPos, yPos, shift);
					break;
				case WM_RBUTTONUP:
					SelectedCount= CountSelectedUnits ( );
					RightUp ( xPos, yPos, shift);
					break;
				case WM_RBUTTONDBLCLK: 
					//RightDoubleClick ( xPos, yPos, shift);
					break;
				case WM_MOUSEMOVE:
					if (! SelectMove ( xPos, yPos, false, LBMD))
					{
						MouseMove ( xPos, yPos);
					}
						
					break;
				}
				
			}
			else
			{// out of map
				InMap= FALSE;
				xPos= xPos- MegaMapScreen.left;
				yPos= yPos- MegaMapScreen.top;
				switch (Msg)
				{
					case WM_LBUTTONDOWN:
						 SelectDown ( xPos, yPos, true);
						break;
					case WM_LBUTTONUP:
						 SelectedCount= CountSelectedUnits ( );
						 SelectUp ( xPos, yPos, true, shift);
						break;
					case WM_MOUSEMOVE:
						 SelectMove ( xPos, yPos, true, LBMD);
						break;
				}
				
			}

			if (IsInControl ())
			{
				return TRUE;
			}
		}
		break;
	}

	

	return FALSE;
}

BOOL MegaMapControl::RightDown (int x, int y, bool shift)
{

	return TRUE;
}
BOOL MegaMapControl::RightUp (int x, int y, bool shift)
{
	BOOL Upt= FALSE;
	if (STOP!=TAmainStruct_Ptr->PrepareOrder_Type)
	{//
		TAmainStruct_Ptr->PrepareOrder_Type= STOP;
		Upt= TRUE;
	}
	else if (0!=TAmainStruct_Ptr->InterfaceType)
	{// R
		//STOP==PrepareOrder_Type
		if (0<SelectedCount)
		{//
			if (cursorselect==TAmainStruct_Ptr->CurrentCursora_Index)
			{// in cursorselect case will do guard 
				TAmainStruct_Ptr->PrepareOrder_Type= DEFEND;
				Upt= TRUE;
			}
			SendOrder ( TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z, TAmainStruct_Ptr->PrepareOrder_Type, shift);
			Upt= TRUE;
		}
	}
	else
	{//L
		if (0<SelectedCount)// deselect unit when no prepare order 
		{
			DeselectUnits ();
			SelectedCount= 0;

			Upt= TRUE;
		}
	}

	if (Upt)
	{

		UpdateSelectUnitEffect ( ) ;
		ApplySelectUnitMenu_Wapper ( );
	}

// 	else
// 	{
// 		TAmainStruct_Ptr->CameraToUnit= 0; 
// 		ScreenPos2TAPos ( &TAmainStruct_Ptr->MouseMapPos, x, y);
// 		MoveScreen ( TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z);
// 		QuitMegaMap ( );
// 	}

	
	return TRUE;
}


BOOL MegaMapControl::LeftDown (int x, int y, bool shift)
{			

	return TRUE;
}

BOOL MegaMapControl::LeftUp (int x, int y, bool shift)
{
	UnitStruct * Begin= TAmainStruct_Ptr->OwnUnitBegin;
	int MouseUnit= TAmainStruct_Ptr->MouseOverUnit;
	if ((x==LastDblXPos)
		&&(y==LastDblYpos))
	{// no double clicked
		return TRUE;
	}
	LastDblXPos= -1;
	LastDblYpos= -1;

	if (cursorselect==TAmainStruct_Ptr->CurrentCursora_Index)
	{
		if (MouseUnit)
		{
			if (TAmainStruct_Ptr->Players[TAmainStruct_Ptr->LocalHumanPlayer_PlayerID].PlayerAryIndex==(TAmainStruct_Ptr->OwnUnitBegin[MouseUnit].Owner_PlayerPtr0->PlayerAryIndex))
			{
				DeselectUnits ( );
				SelectedCount= 0;
				if (0!=(0x20& Begin[MouseUnit].UnitSelected))
				{
					if (0.0F==(Begin[MouseUnit].Nanoframe))
					{
						Begin[MouseUnit].UnitSelected|= 0x10;
						++SelectedCount;
					}
				}

				UpdateSelectUnitEffect ();
				ApplySelectUnitMenu_Wapper ( );
			}
		}
	}
	else if (0<SelectedCount)
	{
		if (0==TAmainStruct_Ptr->InterfaceType)
		{// L
				if (cursorselect!=TAmainStruct_Ptr->CurrentCursora_Index)
				{
					if (BUILD!=TAmainStruct_Ptr->PrepareOrder_Type)
					{
						SendOrder ( TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z, TAmainStruct_Ptr->PrepareOrder_Type, shift);
					}

					UpdateSelectUnitEffect ( );
					ApplySelectUnitMenu_Wapper  ( );
				}
		}
		else
		{// R 
// 			if (MouseUnit)
// 			{
// 				;
// 			}
// 			else
			{
				DeselectUnits ();
				SelectedCount= 0;

				UpdateSelectUnitEffect ( ) ;
				ApplySelectUnitMenu_Wapper ( );
			}

		}
	}
	return TRUE;

}





	




BOOL MegaMapControl::DoubleClick (int x, int y, bool shift)
{
	int SelectedUnit= TAmainStruct_Ptr->MouseOverUnit;

	if (0!= SelectedUnit)
	{
		if (myExternQuickKey->DoubleClick)
		{
			LastDblXPos= x;
			LastDblYpos= y;

			UnitStruct * Begin= TAmainStruct_Ptr->OwnUnitBegin;

			//DeselectUnits ( );
			SelectedCount= 0;

			if (TAmainStruct_Ptr->Players[TAmainStruct_Ptr->LocalHumanPlayer_PlayerID].PlayerAryIndex==(TAmainStruct_Ptr->OwnUnitBegin[SelectedUnit].Owner_PlayerPtr0->PlayerAryIndex))
			{
				Begin[SelectedUnit].UnitSelected|= 0x10;
				SelectAllSelectedUnits ( );
				SelectedCount= CountSelectedUnits ( );
			}
			UpdateSelectUnitEffect ();
			ApplySelectUnitMenu_Wapper ( );
		
		}
	}
	else if (DoubleClickMoveMegamap)
	{// move screen
		MegamapMoveSceen ( x+ MegaMapScreen.left, y+ MegaMapScreen.top);
		QuitMegaMap  ( );
	}

	return TRUE;
}



BOOL MegaMapControl::WheelFont (int xPos, int yPos)
{
	if (WheelZoom)
	{
		if (IsBliting())
		{
			TAmainStruct_Ptr->CameraToUnit= 0; 

			if (WheelMoveMegaMap)
			{
				MegamapMoveSceen ( xPos, yPos);
			}
			QuitMegaMap ( );
		}
	}

	return TRUE;
}

BOOL MegaMapControl::WheelBack (int xPos, int yPos)
{
	if (WheelZoom)
	{
		if (! IsBliting())
		{
			EnterMegaMap ();
		}
	}

	return TRUE;
}
BOOL MegaMapControl::IsBliting(void)
{
	return parent->IsBliting();
}

BOOL MegaMapControl::IsInControl(void)
{
	
	return InControl;
}
BOOL MegaMapControl::IsInMap(void)
{

	return InMap;
}


BOOL MegaMapControl::MouseMove (int x, int y)
{
	if (IsInMap ( ))
	{
		ScreenPos2TAPos ( &TAmainStruct_Ptr->MouseMapPos, x, y, TRUE);

		RECT MouseRect;
		MouseRect.left= TAmainStruct_Ptr->MouseMapPos.X- HalfMaxIconWidth_TAPos;
		MouseRect.right= TAmainStruct_Ptr->MouseMapPos.X+ HalfMaxIconWidth_TAPos;
		MouseRect.top= TAmainStruct_Ptr->MouseMapPos.Y- HalfMaxIconHeight_TAPos;
		MouseRect.bottom= TAmainStruct_Ptr->MouseMapPos.Y+ HalfMaxIconHeight_TAPos;

		int Count= TAmainStruct_Ptr->NumHotRadarUnits;

		RadarUnit_ * RadarUnits_v= (*TAmainStruct_PtrPtr)->RadarUnits;
		UnitStruct * Begin= TAmainStruct_Ptr->OwnUnitBegin;

		UnitStruct * unitPtr;

		BOOL UnitUnderMouse= FALSE;
		for (int i= 0; i<Count; ++i)
		{
			unitPtr=  &Begin[RadarUnits_v[i].ID];
			int X= static_cast<int>(unitPtr->XPos+ unitPtr->UnitType->FootX/ 2);
			int Y=static_cast<int>(unitPtr->YPos+ unitPtr->UnitType->FootY/ 2- unitPtr->ZPos/ 2);
			if ((MouseRect.left<X)
				&&(X<MouseRect.right)
				&&(MouseRect.top<Y)
				&&(Y<MouseRect.bottom))
			{
				TAmainStruct_Ptr->MouseOverUnit= RadarUnits_v[i].ID;
				UnitUnderMouse= TRUE;
				break;
			}
		}

		if (!UnitUnderMouse)
		{
			TAmainStruct_Ptr->MouseOverUnit= 0;
		}
/*
		if (0!=TAmainStruct_Ptr->MouseOverUnit)
		{
			if (STOP==TAmainStruct_Ptr->PrepareOrder_Type)
			{
				int NewCursorIndex= CorretCursor_InGame ( TAmainStruct_Ptr->PrepareOrder_Type);

				if (NewCursorIndex!=TAmainStruct_Ptr->CurrentCursora_Index)
				{
					TAmainStruct_Ptr->CurrentCursora_Index= NewCursorIndex;
					SetUICursor ( &(TAmainStruct_Ptr->desktopGUI), TAmainStruct_Ptr->cursor_ary[NewCursorIndex]);
				}
			}
		}*/
	}

	return TRUE;
}

BOOL MegaMapControl::SelectDown (int x, int y, bool out)
{
	if (false==out)
	{
		SelectState= selectbuttom::down;

		SelectScreenRect.left= x;
		SelectScreenRect.top= y;

		SelectTick= GetTickCount ( );
	}
	return FALSE;
}

BOOL MegaMapControl::SelectUp (int x, int y, bool out, bool shift)
{
	BOOL Rtn_b= FALSE;

	UnitStruct * Begin= TAmainStruct_Ptr->OwnUnitBegin;


	if (selectbuttom::select==SelectState)
	{
		SelectState= selectbuttom::up;
		// do select
		LONG Tmp;
		Position_Dword TmpPos;
		if (SelectScreenRect.right<SelectScreenRect.left)
		{
			Tmp= SelectScreenRect.left;
			SelectScreenRect.left= SelectScreenRect.right;
			SelectScreenRect.right= Tmp;
		}

		if (SelectScreenRect.bottom<SelectScreenRect.top)
		{
			Tmp= SelectScreenRect.top;
			SelectScreenRect.top= SelectScreenRect.bottom;
			SelectScreenRect.bottom= Tmp;
		}


		if ((32<(SelectScreenRect.bottom- SelectScreenRect.top))
			&&(32<(SelectScreenRect.right- SelectScreenRect.left))
			&&((SelectTick/ 1000)<= (GetTickCount ( )/ 1000)))
		{
			ScreenPos2TAPos ( &TmpPos, SelectScreenRect.left, SelectScreenRect.top);
			SelectScreenRect.left= TmpPos.X;
			SelectScreenRect.top= TmpPos.Y;

			ScreenPos2TAPos ( &TmpPos, SelectScreenRect.right, SelectScreenRect.bottom);
			SelectScreenRect.right= TmpPos.X;
			SelectScreenRect.bottom= TmpPos.Y;
			myExternQuickKey->SelectUnitInRect ( ALL, &SelectScreenRect);
			SelectedCount= myExternQuickKey->FilterSelectedUnit ( );
			//

			Rtn_b= TRUE;
		}
	}
	else if ((! out)
		&&(0<SelectedCount))
	{
		if (STOP!=TAmainStruct_Ptr->PrepareOrder_Type)
		{
			//
			MOUSEEVENT MEvent;
			memset ( &MEvent, 0, sizeof(MOUSEEVENT));
			MEvent.fwKeys= shift? 4: 0;

		//	ScreenPos2TAPos ( &TAmainStruct_Ptr->MouseMapPos, x, y, TRUE);

			TAMapClick ( &MEvent);

			Rtn_b= TRUE;
		}
	}


	SelectState= selectbuttom::none;
	return Rtn_b;
}
BOOL MegaMapControl::SelectMove (int x, int y, bool Out_b, bool LBMD)
{
	if (LBMD)
	{
		if ((selectbuttom::down==SelectState
			||selectbuttom::select==SelectState)
			&&(! IsDrawOrder ( )))
		{
			SelectState= selectbuttom::select;

			if (Out_b)
			{
				if (x<0)
				{
					x= 0;
				}
				if (MegaMapWidth<x)
				{
					x= MegaMapWidth;
				}

				if (y<0)
				{
					y= 0;
				}
				if (MegaMapHeight<y)
				{
					y= MegaMapHeight;
				}
			}
			SelectScreenRect.right= x;
			SelectScreenRect.bottom= y;
			return TRUE;
		}
	}
	else
	{
		SelectState= selectbuttom::none;
	}


	return FALSE;
}

void MegaMapControl::LockBlit (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch)
{
	if (IsDrawGameRect ())
	{
			BlitSelect ( lpSurfaceMem,  dwWidth,  dwHeight,  lPitch);
	}

	if (IsDrawOrder ( ))
	{
			BlitOrder ( lpSurfaceMem,  dwWidth,  dwHeight,  lPitch);
	}
}

BOOL MegaMapControl::IsDrawOrder (void)
{
	return (0!= GetAsyncKeyState ( VK_SHIFT));
}

BOOL MegaMapControl::IsDrawGameRect (BOOL Build_b)
{
	if (Build_b)
	{
		return (BUILD==TAmainStruct_Ptr->PrepareOrder_Type);
	}
	return (selectbuttom::select==SelectState)
		||(BUILD==TAmainStruct_Ptr->PrepareOrder_Type);
}

void MegaMapControl::DrawBuildRect (OFFSCREEN * offscren_p, unsigned char  Color, 
	UnitDefStruct * BuildTargat, int TAx, int TAy, int TAz)
{// draw in TA screen
	POINT Pos;
	TAPos2ScreenPos (  &Pos, TAx, TAy, TAz);
	RECT Rect;
	Rect.left= Pos.x;
	Rect.top= Pos.y;


	TAPos2ScreenPos (  &Pos, BuildTargat->FootX* 16, BuildTargat->FootY* 16, 0);
	Rect.left-= Pos.x/ 2;
	Rect.top-= Pos.y/ 2;

	if (Rect.left<0)
	{
		Rect.left= 0;
	}
	if (Rect.top<0)
	{
		Rect.top= 0;
	}

	Rect.left+= MegaMapScreen.left;
	Rect.top+= MegaMapScreen.top;

	Rect.right= Rect.left+ Pos.x;
	Rect.bottom= Rect.top+ Pos.y;

	if (MegaMapScreen.right<Rect.right)
	{
		Rect.right= MegaMapScreen.right;
		Rect.left= Rect.right- Pos.x;
	}

	if (MegaMapScreen.bottom<Rect.bottom)
	{
		Rect.bottom= MegaMapScreen.bottom;
		Rect.top= Rect.bottom- Pos.y;
	}

	TADrawRect ( offscren_p, &Rect, Color);
}

void MegaMapControl::DrawTargatOrder (OFFSCREEN * OffScreen, UnitOrdersStruct * Order, PlayerStruct * me)
{
	POINT Pos;

	if (Order->AttackTargat)
	{
		do 
		{
			if (! CheckUnitInPlayerLOS ( me, Order->AttackTargat))
			{
				if (0x200000&Order->Order_State)
				{
					break;
				}
				Order->Order_State|= 0x200000;											
			}
			Order->RemeberX= Order->AttackTargat->XPos;
			Order->RemeberY= Order->AttackTargat->YPos- Order->AttackTargat->ZPos/ 2;

		} while (false);
		TAPos2ScreenPos ( &Pos, Order->RemeberX, Order->RemeberY, 0);
	}
	else
	{
		TAPos2ScreenPos ( &Pos, Order->Pos.X, Order->Pos.Y, Order->Pos.Z);
	}

	// 							RECT MouseRect;
	// 							MouseRect.top= Pos.y+ MegaMapScreen.top;
	// 							MouseRect.left= Pos.x+ MegaMapScreen.left;
	Pos.x+= MegaMapScreen.left;
	Pos.y+= MegaMapScreen.top;
	int Index= (*COBSciptHandler_Begin)[Order->COBHandler_index].cursorIndex;
	if ((cursorattack<=Index)
		&&(Index<=cursorhourglass))
	{
		PGAFSequence GafSeq_p= TAmainStruct_Ptr->cursor_ary[Index];

		PGAFFrame Gaf_p= GafSeq_p->PtrFrameAry[(TAmainStruct_Ptr->GameTime/ (GafSeq_p->PtrFrameAry->Animated* 2))% GafSeq_p->Frames].PtrFrame;

		GafFrame2OFFScreen ( OffScreen, Gaf_p, 
			Pos.x, Pos.y);
	}

}

void MegaMapControl::DrawOrderPath (OFFSCREEN * OffScreen, UnitOrdersStruct * Order, Position_Dword * UnitPos)
{
	//Order->Pos;
	double x;
	double y; 
	int X1;
	int X2;
	int Y1;
	int Y2;

	if (Order->AttackTargat)
	{
		X1= Order->RemeberX;
		X2= UnitPos->X;
		Y1= Order->RemeberY;
		Y2=  UnitPos->Y;
	}
	else
	{
		X1= Order->Pos.X;
		X2= UnitPos->X;
		Y1=  Order->Pos.Y;
		Y2=  UnitPos->Y;
	}



	if (X1<TAMap.left)
	{
		X1= TAMap.left;
	}

	if (X2<TAMap.left)
	{
		X2= TAMap.left;
	}
	if (TAMap.right<X1)
	{
		X1= TAMap.right;
	}

	if (TAMap.right<X2)
	{
		X2= TAMap.right;
	}



	if (Y1<TAMap.top)
	{
		Y1= TAMap.top;
	}
	if (Y2<TAMap.top)
	{
		Y2= TAMap.top;
	}

	if (TAMap.bottom<Y1)
	{
		Y1= TAMap.bottom;
	}

	if (TAMap.bottom<Y2)
	{
		Y2= TAMap.bottom;
	}


	x= X1- X2;
	y= Y1- Y2;
	double Sqrt= sqrt (x* x+ y* y);
	Position_Dword Pos;
	ScreenPos2TAPos ( &Pos, ORDERPATHSPACING, ORDERPATHSPACING);
	double SpacingSqrt= sqrt( (double)(Pos.X* Pos.X)+ (double)(Pos.Y* Pos.Y));

	if (SpacingSqrt<Sqrt)
	{//
		int TimeDelay= TAmainStruct_Ptr->GameTime- Order->StartTime;
		PGAFSequence path= TAmainStruct_Ptr->pathicon;

		if (TimeDelay<0)
		{
			TimeDelay= 0;
		}
		int FrameIndex= (TimeDelay/ path->PtrFrameAry->Animated)% path->Frames;

		
		double Curt= (SpacingSqrt* (TimeDelay% ORDERPATHSPACING))/ ORDERPATHSPACING;

		double Ratio;
		POINT ScreenPos;
		while (Curt<Sqrt)
		{
			Ratio= Curt/ Sqrt;

			TAPos2ScreenPos ( &ScreenPos, UnitPos->X+ static_cast<int>(x* Ratio), UnitPos->Y+ static_cast<int>(y* Ratio)- UnitPos->Z/ 2, 0);
			GafFrame2OFFScreen ( OffScreen, path->PtrFrameAry[FrameIndex].PtrFrame, 
				ScreenPos.x+ MegaMapScreen.left, ScreenPos.y+ MegaMapScreen.top);

			Curt+= SpacingSqrt;

			FrameIndex= (++FrameIndex)% path->Frames;
		}
	}

}
void MegaMapControl::BlitOrder (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch)
{
	IDDrawSurface::OutptTxt ( "BlitOrder");
	int ColorIndex;
	OFFSCREEN OffScreen;
	memset ( &OffScreen, 0, sizeof(OFFSCREEN));
	OffScreen.Height= dwHeight;
	OffScreen.Width= dwWidth;
	OffScreen.lPitch= lPitch;
	OffScreen.lpSurface= lpSurfaceMem;

	OffScreen.ScreenRect.left= 0;
	OffScreen.ScreenRect.right=  dwWidth;;

	OffScreen.ScreenRect.top= 0;
	OffScreen.ScreenRect.bottom= dwHeight;

	BOOL Draw= FALSE, OtherBuilder= FALSE;
	PlayerStruct * me= &TAmainStruct_Ptr->Players[TAmainStruct_Ptr->LocalHumanPlayer_PlayerID];
	UnitStruct * Begin= me->Units;
	UnitStruct * End= me->UnitsAry_End;
	unsigned int MouseID= TAmainStruct_Ptr->MouseOverUnit;
	unsigned int ShowRange= TAmainStruct_Ptr->ShowRangeUnitIndex;
	UnitStruct *Camera= TAmainStruct_Ptr->CameraToUnit;
	POINT Aspect;



	if (Camera)
	{
		if (Camera->UnitType->CANBUILD_ptr)
		{
			OtherBuilder= TRUE;
		}				
	}
	if (ShowRange)
	{
		if (TAmainStruct_Ptr->OwnUnitBegin[ShowRange].UnitType->CANBUILD_ptr)
		{
			OtherBuilder= TRUE;
		}
	}
	if (MouseID)
	{
		if (TAmainStruct_Ptr->OwnUnitBegin[MouseID].UnitType->CANBUILD_ptr)
		{
			OtherBuilder= TRUE;
		}
	}

	Position_Dword UnitPos;
	for (; Begin!=End; Begin= &Begin[1])
	{
		if ((0x10000000&Begin->UnitSelected)
			&&(0==(0x4000&Begin->UnitSelected)))
		{	
			Draw= FALSE;
			if ((Begin->UnitInGameIndex==MouseID)
				||(Begin==Camera)
				||(Begin->UnitInGameIndex==ShowRange)
				)
			{//
				Draw= TRUE;
			}	
			if ((Begin->UnitInGameIndex==MouseID)
				||(Begin->UnitInGameIndex==ShowRange))
			{
				DWORD Radius;
				int TAx;
				int TAy;

				TAPos2ScreenPos ( &Aspect, Begin->XPos, Begin->YPos, Begin->ZPos);

				TAx= Aspect.x+ MegaMapScreen.left;
				TAy= Aspect.y+ MegaMapScreen.top;

				Aspect.x= dwWidth;
				Aspect.y= dwHeight;
				//if ((*TAmainStruct_PtrPtr)->Showranges)

				if (2&Begin->Weapon1Valid)
				{
					if (Begin->Weapon1->Range)
					{
						Radius= (static_cast<int>(Begin->Weapon1->Range)* MegaMapWidth)/ TAMapWidth;
						DrawRadarCircle ( (LPBYTE)lpSurfaceMem, &Aspect,
							TAx, TAy, 
							Radius, 
							(*TAmainStruct_PtrPtr)->desktopGUI.RadarObjecColor[6] );
					}
				}
				if (2&Begin->Weapon2Valid)
				{
					if (Begin->Weapon2->Range)
					{
						Radius= (static_cast<int>(Begin->Weapon2->Range)* MegaMapWidth)/ TAMapWidth;
						DrawRadarCircle ( (LPBYTE)lpSurfaceMem, &Aspect,
							TAx, TAy, 
							Radius, 
							(*TAmainStruct_PtrPtr)->desktopGUI.RadarObjecColor[7] );
					}
				}
				if (2&Begin->Weapon3Valid)
				{
					if (Begin->Weapon3->Range)
					{
						Radius= (static_cast<int>(Begin->Weapon3->Range)* MegaMapWidth)/ TAMapWidth;
						DrawRadarCircle ( (LPBYTE)lpSurfaceMem, &Aspect,
							TAx, TAy, 
							Radius, 
							(*TAmainStruct_PtrPtr)->desktopGUI.RadarObjecColor[7] );
					}
				}
			}
			//
			if (Draw
				||(UnitSelected_State& Begin->UnitSelected)
				||OtherBuilder)
			{			
				
				if (Begin->UnitOrders)
				{
					UnitOrdersStruct *Order= Begin->UnitOrders;
	 
					UnitPos.X= Begin->XPos;
					UnitPos.Y= Begin->YPos;
					UnitPos.Z= Begin->ZPos;
					do 
					{
						DWORD OrderMask= (*COBSciptHandler_Begin)[Order->COBHandler_index].COBScripMask;

						if (OrderMask& 1)
						{// build
							if (0x10& Begin->UnitSelected)
							{
								ColorIndex= TAmainStruct_Ptr->desktopGUI.RadarObjecColor[10];
							}
							else
							{
								ColorIndex= TAmainStruct_Ptr->desktopGUI.RadarObjecColor[9];
							}
							DrawBuildRect ( &OffScreen, ColorIndex, 
								&(TAmainStruct_Ptr->UnitDef[Order->BuildUnitID]), Order->Pos.X, Order->Pos.Y, Order->Pos.Z);

							if (Draw)
							{
								DrawOrderPath ( &OffScreen, Order, &UnitPos);
							}

							UnitPos.X= Order->Pos.X;
							UnitPos.Y= Order->Pos.Y;
							UnitPos.Z= Order->Pos.Z;
						}

						if (OrderMask& 2)
						{
							if (Draw
								||(UnitSelected_State& Begin->UnitSelected))
							{
								DrawTargatOrder ( &OffScreen, Order, me);
								if (Draw)
								{
									DrawOrderPath ( &OffScreen, Order, &UnitPos);
								}

							}
						
							
						
							if (Order->AttackTargat)
							{
								UnitPos.X= Order->RemeberX;
								UnitPos.Y= Order->RemeberY;
								UnitPos.Z= 0;
							}
							else
							{
								UnitPos.X= Order->Pos.X;
								UnitPos.Y= Order->Pos.Y;
								UnitPos.Z= Order->Pos.Z;
							}
						}

						if (OrderMask& 8)
						{// cursor

							if (Draw
								||(UnitSelected_State& Begin->UnitSelected))
							{
								DrawTargatOrder ( &OffScreen, Order, me);
							}
							
							if (Order->AttackTargat)
							{
								UnitPos.X= Order->RemeberX;
								UnitPos.Y= Order->RemeberY;
								UnitPos.Z= 0;
							}
							else
							{
								UnitPos.X= Order->Pos.X;
								UnitPos.Y= Order->Pos.Y;
								UnitPos.Z= Order->Pos.Z;
							}
	
						}

						if (OrderMask& 0x10)
						{
							if (Draw)
							{
								if (4&Begin->cIsCloaked)
								{


									DWORD Radius= (static_cast<int>(Begin->UnitType->mincloakdistance)* MegaMapWidth)/ TAMapWidth;
									int TAx;
									int TAy;

									TAPos2ScreenPos ( &Aspect, Begin->XPos, Begin->YPos, Begin->ZPos);
									TAx= Aspect.x+ MegaMapScreen.left;
									TAy= Aspect.y+ MegaMapScreen.top;

									Aspect.x= dwWidth;
									Aspect.y= dwHeight;

									DrawRadarCircle ( (LPBYTE)lpSurfaceMem, &Aspect,
										TAx, TAy, 
										Radius, 
										(*TAmainStruct_PtrPtr)->desktopGUI.RadarObjecColor[0xf] );
								}
							}
						}

					} while (Order= Order->NextOrder);
				}
			}
		}
	}
	
}
void MegaMapControl::BlitSelect (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch)
{
	if (IsDrawGameRect ())
	{
		int ColorIndex;
		OFFSCREEN OffScreen;
		memset ( &OffScreen, 0, sizeof(OFFSCREEN));
		OffScreen.Height= dwHeight;
		OffScreen.Width= dwWidth;
		OffScreen.lPitch= lPitch;
		OffScreen.lpSurface= lpSurfaceMem;

		OffScreen.ScreenRect.left= 0;
		OffScreen.ScreenRect.right=  dwWidth;;

		OffScreen.ScreenRect.top= 0;
		OffScreen.ScreenRect.bottom= dwHeight;

		RECT SelectInScreen;

		if (selectbuttom::select==SelectState)
		{// draw

			SelectInScreen.left= SelectScreenRect.left+ MegaMapScreen.left;
			SelectInScreen.right= SelectScreenRect.right+ MegaMapScreen.left;
			SelectInScreen.top= SelectScreenRect.top+ MegaMapScreen.top;
			SelectInScreen.bottom= SelectScreenRect.bottom+ MegaMapScreen.top;

			ColorIndex= 0xf;

			TADrawRect ( &OffScreen, &SelectInScreen, TAmainStruct_Ptr->desktopGUI.RadarObjecColor[ColorIndex]);
		}
		else if (BUILD==TAmainStruct_Ptr->PrepareOrder_Type)
		{// draw build rect

			if (! IsInMap ( ))
			{
				return ;
			}

			ColorIndex = ((TAmainStruct_Ptr->BuildSpotState) & 0x40 )!= 0 ? 10 : 4;

			DrawBuildRect ( &OffScreen, TAmainStruct_Ptr->desktopGUI.RadarObjecColor[ColorIndex], &(TAmainStruct_Ptr->UnitDef[TAmainStruct_Ptr->BuildUnitID]), TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z);

			//ScreenPos2TAPos ( &TAmainStruct_Ptr->MouseMapPos, x, y, TRUE);
// 			POINT Po;
// 
// 			TAPos2ScreenPos ( &Po,  TAmainStruct_Ptr->CircleSelect_Pos1TAx, TAmainStruct_Ptr->CircleSelect_Pos1TAy, TAmainStruct_Ptr->CircleSelect_Pos1TAz);
// 			SelectInScreen.left= Po.x;
// 			SelectInScreen.top= Po.y;
// 
// 			TAPos2ScreenPos ( &Po,  TAmainStruct_Ptr->CircleSelect_Pos2TAx, TAmainStruct_Ptr->CircleSelect_Pos2TAy, TAmainStruct_Ptr->CircleSelect_Pos2TAz);
// 			
// 			//SelectInScreen.bottom= Po.y;
// 
// 			if (SelectInScreen.left<Po.x)
// 			{
// 				SelectInScreen.right= Po.x;
// 			}
// 			else
// 			{
// 				SelectInScreen.right= SelectInScreen.left;
// 				SelectInScreen.left= Po.x;
// 			}
// 
// 
// 			if (SelectInScreen.top<Po.y)
// 			{
// 				SelectInScreen.bottom= Po.y;
// 			}
// 			else
// 			{
// 				SelectInScreen.bottom= SelectInScreen.top;
// 				SelectInScreen.top= Po.y;
// 			}
// 
// 			SelectInScreen.left= SelectInScreen.left+ MegaMapScreen.left;
// 			SelectInScreen.right= SelectInScreen.right+ MegaMapScreen.left;
// 			SelectInScreen.top= SelectInScreen.top+ MegaMapScreen.top;
// 			SelectInScreen.bottom= SelectInScreen.bottom+ MegaMapScreen.top;


		}


		
// 		--SelectInScreen.bottom;
// 		--SelectInScreen.right;
// 		++SelectInScreen.top;
// 		++SelectInScreen.left;
// 		TADrawRect ( &OffScreen, &SelectInScreen, TAmainStruct_Ptr->desktopGUI.RadarObjecColor[ColorIndex]);
	}

}

void MegaMapControl::MegamapMoveSceen (int xPos, int yPos)
{
	if (MegaMapScreen.right<xPos)
	{
		xPos=MegaMapScreen.right;
	}
	if (xPos<MegaMapScreen.left)
	{
		xPos= MegaMapScreen.left;
	}
	if (MegaMapScreen.bottom<yPos)
	{
		yPos=MegaMapScreen.bottom;
	}
	if (yPos<MegaMapScreen.top)
	{
		yPos= MegaMapScreen.top;
	}


	xPos= xPos- MegaMapScreen.left;
	yPos= yPos- MegaMapScreen.top;

	ScreenPos2TAPos ( &TAmainStruct_Ptr->MouseMapPos, xPos, yPos);

	MoveScreen ( TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z);

}

void MegaMapControl::MoveScreen ( int TAX,  int TAY,  int TAZ)
{
	//ScrollToCenter ( TAX, TAY- TAZ/ 2);

	int *PTR = (int*)TAmainStruct_PtrPtr;
	int *XPointer = (int*)(*PTR + 0x1431f);
	int *YPointer = (int*)(*PTR + 0x14323);
	
 	TAX -= (((*TAProgramStruct_PtrPtr)->ScreenWidth)-(*TAmainStruct_PtrPtr)->GameSreen_Rect.left)/2;
 	TAY -= (((*TAProgramStruct_PtrPtr)->ScreenHeight)-(*TAmainStruct_PtrPtr)->GameSreen_Rect.top)/2;

	if(TAX<0)
		TAX = 0;
	if(TAY<0)
		TAY = 0;
	if(TAX>(GetMaxScrollX()))
		TAX = (GetMaxScrollX());
	if(TAY>(GetMaxScrollY()))
		TAY = (GetMaxScrollY());

	(*TAmainStruct_PtrPtr)->EyeBallMapXPos= TAX;
	(*TAmainStruct_PtrPtr)->EyeBallMapYPos= TAY;
	(*TAmainStruct_PtrPtr)->MapXScrollingTo= TAX;
	(*TAmainStruct_PtrPtr)->MapYScrollingTo= TAY;

	UpdateLosState ( 0);
	//ScrollMinimap ( );
	//041C3C0
}


/*
void MoveOrder (unsigned int TAX, unsigned int TAY, unsigned int TAZ, bool shift)
{

	SendOrder ( TAX, TAY, TAZ, MOVE, shift);
}

void PartolOrder (unsigned int TAX, unsigned int TAY, unsigned int TAZ, bool shift)
{
	SendOrder ( TAX, TAY, TAZ, PATROL, shift);
}

void AttackOrder (unsigned int TAX, unsigned int TAY, unsigned int TAZ, bool shift)
{

	SendOrder ( TAX, TAY, TAZ, ATTACK, shift);

}


BOOL BuildOrder ()
{
	return FALSE;
}

int TADRBuildOrder ()
{
	int OrderCount= 0;
	return OrderCount;
};
*/


#endif