#include "oddraw.h"
#include "iddrawsurface.h"
#include <vector>
using namespace std;
#include "tamem.h"
#include "tafunctions.h"
#include "hook/etc.h"
#include "hook/hook.h"
#include <vector>
#include "UnicodeSupport.h"
#include "LimitCrack.h"

UnicodeSupport::UnicodeSupport(LPCSTR FontName)
{
	DrawTextInScreen_ISH= NULL;
	ValidChar_ISH= NULL;
	DeleteChar_ISH= NULL;
	DBSC2rdByte_ISH= NULL;
	Blt_BottomState0_Text_ISH= NULL;
	PopadState_ISH= NULL;
//	CreateWindowExW_ISH= NULL;

	UnicodeWhiteFont= NULL;
	UnicodeYellowFont=NULL;
	TADDraw_lp= NULL;
	UnicodeValid= FALSE;
	ImeShowing= FALSE;
	LMouseDown= FALSE;

	lpCandList= NULL;
	CurrentCandListLen= 0;
	
	ImeSurfaceBackground= 0x01;
	lpImeSurface= NULL;
	Orginal_hIMC = ImmAssociateContext ( *reinterpret_cast<HWND *>(0x51F320+ 0x0040), NULL); 
	hIMC= NULL;
	memset ( IMEName, 0, 0x100);
	memset ( InputStrBuf, 0, 0x100);
	
	if ((NULL==FontName)
		||(0==FontName[0]))
	{
		return ;
	}

	if (0==*reinterpret_cast<DWORD *>(0x51F320+ 0x84) )
	{// lpDDraw==NULL
		return ;
	}
	UnicodeYellowFont= CreateFontA ( 
		12,                                                 //   nHeight 
		0,                                                   //   nWidth 
		0,                                                   //   nEscapement 
		0,                                                   //   nOrientation 
		FW_MEDIUM,                                   //   nWeight 
		FALSE,                                           //   bItalic 
		FALSE,                                           //   bUnderline 
		FALSE,                                                   //   cStrikeOut 
		GB2312_CHARSET,                             //   nCharSet 
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH,
		FontName);     //   lpszFacename 
	
	if (NULL==UnicodeYellowFont)
	{
		return;
	}

	//install DrawTextInScreen HOOK to TA.
	DrawTextInScreen_ISH= new InlineSingleHook ( 0x4A50E0, 0x5, INLINE_5BYTESLAGGERJMP, USDrawTextInScreen_HookRouter);

	//BYTE FF_Dw[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	ValidChar_ISH= new InlineSingleHook ( 0x004AB79B, 5, INLINE_5BYTESLAGGERJMP, myValidChar);// 0x04AB942
	DeleteChar_ISH= new InlineSingleHook ( 0x004AB7E8, 5, INLINE_5BYTESLAGGERJMP, myDeleteChar);// 
	//	DBSC2rdByte_ISH=  new InlineSingleHook ( 0x004AB79B, 5, INLINE_5BYTESLAGGERJMP, DBSC2rdByte);// 

	Blt_BottomState0_Text_ISH= new InlineSingleHook ( Blt_BottomState0_TextAddr, 5, INLINE_5BYTESLAGGERJMP, Blt_BottomState0_Text);
	Blt_BottomState1_Text_ISH= new InlineSingleHook ( Blt_BottomState1_TextAddr, 5, INLINE_5BYTESLAGGERJMP, Blt_BottomState1_Text);
	PopadState_ISH= new InlineSingleHook ( PopadStateAddr, 5, INLINE_5BYTESLAGGERJMP, PopadState);

/*
	BYTE Tag_[]= {0x5D, 0xC2, 0x30, 0x00};
	InnerCreateWindow= reinterpret_cast<unsigned int>(memfind ( reinterpret_cast<LPVOID>(CreateWindowExW), 0x100, Tag_, 0x4))- 5;
	CreateWindowExW_rtn= InnerCreateWindow+ 6;
	InnerCreateWindow= InnerCreateWindow+ *reinterpret_cast<unsigned int *>(InnerCreateWindow+ 1)+ 5;

	CreateWindowExW_ISH=  new InlineSingleHook ( reinterpret_cast<unsigned int>(CreateWindowExW), 5, INLINE_5BYTESLAGGERJMP, CreateWindowExW_new);
	CreateWindowExW_ISH->SetParamOfHook ( reinterpret_cast<LPVOID>(&VisibleWnd_Vec));*/


	UnicodeValid= TRUE;

	TADDraw_lp= (LPDIRECTDRAW)LocalShare->TADirectDraw;

	UnicodeFontDrawCache= NULL;

	ImmAssociateContext ( *reinterpret_cast<HWND *>(0x51F320+ 0x0040), Orginal_hIMC); //
	hIMC= Orginal_hIMC;

	
	lpCandList= reinterpret_cast<LPCANDIDATELIST>(new BYTE[0x100]);
	CurrentCandListLen= 0x100;
	memset ( lpCandList, 0, CurrentCandListLen);
	
	xPos= *reinterpret_cast<DWORD *>(0x51F320+ 0xd4)- 200;
	yPos= *reinterpret_cast<DWORD *>(0x51F320+ 0xd8) - 300;

	RestoreLocalSurf ( );
}

UnicodeSupport::UnicodeSupport(void)
{
	DrawTextInScreen_ISH= NULL;
	ValidChar_ISH= NULL;
	DeleteChar_ISH= NULL;
	DBSC2rdByte_ISH= NULL;
	PopadState_ISH= NULL;

	UnicodeWhiteFont= NULL;
	UnicodeYellowFont=NULL;
	TADDraw_lp= NULL;
	UnicodeValid= FALSE;
	ImeShowing= FALSE;
	LMouseDown= FALSE;

	lpCandList= NULL;
	CurrentCandListLen= 0;



	memset ( IMEName, 0, 0x100);
	memset ( InputStrBuf, 0, 0x100);

	Orginal_hIMC = ImmAssociateContext(*reinterpret_cast<HWND *>(0x51F320+ 0x0040), NULL); 
	hIMC= NULL;

	Width= Height= 0x200;
	xPos= yPos= 0;
	ImeSurfaceBackground= 0x01;
	lpImeSurface= NULL;
}

