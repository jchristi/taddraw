/*
HPI PATH bin struct

0x00 HPIHeader TheHPIHeader;
0x14 HPIiIndexDirectory TheRootDir;//

normaly, the directory and file iIndex is all store in file begin, just like a part of HPIHeader;
so HPIHeader is a must exist header, directory is optional header

in HPI File Packer, 
none mean no compress
TA use lz77 compress
TACC use zlib compress
TAK, same as TACC
*/
int dirFirst (LPCSTR Path, int Path_StrLen);
int dirLast (LPCSTR Path, int Path_StrLen);
int lwrncmp (char * str1, char* str2, size_t Max_Size);

#pragma pack(1)
typedef struct _HPIHEADER {
	long HPIMarker;              /* 'HAPI' */
	long SaveMarker;             /* 'BANK' if savegame */
	long HeaderSize;             /* Whole header size, include iIndex */
	long Key;                    /* decode key */
	long OptionalHeaderRVA;      /* offset of HPIiIndexDirectory TheRootDir*/
} HPIHEADER;

typedef struct _HPIENTRY {
	int NameRVA;
	int iEntryRVA;// point to HPIiIndexDirectory or HPIiIndexFile
	unsigned char IsDir;//1 byte bool, 0==false, 1==true
} HPIENTRY;

typedef struct _HPIiIndexDirectory
{
	unsigned int Count;
	unsigned int iEntryRVA;// offset from file start. ptr to iEntryAry
	HPIENTRY iEntryAry[0];// elem number as Count. this array usually lie after iEntryRVA.
} HPIiIndexDirectory;


typedef struct _HPIiIndexFile
{
	unsigned int * HPICHUNKSizeAryRVA;// ptr to a array of chunk size, 4 bytes for at most 65536+ sizeof(HPICHUNK); HPICHUNKSizeAry
	unsigned int OrgFileSize;
	unsigned char CompMethod;
} HPIiIndexFile;

// struct HPICHUNKSizeAry
// 	// ChunkNumber= (OrgFileSize/ 65536)+ 1;
// {
// 	unsigned int HPICHUNKSize [0];
// 
// 	struct 
// 	{
// 		HPICHUNK ChunkHeader;
// 		unsigned char Data[0];
// 	} HPICHUNKArray [0];
// };

typedef struct _HPICHUNK {
	long Marker;            /* always 0x48535153 (SQSH) */
	unsigned char Unknown1;          /* I have no idea what these mean */
	unsigned char CompMethod;				/* 1=lz77 2=zlib */
	unsigned char Encrypt;           /* Is the chunk encrypted? */
	long CompressedSize;    /* the length of the compressed data */
	long DecompressedSize;  /* the length of the decompressed data */
	long Checksum;          /* check sum */
	unsigned char data [0]; //65536
} HPICHUNK;

typedef struct _HPIFILEINFO
{
	iIndex * FileiIndex;
	unsigned int SeekPointer;
} HPIFILEINFO;
#pragma pack()

///----------------application's struct

typedef struct _BinFileInfo {
	HANDLE f;   // handle to open file
	LPSTR d;    // pointer to decrypted directory
	int Key;	  // Key
	int Start;  // Start of directory
	iIndex *Root;
	int iIndexCounter;
	int DirSize;
	int Key;
	int CompressMethod;
} BinFileInfo;

typedef struct _iIndex {
	struct _iIndex *Next;
	struct _iIndex *Prev;
	struct _iIndex *FirstSub;
	struct _iIndex *LastSub;
	string _iIndex * Parent;
	int count;
	unsigned int * DirOffset;
	LPSTR Name;
	LPSTR FileName;
	LPBYTE Data;
	unsigned char dirflag;
	unsigned char CompressMode;
	unsigned char memflag;
	unsigned char padding;
} iIndex;

typedef struct _TREENODE {
	struct _TREENODE *tree_r;
	struct _TREENODE *tree_l;
	int tree_b;
	void *tree_p;
} TREENODE;


#define NO_COMPRESSION (0)
#define LZ77_COMPRESSION (1)
#define ZLIB_COMPRESSION (2)

//this is copy from Joe's HPI DLL Source, so thanks to him like me!

#define HEX_HAPI (0x49504148)
// 'BANK'
#define HEX_BANK (0x4B4E4142)
// 'SQSH'
#define HEX_SQSH (0x48535153)
#define HEX_MARKER (0x00010000)

#define OUTBLOCKSIZE (16384)

#define INBLOCKSIZE (65536+17)

#define TADIR_REG_ENTRY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Total Annihilation"



class HPIArchive
{
public:
	enum HPIPATHBINSTATE
	{
		STATE_NOFILE= 0,
		STATE_OPENED,
		STATE_READING,
		STATE_WRITING,
		STATE_CLOSED=0x80000000
	};

	enum EnumType
	{
		INVALID= 0,
		FILE= 1,
		DIRECTORY= 2,
		STEP= 4,
		INCLUDETHISDIR= 8,
		ATTEMPTBUFFER= 16,
		SUBDIRECTORY= 32
	};

public:
	iIndex * fHPIInPath (LPCSTR DirPath, LPCSTR FileName, int FileName_StrLen);
	iIndex * fHPIInPath (iIndex * Dir_iIndex, LPCSTR FileName, int FileName_StrLen);
	iIndex * fHPIInPath (iIndex * Dir_iIndex, iIndex * File_iIndex);
	iIndex * fHPIlocale (LPCSTR DirPath);

	HPIFILEINFO * fHPICreate (LPCSTR FilePath);
	HPIFILEINFO * fHPIOpen (LPCSTR FilePath);
	HPIFILEINFO * fHPIOpen (iIndex * FileiIndex);
	void fHPIClose (HPIFILEINFO * Handle);
	BOOL fHPIRead (HPIFILEINFO * Handle, LPBYTE Buf, unsigned int BufSize, unsigned int * Readed);
	unsigned int fHPISeek (HPIFILEINFO * Handle, unsigned int NewPostion);
	BOOL fHPIWrite (HPIFILEINFO * Handle, LPBYTE Buf, unsigned int BufSize, unsigned int * Written);
	int fHPILs (iIndex * Dir_iIndex, EnumType EnumMethod, iIndex * * RtnBuf_IO, int MaxBufSize);
	int fHPILs (LPCSTR DirPath, EnumType EnumMethod, iIndex * * RtnBuf_IO, int MaxBufSize);
	iIndex * fHPIMkDir (LPCSTR DirPath);
	BOOL fHPIDel (LPCSTR Path);
	BOOL fHPIDel (iIndex * FileiIndex);
	int fHPIlen (LPCSTR FilePath);
	LPCSTR fHPIrename (HPIFILEINFO * Handle, LPCSTR NewName);

	iIndex * fHPIMoveInDir (iIndex * Dir_iIndex, iIndex * File_iIndex);

