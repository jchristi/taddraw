#include "oddraw.h"

#include "hook/etc.h"
#include "hook/hook.h"
#include "tamem.h"
#include "tafunctions.h"
#include "PCX.H"

#include "megamaptastuff.h"
#include "fullscreenminimap.h"

#include "MegamapControl.h"
#include "PCX.H"

#include "hook/etc.h"
#include "hook/hook.h"
#include "tahook.h"
#include "gaf.h"
#include <math.h>
#include <vector>

#include "iddrawsurface.h"
using namespace std;
MegamapTAStuff::MegamapTAStuff (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p, RECT * GameScreen_p,
	int MaxIconWidth, int MaxIconHeight)
{
	IDDrawSurface::OutptTxt ( "MegamapTAStuff");
	ScreenBuf= NULL;
	Init ( parent_p, MegaMapScreen_p, TAMap_p, GameScreen_p, MaxIconWidth, MaxIconHeight);
}
MegamapTAStuff::~MegamapTAStuff()
{
	if (ScreenBuf)
	{
		delete ScreenBuf;
		ScreenBuf= NULL;
	}
}


void MegamapTAStuff::Init (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p, RECT * GameScreen_p,
	int MaxIconWidth, int MaxIconHeight)
{
	parent= parent_p;
	TAmainStruct_Ptr= *TAmainStruct_PtrPtr;

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

	if (ScreenBuf)
	{
		delete ScreenBuf;
		ScreenBuf= NULL;
	}
	ScreenBuf= new BYTE [((*TAmainStruct_PtrPtr)->ScreenWidth+ 3)* (*TAmainStruct_PtrPtr)->ScreenHeight];

	Position_Dword temp;

	ScreenPos2TAPos ( &temp, MaxIconWidth, MaxIconHeight);

	HalfMaxIconWidth_TAPos= temp.X/ 2;
	HalfMaxIconHeight_TAPos= temp.Y/ 2;
}


void MegamapTAStuff::LockBlit_TA (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch)
{
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



	
	char Buf[0x10];

	BYTE KeepActiveBits[]= {0x90, 0x90};
	
	SingleHook * KeepActive= NULL;

	if (TAmainStruct_Ptr->desktopGUI.TheActive_GUIMEM
		&&TAmainStruct_Ptr->desktopGUI.TheActive_GUIMEM->topControl)
	{
/*
		DWORD   OnCommand_SOUNDSRT_GUI= 0x0045DA90;
		DWORD   OnCommand_MUSICRT_GUI= 0x0045D280;
		DWORD  OnCommand_SPEEDSRT_GUI= 0x045EAD0;
		DWORD  OnCommand_VISUALRT_GUI = 0x045E100;
		*/
		strcpy_s ( Buf, 0x10, TAmainStruct_Ptr->desktopGUI.TheActive_GUIMEM->GUIName);
		_strupr_s ( Buf, 0x10);
		if (0==_strcmpi  ( Buf, "SOUNDSRT.GUI")
			||0==_strcmpi  ( Buf, "MUSICRT.GUI")
			||0==_strcmpi  ( Buf, "SPEEDSRT.GUI")
			||0==_strcmpi  ( Buf, "VISUALRT.GUI"))
		{
			//need expand draw,
			//TAmainStruct_Ptr->desktopGUI.TheActive_GUIMEM->ActiveUp= 1;
			//
			
			KeepActive= new SingleHook ( KeepActiveAddr, sizeof(KeepActiveBits), INLINE_UNPROTECTEVINMENT, KeepActiveBits);
		}
	}

	DrawPopupF4Dialog ( &OffScreen);
	memcpy ( &OffScreen.ScreenRect, &(*TAmainStruct_PtrPtr)->GameSreen_Rect, sizeof(RECT));
	
	DrawChatText ( &OffScreen);
	DrawUnitUI ( &((*TAmainStruct_PtrPtr)->desktopGUI), &OffScreen, &((*TAmainStruct_PtrPtr)->GameSreen_Rect));
	
	if (KeepActive)
	{
		delete KeepActive;
	}
	DrawPopupButtomDialog ( &OffScreen);


	if ((*TAmainStruct_PtrPtr)->IsGamePaused)
	{//draw pause
		
		POINT Aspect;
		Aspect.x= lPitch;
		Aspect.y= dwHeight;

		CopyGafToBits ( (LPBYTE)lpSurfaceMem, &Aspect, 
			TAGameScreen.left+ (TAGameScreen.right- TAGameScreen.left)/ 2, TAGameScreen.top+ (TAGameScreen.bottom- TAGameScreen.top)/ 2, 
			(*TAmainStruct_PtrPtr)->igpaused->PtrFrameAry->PtrFrame);
	}

}



