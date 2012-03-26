#include "stdafx.h"
#include "tamem.h"
#include <vector>


template<class Type> \
	Type gcd (Type Num)
{
	Type NumTemp= 0- Num;
	if (NumTemp>Num)
	{
		return NumTemp;
	}
	else
	{
		return Num;
	}
}

typedef struct tag_PixelByteRegion
{
	LPBYTE Byte_P;
	DWORD ByteCount_ui;
	DWORD Width_ui;
	DWORD Height_ui;
	RECT ControlRect;
} PixelByteRegion, * PPixelByteRegion;
//DWORD ByteMask_Desc== 0xff, 0xffff, 0xffffff
typedef void __cdecl (* CopyPiexl_FuncPtr) (LPBYTE Current_Desc, DWORD ByteMask_Desc, LPBYTE Current_Src, DWORD ByteMask_Src, LPVOID Param);
LPBYTE ByteRegionZoomCopy (PPixelByteRegion Desc, PPixelByteRegion Src, CopyPiexl_FuncPtr CopyPixel, LPVOID Param_CopyPixel);


class WEAPONDrawer
{
private:
	TAdynmemStruct * TheTAMainStruct;
	MiniMapPicture * WorkingTAMap_P;
	RECT HotMapRect;
	HANDLE IsDrawing;

public:
	int HotWEAPONCounter;
	vector<POINT> WEAPONPosition_Vec;

private:
	ProjectileStruct * NextWEAPON (ProjectileStruct * Current);
	DWORD LoopAllWEAPON ();
	BOOL DrawWEAPON (ProjectileStruct * Current);

public:
	WEAPONDrawer ();
	WEAPONDrawer (TAdynmemStruct * TAMainStruct_p);
	~WEAPONDrawer ();

	int InsertProjectile (ProjectileStruct * In_PjtilePtr);

	DWORD DrawNow (LPBYTE TAMapBits, int BitsHeight, int BitsWidth);

	const LPRECT SetHotRect (LPRECT In_Rect);
	const LPRECT GetHotRect ( );
};

//参数，宽度，高度，TA格式的像素数组。
enum TAUnitType
{
	COMMANDER= 1,
	ENGINEER= 2,
	FACTORYS= 4,
	WEAPONUNITS= 8, 
	LANDUNITS= 16,
	SEAUNITS= 32,
	AIRUNITS= 64, 

	INVALIDTYPE= -1
};

class UnitsDrawer 
{
private:
    TAdynmemStruct * TheTAMainStruct;
	MiniMapPicture * WorkingTAMap_P;
	RECT HotMapRect;
	HANDLE IsDrawing;

	UnitPictures * PlayerPictureArray;

	//RECT WorkingTAMapRect;
public:
	int HotUnitCounter;
	vector<POINT> UnitPositionAry_Vec;

private:
	int InsertUnitAry (UnitStruct * In_UStructPtr);

	UnitStruct * NextUnit (UnitStruct * Current);
	DWORD LoopAllUnits ( );
	BOOL DrawTAUnit (UnitStruct * Current);
	BOOL DrawDefinedUnit(TAUnitType UnitType, int UnitXPos, int UnitYPos, int UnitSize);

public:
	UnitsDrawer (TAdynmemStruct * Argc_TAMainStruct);
	~UnitsDrawer ();
	DWORD DrawNow (LPBYTE TAMapBits, int BitsHeight, int BitsWidth);

	const LPRECT SetHotRect (LPRECT In_Rect);
	const LPRECT GetHotRect ( );
};




UnitsDrawer::UnitsDrawer (TAdynmemStruct * Argc_TAMainStruct)
{
	TheTAMainStruct= Argc_TAMainStruct;
	IsDrawing= CreateEvent ( NULL, FALSE, TRUE, NULL);
	SetEvent ( IsDrawing);

	HotUnitCounter= 0;
}
UnitsDrawer::~UnitsDrawer ()
{
	if (INVALID_HANDLE_VALUE!=IsDrawing)
	{
		CloseHandle (IsDrawing);
	}
}
DWORD UnitsDrawer::DrawAtNow (LPBYTE TAMapBits, int BitsHeight, int BitsWidth)
{
	if (WAIT_OBJECT_0!=WaitForSingleObject ( IsDrawing, 2000))
	{
		return 0;
	}
	ResetEvent ( IsDrawing);

	LoopAllUnits ( TheTAMainStruct, &DrawAUnitToMapBits);


	SetEvent ( IsDrawing);
}