	SetCompression ();

	CreateHPIPathBin (LPCSTR HPIPATHBinName_Cstrp);

	LPCSTR OpenHPIPathBin (LPCSTR HPIPATHBinName_Cstrp);
	BOOL WriteHPIPathBin (HANDLE HPIPATHBin_HFile, LPDWORD FileSize_Pdw);
	BOOL WriteHPIPathBin (LPDWORD FileSize_Pdw);
	BOOL WriteHPIPathBin (LPCSTR HPIPATHBinName_Cstrp, LPDWORD FileSize_Pdw);
	CloseHPIPathBin ();
	HPIPATHBINSTATE GetHPIPATHBinState ();

	HPIArchive ();
	HPIArchive (LPCSTR HPIPATHBinName_Cstrp);

	~HPIArchive ();

private:
	HPIPATHBINSTATE TheHPIPATHBinState;
	WCHAR * CurtHPIPATHBinName_Cstrp;
	HANDLE CurtHPIPATHBin_HFile;

	HPIHEADER CurtHPIHeader;
	BinFileInfo CurtBinFileInfo;
	LPSTR CurtDirectroy;

private:
	int GenerateHPIChunk_Dir (LPBYTE WriteToMemPtr, unsigned int HPIRVA, iIndex * Dir, int CMethod);
	int GenerateHPIChunk_File (LPBYTE WriteToMemPtr, unsigned int HPIRVA, iIndex * In, int CMethod);
	int iIndex2HPIiIndexFile (HPIiIndexFile * Out, unsigned int HPIRVA, iIndex * In, int CMethod);
	int iIndex2HPIiIndexDirectory (HPIiIndexDirectory * Out, unsigned int HPIRVA, iIndex * In, int CMethod);

	int ReloadHPIChunk_File (unsigned int HPIRVA, LPBYTE BaseOfFile, iIndex * Dir, int CMethod);
	int ReloadHPIChunk_Directory (unsigned int HPIRVA, LPBYTE BaseOfFile, iIndex * In, int CMethod);
	int HPIiIndexFile2iIndex (iIndex * Out, int HPIRVA, HPIiIndexFile * In, LPSTR FileName);
	int HPIiIndexDirectory2iIndex (iIndex * Out, LPBYTE BaseOfFile, int HPIRVA, HPIiIndexDirectory * In, HPIENTRY * Dir_HPIEntry);

	void XorBuffByKey (int fpos, LPBYTE buff, int buffsize);
	int WriteData (int fpos, LPBYTE b, int buffsize);
	int ReadData ( int fpos, LPBYTE buff, int buffsize);

	int inline InsertRtnBuf_fHPILsRoutine (iIndex * * RtnBuf_IO, int MaxBufSize, int Pos,iIndex * NewOne);
};

HPIArchive::HPIArchive ( )
{
	TheHPIPATHBinState= STATE_NOFILE;
	CurtHPIPATHBinName_Cstrp= malloc ( MAX_PATH+ 1);
	CurtHPIPATHBin_HFile= INVALID_HANDLE_VALUE;

	memset ( &CurtHPIHeader, 0, sizeof( HPIHEADER));
	memset ( &CurtBinFileInfo, 0, sizeof( BinFileInfo));
}

HPIArchive::~HPIArchive ( )
{
	TheHPIPATHBinState= STATE_CLOSED;
	if (NULL!=CurtHPIPATHBinName_Cstrp)
	{
		free ( CurtHPIPATHBinName_Cstrp);
	}

	if (NULL!=CurtBinFileInfo->d)
	{
		free (CurtBinFileInfo->d);
	}
}

LPCSTR HPIArchive::OpenHPIPathBin (LPCSTR HPIPATHBinName_Cstrp)
{
	if ((STATE_NOFILE!=TheHPIPATHBinState)
		||(NULL==HPIPATHBinName_Cstrp))
	{
		goto WorseEnd;
	}
	
	CurtHPIPATHBin_HFile= CreateFileA ( HPIPATHBinName_Cstrp, GENERIC_WRITE| GENERIC_READ, FILE_SHARE_DELETE| FILE_SHARE_READ| FILE_SHARE_WRITE,
								NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE==CurtHPIPATHBin_HFile)
	{
		goto WorseEnd;
	}

	int BytesRead;

	if (! ReadFile ( CurtHPIPATHBin_HFile, &CurtHPIHeader, sizeof(HPIHEADER), &BytesRead, NULL)) 
	{
		goto BadEnd;
	}

	if (BytesRead != sizeof(Header)) 
	{
		goto BadEnd;
	}

	if (Header.HPIMarker != HEX_HAPI) 
	{  /* 'HAPI' */

		goto BadEnd;
	}

	if (Header.SaveMarker != HEX_MARKER) 
	{

		goto BadEnd;
	}

	CurtBinFileInfo->f = CurtHPIPATHBin_HFile;
	if (Header.Key)
	{
		CurtBinFileInfo->Key = ~((CurtHPIHeader.Key * 4)| (CurtHPIHeader.Key >> 6));
	}
	else
	{
		CurtBinFileInfo->Key = 0;
	}

	CurtBinFileInfo->Start = CurtHPIHeader.OptionalHeaderRVA;
	CurtBinFileInfo->d= static_cast<LPSTR>( malloc ( GetFileSize ( CurtHPIPATHBin_HFile, NULL), TRUE));

	BytesRead = ReadData ( 0, CurtBinFileInfo->d, 
							 GetFileSize ( CurtHPIPATHBin_HFile, NULL));
	if (BytesRead != (  GetFileSize ( CurtHPIPATHBin_HFile, NULL))) 
	{
		if (NULL!=CurtBinFileInfo->d)
		{
			free (CurtBinFileInfo->d);
		}
		
		memset ( &CurtBinFileInfo, 0, sizeof( BinFileInfo));

		goto BadEnd;
	}

	
	//load iIndex

	HPIENTRY * Root_HPIEntry= reinterpret_cast<HPIENTRY *>(CurtBinFileInfo.d+ CurtHPIHeader.OptionalHeaderRVA);
	HPIiIndexDirectory * Root_HPIiIndex= reinterpret_cast<HPIiIndexDirectory *>(CurtBinFileInfo.d+ Root_HPIEntry->iEntryRVA);
	
	CurtBinFileInfo.Root= reinterpret_cast<iIndex *>(malloc ( sizeof( iIndex)));
	int RVAHPI= HPIiIndexDirectory2iIndex ( CurtBinFileInfo.Root, CurtBinFileInfo->d, 0, 
		Root_HPIiIndex, Root_HPIEntry);
	//Load chunk
	///(unsigned int HPIRVA, iIndex * In, int CMethod
	ReloadHPIChunk_Directory ( RVAHPI, CurtBinFileInfo->d,  CurtBinFileInfo.Root, CurtBinFileInfo.CompressMethod);
	
	strcpy_s ( CurtHPIPATHBinName_Cstrp, MAX_PATH+ 1,HPIPATHBinName_Cstrp);

	TheHPIPATHBinState= STATE_OPENED;
	return CurtHPIPATHBinName_Cstrp;

BadEnd:
	memset ( &CurtHPIHeader, 0, sizeof( HPIHEADER));


	CloseHandle(CurtHPIPATHBin_HFile);
	CurtHPIPATHBin_HFile= INVALID_HANDLE_VALUE;
WorseEnd:
	return NULL;
}

