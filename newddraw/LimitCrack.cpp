#include "iddraw.h"
#include "iddrawsurface.h"
#include <vector>
using namespace std;

#include "hook/etc.h"
#include "hook/hook.h"

#include "AiSeardchMapEntriesLimit.h"
#include "WeaponIDLimit.h"
#include "UnitTypeLimit.h"
#include "IncreaseCompositeSize.h"
#include "MixingBuffers.h"
#include "IncreaseSfxLimit.h"
#include "SwitchAlt.h"
#include "MultiGameWeaponID.h"
#include "UnicodeSupport.h"

#include "tamem.h"
#include "tafunctions.h"
#include "LimitCrack.h"

#include "TAConfig.h"

LimitCrack::LimitCrack ( void)
{
	NowIncreaseAISearchMapEntriesLimit= new IncreaseAISearchMapEntriesLimit ( MyConfig->GetIniInt ( "AISearchMapEntries", 66650));
	DWORD tmp_i= MyConfig->GetIniInt ( "WeaponType", 0);
	NowIncreaseWeaponTypeLimit= new IncreaseWeaponTypeLimit ( tmp_i);
	if ((tmp_i* 0x115)<=(*(WeaponAryLen[0])))
	{
		DataShare->WeaponIDLimit= tmp_i;
	}
	else
	{
		DataShare->WeaponIDLimit= 0;
	}

	NowIncreaseUnitTypeLimit= new IncreaseUnitTypeLimit ( MyConfig->GetIniInt ( "UnitType", 0) );
	NowIncreaseCompositeBuf= new IncreaseCompositeBuf ( MyConfig->GetIniInt ( "X_CompositeBuf", 0x500) , MyConfig->GetIniInt ( "Y_CompositeBuf", 0x500));
	//NowIncreaseMixingBuffers= new IncreaseMixingBuffers ( MyConfig->GetIniInt ( "MixingBuffers", 128));
	//NowSwitchalt= new Switchalt ( MyConfig->GetIniBool ( "SwitchAlt", TRUE));
	NowIncreaseSfxLimit= new IncreaseSfxLimit ( MyConfig->GetIniInt ( "SfxLimit", 0x5000));

	tmp_i= MyConfig->GetIniBool ( "MultiGameWeapon", FALSE);
	NowModifyWeaponPacket= new ModifyWeaponPacket ( tmp_i);

	if (TRUE==tmp_i)
	{
		DataShare->MultiWeaponID= 1;
	}
	else
	{
		DataShare->MultiWeaponID= 0;
	}

	//set the Reg things;
	LPVOID Data;
	LPCSTR Name_p;

	PEnumRegInfo RegInfo_Enum;
	int Type= MyConfig->EnumIniRegInfo_Begin ( &RegInfo_Enum, &Name_p, (LPCVOID *)&Data);

	while (NULL!=Name_p)
	{
		if (0==Type)
		{//DWORD
			MyConfig->WriteTAReg_Dword ( (LPSTR)Name_p, (DWORD)Data); 
		}
		else
		{//REG_SZ
			MyConfig->WriteTAReg_Str( (LPSTR)Name_p, (LPSTR)Data, strlen ( (LPSTR)Data)); 
		}
		Type= MyConfig->EnumIniRegInfo_Next ( &RegInfo_Enum, &Name_p, (LPCVOID *)&Data);
	}
	MyConfig->EnumIniRegInfo_End ( &RegInfo_Enum);

	DataShare->IniCRC= MyConfig->GetIniCrc ( );

	//
	char FontName[0x100];
	FontName[0]= 0;
	MyConfig->GetIniStr ( "UnicodeSupport", FontName, 0x100, NULL);
	NowSupportUnicode= new UnicodeSupport ( FontName);
}

