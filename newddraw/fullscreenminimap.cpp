// .text:00483638 0B4 8B D8                                                           mov     ebx, eax
// 	.text:0048363A 0B4 8B 03                                                           mov     eax, [ebx]
// .text:0048363C 0B4 3D 20 10 00 00                                                  cmp     eax, 4128
#include "oddraw.h"


#include "hook/hook.h"
#include "hook/etc.h"
#include "tamem.h"
#include "tafunctions.h"
#include "mapParse.h"
#include "gameredrawer.h"
#include "UnitDrawer.h"
#include "mappedmap.h"
#include "PCX.H"

#include "ProjectilesMap.h"
#include "MegamapControl.h"
#include "fullscreenminimap.h"
#include "dialog.h"
#include "gaf.h"
#include "iddrawsurface.h"

#include <vector>
using namespace std;
#include "TAConfig.h"

#ifdef USEMEGAMAP
int __stdcall LoadMap_Routine (PInlineX86StackBuffer X86StrackBuffer)
{
	TNTHeaderStruct * TNTPtr= (TNTHeaderStruct *)X86StrackBuffer->Eax;
	FullScreenMinimap * thethis= (FullScreenMinimap * )(X86StrackBuffer->myInlineHookClass_Pish->ParamOfHook);

	
	thethis->InitMinimap ( TNTPtr);
	return 0;
}

FullScreenMinimap::FullScreenMinimap (BOOL Doit)
{
	IDDrawSurface::OutptTxt ( "FullScreenMinimap init");
	LoadMap_hook= NULL;
	MyMinimap_p= NULL;
	GameDrawer= NULL;
	UnitsMap= NULL;
	Mapped_p= NULL;
	ProjectilesMap_p= NULL;

	MegamapVirtualKey= VK_TAB;

	Blit_b= FALSE;
	Flipping= FALSE;

	Controler= NULL;

	

	memset ( &MegamapRect, 0, sizeof(RECT) );
	memset ( &MegaMapInscren, 0, sizeof(RECT) );
	memset ( &TAMAPTAPos, 0, sizeof(RECT) );
	
	MegamapWidth= 0;
	MegamapHeight= 0;

	Do_b= Doit;
	if (Doit)
	{
		LoadMap_hook= new InlineSingleHook ( 0x00483638, 5, INLINE_5BYTESLAGGERJMP, LoadMap_Routine);
		
		LoadMap_hook->SetParamOfHook ( (LPVOID)this);
		GameDrawer= new TAGameAreaReDrawer;
	}



	DrawBackground= MyConfig->GetIniBool ( "DrawBackground", TRUE);
	DrawMapped= MyConfig->GetIniBool ( "DrawMapped", TRUE);
	DrawProjectile= MyConfig->GetIniBool ( "DrawProjectile", TRUE);
	DrawUnits= MyConfig->GetIniBool ( "DrawUnits", TRUE);
	DrawMegamapRect= MyConfig->GetIniBool ( "DrawMegamapRect", TRUE);
	DrawMegamapBlit= MyConfig->GetIniBool ( "DrawMegamapBlit", TRUE);
	DrawSelectAndOrder= MyConfig->GetIniBool ( "DrawSelectAndOrder", TRUE);
	DrawMegamapCursor= MyConfig->GetIniBool ( "DrawMegamapCursor", TRUE);

	DoubleClickMoveMegamap=  MyConfig->GetIniBool ( "DoubleClickMoveMegamap", FALSE);
	WheelMoveMegaMap= MyConfig->GetIniBool ( "WheelMoveMegaMap", TRUE);
	WheelZoom= MyConfig->GetIniBool ( "WheelZoom", TRUE);

// 	DrawTAScreen_hok= new InlineSingleHook ( (unsigned int)DrawGameScreen_Addr, 5, 
// 		INLINE_5BYTESLAGGERJMP, BlockTADraw);
// 
// 	DrawTAScreen_hok->SetParamOfHook ( reinterpret_cast<LPVOID>(this));
}