UnicodeSupport::~UnicodeSupport(void)
{
		//uninstall DrawTextInScreen HOOK to TA.
		if (NULL!=ValidChar_ISH)
		{
			delete ValidChar_ISH;
		}
		if (NULL!=DeleteChar_ISH)
		{
			delete DeleteChar_ISH;
		}
		if (NULL!=DBSC2rdByte_ISH)
		{
			delete DBSC2rdByte_ISH;
		}
		

		if (NULL!=DrawTextInScreen_ISH)
		{
			delete DrawTextInScreen_ISH;
		}
		
		if (NULL!=Blt_BottomState0_Text_ISH)
		{
			delete Blt_BottomState0_Text_ISH;
		}

		if (NULL!=Blt_BottomState1_Text_ISH)
		{
			delete Blt_BottomState1_Text_ISH;
		}

		if (NULL!=PopadState_ISH)
		{
			delete PopadState_ISH;
		}
/*
		if (NULL!=CreateWindowExW_ISH)
		{
			delete CreateWindowExW_ISH;
		}*/
		
		if (NULL!=UnicodeYellowFont)
		{
			DeleteObject ( UnicodeYellowFont);
		}
		if (NULL!=UnicodeWhiteFont)
		{
			DeleteObject ( UnicodeWhiteFont);
		}
		
		PSpecScreenSurface temp2_PSSS;

		for (PSpecScreenSurface temp_PSSS= UnicodeFontDrawCache; NULL!=temp_PSSS; FreeSpecScreenSurface ( temp2_PSSS))
		{
			temp2_PSSS= temp_PSSS;
			temp_PSSS= temp_PSSS->Next;
		}
		//allow IME
		if (NULL!=Orginal_hIMC)
		{
			ImmAssociateContext(*reinterpret_cast<HWND *>(0x51F320+ 0x0040), Orginal_hIMC); 
			hIMC= Orginal_hIMC;
		}
		//ReleaseAllVisibleWindow ( );
		if (NULL!=lpImeSurface)
		{
			lpImeSurface->Release ( );
		}
		if (NULL!=lpCandList)
		{
			delete	lpCandList;
		}
		ReleaseCandidateList ( );
		
}
////////////////
LPDIRECTDRAWSURFACE UnicodeSupport::CreateSurfaceFromContextScreen (OFFSCREEN * OFFSCREEN_Ptr, LPDIRECTDRAW lpDDraw, LPDIRECTDRAWSURFACE * lplpDDSurface)
{
	*lplpDDSurface= NULL;

	DDSURFACEDESC DDSurfaceDesc_temp;
	DDRAW_INIT_STRUCT( DDSurfaceDesc_temp);
	DDSurfaceDesc_temp.dwFlags|= DDSD_HEIGHT;
	DDSurfaceDesc_temp.dwFlags|= DDSD_WIDTH;
	DDSurfaceDesc_temp.dwFlags|= DDSD_CAPS;
	//	DDSurfaceDesc_temp.dwFlags|= DDSD_PITCH;
	//	DDSurfaceDesc_temp.dwFlags|= DDSD_LPSURFACE;

	DDSurfaceDesc_temp.dwHeight= OFFSCREEN_Ptr->Height;
	DDSurfaceDesc_temp.dwWidth= OFFSCREEN_Ptr->Width;
 	DDSurfaceDesc_temp.ddsCaps.dwCaps= DDSCAPS_OFFSCREENPLAIN| DDSCAPS_SYSTEMMEMORY;

	//	DDSurfaceDesc_temp.lPitch= (OFFSCREEN_Ptr->lPitch);//一个字节代表8个bits
	//	DDSurfaceDesc_temp.lpSurface= OFFSCREEN_Ptr->lpSurface;

	if (DD_OK==lpDDraw->CreateSurface ( &DDSurfaceDesc_temp, lplpDDSurface, NULL))
	{
		return *lplpDDSurface;
	}
	else
	{
		UnicodeValid= FALSE;
		return NULL;
	}
}

/////////////////////
//SpecScreenSurface
/////////////////////
PSpecScreenSurface UnicodeSupport::NewSpecScreenSurface (OFFSCREEN * OFFSCREEN_Ptr)
{
	IDDrawSurface::OutptTxt ( "NewSpecScreenSurface!");
	PSpecScreenSurface temp_PSSS= static_cast <PSpecScreenSurface> (malloc ( sizeof(SpecScreenSurface)+ 1));
	memset ( temp_PSSS, 0, sizeof(SpecScreenSurface));

	//TADDraw_lp= (LPDIRECTDRAW)LocalShare->TADirectDraw;
	////
	if (IsIDDrawLost())
	{
		RestoreLocalSurf ( );/////////ugly
	}

	CreateSurfaceFromContextScreen ( OFFSCREEN_Ptr, TADDraw_lp, &(temp_PSSS->DDSurface_P));
	if (NULL==temp_PSSS->DDSurface_P)
	{
		free ( temp_PSSS);
		return NULL;
	}
	//GetDC ( );
	DDRAW_INIT_STRUCT( temp_PSSS->DDSurfaceDescForLock);

	temp_PSSS->Width= OFFSCREEN_Ptr->Width;
	temp_PSSS->Height=  OFFSCREEN_Ptr->Height;
	temp_PSSS->lPitch=  OFFSCREEN_Ptr->lPitch;
	temp_PSSS->SurfaceMemBytesLen= (temp_PSSS->Height- 1)* (temp_PSSS->Width);
	temp_PSSS->myDDraw_Ptr= TADDraw_lp;

	return temp_PSSS;
}

void UnicodeSupport::FreeSpecScreenSurface (PSpecScreenSurface ForFree_PSSS)
{	

	//
		if (NULL!=ForFree_PSSS)
		{
			if (NULL!=(ForFree_PSSS->DDSurface_P))
			{
				if (TADDraw_lp==ForFree_PSSS->myDDraw_Ptr)
				{//* DDraw的释放，会把这些surface都摧毁 *//
					__try	
					{

						if (NULL!=(ForFree_PSSS->mySurface_HDC))
						{
							(ForFree_PSSS->DDSurface_P)->ReleaseDC ( ForFree_PSSS->mySurface_HDC);
						}
						(ForFree_PSSS->DDSurface_P)->Release ( );
					}
					__except (EXCEPTION_EXECUTE_HANDLER)
					{
						;
					}
				}
			}
			free ( ForFree_PSSS);//* 结构的释放，可能会出错  ???? *//
		}

}

