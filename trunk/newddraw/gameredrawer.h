#pragma once

class TAGameAreaReDrawer
{
private:
	LPDIRECTDRAWSURFACE OwnGameAreaSurface_ptr;

public:
	TAGameAreaReDrawer ();
	~TAGameAreaReDrawer ();

	void BlitTAGameArea (LPDIRECTDRAWSURFACE DestSurf);

	BOOL MixDSufInBlit (LPRECT DescRect, LPDIRECTDRAWSURFACE Src_DDrawSurface, LPRECT SrcScope, DWORD Background_Color);
	BOOL MixBitsInBlit (LPRECT DescRect, LPBYTE SrcBits, LPPOINT SrcAspect, LPRECT SrcScope);

	LPRECT TAWGameAreaRect (LPRECT Out_Rect);

	LPDIRECTDRAWSURFACE InitOwnSurface (LPDIRECTDRAW TADD);

	void ReleaseSurface (void);

	HRESULT Lock (  LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent);
	HRESULT Unlock(  LPVOID lpSurfaceData);
};