FullScreenMinimap::~FullScreenMinimap (void)
{

	if (NULL!=LoadMap_hook)
	{
		delete LoadMap_hook;
	}
	if (NULL!=MyMinimap_p)
	{
		delete MyMinimap_p;
	}
	if (NULL!=GameDrawer)
	{
		delete GameDrawer;
	}

	if (NULL!=Mapped_p)
	{
		delete Mapped_p;
	}

	if (NULL!=ProjectilesMap_p)
	{
		delete ProjectilesMap_p;
	}

	if (NULL!=Controler)
	{
		delete Controler;
	}

// 	if(DrawTAScreen_hok)
// 	{
// 		delete DrawTAScreen_hok;
// 		DrawTAScreen_hok= NULL;
// 	}
}

void FullScreenMinimap::InitMinimap (TNTHeaderStruct * TNTPtr, RECT * GameScreen)
{
	if (!(IDirectDraw*)LocalShare->TADirectDraw)
	{
		return ;
	}

	QuitMegaMap ( );
	if (MyMinimap_p)
	{
		delete MyMinimap_p;
		MyMinimap_p= NULL;
	}

	if (NULL==GameDrawer->GameAreaSurfaceFront_ptr)
	{
		GameDrawer->InitOwnSurface ( (IDirectDraw*)LocalShare->TADirectDraw);
	}
	
	if (NULL==GameScreen)
	{
		GameScreen= GameDrawer->TAWGameAreaRect ( NULL);
	}

	int GameWidth= GameScreen->right- GameScreen->left;
	int GameHeight= GameScreen->bottom- GameScreen->top;

	POINT MinimapAspect;

	MyMinimap_p= new TNTtoMiniMap ( GameWidth, GameHeight);
	if (MyMinimap_p)
	{
		MyMinimap_p->MapFromTNTInMem ( reinterpret_cast<LPVOID>(TNTPtr));

		MyMinimap_p->PictureInfo ( NULL, &MinimapAspect);

		GameDrawer->TAWGameAreaRect ( &MegaMapInscren);

		MegamapWidth= MinimapAspect.x;
		MegamapHeight= MinimapAspect.y;

		MegamapRect.right= MegaMapInscren.right- MegaMapInscren.left;
		MegamapRect.bottom= MegaMapInscren.bottom- MegaMapInscren.top;
		MegamapRect.top= 0;
		MegamapRect.left= 0;

		if ((MegamapWidth<MegamapRect.right)
			&&(2<(MegamapRect.right- MegamapWidth)))
		{
			MegamapRect.left= (MegamapRect.right- MegamapWidth)/ 2;
			MegamapRect.right= MegamapRect.left+ MegamapWidth;
		}

		if ((MegamapHeight<MegamapRect.bottom)
			&&(2<(MegamapRect.bottom- MegamapHeight)))
		{
			MegamapRect.top= (MegamapRect.bottom- MegamapHeight)/ 2;
			MegamapRect.bottom= MegamapRect.top+ MegamapHeight;
		}

		MegaMapInscren.top= MegaMapInscren.top+ MegamapRect.top;
		MegaMapInscren.bottom= MegaMapInscren.top+ MegamapHeight;
		MegaMapInscren.left= MegaMapInscren.left+ MegamapRect.left;
		MegaMapInscren.right= MegaMapInscren.left+ MegamapWidth;

		TAMAPTAPos.left= 0;
		TAMAPTAPos.top= 0;
		TAMAPTAPos.right= (TNTPtr->Width- 1)* 16;
		TAMAPTAPos.bottom= (TNTPtr->Height- 4)* 16;


		if (UnitsMap)
		{
			delete UnitsMap;
			UnitsMap= NULL; 
		}

		UnitsMap= new UnitsMinimap (  this, MinimapAspect.x, MinimapAspect.y);
		UnitsMap->InitSurface ( (IDirectDraw*)LocalShare->TADirectDraw);


		if (Mapped_p)
		{
			delete Mapped_p;
			Mapped_p= NULL;
		}
		Mapped_p= new MappedMap ( MinimapAspect.x, MinimapAspect.y);

		if (NULL!=Controler)
		{
			Controler->Init ( this, &MegaMapInscren, &TAMAPTAPos, GameScreen, ICONMAXWIDTH, ICONMAXHEIGHT, MegamapVirtualKey, WheelMoveMegaMap, DoubleClickMoveMegamap, WheelZoom);

			Controler->InitSurface ( (IDirectDraw*)LocalShare->TADirectDraw);
		}
		else
		{
			Controler= new MegaMapControl ( this, &MegaMapInscren, &TAMAPTAPos, GameScreen, ICONMAXWIDTH, ICONMAXHEIGHT, MegamapVirtualKey, WheelMoveMegaMap, DoubleClickMoveMegamap, WheelZoom);

			Controler->InitSurface ( (IDirectDraw*)LocalShare->TADirectDraw);
		}

		if (ProjectilesMap_p)
		{
			delete ProjectilesMap_p;
		}
		if (UnitsMap)
		{
			ProjectilesMap_p= new ProjectileMap ( this, UnitsMap);
		}
		
	}
}