DWORD UnitsDrawer::LoopAllUnits (void)
{
	for (int i= 0; i<TheTAMainStruct->TotalPlayers_CanBe; ++i)
	{
		//Argc_TAMainStruct->LOS_Sight_PlayerID_Game!=Argc_TAMainStruct->PlayerStructArray[i].
		UnitStruct * Current=TheTAMainStruct->PlayerStructArray[i].Units_Begin;
		UnitStruct * End= TheTAMainStruct->PlayerStructArray[i].Units_End;
		int PlayerUnitsNum= TheTAMainStruct->PlayerStructArray[i].UnitsNumber;
		for (int j= 0; (Current<=End)&&(j<PlayerUnitsNum); Current= &(Current[1]))
		{
			
			if (0x80000000u!=Current->UsedSpot||(0==Current->UnitTypeID))
			{
				continue;
			}
			
			InsertUnitAry ( Current);
			++j;
		}
	}
	return HotUnitCounter;
}

BOOL UnitsDrawer::DrawTAUnit (UnitStruct * Current)
{
	//
	int XPos= 0;
	int YPos= 0;
	TAUnitType UnitType= INVALIDTYPE;
	XPos= Current->XPos;
	YPos= Current->YPos;

	//UnitType判断
	UnitDefStruct * CurrentUnitType= Current->UnitType;

	if (NULL==Current->MoveClass)
	{
		UnitType= FACTORYS;
	}
	else if (0!=(COMMANDER_MASKBIT& CurrentUnitType->UnitTypeMask_1))
	{
		UnitType= COMMANDER;
	} 
	else if (0<CurrentUnitType->nWorkerTime)
	{
		UnitType= ENGINEER;
	}
	else if (0!=(WEAPONUNIT_MASKBIT& CurrentUnitType->UnitTypeMask_))
	{
		UnitType= WEAPONUNITS;
	}

	if (0!=(CANFLY_MASKBIT& (CurrentUnitType->UnitTypeMask_)))
	{
		UnitType|= AIRUNITS;
	}
	else if (CurrentUnitType->minwaterdepth> 0)//* only sea units need water to be deep. *//
	{
		UnitType|= SEAUNITS;
	}
	else
	{
		UnitType|= LANDUNITS;
	}
#define COMMHEALTH 3000
#define TECH2HEALTH 600
#define TECH3HEALTH 3000
#define KROGTHEALTH 25000
	int UnitSize= 1;

	if (KROGTHEALTH<Current->Health)
	{
		UnitSize= 4;/// KROG!!!
	}
	else if (TECH3HEALTH<Current->Health)
	{
		UnitSize= 3;
	}
	else if (TECH2HEALTH<Current->Health)
	{
		UnitSize= 2;
	}
	DrawDefinedUnit ( UnitType, Current->XPos, Current->YPos, UnitSize);
}

BOOL DrawDefinedUnit (TAUnitType UnitType, int UnitXPos, int UnitYPos, int UnitSize)
{//单位图标按照坐标绘制而超出地图矩形的区域，需要针对性不绘制。 超出的区域，绘制部分的单位图标。单位图标的放大和颜色，用两层数组的形式来定位到。 //
 //单位图标的大小和样子设计。doing //
 //
	WorkingTAMapBits= TAMapBits;
	unsigned int XPos;
	unsigned int YPos;
	//WorkingTAMapRect
	//Translate UnitPos To pixel Pos
	//图片 4体积,每个 10玩家颜色
	//*绘图，从i绘到j，窗口低出时，设置i为在矩形内的第一坐标。高出时，设置j *//


}