BOOL HPIArchive::WriteHPIPathBin (HANDLE HPIPATHBin_HFile, LPDWORD FileSize_Pdw)
{
	if ((INVALID_HANDLE_VALUE==HPIPATHBin_HFile)
		||(NULL==HPIPATHBin_HFile))
	{
		return FALSE;
	}
	TheHPIPATHBinState= STATE_WRITING;

	int WholeFileSize= 0;
	int AlliIndexPtrSize= fHPILs ( ".", SUBDIRECTORY| ATTEMPTBUFFER, NULL, 0);
	iIndex * * AlliIndexPtrAry= reinterpret_cast<iIndex * *>(malloc ( AlliIndexPtrSize));
	memset ( AlliIndexPtrAry, 0, AlliIndexPtrSize);
	fHPILs ( ".", SUBDIRECTORY, AlliIndexPtrAry, AlliIndexPtrSize);
	int Counter=  AlliIndexPtrSize/ sizeof( iIndex *);
	if (0==AlliIndexPtrSize)
	{
		return FALSE;
	}

	while (0<=(--Counter))
	{
		if (1!=AlliIndexPtrAry[Counter].dirflag)
		{
			WholeFileSize= WholeFileSize+ sizeof( HPIENTRY)+ sizeof(HPIiIndexFile)+ AlliIndexPtrAry[AlliIndexPtrSize].count;
		}
		else
		{
			WholeFileSize= WholeFileSize+ sizeof( HPIENTRY)+ sizeof(HPIiIndexDirectory)+ 4* AlliIndexPtrAry[AlliIndexPtrSize].count;
		}
	}
	WholeFileSize= WholeFileSize+ sizeof(HPIHEADER);
	WholeFileSize= ((WholeFileSize/ 65536)+ 1)* (sizeof(HPICHUNK)+ 4);

	WholeFileSize= WholeFileSize- (WholeFileSize& (0x1000- 1))+ 0x1000;// align to 0x1000, 1 memory page

	LPBYTE FileMem= static_cast<LPBYTE>( malloc ( WholeFileSize));
	if (NULL==FileMem)
	{
		return FALSE;
	}
	//header
	memcpy ( FileMem, CurtHPIHeader, sizeof(HPIHEADER));

	//iIndex
	int OffToData= iIndex2HPIiIndexDirectory ( FileMem+ sizeof(HPIHEADER), sizeof(HPIHEADER), CurtBinFileInfo.Root, CurtBinFileInfo.CompressMethod);
	//chunk
	int OffToEnd= GenerateHPIChunk_Dir ( FileMem+ OffToData, OffToData, CurtBinFileInfo.Root, CurtBinFileInfo.CompressMethod);
	//write to hard disk
	DWORD Written_dw;
	if (NULL==FileSize_Pdw)
	{
		FileSize_Pdw= &Written_dw;
	}

	TheHPIPATHBinState= STATE_OPENED;
	return WriteFile ( HPIPATHBin_HFile, FileMem, OffToEnd, FileSize_Pdw, NULL);
}

