#pragma once

struct _GAFFrame;
typedef struct _GafHeader 
{
	unsigned int Signature;//0x10100
	unsigned int Entries;//number
	unsigned int Always0;
}GafHeader, * PGafHeader;

typedef struct _GAFSequence
{
	unsigned short Frames;
	unsigned int Signature;//00000001
	char Name[32];
	_GAFFrame * PtrFrameAry;// this one will reloc to own PGAFFrame on running time as well.
	int Animated;//2 = animated, 10 = fixed.
}GAFSequence, * PGAFSequence;

typedef struct _GAFFrame
{
	unsigned short Width;
	unsigned short Height;
	unsigned short xPosition;
	unsigned short yPosition;
	unsigned char Background;//0x9
	unsigned char Compressed; //
	unsigned char unknow_0;
	unsigned char Unknow_1;//0x0
	unsigned int Unknow_2;//0x0
	unsigned char * PtrFrameBits;
	unsigned int FPS;
} GAFFrame, * PGAFFrame;


LPDIRECTDRAWSURFACE CreateSurfByGafFrame (PGAFFrame Cursor_P, bool VidMem);