class UnitPictures 
{
	//
private:
	const LPDWORD myPlayerColor_Ary; 
	PlayerUnitPicture * myPictures_AryPtr;
	const int myPlayerCounter_I;
public:
	UnitPictures (LPSTR PictureFileNmae_cstrp, int Width_I, int Height_I
		LPDWORD PlayerColor_PtrAry, int PlayerCounter_I);
	~UnitPictures();

	LPBYTE operator [] (int index);
	const LPBYTE operator [] (int index);

private:
	int LoadPictureFile (PlayerUnitPicture * Pictures_AryPtr, LPSTR PictureFileName_cstrp, int Width_I, int Height_I, 
		LPDWORD PlayerColor_PtrAry, int PlayerCounter_I);
};

UnitPictures::UnitPictures (LPSTR PictureFileNmae_cstrp, int Width_I, int Height_I
	LPDWORD PlayerColor_PtrAry, int PlayerCounter_I)
{
	myPlayerCounter_I= PlayerCounter_I;
	myPictures_AryPtr= malloc (sizeof(PlayerUnitPicture *)* myPlayerCounter_I);
	myPlayerColor_Ary= PlayerColor_PtrAry;
	//PlayerColor_Ary= {212, 80, 235, 108, 219, 208, 93, 130, 67};//
	LoadPictureFile ( PlayerPicture, PictureFileNmae_cstrp, Width_I, Height_I, myPlayerColor_Ary, myPlayerCounter_I);
}

int UnitPictures::LoadPictureFile (PlayerUnitPicture * Pictures_AryPtr, LPSTR PictureFileName_cstrp, int Width_I, int Height_I, 
	LPDWORD PlayerColor_PtrAry, int MaxPlayerCounter_I)
{
	if (NULL==Pictures_AryPtr||NULL==PictureFileName_cstrp||0==Width_I||0==Height_I||0==PlayerCounter_I)
	{
		return 0;
	}
	// NULL==PlayerColor_PtrAry,全部按默认颜色212

	//load picture from config file!!!
}

int UnitsDrawer::SyncProjectile (void)
{
	HotProjectileCounter= 0;
	if ((WorkingTAMapRect.left>=WorkingTAMapRect.right)||
		(WorkingTAMapRect.top>=WorkingTAMapRect.bottom)||
		(0==WorkingTAMapRect.right)||
		(0==WorkingTAMapRect.bottom))
	{
		return 0;
	}
	ProjectileStruct * Current_Projectile;
	for (int i= TAGameStruct_Ptr->NumProjectiles; i>=0; --i)
	{
		Current_Projectile= &(TAGameStruct_Ptr->Projectiles[i]);
		if ((WorkingTAMapRect.left<=Current_Projectile->XPos)&&(Current_Projectile->XPos<=WorkingTAMapRect.right))
		{
			if ((WorkingTAMapRect.top<=Current_Projectile->YPos)&&(Current_Projectile->YPos<=WorkingTAMapRect.bottom))
			{
				InsertProjectileAry ( Current_Projectile);
			}
		}
	}
	return HotProjectileCounter;
}

int UnitsDrawer::SyncUnit (void)
{
	HotUnitCounter= 0;
	if ((WorkingTAMapRect.left>=WorkingTAMapRect.right)||
		(WorkingTAMapRect.top>=WorkingTAMapRect.bottom)||
		(0==WorkingTAMapRect.right)||
		(0==WorkingTAMapRect.bottom))
	{
		return 0;
	}
	UnitStruct * Current_UStruct;

	for (int i= 0; i<10; ++i)
	{
		int Countter= 0;

		while (Countter<TAGameStruct_Ptr->Players[i].UnitsCounters)
		{
			Current_UStruct= TAGameStruct_Ptr->Players[i].Units[Countter];
			if ((WorkingTAMapRect.left<=Current_UStruct->XPos)&&(Current_UStruct->XPos<=WorkingTAMapRect.right))
			{
				if ((WorkingTAMapRect.top<=Current_UStruct->YPos)&&(Current_UStruct->YPos<=WorkingTAMapRect.bottom))
				{
					InsertUnitAry ( Current_UStruct);
				}
			}
		}
	}
	return HotUnitCounter;
}



