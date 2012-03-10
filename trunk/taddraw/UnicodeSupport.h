#ifndef UNICODESUPPORT_HAYU32Y

#define UNICODESUPPORT_HAYU32Y
#ifndef __DDRAW_INCLUDED__
	struct IDirectDrawSurface;
	typedef struct IDirectDrawSurface FAR *LPDIRECTDRAWSURFACE;
	struct IDirectDraw;
	typedef struct IDirectDraw FAR *LPDIRECTDRAW;

#endif

	class InlineSingleHook;
	struct Context_Screen;


#ifndef _WINDEF_
	struct tagRECT;
	typedef struct tagRECT *LPRECT;
	typedef unsigned int UINT; 
#endif

	typedef struct tagSpecScreenSurface
	{
		DWORD Width;
		DWORD Height;
		DWORD PixelPerLine;
		LPVOID SurfaceMem_Pvoid;
		int SurfaceMemBytesLen;
		LPDIRECTDRAWSURFACE DDSurface_P;
		HDC mySurface_HDC;
		DDSURFACEDESC DDSurfaceDescForLock;
		LPDIRECTDRAW myDDraw_Ptr;
		struct tagSpecScreenSurface * Next;
	} SpecScreenSurface, * PSpecScreenSurface;

	class UnicodeSupport 
	{
	public:
		bool IsStunkTA;

// 
// 		int TAProcessWhenExchange;//maybe TA use new surface when we backup old surface's clipper
// 		LPDIRECTDRAWCLIPPER IME_Clipper;
// 
// 		LPDIRECTDRAWCLIPPER FrontOrg_Clipper;
// 		LPDIRECTDRAWCLIPPER BackOrg_Clipper;

		BOOL UnicodeValid;
	//*公共函数 *//
	public:	
		UnicodeSupport ();
		~UnicodeSupport();
		LPDIRECTDRAWSURFACE CreateSurfaceFromContextScreen (Context_Screen * Context_Screen_Ptr, LPDIRECTDRAW lpDDraw, LPDIRECTDRAWSURFACE * lplpDDSurface);

		BOOL UnicodeDrawTextA (Context_Screen * Context_Screen_Ptr, char * Str_cstrp, LPDIRECTDRAW TADDrawArgc_lp, int X_Off, int Y_Off);
	    bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	//*内部变量 *//
	private:
		HFONT UnicodeWhiteFont;
		HFONT UnicodeYellowFont;
		LPDIRECTDRAW TADDraw_lp;
		
		InlineSingleHook * DrawTextInScreen_ISH;
		InlineSingleHook * ValidChar_ISH;
		InlineSingleHook * DeleteChar_ISH;
		InlineSingleHook * DBSC2rdByte_ISH;
		PSpecScreenSurface UnicodeFontDrawCache;
	//*内部函数 *//
	private:
		PSpecScreenSurface NewSpecScreenSurface (Context_Screen * Context_Screen_Ptr);
		void FreeSpecScreenSurface (PSpecScreenSurface ForFree_PSSS);
		PSpecScreenSurface GetSpecScreenSurface (Context_Screen * Context_Screen_Ptr);
	};
	int USDrawTextInScreen_HookRouter (PInlineX86StackBuffer X86StrackBuffer);

	int myDeleteChar (PInlineX86StackBuffer X86StrackBuffer);
	int myValidChar  (PInlineX86StackBuffer X86StrackBuffer);
	int DBSC2rdByte (PInlineX86StackBuffer X86StrackBuffer);
#endif //UNICODESUPPORT_HAYU32Y