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

#include "fullscreenminimap.h"
#include "dialog.h"
#include "gaf.h"
#include "iddrawsurface.h"

#include <vector>
using namespace std;
#include "TAConfig.h"
#include "ExternQuickKey.h"

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
	Inited= FALSE;
	for (int i= 0; i<PLAYERNUM; ++i)
	{
		PlayerBits[i]= NULL;


		PlayerAspect[i].x= 0;
		PlayerAspect[i].y= 0;

		PlayerColros[i]= 0xff;
	}
		
	ReSet ( Width, Height);
	// LoadUnitPicture ( );
	// 	
	// 	ReSet_hook= new InlineSingleHook ( 0x0466DC0, 0x5, INLINE_5BYTESLAGGERJMP, ReSet_proc);
	// 	DrawUnit_hook= new InlineSingleHook ( 0x0466E83, 0x5, INLINE_5BYTESLAGGERJMP, drawunit_proc);
}

UnitsMinimap::UnitsMinimap (FullScreenMinimap * inheritFrom)
{
	parent= inheritFrom;
	UnitsMapSfc= NULL;
	Inited= FALSE;

	for (int i= 0; i<PLAYERNUM; ++i)
	{
		PlayerBits[i]= NULL;


		PlayerAspect[i].x= 0;
		PlayerAspect[i].y= 0;

		PlayerColros[i]= 0xff;
	}
	ReSet ( 0, 0);
	// LoadUnitPicture ( );
	// 	ReSet_hook= new InlineSingleHook ( 0x0466DC0, 0x5, INLINE_5BYTESLAGGERJMP, ReSet_proc);
	// 	DrawUnit_hook= new InlineSingleHook ( 0x0466E83, 0x5, INLINE_5BYTESLAGGERJMP, drawunit_proc);
	//	ReSet_hook->SetParamOfHook ( (LPVOID)this);
}

