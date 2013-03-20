#pragma once

class FullScreenMinimap;
class PCX;
struct UnitStruct;
//DWORD WINAPI UnitMapDrawerProc( LPVOID argc);
class TABMP;
class PCX;
#define USEPCX (1)
#define USEBMP (0)
#define USEBAD (-1)
class UnitIcon
{
public:
	UnitIcon (char * FileName);
	~UnitIcon ();
	LONG Width ();
	LONG Height ();
	LPBYTE Data ();
private:
	TABMP * bmp;
	PCX * pcx;
	int Use;
};

class UnitsMinimap 
{
public:
	FullScreenMinimap * parent;
	
public:
	UnitsMinimap ( FullScreenMinimap * inheritFrom, int Width, int Height);
	UnitsMinimap ( FullScreenMinimap * inheritFrom);

	~UnitsMinimap();
	void ReSet (int Width, int Height);

	LPDIRECTDRAWSURFACE InitSurface (LPDIRECTDRAW TADD);
	void ReleaseSurface (void);

// 	void LockEvent (void);
// 	void UnLockEvent (void);
 	LPBYTE LockOn (LPBYTE * PixelBits_pp, POINT * Aspect);
 	void Unlock ( LPBYTE PixelBits_p);
	LPDIRECTDRAWSURFACE GetSurface (void);
	void NowDrawUnits ( LPBYTE PixelBitsBack, POINT * AspectSrc);

	void LoadUnitPicture ( void);
	void FreeUnitPicture ( void);
private:
	void DrawUnit ( LPBYTE PixelBits, POINT * Aspect, UnitStruct * unitPtr);

	LPBYTE UnitPicture (UnitStruct * unitPtr, int PlayerID, LPBYTE * PixelBits_pp,  POINT * Aspect);
	void InitPicturePlayerColors (int PlayeyColor, int FillColor, LPBYTE * * PPCptr);
private:
	LPDIRECTDRAWSURFACE UnitsMapSfc;

//	HANDLE Syncetux;

	HANDLE Thd_h;
	LPDWORD * MaskAry;
	UnitIcon * * PicturesBits;
	LPBYTE * PicturesPlayerColors[PLAYERNUM];

	int MaskNum;
	int UnknowMaskIndex;
	int FillColor;
	int TransportColor;
	BOOL UseDefaultIcon;
// 	InlineSingleHook * Clean_hook;
// 	InlineSingleHook * DrawUnit_hook;
	int Width_m;
	int Height_m;
	BYTE PlayerColros[PLAYERNUM];
	LPBYTE  PlayerBits[PLAYERNUM];
	POINT PlayerAspect[PLAYERNUM];
};

#define DEFAULTFILLCOLOR (0)
#define DEFAULTTRANSPORTCOLOR (9)