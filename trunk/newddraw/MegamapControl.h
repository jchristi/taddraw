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


class MegaMapControl 
{
public:
	MegaMapControl (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p,
		int MaxIconWidth, int MaxIconHeight, int MegaMapVirtulKey_arg);
	~ MegaMapControl ();
	void Init (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p,
		int MaxIconWidth, int MaxIconHeight, int MegaMapVirtulKey_arg);
	
	void DrawCursor (LPDIRECTDRAWSURFACE DestSurf,  unsigned int X, unsigned int Y);
	bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	void LockBlit (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);
	void BlitSelect (LPDIRECTDRAWSURFACE DestSurf);
	BOOL IsBliting(void);

	BOOL IsInControl(void);

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
	BOOL RightDoubleClick (int x, int y, bool shift);
	BOOL LeftDoubleClick (int x, int y, bool shift);
	BOOL MouseMove (int x, int y);

	void MoveScreen (unsigned int TAX, unsigned int TAY, unsigned int TAZ);

	BOOL SelectDown (int x, int y, bool out);
	BOOL SelectUp (int x, int y, bool out);
	BOOL SelectMove (int x, int y, bool Out_b);
	Position_Dword * ScreenPos2TAPos (Position_Dword * TAPos, int x, int y);
	POINT * TAPos2ScreenPos (POINT * ScreenPos, unsigned int TAX, unsigned int TAY, unsigned int TAZ);
private:
	TAdynmemStruct * TAmainStruct_Ptr;

	InlineSingleHook*  FindMouseUnitHook;

	int SelectedCount;
	BOOL InControl;
	selectbuttom::SELECTBUTTOM SelectState;
	RECT SelectScreenRect;


	int OrderType;

	int HalfMaxIconWidth_TAPos;
	int HalfMaxIconHeight_TAPos;

	FullScreenMinimap * parent;
	int MegaMapVirtulKey;


	RECT MegaMapScreen;
	int MegaMapWidth;
	int MegaMapHeight;
	RECT TAMap;
	int TAMapWidth;
	int TAMapHeight;

	float Screen2MapWidthScale;
	float Screen2MapHeightScale;

	LPDIRECTDRAWSURFACE Cursor_SurfcAry[0x15];
	POINT Cursor_AspectAry[0x15];
};
