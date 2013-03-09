#pragma once

class FullScreenMinimap;

//DWORD WINAPI UnitMapDrawerProc( LPVOID argc);

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
	void NowDrawUnits ( void);
private:
	void DrawUnit ( LPBYTE PixelBits, POINT * Aspect, int PlayerID, unsigned int TAx, unsigned int TAy);

	LPBYTE UnitPicture ( int PlayerID, LPBYTE * PixelBits_pp,  POINT * Aspect);
private:
	LPDIRECTDRAWSURFACE UnitsMapSfc;

	HANDLE Syncetux;
	HANDLE Thd_h;

// 	InlineSingleHook * Clean_hook;
// 	InlineSingleHook * DrawUnit_hook;
	int Width_m;
	int Height_m;
	BYTE PlayerColros[PLAYERNUM];
	LPBYTE  PlayerBits[PLAYERNUM];
	POINT PlayerAspect[PLAYERNUM];
};
