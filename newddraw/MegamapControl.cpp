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


MegaMapControl::MegaMapControl (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p,
	int MaxIconWidth, int MaxIconHeight, int MegaMapVirtulKey_arg)
{
	FindMouseUnitHook= new InlineSingleHook ( (unsigned int)FindMouseUnit, 5, INLINE_5BYTESLAGGERJMP, FindMouseUnitRounte);
	FindMouseUnitHook->SetParamOfHook ( (LPVOID)this);


	for (int i= 0; i<0x15; ++i)
	{
		Cursor_SurfcAry[i]= NULL;
	}

	Init ( parent_p, MegaMapScreen_p, TAMap_p,  MaxIconWidth, MaxIconHeight, MegaMapVirtulKey_arg);
}


MegaMapControl::~ MegaMapControl()
{
	if (FindMouseUnitHook)
	{
		delete FindMouseUnitHook;
	}
	return ;
}

void MegaMapControl::Init (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p,
	int MaxIconWidth, int MaxIconHeight, int MegaMapVirtulKey_arg)
{
	parent= parent_p;
	
	MegaMapVirtulKey= MegaMapVirtulKey_arg;

	TAmainStruct_Ptr= *TAmainStruct_PtrPtr;

	SelectedCount= 0;
	//OrderType= 
	memcpy ( &MegaMapScreen, MegaMapScreen_p, sizeof(RECT));
	MegaMapWidth= MegaMapScreen.right- MegaMapScreen.left;
	MegaMapHeight= MegaMapScreen.bottom- MegaMapScreen.top;

	memcpy ( &TAMap, TAMap_p, sizeof(RECT));
	TAMapWidth= TAMap.right- TAMap.left;
	TAMapHeight= TAMap.bottom- TAMap.top;

	Screen2MapWidthScale= static_cast<float>(MegaMapWidth)/ static_cast<float>(TAMapWidth);
	Screen2MapHeightScale= static_cast<float>(MegaMapHeight)/ static_cast<float>(TAMapHeight);

	Position_Dword temp;

	ScreenPos2TAPos ( &temp, MaxIconWidth, MaxIconHeight);

	HalfMaxIconWidth_TAPos= temp.X/ 2;
	HalfMaxIconHeight_TAPos= temp.Y/ 2;

	QuitMegaMap ( );
}


void MegaMapControl::InitSurface ( LPDIRECTDRAW TADD)
{
	ReleaseSurface ( );
	for (int i= 0; i<0x15; ++i)
	{
		GAFSequence * Cursor_Seq= (*TAmainStruct_PtrPtr) ->cursor_ary[i];
		if (Cursor_Seq)
		{
			Cursor_SurfcAry[i]= CreateSurfByGafSequence ( TADD, Cursor_Seq, true);
			DDSURFACEDESC ddsc;
			DDRAW_INIT_STRUCT(ddsc);


			Cursor_SurfcAry[i]->GetSurfaceDesc ( &ddsc);
			Cursor_AspectAry[i].x= ddsc.dwWidth;
			Cursor_AspectAry[i].y= ddsc.dwHeight;
		}
	}
}

void MegaMapControl::ReleaseSurface (void) 
{
	for (int i= 0; i<0x15; ++i)
	{
		if (NULL!=Cursor_SurfcAry[i])
		{
			Cursor_SurfcAry[i]->Release ( );
			Cursor_SurfcAry[i]= NULL;

			Cursor_AspectAry[i].x= 0;
			Cursor_AspectAry[i].y= 0;
		}
	}
}

