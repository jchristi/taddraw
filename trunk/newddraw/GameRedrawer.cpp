//用来从TNT地图上创建一个随意大小的小地图出来。
#include "oddraw.h"
//#include "ddraw.h"
#include "tamem.h"
#include "tafunctions.h"
#include "iddrawsurface.h"
#include "gameredrawer.h"

TAGameAreaReDrawer::TAGameAreaReDrawer()
{
	OwnGameAreaSurface_ptr= NULL;
}

LPDIRECTDRAWSURFACE TAGameAreaReDrawer::InitOwnSurface (LPDIRECTDRAW TADD)
{
	if (NULL!=OwnGameAreaSurface_ptr)
	{
		OwnGameAreaSurface_ptr->Release ( );
	}
	RECT GameAreaRect;

	if (TADD)
	{
		TAWGameAreaRect ( &GameAreaRect);

		DDSURFACEDESC ddsd;
		DDRAW_INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth = GameAreaRect.right- GameAreaRect.left;
		ddsd.dwHeight = GameAreaRect.bottom- GameAreaRect.top;

		TADD->CreateSurface( &ddsd, &OwnGameAreaSurface_ptr, NULL);
	}
	
	return OwnGameAreaSurface_ptr;
}
TAGameAreaReDrawer::~TAGameAreaReDrawer()
{
	if (NULL!=OwnGameAreaSurface_ptr)
	{
		OwnGameAreaSurface_ptr->Release ( );
	}
}

void TAGameAreaReDrawer::BlitTAGameArea(LPDIRECTDRAWSURFACE DestSurf)
{
	if (NULL!=OwnGameAreaSurface_ptr)
	{
		if ( DD_OK!=OwnGameAreaSurface_ptr->IsLost ( ))
		{
			OwnGameAreaSurface_ptr->Restore ( );
		}
		DDBLTFX ddbltfx;
		DDRAW_INIT_STRUCT(ddbltfx);

		ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue= 0xffff;
		ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue= 0xffff;
		RECT GameScreen;
		TAWGameAreaRect ( &GameScreen);

		if(DestSurf->Blt ( &GameScreen, OwnGameAreaSurface_ptr, NULL, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE , &ddbltfx)!=DD_OK)
		{
			DestSurf->Blt ( &GameScreen, OwnGameAreaSurface_ptr, NULL, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE , &ddbltfx);
		}
	}
}

HRESULT TAGameAreaReDrawer::Lock (  LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
	if (OwnGameAreaSurface_ptr)
	{
		return OwnGameAreaSurface_ptr->Lock ( lpDestRect, lpDDSurfaceDesc, dwFlags, hEvent);
	}

	return DDERR_SURFACELOST;
}

HRESULT TAGameAreaReDrawer::Unlock(  LPVOID lpSurfaceData)
{
	if (OwnGameAreaSurface_ptr)
	{
		return OwnGameAreaSurface_ptr->Unlock ( lpSurfaceData);
	}

	return DDERR_SURFACELOST;
}


