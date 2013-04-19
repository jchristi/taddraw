#pragma once

#define USEMEGAMAP

#define MEGAMAPFPS (40)

struct tagTNTHeaderStruct;
class TNTtoMiniMap;
class TAGameAreaReDrawer;
class InlineSingleHook;

class UnitsMinimap;
class MappedMap;
class ProjectileMap;
class MegaMapControl;

struct tagInlineX86StackBuffer;
typedef struct tagInlineX86StackBuffer * PInlineX86StackBuffer;

class FullScreenMinimap
{
public:
	TAGameAreaReDrawer * GameDrawer;
	UnitsMinimap * UnitsMap;
	TNTtoMiniMap * MyMinimap_p;
	MappedMap* Mapped_p;
	ProjectileMap* ProjectilesMap_p;
	MegaMapControl * Controler;

	int MegamapWidth;
	int MegamapHeight;
	RECT MegaMapInscren;
	RECT MegamapRect;

	RECT TAMAPTAPos;
public:
	FullScreenMinimap (BOOL Doit);
	~FullScreenMinimap (void);

	void InitMinimap (tagTNTHeaderStruct * TNTPtr, RECT *  GameScreen= NULL);

	void Blit(LPDIRECTDRAWSURFACE DestSurf);
	void LockBlit (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);
	void InitSurface (LPDIRECTDRAW TADD);
	void ReleaseSurface (void);

	bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	BOOL IsBliting ( );
	void EnterMegaMap ();
	void QuitMegaMap ( );

	void Set (int VirtualKey);
private:
	InlineSingleHook * LoadMap_hook;
	BOOL Blit_b;
	BOOL Flipping;
	
	BOOL Do_b;



	int MegamapVirtualKey;



	BOOL DrawBackground;
	BOOL DrawMapped;
	BOOL DrawProjectile;
	BOOL DrawUnits;
	BOOL DrawMegamapRect;
	BOOL DrawMegamapBlit;
	BOOL DrawSelectAndOrder;
	BOOL DrawMegamapCursor;

	BOOL WheelZoom;
	BOOL WheelMoveMegaMap;
	BOOL DoubleClickMoveMegamap;

	InlineSingleHook * DrawTAScreen_hok;
};



int __stdcall BlockTADraw (PInlineX86StackBuffer X86StrackBuffer);