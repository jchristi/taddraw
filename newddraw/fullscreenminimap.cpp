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

#include "fullscreenminimap.h"
#include "dialog.h"
#include "gaf.h"
#include "iddrawsurface.h"

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

	Blit_b= FALSE;

	Do_b= Doit;
	if (Doit)
	{
		LoadMap_hook= new InlineSingleHook ( 0x00483638, 5, INLINE_5BYTESLAGGERJMP, LoadMap_Routine);
		
		LoadMap_hook->SetParamOfHook ( (LPVOID)this);
		GameDrawer= new TAGameAreaReDrawer;
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
}

void FullScreenMinimap::InitMinimap (TNTHeaderStruct * TNTPtr, RECT * GameScreen)
{
	Blit_b= FALSE;
	if (MyMinimap_p)
	{
		delete MyMinimap_p;
		MyMinimap_p= NULL;
	}

	if (NULL==GameDrawer->OwnGameAreaSurface_ptr)
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
	}

	if (UnitsMap)
	{
		delete UnitsMap;
		UnitsMap= NULL;
	}

	if (MyMinimap_p)
	{
	
		UnitsMap= new UnitsMinimap (  this, MinimapAspect.x, MinimapAspect.y);
		UnitsMap->InitSurface ( (IDirectDraw*)LocalShare->TADirectDraw);
	}

	if (Mapped_p)
	{
		delete Mapped_p;
		Mapped_p= NULL;
	}

	if (MyMinimap_p)
	{
		Mapped_p= new MappedMap ( MinimapAspect.x, MinimapAspect.y);
	}
}

void FullScreenMinimap::Blit(LPDIRECTDRAWSURFACE DestSurf)
{
	static int i= 0;

	if (Do_b
		&&Blit_b)
	{
		if (TAInGame==DataShare->TAProgress)
		{
			IDDrawSurface::OutptTxt ( "FullScreenMinimap blit");

			int CursorX= (*TAmainStruct_PtrPtr)->CurtMousePostion.x;
			int CursorY= (*TAmainStruct_PtrPtr)->CurtMousePostion.y;


			i++;
			if (30<i)
			{//
				i= 0;

				POINT Aspect;
				LPBYTE PixelBits;
				RECT DescRect;
				BOOL NoMapped_b;
				GameDrawer->TAWGameAreaRect ( &DescRect);
				MyMinimap_p->PictureInfo ( &PixelBits, &Aspect);
				DescRect.right= DescRect.right- DescRect.left;
				DescRect.bottom= DescRect.bottom- DescRect.top;
				DescRect.top= 0;
				DescRect.left= 0;

				DescRect.right= Aspect.x;
				DescRect.bottom= Aspect.y;

				// 
				// 			if ((Aspect.x<DescRect.right)
				// 				&&(2<(DescRect.right- Aspect.x)))
				// 			{
				// 				DescRect.left= (DescRect.right- Aspect.x)/ 2;
				// 				DescRect.right= DescRect.left+ Aspect.x;
				// 			}
				// 
				// 			if ((Aspect.y<DescRect.bottom)
				// 				&&(2<(DescRect.bottom- Aspect.y)))
				// 			{
				// 				DescRect.top= (DescRect.bottom- Aspect.y)/ 2;
				// 				DescRect.bottom= DescRect.top+ Aspect.y;
				// 			}

				GameDrawer->MixBitsInBlit ( &DescRect, PixelBits, &Aspect, NULL);


				NoMapped_b= Mapped_p->NowDrawMapped ( );
				Mapped_p->PictureInfo (  &PixelBits, &Aspect);
				GameDrawer->GrayBlitOfBits ( &DescRect, PixelBits, &Aspect, NULL, NoMapped_b);
				//GameDrawer->MixBitsInBlit ( &DescRect, PixelBits, &Aspect, NULL);

				UnitsMap->NowDrawUnits ( );
				GameDrawer->MixDSufInBlit ( &DescRect, UnitsMap->GetSurface ( ) , NULL, 0xffffffff);
			}

			GameDrawer->BlitTAGameArea ( DestSurf);


			if ((CursorY!=-1)
				&&(CursorX!=-1))
			{
				((Dialog *)LocalShare->Dialog)->BlitCursor ( DestSurf, CursorX, CursorY);
			}
		}
	}
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
	
}

