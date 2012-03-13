#pragma once

typedef void (__stdcall * _InitTAHPIAry)(void);
typedef char * (__stdcall * _readfile_HPI)(char *FilePath, unsigned int *ReadLen_Ptr);
typedef unsigned int (__stdcall * _filelen_HPI)(OPENTAFILE *TAFILE);
typedef unsigned int (__stdcall * _filelength_HPI)(char *FilePath);
typedef int (__stdcall * _fclose_HPI)(OPENTAFILE *TAFile);
typedef unsigned int (__stdcall * _read_HPI) (OPENTAFILE * TAFile , void *DstBuf, int len);
typedef OPENTAFILE * (__stdcall * _fopen_HPI)(char *FilePath, char *Mode);


extern _fopen_HPI fopen_HPI;
extern _read_HPI read_HPI;
extern _fclose_HPI fclose_HPI;
extern _filelength_HPI filelength_HPI;
extern _filelen_HPI filelen_HPI;
extern _readfile_HPI readfile_HPI;//freeTAMem for free the rtn buf
extern _InitTAHPIAry InitTAHPIAry;

class _TAHPI
{
public:
	_TAHPI (BOOL Inited);
	~_TAHPI ();

	POPENTAFILE fopen(char *FilePath, char *Mode);
	int read ( POPENTAFILE TAFile , void *DstBuf, int len);
	int fclose (POPENTAFILE TAFile);
	unsigned int filelen (POPENTAFILE TAFILE);
	unsigned int filelength (char *FilePath);
	char * readfile (char * FilePath, unsigned int *ReadLen_Ptr);
	void free_readfile ( char * mem_ptr);
};


extern _TAHPI * TAHPI;

