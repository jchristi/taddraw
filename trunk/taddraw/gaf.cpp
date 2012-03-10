#include "oddraw.h"
#include "iddrawsurface.h"
#include "gaf.h"


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
/*
			unsigned char *  SrcLine= &(GafFrame->PtrFrameBits[i*Width]);
			unsigned char * DescLine= &SurfPTR[i*ddsd.lPitch];
			for (int i_1= 0; i_1<Width; i_1++)
			{
				DescLine[i_1]= (0x9==SrcLine[ i_1])? 1: SrcLine[ i_1];
			}*/
		}
	}

	RetSurf->Unlock(NULL);
	return RetSurf;
}