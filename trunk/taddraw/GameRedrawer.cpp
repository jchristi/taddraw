//用来从TNT地图上创建一个随意大小的小地图出来。
#include "oddraw.h"
//#include "ddraw.h"
#include "tamem.h"
#include "iddrawsurface.h"
class TAGameAreaReDrawer
{
private:
	LPDIRECTDRAWSURFACE OwnGameAreaSurface_ptr;
	
public:
	TAGameAreaReDrawer ();
	~TAGameAreaReDrawer ();

	
	void BlitTAGameArea (LPDIRECTDRAWSURFACE DestSurf);

	BOOL MixInBlit_DSuf (LPRECT DescRect, LPDIRECTDRAWSURFACE Src_DDrawSurface, LPRECT SrcScope);
	BOOL MixInBlit_Bits (LPRECT DescRect, LPBYTE SrcBits, LPPOINT SrcAspect, LPRECT SrcScope);

	LPRECT TAWGameAreaRect (LPRECT Out_Rect);
};

TAGameAreaReDrawer::TAGameAreaReDrawer()
{

}

TAGameAreaReDrawer::~TAGameAreaReDrawer()
{

}

BOOL TAGameAreaReDrawer::MixInBlit_Bits (LPRECT DescRect, LPBYTE SrcBits, LPPOINT SrcAspect, LPRECT SrcScope)
{
	BOOL Rtn_B= FALSE;
	DDSURFACEDESC ddsd;
	DDRAW_INIT_STRUCT ( ddsd);
	
	DWORD DescYStart;
	DWORD DescXStart;
	DWORD CopyHeight;
	DWORD CopyWidth;
	DWORD DescPitch;
	DWORD DescHeight;
	LPBYTE DescMem;
	if (DD_OK==OwnGameAreaSurface_ptr->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR| DDLOCK_WAIT, NULL))
	{
		DescXStart= SrcScope->left;
		DescYStart= SrcScope->top;
		CopyHeight= SrcScope->bottom- SrcScope->top;
		CopyWidth= SrcScope->right- SrcScope->left;

		DescMem= reinterpret_cast<LPBYTE>(ddsd.lpSurface);
		if (DescXStart<0)
		{
			DescXStart= 0;
		}
		if (DescYStart<0)
		{
			DescYStart= 0;
		}

		DescPitch= ddsd.lPitch;
		DescHeight= ddsd.dwHeight;

		if (CopyHeight>DescHeight)
		{
			CopyHeight= DescHeight;
		}
		if (CopyWidth>DescHeight)
		{
			CopyHeight= DescHeight;
		}

		DWORD TailCopyWidth= CopyWidth% 4;
		DWORD i;
		DWORD i_1;
		CopyWidth= CopyWidth- TailCopyWidth;
		for ( i= DescYStart; i< CopyHeight; ++i)
		{
			int DescYOffset= i* DescPitch;
			int SrcYOffset= i* SrcAspect->x;
			for ( i_1= DescXStart; i_1< CopyWidth; i_1= i_1+ 4)
			{
				*reinterpret_cast<LPDWORD>( &(DescMem[DescYOffset+ i_1]))= *reinterpret_cast<LPDWORD>( &(SrcBits[SrcYOffset+ i_1]));
			}

			for (DWORD temp_counter= 0; temp_counter<TailCopyWidth; ++temp_counter)
			{
				DescMem[DescYOffset+ i_1+ temp_counter]= SrcBits[SrcYOffset+ i_1+ temp_counter];
			}
			
		}
		
		OwnGameAreaSurface_ptr->Unlock ( ddsd.lpSurface);
		Rtn_B= TRUE;
	}

	return Rtn_B;
}

BOOL TAGameAreaReDrawer::MixInBlit_DSuf (LPRECT DescRect, LPDIRECTDRAWSURFACE Src_DDrawSurface, LPRECT SrcScope)
{

	BOOL Rtn_B= TRUE;

	DDBLTFX ddbltfx;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 1;
	ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 1;
	if(OwnGameAreaSurface_ptr->Blt ( DescRect, Src_DDrawSurface, SrcScope, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE , &ddbltfx)!=DD_OK)
	{
		if (OwnGameAreaSurface_ptr->Blt ( DescRect, Src_DDrawSurface, SrcScope, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE , &ddbltfx)!=DD_OK)
		{
			Rtn_B= FALSE;
		}
	}

	//Srcddsd.
	return Rtn_B;
}