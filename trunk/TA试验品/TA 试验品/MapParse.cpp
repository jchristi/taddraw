#include "stdafx.h"
//one pixel mean 1 byte, in here, in TA. 
//use TA tiles Pos in TNT class. use pixel Pos in minimap class.
#include "mapParse.h"

LPLOGPALETTE TNTtoMiniMap::TALogPalette_Ptr= NULL;
int TNTtoMiniMap::PaletteRefCount= 0;

extern RGBQUAD rqAry[256];
TNTtoMiniMap::TNTtoMiniMap ()
{
	//myMiniMap= new MiniMapPicture ( 2000, 1500);
	myMiniMap= new MiniMapPicture ( 3200, 1800);
	memset ( &TNTHeader, 0, sizeof(TNTHeaderStruct));
	MapPixelBytes_PB= NULL;
	
	PaletteRefCount= PaletteRefCount+ 1;
	if (NULL==TALogPalette_Ptr)
	{
		TALogPalette_Ptr= reinterpret_cast<LPLOGPALETTE>(new BYTE[sizeof(LOGPALETTE)+ sizeof(RGBQUAD)* 256]);
		TALogPalette_Ptr->palNumEntries= 256;
		TALogPalette_Ptr->palVersion= 0x300;
		memcpy ( &(TALogPalette_Ptr->palPalEntry), rqAry, sizeof(RGBQUAD)* 256);
	}
}

TNTtoMiniMap::TNTtoMiniMap (DWORD Width, DWORD Height)
{
	myMiniMap= new MiniMapPicture ( Width, Height);
	memset ( &TNTHeader, 0, sizeof(TNTHeaderStruct));
	MapPixelBytes_PB= NULL;

	PaletteRefCount= PaletteRefCount+ 1;
	if (NULL==TALogPalette_Ptr)
	{
		TALogPalette_Ptr= reinterpret_cast<LPLOGPALETTE>(new BYTE[sizeof(LOGPALETTE)+ sizeof(RGBQUAD)* 256]);
		TALogPalette_Ptr->palNumEntries= 256;
		TALogPalette_Ptr->palVersion= 0x300;
		memcpy ( &(TALogPalette_Ptr->palPalEntry), rqAry, sizeof(RGBQUAD)* 256);
	}
}

TNTtoMiniMap::~TNTtoMiniMap ()
{
	if (NULL!=myMiniMap)
	{
		delete myMiniMap;
	}
/*
	if (NULL!=my16_9MiniMap)
	{
		delete my16_9MiniMap;
	}*/
	

	if (NULL!=MapPixelBytes_PB)
	{
		delete MapPixelBytes_PB;
	}
	PaletteRefCount= PaletteRefCount- 1;
	if (0==PaletteRefCount)
	{
		if (NULL!=TALogPalette_Ptr)
		{
			delete TALogPalette_Ptr;
		}
	}
}

PTNTHeaderStruct TNTtoMiniMap::ParseMyTNTHeader (PTNTHeaderStruct In_PTNTH)
{
	if (NULL!=In_PTNTH)
	{
		memcpy_s ( &TNTHeader, sizeof(TNTHeaderStruct), In_PTNTH, sizeof(TNTHeaderStruct));
		TNTHeader.Height= TNTHeader.Height/ 2; //*这样意思才是行竖上的tiles数*//
		TNTHeader.Width= TNTHeader.Width/ 2;
		TNTHeader.PTRmapdata= (WORD *)((int)(TNTHeader.PTRmapdata)+ (int)In_PTNTH);
		TNTHeader.PTRmapattr= (WORD *)((int)(TNTHeader.PTRmapattr)+ (int)In_PTNTH);
		TNTHeader.PTRtilegfx= (LPBYTE)((int)(TNTHeader.PTRtilegfx)+ (int)In_PTNTH);
		TNTHeader.PTRtileanim= ((int)(TNTHeader.PTRtileanim)+ (int)In_PTNTH);
		TNTHeader.PTRminimap= (LPBYTE)((int)TNTHeader.PTRminimap+ (int)In_PTNTH);
	}
	return In_PTNTH;
}