/*
DrawSelectRect ()
{

}*/
void MegaMapControl::DrawCursor (LPDIRECTDRAWSURFACE DestSurf, unsigned int X, unsigned int Y)
{
	GAFSequence * Cursor_Seq= TAmainStruct_Ptr->cursor_ary[TAmainStruct_Ptr->CurrentCursora_Index];

	if (NULL!=Cursor_SurfcAry[TAmainStruct_Ptr->CurrentCursora_Index])
	{
		LPDIRECTDRAWSURFACE SrcSurf= Cursor_SurfcAry[TAmainStruct_Ptr->CurrentCursora_Index];
		if (SrcSurf->IsLost ( ))
		{
			SrcSurf->Restore ( );


			DDSURFACEDESC ddsd;
			DDRAW_INIT_STRUCT(ddsd);
			POINT Aspect;
			if (DD_OK==SrcSurf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL))
			{
				unsigned char *SurfPTR = (unsigned char*)ddsd.lpSurface;

				Aspect.x= ddsd.lPitch;
				Aspect.y= ddsd.dwHeight;
				CopyGafSequenceToBits ( SurfPTR, &Aspect, Cursor_Seq, 0);
				SrcSurf->Unlock(NULL);
			}
		}
		DDBLTFX ddbltfx;
		DDRAW_INIT_STRUCT(ddbltfx);
		RECT Dest;
		Dest.left = X;
		Dest.top = Y;
		Dest.right = X + Cursor_AspectAry[TAmainStruct_Ptr->CurrentCursora_Index].x;
		Dest.bottom = Y + Cursor_AspectAry[TAmainStruct_Ptr->CurrentCursora_Index].y;

		ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue= Cursor_Seq->PtrFrameAry[0].PtrFrame->Background;
		ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue= 0;

		if(DestSurf->Blt(&Dest, SrcSurf, NULL, DDBLT_ASYNC| DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
		{
			DestSurf->Blt(&Dest, SrcSurf, NULL, DDBLT_WAIT| DDBLT_KEYSRCOVERRIDE, &ddbltfx);
		}
	}
	else
	{
		((Dialog *)(LocalShare->Dialog))->BlitCursor ( DestSurf, X, Y);
	}
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

Position_Dword * MegaMapControl::ScreenPos2TAPos (Position_Dword * TAPos, int x, int y)
{
	if (NULL==TAPos)
	{
		return NULL;
	}
	TAPos->X= static_cast<int>(static_cast<float>(x)/ Screen2MapWidthScale);
	TAPos->Y= static_cast<int>(static_cast<float>(y)/ Screen2MapHeightScale);
	TAPos->Z= TAmainStruct_Ptr->SeaLevel;

	return TAPos;
}


void MegaMapControl::EnterMegaMap ()
{
	parent->EnterMegaMap ( );

	// init those megamap control's data
	InControl= FALSE;

	SelectState= selectbuttom::none;
	SelectedCount= CountSelectedUnits ( );
	TAmainStruct_Ptr->MouseOverUnit= 0;
	//TAmainStruct_Ptr->BuildSpotState&= !CIRCLESELECTING;
}

void MegaMapControl::QuitMegaMap ( )
{
	parent->QuitMegaMap ( );
	InControl= FALSE;
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
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK: 
	case WM_MOUSEMOVE:
		if ( IsBliting ())
		{
			int xPos = LOWORD(lParam);  // horizontal position of cursor 
			int yPos = HIWORD(lParam);  // vertical position of cursor 

			if ((xPos<MegaMapScreen.right)
				&&(MegaMapScreen.left<xPos)
				&&(yPos<MegaMapScreen.bottom)
				&&(MegaMapScreen.top<yPos))
			{// in control
				InControl= TRUE;
				bool shift= MK_SHIFT==(MK_SHIFT& wParam);
				xPos= xPos- MegaMapScreen.left;
				yPos= yPos- MegaMapScreen.top;
				if (0==TAmainStruct_Ptr->InterfaceType)
				{// L
					switch (Msg)
					{
					case WM_LBUTTONDOWN:
						if (! SelectDown ( xPos, yPos, false))
						{
							LeftDown ( xPos, yPos, shift);
						}
						
						
						break;
					case WM_LBUTTONDBLCLK:
						LeftDoubleClick ( xPos, yPos, shift);
						break;
					case WM_LBUTTONUP:
						if (!SelectUp ( xPos, yPos, false))
						{
							LeftUp ( xPos, yPos, shift);
						}
						
						break;
					case WM_RBUTTONDOWN:
						RightDown( xPos, yPos, shift);
						break;
					case WM_RBUTTONUP:
						RightUp ( xPos, yPos, shift);
						break;
					case WM_RBUTTONDBLCLK: 
						RightDoubleClick ( xPos, yPos, shift);
						break;
					case WM_MOUSEMOVE:
						if (! SelectMove ( xPos, yPos, false))
						{
							MouseMove ( xPos, yPos);
						}
						
						break;
					}
				}
				else
				{//R

					switch (Msg)
					{
					case WM_LBUTTONDOWN:
						if (! SelectDown ( xPos, yPos, false))
						{
							RightDown( xPos, yPos, shift);
						}
						
						break;
					case WM_LBUTTONDBLCLK:
						RightDoubleClick ( xPos, yPos, shift);
						break;
					case WM_LBUTTONUP:
						if (!SelectUp ( xPos, yPos, false))
						{
							RightUp ( xPos, yPos, shift);
						}
						
						break;
					case WM_RBUTTONDOWN:
						LeftDown ( xPos, yPos, shift);
						
						break;
					case WM_RBUTTONUP:
						
						LeftUp ( xPos, yPos, shift);
						break;
					case WM_RBUTTONDBLCLK: 
						LeftDoubleClick ( xPos, yPos, shift);
						
						break;
					case WM_MOUSEMOVE:
						if (! SelectMove ( xPos, yPos, false))
						{
							MouseMove ( xPos, yPos);
						}
						break;
					}
				}
				return TRUE;
			}
			else
			{// out of control

				InControl= FALSE;
				xPos= xPos- MegaMapScreen.left;
				yPos= yPos- MegaMapScreen.top;
				switch (Msg)
				{
					case WM_LBUTTONDOWN:
						 SelectDown ( xPos, yPos, true);
						break;
					case WM_LBUTTONUP:
						 SelectUp ( xPos, yPos, true);
						break;
					case WM_MOUSEMOVE:
						 SelectMove ( xPos, yPos, true);
						break;
				}
				
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
	TAmainStruct_Ptr->CameraToUnit= 0; 
	ScreenPos2TAPos ( &TAmainStruct_Ptr->MouseMapPos, x, y);
	MoveScreen ( TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z);


	if (0<SelectedCount)
	{
		SelectedCount= 0;
		DeselectUnits ();

		UpdateSelectUnitEffect ( ) ;
		ApplySelectUnitMenu_Wapper ( );

	}
	else
	{
		QuitMegaMap ( );
	}

	
	return TRUE;
}


BOOL MegaMapControl::LeftDown (int x, int y, bool shift)
{			

	UnitStruct * Begin= TAmainStruct_Ptr->OwnUnitBegin;
	int MouseUnit= TAmainStruct_Ptr->MouseOverUnit;

	TAmainStruct_Ptr->CameraToUnit= 0; 

	SelectedCount= CountSelectedUnits ( );

	ScreenPos2TAPos ( &TAmainStruct_Ptr->MouseMapPos, x, y);

	if (0<SelectedCount)
	{//
		if (cursorselect!=TAmainStruct_Ptr->CurrentCursora_Index)
		{
			SendOrder ( TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z, TAmainStruct_Ptr->PrepareOrder_Type, shift);
			MoveScreen ( TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z);

			UpdateSelectUnitEffect ( );
			ApplySelectUnitMenu_Wapper  ( );
		}
		else
		{
			DeselectUnits ( );
			UpdateSelectUnitEffect ();
			ApplySelectUnitMenu_Wapper ( );
		}

	}

	if (MouseUnit)
	{
		if (TAmainStruct_Ptr->Players[TAmainStruct_Ptr->LocalHumanPlayer_PlayerID].PlayerAryIndex==(TAmainStruct_Ptr->OwnUnitBegin[MouseUnit].Owner_PlayerPtr0->PlayerAryIndex))
		{
			Begin[MouseUnit].UnitSelected|= 0x10;

			UpdateSelectUnitEffect ();
			ApplySelectUnitMenu_Wapper ( );
			SelectedCount= 1;
		}
	}

	return TRUE;
}

BOOL MegaMapControl::LeftUp (int x, int y, bool shift)
{

	

	
	return TRUE;
}

BOOL MegaMapControl::RightDoubleClick (int x, int y, bool shift)
{
	return TRUE;
}


BOOL MegaMapControl::LeftDoubleClick (int x, int y, bool shift)
{
	if (myExternQuickKey->DoubleClick)
	{
		int SelectedUnit= TAmainStruct_Ptr->MouseOverUnit;

		DeselectUnits ();
		SelectedCount= 0;
		if (0!= SelectedUnit)
		{
			UnitStruct * Begin= TAmainStruct_Ptr->OwnUnitBegin;

			if (TAmainStruct_Ptr->Players[TAmainStruct_Ptr->LocalHumanPlayer_PlayerID].PlayerAryIndex==(TAmainStruct_Ptr->OwnUnitBegin[SelectedUnit].Owner_PlayerPtr0->PlayerAryIndex))
			{
				Begin[SelectedUnit].UnitSelected|= 0x10;
				SelectAllSelectedUnits ( );
				SelectedCount= CountSelectedUnits ( );
			}
		}
		UpdateSelectUnitEffect ();
		ApplySelectUnitMenu_Wapper ( );
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

BOOL MegaMapControl::MouseMove (int x, int y)
{
	ScreenPos2TAPos ( &TAmainStruct_Ptr->MouseMapPos, x+ 5, y+ 5);

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
		if ((MouseRect.left<static_cast<int>(unitPtr->XPos))
			&&(static_cast<int>(unitPtr->XPos)<MouseRect.right)
			&&(MouseRect.top<static_cast<int>(unitPtr->YPos))
			&&(static_cast<int>(unitPtr->YPos)<MouseRect.bottom))
		{
// 			POINT IconSize;
// 
// 			parent->UnitsMap->UnitPicture ( unitPtr, unitPtr->myLos_PlayerID, NULL, &IconSize);
// 			IconSize.x= IconSize.x/ 2+ 5;
// 			IconSize.y= IconSize.y/ 2+ 5;
// 
// 			if (((TAmainStruct_Ptr->MouseMapPos.X- IconSize.x)< static_cast<int>(unitPtr->XPos))
// 				&&(static_cast<int>(unitPtr->XPos)< (TAmainStruct_Ptr->MouseMapPos.X+ IconSize.x))
// 				&&((TAmainStruct_Ptr->MouseMapPos.Y- IconSize.y)<static_cast<int>(unitPtr->YPos))
// 				&&(static_cast<int>(unitPtr->YPos)<(TAmainStruct_Ptr->MouseMapPos.Y+ IconSize.y)))
// 			{
				TAmainStruct_Ptr->MouseOverUnit= RadarUnits_v[i].ID;
				UnitUnderMouse= TRUE;
				break;
//			}
		}
	}
	if (!UnitUnderMouse)
	{
		TAmainStruct_Ptr->MouseOverUnit= 0;
	}
	if (0!=TAmainStruct_Ptr->MouseOverUnit)
	{
		int NewCursorIndex= CorretCursor_InGame ( TAmainStruct_Ptr->PrepareOrder_Type);

		if (NewCursorIndex!=TAmainStruct_Ptr->CurrentCursora_Index)
		{
			TAmainStruct_Ptr->CurrentCursora_Index= NewCursorIndex;
			SetUICursor ( &(TAmainStruct_Ptr->desktopGUI), TAmainStruct_Ptr->cursor_ary[NewCursorIndex]);
		}
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
	}
	return FALSE;
}

BOOL MegaMapControl::SelectUp (int x, int y, bool out)
{
	BOOL Rtn_b= FALSE;

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

	SelectState= selectbuttom::none;
	return Rtn_b;
}
BOOL MegaMapControl::SelectMove (int x, int y, bool Out_b)
{
	if ( selectbuttom::down==SelectState
		||selectbuttom::select==SelectState)
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

	return FALSE;
}

void MegaMapControl::BlitSelect (LPDIRECTDRAWSURFACE DestSurf)
{
	DDSURFACEDESC ddsd;
	DDRAW_INIT_STRUCT(ddsd);

	if (DD_OK==DestSurf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL))
	{
		LockBlit ( ddsd.lpSurface, ddsd.dwWidth, ddsd.dwHeight, ddsd.lPitch);
		DestSurf->Unlock ( NULL);
	}
}
void MegaMapControl::LockBlit (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch)
{
	if (selectbuttom::select==SelectState)
	{// draw
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
		SelectInScreen.left= SelectScreenRect.left+ MegaMapScreen.left;
		
		SelectInScreen.right= SelectScreenRect.right+ MegaMapScreen.left;
		SelectInScreen.top= SelectScreenRect.top+ MegaMapScreen.top;
		SelectInScreen.bottom= SelectScreenRect.bottom+ MegaMapScreen.top;


		TADrawRect ( &OffScreen, &SelectInScreen, TAmainStruct_Ptr->desktopGUI.RadarObjecColor[0xe]);
	}
}

void MegaMapControl::MoveScreen (unsigned int TAX, unsigned int TAY, unsigned int TAZ)
{
	ScrollToCenter ( TAX, TAY);
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

