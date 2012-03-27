
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "TA 试验品.h"
#include "ChildView.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "mapParse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	MyView_CBitmapPTR= NULL;
	ColorsBit= NULL;
	myBitmap_H= NULL;
	
}

CChildView::~CChildView()
{
	if (NULL!=MyView_CBitmapPTR)
	{
		MyView_CBitmapPTR->DeleteObject();
		delete MyView_CBitmapPTR;
	}

	if (NULL!=ColorsBit)
	{
		delete ColorsBit;
		ColorsBit= NULL;
	}

}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE( )
END_MESSAGE_MAP()


// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
	
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);
	CMainFrame * myMainFrame= STATIC_DOWNCAST(CMainFrame, (static_cast<CCreateContext *>(cs.lpCreateParams))->m_pCurrentFrame);
	//myTNTFile_CFile= new CFile (myMainFrame->myTNTFile_Cstr.GetBuffer(),
	//	 CFile::readOnly);
	//从myTNTFile_CFile初始化出需要的TNT数据来。
	myTNTMiniMap_PTNTMM= new TNTtoMiniMap ( );
	TNTFileName_CstrA.Empty ( );
	TNTFileName_CstrA.Format ( "%ls", myMainFrame->myTNTFile_Cstr);
 	myTNTMiniMap_PTNTMM->MapFromTNTFileA ( TNTFileName_CstrA.GetBuffer());
// 
 	TNTFileName_CstrA.ReleaseBuffer();
// 	
	LPLOGPALETTE BufForPalette= reinterpret_cast<LPLOGPALETTE>(new BYTE [sizeof(LOGPALETTE)+ sizeof(RGBQUAD)* 255]);
	memcpy ( BufForPalette, myTNTMiniMap_PTNTMM->TALogPalette_Ptr, sizeof(LOGPALETTE)+ sizeof(RGBQUAD)* 255);
	for (int i= 0; i<256; ++i)
	{
		BufForPalette->palPalEntry[i].peFlags= PC_NOCOLLAPSE;
	}
	TAPalette_CPal.CreatePalette ( BufForPalette);


	delete BufForPalette;
	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	//dc.SelectPalette ( );
	//we draw mini-map  demo in here

	if (NULL!=MyView_CBitmapPTR->GetSafeHandle())
	{
		
	CDC BitmapDC;
	BitmapDC.CreateCompatibleDC ( &dc);
	HGDIOBJ temp_HGDIOBJ= BitmapDC.SelectObject ( myBitmap_H);

	dc.StretchBlt ( 0, 0, dc.GetWindowExt().cx, dc.GetWindowExt().cy, 
	&BitmapDC, 0, 0, dc.GetWindowExt().cx, dc.GetWindowExt().cy,  SRCCOPY);
	BitmapDC.DeleteDC ( );

	}

 	dc.TextOutW ( 0, 0, L"在TNT目录看看");
}
BOOL SaveCBitmap2DIB_TA (LPSTR FileName_tstrp, CBitmap * In_Cbitmap, LPLOGPALETTE LogPalette_Ptr);
void CChildView::OnSize(UINT nType, int cx, int cy)
{
	if (NULL!=MyView_CBitmapPTR)
	{
		MyView_CBitmapPTR->DeleteObject();
	}
	else
	{
		MyView_CBitmapPTR= new CBitmap;
	}
	if (NULL!=ColorsBit)
	{
		delete ColorsBit;
		ColorsBit= NULL;
	}
	if (0!=(cx% 4))
	{
		cx= (cx/ 4+ 1)* 4;
	}
	
	
	ColorsBit= new char[cx* cy];


	if (TRUE==MyView_CBitmapPTR->CreateBitmap ( cx, cy, 1, 8, ColorsBit))
	{
		BITMAP bitmap_Temp;
		MyView_CBitmapPTR->GetBitmap ( &bitmap_Temp);
		myTNTMiniMap_PTNTMM->DrawMiniMap ( static_cast<LPBYTE>(ColorsBit), bitmap_Temp.bmWidthBytes, bitmap_Temp.bmHeight);
		MyView_CBitmapPTR->SetBitmapBits (  cx* cy, ColorsBit);
		CStringA BitmapFileName_Cstring;
		BitmapFileName_Cstring= TNTFileName_CstrA+ ".bmp";
		SaveCBitmap2DIB_TA ( BitmapFileName_Cstring.GetBuffer ( ), MyView_CBitmapPTR, myTNTMiniMap_PTNTMM->TALogPalette_Ptr);
		BitmapFileName_Cstring.ReleaseBuffer ( );

		CString J8MGOURIDE ( BitmapFileName_Cstring);
		CFile BitmapFile ( J8MGOURIDE.GetBuffer(), CFile::modeRead);
		J8MGOURIDE.ReleaseBuffer();
		LPBYTE Temp_Buf= new BYTE[static_cast<UINT> (BitmapFile.GetLength())];
		BitmapFile.Read ( Temp_Buf, static_cast<UINT> (BitmapFile.GetLength()));

		if (NULL!=myBitmap_H)
		{
			::DeleteObject ( myBitmap_H);
			myBitmap_H= NULL;
		}
		CDC * ownDC= GetDC ( );
		ownDC->SelectPalette ( &TAPalette_CPal, FALSE);
		ownDC->RealizePalette ( );

		LPVOID  BufForDibSecion;
		myBitmap_H= ::CreateDIBSection ( ownDC->GetSafeHdc(),  reinterpret_cast<BITMAPINFO *>(Temp_Buf+ sizeof(BITMAPFILEHEADER)), DIB_RGB_COLORS, &BufForDibSecion, NULL, 0);
		memcpy ( BufForDibSecion, ColorsBit, cx* cy);
		
	}
	//delete ColorsBit;
}


