#include "oddraw.h"
#include "hook/etc.h"
#include "hook/hook.h"
#include "AiSeardchMapEntriesLimit.h"
#include "WeaponIDLimit.h"
#include "UnitTypeLimit.h"
#include "IncreaseCompositeSize.h"

#include "tamem.h"
#include "tafunctions.h"

IncreaseAISearchMapEntriesLimit::IncreaseAISearchMapEntriesLimit ()
{
	//OrginalLimit= *AISearchMapEntriesLimit;
	DWORD Const5000= 0x5000;
	ModifyTheLimit= new SingleHook ( reinterpret_cast<LPBYTE>(AISearchMapEntriesLimit), 4, INLINE_UNPROTECTEVINMENT, reinterpret_cast<LPBYTE>(&Const5000));
};

IncreaseAISearchMapEntriesLimit::IncreaseAISearchMapEntriesLimit (DWORD NewLimit)
{
	//OrginalLimit= *AISearchMapEntriesLimit;
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
	reinterpret_cast<LPVOID *>(0x049D28D),
	reinterpret_cast<LPVOID *>(0x42ED48),
	reinterpret_cast<LPVOID *>(0x042ED70),
	reinterpret_cast<LPVOID *>(0x49E5CC),
	reinterpret_cast<LPVOID *>(0x042ECB2),
	reinterpret_cast<LPVOID *>(0x042F360),
	reinterpret_cast<LPVOID *>(0x42F378),
	reinterpret_cast<LPVOID *>(0x042ED48)
};

LPDWORD WeaponAryLen [LENARYNUM]= 
{
	reinterpret_cast<LPDWORD>(0x42E33F),
	reinterpret_cast<LPDWORD>(0x42F433),
	reinterpret_cast<LPDWORD>(0x49E5ED)
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

void IncreaseWeaponTypeLimit::NewLimit (DWORD NewLen)
{
	OrgPtr= *(WeaponRelatedPtr[0]);
	OrglLen= *(WeaponAryLen[0]);

	CurtPtr= NULL;
	CurtLen= NewLen;

	for (int i= 0; i<PTRARYNUM; ++i)
	{
		PtrHookAry[i]= NULL;
	}
	for (int i= 0; i<LENARYNUM; ++i)
	{
		LenHookAry[i]=NULL;
	}

	if (CurtLen<=OrglLen)
	{
		return ;
	}
	DWORD tempCurtLen= CurtLen+ reinterpret_cast<DWORD>((&(reinterpret_cast<TAdynmemStruct *>(0)->Weapons)))+ sizeof(WeaponStruct);
	LPVOID tempCurtPtr= malloc ( tempCurtLen);
	


	WriteWeaponAryPtr ( &CurtPtr);
	WriteWeaponAryLen ( CurtLen);
	CurtPtr=tempCurtPtr;
}


IncreaseWeaponTypeLimit::~IncreaseWeaponTypeLimit ()
{
	for (int i= 0; i<PTRARYNUM; ++i)
	{
		if (NULL!=PtrHookAry[i])
		{
			delete PtrHookAry[i];
		}
		
	}
	for (int i= 0; i<LENARYNUM; ++i)
	{
		if (NULL!= LenHookAry[i])
		{
			delete LenHookAry[i];
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
		PtrHookAry[i]= new SingleHook ( reinterpret_cast<LPBYTE>(WeaponRelatedPtr[i]), 4, INLINE_UNPROTECTEVINMENT, reinterpret_cast<LPBYTE>( &NewPtr));
	}

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
	WriteNewLimit ( 0x3000);
}

IncreaseUnitTypeLimit::IncreaseUnitTypeLimit ( int Number)
{	
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
	WriteNewLimit ( Number);
}
//
IncreaseUnitTypeLimit::~IncreaseUnitTypeLimit ()
{
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

}

void IncreaseUnitTypeLimit::WriteNewLimit (DWORD Number)
{
	//weight func
	DWORD Orginal= 0x40;
	DWORD New;
	New= ((Number/ 8/ 0x40)+ 1)* 0x40;// 0x200 Number== 0x40 New

	if (Number<=Orginal)
	{
		return ;
	}

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
	*(DWORD *)(&Argc_2_limit_bits[4])= New- 0x40+ 0x54;

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
}


/*
int __stdcall PushCompositeBuf (PInlineX86StackBuffer X86StrackBuffer)
{
	X86StrackBuffer->Esp-= 4;
	*(DWORD *)X86StrackBuffer->Esp= NowIncreaseCompositeBuf->CurtX;//width
	
	X86StrackBuffer->Esp-= 4;
	*(DWORD *)X86StrackBuffer->Esp= NowIncreaseCompositeBuf->CurtY;//height

	return X86STRACKBUFFERCHANGE;
}*/

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
	delete CompositeBufSizeHook;
}

void IncreaseCompositeBuf::WriteNewLimit (DWORD x, DWORD y)
{
	CurtX= x;
	CurtY= y;
	BYTE NewBits[]= {0x68,0x58,0x02,0x00,0x00,0x68,0x58,0x02,0x00,0x00};

	*(DWORD *)(&NewBits[1])= y;
	*(DWORD *)(&NewBits[6])= x;
	CompositeBufSizeHook= new SingleHook ( 0x458195, 10, INLINE_UNPROTECTEVINMENT, NewBits);
}

