#pragma once
class FullScreenMinimap;
class InlineSingleHook;

namespace selectbuttom
{
	enum SELECTBUTTOM
	{
		down= 0,
		select,
		up,
		none
	};
};

struct _GAFFrame;

class MegaMapControl 
{
public:
	MegaMapControl (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p, RECT * GameScreen_p,
		int MaxIconWidth, int MaxIconHeight, int MegaMapVirtulKey_arg, BOOL WheelMoveMegaMap_v, BOOL DoubleClickMoveMegamap_v, BOOL WheelZoom_v);
	~ MegaMapControl ();
	void Init (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p, RECT * GameScreen_p,
		int MaxIconWidth, int MaxIconHeight, int MegaMapVirtulKey_arg, BOOL WheelMoveMegaMap_v, BOOL DoubleClickMoveMegamap_v, BOOL WheelZoom_v);
	
	//void DrawCursor (LPDIRECTDRAWSURFACE DestSurf,  unsigned int X, unsigned int Y);
	void DrawCursor(LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch
		, unsigned int X, unsigned int Y);
	bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	void BlitSelect (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);
	void BlitOrder (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);

	void LockBlit (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);


	BOOL IsBliting(void);
	BOOL IsDrawGameRect (BOOL Build_b=FALSE);
	BOOL IsDrawOrder (void);

	BOOL IsInControl(void);
	BOOL IsInMap(void);

	void InitSurface ( LPDIRECTDRAW TADD);
	void ReleaseSurface (void) ;

	void EnterMegaMap ();

	void QuitMegaMap ( );

	void Set (int VirtualKey);
private:
	BOOL RightDown (int x, int y, bool shift);
	BOOL RightUp (int x, int y, bool shift);
	BOOL LeftDown (int x, int y, bool shift);
	BOOL LeftUp (int x, int y, bool shift);
	//BOOL RightDoubleClick (int x, int y, bool shift);
	BOOL DoubleClick (int x, int y, bool shift);
	BOOL MouseMove (int x, int y);

	BOOL WheelBack (int xPos, int yPos);
	BOOL WheelFont (int xPos, int yPos);

	void MoveScreen ( int TAX,  int TAY,  int TAZ);
	void MegamapMoveSceen (int xPos, int yPos);
	BOOL SelectDown (int x, int y, bool out);
	BOOL SelectUp (int x, int y, bool out, bool shift);
	BOOL SelectMove (int x, int y, bool Out_b, bool LBMD);
	Position_Dword * ScreenPos2TAPos (Position_Dword * TAPos, int x, int y, BOOL UseTAHeight= FALSE);
	POINT * TAPos2ScreenPos (POINT * ScreenPos, unsigned int TAX, unsigned int TAY, unsigned int TAZ);


	void MegamapTADrawRect (OFFSCREEN * offscreen, RECT * rect, int RectType);

	void DrawBuildRect (OFFSCREEN * offscren_p, unsigned char  Color, 
		UnitDefStruct * BuildTargat, int TAx, int TAy, int TAz);

	void DrawTargatOrder (OFFSCREEN * OffScreen, UnitOrdersStruct * Order, PlayerStruct * me);
	void DrawOrderPath (OFFSCREEN * OffScreen, UnitOrdersStruct * Order, Position_Dword * UnitPos);
private:
	TAdynmemStruct * TAmainStruct_Ptr;

	InlineSingleHook*  FindMouseUnitHook;
	InlineSingleHook * GetPosition_DwordHook;
	InlineSingleHook * GetGridPosFeatureHook;

	int SelectedCount;
	BOOL InControl;
	BOOL InMap;
	selectbuttom::SELECTBUTTOM SelectState;
	RECT SelectScreenRect;
	DWORD SelectTick;


	int OrderType;

	int LastDblXPos;
	int LastDblYpos;

	int HalfMaxIconWidth_TAPos;
	int HalfMaxIconHeight_TAPos;

	FullScreenMinimap * parent;
	int MegaMapVirtulKey;


	RECT TAGameScreen;

	RECT MegaMapScreen;
	int MegaMapWidth;
	int MegaMapHeight;

	RECT TAMap;
	int TAMapWidth;
	int TAMapHeight;

	float Screen2MapWidthScale;
	float Screen2MapHeightScale;

	_GAFFrame * LastCursor_GAFp;
	LPDIRECTDRAWSURFACE Cursor_Surfc;


	BOOL WheelMoveMegaMap;
	BOOL DoubleClickMoveMegamap;
	BOOL WheelZoom;

	
};
#define ORDERPATHSPACING (16)