PSpecScreenSurface UnicodeSupport::GetSpecScreenSurface (OFFSCREEN * OFFSCREEN_Ptr)
{
	PSpecScreenSurface temp_PSSS= UnicodeFontDrawCache;
	PSpecScreenSurface Last_PSSS= temp_PSSS;
	for (; NULL!=temp_PSSS; Last_PSSS= temp_PSSS, temp_PSSS= temp_PSSS->Next)
	{
		if ((static_cast<int>(temp_PSSS->lPitch)>=(OFFSCREEN_Ptr->lPitch))&&(static_cast<int>(temp_PSSS->Height)>=(OFFSCREEN_Ptr->Height)))
		{
			if (TADDraw_lp==temp_PSSS->myDDraw_Ptr)
			{
				return temp_PSSS;
			}
			else
			{
				
				Last_PSSS->Next= temp_PSSS->Next;//* 断开链条 *//
				FreeSpecScreenSurface ( temp_PSSS);
				if (UnicodeFontDrawCache==temp_PSSS)
				{
					UnicodeFontDrawCache= NULL;
				}
			}
		}
	}
	temp_PSSS= NewSpecScreenSurface ( OFFSCREEN_Ptr);
	temp_PSSS->Next= UnicodeFontDrawCache;
	UnicodeFontDrawCache= temp_PSSS;
	return temp_PSSS;
}
////////////////
//DrawText
///////////////////////////
BOOL UnicodeSupport::UnicodeDrawTextA (OFFSCREEN * OFFSCREEN_Ptr, char * Str_cstrp, LPDIRECTDRAW TADDrawArgc_lp, int X_Off, int Y_Off)
{
	__try
	{
		//TADDraw_lp= TADDrawArgc_lp;
		RECT TextRect;
		TextRect.top= X_Off;
		TextRect.left= Y_Off;
		TextRect.right= OFFSCREEN_Ptr->ScreenRect.right;
		TextRect.bottom= OFFSCREEN_Ptr->ScreenRect.bottom;

		int ContextScreenMemBytesLen= (OFFSCREEN_Ptr->Height- 1)* (OFFSCREEN_Ptr->lPitch);
// 		IDDrawSurface::OutptTxt ( Str_cstrp);
// 		IDDrawSurface::OutptTxt ( "x:");
// 		IDDrawSurface::OutptInt ( X_Off);
// 		IDDrawSurface::OutptTxt ( "y:");
// 		IDDrawSurface::OutptInt ( Y_Off);

		PSpecScreenSurface temp_PSSS= GetSpecScreenSurface ( OFFSCREEN_Ptr);
		if (TRUE==UnicodeValid)
		{//only drawText when shall using unicode

			//*现在的实现，会让屏幕刷新速度慢到爆 *//
			// Copy Context Screen In
			if (DD_OK==temp_PSSS->DDSurface_P->Lock ( NULL, &(temp_PSSS->DDSurfaceDescForLock), DDLOCK_SURFACEMEMORYPTR, NULL))
			{
				temp_PSSS->SurfaceMem_Pvoid= temp_PSSS->DDSurfaceDescForLock.lpSurface;
				temp_PSSS->lPitch= (temp_PSSS->DDSurfaceDescForLock.lPitch);
				////just copy all bytes in SurfaceMem;
				if (ContextScreenMemBytesLen==temp_PSSS->lPitch)
				{
					memcpy_s ( temp_PSSS->SurfaceMem_Pvoid, ContextScreenMemBytesLen, OFFSCREEN_Ptr->lpSurface, ContextScreenMemBytesLen);
				}
				else
				{
					CopyToSpecSurfaceMem ( temp_PSSS, OFFSCREEN_Ptr);
				}
				
				temp_PSSS->DDSurface_P->Unlock ( &(temp_PSSS->DDSurfaceDescForLock));
				DDRAW_INIT_STRUCT( temp_PSSS->DDSurfaceDescForLock);
			}
			else
			{
				__leave;
			}

		// SetFontStyle_InDDSurface

			
			if (DD_OK==(temp_PSSS->DDSurface_P)->GetDC ( &(temp_PSSS->mySurface_HDC)))
			{
				//SetBkMode ( temp_PSSS->mySurface_HDC, TRANSPARENT);
				SelectObject ( temp_PSSS->mySurface_HDC, UnicodeYellowFont);
				SetTextColor ( temp_PSSS->mySurface_HDC, RGB(0xd0, 0xd0, 0xb3));///黄色 (?)
				SetBkColor (  temp_PSSS->mySurface_HDC, RGB(139,139,139));
				::DrawTextExA ( temp_PSSS->mySurface_HDC, Str_cstrp, -1, &(TextRect), DT_NOCLIP, NULL);
			}
			else
			{
				__leave;
			}


			if (NULL!=temp_PSSS->mySurface_HDC)
			{
				temp_PSSS->DDSurface_P->ReleaseDC ( temp_PSSS->mySurface_HDC);
			}

		//CopySurfaceMemOut
			if (DD_OK==temp_PSSS->DDSurface_P->Lock ( NULL, &(temp_PSSS->DDSurfaceDescForLock), DDLOCK_SURFACEMEMORYPTR, NULL))
			{
				temp_PSSS->SurfaceMem_Pvoid= temp_PSSS->DDSurfaceDescForLock.lpSurface;
				temp_PSSS->lPitch= (temp_PSSS->DDSurfaceDescForLock.lPitch);
				if (ContextScreenMemBytesLen==temp_PSSS->lPitch)
				{
					memcpy_s ( OFFSCREEN_Ptr->lpSurface, ContextScreenMemBytesLen, temp_PSSS->SurfaceMem_Pvoid, ContextScreenMemBytesLen);
				}
				else
				{
					CopyToContextScreenMem ( OFFSCREEN_Ptr, temp_PSSS);
				}
				//
				temp_PSSS->DDSurface_P->Unlock ( &(temp_PSSS->DDSurfaceDescForLock));
				DDRAW_INIT_STRUCT( temp_PSSS->DDSurfaceDescForLock);
			}
			else
			{
				__leave;
			}

			return TRUE;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		;	
	}
	return FALSE;
/*	ReleaseFontStyle_InDDSurface ( DDSurface_lp, HDC_mySurface);*/
}
BOOL UnicodeSupport::IsIDDrawLost (void)
{
	if (TADDraw_lp!=(LPDIRECTDRAW)LocalShare->TADirectDraw)
	{//
		return TRUE;
	}
	return FALSE;
}
void UnicodeSupport::RestoreLocalSurf ( void)
{
	//= (LPDIRECTDRAW)LocalShare->TADirectDraw;


	Width= 200;
	Height= 300;

	TADDraw_lp=(LPDIRECTDRAW)LocalShare->TADirectDraw;

	DDSURFACEDESC DDSD_temp;
	DDRAW_INIT_STRUCT( DDSD_temp);
	DDSD_temp.dwFlags|= DDSD_HEIGHT;
	DDSD_temp.dwFlags|= DDSD_WIDTH;
	DDSD_temp.dwFlags|= DDSD_CAPS;

	DDSD_temp.dwHeight= Height;
	DDSD_temp.dwWidth= Width;
	DDSD_temp.ddsCaps.dwCaps= DDSCAPS_OFFSCREENPLAIN| DDSCAPS_SYSTEMMEMORY;
	lpImeSurface= NULL;//no use ptr at here.

	TADDraw_lp->CreateSurface ( &DDSD_temp, &lpImeSurface, NULL);

	UpdateImeFrame ( );

	PSpecScreenSurface temp2_PSSS;
	for (PSpecScreenSurface temp_PSSS= UnicodeFontDrawCache; NULL!=temp_PSSS; FreeSpecScreenSurface ( temp2_PSSS))
	{
		temp2_PSSS= temp_PSSS;
		temp_PSSS= temp_PSSS->Next;
	}
}

bool UnicodeSupport::Blt (LPDIRECTDRAWSURFACE DescSurface)
{
	//
	__try
	{
			if (NULL==DescSurface)
		{
			return false;
		}

		if (!UnicodeValid)
		{
			return false;
		}

		if (!ImeShowing)
		{
			return false;
		}

		if (IsIDDrawLost())
		{
			RestoreLocalSurf ( );/////////ugly
		}

		if (DD_OK!=lpImeSurface->IsLost ( ))
		{
			lpImeSurface->Restore ( );
		}

		RECT Dest;
		Dest.left = xPos;
		Dest.top = yPos;
		Dest.right = xPos + Width;
		Dest.bottom = yPos + Height;
		DDBLTFX ddbltfx;
		DDRAW_INIT_STRUCT( ddbltfx);
		ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue= ImeSurfaceBackground;

		RECT Src;

		Src.left= Src.top= 0;
		Src.right= Width;
		Src.bottom= Height;
		if(DescSurface->Blt(&Dest, lpImeSurface, &Src, DDBLT_ASYNC| DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
		{
			DescSurface->Blt(&Dest, lpImeSurface, &Src, DDBLT_WAIT| DDBLT_KEYSRCOVERRIDE, &ddbltfx);
		}

		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return false;
}



/*
template<class InputIterator, class T>
InputIterator vector_find ( InputIterator first, InputIterator last, const T& value )
{
	for ( ;first!=last; first++) 
	{
		if ( 0==memcmp ( &(*first), &value, sizeof(T)) )
		{
			break;
		}
	}
	return first;
}
BOOL CALLBACK UAVW_EnumProc (HWND hwnd, LPARAM lParam)
{
	vector<VisibleWnd> * VisibleWnd_VecP= reinterpret_cast<vector<VisibleWnd> * >(lParam);
	if (NULL!=hwnd)
	{
		DWORD PID;
		GetWindowThreadProcessId ( hwnd, &PID);
		if (PID!=GetCurrentProcessId ( ))
		{
			return TRUE;
		}
		char Text[255];
		GetClassName ( hwnd,Text,254);
		if(strcmp(Text,"Total Annihilation Class")!=0)
		{// we don't care about TA wnd
			VisibleWnd tmp;
			tmp.hdc= GetWindowDC ( hwnd);
			tmp.hwnd= hwnd;
			GetWindowRect ( hwnd, &(tmp.rect));
			if ((0!=(tmp.rect.right- tmp.rect.left))
				&&(0!=(tmp.rect.bottom- tmp.rect.top)))
			{
				if (VisibleWnd_VecP->end ()==vector_find ( VisibleWnd_VecP->begin (), VisibleWnd_VecP->end (), tmp))
				{
					VisibleWnd_VecP->push_back ( tmp);
				}
			}
		}
	}

	return TRUE;
}

void UnicodeSupport::UpdateAllVisibleWindow (void)
{
	EnumWindows( UAVW_EnumProc, reinterpret_cast <LPARAM>(&VisibleWnd_Vec));
}*/


void UnicodeSupport::ReleaseCandidateList (void)
{
	while (!CandidateList.empty ( ))
	{
		if (NULL!=CandidateList.back ( ))
		{
			delete	CandidateList.back ( );	
		}
		CandidateList.pop_back ( );
	}
	memset ( InputStrBuf, 0, 0x100);

}

int UnicodeSupport::DrawSeparator (HDC ImeHdc, int Length, int * Curt_Width, int * Curt_Height)
{
	RECT tmp_Rect;
	tmp_Rect.left= 0;
	tmp_Rect.right= *Curt_Width;
	tmp_Rect.top= *Curt_Height;
	*Curt_Height+= Length;
	tmp_Rect.bottom= *Curt_Height;

	return FillRect ( ImeHdc, &tmp_Rect, (HBRUSH) COLOR_BTNHIGHLIGHT);
}
void UnicodeSupport::DrawALine_Ime (HDC ImeHdc, LPSTR StrBuf, int * Curt_Width, int * Curt_Height)
{
	SIZE Size_buf;
	RECT TextRect;

	GetTextExtentPoint32 ( ImeHdc, StrBuf, strnlen ( StrBuf, 0x100), &Size_buf);
	//Height=
	//memset ( );
	TextRect.top= *Curt_Height;
	TextRect.left= 0;
	TextRect.right= Size_buf.cx;
	*Curt_Height+= Size_buf.cy;
	TextRect.bottom= *Curt_Height;
	
	DrawTextExA ( ImeHdc, StrBuf, -1, &(TextRect), DT_NOCLIP, NULL);
	if (*Curt_Width< Size_buf.cx)
	{
		*Curt_Width= Size_buf.cx;
	}
}

bool UnicodeSupport::UpdateImeFrame (void)
{
	Width= 0;
	Height= 0;// delay 10 pixel for the top
	
//	RECT ImeRect;
	DDSURFACEDESC ddsd;
	DDRAW_INIT_STRUCT( ddsd);
	if (NULL==lpImeSurface)
	{
		return false;
	}

	if (IsIDDrawLost())
	{
		RestoreLocalSurf ( );/////////ugly
	}

	if (DD_OK!=lpImeSurface->IsLost ( ))
	{
		lpImeSurface->Restore ( );
	}

	if (DD_OK!=lpImeSurface->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR, NULL))
	{
		return false;
	}

	memset (ddsd.lpSurface, 1, ddsd.lPitch* ddsd.dwHeight);
	lpImeSurface->Unlock ( ddsd.lpSurface);
	HDC ImeHdc;
	lpImeSurface->GetDC ( &ImeHdc);
	if (NULL==ImeHdc)
	{
		return false; //
	}

	//SetBkMode ( ImeHdc, TRANSPARENT);
	SelectObject ( ImeHdc, UnicodeYellowFont);
	SetTextColor ( ImeHdc, RGB(0xd0, 0xd0, 0xb3));///黄色 (?)
	SetBkColor ( ImeHdc, RGB(139,139,139));

	DrawSeparator ( ImeHdc, 10, &Width, &Height);
	if (0!=IMEName[0])
	{
		DrawALine_Ime ( ImeHdc, IMEName, &Width, &Height);
		DrawSeparator ( ImeHdc, 10, &Width, &Height);
	}
	if (0!=InputStrBuf[0])
	{
		DrawALine_Ime ( ImeHdc, InputStrBuf, &Width, &Height);
		DrawSeparator ( ImeHdc, 10, &Width, &Height);
	}

	char CandLine[0x100];
	int i= 1;
	for (vector<LPSTR>::iterator Cand_iter= CandidateList.begin ( ); Cand_iter!=CandidateList.end ( ); ++Cand_iter, ++i)
	{
		wsprintfA ( CandLine, "%d.%s", i,  *Cand_iter);
		DrawALine_Ime ( ImeHdc, CandLine, &Width, &Height);
		DrawSeparator ( ImeHdc, 10, &Width, &Height);
	}

	lpImeSurface->ReleaseDC ( ImeHdc);

	return true;
}

void UnicodeSupport::SendStr (char * InputStrBuf)
{
	for (int i= 0; 0!=InputStrBuf[i]; ++i)
	{
		SendMessageA ( *reinterpret_cast<HWND *>(0x51F320+ 0x0040), WM_CHAR, InputStrBuf[i], 0);
	}
}

bool UnicodeSupport::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	bool RTN_b= false;
	if (!UnicodeValid)
	{
		return RTN_b;
	}

	switch(Msg) 
	{ 
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
		int X, Y;
		X = (int) LOWORD(lParam);    // horizontal position 
		Y = (int) HIWORD(lParam);    // vertical position 

		if (WM_LBUTTONDOWN==Msg)
		{
			if (ImeShowing)
			{		
				if ((xPos<X)
					&&(X<(xPos+ Width))
					&&(yPos<Y)
					&&(Y<(yPos+ Height)))
				{// inner ime surface
					LMouseDown= TRUE;
					RTN_b= TRUE;
				}
			}
		}
		else if (WM_LBUTTONUP==Msg)
		{
			if (LMouseDown)
			{
				xPos= X;
				yPos= Y;
				LMouseDown= FALSE;
				RTN_b= TRUE;
			}
		}


		break;
	case WM_IME_STARTCOMPOSITION:
		// *这个消息出现时，应该把屏幕stunk住，好让输入法的显示不被拦截。但还是需要交给DefWindowsProc处理 * //
		IDDrawSurface::OutptTxt ( "WM_IME_STARTCOMPOSITION");
		ReleaseCandidateList ( );
		UpdateImeFrame ( );

		RTN_b= true;
		ImeShowing= TRUE;
		break;

	case WM_IME_COMPOSITION:
		IDDrawSurface::OutptTxt ( "WM_IME_COMPOSITION");
#ifdef DEBUG_INFO
		IDDrawSurface::OutptTxt ( IME_COMPOSITION_STR[lParam- GCS_COMPREADSTR]);
#endif
		DWORD dwIndex;

		//RTN_b= true;
		switch((BOOL)lParam)
		{
		case GCS_COMPSTR:
			
		case GCS_COMPREADSTR:
			//dwIndex= GCS_COMPREADSTR;
			//IDDrawSurface::OutptTxt ( "GCS_COMPSTR");
			dwIndex= (BOOL)lParam;
			memset ( InputStrBuf, 0, sizeof(InputStrBuf));
			
			ImmGetCompositionString	( hIMC, dwIndex, InputStrBuf, sizeof(InputStrBuf));
			//RTN_b= true;
			strcat_s ( InputStrBuf, sizeof(InputStrBuf), InputStrBuf);
			IDDrawSurface::OutptTxt ( InputStrBuf);
			UpdateImeFrame ( );
			ImeShowing= TRUE;
			break;

		case GCS_RESULTSTR:
			//IDDrawSurface::OutptTxt ( "GCS_RESULTSTR");
			memset ( InputStrBuf, 0, sizeof(InputStrBuf));
			ImmGetCompositionString	( hIMC, GCS_RESULTSTR, InputStrBuf, sizeof(InputStrBuf));

			strcat_s ( InputStrBuf, sizeof(InputStrBuf), InputStrBuf);

			IDDrawSurface::OutptTxt ( InputStrBuf);

			SendStr ( InputStrBuf);

			InputStrBuf[0]= 0; // no need the str at now

			UpdateImeFrame ( );
			ImeShowing= FALSE;

			//RTN_b= false;
			break;
		default:
			;
		}
		// 
		//RTN_b= true;
		break;
	case WM_IME_CHAR:
		IDDrawSurface::OutptTxt ( "WM_IME_CHAR");
		IDDrawSurface::OutptTxt ( reinterpret_cast<char *>(&wParam));
		break;
	case WM_IME_NOTIFY:
		IDDrawSurface::OutptTxt ( "WM_IME_NOTIFY");
#ifdef DEBUG_INFO
		IDDrawSurface::OutptTxt ( IME_NOTIFY_STR[wParam- IMN_CLOSESTATUSWINDOW]);
#endif
		switch (wParam)
		{
		case IMN_OPENCANDIDATE:
		case IMN_CHANGECANDIDATE:
			//IDDrawSurface::OutptTxt ( "IMN_CHANGECANDIDATE");
			DWORD NewLen;
			if (IMN_CHANGECANDIDATE==wParam)
			{
				memset ( InputStrBuf, 0, sizeof(InputStrBuf));
				ImmGetCompositionString	( hIMC, GCS_COMPSTR, InputStrBuf, sizeof(InputStrBuf));
				//strcat_s ( InputStrBuf, sizeof(InputStrBuf), InputStrBuf);
				IDDrawSurface::OutptTxt ( InputStrBuf);
			}
			else
			{
				InputStrBuf[0]= 0;
			}
			
			NewLen= ImmGetCandidateList ( hIMC, 0, NULL, 0);
			if ((NULL==lpCandList)||
				(NewLen>CurrentCandListLen))
			{
				if (NULL!=lpCandList)
				{
					delete lpCandList;
				}
				lpCandList= reinterpret_cast<LPCANDIDATELIST>(new BYTE[NewLen]);
				CurrentCandListLen= NewLen;
			}

			memset ( lpCandList, 0, CurrentCandListLen);
			
			ReleaseCandidateList ( );
			ImmGetCandidateList ( hIMC, 0, lpCandList, CurrentCandListLen);
			if ((IME_CAND_CODE==lpCandList->dwStyle)
				&&(1==lpCandList->dwCount))
			{
				CandidateList.push_back ( reinterpret_cast<char *>(lpCandList->dwOffset));
				//IDDrawSurface::OutptTxt ( CandidateList.back ( ));
			}
			else
			{
				for (DWORD i= 0; i<lpCandList->dwCount; ++i)
				{
					CandidateList.push_back ( duplicate_str (reinterpret_cast<LPSTR>(lpCandList)+ lpCandList->dwOffset[i]));
					//CandidateList.back ( );
					//IDDrawSurface::OutptTxt ( CandidateList.back ( ));
				}
			}
			UpdateImeFrame ( );
			RTN_b= true;
			ImeShowing= TRUE;
			break;
		case IMN_CLOSECANDIDATE:
			//cls the lpImeSurface;
			//IDDrawSurface::OutptTxt ( "IMN_CLOSECANDIDATE");

			//memset ( InputStrBuf, 0, sizeof(InputStrBuf));
			//ImmGetCompositionString	( hIMC, GCS_RESULTSTR, InputStrBuf, sizeof(InputStrBuf));

			//strcat_s ( InputStrBuf, sizeof(InputStrBuf), InputStrBuf);
			//IDDrawSurface::OutptTxt ( InputStrBuf);

			//SendStr ( InputStrBuf);

			ReleaseCandidateList ( );	
			InputStrBuf[0]= 0;
			UpdateImeFrame ( );
			RTN_b= true;
			
			ImeShowing= FALSE;
			break;
		default:

			;
		}
			
		break;
	case WM_INPUTLANGCHANGE:
		IDDrawSurface::OutptTxt ( "WM_INPUTLANGCHANGE");
		HKL hKL;
		hKL= GetKeyboardLayout( 0 );
		if( ImmIsIME( hKL ))
		{
			//HIMC hIMC = ImmGetContext( hWnd );
			ImmEscape ( hKL, hIMC, IME_ESC_IME_NAME, IMEName); 
			//ImmReleaseContext( hWnd, hIMC );
		}
		else 
		{
			IMEName[0]= 0;
			//strcpy_s ( , 0x100, "");
		}
		IDDrawSurface::OutptTxt ( IMEName);

		UpdateImeFrame ( );
		break;
	case WM_IME_ENDCOMPOSITION:
		IDDrawSurface::OutptTxt ( "WM_IME_ENDCOMPOSITION");
		memset ( InputStrBuf, 0, sizeof(InputStrBuf));
		// no need the str at now, 
		UpdateImeFrame ( );
		ImeShowing= FALSE;

		RTN_b= true;
		break;
	case WM_IME_CONTROL:
		IDDrawSurface::OutptTxt ( "WM_IME_CONTROL");
		break;
	case WM_IME_KEYDOWN:
		IDDrawSurface::OutptTxt ( "WM_IME_KEYDOWN");
		break;
	case WM_IME_KEYUP:
		IDDrawSurface::OutptTxt ( "WM_IME_KEYUP");
		break;
	case WM_IME_SELECT:
		IDDrawSurface::OutptTxt ( "WM_IME_SELECT");
		break;
	case WM_IME_SETCONTEXT:
		IDDrawSurface::OutptTxt ( "WM_IME_SETCONTEXT");
		break;
	default:
		;
	}

	return RTN_b;
}


#ifdef DEBUG_INFO

LPSTR IME_COMPOSITION_STR[]=
{
	"GCS_COMPREADSTR",
	"GCS_COMPREADATTR",
	"GCS_COMPREADCLAUSE",
	"GCS_COMPSTR",
	"GCS_COMPATTR",
	"GCS_COMPCLAUSE",
	"GCS_CURSORPOS",
	"GCS_DELTASTART",
	"GCS_RESULTREADSTR",
	"GCS_RESULTREADCLAUSE",
	"GCS_RESULTSTR",
	"GCS_RESULTCLAUSE"
};
LPSTR IME_NOTIFY_STR[]= 
{
	"IMN_CLOSESTATUSWINDOW",
	"IMN_OPENSTATUSWINDOW",
	"IMN_CHANGECANDIDATE",
	"IMN_CLOSECANDIDATE",
	"IMN_OPENCANDIDATE",
	"IMN_SETCONVERSIONMODE",
	"IMN_SETSENTENCEMODE",
	"IMN_SETOPENSTATUS",
	"IMN_SETCANDIDATEPOS",
	"IMN_SETCOMPOSITIONFONT",
	"IMN_SETCOMPOSITIONWINDOW",
	"IMN_SETSTATUSWINDOWPOS",
	"IMN_GUIDELINE",
	"IMN_PRIVATE"
};
#endif


void CopyToContextScreenMem (OFFSCREEN * OFFSCREEN_Ptr, PSpecScreenSurface SrcSpecScreenSurface)
{
	//
	int DesclPitch= OFFSCREEN_Ptr->lPitch;
	int SrclPitch= SrcSpecScreenSurface->lPitch;
	LPBYTE Desc= static_cast<LPBYTE>(OFFSCREEN_Ptr->lpSurface);
	LPBYTE Src= static_cast<LPBYTE>(SrcSpecScreenSurface->SurfaceMem_Pvoid);
	for (unsigned int i= 0; i<(OFFSCREEN_Ptr->Height- 2); ++i)
	{
		memcpy_s ( Desc+ (i* DesclPitch), DesclPitch, Src+ (i* SrclPitch),  DesclPitch);
	}
}

void CopyToSpecSurfaceMem (PSpecScreenSurface SrcSpecScreenSurface, OFFSCREEN * OFFSCREEN_Ptr)
{
	//
	int SrclPitch= OFFSCREEN_Ptr->lPitch;
	int DesclPitch= SrcSpecScreenSurface->lPitch;
	LPBYTE Src= static_cast<LPBYTE>(OFFSCREEN_Ptr->lpSurface);
	LPBYTE Desc= static_cast<LPBYTE>(SrcSpecScreenSurface->SurfaceMem_Pvoid);
	for (unsigned int i= 0; i<(OFFSCREEN_Ptr->Height- 2); ++i)
	{
		memcpy_s ( Desc+ (i* DesclPitch), DesclPitch, Src+ (i* SrclPitch),  DesclPitch);
	}
}

int __stdcall USDrawTextInScreen_HookRouter (PInlineX86StackBuffer X86StrackBuffer)
{
	__try
	{
		if(!NowCrackLimit->NowSupportUnicode->UnicodeValid)
		{
			__leave;
		}
		LPVOID RtnAddr= * ((LPVOID *)X86StrackBuffer->Esp);
		OFFSCREEN * OFFSCREEN_Ptr= * ((OFFSCREEN * *)(X86StrackBuffer->Esp+ 4));
		char * Str= * ((char * *)(X86StrackBuffer->Esp+ 8));
		int Y_Off= * ((int *)(X86StrackBuffer->Esp+ 0xc));
		int X_Off= * ((int *)(X86StrackBuffer->Esp+ 0x10));

		if (0>=strlen ( Str))
		{
			__leave;
		}
		LPDIRECTDRAW temp_PDDraw= static_cast <LPDIRECTDRAW>(LocalShare->TADirectDraw);
		if (NULL==temp_PDDraw)
		{//窗口模式
			__leave;
		}

		if (((NowCrackLimit->NowSupportUnicode))->UnicodeDrawTextA ( OFFSCREEN_Ptr, Str, temp_PDDraw, X_Off, Y_Off))
		{
			X86StrackBuffer->Esp= X86StrackBuffer->Esp+ 0x4+ 0x18;// 先pop  addr,然后加上retn 0x18
			X86StrackBuffer->rtnAddr_Pvoid= RtnAddr;
			return X86STRACKBUFFERCHANGE;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return 0;
}

////////
// .text:004AB942 83 FB 20                                      cmp     ebx, 20h
// 	.text:004AB945 0F 8C 84 00 00 00                             jl      loc_4AB9CF
// 	.text:004AB94B 83 FB 7F                                      cmp     ebx, 7Fh
// 	.text:004AB94E 7F 7F                                         jg      short loc_4AB9CF
static bool DBSC_Flag_Global= false;
int __stdcall myValidChar  (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(!NowCrackLimit->NowSupportUnicode->UnicodeValid)
		{
			__leave;
		}
		if (DBSC_Flag_Global)
		{
			//* 汉字的第二个字节 *//
			//
			DBSC_Flag_Global= false;

			X86StrackBuffer->rtnAddr_Pvoid= (LPVOID)0x4ab950;

// 			mov     eax, [esp+20h+var_10]
			X86StrackBuffer->Eax= *((DWORD *)(X86StrackBuffer->Esp+ 0x10));
// 			movsx   edx, word ptr [eax+138h]
			X86StrackBuffer->Edx= *((WORD *)(X86StrackBuffer->Eax+ 0x138));


			return X86STRACKBUFFERCHANGE;
		}
		DWORD TAProgramStruct= * ((DWORD *)0x51FBD0);
		DWORD CurrentReaded= * ((DWORD *)(TAProgramStruct+ 0x172));
		DWORD Total= * ((DWORD *)(TAProgramStruct+ 0x16e));
// 
 		if (Total<=CurrentReaded)
		{//汉字最少会同时有二个字节
			__leave;
		}

		BYTE CurrentChar= * ((BYTE *)(TAProgramStruct+ 0xf2+ CurrentReaded* 4));//static_cast<BYTE> (X86StrackBuffer->Ebx);

		if (! IsDBCSLeadByte( CurrentChar))
		{
			__leave;
		}
		DBSC_Flag_Global= true;

		X86StrackBuffer->rtnAddr_Pvoid= (LPVOID)0x4ab950;


		// 			mov     eax, [esp+20h+var_10]
		X86StrackBuffer->Eax= *((DWORD *)(X86StrackBuffer->Esp+ 0x10));
		// 			movsx   edx, word ptr [eax+138h]
		X86StrackBuffer->Edx= *((WORD *)(X86StrackBuffer->Eax+ 0x138));

		return X86STRACKBUFFERCHANGE;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return 0;
}
// .text:004AB79B 0F 87 8E 01 00 00                             ja      GetCharFromInput ; default

int __stdcall DBSC2rdByte (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(!NowCrackLimit->NowSupportUnicode->UnicodeValid)
		{
			__leave;
		}
		if (DBSC_Flag_Global)
		{
			X86StrackBuffer->rtnAddr_Pvoid= (LPVOID)0x04AB92F;
			return X86STRACKBUFFERCHANGE;
		}
	}
	
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return 0;
}

//004AB7F9   C6442F FF 00     MOV BYTE PTR DS:[EDI+EBP-1],0
int __stdcall myDeleteChar (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(!NowCrackLimit->NowSupportUnicode->UnicodeValid)
		{
			__leave;
		}
			
		LPSTR Str= (LPSTR)(X86StrackBuffer->Ebp);
		LPSTR DBSCchar_P= (Str+ X86StrackBuffer->Eax- 1);
		if (NULL!=Str&&NULL!=DBSCchar_P&&(X86StrackBuffer->Edi>1))
		{
			if (DBSCchar_P>=Str)
			{// at least, 2 bytes for a DBSC chinese.
				if (IsDBCSLeadByte ( *DBSCchar_P))
				{
					*DBSCchar_P= 0;
					--(*((DWORD *)(X86StrackBuffer->Esi+ 0x74)));
					//--(X86StrackBuffer->Eax);
				}
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return 0;
}
// 
// .text:004C1744 E8 17 B8 00 00                                                  call    Blt_BottomState_Text
// 	.text:004C1749 83 C4 24                                                        add     esp, 24h

int __stdcall Blt_BottomState0_Text (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(!NowCrackLimit->NowSupportUnicode->UnicodeValid)
		{
			__leave;
		}

		LPVOID RtnAddr= (LPVOID)(Blt_BottomState0_TextRtn);
		OFFSCREEN * OFFSCREEN_Ptr= ((OFFSCREEN *)(X86StrackBuffer->Esp+ 0x5c));
		
		char * Str= * (char * *)(X86StrackBuffer->Esp+ 0x0c);
		int Y_Off= * ((int *)(X86StrackBuffer->Esp+ 0x10));
		int X_Off= * ((int *)(X86StrackBuffer->Esp+ 0x14));
		LPVOID lpSurface= *(LPVOID *)(X86StrackBuffer->Esp);

		if (0>=strlen ( Str))
		{
			__leave;
		}
		LPDIRECTDRAW temp_PDDraw= static_cast <LPDIRECTDRAW>(LocalShare->TADirectDraw);
		if (NULL==temp_PDDraw)
		{//窗口模式
			__leave;
		}

		if (((NowCrackLimit->NowSupportUnicode))->UnicodeDrawTextA ( OFFSCREEN_Ptr, Str, temp_PDDraw, X_Off, Y_Off))
		{
			X86StrackBuffer->rtnAddr_Pvoid= RtnAddr;
			return X86STRACKBUFFERCHANGE;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return 0;
}
// 
	// .text:004C16D4 E8 87 B8 00 00                                                  call    Blt_BottomState_Text
	// 	.text:004C16D9 83 C4 24                                                        add     esp, 24h
int __stdcall Blt_BottomState1_Text (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(!NowCrackLimit->NowSupportUnicode->UnicodeValid)
		{
			__leave;
		}

		LPVOID RtnAddr= (LPVOID)(Blt_BottomState1_TextRtn);
		OFFSCREEN * OFFSCREEN_Ptr= ((OFFSCREEN *)(X86StrackBuffer->Edi));

		char * Str= * (char * *)(X86StrackBuffer->Esp+ 0x0c);
		int Y_Off= * ((int *)(X86StrackBuffer->Esp+ 0x10));
		int X_Off= * ((int *)(X86StrackBuffer->Esp+ 0x14));
		LPVOID lpSurface= *(LPVOID *)(X86StrackBuffer->Esp);

		if (0>=strlen ( Str))
		{
			__leave;
		}
		LPDIRECTDRAW temp_PDDraw= static_cast <LPDIRECTDRAW>(LocalShare->TADirectDraw);
		if (NULL==temp_PDDraw)
		{//窗口模式
			__leave;
		}

		if (((NowCrackLimit->NowSupportUnicode))->UnicodeDrawTextA ( OFFSCREEN_Ptr, Str, temp_PDDraw, X_Off, Y_Off))
		{
			X86StrackBuffer->rtnAddr_Pvoid= RtnAddr;
			return X86STRACKBUFFERCHANGE;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return 0;
}

////------
//00468AC0 A1 E8 1D 51 00   mov     eax, TAMainStructPtr

int __stdcall PopadState (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(!NowCrackLimit->NowSupportUnicode->UnicodeValid)
		{
			__leave;
		}

		X86StrackBuffer->Esi= 0xFFFFFFE1u; 
		X86StrackBuffer->Eax= reinterpret_cast<DWORD>(*TAmainStruct_PtrPtr);
		return X86STRACKBUFFERCHANGE;
		
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return 0;
}
/*

unsigned int InnerCreateWindow;
unsigned int CreateWindowExW_rtn;
int __stdcall CreateWindowExW_new (PInlineX86StackBuffer X86StrackBuffer)
{
	
	DWORD RealEBP= X86StrackBuffer->Esp- 4;
	HWND Hwnd;
	DWORD Width= *reinterpret_cast<DWORD *>(X86StrackBuffer->Esp+ 0x20);
	DWORD Height= *reinterpret_cast<DWORD *>(X86StrackBuffer->Esp+ 0x24);
	LPWSTR ClassName= *reinterpret_cast<LPWSTR *>(X86StrackBuffer->Esp+ 0xc);
	VisibleWnd tmp;
	if ((NULL!=ClassName)
		&&(0==wcscmp ( ClassName, L"Total Annihilation Class")))
	{
		return 0;
	}


/ *
	if ((0==Width)
		||(0==Height))
	{
		return 0;
	}* /

	__asm
	{
		mov ESI, RealEBP;
		PUSH 0x40000000
		PUSH DWORD PTR SS:[ESI+0x34]
		PUSH DWORD PTR SS:[ESI+0x30]
		PUSH DWORD PTR SS:[ESI+0x2C]
		PUSH DWORD PTR SS:[ESI+0x28]
		PUSH DWORD PTR SS:[ESI+0x24]
		PUSH DWORD PTR SS:[ESI+0x20]
		PUSH DWORD PTR SS:[ESI+0x1C]
		PUSH DWORD PTR SS:[ESI+0x18]
		PUSH DWORD PTR SS:[ESI+0x14]
		PUSH DWORD PTR SS:[ESI+0x10]
		PUSH DWORD PTR SS:[ESI+0xC]
		PUSH DWORD PTR SS:[ESI+0x8]

		call InnerCreateWindow;
		mov Hwnd, eax;
	}

	
	//tmp.hdc= GetWindowDC ( Hwnd);
	tmp.hwnd= Hwnd;
	
	vector<VisibleWnd> * VisibleWnd_VecP= reinterpret_cast< vector<VisibleWnd> *>(X86StrackBuffer->myInlineHookClass_Pish->ParamOfHook);
	VisibleWnd_VecP->push_back ( tmp);
}*/