BOOL SaveCBitmap2DIB_TA (LPSTR FileName_tstrp, CBitmap * In_Cbitmap, LPLOGPALETTE LogPalette_Ptr)
{
	if ((NULL==FileName_tstrp)||(NULL==In_Cbitmap))
	{
		return FALSE;
	}

	try
	{
		BITMAPFILEHEADER Header;
		BITMAPINFOHEADER InfoHeader;
		memset ( &Header, 0, sizeof(BITMAPFILEHEADER));
		memset ( &InfoHeader, 0, sizeof(BITMAPINFOHEADER));
		
		BITMAP temp_Bitmap;
		In_Cbitmap->GetBitmap ( &temp_Bitmap);
		//In_Cbitmap->
		InfoHeader.biHeight= 0- temp_Bitmap.bmHeight;//正立的bmp图像
		InfoHeader.biWidth= temp_Bitmap.bmWidth;
		InfoHeader.biPlanes= 1;
		InfoHeader.biBitCount= 32;
		InfoHeader.biSize= sizeof(BITMAPINFOHEADER);
		InfoHeader.biCompression= BI_RGB;

		InfoHeader.biXPelsPerMeter= temp_Bitmap.bmWidthBytes;
		InfoHeader.biYPelsPerMeter= temp_Bitmap.bmHeight;
		InfoHeader.biClrUsed= 256;
		InfoHeader.biClrImportant= 256;
		
		if (0==InfoHeader.biHeight||0==InfoHeader.biWidth)
		{
			return FALSE;
		}
		InfoHeader.biSizeImage= InfoHeader.biYPelsPerMeter* InfoHeader.biWidth* (InfoHeader.biBitCount/ 8);

		if (NULL!=LogPalette_Ptr)
		{
			InfoHeader.biBitCount= 8;
			Header.bfSize= sizeof(BITMAPFILEHEADER)+ InfoHeader.biSize+ sizeof(RGBQUAD)* 256+ InfoHeader.biSizeImage;
			Header.bfOffBits= sizeof(BITMAPFILEHEADER)+ InfoHeader.biSize+ sizeof(RGBQUAD)* 256;
		}
		else
		{
			Header.bfSize= sizeof(BITMAPFILEHEADER)+ InfoHeader.biSize+ InfoHeader.biSizeImage;
			Header.bfOffBits= sizeof(BITMAPFILEHEADER)+ InfoHeader.biSize;
		}
		
		Header.bfType= 'MB';

		//忘掉怎么让Cbitmap可以用调色板,直接用TA的。
		CString BullShit_Cstring;
		BullShit_Cstring.Format ( _T("%hs"), FileName_tstrp);

		CFile BitmapFile_Cfile ( BullShit_Cstring, CFile::modeRead| CFile::modeCreate| CFile::modeWrite);

		BitmapFile_Cfile.SeekToBegin();
		BitmapFile_Cfile.Write ( &Header, sizeof(BITMAPFILEHEADER));
		BitmapFile_Cfile.Write ( &InfoHeader, sizeof(BITMAPINFOHEADER));
		if (NULL!=LogPalette_Ptr)
		{
			BitmapFile_Cfile.Write ( LogPalette_Ptr->palPalEntry, sizeof(RGBQUAD)* 256);
		}

		int BitmapBitCount= InfoHeader.biSizeImage;
		LPVOID BitmapBit= malloc ( BitmapBitCount);
		if (NULL==BitmapBit)
		{
			return FALSE;
		}

		In_Cbitmap->GetBitmapBits ( BitmapBitCount, BitmapBit);
		BitmapFile_Cfile.Write ( BitmapBit, BitmapBitCount);
		free ( BitmapBit);
	}
	catch (CException* e)
	{
		e->ReportError();
		return FALSE;
	}
	return TRUE;
}