BOOL HPIArchive::WriteHPIPathBin (LPCSTR HPIPATHBinName_Cstrp, LPDWORD FileSize_Pdw)
{
	if (NULL==HPIPATHBinName_Cstrp)
	{
		return FALSE;
	}
	//malloc suitable memory
	HANDLE HPIPATHBin_HFile= CreateFileA ( HPIPATHBinName_Cstrp, GENERIC_WRITE| GENERIC_READ, FILE_SHARE_DELETE| FILE_SHARE_READ| FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	BOOL Rtn_B= WriteHPIPathBin ( HPIPATHBin_HFile, FileSize_Pdw);

	CloseHandle ( HPIPATHBin_HFile);
	return Rtn_B;
}

BOOL HPIArchive::WriteHPIPathBin (LPDWORD FileSize_Pdw)
{
	return WriteHPIPathBin ( CurtHPIPATHBin_HFile, FileSize_Pdw);
}
//

int HPIArchive::iIndex2HPIiIndexDirectory (HPIiIndexDirectory * Out, unsigned int HPIRVA, iIndex * In, int CMethod)
{
	int UsedRVA;
	iIndex * CurtiIndex;
	LPSTR NamePtr;
	int Counter= 0;
	LPBYTE OutBuffer= reinterpret_cast<LPBYTE>(Out);
	Out->Count= In->count;
	Out->iEntryRVA= HPIRVA+ sizeof( HPIiIndexDirectory);//alem number is  [0], so will not count HPIENTRY in here

	UsedRVA = Out->iEntryRVA + (In->count * sizeof(HPIENTRY));
	CurtiIndex = In->FirstSub;
	while (Counter<(In->count)) 
	{
		Out->iEntryAry[Counter].NameRVA= UsedRVA;
		
		NamePtr = CurtiIndex->Name;
		while (*NamePtr)
		{
			OutBuffer[UsedRVA++] = *NamePtr++;
		}
		OutBuffer[UsedRVA++] = 0;
		Out->iEntryAry[Counter].iEntryRVA = UsedRVA;

		if (CurtiIndex->dirflag) 
		{
			Out->iEntryAry[Counter].IsDir = 1;
			UsedRVA = iIndex2HPIiIndexDirectory ( reinterpret_cast<HPIiIndexDirectory *>(Out->iEntryAry[Counter].iEntryRVA), Out->iEntryAry[Counter].iEntryRVA, CurtiIndex, CMethod);
		}
		else 
		{
			Out->iEntryAry[Counter].IsDir = 0;
			UsedRVA= iIndex2HPIiIndexFile ( reinterpret_cast<HPIiIndexFile *>(Out->iEntryAry[Counter].iEntryRVA), Out->iEntryAry[Counter].iEntryRVA, CurtiIndex, CMethod);
		}
		CurtiIndex= CurtiIndex->Next;
	}
	return UsedRVA;
}


//---------------- 
int HPIArchive::iIndex2HPIiIndexFile (HPIiIndexFile * Out, unsigned int HPIRVA, iIndex * In, int CMethod)
{
	Out->OrgFileSize= In->count;
	In->DirOffset= &(Out->HPICHUNKSizeAryRVA);
	Out->CompMethod= CMethod;
	return HPIRVA+ sizeof( HPIiIndexFile);
}

int HPIArchive::GenerateHPIChunk_File (LPBYTE WriteToMemPtr, unsigned int HPIRVA, iIndex * In, int CMethod)
{
	HPICHUNK Chunk_Header;
	unsigned int remain;
	unsigned int Written;
	*In->DirOffset = HPIRVA;
	LPBYTE OrgMemPtr= WriteToMemPtr;
	

	if (NO_COMPRESSION!=CurtBinFileInfo.CompressMethod) 
	{
		unsigned int ChunkCount= d->count / 65536;
		unsigned int CurtChunkCount= 0;
		unsigned int * PtrToSizeAry;
		unsigned int * SizeAryBuf;
		unsigned int SizeAryLen;
		unsigned char * ChunkBuf= static_cast<unsigned char * >(malloc ( INBLOCKSIZE));

		if ((In->count) % 65536)
			ChunkCount++;

		PtrToSizeAry = static_cast<unsigned int *>( WriteToMemPtr);

		SizeAryLen = ChunkCount * 4;
		SizeAryBuf = static_cast<unsigned int *>(malloc ( SizeAryLen, TRUE));
		memset ( SizeAryBuf, 0, SizeAryLen);

		WriteToMemPtr += SizeAryLen;

		remain = In->count;
		Written= 0;

		while (remain) 
		{
			Chunk_Header.Marker = HEX_SQSH;
			Chunk_Header.Unknown1 = 0x02;
			Chunk_Header.CompMethod = CurtBinFileInfo.CompressMethod;
			Chunk_Header.Encrypt = (0!=CurtBinFileInfo.Key)? 1: 0;
			Chunk_Header.CompressedSize = 0;
			Chunk_Header.DecompressedSize = (remain > 65536 ? 65536 : remain);
			Chunk_Header.Checksum = 0;

			//EncryptAndWrite(Pack, fpos, &BlockHeader, sizeof(BlockHeader));
			//

			memset ( ChunkBuf, INBLOCKSIZE);

			//MoveMemory(InBlock, d->FileName+Pack->FileBytes, BlockHeader.DecompressedSize);
			CompressChunk ( WriteToMemPtr+ sizeof( HPICHUNK), In->Data+ Written, Chunk_Header, CMethod);

			memcpy	( WriteToMemPtr, &Chunk_Header, sizeof( HPICHUNK)); 

			SizeAryBuf[CurtChunkCount] = Chunk_Header.CompressedSize+ sizeof( HPICHUNK);

			WriteToMemPtr += sizeof( HPICHUNK);
			WriteToMemPtr += Chunk_Header.CompressedSize;
			remain -= Chunk_Header.DecompressedSize;
			Written+= Chunk_Header.DecompressedSize;
			CurtChunkCount++;
		}//while

		memcpy	( PtrToSizeAry, SizeAryBuf, SizeAryLen); 
		
		free ( SizeAryBuf);
		free ( ChunkBuf);
	}
	else 
	{
		memcpy	( WriteToMemPtr, In->Data, In->count); 

		WriteToMemPtr += In->count;
	}

	return (HPIRVA+ (WriteToMemPtr- OrgMemPtr));
}

int HPIArchive::GenerateHPIChunk_Dir (LPBYTE WriteToMemPtr, unsigned int HPIRVA, iIndex * Dir, int CMethod)
{
	unsigned int OrgHPIRVA= HPIRVA;

	iIndex * CurtiIndex= Dir->FirstSub;
	while (CurtiIndex) 
	{
		WriteToMemPtr= WriteToMemPtr- OrgHPIRVA+ HPIRVA;//  :)
		if (CurtiIndex->dirflag)
			HPIRVA = GenerateHPIChunk_Dir ( WriteToMemPtr, HPIRVA, CurtiIndex, CMethod);
		else
			HPIRVA = GenerateHPIChunk_File ( WriteToMemPtr, HPIRVA, CurtiIndex, CMethod);
		d = d->Next;
	}

	return HPIRVA;
}


int HPIArchive::ReloadHPIChunk_File (unsigned int HPIRVA, LPBYTE BaseOfFile, iIndex * In, int CMethod)
{

	unsigned int OrginalHPIRVA= HPIRVA;
	HPIRVA= *(In->DirOffset);
	unsigned * HPICHUNKSizeAry= reinterpret_cast<unsigned int *>(BaseOfFile+ HPIRVA);
	
	int Remain;
	int Counter;
	int Decompressed= 0;
	LPBYTE Data;
	HPICHUNK * CurtChunk;
	Remain= In->count/ 65536;

	Data= static_cast<LPBYTE>(malloc ( In->count));
	Counter= 0;
	HPIRVA= reinterpret_cast<unsigned int>(&HPICHUNKSizeAry[Remain])- reinterpret_cast<unsigned int>(BaseOfFile);
	
	while (Counter<Remain)
	{
		CurtChunk= reinterpret_cast<HPICHUNK *>(BaseOfFile+ HPIRVA);
		HPIRVA+= sizeof(HPICHUNK);
		
		Decompressed+= DeCompressChunk ( Data+ Decompressed, BaseOfFile+ HPIRVA, CurtChunk, CMethod);

		HPIRVA+= HPICHUNKSizeAry[Counter];
		Remain-= Decompressed;
		Counter++;
	}

	if (0!=Remain)
	{
		free ( Data);
		return OrginalHPIRVA;
	}
	In->Data= Data;
	return HPIRVA;
}

int HPIArchive::ReloadHPIChunk_Directory (unsigned int HPIRVA, LPBYTE BaseOfFile, iIndex * In, int CMethod)
{
	iIndex * SubFile;

	SubFile= In->FirstSub;

	while (SubFile!=In->LastSub)
	{
		if (1==SubFile->dirflag)
		{
			HPIRVA= ReloadHPIChunk_Directory ( HPIRVA, BaseOfFile, SubFile, CMethod);
		}
		else
		{
			HPIRVA= ReloadHPIChunk_File ( HPIRVA, BaseOfFile, SubFile, CMethod);
		}
	}
	return HPIRVA;
}

int HPIArchive::HPIiIndexFile2iIndex (iIndex * Out, int HPIRVA, HPIiIndexFile * In, LPSTR FileName)
{
	Out->Data= NULL;
	Out->count= In->OrgFileSize;
	Out->FirstSub= NULL;
	Out->LastSub= NULL;
	Out->Next= NULL;
	Out->Prev= NULL;
	Out->FileName= malloc ( strnlen ( FileName, MAX_PATH));
	strcpy_s ( Out->FileName, MAX_PATH,  FileName);
	Out->dirflag= 0;
	Out->CompressMode= In->CompMethod;
	Out->DirOffset= &(In->HPICHUNKSizeAryRVA);
	HPIRVA+= sizeof ( HPIiIndexFile);

	return HPIRVA;
}

int HPIArchive::HPIiIndexDirectory2iIndex (iIndex * Out, LPBYTE BaseOfFile, int HPIRVA, HPIiIndexDirectory * In, HPIENTRY * Dir_HPIEntry)
{
	//char FileName[MAX_PATH];

	Out->Data= NULL;
	Out->count= 0;

	Out->FirstSub= NULL;
	Out->LastSub= NULL;
	Out->Next= NULL;
	Out->Prev= NULL;
	Out->FileName= malloc ( strnlen ( BaseOfFile+ Dir_HPIEntry->NameRVA, MAX_PATH));
	strcpy_s ( Out->FileName, MAX_PATH, BaseOfFile+ Dir_HPIEntry->NameRVA);
	Out->dirflag= 1;
	Out->DirOffset= NULL;
	Out->CompressMode= NO_COMPRESSION;
	HPIRVA+= sizeof( HPIiIndexDirectory);

	//In->
	int Counter= 0;
	HPIENTRY * SubFile_HPIEntry;
	iIndex * SubFile_iIndex;

	while (Counter<(In->Count))
	{
		SubFile_iIndex= reinterpret_cast<iIndex *>(malloc ( sizeof(iIndex)));
		SubFile_HPIEntry= &(reinterpret_cast<HPIENTRY *>(BaseOfFile+ In->iEntryRVA)[Counter]);
		//In->iEntryAry[]
		if (1==SubFile_HPIEntry->IsDir)
		{
			HPIiIndexDirectory2iIndex ( SubFile_iIndex, BaseOfFile, reinterpret_cast<LPBYTE>(SubFile_HPIEntry)- BaseOfFile, SubFile_HPIEntry->iEntryRVA+ BaseOfFile, SubFile_HPIEntry);
		}
		else
		{
			HPIiIndexFile2iIndex ( SubFile_iIndex, reinterpret_cast<LPBYTE>(SubFile_HPIEntry)- BaseOfFile, SubFile_HPIEntry, BaseOfFile+ SubFile_HPIEntry->NameRVA);
		}

		fHPIMoveInDir ( Out, SubFile_iIndex);

		++(Out->count);
	}
	//
	HPIRVA= reinterpret_cast<int>(&(reinterpret_cast<HPIENTRY *>(BaseOfFile+ In->iEntryRVA)[In->Count]));

	return HPIRVA;
}

//---------
iIndex * HPIArchive::fHPIMoveInDir (iIndex * Dir_iIndex, iIndex * File_iIndex)
{
	//clean the old relationship

	if (NULL!=File_iIndex->Parent)
	{
		if ( File_iIndex==(File_iIndex->Parent->FirstSub))
		{
			File_iIndex->Parent->FirstSub= File_iIndex->Parent->FirstSub->Next;
			if (NULL!=(File_iIndex->Parent->FirstSub))
			{
				File_iIndex->Parent->FirstSub->Prev= NULL;
			}
			
		}

		if ( File_iIndex==(File_iIndex->Parent->LastSub))
		{
			File_iIndex->Parent->LastSub= File_iIndex->Parent->LastSub->Prev;
			if (NULL!=(File_iIndex->Parent->LastSub))
			{
				File_iIndex->Parent->LastSub->Next= NULL;
			}
		}

		--(File_iIndex->Parent->count);
		File_iIndex->Next= NULL;
		File_iIndex->Prev= NULL;
	}

	//
	File_iIndex->Parent= Dir_iIndex;
	if (NULL!=File_iIndex->Parent)
	{
		if (NULL==File_iIndex->Parent->FirstSub)
		{
			File_iIndex->Parent->FirstSub= 
				File_iIndex->Parent->LastSub= File_iIndex;
			File_iIndex->Prev= NULL;
			File_iIndex->Next= NULL;
		}
		else if ( NULL!=File_iIndex->Parent->LastSub)
		{
			File_iIndex->Parent->LastSub->Next= File_iIndex;
			File_iIndex->Prev= File_iIndex->Parent->LastSub;
			File_iIndex->Next= NULL;

			File_iIndex->Parent->LastSub= File_iIndex;
		}
		else
		{//last null, first not null
			iIndex * RealLastOne= File_iIndex->Parent->FirstSub;
			while (NULL!=(RealLastOne->Next)) 
			{
				RealLastOne= RealLastOne->Next;
			};
			RealLastOne->Next= File_iIndex;
			File_iIndex->Prev= RealLastOne;
			File_iIndex->Next= NULL;

			File_iIndex->Parent->LastSub= File_iIndex;
		}
		++(File_iIndex->Parent->count);
	}
	return File_iIndex;
}

HPIFILEINFO * HPIArchive::fHPICreate (LPCSTR FilePath, iIndex * * Rtn_iIndexPtr, BOOL IsDir)
{
	iIndex * FileiIndex;

	FileiIndex= fHPIlocale ( FilePath);
	if (NULL!=FileiIndex)
	{
		goto TheEnd;
	}

	//create new iIndex!
	FileiIndex= reinterpret_cast<iIndex *> (malloc ( sizeof( iIndex)));

	memset ( FileiIndex, 0, sizeof( iIndex));
	FileiIndex->count= 0;
	FileiIndex->Data= NULL;
	FileiIndex->dirflag= IsDir?1: 0;//

	//locale file name, and file directory.
	int temp;

	int PathLen= strlen ( FilePath);
	temp= PathLen;
	while(('\\'!=FilePath[temp])
	&&('/'!=FilePath[temp--])
	&&(temp>=0));

	LPSTR DirName= malloc ( PathLen);
	memset ( DirName, 0, PathLen);
	FileiIndex->Name= malloc ( PathLen);
	if (temp<0)
	{
		strcpy_s ( FileiIndex->Name, PathLen, FilePath);
	}
	else
	{
		strcpy_s ( DirName, temp, FilePath);
		strcpy_s ( FileiIndex->Name, PathLen, FilePath+ temp );
	}
	
	fHPIMoveInDir ( fHPIlocale ( DirName), FileiIndex);

	free ( DirName);
	//FileiIndex->Parent= ;
	if (NULL==FileiIndex->Parent)
	{
		free ( FileiIndex->Name);
		free ( FileiIndex);
		FileiIndex= NULL;
		goto TheEnd;
	}

TheEnd:
	if (NULL!=Rtn_iIndexPtr)
	{
		*Rtn_iIndexPtr= FileiIndex;
	}
	return fHPIOpen ( FileiIndex);
}

HPIFILEINFO * HPIArchive::fHPICreate (LPCSTR FilePath)
{
	fHPICreate ( FilePath, NULL, FALSE);
}

iIndex * HPIArchive::fHPIMkDir (LPCSTR DirPath)
{
	iIndex * Rtn_iIndexPtr;
	fHPICreate ( DirPath, &Rtn_iIndexPtr, TRUE);
	return Rtn_iIndexPtr;
}

HPIFILEINFO * HPIArchive::fHPIOpen (LPCSTR FilePath)
{
	return fHPIOpen ( fHPIlocale ( FilePath));
}
HPIFILEINFO * HPIArchive::fHPIOpen (iIndex * FileiIndex)
{
	if (NULL==FileiIndex)
	{
		return NULL;
	}

	HPIFILEINFO * Rtn= static_cast<HPIFILEINFO *>(malloc ( sizeof( HPIFILEINFO)));
	Rtn->FileiIndex= FileiIndex;
	Rtn->SeekPointer= 0;
}
void HPIArchive::fHPIClose (HPIFILEINFO * Handle)
{
	if (NULL!=Handle)
	{
		free ( Handle);
	}
}

LPCSTR HPIArchive::fHPIrename (HPIFILEINFO * Handle, LPCSTR NewName)
{
	if ((NULL!=Handle)
		||(NULL==Handle->FileiIndex)
		||(NULL==NewName))
	{
		return NULL;
	}
	if (NULL!=(Handle->FileiIndex->Name))
	{
		free (Handle->FileiIndex->Name);
		Handle->FileiIndex->Name= NULL;
	}
	int NewNameLen= strnlen ( NewName, 0x200);// should be ok.

	Handle->FileiIndex->Name= malloc ( NewNameLen);
	strcpy_s ( Handle->FileiIndex->Name, NewNameLen, NewName);

	return Handle->FileiIndex->Name;
}


BOOL HPIArchive::fHPIRead (HPIFILEINFO * Handle, LPBYTE Buf, unsigned int BufSize, unsigned int * Readed)
{
	if (NULL==Buf||0<=BufSize)
	{
		return FALSE;
	}
	LPBYTE CopyPtr= (Handle->FileiIndex->Data)+ Handle->SeekPointer;
	LPBYTE FileEnd= Handle->FileiIndex->Data+ Handle->FileiIndex->count;
	if ((CopyPtr<(Handle->FileiIndex->Data))
		||(FileEnd<CopyPtr))
	{
		return FALSE;
	}
	if (FileEnd<(CopyPtr+ BufSize))
	{
		BufSize= FileEnd- CopyPtr;
	}

	memcpy ( Buf, CopyPtr , BufSize);
	if (NULL!=Readed)
	{
		*Readed= BufSize;
	}
	return TRUE;
}

unsigned int HPIArchive::fHPISeek (HPIFILEINFO * Handle, unsigned int NewPostion)
{
	if (NULL!=Handle)
	{
		LPBYTE CopyPtr= (Handle->FileiIndex->Data)+ Handle->SeekPointer;
		LPBYTE FileEnd= Handle->FileiIndex->Data+ Handle->FileiIndex->count;
		if (((Handle->FileiIndex->Data)<=CopyPtr)
			&&(CopyPtr<FileEnd))
		{
			Handle->SeekPointer= NewPostion;
		}
		
		return Handle->SeekPointer;
	}
	return 0;
}

BOOL HPIArchive::fHPIWrite (HPIFILEINFO * Handle, LPBYTE Buf, unsigned int BufSize, unsigned int * Written)
{
	if (NULL==Buf||0<=BufSize)
	{
		return FALSE;
	}
	LPBYTE CopyPtr= (Handle->FileiIndex->Data)+ Handle->SeekPointer;
	LPBYTE FileEnd= Handle->FileiIndex->Data+ Handle->FileiIndex->count;
	if ((CopyPtr<(Handle->FileiIndex->Data))
		||(FileEnd<CopyPtr))
	{
		return FALSE;
	}

	if ((NULL==Handle->FileiIndex->Data)
		||(FileEnd<(CopyPtr+ BufSize)))
	{
		LPBYTE OldData= Handle->FileiIndex->Data;
		Handle->FileiIndex->Data= static_cast<LPBYTE>(malloc ( Handle->SeekPointer+ BufSize));
		if (NULL==Handle->FileiIndex->Data)
		{
			free ( OldData);
			return FALSE;
		}
		memcpy ( Handle->FileiIndex->Data, OldData, Handle->SeekPointer);
		CopyPtr= Handle->FileiIndex->Data+ Handle->SeekPointer;
		memcpy ( CopyPtr, Buf, BufSize);
		free ( OldData);
	}
	else
	{
		memcpy ( CopyPtr,  Buf, BufSize);
	}
	
	if (NULL!=Written)
	{
		*Written= BufSize;
	}
	return TRUE;
}


BOOL HPIArchive::fHPIDel (LPCSTR Path)
{
	return fHPIDel ( fHPIlocale ( Path));
}

BOOL HPIArchive::fHPIDel (iIndex * FileiIndex)
{
	if (NULL!=FileiIndex)
	{
		if (NULL!=FileiIndex->Data)
		{
			free (FileiIndex->Data);
		}
		if (NULL!=FileiIndex->Name)
		{
			free (FileiIndex->Name);
		}
		
		if (NULL!=FileiIndex->Next)
		{
			FileiIndex->Next->Prev= FileiIndex->Prev;
		}
		if (NULL!=FileiIndex->Prev)
		{
			FileiIndex->Prev->Next= FileiIndex->Next;
		}
		if (NULL!=FileiIndex->Parent)
		{
			if (FileiIndex==FileiIndex->Parent->FirstSub)
			{
				FileiIndex->Parent->FirstSub= FileiIndex->Next;
			}
			if (FileiIndex==FileiIndex->Parent->LastSub)
			{
				FileiIndex->Parent->LastSub= FileiIndex->Prev;
			}
		}
		free ( FileiIndex);
		return TRUE;
	}
	return FALSE;
}

int HPIArchive::fHPIlen (LPCSTR FilePath)
{
	return fHPILs ( FilePath, FILE| ATTEMPTBUFFER);
}

int HPIArchive::fHPILs (LPCSTR DirPath, EnumType EnumMethod, iIndex * * RtnBuf_IO, int MaxBufSize)
{
	return fHPILs ( fHPIlocale ( DirPath), EnumMethod, RtnBuf_IO);
}

int HPIArchive::fHPILs (iIndex * Dir_iIndex, EnumType EnumMethod, iIndex * * RtnBuf_IO, int MaxBufSize)
{// silly function, but I'm no interesting to modify it at now.
	int EnumCounter;
	iIndex * Curt_iIndex;

	BOOL AttempBuffer_B= (0!=(ATTEMPTBUFFER& EnumMethod));
	BOOL EnumFile_B= (0!=(FILE& EnumMethod));
	BOOL EnumDirectory_B= (0!=(DIRECTORY& EnumMethod));
	BOOL StepEnum_B= (0!=(STEP& EnumMethod));
	BOOL EnumThisOne_B= (0!=(INCLUDETHISDIR& EnumMethod));
	BOOL EnumSubDir_B=  (0!=(SUBDIRECTORY& EnumMethod));
	BOOL TRUEWhenEnumed_B= FALSE;

	if ((AttempBuffer_B)
		&&(NULL!=Dir_iIndex))
	{
		//
		if ((!EnumDirectory_B)
			&& EnumFile_B)
		{// when enum file SIZE
			int FileSize= 0;

			if (EnumThisOne_B)
			{
				if (1!=Dir_iIndex->dirflag)
				{
					FileSize= Dir_iIndex->count;
				}
			}
			if (EnumSubDir_B)
			{//SUB DIRECTORY file size!!!!
				Curt_iIndex= Dir_iIndex->FirstSub;
				while (Curt_iIndex!=Dir_iIndex->LastSub)
				{
					if (1==Curt_iIndex->dirflag)
					{
						//enum sub directory needed buff size
						FileSize= FileSize+ fHPILs ( Curt_iIndex, EnumMethod, RtnBuf_IO, MaxBufSize);
					}
					else
					{
						FileSize= Curt_iIndex->count;
					}
				}
			}
			return FileSize;
		}
		int NeededBufSize= Dir_iIndex->count* sizeof(iIndex *);
		if (EnumThisOne_B)
		{
			NeededBufSize+= sizeof(iIndex *);
		}

		if (EnumSubDir_B)
		{//SUB DIRECTORY!!!!
			Curt_iIndex= Dir_iIndex->FirstSub;
			while (Curt_iIndex!=Dir_iIndex->LastSub)
			{
				if (1==Curt_iIndex->dirflag)
				{
					//enum sub directory needed buff size
					NeededBufSize= NeededBufSize+ fHPILs ( Curt_iIndex, EnumMethod, RtnBuf_IO, MaxBufSize);
				}
			}
		}
		return NeededBufSize;
	
	}
		
	if ((NULL==Dir_iIndex)
		||(NULL==RtnBuf_IO))
	{
		return 0;
	}

	if (0==EnumMethod
		||((!EnumFile_B)&&(!EnumDirectory_B)))
	{
		EnumFile_B= TRUE;
		EnumDirectory_B= TRUE;
	}

	EnumCounter= 0;
	if (EnumThisOne_B)
	{
		Curt_iIndex= Dir_iIndex;
		
		if ((EnumCounter==InsertRtnBuf_fHPILsRoutine ( RtnBuf_IO, MaxBufSize, EnumCounter, Curt_iIndex))
			&&(StepEnum_B)
		{//
			return ++EnumCounter;
		}
		++EnumCounter;
	}

	Curt_iIndex= Dir_iIndex->FirstSub;
	while (Curt_iIndex!=Dir_iIndex->LastSub)
	{
		EnumThisOne_B= FALSE;
		TRUEWhenEnumed_B= FALSE;
		if ((EnumDirectory_B)
			&&(1==Curt_iIndex->dirflag))
		{
			//enum sub directory
			if (EnumSubDir_B)
			{
				int SubDirectoryEnumCounter= fHPILs ( Curt_iIndex, EnumMethod, &RtnBuf_IO[EnumCounter], MaxBufSize);
				if (0!=SubDirectoryEnumCounter)
				{
					TRUEWhenEnumed_B= TRUE;
					EnumCounter= EnumCounter+ SubDirectoryEnumCounter;
				}
			}
			else
			{
				EnumThisOne_B= TRUE;
			}
		}

		if ((EnumFile_B)
			&&(0==Curt_iIndex->dirflag))
		{
			EnumThisOne_B= TRUE;
		}

		if (EnumThisOne_B)
		{//
			++EnumCounter;
			if (EnumCounter!=InsertRtnBuf_fHPILsRoutine ( RtnBuf_IO, MaxBufSize, EnumCounter, Curt_iIndex))
			{
				TRUEWhenEnumed_B= TRUE;
			}
		}
		if (TRUEWhenEnumed_B&&StepEnum_B)
		{// yes, we add some new iIndex to Buff, maybe this enum produce is set as step-step .
			break;
		}

		Curt_iIndex= Dir_iIndex->Next;
	}

	return EnumCounter;
}

iIndex * HPIArchive::fHPIInPath (LPCSTR DirPath, LPCSTR FileName, int FileName_StrLen)
{
	return fHPIInPath ( fHPIlocale ( DirPath), FileName, FileName_StrLen);
}

iIndex * HPIArchive::fHPIInPath (iIndex * Dir_iIndex, LPCSTR FileName, int FileName_StrLen)
{
	if ((NULL==Dir_iIndex)
		||(NULL==FileName)
		||(0==FileName_StrLen))
	{
		return NULL;
	}
	if ('.'==FileName[0])
	{
		if ('.'==FileName[1])
		{//
			return Dir_iIndex->Parent;
		}
		return Dir_iIndex;
	}

	int iIndexBufSize_I= fHPILs ( Dir_iIndex, ATTEMPTBUFFER, NULL, 0);
	iIndex * * iIndexBuf= reinterpret_cast<iIndex * *>(malloc ( iIndexBufSize_I));
	memset ( iIndexBuf, 0, iIndexBufSize_I);
	iIndex * Rtn= NULL;

	fHPILs ( Dir_iIndex, 0, iIndexBuf, iIndexBufSize_I);

	iIndexBufSize_I= iIndexBufSize_I/ sizeof ( iIndex *);
	for (int i= 0; i<iIndexBufSize_I; ++i)
	{
		if (0==lwrncmp ( FileName, iIndexBuf[i].Name, FileName_StrLen))
		{
			Rtn= iIndexBuf[i];
		}
	}

	free ( iIndexBuf);
	return Rtn;
}

iIndex * HPIArchive::fHPIInPath (iIndex * Dir_iIndex, iIndex * File_iIndex)
{
	if (NULL==File_iIndex)
	{
		return FALSE;
	}

	int Buf_StrLen= strnlen ( File_iIndex->Name, MAX_PATH);
	char * Buf= malloc ( Buf_StrLen);

	int Rtn_I= fHPIInPath ( Dir_iIndex, Buf, Buf_StrLen);
	free ( Buf);
	return Rtn_I;
}

iIndex * HPIArchive::fHPIlocale (LPCSTR DirPath)
{
	if (NULL==DirPath)
	{
		return NULL;
	}

	int DirPath_StrLen= strnlen ( DirPath, MAX_PATH);

	int CurtDirOff;
	int CurtDirLen;
	iIndex * CurtDir_iIndex;
	iIndex * ParentDir_iIndex;

	CurtDirOff= dirFirst ( DirPath, DirPath_StrLen);

	if (0==strncmp ( DirPath+ CurtDirOff, "..", 2))
	{// never support .. as a root path.
		return NULL;
	}
	
	CurtDirLen= 0;
	CurtDir_iIndex= CurtBinFileInfo.Root;

	// parse DirPath to exactly iIndex.
	do 
	{
		if (0==strncmp ( DirPath+ CurtDirOff, "..", 2))
		{// back to parent directory
			CurtDir_iIndex= ParentDir_iIndex;
		}

		CurtDirLen= dirLast ( DirPath+ CurtDirOff);//
		CurtDirLen= (0<CurtDirLen)		\
			? (CurtDirLen- CurtDirOff)	\
			: (DirPath_StrLen- CurtDirOff);
		ParentDir_iIndex= CurtDir_iIndex;
		if (NULL==(CurtDir_iIndex= 
			fHPIInPath ( CurtDir_iIndex, DirPath+ CurtDirOff, CurtDirLen))
			)
		{//
			break;
		}

		CurtDirOff=  DirPath+ CurtDirOff+ CurtDirLen;
	} while ((0<CurtDirLen)
		&&(0<CurtDirOff)
		&&(CurtDirOff<DirPath_StrLen));

	return CurtDir_iIndex;
}

//static int LZ77CompressChunk(PACKFILE *Pack, LPSTR Chunk, HPICHUNK *BlockHeader, int fpos){...}
//static int ZLibCompressChunk(PACKFILE *Pack, char *Chunk, HPICHUNK *BlockHeader, int fpos){...}
int HPIArchive::CompressChunk (LPBYTE Out, LPBYTE In, HPICHUNK * Chunk_Ptr, int CMethod)
{// rtn the out len;
	int x;
	int Checksum;
	int Rtn= 0;
	Checksum = 0;

	switch (CMethod)
	{
	case LZ77_COMPRESSION:
		Rtn= LZ77CompressChunk ( Out, In, Chunk_Ptr);
		break;
	case ZLIB_COMPRESSION:
		Rtn= ZLibCompressChunk ( Out, In, Chunk_Ptr);
		break;
	}

	return Rtn;
}
//static int LZ77Decompress(char *out, char *in, HPICHUNK *Chunk){...}
//int ZLibDecompress(char *out, char *in, HPICHUNK *Chunk){...}

int HPIArchive::DeCompressChunk (LPBYTE Out, LPBYTE In, HPICHUNK * Chunk_Ptr, int CMethod)
{// rtn the out len;
	int x;
	int Checksum;
	int Rtn= 0;
	Checksum = 0;

	for (x = 0; x < Chunk_Ptr->CompressedSize; x++) {
		Checksum += (unsigned char) in[x];
	}

	if (Chunk_Ptr->Checksum != Checksum) 
	{// make sure checksum
		return 0;
	}

	switch (CMethod)
	{
	case LZ77_COMPRESSION:
		Rtn= LZ77Decompress ( Out, In, Chunk_Ptr);
		break;
	case ZLIB_COMPRESSION:
		Rtn= ZLibDecompress ( Out, In, Chunk_Ptr);
		break;
	}

	return Rtn;
}

void HPIArchive::XorBuffByKey (int fpos, LPBYTE buff, int buffsize)
{
	int count;
	int tkey;

	for (count = 0; count < result; count++) 
	{
		tkey = (fpos + count) ^ CurtBinFileInfo.Key;
		buff[count] = tkey ^ ~buff[count];
	}
}

int HPIArchive::WriteData (int fpos, LPBYTE b, int buffsize)
{
	int result;
	char *buff;

	if (0!CurtBinFileInfo->Key) 
	{
		buff = malloc (buffsize, FALSE);
		XorBuffByKey ( fpos, buff, buffsize);
	}
	else 
	{
		buff = b;
	}
	SetFilePointer ( CurtHPIPATHBin_HFile, fpos, 0, FILE_BEGIN);

	WriteFile ( CurtHPIPATHBin_HFile, buff, buffsize, &result, NULL);

	if (buff != b)
	{
		free (buff);
	}

	return result;
}

int HPIArchive::ReadData ( int fpos, LPBYTE buff, int buffsize)
{

	int result;

	SetFilePointer ( CurtBinFileInfo.f, fpos, NULL, FILE_BEGIN); 
	if (!ReadFile ( CurtBinFileInfo.f, buff, buffsize, &result, NULL))
		return 0;
	if (0!=CurtBinFileInfo.Key) 
	{
		XorBuffByKey ( fpos, buff, result);
	}
	return result;
}

int HPIArchive::InsertRtnBuf_fHPILsRoutine (iIndex * * RtnBuf_IO, int MaxBufSize, int Pos,iIndex * NewOne)
{
	for (int i= 0; i<MaxBufSize;++i)
	{
		if (NewOne==RtnBuf_IO[i])
		{
			return i;
		}
	}
	RtnBuf_IO[Pos]= NewOne;
	return NewOne;
};


int lwrncmp (char * str1, char* str2, size_t Max_Size)
{
	if ((NULL==str1)||
		(NULL==str2))
	{
		return 0;
	}
	int Rtn_I= 0;
	char * lwrstr1= malloc ( Max_Size);
	char * lwrstr2= malloc ( Max_Size);

	if ((NULL==lwrstr1)
		||(NULL==lwrstr2))
	{
		goto BadEnd;
	}

	strcpy_s ( lwrstr1, str1, Max_Size);
	strlwr_s ( lwrstr2, str2, Max_Size);

	Rtn_I= strncmp ( lwrstr1, lwrstr2, Max_Size);

BadEnd:
	if (NULL!=lwrstr1)
	{
		free ( lwrstr1);
	}
	if (NULL!=lwrstr2)
	{
		free ( lwrstr2);
	}
	return Rtn_I;
}


int dir_SearchRoutine (LPCSTR Path, int Path_StrLen, BOOL IsNeed)
{
	if ((NULL==Path)||(0==Path_StrLen))
	{
		return 0;
	}
	int CurtPos;

	for ( CurtPos= 0; CurtPos<Path_StrLen; ++CurtPos)
	{
		if (IsNeed==(NULL!=strchr ( "\\/\"':", Path[CurtPos])))
		{
			break;
		}
	}
	if (IsNeed)
	{
		--CurtPos;
	}
	if (0>CurtPos)
	{
		CurtPos= 0;
	}
	if (CurtPos>Path_StrLen)
	{
		CurtPos= Path_StrLen
	}

	return CurtPos;
}

int dirLast (LPCSTR Path, int Path_StrLen)
{
	int rtn;
	do 
	{
		rtn= dir_SearchRoutine ( Path, Path_StrLen, TRUE);
	} while ((0!=rtn)
		&&('\\'!=Path[rtn])
		&&('/'==Path[rtn]));
	return rtn;
}

int dirFirst (LPCSTR Path, int Path_StrLen)
{//
	return dir_SearchRoutine ( Path, Path_StrLen, FALSE);
}