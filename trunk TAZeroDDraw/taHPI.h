#pragma once

typedef void (__stdcall * _InitTAHPIAry)(void);
typedef char * (__stdcall * _readfile_HPI)(char *FilePath, int *ReadLen_Ptr);
typedef int (__stdcall * _filelen_HPI)(OPENTAFILE *TAFILE);
typedef int (__stdcall * _filelength_HPI)(char *FilePath);
typedef int (__stdcall * _fclose_HPI)(OPENTAFILE *TAFile);
typedef int (__stdcall * _read_HPI) (OPENTAFILE * TAFile , void *DstBuf, int len);
typedef OPENTAFILE * (__stdcall * _fopen_HPI)(char *FilePath, char *Mode);

extern _fopen_HPI fopen_HPI;
extern _read_HPI read_HPI;
extern _fclose_HPI fclose_HPI;
extern _filelength_HPI filelength_HPI;
extern _filelen_HPI filelen_HPI;
extern _readfile_HPI readfile_HPI;//freeTAMem for free the rtn buf
extern _InitTAHPIAry InitTAHPIAry;