UnitsMinimap::~UnitsMinimap()
{
	ReSet ( 0, 0);

	FreeUnitPicture ( );
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

void UnitsMinimap::InitPicturePlayerColors (int PlayeyColor, int FillColor, LPBYTE * * PPCptr)
{
	IDDrawSurface::OutptTxt( "InitPictureColors");
	LPBYTE * PPC;
	BOOL Replace;
	LPBYTE ptr;
	if (*PPCptr)
	{
		PPC= *PPCptr;
		Replace= TRUE;
	}
	else
	{
		PPC= (LPBYTE *)malloc ( sizeof(LPBYTE)* MaskNum);
		memset ( PPC, 0, sizeof(LPBYTE)* MaskNum);
		Replace= FALSE;
	}


	for (int i= 0; i<MaskNum; ++i)
	{
		if (PicturesBits[i])
		{
			int Width= PicturesBits[i]->Width();
			int Height= PicturesBits[i]->Height();
			LPBYTE bits= PicturesBits[i]->Data();
			if (!Replace)
			{
				PPC[i]= (LPBYTE)malloc ( PicturesBits[i]->BufSize());
			}

			if (NULL!=PPC[i])
			{
				ptr= PPC[i];
				for (int j= 0; j<Height; ++j)
				{
					int Line= j* Width;
					for (int y= 0; y<Width; ++y)
					{
						register int Color= bits[Line+ y];
						if (FillColor==Color)
						{
							Color= PlayeyColor;
						}

						ptr[Line+ y]= Color;
					}
				}
			}

		}
	}

	*PPCptr= PPC;
}

void UnitsMinimap::InitUnSelectedPicturePlayerColors (int PlayeyColor, int FillColor, int UnSelectedColor,int TransparentColor,  
	LPBYTE * * PPCptr)
{
	IDDrawSurface::OutptTxt( "InitUnSelectedPictureColors");
	LPBYTE * PPC;
	BOOL Replace;
	LPBYTE ptr;
	if (*PPCptr)
	{
		PPC= *PPCptr;
		Replace= TRUE;
	}
	else
	{
		PPC= (LPBYTE *)malloc ( sizeof(LPBYTE)* MaskNum);
		memset ( PPC, 0, sizeof(LPBYTE)* MaskNum);
		Replace= FALSE;
	}


	for (int i= 0; i<MaskNum; ++i)
	{
		if (PicturesBits[i])
		{
			int Width= PicturesBits[i]->Width();
			int Height= PicturesBits[i]->Height();
			LPBYTE bits= PicturesBits[i]->Data();
			if (!Replace)
			{
				PPC[i]= (LPBYTE)malloc ( PicturesBits[i]->BufSize());
			}

			if (NULL!=PPC[i])
			{
				ptr= PPC[i];
				for (int j= 0; j<Height; ++j)
				{
					int Line= j* Width;
					for (int y= 0; y<Width; ++y)
					{
						register int Color= bits[Line+ y];

						if (UnSelectedColor==Color)
						{
							Color= TransparentColor;
						}
						if (FillColor==Color)
						{
							Color= PlayeyColor;
						}

						ptr[Line+ y]= Color;
					}
				}
			}

		}
	}
	*PPCptr= PPC;
}


void UnitsMinimap::LoadUnitPicture ( void)
{
	if (Inited)
	{
		return ;
	}
	Inited= TRUE;
	

	MaskAry= NULL;
	PicturesBits= NULL;
	for (int i= 0; i<PLAYERNUM; ++i)
	{
		PicturesPlayerColors[i]= NULL;
	}

	for (int i= 0; i<PLAYERNUM; ++i)
	{
		UnSelectedPicturesPlayerColors[i]= NULL;
	}

	MaskNum= 0;

	UseDefaultIcon= FALSE;
	UnknowMaskIndex= 0;
	NothingMaskIndex= 0;

	FillColor= DEFAULTFILLCOLOR;
	TransparentColor= DEFAULTTRANSPARENTCOLOR;
	UnSelectedColor= DEFAULTUNSELECTEDCOLOR;

	char ConfigFileName[MAX_PATH]={0};
	char TAPath[MAX_PATH]= {0};
	char ConfigFilePath[MAX_PATH]= {0};
	char * Aftequ;

	MyConfig->GetIniStr ( "MegaMapConfig", ConfigFileName, MAX_PATH, NULL);
	clean_remark (  ConfigFileName, ';');
	if (0!=ConfigFileName[0])
	{
		GetCurrentDirectoryA  ( MAX_PATH, TAPath);
		wsprintfA ( ConfigFilePath, "%s\\%s", TAPath, ConfigFileName);
		if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
		{
			// valid
			FillColor= GetPrivateProfileIntA (  "Option", "FillColor", DEFAULTFILLCOLOR, ConfigFilePath);
			TransparentColor= GetPrivateProfileIntA (  "Option", "TransparentColor", DEFAULTTRANSPARENTCOLOR, ConfigFilePath);
			UnSelectedColor= GetPrivateProfileIntA (  "Option", "SelectedColor", DEFAULTUNSELECTEDCOLOR, ConfigFilePath);
			
			GetPrivateProfileStringA ( "Option", "UseDefaultIcon", "true", ConfigFileName , MAX_PATH, ConfigFilePath);
			_strlwr_s ( ConfigFileName , MAX_PATH);
			if (NULL==strstr ( ConfigFileName, "true"))
			{// if not null, use default
				char * CategoryNameAry= (char *)malloc ( 0x10000);

				if (CategoryNameAry)
				{

					int CfgSize= GetPrivateProfileSection ( "Icon", CategoryNameAry, 0x10000, ConfigFilePath);

					for (int i= 0, end= 0; i<CfgSize; i= i+ end+ 1)
					{
						end= strlen ( &CategoryNameAry[i]);
						MaskNum++;
					}
					if (0<MaskNum)
					{
						*strrchr ( ConfigFilePath, '\\')= 0;
						MaskAry= (LPDWORD* )malloc ( sizeof(LPDWORD)* (MaskNum+ RACENUMBER));
						memset ( MaskAry, 0, sizeof(LPDWORD)* (MaskNum+ RACENUMBER));
						PicturesBits= (UnitIcon * *)malloc ( sizeof(UnitIcon *)* (MaskNum+ RACENUMBER));
						memset ( PicturesBits, 0, sizeof(UnitIcon *)* (MaskNum+ RACENUMBER));

						MaskNum= 0;


						//------------  Load Commander
						for (int i= 0; i<RACENUMBER; ++i)
						{
							if ('\0'!=myExternQuickKey->Commanders[i][0])
							{
								wsprintfA ( ConfigFileName, "%s\\%s.PCX", ConfigFilePath, &myExternQuickKey->Commanders[i][0]);
								if (0xffffffff!=GetFileAttributesA ( ConfigFileName))
								{
									MaskAry[MaskNum]= myExternQuickKey->CommandersMask[i];
									PicturesBits[MaskNum]= new UnitIcon( ConfigFileName);
									MaskNum++;
								}
							}
						}
						//--------------
						

						for (int i= 0, end= 0; i<CfgSize; i= i+ end+ 1)
						{
							end= strlen ( &CategoryNameAry[i]);

							if (0<end)
							{
								Aftequ= strchr ( &CategoryNameAry[i], '=');
								if (Aftequ)
								{
									*(Aftequ)= '\0';
									Aftequ= &Aftequ[1];
									
									if (0==strcmp ( &CategoryNameAry[i], "nothing"))
									{
										NothingMaskIndex= MaskNum;
									}
									else if (0==strcmp ( &CategoryNameAry[i], "unknow"))
									{
										UnknowMaskIndex= MaskNum;
									}
									else
									{
										MaskAry[MaskNum]= GetUnitIDMaskAryByCategory ( trim_crlf_( &CategoryNameAry[i]));
									}
									wsprintfA ( ConfigFileName, "%s\\%s", ConfigFilePath, Aftequ);
									clean_remark (  ConfigFileName, ';');
									PicturesBits[MaskNum]= new UnitIcon( ConfigFileName);
									MaskNum++;
								}
							}
						}

					}
					free ( CategoryNameAry);
				}
				return ;
			}

		}
	
	}
	//else if (MyConfig->GetIniBool( "MegaMapUseDefaultIconSet", TRUE))
	UseDefaultIcon= TRUE;

	GetCurrentDirectoryA  ( MAX_PATH, TAPath);

	MaskAry= (LPDWORD* )malloc ( sizeof(LPDWORD)* (9+ RACENUMBER));
	memset ( MaskAry, 0, sizeof(LPDWORD)* (9+ RACENUMBER));
	PicturesBits= (UnitIcon * *)malloc ( sizeof(UnitIcon *)* (9+ RACENUMBER));
	memset ( PicturesBits, 0, sizeof(UnitIcon *)* (9+ RACENUMBER));


	//------------  Load Commander
	for (int i= 0; i<RACENUMBER; ++i)
	{
		if ('\0'!=myExternQuickKey->Commanders[i][0])
		{
			wsprintfA ( ConfigFilePath, "%s\\Icon\\%s.PCX", TAPath, &myExternQuickKey->Commanders[i][0]);
			if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
			{
				MaskAry[MaskNum]= myExternQuickKey->CommandersMask[i];
				PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);
				MaskNum++;
			}
		}
	}
	//--------------
	

	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath, COMMICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		MaskAry[MaskNum]= myExternQuickKey->CommanderMask;
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);
		MaskNum++;
	}

	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath, CONICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		MaskAry[MaskNum]= myExternQuickKey->ConstructorMask;
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);
		MaskNum++;
	}

	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath, MOBILEICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		MaskAry[MaskNum]= myExternQuickKey->MobileWeaponMask;
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);
		MaskNum++;
	}



	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath, FCTYICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		MaskAry[MaskNum]= myExternQuickKey->FactoryMask;
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);
		MaskNum++;
	}


	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath, BLDGICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		MaskAry[MaskNum]= myExternQuickKey->BuildingMask;
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);
		MaskNum++;
	}


	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath, AIRICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		MaskAry[MaskNum]= myExternQuickKey->AirWeaponMask;
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);
		MaskNum++;
	}

	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath,AIRCONICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		MaskAry[MaskNum]= myExternQuickKey->AirConMask;
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);
		MaskNum++;
	}

	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath, NOTHINGICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);

		NothingMaskIndex= MaskNum;
		MaskNum++;
	}

	wsprintfA ( ConfigFilePath, "%s\\Icon\\%s", TAPath, UNKNOWICON);
	if (0xffffffff!=GetFileAttributesA ( ConfigFilePath))
	{
		PicturesBits[MaskNum]= new UnitIcon( ConfigFilePath);

		UnknowMaskIndex= MaskNum;
		MaskNum++;
	}

}