void FullScreenMinimap::Set (int VirtualKey)
{
	MegamapVirtualKey= VirtualKey;

	if (Controler)
	{
		Controler->Set ( VirtualKey);
	}
}
void FullScreenMinimap::Blit(LPDIRECTDRAWSURFACE DestSurf)
{
	static DWORD LastTick= 0;
	static DWORD CurrentTick= 0;;

	if (Do_b
		&&Blit_b)
	{
		if (TAInGame==DataShare->TAProgress)
		{
			IDDrawSurface::OutptTxt ( "FullScreenMinimap blit");

			int CursorX= (*TAmainStruct_PtrPtr)->CurtMousePostion.x;
			int CursorY= (*TAmainStruct_PtrPtr)->CurtMousePostion.y;

			CurrentTick= GetTickCount ( );
			//i++;

			if ((LastTick+ 1000/MEGAMAPFPS)<=CurrentTick)
			{//
				if (!Flipping)
				{
					Flipping= TRUE;

					LastTick= CurrentTick;
					//i= 0;
					POINT Aspect;
					LPBYTE PixelBits;

					if (MyMinimap_p)
					{
						if (DrawBackground)
						{
							MyMinimap_p->PictureInfo ( &PixelBits, &Aspect);
						}
						

						if (DrawMapped)
						{
							Mapped_p->NowDrawMapped ( PixelBits, &Aspect);
						}
						Mapped_p->PictureInfo (  &PixelBits, &Aspect);


						if (DrawProjectile)
						{
							ProjectilesMap_p->NowDrawProjectile ( PixelBits, &Aspect);
						}
						
						if (DrawUnits)
						{
							UnitsMap->NowDrawUnits ( PixelBits, &Aspect);
						}
						

						if (DrawMegamapRect)
						{
							if (DrawUnits)
							{
								GameDrawer->MixDSufInBlit ( &MegamapRect, UnitsMap->GetSurface ( ) , NULL);
							}
							else
							{
								GameDrawer->MixBitsInBlit ( &MegamapRect, PixelBits, &Aspect, NULL);
							}
							
						}
						
						GameDrawer->Flip ( );
					}
			
					Flipping= FALSE;
				}
			}


			if (DrawMegamapBlit)
			{
				GameDrawer->BlitTAGameArea ( DestSurf);
			}
			DDSURFACEDESC ddsd;
			DDRAW_INIT_STRUCT(ddsd);

			if (DrawSelectAndOrder||DrawMegamapCursor)
			{
				if (DD_OK==DestSurf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL))
				{
					if (DrawSelectAndOrder)
					{
						LockBlit ( ddsd.lpSurface, ddsd.dwWidth, ddsd.dwHeight, ddsd.lPitch);
					}

					if (DrawMegamapCursor)
					{

						if ((CursorY!=-1)
							&&(CursorX!=-1))
						{
							if ((! DrawSelectAndOrder)||
								Controler==NULL||
								(Controler->IsInMap ( )&&(! Controler->IsDrawGameRect ( TRUE))))
							{
								Controler->DrawCursor ( ddsd.lpSurface, ddsd.dwWidth, ddsd.dwHeight, ddsd.lPitch,
									CursorX, CursorY);
							}
						}

					}
					DestSurf->Unlock ( NULL);
				}
			}
		}
		
	}
}
void FullScreenMinimap::LockBlit (LPVOID lpSurfaceMem, int dwWidth,int dwHeight, int lPitch)
{
	Controler->LockBlit ( lpSurfaceMem, dwWidth, dwHeight, lPitch);
}