bool FullScreenMinimap::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LPRECT GameScreen;
	int xPos, yPos;



	if (TAInGame==DataShare->TAProgress)
	{
		switch(Msg)
		{
		case WM_KEYUP:
			if(wParam == 0x73	)
			{
				Blit_b= ! Blit_b;
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
}
//hook .text:00466DC0 000 83 EC 1C                                                        sub     esp, 1Ch
//  .text:00466E83 02C 0F BF 43 6C      movsx   eax, word ptr [ebx+(UnitsInGame.UnitPosition.X+2)] ; //0x6A

// 
// int __stdcall ReSet_proc (PInlineX86StackBuffer X86StrackBuffer)
// {
// 	//GUIExpander->myMinimap->Uni
// 	UnitsMinimap * class_ptr= (UnitsMinimap *)X86StrackBuffer->myInlineHookClass_Pish->ParamOfHook;
// 	POINT Aspect;
// 	LPBYTE PixelBits;
// 	if (class_ptr)
// 	{
// 		class_ptr->PictureInfo ( &PixelBits, &Aspect);
// 
// 		if (NULL==PixelBits)
// 		{
// 			RECT * GameRect= &(*TAmainStruct_PtrPtr)->GameSreen_Rect;
// 			Aspect.x= GameRect->right- GameRect->left;
// 			Aspect.y= GameRect->bottom- GameRect->top;
// 			PixelBits= class_ptr->Init ( Aspect.x, Aspect.y);
// 		}
// 		memset ( PixelBits, 0xffffff, Aspect.x* Aspect.y);
// 	}
// 
// 	
// 	return 0;
// }
// 
// int __stdcall drawunit_proc (PInlineX86StackBuffer X86StrackBuffer)
// {
// 	//
// 	UnitsMinimap * class_ptr= (UnitsMinimap *)X86StrackBuffer->myInlineHookClass_Pish->ParamOfHook;
// 
// 	UnitStruct * unitPtr= (UnitStruct *)X86StrackBuffer->Ebx;
// 
// 	if (class_ptr)
// 	{
// 		class_ptr->DrawUnit (  unitPtr->PlayerStruct_index, unitPtr->XPos, unitPtr->YPos);
// 	}
// 	
// 	return 0;
//}
// 
// DWORD WINAPI UnitMapDrawerProc( LPVOID argc)
// {
// 	IDDrawSurface::OutptTxt ( "UnitMapDrawer Thread Start");
// 	UnitsMinimap * class_ptr= (UnitsMinimap *)argc;
// 	if (class_ptr)
// 	{
// 		POINT Aspect;
// 		LPBYTE PixelBits;
// 		while (true)
// 		{
// 			if ((TAInGame==DataShare->TAProgress))
// 			{
// 				class_ptr->LockOn ( &PixelBits, &Aspect);
// 
// 				if (NULL!=PixelBits)
// 				{
// 					memset ( PixelBits, 0xffffff, Aspect.x* Aspect.y);
// 
// 					UnitStruct * unitPtr= (*TAmainStruct_PtrPtr)->OwnUnitBegin;
// 					UnitStruct * unitEndPtr= (*TAmainStruct_PtrPtr)->OwnUnitEnd;
// 
// 					for (; unitPtr!=unitEndPtr; ++unitPtr)
// 					{
// 						class_ptr->DrawUnit (  PixelBits, &Aspect, unitPtr->PlayerStruct_index, unitPtr->XPos, unitPtr->YPos);
// 					}
// 				}
// 				class_ptr->Unlock ( PixelBits);
// 			}
// 			Sleep ( 500); 
// 		}
// 	}
// 	return 0;
// }

UnitsMinimap::UnitsMinimap (FullScreenMinimap * inheritFrom, int Width, int Height)
{
	parent= inheritFrom;

	UnitsMapSfc= NULL;
	for (int i= 0; i<PLAYERNUM; ++i)
	{
		PlayerBits[i]= NULL;


		PlayerAspect[i].x= 0;
		PlayerAspect[i].y= 0;

		PlayerColros[i]= 0xff;
	}

	ReSet ( Width, Height);
// 	
// 	ReSet_hook= new InlineSingleHook ( 0x0466DC0, 0x5, INLINE_5BYTESLAGGERJMP, ReSet_proc);
// 	DrawUnit_hook= new InlineSingleHook ( 0x0466E83, 0x5, INLINE_5BYTESLAGGERJMP, drawunit_proc);
}

UnitsMinimap::UnitsMinimap (FullScreenMinimap * inheritFrom)
{
	parent= inheritFrom;
	UnitsMapSfc= NULL;

	for (int i= 0; i<PLAYERNUM; ++i)
	{
		PlayerBits[i]= NULL;


		PlayerAspect[i].x= 0;
		PlayerAspect[i].y= 0;

		PlayerColros[i]= 0xff;
	}
	ReSet ( 0, 0);
// 	ReSet_hook= new InlineSingleHook ( 0x0466DC0, 0x5, INLINE_5BYTESLAGGERJMP, ReSet_proc);
// 	DrawUnit_hook= new InlineSingleHook ( 0x0466E83, 0x5, INLINE_5BYTESLAGGERJMP, drawunit_proc);
//	ReSet_hook->SetParamOfHook ( (LPVOID)this);
}

UnitsMinimap::~UnitsMinimap()
{
	ReSet ( 0, 0);
//	if (DrawUnit_hook)
// 	{
// 		delete DrawUnit_hook;
// 	}
// 	if (ReSet_hook)
// 	{
// 		delete ReSet_hook;
// 	}
}

void UnitsMinimap::ReSet (int Width, int Height)
{
//	DWORD TID;
	Width_m= Width;
	Height_m= Height;

	if (UnitsMapSfc)
	{
		UnitsMapSfc->Release ( );
		UnitsMapSfc= NULL;
	}

	int i;
	for (i= 0; i<PLAYERNUM; ++i)
	{
		if (PlayerBits[i])
		{
			free ( PlayerBits[i]);
			PlayerBits[i]= NULL;
		}

		if (0!=PlayerAspect[i].x)
		{
			PlayerAspect[i].x= 0;
			PlayerAspect[i].y= 0;
		}
		PlayerColros[i]= 0xff;
	}
// 	Syncetux= CreateMutexA ( NULL, FALSE, NULL);
// 	if (INVALID_HANDLE_VALUE!=Syncetux)
// 	{
// 		Thd_h= CreateThread ( NULL, 0, UnitMapDrawerProc, (LPVOID)this, 0, &TID);
// 	}

	return;
}

LPDIRECTDRAWSURFACE UnitsMinimap::InitSurface (LPDIRECTDRAW TADD)
{
	IDDrawSurface::OutptTxt ( "UnitsMap Surface Init");
	if (NULL!=UnitsMapSfc)
	{
		UnitsMapSfc->Release ( );
		UnitsMapSfc= NULL;
	}

	if (TADD)
	{
		if ((0==Width_m)
			||(0==Height_m))
		{
			return NULL;
		}
		
		DDSURFACEDESC ddsd;
		DDRAW_INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth = Width_m;
		ddsd.dwHeight = Height_m;

		TADD->CreateSurface(&ddsd, &UnitsMapSfc, NULL);
	}

	return UnitsMapSfc;
}
 

void UnitsMinimap::ReleaseSurface (void)
{
	if (NULL!=UnitsMapSfc)
	{
		UnitsMapSfc->Release ( );
	}
	UnitsMapSfc= NULL;
}

// void UnitsMinimap::LockEvent (void)
// {
// 	WaitForSingleObject ( Syncetux, INFINITE);
// }
// void UnitsMinimap::UnLockEvent (void)
// {
// 	ReleaseMutex ( Syncetux);
// }
// 
LPBYTE UnitsMinimap::LockOn (LPBYTE * PixelBits_pp, POINT * Aspect)
{
	
	//LockEvent ( );
	DDSURFACEDESC ddsd;
	DDRAW_INIT_STRUCT( ddsd);
	if (UnitsMapSfc)
	{
		if(UnitsMapSfc->IsLost() != DD_OK)
		{
			if(UnitsMapSfc->Restore() != DD_OK)
				return NULL;
		}

		if (DD_OK==UnitsMapSfc->Lock ( NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL))
		{
			if (PixelBits_pp)
			{
				*PixelBits_pp= (LPBYTE )ddsd.lpSurface;
			}

			if (Aspect)
			{
				Aspect->x= ddsd.lPitch;
				Aspect->y= ddsd.dwHeight;
			}
		}
	}
	if (PixelBits_pp)
	{
		return *PixelBits_pp;
	}
	else
	{
		return (LPBYTE )ddsd.lpSurface;
	}
	
}

void UnitsMinimap::Unlock ( LPBYTE PixelBits_p)
{
	if (UnitsMapSfc)
	{
		UnitsMapSfc->Unlock ( reinterpret_cast<LPVOID>(PixelBits_p));
	}
	//UnLockEvent ( );
}

LPDIRECTDRAWSURFACE UnitsMinimap::GetSurface (void)
{
	if (UnitsMapSfc)
	{
		if (DD_OK!=UnitsMapSfc->IsLost ( ))
		{
			UnitsMapSfc->Restore ( );
		}
	}
	return UnitsMapSfc;
}

LPBYTE UnitsMinimap::UnitPicture(int PlayerID, LPBYTE * PixelBits_pp, POINT * Aspect)
{
	if ((PLAYERNUM<PlayerID)
		||(PlayerID<0))
	{
		return NULL;
	}
	int CurtColor= (*TAmainStruct_PtrPtr)->Players[PlayerID].PlayerInfo->PlayerLogoColor;
	if (NULL==PlayerBits[PlayerID])
	{
		PGAFFrame UnitPic= Index2Frame_InSequence ( (*TAmainStruct_PtrPtr)->radlogo, CurtColor);
		InstanceGAFFrame ( UnitPic, &PlayerBits[PlayerID], &PlayerAspect[PlayerID]);
	}
	if (PlayerColros[PlayerID]!=CurtColor)
	{
		free ( PlayerBits[PlayerID]);
		PlayerAspect[PlayerID].x= 0;
		PlayerAspect[PlayerID].y= 0;

		PGAFFrame UnitPic= Index2Frame_InSequence ( (*TAmainStruct_PtrPtr)->radlogo, CurtColor);
		InstanceGAFFrame ( UnitPic, &PlayerBits[PlayerID], &PlayerAspect[PlayerID]);
	}

	if (PixelBits_pp)
	{
		*PixelBits_pp= PlayerBits[PlayerID];
	}
	if (Aspect)
	{
		Aspect->x= PlayerAspect[PlayerID].x;
		Aspect->y= PlayerAspect[PlayerID].y;
	}

	return PlayerBits[PlayerID];
}
void UnitsMinimap::DrawUnit ( LPBYTE PixelBits, POINT * Aspect, int  PlayerID, unsigned int TAx, unsigned int TAy)
{
	if ((NULL==PixelBits)
		||(NULL==Aspect))
	{
		return ;
	}
	

	POINT GafAspect= {0, 0};
	LPBYTE GafPixelBits= NULL;
	RECT DescRect;
	int DescHeight_I;
	int DescWidth_I;
	int DesclPitch= Aspect->x;

	//parent->GameDrawer->TAWGameAreaRect ( &DescRect);


	UnitPicture( PlayerID, &GafPixelBits, &GafAspect);
	DescRect.left= static_cast<int>(static_cast<float>(TAx)* (static_cast<float>(Width_m)/ static_cast<float>((*TAmainStruct_PtrPtr)->MapSizeX)))-  GafAspect.x/ 2;
	DescRect.right= DescRect.left+ GafAspect.x/ 2;
	DescRect.top= static_cast<int>(static_cast<float>(TAy)* (static_cast<float>(Height_m)/ static_cast<float>((*TAmainStruct_PtrPtr)->MapSizeY)))-  GafAspect.y/ 2;
	DescRect.bottom= DescRect.top+ GafAspect.y/ 2;

	Aspect->x= (Aspect->x)/ 4* 4;// avoid draw out of the surface, this x== pitch
	if ((DescRect.right<0)
		||((Aspect->x)<DescRect.left)
		||(Aspect->y<DescRect.top)
		||(DescRect.bottom<0))
	{// out of map
		return ;
	}

	// do not draw out of the map part
	if (DescRect.left<0)
	{
		DescRect.left= 0;
	}
	if (Aspect->x<DescRect.right)
	{
		DescRect.right= Aspect->x;
	}
	if (DescRect.top<0)
	{
		DescRect.top= 0;
	}
	if (Aspect->y<DescRect.bottom)
	{
		DescRect.bottom= Aspect->y;
	}

	DescHeight_I= DescRect.bottom- DescRect.top;
	DescWidth_I= DescRect.right- DescRect.left;

	try
	{
		for (int YPos= 0; YPos<DescHeight_I; YPos++)
		{	//Y
			int DescPixelYStart= (YPos+ DescRect.top)* (DesclPitch);
			int SrcPixelYStart= YPos* GafAspect.x;
			for (int XPos= 0; XPos<DescWidth_I; XPos++)
			{//X 
				PixelBits[DescPixelYStart+ (XPos+ DescRect.left)]= GafPixelBits[SrcPixelYStart+ XPos];
			}
		}
	}
	catch (...)
	{
		
	}
}

void UnitsMinimap::NowDrawUnits ( void)
{

	if (TAInGame!=DataShare->TAProgress)
	{
		return ;
	}

		IDDrawSurface::OutptTxt ( "Draw units");
		POINT Aspect= {0, 0};
		LPBYTE PixelBits= NULL;

		LockOn ( &PixelBits, &Aspect);
		
		if ((TAInGame==DataShare->TAProgress))
		{

			if (NULL!=PixelBits)
			{
				memset ( PixelBits, 0xffffffff, Aspect.x* Aspect.y);

				UnitStruct * Begin= (*TAmainStruct_PtrPtr)->OwnUnitBegin;
				UnitStruct * unitPtr;

				int NumHotRadarUnits= (*TAmainStruct_PtrPtr)->NumHotRadarUnits;
				RadarUnit_ * RadarUnits_v= (*TAmainStruct_PtrPtr)->RadarUnits;

				
				for (int i= 0; i<NumHotRadarUnits; ++i)
				{
					unitPtr= &Begin[RadarUnits_v[i].ID];
					if (0!=unitPtr->UnitID)
					{
						DrawUnit (  PixelBits, &Aspect, unitPtr->Owner_PlayerPtr0->PlayerAryIndex, unitPtr->XPos,  unitPtr->YPos- (unitPtr->ZPos));  (Height_m/ ((*TAmainStruct_PtrPtr)->RadarPicSizeY));
					}
				}
			
// 				if (NOMAPPING!=(NOMAPPING&((*TAmainStruct_PtrPtr)->LosType))
// 					&&Permanent!=(Permanent&((*TAmainStruct_PtrPtr)->LosType)))
// 				{
// 					for (; unitPtr!=unitEndPtr; ++unitPtr)
// 					{
// 						if (0!=unitPtr->UnitID)
// 						{
// 							DrawUnit (  PixelBits, &Aspect, unitPtr->Owner_PlayerPtr0->PlayerAryIndex, unitPtr->XPos,  unitPtr->YPos- (unitPtr->ZPos));  //(Height_m/ ((*TAmainStruct_PtrPtr)->RadarPicSizeY));
// 						}
// 					}
// 				}
// 				else
// 				{
// 					for (; unitPtr!=unitEndPtr; ++unitPtr)
// 					{
// 						if (0!=unitPtr->UnitID)
// 						{
// 							if (((*TAmainStruct_PtrPtr)->LOS_Sight_PlayerID==unitPtr->myLos_PlayerID)
// 								||(unitPtr->Owner_PlayerPtr0->AllyFlagAry[(*TAmainStruct_PtrPtr)->LOS_Sight_PlayerID]))
// 							{
// 								DrawUnit (  PixelBits, &Aspect, unitPtr->Owner_PlayerPtr0->PlayerAryIndex, unitPtr->XPos,  unitPtr->YPos- (unitPtr->ZPos));  //(Height_m/ ((*TAmainStruct_PtrPtr)->RadarPicSizeY));
// 							}
// 							else
// 							{// not ally or my unit
// 
// 							}
// 						}
// 					}
// 				}
			}
		}
		Unlock ( PixelBits);
	
}


MappedMap::MappedMap (int Width, int Height)
{
	Width_m= Width;
	Height_m= Height;
	MappedBits= static_cast<LPBYTE>(malloc ( Width_m* Height_m+ 1));
}

MappedMap::~MappedMap()
{
	if (MappedBits)
	{
		free ( MappedBits);
	}
}

BOOL MappedMap::NowDrawMapped (void)
{
	if (TAInGame!=DataShare->TAProgress)
	{
		return FALSE;
	}
	IDDrawSurface::OutptTxt ( "Draw Mapped");

	memset ( MappedBits, 1, Width_m* Height_m);

	if (NOMAPPING==(NOMAPPING&((*TAmainStruct_PtrPtr)->LosType)))
	{//
		;
		if (Permanent==(Permanent&((*TAmainStruct_PtrPtr)->LosType)))
		{// total visual 
			;
		}
		else
		{//
			int PlayerID= (*TAmainStruct_PtrPtr)->LocalHumanPlayer_PlayerID;
			PlayerStruct * Player_p= &((*TAmainStruct_PtrPtr)->Players[PlayerID]);
			int MapX= ((*TAmainStruct_PtrPtr)->FeatureMapSizeX)/ 2;
			int MapY= ((*TAmainStruct_PtrPtr)->FeatureMapSizeY)/ 2;
			LPWORD MappedMemory_p= (*TAmainStruct_PtrPtr)->MAPPED_MEMORY_p;

			float XScale= (static_cast<float>(MapX)/ static_cast<float>(Width_m));
			float YScale= (static_cast<float>(MapY)/ static_cast<float>(Height_m));
			float MAPPEDMEM_h, MAPPEDMEM_w;
			int i, j;
			for	( i= 0, MAPPEDMEM_h= 0.0; i<Height_m; ++i, MAPPEDMEM_h= MAPPEDMEM_h+ YScale)
			{
				int YOff= i* Width_m;
				int LosBitYOff=  static_cast<int>(MAPPEDMEM_h)* MapX;

				for	( j= 0, MAPPEDMEM_w= 0.0; j<Width_m; ++j, MAPPEDMEM_w= MAPPEDMEM_w+ XScale)
				{
					MappedBits[YOff+ j]= ((1<<PlayerID)& MappedMemory_p[LosBitYOff+ static_cast<int>(MAPPEDMEM_w)])>> PlayerID;
				}
			}

			return TRUE;
		}
	}
	else
	{
		PlayerStruct * Player_p= &((*TAmainStruct_PtrPtr)->Players[(*TAmainStruct_PtrPtr)->LocalHumanPlayer_PlayerID]);
		int MapX= Player_p->LOS_Tilewidth;
		int MapY= Player_p->LOS_Tileheight;
		LPBYTE PlayerLosBits= Player_p->LOS_MEMORY_p;

		float XScale= static_cast<float>(MapX)/ static_cast<float>(Width_m);
		float YScale= static_cast<float>(MapY)/ static_cast<float>(Height_m);
		float Los_w, Los_h;
		int i, j;
		for	( i= 0, Los_h= 0.0; i<Height_m; ++i, Los_h= Los_h+ YScale)
		{
			int YOff= i* Width_m;
			
			int LosBitYOff=  static_cast<int>(Los_h)* MapX;

			for	( j= 0, Los_w= 0.0; j<Width_m; ++j, Los_w= Los_w+ XScale)
			{
				MappedBits[YOff+ j]= PlayerLosBits[LosBitYOff+ static_cast<int>(Los_w)];
			}
		}
	}

	return FALSE;
}

LPBYTE MappedMap::PictureInfo (LPBYTE * PixelBits_pp, POINT * Aspect)
{
	if (PixelBits_pp)
	{
		*PixelBits_pp= MappedBits;
	}

	if (Aspect)
	{
		Aspect->x= Width_m;
		Aspect->y= Height_m;
	}

	return MappedBits;
}