void UnitsMinimap::FreeUnitPicture ( void)
{	
	if (MaskAry)
	{
		free ( MaskAry);
		MaskAry= NULL;
	}

	if (PicturesBits)
	{
		for (int i= 0; i<MaskNum; ++i)
		{
			delete PicturesBits[i];
			PicturesBits[i]= NULL;
		}

		free  ( PicturesBits);
		PicturesBits= NULL;
	}


	for (int i= 0; i<PLAYERNUM; ++i)
	{
		if (PicturesPlayerColors[i])
		{
			for (int j= 0; j<MaskNum; ++j)
			{
				if (PicturesPlayerColors[i][j])
				{
					free ( PicturesPlayerColors[i][j]);
				}
			}
		}
		PicturesPlayerColors[i]= NULL;
	}



	for (int i= 0; i<PLAYERNUM; ++i)
	{
		if (UnSelectedPicturesPlayerColors[i])
		{
			for (int j= 0; j<MaskNum; ++j)
			{
				if (UnSelectedPicturesPlayerColors[i][j])
				{
					free ( UnSelectedPicturesPlayerColors[i][j]);
				}
			}
		}
		UnSelectedPicturesPlayerColors[i]= NULL;
	}

	for (int i= 0; i<PLAYERNUM; ++i)
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

	MaskNum= 0;
	UnknowMaskIndex= 0;

	Inited= FALSE;
}