BOOL TAGameAreaReDrawer::MixBitsInBlit (LPRECT DescRect, LPBYTE SrcBits, LPPOINT SrcAspect, LPRECT SrcScope)
{
	BOOL Rtn_B= FALSE;
	if (NULL!=OwnGameAreaSurface_ptr)
	{
		if ( DD_OK!=OwnGameAreaSurface_ptr->IsLost ( ))
		{
			OwnGameAreaSurface_ptr->Restore ( );
		}
		DDSURFACEDESC ddsd;
		DDRAW_INIT_STRUCT ( ddsd);


		if (DD_OK==OwnGameAreaSurface_ptr->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR| DDLOCK_WAIT, NULL))
		{
			DWORD DescYStart;
			DWORD DescXStart;
			DWORD SrcXStart;
			DWORD SrcYStart;
			DWORD CopyHeight;
			DWORD CopyWidth;
			DWORD DescPitch;
			DWORD DescHeight;
			DWORD DescWidth;
			LPBYTE DescMem;

			DescMem= reinterpret_cast<LPBYTE>(ddsd.lpSurface);
			DescPitch= ddsd.lPitch;

			if (NULL==DescRect)
			{
				DescXStart= 0;
				DescYStart= 0;

				DescWidth= ddsd.dwWidth;
				DescHeight= ddsd.dwHeight;
			}
			else
			{
				DescXStart= DescRect->left;
				DescYStart= DescRect->top;
				DescWidth= DescRect->right;
				DescHeight= DescRect->bottom;
			}

			if (NULL==SrcScope)
			{
				SrcXStart= 0;
				SrcYStart= 0;
				CopyHeight= SrcAspect->y;
				CopyWidth= SrcAspect->x;
			}
			else
			{
				SrcXStart= SrcScope->left;
				SrcYStart= SrcScope->right;
				CopyHeight= SrcScope->bottom;
				CopyWidth= SrcScope->right;
			}



			if ((ddsd.dwWidth<DescXStart)
				||(DescHeight<0)
				||(ddsd.dwHeight<DescYStart)
				||(DescWidth<0)
				||(CopyHeight<0)
				||(CopyWidth<0))
			{
				return Rtn_B;
			}

			if (DescXStart<0)
			{
				DescXStart= 0;
			}
			if (DescYStart<0)
			{
				DescYStart= 0;
			}
			if (ddsd.dwWidth<DescWidth)
			{
				DescWidth= ddsd.dwWidth;
			}
			if (ddsd.dwHeight<DescHeight)
			{
				DescHeight= ddsd.dwHeight;
			}
			//
			if (SrcXStart<0)
			{
				SrcXStart= 0;
			}
			if (SrcYStart<0)
			{
				SrcYStart= 0;
			}

			if (CopyHeight>static_cast<DWORD>(SrcAspect->y))
			{
				CopyHeight= static_cast<DWORD>(SrcAspect->y);
			}
			if (CopyWidth>static_cast<DWORD>(SrcAspect->x))
			{
				CopyWidth= static_cast<DWORD>(SrcAspect->x);
			}

			if ((CopyHeight)>DescHeight)
			{
				CopyHeight= DescHeight;
			}
			if ((CopyWidth)>DescWidth)
			{
				CopyWidth= DescWidth;
			}
			
			DWORD TailCopyWidth= CopyWidth% 4;
			DWORD i, i_1, i_2, i_3;
			CopyWidth= CopyWidth- TailCopyWidth;

			for ( i= DescYStart, i_1= SrcYStart; i< CopyHeight; ++i, ++i_1)
			{
				int DescYOffset= i* DescPitch;
				int SrcYOffset= i_1* SrcAspect->x;
				for ( i_2= DescXStart, i_3= SrcXStart; i_2< CopyWidth; i_2= i_2+ 4, i_3= i_3+ 4)
				{
					*reinterpret_cast<LPDWORD>( &(DescMem[DescYOffset+ i_2]))= *reinterpret_cast<LPDWORD>( &(SrcBits[SrcYOffset+ i_3]));
				}

				for (DWORD temp_counter= 0; temp_counter<TailCopyWidth; ++temp_counter)
				{
					DescMem[DescYOffset+ i_2+ temp_counter]= SrcBits[SrcYOffset+ i_3+ temp_counter];
				}
			}
		
			OwnGameAreaSurface_ptr->Unlock ( ddsd.lpSurface);
			Rtn_B= TRUE;
		}
	}
	return Rtn_B;
}