void MegamapTAStuff::LockBlit_MEGA (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch)
{
	if (parent->Controler->IsDrawGameRect ())
	{
		BlitSelect ( lpSurfaceMem,  dwWidth,  dwHeight,  lPitch);
	}

	if (parent->Controler->IsDrawOrder ( ))
	{
		BlitOrder ( lpSurfaceMem,  dwWidth,  dwHeight,  lPitch);
	}
}


void MegamapTAStuff::DrawCursor(LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch
	, unsigned int X, unsigned int Y)
{
	GAFFrame * Cursor_GafP= (*TAProgramStruct_PtrPtr)->Cursor;


	X= X+  Cursor_GafP->xPosition; //(*TAProgramStruct_PtrPtr)->CursorX+ Cursor_GafP->xPosition;//= X;
	Y= Y+ Cursor_GafP->yPosition;  //(*TAProgramStruct_PtrPtr)->CursorY+ Cursor_GafP->yPosition;//= Y;

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


void MegamapTAStuff::TADrawRect (OFFSCREEN * offscreen, int TAx1, int TAy1, int TAz1, 
	int TAx2, int TAy2, int TAz2, 
	int color)
{
	POINT Pos;
	TAPos2ScreenPos (  &Pos, TAx1, TAy1, TAz1);

	RECT Rect;
	Rect.left= Pos.x;
	Rect.top= Pos.y;

	if (Rect.left<0)
	{
		Rect.left= 0;
	}
	if (Rect.top<0)
	{
		Rect.top= 0;
	}
	Rect.left+= MegaMapScreen.left- TAGameScreen.left;
	Rect.top+= MegaMapScreen.top- TAGameScreen.top;

	TAPos2ScreenPos ( &Pos, TAx2, TAy2, TAz2);

	Rect.right= Pos.x;
	Rect.bottom= Pos.y;
	if ((MegaMapScreen.right- 1)<Rect.right)
	{
		Rect.right= (MegaMapScreen.right- 1);
	}

	if ((MegaMapScreen.bottom- 1)<Rect.bottom)
	{
		Rect.bottom= (MegaMapScreen.bottom- 1);
	}
	Rect.right+= MegaMapScreen.left- TAGameScreen.left;
	Rect.bottom+= MegaMapScreen.top- TAGameScreen.top;

	::TADrawRect ( offscreen, &Rect, color);
}
void MegamapTAStuff::DrawBuildRect (OFFSCREEN * offscren_p, unsigned char  Color, 
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

	if ((MegaMapWidth- 1)<(Rect.left+ Pos.x))
	{
		Pos.x= MegaMapWidth- 1- Rect.left;
	}
	if ((MegaMapHeight- 1)<(Rect.top+ Pos.y))
	{
		Pos.y= MegaMapHeight- 1- Rect.top;
	}

	
	Rect.left+= MegaMapScreen.left- TAGameScreen.left;
	Rect.top+= MegaMapScreen.top- TAGameScreen.top;

	Rect.right= Rect.left+ Pos.x;
	Rect.bottom= Rect.top+ Pos.y;

/*
	if (MegaMapScreen.right<Rect.right)
	{
		Rect.right= MegaMapScreen.right;
		Rect.left= Rect.right- Pos.x;
	}

	if (MegaMapScreen.bottom<Rect.bottom)
	{
		Rect.bottom= MegaMapScreen.bottom;
		Rect.top= Rect.bottom- Pos.y;
	}*/

	::TADrawRect ( offscren_p, &Rect, Color);
}

void MegamapTAStuff::DrawTargatOrder (OFFSCREEN * OffScreen, UnitOrdersStruct * Order, PlayerStruct * me)
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
	Pos.x+= MegaMapScreen.left- TAGameScreen.left;
	Pos.y+= MegaMapScreen.top- TAGameScreen.top;
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

void MegamapTAStuff::DrawOrderPath (OFFSCREEN * OffScreen, UnitOrdersStruct * Order, Position_Dword * UnitPos)
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
				ScreenPos.x+ MegaMapScreen.left- TAGameScreen.left, ScreenPos.y+ MegaMapScreen.top- TAGameScreen.top);

			Curt+= SpacingSqrt;

			FrameIndex= (++FrameIndex)% path->Frames;
		}
	}
}