const LPRECT UnitsDrawer::SetHotRect (LPRECT In_Rect)
{
	WorkingTAMapRect.bottom= In_Rect->bottom;
	WorkingTAMapRect.left= In_Rect->left;
	WorkingTAMapRect.right= In_Rect->right;
	WorkingTAMapRect.top= In_Rect->top;

	return &WorkingTAMapRect;
}

const LPRECT UnitsDrawer::GetHotRect ( )
{
	return &WorkingTAMapRect;
}


class PlayerUnitPicture
{
protected:
	LPBYTE PlayerPicture;
	DWORD NewColorIndex_Dw;
	DWORD OrginalColor_Dw;

public:
	PlayerUnitPicture (LPSTR PictureFileNmae_cstrp, int Width_I, int Height_I, DWORD PlayerColor_TAPlatte);
	~PlayerUnitPicture();

	CopyOut (LPBYTE Out_ColorBits, int Out_Width, int Out_Height, 
			int Out_XPos, int Out_YPos);
	CopyIn (const LPBYTE In_ColorBits, int In_Width, int In_Height);

private:
	int LoadPictureFile (LPSTR PictureFileNmae_cstrp, int Width_I, int Height_I, 
		DWORD PlayerColor_TAPlatte);
};

LPBYTE PlayerUnitPicture::LoadPictureFile (LPSTR PictureFileNmae_cstrp, int Width_I, int Height_I, 
	DWORD PlayerColor_TAPlatte)
{
	//这儿只认得 RGB 00, 00, 00的黑色
	//
	CFile Bitmap_CFile;
	Bitmap_CFile.
	HANDLE File_H= CreateFileA ( PictureFileNmae_cstrp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD FileSize_ui= GetFileSize ( File_H, NULL);
	LPBYTE BmpFileBuffer_Pbyte= static_cast<LPBYTE> (malloc ( FileSize_ui));

	NewColorIndex_Dw= PlayerColor_TAPlatte;

	ReadFile ( File_H, BmpFileBuffer_Pbyte, FileSize_ui, &FileSize_ui, NULL);

	//parse bmp
	LPBITMAPFILEHEADER BitmapFileHeader_Ptr= BmpFileBuffer_Pbyte;
	LPBITMAPINFOHEADER BitmapInfoHeader_Ptr= static_cast<LPBITMAPINFOHEADER>(&BitmapFileHeader_Ptr[1]);
	if ('BM'==BitmapFileHeader_Ptr->bfType)
	{
		if (0==BitmapInfoHeader_Ptr->biCompression)
		{
			//*最好不要有人用这种压缩的文件，我可不想再写给BMP解压缩的代码。*//
			int BlackColorIndex= 0x0;
			int PixelStep= 0x0;
			if (8==BitmapInfoHeader_Ptr->biBitCount)
			{//
				
				for (int End= BitmapInfoHeader_Ptr->biClrUsed* sizeof(RGBQUAD); End	>0; End= End- sizeof(RGBQUAD))
				{
					if (0xffffffu==*static_cast<DWORD *>(static_cast<DWORD>(&BitmapInfoHeader_Ptr[1])+ End))
					{
						BlackColorIndex= End/ sizeof(RGBQUAD);
					}
				}
				PixelStep= 0x1;
			}
			else if (16==BitmapInfoHeader_Ptr->biBitCount)
			{
				//带替换颜色的区域放大、缩小拷贝
				BlackColorIndex= 0xffff;
				PixelStep= 0x2;
			}
			else if (24==BitmapInfoHeader_Ptr->biBitCount)
			{
				BlackColorIndex= 0xffffffu;
				PixelStep= 0x3;
			}
			else if (32==BitmapInfoHeader_Ptr->biBitCount)
			{
				BlackColorIndex= 0xffffffu;
				PixelStep= 0x4;
			}
			//只复制黑色作为特定的玩家颜色，其他的不管//
			PixelByteRegion BmpSrc;
			PixelByteRegion MyBytePtrDesc;
			RegionZoomCopy ( &MyBytePtrDesc, &BmpSrc, CopyBlackAsMyColor, static_cast<LPVOID>(&this));

		}
	}
	//0- num. shr (sizeof(int)* 4), - 1

	free ( BmpFileBuffer_Pbyte);
	CloseHandle ( File_H);
	return NULL;
}

void __cdecl CopyBlackAsMyColor (LPBYTE Current_Desc, DWORD ByteMask_Desc, LPBYTE Current_Src, DWORD ByteMask_Src, LPVOID Param)
{//Param==新的玩家颜色
	//
	PlayerUnitPicture * Temp_PUP= static_cast<PlayerUnitPicture *>(Param);

	DWORD ByteOfSrc= *(static_cast<DWORD *>(Current_Src))& ByteMask_Src;
	DWORD ByteOfDesc= *(static_cast<DWORD *>(Current_Desc))& ByteMask_Desc;

	ByteOfDesc&= ! ByteMask_Desc;//

	if (ByteOfSrc==Temp_PUP->OrginalColor_Dw)
	{
		ByteOfDesc|= Temp_PUP->NewColorIndex_Dw;
	}
	else
	{
		ByteOfDesc|= 0;
	}
	*(static_cast<DWORD *>(Current_Desc))= ByteOfDesc;
}

LPBYTE ByteRegionZoomCopy (PPixelByteRegion Desc, PPixelByteRegion Src, LPVOID Param_CopyPixel)
{//return Desc->Byte_P
	if ((NULL==Desc)||(NULL==Src))
	{
		return MMPERROR_NOTVALIDBUF;
	}

	if (Desc->ByteCount_ui>4||Desc->ByteCount_ui<1||Src->ByteCount_ui>4||Src->ByteCount_ui<1)
	{
		return MMPERROR_NOTVALIDBYTECOUNT;
	}
	int Rtn_I= 0;
	RECT DescRect_Local;
	RECT SrcRect_Local;
	
	memcpy ( &DescRect_Local, Desc->ControlRect, sizeof(RECT));
	memcpy ( &SrcRect_Local, Src->ControlRect, sizeof(RECT));

	float XInterval_I;
	float YInterval_I;

	int Control_DescWidth_I;
	int Control_DescHeight_I;

	int Control_SrcWidth_I;
	int Control_SrcHeight_I;

	int GcdControl_DescWidth_I;
	int GcdControl_DescHeight_I;
	int SignOfControl_DescHeight_I;
	int SignOfControl_DescWidth_I;

	int GcdControl_SrcWidth_I;
	int GcdControl_SrcHeight_I;
	int SignOfControl_SrcHeight_I;
	int SignOfControl_SrcWidth_I;

	__try	
	{
		//*让src操作的矩形正常*//

		if ((SrcRect_Local.left==SrcRect_Local.right)||(SrcRect_Local.top==SrcRect_Local.bottom))
		{
			SrcRect_Local.left= 0;
			SrcRect_Local.top= 0;

			SrcRect_Local.right= Src->Width_ui;
			SrcRect_Local.bottom= Src->Height_ui;
			Rtn_I= MMPERROR_MODIFYRECT;
		}

		if ((DescRect_Local.left==DescRect_Local.right)||(DescRect_Local.top==DescRect_Local.bottom))
		{
			DescRect_Local.left= 0;
			DescRect_Local.top= 0;

			DescRect_Local.right= Desc->Width_ui;
			DescRect_Local.bottom= Desc->Height_ui;
			Rtn_I= MMPERROR_MODIFYRECT;
		}

		if ((SrcRect_Local->right)>(Src->Width_ui))
		{
			SrcRect_Local->right= Src->Width_ui;
			Rtn_I= MMPERROR_MODIFYRECT;
		}
		if ((SrcRect_Local->bottom)>(Src->Height))
		{
			SrcRect_Local->bottom=Src->Height;
			Rtn_I= MMPERROR_MODIFYRECT;
		}
		if (SrcRect_Local->left<0)
		{
			SrcRect_Local->left= 0;
			Rtn_I= MMPERROR_MODIFYRECT;
		}
		if (SrcRect_Local->top<0)
		{
			SrcRect_Local->top= 0;
			Rtn_I= MMPERROR_MODIFYRECT;
		}
		//*让desc操作的矩形正常*//
		if ((DescRect_Local->right)>(Desc->Width_ui))
		{
			DescRect_Local->right= Desc->Width_ui;
			Rtn_I= MMPERROR_MODIFYRECT;
		}
		if ((DescRect_Local->bottom)>(Desc->Height))
		{
			DescRect_Local->bottom=Desc->Height;
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

		Control_SrcWidth_I= SrcRect_Local.right- SrcRect_Local.left;
		Control_SrcHeight_I= SrcRect_Local.bottom- SrcRect_Local.top;

		Control_DescWidth_I= DescRect_Local.right- DescRect_Local.left;
		Control_DescHeight_I= DescRect_Local.bottom- DescRect_Local.top;


		GcdControl_DescWidth_I= gcd (Control_DescWidth_I);
		GcdControl_DescHeight_I= gcd( Control_DescHeight_I);
		SignOfControl_DescHeight_I= Control_DescHeight_I/ GcdControl_DescHeight_I;
		SignOfControl_DescWidth_I= Control_DescWidth_I/ GcdControl_DescWidth_I;
		GcdControl_SrcWidth_I= gcd (Control_SrcWidth_I);
		GcdControl_SrcHeight_I= gcd (Control_SrcHeight_I);
		SignOfControl_SrcHeight_I= Control_SrcHeight_I/ GcdControl_SrcHeight_I;
		SignOfControl_SrcWidth_I= Control_SrcWidth_I/ GcdControl_SrcWidth_I;

		if (GcdControl_SrcWidth_I>GcdControl_SrcHeight_I)
		{
			if (GcdControl_DescHeight_I>GcdControl_SrcHeight_I)
			{
				int HeightBorder_I= (GcdControl_DescHeight_I- GcdControl_SrcHeight_I)/ 2;
				Control_DescHeight_I=  SignOfControl_DescHeight_I* GcdControl_SrcHeight_I;//*取得正负号，再得到高度*//
				DescRect_Local.top+= SignOfControl_DescHeight_I* HeightBorder_I;
				DescRect_Local.bottom= DescRect_Local.top+ Control_DescHeight_I;
			}
			int NewDescWidth_I= GcdControl_SrcWidth_I* GcdControl_DescHeight_I/ GcdControl_SrcHeight_I;
			int WidthBorder_I= (NewDescWidth_I- GcdControl_DescWidth_I)/ 2;
			Control_DescWidth_I= SignOfControl_DescWidth_I* NewDescWidth_I;
			DescRect_Local.left+= SignOfControl_DescWidth_I* WidthBorder_I;
			DescRect_Local.right= DescRect_Local.left+ Control_DescWidth_I;
		}
		else if (GcdControl_SrcWidth_I<=GcdControl_SrcHeight_I)
		{
			if (GcdControl_DescWidth_I>GcdControl_SrcWidth_I)
			{
				int WidthBorder_I= (GcdControl_SrcWidth_I- GcdControl_SrcWidth_I)/ 2;
				Control_DescWidth_I= SignOfControl_DescWidth_I* GcdControl_SrcWidth_I;
				DescRect_Local.left+= SignOfControl_DescWidth_I* WidthBorder_I;
				DescRect_Local.right= DescRect_Local.left+ Control_DescWidth_I;
			}
			int NewDescHeight_I= GcdControl_SrcHeight_I* GcdControl_DescWidth_I/ GcdControl_SrcWidth_I;
			int HeightBorder_I= (NewDescHeight_I- GcdControl_DescHeight_I)/ 2;
			Control_DescHeight_I= SignOfControl_DescHeight_I* NewDescHeight_I;
			DescRect_Local.top+= SignOfControl_DescHeight_I* HeightBorder_I;
			DescRect_Local.bottom= DescRect_Local.top+ Control_DescHeight_I;
		}

		// now, the Rect is suitable to Src. so we can stretch Src to Desc.

		if (0==Control_DescWidth_I||0==Control_DescHeight_I||0==Control_SrcHeight_I||0==Control_DescWidth_I)
		{
			Rtn_I= MMPERROR_NOTVALIDDESCSIZE;
			return Rtn_I;
		}


		XInterval_I= static_cast<float>(GcdControl_SrcWidth_I)/ static_cast<float>(GcdControl_DescWidth_I);
		YInterval_I= static_cast<float>(GcdControl_SrcHeight_I)/ static_cast<float>(GcdControl_DescHeight_I);
	}
	__except ( EXCEPTION_EXECUTE_HANDLER)
	{
		Rtn_I= MMPERROR_NOTVALIDDESCSIZE;
		return Rtn_I;
	}

	////
	__try
	{
		int DescTop_Deley= DescRect_Local.top* Desc->Width_ui* Desc->ByteCount_ui;
		int DescLeft_Delay= DescRect_Local.left* Desc->ByteCount_ui;
		
		//正常情况 if ((SignOfControl_SrcHeight_I==SignOfControl_DescHeight_I)&&(SignOfControl_SrcWidth_I==SignOfControl_DescWidth_I))
		SignOfControl_DescHeight_I= 1;
		SignOfControl_DescWidth_I= 1;

		GcdControl_DescHeight_I= GcdControl_DescHeight_I* Desc->ByteCount_ui;
		GcdControl_DescWidth_I= GcdControl_DescWidth_I* Desc->ByteCount_ui;
		YInterval_I= YInterval_I* Src->ByteCount_ui;
		XInterval_I= XInterval_I* Src->ByteCount_ui;

		//*处理负数时候颠倒过来的情况  *//
		if (SignOfControl_SrcHeight_I!=SignOfControl_DescHeight_I)
		{
			DescTop_Deley= DescTop_Deley+ GcdControl_DescHeight_I* Desc->Width_ui;
			SignOfControl_DescHeight_I= -1;
			YInterval_I= YInterval_I* SignOfControl_DescHeight_I;
		}

		if (SignOfControl_SrcWidth_I!=SignOfControl_DescWidth_I)
		{//
			DescLeft_Delay= DescLeft_Delay+ GcdControl_DescWidth_I;
			SignOfControl_DescWidth_I= -1;
			XInterval_I= XInterval_I* SignOfControl_DescWidth_I;
		}

		
		if (NULL==CopyPixel)
		{
			switch (Desc->ByteCount_ui)
			{
				case 1:
					for (int YPos= 0; YPos<GcdControl_DescHeight_I; ++YPos)
					{	//Y
						int DescPixelYStart= YPos* Desc->Width_ui* SignOfControl_DescHeight_I* Desc->ByteCount_ui+ DescTop_Deley;
						int SrcPixelYStart= static_cast<int>(YPos* YInterval_I)* Src->Width_ui;

						for (int XPos= 0; XPos<GcdControl_DescWidth_I; ++XPos)
						{//X 
							Desc->Byte_P[DescPixelYStart+ (XPos* SignOfControl_DescWidth_I* Desc->ByteCount_ui+ DescLeft_Delay)]= 
								Src->Byte_P[SrcPixelYStart+ static_cast<int>(XPos* XInterval_I)];
						}

					}
					break;
				case 2:
					for (int YPos= 0; YPos<GcdControl_DescHeight_I; ++YPos)
					{	//Y
						int DescPixelYStart= YPos* Desc->Width_ui* SignOfControl_DescHeight_I* Desc->ByteCount_ui+ DescTop_Deley;
						int SrcPixelYStart= static_cast<int>(YPos* YInterval_I)* Src->Width_ui;

						for (int XPos= 0; XPos<GcdControl_DescWidth_I; ++XPos)
						{//X 
							*static_cast<WORD *>(&Desc->Byte_P[DescPixelYStart+ (XPos* SignOfControl_DescWidth_I* Desc->ByteCount_ui+ DescLeft_Delay)])= 
								*static_cast<WORD *>(&Src->Byte_P[SrcPixelYStart+ static_cast<int>(XPos* XInterval_I)]);
						}

					}
					break;
				case 3:
					for (int YPos= 0; YPos<GcdControl_DescHeight_I; ++YPos)
					{	//Y
						int DescPixelYStart= YPos* Desc->Width_ui* SignOfControl_DescHeight_I* Desc->ByteCount_ui+ DescTop_Deley;
						int SrcPixelYStart= static_cast<int>(YPos* YInterval_I)* Src->Width_ui;

						for (int XPos= 0; XPos<GcdControl_DescWidth_I; ++XPos)
						{//X 
							int Index_DescBytePtr= DescPixelYStart+ (XPos* SignOfControl_DescWidth_I* Desc->ByteCount_ui+ DescLeft_Delay);
							int Index_SrcBytePtr= SrcPixelYStart+ static_cast<int>(XPos* XInterval_I);
							*static_cast<WORD *>(&Desc->Byte_P[Index_DescBytePtr])= 
								*static_cast<WORD *>(&Src->Byte_P[Index_SrcBytePtr]);
							Desc->Byte_P[Index_DescBytePtr+ 2]= Src->Byte_P[Index_SrcBytePtr+ 2];
						}

					}
					break;
				case 4:
					for (int YPos= 0; YPos<GcdControl_DescHeight_I; ++YPos)
					{	//Y
						int DescPixelYStart= YPos* Desc->Width_ui* SignOfControl_DescHeight_I* Desc->ByteCount_ui+ DescTop_Deley;
						int SrcPixelYStart= static_cast<int>(YPos* YInterval_I)* Src->Width_ui;

						for (int XPos= 0; XPos<GcdControl_DescWidth_I; ++XPos)
						{//X 
							*static_cast<DWORD *>(&Desc->Byte_P[DescPixelYStart+ (XPos* SignOfControl_DescWidth_I* Desc->ByteCount_ui+ DescLeft_Delay)])= 
								*static_cast<DWORD *>(&Src->Byte_P[SrcPixelYStart+ static_cast<int>(XPos* XInterval_I)]);
						}
					}
					break;
			}
		}
		else
		{
			DWORD SrcMask_dw= 0;
			DWORD DescMask_dw= 0;
			for (int i= 0; i<Desc->ByteCount_ui; ++i)
			{
				DescMask_dw= DescMask_dw<< 8;
				DescMask_dw|= 0xff;
			}
			for (int i= 0; i<Src->ByteCount_ui; ++i)
			{
				SrcMask_dw= SrcMask_dw<< 8;
				SrcMask_dw|= 0xff;
			}
			for (int YPos= 0; YPos<GcdControl_DescHeight_I; ++YPos)
			{	//Y
				int DescPixelYStart= YPos* Desc->Width_ui* SignOfControl_DescHeight_I* Desc->ByteCount_ui+ DescTop_Deley;
				int SrcPixelYStart= static_cast<int>(YPos* YInterval_I)* Src->Width_ui;

				for (int XPos= 0; XPos<GcdControl_DescWidth_I; ++XPos)
				{//X 
					CopyPixel ( &(Desc->Byte_P[DescPixelYStart+ (XPos* SignOfControl_DescWidth_I* Desc->ByteCount_ui+ DescLeft_Delay)]), DescMask_dw, 
						&(Src->Byte_P[SrcPixelYStart+ static_cast<int>(XPos* XInterval_I)]), SrcMask_dw, 
						Param_CopyPixel);

				}

			}
		}
	}
	__except ( EXCEPTION_EXECUTE_HANDLER)
	{
		Rtn_I= MMPERROR_NOTVALIDBUF;
	}
	//SaveToLagBmp ( Height, Width, MiniMapPixelBits);
	return Rtn_I;
}


