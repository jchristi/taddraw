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

	MegamapVirtualKey= VK_F4;

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

		ProjectilesMap_p= new ProjectileMap ( );
	}
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
}

void FullScreenMinimap::InitMinimap (TNTHeaderStruct * TNTPtr, RECT * GameScreen)
{

	
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
			Controler->Init ( this, &MegaMapInscren, &TAMAPTAPos, ICONMAXWIDTH, ICONMAXHEIGHT, MegamapVirtualKey);

			Controler->InitSurface ( (IDirectDraw*)LocalShare->TADirectDraw);
		}
		else
		{
			Controler= new MegaMapControl ( this, &MegaMapInscren, &TAMAPTAPos, ICONMAXWIDTH, ICONMAXHEIGHT, MegamapVirtualKey);

			Controler->InitSurface ( (IDirectDraw*)LocalShare->TADirectDraw);
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
	static int i= 10;

	if (Do_b
		&&Blit_b)
	{
		if (TAInGame==DataShare->TAProgress)
		{
			IDDrawSurface::OutptTxt ( "FullScreenMinimap blit");

			int CursorX= (*TAmainStruct_PtrPtr)->CurtMousePostion.x;
			int CursorY= (*TAmainStruct_PtrPtr)->CurtMousePostion.y;

			i++;
			if (10<i)
			{//
				if (!Flipping)
				{
					Flipping= TRUE;
					i= 0;
					POINT Aspect;
					LPBYTE PixelBits;

					if (MyMinimap_p)
					{
						MyMinimap_p->PictureInfo ( &PixelBits, &Aspect);

						Mapped_p->NowDrawMapped ( PixelBits, &Aspect);

						Mapped_p->PictureInfo (  &PixelBits, &Aspect);

						ProjectilesMap_p->NowDrawProjectile ( PixelBits, &Aspect);

						UnitsMap->NowDrawUnits ( PixelBits, &Aspect);


						GameDrawer->MixDSufInBlit ( &MegamapRect, UnitsMap->GetSurface ( ) , NULL);
						GameDrawer->Flip ( );
					}
			
					Flipping= FALSE;
				}
			
			}


			GameDrawer->BlitTAGameArea ( DestSurf);
			Controler->BlitSelect ( DestSurf);

			if ((CursorY!=-1)
				&&(CursorX!=-1))
			{
				Controler->DrawCursor ( DestSurf, CursorX, CursorY);
			}
		}
	}
}
void FullScreenMinimap::LockBlit (char * lpSurfaceMem, int dwWidth,int dwHeight, int lPitch)
{
	Controler->LockBlit ( lpSurfaceMem, dwWidth, dwHeight, lPitch);
}
void FullScreenMinimap::InitSurface (LPDIRECTDRAW TADD)
{
	if (Do_b
		&&(TALobby!=DataShare->TAProgress))
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
