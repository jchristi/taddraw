#pragma once

struct tagTNTHeaderStruct;
class TNTtoMiniMap;
class TAGameAreaReDrawer;
class InlineSingleHook;

class UnitsMinimap;
class MappedMap;


class FullScreenMinimap
{
public:
	TAGameAreaReDrawer * GameDrawer;
	UnitsMinimap * UnitsMap;
	TNTtoMiniMap * MyMinimap_p;
	MappedMap* Mapped_p;

public:
	FullScreenMinimap (BOOL Doit);
	~FullScreenMinimap (void);

	void InitMinimap (tagTNTHeaderStruct * TNTPtr, RECT *  GameScreen= NULL);

	void Blit(LPDIRECTDRAWSURFACE DestSurf);
	void InitSurface (LPDIRECTDRAW TADD);
	void ReleaseSurface (void);

	bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
private:
	InlineSingleHook * LoadMap_hook;
	
	
	BOOL Blit_b;
	BOOL Do_b;
};
