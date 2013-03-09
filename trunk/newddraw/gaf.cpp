#include "oddraw.h"
#include "iddrawsurface.h"
#include "gaf.h"
#include "tamem.h"
#include "tafunctions.h"

LPDIRECTDRAWSURFACE CreateSurfByGafFrame (PGAFFrame Cursor_P, bool VidMem)
{
	//PCXPic.buffer= (unsigned char *)malloc (	SizeofResource ( HInstance, FResource));
	if (NULL==Cursor_P)
	{
		return NULL;
	}
	PGAFFrame GafFrame= Cursor_P;

	if (1==GafFrame->Compressed)
	{//no compressed
		return NULL;
	}
	int Height = GafFrame->Height;
	int Width = GafFrame->Width;

	LPDIRECTDRAWSURFACE RetSurf;

	DDSURFACEDESC ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	if(VidMem)
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = Width;
	ddsd.dwHeight = Height;

	((LPDIRECTDRAW)LocalShare->TADirectDraw)->CreateSurface(&ddsd, &RetSurf, NULL);

	DDRAW_INIT_STRUCT(ddsd);

	RetSurf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	unsigned char *SurfPTR = (unsigned char*)ddsd.lpSurface;
	if (NULL!=SurfPTR)
	{
		for(int i=0; i<Height; i++)
		{
			memcpy(&SurfPTR[i*ddsd.lPitch], &reinterpret_cast<LPBYTE>(GafFrame->PtrFrameBits)[i*Width], Width);
		}
	}

	RetSurf->Unlock(NULL);
	return RetSurf;
}

LPBYTE InstanceGAFFrame (PGAFFrame GafFrame, LPBYTE * FrameBits, POINT * Aspect)// need free
{
	if (NULL==GafFrame)
	{
		return NULL;
	}
	OFFSCREEN OffScreen={0};

	OffScreen.Height= GafFrame->Height;
	OffScreen.Width= GafFrame->Width;
	OffScreen.lPitch= GafFrame->Width;
	OffScreen.lpSurface= malloc(OffScreen.Height* OffScreen.lPitch);

	OffScreen.ScreenRect.left= 0;
	OffScreen.ScreenRect.right= OffScreen.Width;
	OffScreen.ScreenRect.top= 0;
	OffScreen.ScreenRect.bottom= OffScreen.Height;

	CopyGafToContext ( &OffScreen, GafFrame, 0, 0);

	if (Aspect)
	{
		Aspect->y = GafFrame->Height;
		Aspect->x = GafFrame->Width;
	}
	if (FrameBits)
	{
		*FrameBits= reinterpret_cast<LPBYTE>(OffScreen.lpSurface);
	}
	
	
	return reinterpret_cast<LPBYTE>(OffScreen.lpSurface);
}