void TNTtoMiniMap::MapFromTNTFileA (LPSTR TNTFileName)
{//?
	HANDLE File_H;
	DWORD FileLen_I;
	LPVOID Buf_BigMem;
	Buf_BigMem= NULL;
	__try
	{
		File_H= CreateFileA ( TNTFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE==File_H)
		{
			__leave;
		}
		FileLen_I= GetFileSize ( File_H, NULL);
		if (0==FileLen_I)
		{
			__leave;
		}
		Buf_BigMem=  malloc ( FileLen_I);

		ReadFile ( File_H, Buf_BigMem, FileLen_I, &FileLen_I, NULL);
		CloseHandle ( File_H);


		//
		MapFromTNTInMem ( Buf_BigMem);
		free ( Buf_BigMem);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
	return ;
}

void TNTtoMiniMap::MapFromTNTInMem (LPVOID Argc_PTNTH)
{
	ParseMyTNTHeader ( static_cast<PTNTHeaderStruct>(Argc_PTNTH));
	MapFromValidTNTHeader ( &TNTHeader);
}

void SaveToLagBmp (int a_Height, int b_Width,LPVOID MapPixelBytes_PB)
{
	BITMAPFILEHEADER Header;
	BITMAPINFOHEADER InfoHeader;
	memset ( &Header, 0, sizeof(BITMAPFILEHEADER));
	memset ( &InfoHeader, 0, sizeof(BITMAPINFOHEADER));
	//In_Cbitmap->
	InfoHeader.biHeight= 0- a_Height;
	InfoHeader.biWidth=  b_Width;
	InfoHeader.biPlanes= 1;
	InfoHeader.biSize= sizeof(BITMAPINFOHEADER);
	InfoHeader.biCompression= BI_RGB;

	InfoHeader.biXPelsPerMeter=  b_Width;
	InfoHeader.biYPelsPerMeter= a_Height;

	InfoHeader.biBitCount= 8;
	Header.bfSize= sizeof(BITMAPFILEHEADER)+ InfoHeader.biSize+ sizeof(RGBQUAD)* 256+ InfoHeader.biSizeImage;
	Header.bfOffBits= sizeof(BITMAPFILEHEADER)+ InfoHeader.biSize+ sizeof(RGBQUAD)* 256;

	InfoHeader.biSizeImage= InfoHeader.biYPelsPerMeter* InfoHeader.biWidth* (InfoHeader.biBitCount/ 8);
	Header.bfType= 'MB';

	//忘掉怎么让Cbitmap可以用调色板,直接用TA的。
	CString BullShit_Cstring( _T("c:\\temp.bmp"));

	CFile BitmapFile_Cfile ( BullShit_Cstring, CFile::modeRead| CFile::modeCreate| CFile::modeWrite);

	BitmapFile_Cfile.SeekToBegin();
	BitmapFile_Cfile.Write ( &Header, sizeof(BITMAPFILEHEADER));
	BitmapFile_Cfile.Write ( &InfoHeader, sizeof(BITMAPINFOHEADER));
	BitmapFile_Cfile.Write ( TNTtoMiniMap::TALogPalette_Ptr->palPalEntry, sizeof(RGBQUAD)* 256);
	int BitmapBitCount= InfoHeader.biSizeImage;
	BitmapFile_Cfile.Write ( MapPixelBytes_PB, BitmapBitCount);
}

void TNTtoMiniMap::MapFromValidTNTHeader (PTNTHeaderStruct Argc_PTNTH)
{
	if (Argc_PTNTH->Width<=0||Argc_PTNTH->Height<=0)
	{
		return;
	}
	RECT Whole_Rect;
	Whole_Rect.left= 0;
	Whole_Rect.top= 0;
	Whole_Rect.bottom= Argc_PTNTH->Height;
	Whole_Rect.right= Argc_PTNTH->Width;
//DrawRectMapToBuf ( (LPBYTE *)&MapPixelBytes_PB, Argc_PTNTH, &Whole_Rect);
	myMiniMap->StretchTATNTDataToMiniMap ( Argc_PTNTH);
// 	
// 	my16_9MiniMap->StretchTAMapToMiniMap ( (LPBYTE)MapPixelBytes_PB, &Whole_Rect);
// 	my4_3MiniMap->StretchTAMapToMiniMap ( (LPBYTE)MapPixelBytes_PB, &Whole_Rect);
}


int TNTtoMiniMap::DrawMiniMap (LPBYTE DescPixelBitsBegin, int Width_I, int Height_I)
{
/*
	float Divisor_F= static_cast<float>(Height_I);
	float Dividend_F= static_cast<float>(Width_I);
*/

/*
	MiniMapPicture * ;
	myMiniMap= my4_3MiniMap;
	if ((Dividend_F/ Divisor_F)>= (16.0f/ 9.0f))
	{
		myMiniMap= my16_9MiniMap;
	}*/
	RECT DescRect;
	DescRect.left= 0;
	DescRect.top= 0;
	DescRect.right= Width_I;
	DescRect.bottom= Height_I;

	return myMiniMap->DrawMiniMap ( DescPixelBitsBegin, &DescRect, Width_I, Height_I, NULL);
}

void inline TNTtoMiniMap::CopyTileToTAPos_Inline (LPBYTE PixelBitsBuf, POINT * TAPos, __int16 TileIndex, PTNTHeaderStruct ArgcTNTHeader)
{
	int PiexelPerLine_I= ArgcTNTHeader->Width* 32;
	LPBYTE begin_PixelBits= &PixelBitsBuf[(TAPos->y* 32)* PiexelPerLine_I+ TAPos->x* 32];
	LPBYTE begin_TilesPixelBits= &(ArgcTNTHeader->PTRtilegfx[ArgcTNTHeader->PTRmapdata[(TAPos->y)* ArgcTNTHeader->Width+ TAPos->x] * 32* 32]);
	//TAPos和Width都是按tiles作单位的，一个tiles的大小是32 width,32 height,所以得到了tiles的坐标后，还要*32* 32才能得到在实际TilesPixelBits中的位置。
	
	for (int yCount= 0; yCount<32; ++yCount)
	{
		memcpy (  begin_PixelBits+ yCount* PiexelPerLine_I, begin_TilesPixelBits+ yCount* 32, 32);
	}
}

LPBYTE TNTtoMiniMap::DrawRectMapToBuf (LPBYTE * RectPixelBitsBuf_PtrToPB, PTNTHeaderStruct Argc_PTNTH, RECT * TAPosRect)
{//这个是按照TNTHeader中的来绘制的，这个RectPixelBitsBeginBuf_PtrToPB会返回malloc来申请的对应于Rect大小的图片。
	LPBYTE Buf_PB= (LPBYTE)malloc (((TAPosRect->bottom- TAPosRect->top)* 32)* ((TAPosRect->right- TAPosRect->left)* 32)+ 1);

	int TAXPosEnd;
	int TAYPosEnd;
	TAXPosEnd= TAPosRect->right- TAPosRect->left;
	TAYPosEnd= TAPosRect->bottom- TAPosRect->top;

	int PiexelPerLine_I= (Argc_PTNTH->Width)* 32;
	LPBYTE begin_PixelBits;
	LPBYTE begin_TilesPixelBits;

	for (int TAYPos= 0; TAYPos<TAYPosEnd; ++TAYPos)
	{	//Y
		for (int TAXPos= 0; TAXPos<TAXPosEnd; ++TAXPos)
		{//X 
			int CurrentTAYPos= (TAPosRect->top+TAYPos);
			int CurrentTAXPos= (TAPosRect->left+ TAXPos);
			begin_PixelBits= &Buf_PB[CurrentTAYPos* 32* PiexelPerLine_I+ CurrentTAXPos* 32];
			int TileIndex_I= Argc_PTNTH->PTRmapdata[CurrentTAYPos* ((Argc_PTNTH->Width))+ CurrentTAXPos];
			if (0>TileIndex_I)
			{
				TileIndex_I= 0;
			}
			begin_TilesPixelBits= &(Argc_PTNTH->PTRtilegfx[TileIndex_I* 32* 32]);
			for (int yCount= 0; yCount<32; ++yCount)
			{//* tiles的width是32 *//
				memcpy (  begin_PixelBits+ yCount* PiexelPerLine_I, begin_TilesPixelBits+ yCount* 32, 32);
			}
		}
	}
	*RectPixelBitsBuf_PtrToPB= Buf_PB;
	//SaveToLagBmp ( TAYPosEnd* 32, TAXPosEnd* 32,   Buf_PB);

	return Buf_PB;
}

MiniMapPicture::MiniMapPicture (int Width_I, int Height_I)
{
	MiniMapPixelBits= NULL;
	
	WholeBytesInPixelsBits= Width_I* Height_I;
	MiniMapPixelBits= (LPBYTE)malloc ( WholeBytesInPixelsBits);
	
	this->Width= Width_I;
	this->Height= Height_I;
}
MiniMapPicture::~MiniMapPicture ()
{
	if (NULL!=MiniMapPixelBits)
	free ( MiniMapPixelBits);
}

LPBYTE MiniMapPicture::StretchTATNTDataToMiniMap (PTNTHeaderStruct TATNT_PTNTH)
{
	int MapDataWidth_I= TATNT_PTNTH->Width;
	int MapDataHeight_I= TATNT_PTNTH->Height;
	float XInterval_I;
	float YInterval_I;
	if (MapDataHeight_I<MapDataWidth_I)
	{
		Width= (MapDataWidth_I* Height/ MapDataHeight_I);
	}
	else if (MapDataHeight_I>MapDataWidth_I)
	{//

		Height= (MapDataHeight_I* Width/ MapDataWidth_I);
	}
	XInterval_I= static_cast<float>(MapDataWidth_I)* 32.0f;
	XInterval_I= XInterval_I/ Width;
	YInterval_I= static_cast<float>(MapDataHeight_I)* 32.0f;
	YInterval_I= YInterval_I/ Height;
	if (1>XInterval_I)
	{
		XInterval_I= 1;
	}
	if (1>YInterval_I)
	{
		YInterval_I= 1;
	}
	if ((DWORD)(Width* Height)>WholeBytesInPixelsBits)
	{
		free ( MiniMapPixelBits);
		MiniMapPixelBits= NULL;
		WholeBytesInPixelsBits= Width* Height;
		MiniMapPixelBits= (LPBYTE)malloc ( WholeBytesInPixelsBits);
	}
	
	if (NULL==MiniMapPixelBits)
	{
		return NULL;
	}

	for (int YPos= 0, YInTrue= 0; YPos<Height; YPos++, YInTrue= YPos* YInterval_I)
	{	//Y
		int MiniMapPixelYStart= YPos* Width;
		int MiniMapTileIndexYoffset= (YInTrue/ 32)* (MapDataWidth_I);
		int MiniMapTileYOffset= (YInTrue% 32)* 32;

		// what's the hell wrong.
		for (int XPos= 0, XInTrue= 0; XPos<Width; XPos++, XInTrue= XPos* XInterval_I)
		{//X 
			int TileIndex_I= TATNT_PTNTH->PTRmapdata[MiniMapTileIndexYoffset+ XInTrue/ 32];
			if (0>TileIndex_I)
			{
				TileIndex_I= 0;
			}
			LPBYTE begin_TilesPixelBits= &(TATNT_PTNTH->PTRtilegfx[TileIndex_I* 32* 32]);

			int MiniMapByte= begin_TilesPixelBits[MiniMapTileYOffset+ (XInTrue% 32)];
			//RectPixelBitsBuf_PB[PixelPerYPosMean_I+ static_cast<int>(XPos* XInterval_I)];
			MiniMapPixelBits[MiniMapPixelYStart+ XPos]= MiniMapByte;
		}
	}
	//	
//	SaveToLagBmp ( Height, Width, MiniMapPixelBits);
	return MiniMapPixelBits;
}

LPBYTE MiniMapPicture::StretchTAMapToMiniMap (LPBYTE RectPixelBitsBuf_PB, RECT * TAMapRect)
{
	int MapWidth_I= (TAMapRect->right- TAMapRect->left);
	int MapHeight_I= (TAMapRect->bottom- TAMapRect->top);
	float XInterval_I;
	float YInterval_I;
	if (MapHeight_I<MapWidth_I)
	{
		Width= MapWidth_I* Height/ MapHeight_I;
	}
	else if (MapHeight_I>MapWidth_I)
	{//

		Height= MapHeight_I* Width/ MapWidth_I;
	}
	XInterval_I= static_cast<float>(MapWidth_I)* 32.0f;
	XInterval_I= XInterval_I/ Width;
	YInterval_I= static_cast<float>(MapHeight_I)* 32.0f;
	YInterval_I= YInterval_I/ Height;
	if (1>XInterval_I)
	{
		XInterval_I= 1;
	}
	if (1>YInterval_I)
	{
		YInterval_I= 1;
	}
	if ((DWORD)(Width* Height)>WholeBytesInPixelsBits)
	{
		free ( MiniMapPixelBits);
		MiniMapPixelBits= NULL;
		WholeBytesInPixelsBits= Width* Height;
		MiniMapPixelBits= (LPBYTE)malloc ( WholeBytesInPixelsBits);
	}

	if (NULL==MiniMapPixelBits)
	{
		return NULL;
	}
	int MapPixelWidth_I=  MapWidth_I* 32;
	for (int YPos= 0; YPos<Height; YPos++)
	{	//Y
		int MiniMapPixelYStart= YPos* Width;
		int PixelPerYPosMean_I= static_cast<int>(YPos* YInterval_I)* MapPixelWidth_I;
		for (int XPos= 0; XPos<Width; XPos++)
		{//X 
			MiniMapPixelBits[MiniMapPixelYStart+ XPos]= RectPixelBitsBuf_PB[PixelPerYPosMean_I+ static_cast<int>(XPos* XInterval_I)];
		}
	}
//	
	return MiniMapPixelBits;
}

int MiniMapPicture::DrawMiniMap (LPBYTE DescPixelBitsBegin, RECT * DescRect, int DescPixelWidth_I, int DescPixelHeight_I,  RECT * MiniMapRect)
{	
	if ((NULL==DescPixelBitsBegin)||(NULL==MiniMapPixelBits))
	{
		return MMPERROR_NOTVALIDBUF;
	}

	int Rtn_I= 0;
	RECT FullMiniMap;
	RECT DescRect_Local;
	memcpy ( &DescRect_Local, DescRect, sizeof(RECT));

	if (NULL==MiniMapRect)
	{
		FullMiniMap.right= Width;
		FullMiniMap.bottom= Height;
		FullMiniMap.left= 0;
		FullMiniMap.top= 0;
		MiniMapRect= &FullMiniMap;
	}

	float XInterval_I;
	float YInterval_I;

	int DescWidth_I;
	int DescHeight_I;

	int SrcWidth_I;
	int SrcHeight_I;
	__try	
	{
		if (MiniMapRect->right>Width)
		{
			MiniMapRect->right= Width;
			Rtn_I= MMPERROR_MODIFYRECT;
		}
		if (MiniMapRect->bottom>Height)
		{
			MiniMapRect->bottom=Height;
			Rtn_I= MMPERROR_MODIFYRECT;
		}
		if (MiniMapRect->left<0)
		{
			MiniMapRect->left= 0;
			Rtn_I= MMPERROR_MODIFYRECT;
		}
		if (MiniMapRect->top<0)
		{
			MiniMapRect->top= 0;
			Rtn_I= MMPERROR_MODIFYRECT;
		}

		if (DescRect_Local.left<0)
		{
			DescRect_Local.left= 0;
			Rtn_I= MMPERROR_MODIFYDESCRECT;
		}
		if (DescRect_Local.top<0)
		{
			DescRect_Local.top= 0;
			Rtn_I= MMPERROR_MODIFYDESCRECT;
		}

		DescWidth_I= DescRect_Local.right- DescRect_Local.left;
		DescHeight_I= (DescRect_Local.bottom- DescRect_Local.top);

		SrcWidth_I= MiniMapRect->right- MiniMapRect->left;
		SrcHeight_I= MiniMapRect->bottom- MiniMapRect->top;
		if (DescWidth_I>DescHeight_I)
		{
			if (DescHeight_I>SrcHeight_I)
			{
				int HeightBorder_I= (DescHeight_I- SrcHeight_I)/ 2;
				DescHeight_I= SrcHeight_I;
				DescRect_Local.top+= HeightBorder_I;
				DescRect_Local.bottom= DescRect_Local.top+ DescHeight_I;
			}
			int NewDescWidth_I= SrcWidth_I* DescHeight_I/ SrcHeight_I;
			int WidthBorder_I= (DescWidth_I- NewDescWidth_I)/ 2;
			DescWidth_I= NewDescWidth_I;
			DescRect_Local.left+= WidthBorder_I;
			DescRect_Local.right= DescRect_Local.left+ DescWidth_I;
		}
		else if (DescWidth_I<DescHeight_I)
		{
			if (DescWidth_I>SrcWidth_I)
			{
				int WidthBorder_I= (DescWidth_I- SrcWidth_I)/ 2;
				DescWidth_I= SrcWidth_I;
				DescRect_Local.left+= WidthBorder_I;
				DescRect_Local.right= DescRect_Local.left+ DescWidth_I;
			}
			int NewDescHeight_I= SrcHeight_I* DescWidth_I/ SrcWidth_I;
			int HeightBorder_I= (NewDescHeight_I- DescHeight_I)/ 2;
			DescHeight_I= NewDescHeight_I;
			DescRect_Local.top+= HeightBorder_I;
			DescRect_Local.bottom= DescRect_Local.top+ DescHeight_I;
		}

		// now, the Rect is suitable to mini map. so we can stretch map.

		if (0==DescWidth_I||0==DescHeight_I)
		{
			return MMPERROR_NOTVALIDDESCSIZE;
		}


		XInterval_I= static_cast<float>(Width)/ static_cast<float>(DescWidth_I);
		YInterval_I= static_cast<float>(Height)/ static_cast<float>(DescHeight_I);
	}
	__except ( EXCEPTION_EXECUTE_HANDLER)
	{
		return MMPERROR_NOTVALIDDESCSIZE;
	}

	////
	__try
	{
		for (int YPos= 0; YPos<DescHeight_I; YPos++)
		{	//Y
			int DescPixelYStart= (YPos+ DescRect_Local.top)* (DescPixelWidth_I);
			int MiniMapPixelYStart= static_cast<int>(YPos* YInterval_I)* Width;
			for (int XPos= 0; XPos<DescWidth_I; XPos++)
			{//X 
				DescPixelBitsBegin[DescPixelYStart+ (XPos+ DescRect_Local.left)]= MiniMapPixelBits[MiniMapPixelYStart+ static_cast<int>(XPos* XInterval_I)];
			}
		}
	}
	__except ( EXCEPTION_EXECUTE_HANDLER)
	{
		return MMPERROR_NOTVALIDBUF;
	}
	//SaveToLagBmp ( Height, Width, MiniMapPixelBits);
	return Rtn_I;
}


RGBQUAD rqAry[256]=
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00
	, 0xC0, 0xDC, 0xC0, 0x00, 0xFC, 0x54, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	, 0xF3, 0xEB, 0xFF, 0x00, 0xD3, 0xC7, 0xEB, 0x00, 0xB3, 0xA3, 0xD7, 0x00, 0x97, 0x87, 0xC3, 0x00, 0x7F, 0x6F, 0xAF, 0x00, 0x63, 0x5B, 0x9B, 0x00, 0x4F, 0x47, 0x8B, 0x00, 0x47, 0x3B, 0x7B, 0x00
	, 0x3B, 0x33, 0x6F, 0x00, 0x33, 0x2B, 0x63, 0x00, 0x2B, 0x23, 0x57, 0x00, 0x27, 0x1B, 0x4B, 0x00, 0x1F, 0x17, 0x3B, 0x00, 0x17, 0x0F, 0x2F, 0x00, 0x0F, 0x0B, 0x23, 0x00, 0x0B, 0x07, 0x17, 0x00
	, 0xDF, 0xFF, 0x73, 0x00, 0xBF, 0xE7, 0x57, 0x00, 0x9F, 0xCF, 0x43, 0x00, 0x83, 0xB7, 0x2F, 0x00, 0x67, 0x9F, 0x1F, 0x00, 0x4F, 0x8B, 0x13, 0x00, 0x3F, 0x77, 0x0F, 0x00, 0x37, 0x6B, 0x0B, 0x00
	, 0x2F, 0x5F, 0x07, 0x00, 0x2B, 0x53, 0x07, 0x00, 0x27, 0x47, 0x00, 0x00, 0x23, 0x3F, 0x00, 0x00, 0x1B, 0x33, 0x00, 0x00, 0x17, 0x27, 0x00, 0x00, 0x0F, 0x1B, 0x00, 0x00, 0x0B, 0x13, 0x00, 0x00
	, 0xFF, 0xEF, 0xE3, 0x00, 0xE7, 0xDF, 0xC7, 0x00, 0xCB, 0xCF, 0xAF, 0x00, 0xA7, 0xB7, 0x93, 0x00, 0x83, 0x9F, 0x7F, 0x00, 0x67, 0x87, 0x6B, 0x00, 0x53, 0x6F, 0x5F, 0x00, 0x47, 0x63, 0x5F, 0x00
	, 0x3B, 0x57, 0x5B, 0x00, 0x33, 0x43, 0x53, 0x00, 0x2B, 0x3B, 0x47, 0x00, 0x23, 0x33, 0x3B, 0x00, 0x1B, 0x2B, 0x2F, 0x00, 0x13, 0x1F, 0x23, 0x00, 0x0F, 0x13, 0x17, 0x00, 0x07, 0x0B, 0x0B, 0x00
	, 0xD7, 0xFB, 0xFB, 0x00, 0xB7, 0xDF, 0xDF, 0x00, 0x9B, 0xC3, 0xC3, 0x00, 0x83, 0xAB, 0xAB, 0x00, 0x6F, 0x93, 0x93, 0x00, 0x57, 0x77, 0x77, 0x00, 0x43, 0x63, 0x63, 0x00, 0x33, 0x53, 0x53, 0x00
	, 0x23, 0x43, 0x43, 0x00, 0x17, 0x33, 0x33, 0x00, 0x0F, 0x23, 0x23, 0x00, 0x07, 0x1B, 0x1B, 0x00, 0x07, 0x17, 0x17, 0x00, 0x00, 0x13, 0x13, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x0B, 0x0B, 0x00
	, 0xFB, 0xFB, 0xFB, 0x00, 0xEB, 0xEB, 0xEB, 0x00, 0xDB, 0xDB, 0xDB, 0x00, 0xCB, 0xCB, 0xCB, 0x00, 0xBB, 0xBB, 0xBB, 0x00, 0xAB, 0xAB, 0xAB, 0x00, 0x9B, 0x9B, 0x9B, 0x00, 0x8B, 0x8B, 0x8B, 0x00
	, 0x7B, 0x7B, 0x7B, 0x00, 0x6B, 0x6B, 0x6B, 0x00, 0x5B, 0x5B, 0x5B, 0x00, 0x4B, 0x4B, 0x4B, 0x00, 0x3B, 0x3B, 0x3B, 0x00, 0x2B, 0x2B, 0x2B, 0x00, 0x1F, 0x1F, 0x1F, 0x00, 0x0F, 0x0F, 0x0F, 0x00
	, 0xFF, 0xF3, 0xEB, 0x00, 0xFF, 0xE3, 0xCB, 0x00, 0xFF, 0xCF, 0xAF, 0x00, 0xFF, 0xB3, 0x97, 0x00, 0xFF, 0x97, 0x7B, 0x00, 0xFF, 0x7F, 0x67, 0x00, 0xEF, 0x6B, 0x53, 0x00, 0xE3, 0x5B, 0x3F, 0x00
	, 0xD7, 0x4B, 0x33, 0x00, 0xCB, 0x3B, 0x23, 0x00, 0xAF, 0x2F, 0x17, 0x00, 0x97, 0x27, 0x0F, 0x00, 0x7B, 0x1F, 0x07, 0x00, 0x63, 0x17, 0x07, 0x00, 0x47, 0x0F, 0x00, 0x00, 0x2F, 0x0B, 0x00, 0x00
	, 0xFF, 0xF7, 0xE3, 0x00, 0xE7, 0xDB, 0xBF, 0x00, 0xCF, 0xBF, 0x9F, 0x00, 0xB7, 0xA7, 0x83, 0x00, 0xA3, 0x8F, 0x6B, 0x00, 0x8B, 0x77, 0x53, 0x00, 0x73, 0x5F, 0x3F, 0x00, 0x5F, 0x4B, 0x2F, 0x00
	, 0x57, 0x3F, 0x27, 0x00, 0x4F, 0x37, 0x23, 0x00, 0x47, 0x2F, 0x1F, 0x00, 0x3F, 0x27, 0x1B, 0x00, 0x37, 0x1F, 0x17, 0x00, 0x2F, 0x1B, 0x13, 0x00, 0x27, 0x13, 0x0F, 0x00, 0x1F, 0x0F, 0x0B, 0x00
	, 0xFF, 0xEF, 0xD7, 0x00, 0xEF, 0xE3, 0xBB, 0x00, 0xDF, 0xCB, 0x9B, 0x00, 0xCF, 0xB7, 0x83, 0x00, 0xC3, 0xA3, 0x6B, 0x00, 0xB3, 0x8F, 0x53, 0x00, 0xA3, 0x7B, 0x3F, 0x00, 0x97, 0x6B, 0x2F, 0x00
	, 0x87, 0x5B, 0x23, 0x00, 0x77, 0x4B, 0x1B, 0x00, 0x67, 0x3F, 0x13, 0x00, 0x57, 0x33, 0x0B, 0x00, 0x47, 0x27, 0x07, 0x00, 0x37, 0x1B, 0x00, 0x00, 0x27, 0x13, 0x00, 0x00, 0x1B, 0x0B, 0x00, 0x00
	, 0xFF, 0xE7, 0xFF, 0x00, 0xEB, 0xC7, 0xE7, 0x00, 0xD7, 0xAB, 0xD3, 0x00, 0xC3, 0x93, 0xBB, 0x00, 0xB3, 0x7B, 0xA7, 0x00, 0x9F, 0x63, 0x8F, 0x00, 0x8F, 0x4B, 0x77, 0x00, 0x7F, 0x3B, 0x63, 0x00
	, 0x6F, 0x2B, 0x4F, 0x00, 0x63, 0x1F, 0x43, 0x00, 0x57, 0x17, 0x37, 0x00, 0x47, 0x0F, 0x2B, 0x00, 0x3B, 0x07, 0x1F, 0x00, 0x2B, 0x00, 0x13, 0x00, 0x1F, 0x00, 0x0B, 0x00, 0x13, 0x00, 0x07, 0x00
	, 0xA7, 0xFF, 0xD7, 0x00, 0x7F, 0xE7, 0xAB, 0x00, 0x5B, 0xD3, 0x83, 0x00, 0x3F, 0xBF, 0x67, 0x00, 0x2B, 0xAB, 0x4B, 0x00, 0x2B, 0x97, 0x43, 0x00, 0x27, 0x87, 0x37, 0x00, 0x1B, 0x77, 0x2F, 0x00
	, 0x13, 0x67, 0x2B, 0x00, 0x0F, 0x5B, 0x23, 0x00, 0x0B, 0x4F, 0x1F, 0x00, 0x07, 0x43, 0x1B, 0x00, 0x00, 0x33, 0x17, 0x00, 0x00, 0x27, 0x0F, 0x00, 0x00, 0x1B, 0x0B, 0x00, 0x00, 0x0F, 0x07, 0x00
	, 0x9F, 0xE3, 0xFF, 0x00, 0x73, 0xC7, 0xE3, 0x00, 0x53, 0xAF, 0xCB, 0x00, 0x3F, 0x97, 0xB3, 0x00, 0x2F, 0x83, 0x9B, 0x00, 0x23, 0x6F, 0x83, 0x00, 0x17, 0x5B, 0x6B, 0x00, 0x0F, 0x47, 0x53, 0x00
	, 0x0B, 0x3B, 0x4B, 0x00, 0x07, 0x33, 0x43, 0x00, 0x07, 0x2B, 0x3B, 0x00, 0x00, 0x23, 0x37, 0x00, 0x00, 0x1B, 0x2F, 0x00, 0x00, 0x13, 0x27, 0x00, 0x00, 0x0F, 0x1F, 0x00, 0x00, 0x0B, 0x1B, 0x00
	, 0xA3, 0xFF, 0xFF, 0x00, 0x83, 0xF3, 0xFB, 0x00, 0x67, 0xE3, 0xF7, 0x00, 0x4F, 0xD3, 0xF3, 0x00, 0x33, 0xBB, 0xEF, 0x00, 0x1B, 0xA7, 0xEF, 0x00, 0x13, 0x8F, 0xEB, 0x00, 0x0F, 0x7B, 0xE7, 0x00
	, 0x07, 0x4F, 0xDF, 0x00, 0x00, 0x23, 0xD7, 0x00, 0x00, 0x1F, 0xBF, 0x00, 0x00, 0x1B, 0xA7, 0x00, 0x00, 0x17, 0x93, 0x00, 0x00, 0x13, 0x7B, 0x00, 0x00, 0x13, 0x63, 0x00, 0x00, 0x0F, 0x4F, 0x00
	, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xBF, 0xFF, 0x00, 0x00, 0x83, 0xFF, 0x00, 0x00, 0x47, 0xFF, 0x00, 0x00, 0x2B, 0xD3, 0x00, 0x00, 0x17, 0xAB, 0x00, 0x00, 0x07, 0x7F, 0x00, 0x00, 0x00, 0x57, 0x00
	, 0xFF, 0xCB, 0xDF, 0x00, 0xDF, 0x9F, 0xBB, 0x00, 0xBF, 0x77, 0x9B, 0x00, 0x9F, 0x57, 0x7F, 0x00, 0x7F, 0x3B, 0x67, 0x00, 0x5F, 0x23, 0x4B, 0x00, 0x3F, 0x13, 0x33, 0x00, 0x1F, 0x07, 0x1B, 0x00
	, 0xFF, 0xDB, 0xD3, 0x00, 0xF7, 0x9F, 0x87, 0x00, 0xEF, 0x6F, 0x43, 0x00, 0xE7, 0x47, 0x17, 0x00, 0xBB, 0x2B, 0x0B, 0x00, 0x8F, 0x17, 0x07, 0x00, 0x63, 0x07, 0x00, 0x00, 0x37, 0x00, 0x00, 0x00
	, 0x77, 0xFF, 0x7B, 0x00, 0x4F, 0xDF, 0x53, 0x00, 0x2B, 0xBF, 0x33, 0x00, 0x13, 0x9F, 0x1B, 0x00, 0x0B, 0x7F, 0x1B, 0x00, 0x07, 0x5F, 0x17, 0x00, 0x00, 0x3F, 0x13, 0x00, 0x00, 0x1F, 0x0B, 0x00
	, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFB, 0xFF, 0x00, 0xA4, 0xA0, 0xA0, 0x00
	, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00
};