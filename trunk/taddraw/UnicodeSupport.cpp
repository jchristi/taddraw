#include "oddraw.h"
#include "iddrawsurface.h"
#include "tamem.h"
#include "hook/etc.h"
#include "hook/hook.h"
#include "UnicodeSupport.h"

#ifdef UNICODE_SUPPORT
UnicodeSupport::UnicodeSupport(void)
{
	DrawTextInScreen_ISH= NULL;
	ValidChar_ISH= NULL;
	DeleteChar_ISH= NULL;
	DBSC2rdByte_ISH= NULL;

	UnicodeWhiteFont= NULL;
	UnicodeYellowFont=NULL;
	TADDraw_lp= NULL;
	UnicodeValid= FALSE;

	UnicodeYellowFont= CreateFontA ( 
		12,                                                 //   nHeight 
		0,                                                   //   nWidth 
		0,                                                   //   nEscapement 
		0,                                                   //   nOrientation 
		FW_BOLD,                                   //   nWeight 
		FALSE,                                           //   bItalic 
		FALSE,                                           //   bUnderline 
		0,                                                   //   cStrikeOut 
		GB2312_CHARSET,                             //   nCharSet 
		OUT_DEFAULT_PRECIS,                 //   nOutPrecision 
		CLIP_DEFAULT_PRECIS,               //   nClipPrecision 	
		PROOF_QUALITY,                       //   nQuality 
		DEFAULT_PITCH   |   FF_SWISS,     //   nPitchAndFamily 
		"宋体");     //   lpszFacename 
	if (NULL==UnicodeYellowFont)
	{
		UnicodeValid= FALSE;
		return;
	}
	UnicodeWhiteFont= CreateFontA ( 
		12,                                                 //   nHeight 
		0,                                                   //   nWidth 
		0,                                                   //   nEscapement 
		0,                                                   //   nOrientation 
		FW_BOLD,                                   //   nWeight 
		FALSE,                                           //   bItalic 
		FALSE,                                           //   bUnderline 
		0,                                                   //   cStrikeOut 
		GB2312_CHARSET,                             //   nCharSet 
		OUT_DEFAULT_PRECIS,                 //   nOutPrecision 
		CLIP_DEFAULT_PRECIS,               //   nClipPrecision 
		PROOF_QUALITY,                       //   nQuality 
		DEFAULT_PITCH   |   FF_SWISS,     //   nPitchAndFamily 
		"宋体");     //   lpszFacename 
	//install DrawTextInScreen HOOK to TA.
	DrawTextInScreen_ISH= new InlineSingleHook ( 0x4A50E0, 0x5, INLINE_5BYTESLAGGERJMP, USDrawTextInScreen_HookRouter);
	
	//BYTE FF_Dw[]= {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	ValidChar_ISH= new InlineSingleHook ( 0x004AB79B, 5, INLINE_5BYTESLAGGERJMP, myValidChar);// 0x04AB942
	DeleteChar_ISH= new InlineSingleHook ( 0x004AB7E8, 5, INLINE_5BYTESLAGGERJMP, myDeleteChar);// 
//	DBSC2rdByte_ISH=  new InlineSingleHook ( 0x004AB79B, 5, INLINE_5BYTESLAGGERJMP, DBSC2rdByte);// 

	UnicodeValid= TRUE;

	TADDraw_lp= (LPDIRECTDRAW)LocalShare->TADirectDraw;

	UnicodeFontDrawCache= NULL;
	IsStunkTA= false;
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


}
////////////////
LPDIRECTDRAWSURFACE UnicodeSupport::CreateSurfaceFromContextScreen (Context_Screen * Context_Screen_Ptr, LPDIRECTDRAW lpDDraw, LPDIRECTDRAWSURFACE * lplpDDSurface)
{
	*lplpDDSurface= NULL;

	DDSURFACEDESC DDSurfaceDesc_temp;
	DDRAW_INIT_STRUCT( DDSurfaceDesc_temp);
	DDSurfaceDesc_temp.dwFlags|= DDSD_HEIGHT;
	DDSurfaceDesc_temp.dwFlags|= DDSD_WIDTH;
	DDSurfaceDesc_temp.dwFlags|= DDSD_CAPS;
	//	DDSurfaceDesc_temp.dwFlags|= DDSD_PITCH;
	//	DDSurfaceDesc_temp.dwFlags|= DDSD_LPSURFACE;

	DDSurfaceDesc_temp.dwHeight= Context_Screen_Ptr->Height;
	DDSurfaceDesc_temp.dwWidth= Context_Screen_Ptr->Width;
 	DDSurfaceDesc_temp.ddsCaps.dwCaps= DDSCAPS_OFFSCREENPLAIN| DDSCAPS_SYSTEMMEMORY;

	//	DDSurfaceDesc_temp.lPitch= (Context_Screen_Ptr->PixelPerLine);//一个字节代表8个bits
	//	DDSurfaceDesc_temp.lpSurface= Context_Screen_Ptr->ScreenColorBits_Ptr;

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
PSpecScreenSurface UnicodeSupport::NewSpecScreenSurface (Context_Screen * Context_Screen_Ptr)
{
	IDDrawSurface::OutptTxt ( "NewSpecScreenSurface!");
	PSpecScreenSurface temp_PSSS= static_cast <PSpecScreenSurface> (malloc ( sizeof(SpecScreenSurface)+ 1));
	memset ( temp_PSSS, 0, sizeof(SpecScreenSurface));

	LPDIRECTDRAW TADDraw_lp= (LPDIRECTDRAW)LocalShare->TADirectDraw;
	CreateSurfaceFromContextScreen ( Context_Screen_Ptr, TADDraw_lp, &(temp_PSSS->DDSurface_P));
	if (NULL==temp_PSSS->DDSurface_P)
	{
		free ( temp_PSSS);
		return NULL;
	}
	//GetDC ( );
	DDRAW_INIT_STRUCT( temp_PSSS->DDSurfaceDescForLock);

	temp_PSSS->Width= Context_Screen_Ptr->Width;
	temp_PSSS->Height=  Context_Screen_Ptr->Height;
	temp_PSSS->PixelPerLine=  Context_Screen_Ptr->PixelPerLine;
	temp_PSSS->SurfaceMemBytesLen= (temp_PSSS->Height- 1)* (temp_PSSS->Width);
	temp_PSSS->myDDraw_Ptr= TADDraw_lp;

	return temp_PSSS;
}

void UnicodeSupport::FreeSpecScreenSurface (PSpecScreenSurface ForFree_PSSS)
{	
	__try	
	{
	//
		if (NULL!=ForFree_PSSS)
		{
			if (NULL!=(ForFree_PSSS->DDSurface_P))
			{
				if (TADDraw_lp==ForFree_PSSS->myDDraw_Ptr)
				{//* DDraw的释放，会把这些surface都摧毁 *//
					if (NULL!=(ForFree_PSSS->mySurface_HDC))
					{
						(ForFree_PSSS->DDSurface_P)->ReleaseDC ( ForFree_PSSS->mySurface_HDC);
					}
					(ForFree_PSSS->DDSurface_P)->Release ( );
				}
			}
			free ( ForFree_PSSS);//* 结构的释放，可能会出错  ???? *//
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
}

PSpecScreenSurface UnicodeSupport::GetSpecScreenSurface (Context_Screen * Context_Screen_Ptr)
{
	PSpecScreenSurface temp_PSSS= UnicodeFontDrawCache;
	PSpecScreenSurface Last_PSSS= temp_PSSS;
	for (; NULL!=temp_PSSS; Last_PSSS= temp_PSSS, temp_PSSS= temp_PSSS->Next)
	{
		if ((static_cast<int>(temp_PSSS->PixelPerLine)>=(Context_Screen_Ptr->PixelPerLine))&&(static_cast<int>(temp_PSSS->Height)>=(Context_Screen_Ptr->Height)))
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
	temp_PSSS= NewSpecScreenSurface ( Context_Screen_Ptr);
	temp_PSSS->Next= UnicodeFontDrawCache;
	UnicodeFontDrawCache= temp_PSSS;
	return temp_PSSS;
}

void CopyToContextScreenMem (Context_Screen * Context_Screen_Ptr, PSpecScreenSurface SrcSpecScreenSurface)
{
	//
	int DescPixelPerLine= Context_Screen_Ptr->PixelPerLine;
	int SrcPixelPerLine= SrcSpecScreenSurface->PixelPerLine;
	LPBYTE Desc= static_cast<LPBYTE>(Context_Screen_Ptr->ScreenColorBits_Ptr);
	LPBYTE Src= static_cast<LPBYTE>(SrcSpecScreenSurface->SurfaceMem_Pvoid);
	for (int i= 0; i<(Context_Screen_Ptr->Height- 2); ++i)
	{
		memcpy_s ( Desc+ (i* DescPixelPerLine), DescPixelPerLine, Src+ (i* SrcPixelPerLine),  DescPixelPerLine);
	}
}

void CopyToSpecSurfaceMem (PSpecScreenSurface SrcSpecScreenSurface, Context_Screen * Context_Screen_Ptr)
{
	//
	int SrcPixelPerLine= Context_Screen_Ptr->PixelPerLine;
	int DescPixelPerLine= SrcSpecScreenSurface->PixelPerLine;
	LPBYTE Src= static_cast<LPBYTE>(Context_Screen_Ptr->ScreenColorBits_Ptr);
	LPBYTE Desc= static_cast<LPBYTE>(SrcSpecScreenSurface->SurfaceMem_Pvoid);
	for (int i= 0; i<(Context_Screen_Ptr->Height- 2); ++i)
	{
		memcpy_s ( Desc+ (i* DescPixelPerLine), DescPixelPerLine, Src+ (i* SrcPixelPerLine),  DescPixelPerLine);
	}
}
////////////////
//DrawText
///////////////////////////
BOOL UnicodeSupport::UnicodeDrawTextA (Context_Screen * Context_Screen_Ptr, char * Str_cstrp, LPDIRECTDRAW TADDrawArgc_lp, int X_Off, int Y_Off)
{
	__try
	{
		TADDraw_lp= TADDrawArgc_lp;
		RECT TextRect;
		TextRect.top= X_Off;
		TextRect.left= Y_Off;
		TextRect.right= Context_Screen_Ptr->ScreenRect.right;
		TextRect.bottom= Context_Screen_Ptr->ScreenRect.bottom;

		int ContextScreenMemBytesLen= (Context_Screen_Ptr->Height- 1)* (Context_Screen_Ptr->PixelPerLine);
// 		IDDrawSurface::OutptTxt ( Str_cstrp);
// 		IDDrawSurface::OutptTxt ( "x:");
// 		IDDrawSurface::OutptInt ( X_Off);
// 		IDDrawSurface::OutptTxt ( "y:");
// 		IDDrawSurface::OutptInt ( Y_Off);

		PSpecScreenSurface temp_PSSS= GetSpecScreenSurface ( Context_Screen_Ptr);
		if (TRUE==UnicodeValid)
		{//only drawText when shall using unicode

			//*现在的实现，会让屏幕刷新速度慢到爆 *//
			// Copy Context Screen In
			if (DD_OK==temp_PSSS->DDSurface_P->Lock ( NULL, &(temp_PSSS->DDSurfaceDescForLock), DDLOCK_SURFACEMEMORYPTR, NULL))
			{
				temp_PSSS->SurfaceMem_Pvoid= temp_PSSS->DDSurfaceDescForLock.lpSurface;
				temp_PSSS->PixelPerLine= (temp_PSSS->DDSurfaceDescForLock.lPitch);
				////just copy all bytes in SurfaceMem;
				if (ContextScreenMemBytesLen==temp_PSSS->PixelPerLine)
				{
					memcpy_s ( temp_PSSS->SurfaceMem_Pvoid, ContextScreenMemBytesLen, Context_Screen_Ptr->ScreenColorBits_Ptr, ContextScreenMemBytesLen);
				}
				else
				{
					CopyToSpecSurfaceMem ( temp_PSSS, Context_Screen_Ptr);
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

// 				TEXTMETRIC WhiteFont_TMetric;
// 				memset ( &WhiteFont_TMetric, 0, sizeof(TEXTMETRIC));
// 
// 				SelectObject ( temp_PSSS->mySurface_HDC, UnicodeWhiteFont);
// 				
// 				SetTextColor ( temp_PSSS->mySurface_HDC, RGB(0x00, 0x00, 0x00));//白色
 				SetBkMode ( temp_PSSS->mySurface_HDC, TRANSPARENT);
// 
// 				GetTextMetrics ( temp_PSSS->mySurface_HDC, &WhiteFont_TMetric);
// 				int WhiteFontPixel= WhiteFont_TMetric.tmWeight* GetDeviceCaps ( temp_PSSS->mySurface_HDC, HORZSIZE)/ GetDeviceCaps ( temp_PSSS->mySurface_HDC, LOGPIXELSX);
// 				::DrawTextExA ( temp_PSSS->mySurface_HDC,  Str_cstrp, -1, &(TextRect), DT_NOCLIP, NULL);

				SelectObject ( temp_PSSS->mySurface_HDC, UnicodeYellowFont);
				SetTextColor ( temp_PSSS->mySurface_HDC, RGB(0xd0, 0xd0, 0xb3));///黄色 (?)

				::DrawTextExA ( temp_PSSS->mySurface_HDC,  Str_cstrp, -1, &(TextRect), DT_NOCLIP, NULL);

// 				int Str_Len= strlen(Str_cstrp);
// 
// 				for (int i= 0; i<Str_Len; ++i)
// 				{
// 					TextRect.left= TextRect.left+ i* WhiteFont_TMetric.tmMaxCharWidth;
// 					TextRect.right= TextRect.right+ i* WhiteFont_TMetric.tmMaxCharWidth;
// 					::DrawTextExA ( temp_PSSS->mySurface_HDC,  Str_cstrp+ i, 1, &(TextRect), DT_NOCLIP, NULL);
// 				}
				
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
				temp_PSSS->PixelPerLine= (temp_PSSS->DDSurfaceDescForLock.lPitch);
				if (ContextScreenMemBytesLen==temp_PSSS->PixelPerLine)
				{
					memcpy_s ( Context_Screen_Ptr->ScreenColorBits_Ptr, ContextScreenMemBytesLen, temp_PSSS->SurfaceMem_Pvoid, ContextScreenMemBytesLen);
				}
				else
				{
					CopyToContextScreenMem ( Context_Screen_Ptr, temp_PSSS);
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


int USDrawTextInScreen_HookRouter (PInlineX86StackBuffer X86StrackBuffer)
{
	__try
	{
		if(DataShare->ehaOff == 1)
		{
			__leave;
		}
		LPVOID RtnAddr= * ((LPVOID *)X86StrackBuffer->Esp);
		Context_Screen * Context_Screen_Ptr= * ((Context_Screen * *)(X86StrackBuffer->Esp+ 4));
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

		if (((UnicodeSupport *)(LocalShare->TAUnicodeSupport))->UnicodeDrawTextA ( Context_Screen_Ptr, Str, temp_PDDraw, X_Off, Y_Off))
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


bool UnicodeSupport::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg) 
	{ 
	case WM_IME_STARTCOMPOSITION:
		// *这个消息出现时，应该把屏幕stunk住，好让输入法的显示不被拦截。但还是需要交给DefWindowsProc处理 * //
		IDDrawSurface::OutptTxt ( "WM_IME_STARTCOMPOSITION");

// 		HWND TopWindow;
// 		TopWindow = FindWindowA ( "Total Annihilation Class", NULL);
// 
// 		if(TopWindow!=0)
// 		{
// 			IME_Clipper= NULL;
// 
// 			HIMC CurrentImc;
// 			COMPOSITIONFORM tempComPosionForm;
// 			ZeroMemory ( &tempComPosionForm, sizeof(tempComPosionForm));
// // 			CANDIDATEFORM tempCandidateForm;
// // 			ZeroMemory ( &tempCandidateForm, sizeof(tempCandidateForm));
// 			tempComPosionForm.dwStyle=	CFS_RECT;
// 			tempComPosionForm.rcArea.left= (LocalShare->ScreenWidth)/ 2+ 50;
// 			tempComPosionForm.rcArea.right= tempComPosionForm.rcArea.left+ 200;
// 			tempComPosionForm.rcArea.top= (LocalShare->ScreenHeight/ 3)* 2;
// 			tempComPosionForm.rcArea.bottom= tempComPosionForm.rcArea.top+ 200;
// 
// 			CurrentImc= ImmGetContext ( TopWindow);
// 			ImmSetCompositionWindow ( CurrentImc, &tempComPosionForm);
// 			//ImmGetCandidateWindow ( CurrentImc, sizeof(tempCandidateForm), &tempCandidateForm);
// 			TAProcessWhenExchange= -1;
// 
// 			if (DD_OK==((LPDIRECTDRAW)(LocalShare->TADirectDraw))->CreateClipper ( 0, &IME_Clipper,0))
// 			{
// 				RECT ImeRect;
// 				memcpy ( &ImeRect, &(tempComPosionForm.rcArea), sizeof(ImeRect));
// 				LPRGNDATA RgnData_Ptr;
// 				//GetWindowRect ( IME_Hwnd, &ImeRect);
// // 				if (CFS_EXCLUDE==tempCandidateForm.dwStyle)
// // 				{
// // 					memcpy ( &ImeRect, &(tempCandidateForm.rcArea), sizeof(ImeRect));
// // 				}
// // 				else if (CFS_CANDIDATEPOS==tempCandidateForm.dwStyle)
// // 				{
// // 					ImeRect.left= tempCandidateForm.ptCurrentPos.x;
// // 					ImeRect.top= tempCandidateForm.ptCurrentPos.y;
// // 					ImeRect.right= ImeRect.left+ 200;
// // 					ImeRect.bottom= ImeRect.top+ 200;
// // 				}
// // 				else
// // 				{//not good.
// // 					IME_Clipper->Release ( );
// // 					IME_Clipper= NULL;
// // 					IsStunkTA= true;
// // 					break;
// // 				}
// 				
// 				RgnData_Ptr= (LPRGNDATA) malloc ( sizeof(RGNDATAHEADER)+ sizeof(RECT)* 4);
// 				ZeroMemory ( RgnData_Ptr, sizeof(RGNDATAHEADER)+ sizeof(RECT)* 4);
// 				RgnData_Ptr->rdh.dwSize= sizeof(RGNDATAHEADER);
// 				RgnData_Ptr->rdh.iType= RDH_RECTANGLES;
// 				RgnData_Ptr->rdh.nCount= 4;
// 				RgnData_Ptr->rdh.nRgnSize= sizeof(RECT)* 4;
// 				RgnData_Ptr->rdh.rcBound.left= 0;
// 				RgnData_Ptr->rdh.rcBound.top= 0;
// 				RgnData_Ptr->rdh.rcBound.right= LocalShare->ScreenWidth;
// 				RgnData_Ptr->rdh.rcBound.bottom= LocalShare->ScreenHeight;
// 
// 				((LPRECT)(RgnData_Ptr->Buffer))[0].right= LocalShare->ScreenWidth;
// 				((LPRECT)(RgnData_Ptr->Buffer))[0].bottom= ImeRect.top;
// 
// 				((LPRECT)(RgnData_Ptr->Buffer))[1].top=  ImeRect.top;
// 				((LPRECT)(RgnData_Ptr->Buffer))[1].right= ImeRect.left;
// 				((LPRECT)(RgnData_Ptr->Buffer))[1].bottom= ImeRect.bottom;
// 
// 				((LPRECT)(RgnData_Ptr->Buffer))[2].top=  ImeRect.top;
// 				((LPRECT)(RgnData_Ptr->Buffer))[2].left= ImeRect.right;
// 				((LPRECT)(RgnData_Ptr->Buffer))[2].right= LocalShare->ScreenWidth;
// 				((LPRECT)(RgnData_Ptr->Buffer))[2].bottom= ImeRect.bottom;
// 
// 				((LPRECT)(RgnData_Ptr->Buffer))[3].top= ImeRect.bottom;
// 				((LPRECT)(RgnData_Ptr->Buffer))[3].right= LocalShare->ScreenWidth;
// 				((LPRECT)(RgnData_Ptr->Buffer))[3].bottom= LocalShare->ScreenHeight;
// 
// 				TAProcessWhenExchange= DataShare->TAProgress;
// 				
// 				((LPDIRECTDRAWSURFACE) (LocalShare->TADirectDrawFrontSurface))->GetClipper ( &FrontOrg_Clipper);
// 				((LPDIRECTDRAWSURFACE) (LocalShare->TADirectDrawBackSurface))->GetClipper ( &BackOrg_Clipper);
// 
// 				
// 				IME_Clipper->SetClipList ( RgnData_Ptr, 0);
// 				((LPDIRECTDRAWSURFACE) (LocalShare->TADirectDrawFrontSurface))->SetClipper ( IME_Clipper);
// 				((LPDIRECTDRAWSURFACE) (LocalShare->TADirectDrawBackSurface))->SetClipper ( IME_Clipper);
// 
// 				free ( RgnData_Ptr);
// 				break;
// 			}
// 		}
		//找不到IME窗口，把整个TA屏幕暂停住，以让IME窗口自由绘制。
		IsStunkTA= true;
		break;
	case WM_IME_ENDCOMPOSITION:
 		IDDrawSurface::OutptTxt ( "WM_IME_ENDCOMPOSITION");
// 		if (NULL!=IME_Clipper)
// 		{
// 			if (DataShare->TAProgress==TAProcessWhenExchange)
// 			{
// 				((LPDIRECTDRAWSURFACE) (LocalShare->TADirectDrawFrontSurface))->SetClipper ( FrontOrg_Clipper);
// 				((LPDIRECTDRAWSURFACE) (LocalShare->TADirectDrawBackSurface))->SetClipper ( BackOrg_Clipper);
// 			}
// 			IME_Clipper->Release ( );
// 			IME_Clipper= NULL;
// 			break;
// 		}

 		IsStunkTA= false;
		break; 
	}

	return false;
}

////////
// .text:004AB942 83 FB 20                                      cmp     ebx, 20h
// 	.text:004AB945 0F 8C 84 00 00 00                             jl      loc_4AB9CF
// 	.text:004AB94B 83 FB 7F                                      cmp     ebx, 7Fh
// 	.text:004AB94E 7F 7F                                         jg      short loc_4AB9CF
static bool DBSC_Flag_Global= false;
int myValidChar  (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(DataShare->ehaOff == 1)
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

int DBSC2rdByte (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(DataShare->ehaOff == 1)
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
int myDeleteChar (PInlineX86StackBuffer X86StrackBuffer)
{
	__try	
	{
		if(DataShare->ehaOff == 1)
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

#endif