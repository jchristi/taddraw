#include "stdafx.h"
#include "mapParse.h"

class MapDrawer 
{
private:
	LONG m_ScreenMapWidth;
	LONG m_ScreenMapHeight;
	RECT m_ScreenMapRect;

	LONG m_BorderRectWidth;
	LONG m_BorderRectHeight;
	RECT m_BorderRect;
	
	RECT m_MapRect;

	RECT m_HotRect;

	int Ignore_Counter;
	BOOL Valid;

	TNTtoMiniMap * TNTPicture_p;
	UnitsDrawer * UnitDrawer_p;
	WEAPONDrawer * WeaponDrawer_p;
	MiniMapPicture * TempMap_p;
	LPDIRECTDRAWSURFACE InnerBorderMap;
	LPDIRECTDRAWSURFACE BorderRect;
	LPDIRECTDRAWSURFACE MapSurface;

private:
	BOOL InitMapDrawer (LPRECT BorderRect_P);
	BOOL CreateMapBorder (LPRECT BorderRect_P);
	BOOL CreateMapSurface (LPRECT BorderRect_P);
	BOOL CreateInnerBorderMap ( LPRECT ScreenMapRect_P, LPRECT BorderRect_P);
	BOOL CreateDrawer (TAdynmemStruct * TAMainStruct_Ptr);
	BOOL CreateTempMap (LPRECT m_ScreenMapRect_P);
	BOOL CreateTNTPicture (LPRECT MapRect_P);
	
public:
	MapDrawer ();
	MapDrawer (DWORD Width, DWORD Height);
	MapDrawer (LPRECT BorderRect_P);
	
	HRESULT BltTA (LPDIRECTDRAWSURFACE lpTADDSrcSurface, LPRECT lpDestRect= NULL);

	void __stdcall LoopGather (void);
};


MapDrawer::MapDrawer ()
{
	//Load TA config

	RECT BorderRect;
	InitMapDrawer ( &BorderRect);
}

MapDrawer::MapDrawer (DWORD Width, DWORD Height)
{

	RECT BorderRect;
	InitMapDrawer ( &BorderRect);
}

MapDrawer::MapDrawer (LPRECT BorderRect_P)
{
	InitMapDrawer ( BorderRect_P);
}

BOOL MapDrawer::InitMapDrawer (LPRECT BorderRect_P)
{
	Valid= FALSE;
	Ignore_Counter= 1;

	
	memcpy ( &m_BorderRect, BorderRect_P, sizeof(RECT));
	m_MapRect.left= 0;
	m_MapRect.top= 0;

	m_BorderRectWidth= m_BorderRect.right- m_BorderRect.left;
	m_BorderRectHeight= m_BorderRect.bottom- m_BorderRect.top;

	float Divisor_F= static_cast<float>(m_BorderRectWidth);
	float Dividend_F= static_cast<float>(m_BorderRectHeight);

	if ((Dividend_F/ Divisor_F)>= (16.0f/ 9.0f))
	{
		m_MapRect.right= 2000;
		m_MapRect.bottom= 1500;
	}
	else
	{
		m_MapRect.right= 3200;
		m_MapRect.bottom= 1800;
	}

	memcpy ( &m_HotRect, m_MapRect, sizeof(RECT));// init HotRect to full map

	do
	{
		if (! CreateMapBorder ( &m_BorderRect))
		{
			break;
		}
		if (! CreateMapSurface ( &m_BorderRect))
		{
			break;
		}
		if (! CreateInnerBorderMap ( &m_ScreenMapRect, &m_BorderRect))
		{
			break;
		}
		if (! CreateDrawer ( * (TAdynmemStruct * *)0x511DE8))
		{
			break;
		}
		if (! CreateTempMap ( &m_MapRect))
		{
			break;
		}
		if (! CreateTNTPicture ( &m_MapRect))
		{
			break;
		}
		Valid= TRUE;
	} while (false);

	return Valid;
}
void __stdcall MapDrawer::LoopGather (void)
{
	++Ignore_Counter;
	if ((Ignore_Counter% (30))!=0)
	{
		return ;
	}
	Ignore_Counter= 1;
	if (!Valid)
	{
		return ;
	}
	//zoom TNT map to TempMap_p
	//draw units and weapons to TempMap_p
	UnitDrawer_p->LoopAllUnits ();
	WeaponDrawer_p->LoopAllWEAPON ();

	//
	RECT BltRect;
	DDBLTFX ddbltfx_tmp;
	DDSURFACEDESC ddsd;
#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }
	DDRAW_INIT_STRUCT (ddbltfx_tmp);
	DDRAW_INIT_STRUCT (ddsd);
	
	ddbltfx_tmp.ddckSrcColorkey.dwColorSpaceLowValue= 0x00;
	BltRect.left= 0;
	BltRect.right= m_BorderRectWidth;
	BltRect.top= 0;
	BltRect.bottom= m_BorderRectHeight;
	MapSurface->Blt ( &m_BorderRect, m_MapBorder, &BltRect, DDBLT_KEYSRCOVERRIDE, &ddbltfx_tmp);

	//zoom tempMap to InnerBorderMap
	InnerBorderMap->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR, NULL);
	TempMap_p->DrawMiniMap ( ddsd.lpSurface, m_ScreenMapRect, ddsd.lPitch, ddsd.dwHeight, &m_HotRect);
	InnerBorderMap.Unlock ( ddsd.lpSurface);

	//copy InnerBorderMap to middle of border, in MapSurface
	BltRect.left= 0;
	BltRect.right= m_ScreenMapWidth;
	BltRect.top= 0;
	BltRect.bottom= m_ScreenMapHeight;

	DDRAW_INIT_STRUCT (ddbltfx_tmp);
	
	MapSurface->Blt ( &m_ScreenMapRect, InnerBorderMap, &BltRect, 0, &ddbltfx_tmp);
	
}