LimitCrack::~LimitCrack ( void)
{
	delete NowIncreaseAISearchMapEntriesLimit;
	delete NowIncreaseWeaponTypeLimit;
	delete NowIncreaseUnitTypeLimit;
	delete NowIncreaseCompositeBuf;
	//delete NowIncreaseMixingBuffers;
	//delete NowSwitchalt;
	delete NowIncreaseSfxLimit;
	delete NowModifyWeaponPacket;
	
}

IncreaseAISearchMapEntriesLimit::IncreaseAISearchMapEntriesLimit ()
{
	//OrginalLimit= *AISearchMapEntriesLimit;
	IDDrawSurface::OutptTxt ("IncreaseAISearchMapEntriesLimit");
	DWORD Const5000= 0x5000;
	ModifyTheLimit= new SingleHook ( reinterpret_cast<LPBYTE>(AISearchMapEntriesLimit), 4, INLINE_UNPROTECTEVINMENT, reinterpret_cast<LPBYTE>(&Const5000));
};

IncreaseAISearchMapEntriesLimit::IncreaseAISearchMapEntriesLimit (DWORD NewLimit)
{
	//OrginalLimit= *AISearchMapEntriesLimit;
	IDDrawSurface::OutptTxt ("IncreaseAISearchMapEntriesLimit");
	ModifyTheLimit= new SingleHook ( reinterpret_cast<LPBYTE>(AISearchMapEntriesLimit), 4, INLINE_UNPROTECTEVINMENT, reinterpret_cast<LPBYTE>(&NewLimit));
}

IncreaseAISearchMapEntriesLimit::~IncreaseAISearchMapEntriesLimit ()
{
	delete ModifyTheLimit;
}


LPVOID * WeaponRelatedPtr[PTRARYNUM]=
{
	reinterpret_cast<LPVOID *>(0x42CDCE),
	reinterpret_cast<LPVOID *>(0x42E31E),
	reinterpret_cast<LPVOID *>(0x42E46A),
	reinterpret_cast<LPVOID *>(0x42F3AC),
	reinterpret_cast<LPVOID *>(0x0437CF9),

	reinterpret_cast<LPVOID *>(0x0437D15),
	reinterpret_cast<LPVOID *>(0x42ED48),
	reinterpret_cast<LPVOID *>(0x042ED70),
	reinterpret_cast<LPVOID *>(0x49E5CC),

	reinterpret_cast<LPVOID *>(0x042ECB2),
	reinterpret_cast<LPVOID *>(0x042F360),
	reinterpret_cast<LPVOID *>(0x42F378),
	reinterpret_cast<LPVOID *>(0x042ED48),
	reinterpret_cast<LPVOID *>(0x0455476)
};

LPDWORD WeaponAryLen [LENARYNUM]= 
{
	reinterpret_cast<LPDWORD>(0x42E33F),
	reinterpret_cast<LPDWORD>(0x42F433),
	reinterpret_cast<LPDWORD>(0x49E5ED)
};

DWORD IDlimitAddrAry[IDLIMITARYNUMBER]=
{
	0x0042E322,  
	0x0042EC99,
	0x0042ECa4,  
	0x0042ECB8,
	0x042ED54,

	0x0042ECE7,
	0x0042F344,
	0x0049DFA9,
	0x040954D,
	0x0409682,

	0x00409940,
	0x00487A1C,
	0x00487622,
	0x0049E0C2
};

