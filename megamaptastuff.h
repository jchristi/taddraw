#pragma  once

class FullScreenMinimap;
class MegamapTAStuff 
{
public:
	MegamapTAStuff (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p, RECT * GameScreen_p,
		int MaxIconWidth, int MaxIconHeight);
	~MegamapTAStuff();

	void Init (FullScreenMinimap * parent_p, RECT * MegaMapScreen_p, RECT * TAMap_p, RECT * GameScreen_p,
		int MaxIconWidth, int MaxIconHeight);

	void LockBlit_TA (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);
	void LockBlit_MEGA (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);

	void DrawCursor(LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch
		, unsigned int X, unsigned int Y);

	void TADrawRect ( OFFSCREEN * offscreen, int TAx1, int TAy1, int TAz1, 
		int TAx2, int TAy2, int TAz2, 
		int color);

	void DrawBuildRect (OFFSCREEN * offscren_p, unsigned char  Color, 
		UnitDefStruct * BuildTargat, int TAx, int TAy, int TAz);

	void DrawTargatOrder (OFFSCREEN * OffScreen, UnitOrdersStruct * Order, PlayerStruct * me);
	void DrawOrderPath (OFFSCREEN * OffScreen, UnitOrdersStruct * Order, Position_Dword * UnitPos);

	void BlitSelect (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);
	void BlitOrder (LPVOID lpSurfaceMem, int dwWidth, int dwHeight, int lPitch);

private:

	Position_Dword * ScreenPos2TAPos (Position_Dword * TAPos, int x, int y, BOOL UseTAHeight= FALSE);
	POINT * TAPos2ScreenPos (POINT * ScreenPos, unsigned int TAX, unsigned int TAY, unsigned int TAZ);

	FullScreenMinimap * parent;
	TAdynmemStruct * TAmainStruct_Ptr;

	LPBYTE ScreenBuf;

	int HalfMaxIconWidth_TAPos;
	int HalfMaxIconHeight_TAPos;

	RECT TAGameScreen;

	RECT MegaMapScreen;
	int MegaMapWidth;
	int MegaMapHeight;

	RECT TAMap;
	int TAMapWidth;
	int TAMapHeight;

	float Screen2MapWidthScale;
	float Screen2MapHeightScale;

	
};