BOOL TAGameAreaReDrawer::MixDSufInBlit (LPRECT DescRect, LPDIRECTDRAWSURFACE Src_DDrawSurface, LPRECT SrcScope, DWORD Background_Color)
{
	BOOL Rtn_B= TRUE;

	if (NULL!=OwnGameAreaSurface_ptr)
	{
		if ( DD_OK!=OwnGameAreaSurface_ptr->IsLost ( ))
		{
			OwnGameAreaSurface_ptr->Restore ( );
		}

		DDBLTFX ddbltfx;
		DDRAW_INIT_STRUCT(ddbltfx);

		ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue= Background_Color& 0xffff;
		ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue= Background_Color>> 16;
		if(OwnGameAreaSurface_ptr->Blt ( DescRect, Src_DDrawSurface, SrcScope, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
		{
			if (OwnGameAreaSurface_ptr->Blt ( DescRect, Src_DDrawSurface, SrcScope, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
			{
				Rtn_B= FALSE;
			}
		}
	}
	return Rtn_B;
}

BOOL TAGameAreaReDrawer::GrayBlitOfBits (LPRECT DescRect, LPBYTE SrcBits, LPPOINT SrcAspect, LPRECT SrcScope, BOOL NoMapped)
{
	BOOL Rtn_B= FALSE;
	LPBYTE TAGrayTABLE_p= (*TAmainStruct_PtrPtr)->TAProgramStruct_Ptr->GRAY_TABLE;
	if (NULL!=OwnGameAreaSurface_ptr)
	{
		if ( DD_OK!=OwnGameAreaSurface_ptr->IsLost ( ))
		{
			OwnGameAreaSurface_ptr->Restore ( );
		}
		DDSURFACEDESC ddsd;
		DDRAW_INIT_STRUCT ( ddsd);


		if (DD_OK==OwnGameAreaSurface_ptr->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR| DDLOCK_WAIT, NULL))
		{
			DWORD DescYStart;
			DWORD DescXStart;
			DWORD SrcXStart;
			DWORD SrcYStart;
			DWORD CopyHeight;
			DWORD CopyWidth;
			DWORD DescPitch;
			DWORD DescHeight;
			DWORD DescWidth;
			LPBYTE DescMem;

			DescMem= reinterpret_cast<LPBYTE>(ddsd.lpSurface);
			DescPitch= ddsd.lPitch;

			if (NULL==DescRect)
			{
				DescXStart= 0;
				DescYStart= 0;

				DescWidth= ddsd.dwWidth;
				DescHeight= ddsd.dwHeight;
			}
			else
			{
				DescXStart= DescRect->left;
				DescYStart= DescRect->top;
				DescWidth= DescRect->right;
				DescHeight= DescRect->bottom;
			}

			if (NULL==SrcScope)
			{
				SrcXStart= 0;
				SrcYStart= 0;
				CopyHeight= SrcAspect->y;
				CopyWidth= SrcAspect->x;
			}
			else
			{
				SrcXStart= SrcScope->left;
				SrcYStart= SrcScope->right;
				CopyHeight= SrcScope->bottom;
				CopyWidth= SrcScope->right;
			}

			if ((ddsd.dwWidth<DescXStart)
				||(DescHeight<0)
				||(ddsd.dwHeight<DescYStart)
				||(DescWidth<0)
				||(CopyHeight<0)
				||(CopyWidth<0))
			{
				return Rtn_B;
			}

			if (DescXStart<0)
			{
				DescXStart= 0;
			}
			if (DescYStart<0)
			{
				DescYStart= 0;
			}
			if (ddsd.dwWidth<DescWidth)
			{
				DescWidth= ddsd.dwWidth;
			}
			if (ddsd.dwHeight<DescHeight)
			{
				DescHeight= ddsd.dwHeight;
			}
			//
			if (SrcXStart<0)
			{
				SrcXStart= 0;
			}
			if (SrcYStart<0)
			{
				SrcYStart= 0;
			}

			if (CopyHeight>static_cast<DWORD>(SrcAspect->y))
			{
				CopyHeight= static_cast<DWORD>(SrcAspect->y);
			}
			if (CopyWidth>static_cast<DWORD>(SrcAspect->x))
			{
				CopyWidth= static_cast<DWORD>(SrcAspect->x);
			}

			if ((CopyHeight)>DescHeight)
			{
				CopyHeight= DescHeight;
			}
			if ((CopyWidth)>DescWidth)
			{
				CopyWidth= DescWidth;
			}

			DWORD i, i_1, i_2, i_3;
			DWORD DescXPos;


			if (NoMapped)
			{
				for ( i= DescYStart, i_1= SrcYStart; i< CopyHeight; ++i, ++i_1)
				{
					int DescYOffset= i* DescPitch;
					int SrcYOffset= i_1* SrcAspect->x;
					for ( i_2= DescXStart, i_3= SrcXStart; i_2< CopyWidth; i_2++, i_3++)
					{
						if (0==SrcBits[SrcYOffset+ i_3])
						{ // black
			
							DescMem[DescYOffset+ i_2]= 0;
						}
					}
				}
			}
			else
			{
				for ( i= DescYStart, i_1= SrcYStart; i< CopyHeight; ++i, ++i_1)
				{
					int DescYOffset= i* DescPitch;
					int SrcYOffset= i_1* SrcAspect->x;
					for ( i_2= DescXStart, i_3= SrcXStart; i_2< CopyWidth; i_2++, i_3++)
					{
						if (0==SrcBits[SrcYOffset+ i_3])
						{ // too slowly 
							DescXPos= DescYOffset+ i_2;
							DescMem[DescXPos]= TAGrayTABLE_p[DescMem[DescXPos]];
						}
					}
				}
			}
			OwnGameAreaSurface_ptr->Unlock ( ddsd.lpSurface);
			Rtn_B= TRUE;
		}
	}
	return Rtn_B;
}
void TAGameAreaReDrawer::ReleaseSurface (void)
{
	if (NULL!=OwnGameAreaSurface_ptr)
	{
		OwnGameAreaSurface_ptr->Release ( );
	}
	OwnGameAreaSurface_ptr= NULL;

}

LPRECT TAGameAreaReDrawer::TAWGameAreaRect (LPRECT Out_Rect)
{
	static RECT TAGameAera;
	if (NULL==Out_Rect)
	{
		Out_Rect= &TAGameAera;
	}

	memcpy ( Out_Rect, &(*TAmainStruct_PtrPtr)->GameSreen_Rect, sizeof(RECT));

	return Out_Rect;
}