BYTE IDlimit0_bits[]= {0x89, 0x94, 0x08, 0xAF, 0x2D, 0x00, 0x00};
BYTE IDlimit1_bits[]= {0x8B, 0x85, 0xBC, 0x00, 0x00, 0x00};
BYTE IDlimit3_bits[]= {0x85, 0xC0, 0x89, 0x44, 0x24, 0x10};
BYTE IDlimit4_bits[]= {0x90, 0x90,0x90,0x90,0x90};
BYTE IDlimit5_bits[]= {0x8B, 0x44, 0x24, 0x18, 0x8B, 0x4C, 0x24, 0x10, 0x40, 0x90, 0x3B, 0xC1, 0x89, 0x44, 0x24, 0x18};
BYTE IDlimit6_bits[]= {	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
BYTE IDlimit7_bits[]= {	0x8B, 0x9B, 0xBC, 0x00, 0x00, 0x00};
BYTE IDlimit8_bits[]= {	0x8B, 0x86, 0xbc, 0x00, 0x00, 0x00, 0x85, 0xC0};


BYTE IDlimit11_bits[]= {0x8B, 0x91, 0xBC, 0x00, 0x00, 0x00};
BYTE IDlimit12_bits[]= {0x8B, 0x4F, 0xEC, 0x83, 0xC0, 0x1C, 0x89, 0x8A, 0xBC, 0x00, 0x00, 0x00};
BYTE IDlimit13_bits[]= {0x8B, 0x99, 0xBC, 0x00, 0x00, 0x00, 0xC6, 0x46, 0xFF, 0x00, 0x85, 0xDB};

LPBYTE IDlimitBitsAry[IDLIMITARYNUMBER]=
{
	IDlimit0_bits,
	IDlimit1_bits,
	IDlimit3_bits, 
	IDlimit4_bits,
	IDlimit4_bits,

	IDlimit5_bits,
	IDlimit6_bits,
	IDlimit7_bits,
	IDlimit8_bits,
	IDlimit8_bits,

	IDlimit8_bits,
	IDlimit11_bits,
	IDlimit12_bits,
	IDlimit13_bits
};

DWORD IDlimitLenAry[IDLIMITARYNUMBER]=
{
	sizeof( IDlimit0_bits),
	sizeof( IDlimit1_bits),
	sizeof( IDlimit3_bits),
	sizeof( IDlimit4_bits),
	sizeof( IDlimit4_bits),

	sizeof( IDlimit5_bits),
	sizeof( IDlimit6_bits), 
	sizeof( IDlimit7_bits),
	sizeof( IDlimit8_bits),
	sizeof( IDlimit8_bits),

	sizeof( IDlimit8_bits),
	sizeof( IDlimit11_bits),
	sizeof( IDlimit12_bits),
	sizeof( IDlimit13_bits)
};

IncreaseWeaponTypeLimit::IncreaseWeaponTypeLimit ()
{
	NewLimit (0x22a000);
	//Hooked= TRUE;
}
IncreaseWeaponTypeLimit::IncreaseWeaponTypeLimit (DWORD NewLen)
{

	NewLimit ( NewLen* 0x115);
	//Hooked= TRUE;
}


void IncreaseWeaponTypeLimit::WeaponType_IdLimit (void)
{

	for (int i= 0; i<IDLIMITARYNUMBER; ++i)
	{
		IDlimitAry[i]= new SingleHook ( reinterpret_cast<LPBYTE>(IDlimitAddrAry[i]), IDlimitLenAry[i], INLINE_UNPROTECTEVINMENT, IDlimitBitsAry[i]);
	}

	//
	BYTE IDlimit2_bits[]= {0xC7, 0x44, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00};
	IDlimit2= new ModifyHook ( 0x0042EC9f, INLINE_MODIFYCODE, 0x5, IDlimit2_bits, sizeof( IDlimit2_bits), 0x5);


}

void IncreaseWeaponTypeLimit::NewLimit (DWORD NewLen)
{
	IDDrawSurface::OutptTxt ("IncreaseWeaponTypeLimit");
	OrgPtr= *(WeaponRelatedPtr[0]);
	OrglLen= *(WeaponAryLen[0]);

	CurtPtr= NULL;
	CurtLen= NewLen;


	for (int i= 0; i<PTRARYNUM; ++i)
	{
		PtrHookAry[i]= NULL;
	}
	WeaponPtr2= NULL;

	for (int i= 0; i<LENARYNUM; ++i)
	{
		LenHookAry[i]=NULL;
	}

	for (int i= 0; i<IDLIMITARYNUMBER; ++i)
	{
		IDlimitAry[i]= NULL;
	}

	IDlimit2= NULL;


	if (CurtLen<=OrglLen)
	{
		return ;
	}
	DWORD tempCurtLen= CurtLen+ reinterpret_cast<DWORD>((&(reinterpret_cast<TAdynmemStruct *>(0)->Weapons)))+ sizeof(WeaponStruct);
	LPVOID tempCurtPtr= malloc ( tempCurtLen);
	

	WriteWeaponAryPtr ( &CurtPtr);
	WriteWeaponAryLen ( CurtLen);

	WeaponType_IdLimit ();

	CurtPtr=tempCurtPtr;
}


IncreaseWeaponTypeLimit::~IncreaseWeaponTypeLimit ()
{
	IDDrawSurface::OutptTxt ("Release IncreaseWeaponTypeLimit");
	for (int i= 0; i<PTRARYNUM; ++i)
	{
		if (NULL!=PtrHookAry[i])
		{
			delete PtrHookAry[i];
		}
		
	}
	if (NULL!=WeaponPtr2)
	{
		delete WeaponPtr2;
	}

	for (int i= 0; i<LENARYNUM; ++i)
	{
		if (NULL!= LenHookAry[i])
		{
			delete LenHookAry[i];
		}
	}

	if (NULL!=IDlimit2)
	{
		delete IDlimit2;
	}
	for (int i= 0; i<IDLIMITARYNUMBER; ++i)
	{
		if (NULL!=IDlimitAry[i])
		{
			delete IDlimitAry[i];
		}
	}

}

LPVOID IncreaseWeaponTypeLimit::WriteWeaponAryPtr ( LPVOID * NewPtr)
{
	if (NULL!=CurtPtr)
	{
		return CurtPtr;
	}

	for (int i= 0; i<PTRARYNUM; ++i)
	{
		PtrHookAry[i]= new SingleHook ( reinterpret_cast<LPBYTE>(WeaponRelatedPtr[i]), 4, INLINE_UNPROTECTEVINMENT, reinterpret_cast<LPBYTE>( &NewPtr));// &NewPtr, be treat as a LPBYTE data;
	}
	
	BYTE WeaponPtr2Bits[]= {0x8B, 0x0D, 0xE8, 0x1D, 0x51, 0x00, 0x8B, 0x3D, 0xE8, 0x1D, 0x51, 0x00};

	*reinterpret_cast<DWORD *>(&WeaponPtr2Bits[8])= reinterpret_cast<DWORD>(NewPtr);

	WeaponPtr2= new ModifyHook ( reinterpret_cast<LPBYTE>(0x049D28B), INLINE_MODIFYCODE, 0x8, WeaponPtr2Bits, sizeof(WeaponPtr2Bits), 0x8);

	return CurtPtr;
}
DWORD IncreaseWeaponTypeLimit::WriteWeaponAryLen ( DWORD Newlen)
{
	if (NULL!=CurtPtr)
	{
		return CurtLen;
	}

	for (int i= 0; i<LENARYNUM; ++i)
	{
		LenHookAry[i]= new SingleHook ( reinterpret_cast<LPBYTE>(WeaponAryLen[i]), 4, INLINE_UNPROTECTEVINMENT, reinterpret_cast<LPBYTE>( &Newlen));
	}

	return CurtLen;
}


IncreaseUnitTypeLimit::IncreaseUnitTypeLimit ()
{

	WriteNewLimit ( 0x3000);
}

IncreaseUnitTypeLimit::IncreaseUnitTypeLimit ( int Number)
{	
	WriteNewLimit ( Number);
}
//
IncreaseUnitTypeLimit::~IncreaseUnitTypeLimit ()
{
	IDDrawSurface::OutptTxt ("Release IncreaseUnitTypeLimit");
	if (NULL!=Prologue_Weight)
	{
		delete Prologue_Weight;
	}
	if (NULL!=Argc_0_Weight)
	{
		delete Argc_0_Weight;
	}
	
	if (NULL!=Argc_1_Weight)
	{
		delete Argc_1_Weight;
	}
	if (NULL!=Epilogue_Weight)
	{
		delete Epilogue_Weight;
	}
	if (NULL!=Prologue_limit)
	{
		delete Prologue_limit;
	}
	if (NULL!=Argc_0_limit)
	{
		delete Argc_0_limit;
	}
	if (NULL!=Argc_1_limit)
	{
		delete Argc_1_limit;
	}
	if (NULL!=Argc_2_limit)
	{
		delete Argc_2_limit;
	}
	if (NULL!=Epilogue_limit)
	{
		delete Epilogue_limit;
	}
	if (NULL!=Push_FindSpot)
	{
		delete Push_FindSpot;
	}
	if (NULL!=Mov_FindSpot)
	{
		delete Mov_FindSpot;
	}
	

}

void IncreaseUnitTypeLimit::WriteNewLimit (DWORD Number)
{

	IDDrawSurface::OutptTxt ("WriteNewLimit");
	//weight func
	DWORD Orginal= 0x40;
	DWORD New;
	New= ((Number/ 8/ 0x40)+ 1)* 0x40;// 0x200 Number== 0x40 New

	Prologue_Weight= NULL;
	Argc_0_Weight= NULL;
	Argc_1_Weight= NULL;
	Epilogue_Weight= NULL;
	Prologue_limit= NULL;
	Argc_0_limit= NULL;
	Argc_1_limit= NULL;
	Argc_2_limit= NULL;
	Epilogue_limit= NULL;
	Push_FindSpot= NULL;
	Mov_FindSpot= NULL;

	if (Number<=Orginal)
	{
		return ;
	}
	DWORD RepsdEcx= New/ 4;
	
	BYTE Prologue_Weight_bits[]={0x81 ,0xEC ,0x00 ,0x01 ,0x00 ,0x00};
	//00406DB5   83EC 44          SUB ESP,44
	*(DWORD *)(&Prologue_Weight_bits[2])= New- 0x40+ 0x44;
	Prologue_Weight= new ModifyHook ( 0x0406DB5, INLINE_MODIFYCODE, 0x5, Prologue_Weight_bits, sizeof( Prologue_Weight_bits), 0x3);

	BYTE Argc_0_Weight_bits[]={0x8B ,0xB4 ,0x24 ,0x00 ,0x01 ,0x00 ,0x00};
	//00406DC9   8B7424 50        MOV ESI,DWORD PTR SS:[ESP+50]
	*(DWORD *)(&Argc_0_Weight_bits[3])= New- 0x40+ 0x50;
	Argc_0_Weight= new ModifyHook ( 0x406DC9, INLINE_MODIFYCODE, 0x6, Argc_0_Weight_bits, sizeof( Argc_0_Weight_bits), 0x4);
	BYTE Argc_1_Weight_bits[]={0xD9 ,0x9C ,0x24 ,0x00 ,0x01 ,0x00 ,0x00 ,0x8B ,0xAC ,0x24 ,0x00 ,0x01 ,0x00 ,0x00};
	//00406DFD   D95C24 58        FSTP DWORD PTR SS:[ESP+58]
	//00406E01   8B6C24 58        MOV EBP,DWORD PTR SS:[ESP+58]
	*(DWORD *)(&Argc_1_Weight_bits[3])= New- 0x40+ 0x58;
	*(DWORD *)(&Argc_1_Weight_bits[10])= New- 0x40+ 0x58;

	Argc_1_Weight= new ModifyHook ( 0x00406DFD, INLINE_MODIFYCODE, 0xd, Argc_1_Weight_bits, sizeof( Argc_1_Weight_bits), 0x8);
	BYTE Epilogue_Weight_bits[]={0x81 ,0xC4 ,0x00 ,0x01 ,0x00 ,0x00};
	//00406E3A   83C4 44          ADD ESP,44
	*(DWORD *)(&Epilogue_Weight_bits[2])= New- 0x40+ 0x44;
	Epilogue_Weight= new ModifyHook ( 0x0406E3A, INLINE_MODIFYCODE, 0x6, Epilogue_Weight_bits, sizeof( Epilogue_Weight_bits), 0x3);

	//limit func
	BYTE Prologue_limit_bits[]={0x81 ,0xEC ,0x00 ,0x01 ,0x00 ,0x00};
	//00406E45   83EC 40          SUB ESP,40
	*(DWORD *)(&Prologue_limit_bits[2])= New- 0x40+ 0x40;
	Prologue_limit= new ModifyHook ( 0x0406E45, INLINE_MODIFYCODE, 0x5, Prologue_limit_bits, sizeof( Prologue_limit_bits), 0x3);
	BYTE Argc_0_limit_bits[]={0x8B ,0xB4 ,0x24 ,0x00 ,0x01 ,0x00 ,0x00};
	//00406E5D   8B7424 4C        MOV ESI,DWORD PTR SS:[ESP+4C]
	*(DWORD *)(&Argc_0_limit_bits[3])= New- 0x40+ 0x4c;

	Argc_0_limit= new ModifyHook ( 0x00406E5D, INLINE_MODIFYCODE, 0x6, Argc_0_limit_bits, sizeof( Argc_0_limit_bits), 0x4);
	BYTE Argc_1_limit_bits[]={0x8D ,0x84 ,0x24 ,0x00 ,0x01 ,0x00 ,0x00};
	//00406E64   8D4424 50        LEA EAX,DWORD PTR SS:[ESP+50]
	*(DWORD *)(&Argc_1_limit_bits[3])= New- 0x40+ 0x50;

	Argc_1_limit= new ModifyHook ( 0x00406E64, INLINE_MODIFYCODE, 0x5, Argc_1_limit_bits, sizeof( Argc_1_limit_bits), 0x4);
	BYTE Argc_2_limit_bits[]= {0x8B ,0x8C ,0x24 ,0x00 ,0x01 ,0x00 ,0x00};
	//00406EB2   8B4C24 54        MOV ECX,DWORD PTR SS:[ESP+54]
	*(DWORD *)(&Argc_2_limit_bits[3])= New- 0x40+ 0x54;

	Argc_2_limit= new ModifyHook ( 0x00406EB2, INLINE_MODIFYCODE, 0x8, Argc_2_limit_bits, sizeof( Argc_2_limit_bits), 0x4);
	BYTE Epologue_limit_bits[]= {0x81 ,0xC4 ,0x00 ,0x01 ,0x00 ,0x00};//
	//00406ED6   83C4 40          ADD ESP,40
	*(DWORD *)(&Epologue_limit_bits[2])= New- 0x40+ 0x40;

	Epilogue_limit= new ModifyHook ( 0x00406ED6, INLINE_MODIFYCODE, 0x6, Epologue_limit_bits, sizeof( Epologue_limit_bits), 0x3);

	//FindSpot 
	BYTE Push_FindSpot_bits[]= {0x68 ,0x00 ,0x01 ,0x00 ,0x00};
	// 00488CC2 6A 40           push    40h   
	*(DWORD *)(&Push_FindSpot_bits[1])= New- 0x40+ 0x40;
	Push_FindSpot= new ModifyHook ( 0x488CC2, INLINE_MODIFYCODE, 0x7, Push_FindSpot_bits, sizeof( Push_FindSpot_bits), 0x2);

	Mov_FindSpot= new SingleHook ( 0x488CD3, 0x4, INLINE_UNPROTECTEVINMENT, (LPBYTE)(&RepsdEcx)); 
}


IncreaseCompositeBuf::IncreaseCompositeBuf ()
{
	WriteNewLimit ( 0x1000, 0x1000);
}

IncreaseCompositeBuf::IncreaseCompositeBuf (DWORD x, DWORD y)
{
	WriteNewLimit ( x, y);
}

IncreaseCompositeBuf::~IncreaseCompositeBuf ()
{
	IDDrawSurface::OutptTxt ("Release IncreaseCompositeBuf");
	delete CompositeBufSizeHook;
}

void IncreaseCompositeBuf::WriteNewLimit (DWORD x, DWORD y)
{
	IDDrawSurface::OutptTxt ("IncreaseCompositeBuf");
	CurtX= x;
	CurtY= y;
	BYTE NewBits[]= {0x68,0x58,0x02,0x00,0x00,0x68,0x58,0x02,0x00,0x00};

	*(DWORD *)(&NewBits[1])= y;
	*(DWORD *)(&NewBits[6])= x;
	CompositeBufSizeHook= new SingleHook ( 0x458195, 10, INLINE_UNPROTECTEVINMENT, NewBits);
}


unsigned int SfxVectorLimitAry[SFXVECTORLIMITNUM]=
{
	0x00471184,
	0x004713D9,
	0x00471509,
	0x0047163E,
	0x00471783,

	0x004718B2,
	0x00471AD8,
	0x00472072,
	0x004721A0,
	0x004722D0,

	0x004723D7,
	0x004724D6,
	0x004725D5,
	0x004726C1,
	0x004727B1,

	0x0047289B,
	0x00472A5B,
	0x00472CDA
};
/*
hook
	00471C80   6A 4C            PUSH 4C
	00471C82   68 E8030000      PUSH 3E8
	00471C87 B9 10 E6 51 00      mov     ecx, offset SfxEffectProperty
*/


int __stdcall Sfx_mallocBufSizeRouter (PInlineX86StackBuffer X86StrackBuffer)
{
	*reinterpret_cast<DWORD *>(X86StrackBuffer->Esp)= NowCrackLimit->NowIncreaseSfxLimit->CurtMaxLimit;
	return 0;
}

IncreaseSfxLimit::IncreaseSfxLimit ()
{
	SetNewLimit ( 0x5000, 0x50000);
}

IncreaseSfxLimit::IncreaseSfxLimit (DWORD SfxVectorNum)
{
	SetNewLimit ( SfxVectorNum, SfxVectorNum* 10);
}

IncreaseSfxLimit::~IncreaseSfxLimit ()
{
	IDDrawSurface::OutptTxt ("Release IncreaseSfxLimit");
	for (int i= 0; i<SFXVECTORLIMITNUM; ++i)
	{
		if (NULL!=SHookSfxVectorAry[i])
		{
			delete SHookSfxVectorAry[i];
		}
	}
	if (NULL!=Sfx_mallocBufSize)
	{
		delete Sfx_mallocBufSize;
	}
}

void IncreaseSfxLimit::SetNewLimit ( DWORD SfxVectorNum, DWORD MaxLimit)
{
	IDDrawSurface::OutptTxt ("IncreaseSfxLimit");
	for (int i= 0; i<SFXVECTORLIMITNUM; ++i)
	{
		SHookSfxVectorAry[i]= NULL;
	}
	Sfx_mallocBufSize= NULL;

	CurtSfxVectorNum= SfxVectorNum;
	CurtMaxLimit= MaxLimit;

	for (int i= 0; i<SFXVECTORLIMITNUM; ++i)
	{
		SHookSfxVectorAry[i]= new SingleHook ( SfxVectorLimitAry[i], 4, INLINE_UNPROTECTEVINMENT, reinterpret_cast<LPBYTE>(&SfxVectorNum));
	}
	Sfx_mallocBufSize= new InlineSingleHook ( Sfx_mallocBufSizeAddr, 0x5, INLINE_5BYTESLAGGERJMP, Sfx_mallocBufSizeRouter);
}