void FullScreenMinimap::InitSurface (LPDIRECTDRAW TADD)
{
	if (Do_b
		&&(TALobby!=DataShare->TAProgress)
		&&MyMinimap_p)
	{
		if (UnitsMap)
		{
			POINT MinimapAspect;
			MyMinimap_p->PictureInfo ( NULL, &MinimapAspect);
			UnitsMap->ReSet ( MinimapAspect.x, MinimapAspect.y);

			UnitsMap->InitSurface ( TADD);
		}

		if (GameDrawer)
		{
			GameDrawer->InitOwnSurface ( TADD);
		}

		if (Controler)
		{
			Controler->InitSurface ( TADD);
		}
	}
}

void FullScreenMinimap::ReleaseSurface (void) 
{
	if (GameDrawer)
	{
		GameDrawer->ReleaseSurface ( );
		
	}
	if (UnitsMap)
	{
		UnitsMap->ReleaseSurface ( );
	}

	if (Controler)
	{
		Controler->ReleaseSurface ( );
	}
}

bool FullScreenMinimap::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

	if (Do_b)
	{
		if (TAInGame==DataShare->TAProgress)
		{

			if (Controler)
			{
				return Controler->Message ( WinProcWnd,  Msg,  wParam,  lParam);
			}
		}
	}
	return false;
/*

			switch(Msg)
			{
			case WM_KEYUP:
			case WM_KEYDOWN:
				if(wParam == 0x73)
				{
					if (WM_KEYUP==Msg)
					{
						Blit_b= ! Blit_b;
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
				if (Blit_b)
				{
					GameScreen= GameDrawer->TAWGameAreaRect ( NULL);
					xPos = LOWORD(lParam);  // horizontal position of cursor 
					yPos = HIWORD(lParam);  // vertical position of cursor 

					if ((xPos<GameScreen->right)
						&&(GameScreen->left<xPos)
						&&(yPos<GameScreen->bottom)
						&&(GameScreen->top<yPos))
					{

						return true;
					}
				}
				break;
			}
		}
		return false;
		*/


	
	
}

BOOL FullScreenMinimap::IsBliting ( )
{
	return Blit_b;
}
void FullScreenMinimap::EnterMegaMap ()
{
	Blit_b= TRUE;


}
void FullScreenMinimap::QuitMegaMap ( )
{
	Blit_b= FALSE;


}
int __stdcall BlockTADraw (PInlineX86StackBuffer X86StrackBuffer)
{
	FullScreenMinimap * this_me= (FullScreenMinimap *)(X86StrackBuffer->myInlineHookClass_Pish->ParamOfHook);


	if (this_me->IsBliting ( ))
	{
		*reinterpret_cast<LPDWORD>(X86StrackBuffer->Esp+ 4)= 0;

		X86StrackBuffer->Esp-= 0x214;
		return X86STRACKBUFFERCHANGE;
	}

	return 0;
}


#endif