LPBYTE UnitsMinimap::UnitPicture(UnitStruct * unitPtr,int PlayerID, LPBYTE * PixelBits_pp, POINT * Aspect)
{
	if ((PLAYERNUM<PlayerID)
		||(PlayerID<0))
	{
		return NULL;
	}

	BOOL Update_b= FALSE;
	int CurtColor= (*TAmainStruct_PtrPtr)->Players[PlayerID].PlayerInfo->PlayerLogoColor;
	if (PlayerColros[PlayerID]!=CurtColor)
	{
		PlayerColros[PlayerID]= CurtColor;
		Update_b= TRUE;
	}

	//
	if (0<MaskNum)
	{
		switch(CurtColor)
		{
		case 0:
			CurtColor = (char)227;
			break;
		case 1:
			CurtColor = (char)212;
			break;
		case 2:
			CurtColor = (char)80;
			break;
		case 3:
			CurtColor = (char)235;
			break;
		case 4:
			CurtColor = (char)108;
			break;
		case 5:
			CurtColor = (char)219;
			break;
		case 6:
			CurtColor = (char)208;
			break;
		case 7:
			CurtColor = (char)93;
			break;
		case 8:
			CurtColor = (char)130;
			break;
		case 9:
			CurtColor = (char)67;
			break;
		}

		if ((NULL==PicturesPlayerColors[PlayerID])
			||(Update_b))
		{// Init player color

			InitPicturePlayerColors ( CurtColor, FillColor, &PicturesPlayerColors[PlayerID]);
		}

		if ((NULL==UnSelectedPicturesPlayerColors[PlayerID])
			||(Update_b))
		{// Init Unslected player color
			InitUnSelectedPicturePlayerColors ( CurtColor, FillColor, UnSelectedColor, TransparentColor, &UnSelectedPicturesPlayerColors[PlayerID]);
		}

		if (1==CheckUnitInPlayerLOS ( &(*TAmainStruct_PtrPtr)->Players[(*TAmainStruct_PtrPtr)->LOS_Sight_PlayerID], unitPtr))
		{
			if (0!=(0x10& unitPtr->UnitSelected))
			{
				for (int i= 0; i<MaskNum; ++i)
				{
					if ((i!=UnknowMaskIndex)
						&&(i!=NothingMaskIndex)  // avoid null mask
						&&(MatchInTypeAry ( unitPtr->UnitID, MaskAry[i])))
					{
						if (PixelBits_pp)
						{
							*PixelBits_pp= PicturesPlayerColors[PlayerID][i];
						}
						if (Aspect)
						{
							Aspect->x=  PicturesBits[i]->Width ( );
							Aspect->y=  PicturesBits[i]->Height ( );
						}
						return PicturesPlayerColors[PlayerID][i];
					}
				}
				if (PixelBits_pp)
				{
					*PixelBits_pp= PicturesPlayerColors[PlayerID][UnknowMaskIndex];
				}
				if (Aspect)
				{
					Aspect->x=  PicturesBits[UnknowMaskIndex]->Width ( );
					Aspect->y=  PicturesBits[UnknowMaskIndex]->Height ( );
				}
				return PicturesPlayerColors[PlayerID][UnknowMaskIndex];
			}
			else
			{//UnSelectedPicturesPlayerColors
				for (int i= 0; i<MaskNum; ++i)
				{
					if ((i!=UnknowMaskIndex)
						&&(i!=NothingMaskIndex)  // avoid null mask
						&&(MatchInTypeAry ( unitPtr->UnitID, MaskAry[i])))
					{
						if (PixelBits_pp)
						{
							*PixelBits_pp= UnSelectedPicturesPlayerColors[PlayerID][i];
						}
						if (Aspect)
						{
							Aspect->x=  PicturesBits[i]->Width ( );
							Aspect->y=  PicturesBits[i]->Height ( );
						}
						return UnSelectedPicturesPlayerColors[PlayerID][i];
					}
				}
				if (PixelBits_pp)
				{
					*PixelBits_pp= UnSelectedPicturesPlayerColors[PlayerID][UnknowMaskIndex];
				}
				if (Aspect)
				{
					Aspect->x=  PicturesBits[UnknowMaskIndex]->Width ( );
					Aspect->y=  PicturesBits[UnknowMaskIndex]->Height ( );
				}
				return UnSelectedPicturesPlayerColors[PlayerID][UnknowMaskIndex];
			}
		}
		if (PixelBits_pp)
		{
			*PixelBits_pp= PicturesPlayerColors[PlayerID][NothingMaskIndex];
		}
		if (Aspect)
		{
			Aspect->x=  PicturesBits[NothingMaskIndex]->Width ( );
			Aspect->y=  PicturesBits[NothingMaskIndex]->Height ( );
		}
		return PicturesPlayerColors[PlayerID][NothingMaskIndex];
	}

	if ((NULL==PlayerBits[PlayerID])
		||Update_b)
	{
		if (NULL!=PlayerBits[PlayerID])
		{
			free ( PlayerBits[PlayerID]);
			PlayerBits[PlayerID]= NULL;
		}


		PlayerAspect[PlayerID].x= 0;
		PlayerAspect[PlayerID].y= 0;

		PGAFFrame UnitPic= Index2Frame_InSequence ( (*TAmainStruct_PtrPtr)->radlogo, PlayerColros[PlayerID]);
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


void UnitsMinimap::DrawUnit ( LPBYTE PixelBits, POINT * Aspect, UnitStruct * unitPtr)
{
	if ((NULL==PixelBits)
		||(NULL==Aspect))
	{
		return ;
	}
	int PlayerID= unitPtr->Owner_PlayerPtr0->PlayerAryIndex;


	int TAx= unitPtr->XPos; 
	int TAy= unitPtr->YPos- (unitPtr->ZPos)/ 2;
	

	POINT GafAspect= {0, 0};
	LPBYTE GafPixelBits= NULL;
	RECT DescRect;
	int DescHeight_I;
	int DescWidth_I;
	int DesclPitch= Aspect->x;

	UnitPicture( unitPtr, PlayerID, &GafPixelBits, &GafAspect);
	DescRect.left= static_cast<int>(static_cast<float>(TAx)* (static_cast<float>(Width_m)/ static_cast<float>((*TAmainStruct_PtrPtr)->MapSizeX)))- GafAspect.x/ 2;
	DescRect.right= DescRect.left+ GafAspect.x;
	DescRect.top= static_cast<int>(static_cast<float>(TAy)* (static_cast<float>(Height_m)/ static_cast<float>((*TAmainStruct_PtrPtr)->MapSizeY)))- GafAspect.y/ 2;
	DescRect.bottom= DescRect.top+ GafAspect.y;

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
				register int Color= GafPixelBits[SrcPixelYStart+ XPos];
				if (Color!=TransparentColor)
				{
					PixelBits[DescPixelYStart+ (XPos+ DescRect.left)]= Color;
				}
			}
		}
	}
	catch (...)
	{

	}
}

void UnitsMinimap::NowDrawUnits ( LPBYTE PixelBitsBack, POINT * AspectSrc)
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
			for (int i= 0; i<AspectSrc->y; ++i)
			{
				int Line= Aspect.x* i;
				int SrcLine= AspectSrc->x* i;
				for (int j= 0; j<AspectSrc->x; ++j)
				{
					PixelBits[Line+ j]= PixelBitsBack[SrcLine+ j];
				}
			}

			UnitStruct * Begin= (*TAmainStruct_PtrPtr)->OwnUnitBegin;
			UnitStruct * unitPtr;

			int NumHotRadarUnits= (*TAmainStruct_PtrPtr)->NumHotRadarUnits;
			RadarUnit_ * RadarUnits_v= (*TAmainStruct_PtrPtr)->RadarUnits;


			for (int i= 0; i<NumHotRadarUnits; ++i)
			{
				unitPtr= &Begin[RadarUnits_v[i].ID];
				if (0!=unitPtr->UnitID)
				{
					DrawUnit (  PixelBits, &Aspect, unitPtr);
				}
			}
		}
	}
	Unlock ( PixelBits);
}