void MegamapTAStuff::BlitOrder (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch)
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

	vector<Position_Dword> DrawedTargat;
	Position_Dword TargatPos;
	Position_Dword UnitPos;
	memset ( &TargatPos, 0, sizeof(Position_Dword));
	memset ( &UnitPos, 0, sizeof(Position_Dword));
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

				TAx= Aspect.x+ MegaMapScreen.left- TAGameScreen.left;
				TAy= Aspect.y+ MegaMapScreen.top- TAGameScreen.top;

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
							if (Order->AttackTargat)
							{
								TargatPos.X= Order->RemeberX;
								TargatPos.Y= Order->RemeberY;
								TargatPos.Z= 0;
							}
							else
							{
								TargatPos.X= Order->Pos.X;
								TargatPos.Y= Order->Pos.Y;
								TargatPos.Z= Order->Pos.Z;
							}

							if (Draw
								||(UnitSelected_State& Begin->UnitSelected))
							{
			
								BOOL Drawed= FALSE;

								for (vector<Position_Dword>::iterator P= DrawedTargat.begin ( ); P!= DrawedTargat.end ( ); ++P)
								{
									if ((P->X==TargatPos.X)
										&&(P->Y==TargatPos.Y)
										&&(P->Z==TargatPos.Z))
									{
										Drawed= TRUE;
										break;
									}
								}

								if (! Drawed)
								{
									DrawTargatOrder ( &OffScreen, Order, me);
									DrawedTargat.push_back ( TargatPos);
								}

								
								if (Draw)
								{
									DrawOrderPath ( &OffScreen, Order, &UnitPos);
								}
							}
							UnitPos.X= TargatPos.X;
							UnitPos.Y= TargatPos.Y;
							UnitPos.Z= TargatPos.Z;
						}

						if (OrderMask& 8)
						{// cursor

							if (Order->AttackTargat)
							{
								TargatPos.X= Order->RemeberX;
								TargatPos.Y= Order->RemeberY;
								TargatPos.Z= 0;
							}
							else
							{
								TargatPos.X= Order->Pos.X;
								TargatPos.Y= Order->Pos.Y;
								TargatPos.Z= Order->Pos.Z;
							}

							if (Draw
								||(UnitSelected_State& Begin->UnitSelected))
							{
							
								BOOL Drawed= FALSE;

								for (vector<Position_Dword>::iterator P= DrawedTargat.begin ( ); P!= DrawedTargat.end ( ); ++P)
								{
									if (P->X==TargatPos.X
										&&P->Y==TargatPos.Y
										&&P->Z==TargatPos.Z)
									{
										Drawed= TRUE;
										break;
									}
								}
								if (! Drawed)
								{
									DrawTargatOrder ( &OffScreen, Order, me);
									DrawedTargat.push_back ( TargatPos);
								}
							}
							
							UnitPos.X= TargatPos.X;
							UnitPos.Y= TargatPos.Y;
							UnitPos.Z= TargatPos.Z;
	
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


void MegamapTAStuff::BlitSelect (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch)
{
	if (parent->Controler->IsDrawGameRect ())
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
		RECT SelectScreenRect;
		if (selectbuttom::select==parent->Controler->ReadSelectState ())
		{// draw
			parent->Controler->ReadSelectRect ( &SelectScreenRect);
			SelectInScreen.left= SelectScreenRect.left+ MegaMapScreen.left- TAGameScreen.left;
			SelectInScreen.right= SelectScreenRect.right+ MegaMapScreen.left- TAGameScreen.left;
			SelectInScreen.top= SelectScreenRect.top+ MegaMapScreen.top- TAGameScreen.top;
			SelectInScreen.bottom= SelectScreenRect.bottom+ MegaMapScreen.top- TAGameScreen.top;

			ColorIndex= 0xf;

			::TADrawRect ( &OffScreen, &SelectInScreen, TAmainStruct_Ptr->desktopGUI.RadarObjecColor[ColorIndex]);
		}
		else if (ordertype::BUILD==TAmainStruct_Ptr->PrepareOrder_Type)
		{// draw build rect
			if (((CTAHook*)LocalShare->TAHook)->IsLineBuilding())
			{
				((CTAHook*)LocalShare->TAHook)->VisualizeRow_ForME_megamap ( &OffScreen);
			}
			else
			{
				if (! parent->Controler->IsInMap ( ))
				{
					return ;
				}

				ColorIndex = ((TAmainStruct_Ptr->BuildSpotState) & 0x40 )!= 0 ? 10 : 4;
				DrawBuildRect ( &OffScreen, TAmainStruct_Ptr->desktopGUI.RadarObjecColor[ColorIndex], &(TAmainStruct_Ptr->UnitDef[TAmainStruct_Ptr->BuildUnitID]), TAmainStruct_Ptr->MouseMapPos.X, TAmainStruct_Ptr->MouseMapPos.Y, TAmainStruct_Ptr->MouseMapPos.Z);
			}
		}
	}
}



POINT * MegamapTAStuff::TAPos2ScreenPos (POINT * ScreenPos, unsigned int TAX, unsigned int TAY, unsigned int TAZ)
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

Position_Dword * MegamapTAStuff::ScreenPos2TAPos (Position_Dword * TAPos, int x, int y, BOOL UseTAHeight)
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
