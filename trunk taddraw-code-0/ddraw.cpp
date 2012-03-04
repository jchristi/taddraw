//---------------------------------------------------------------------------
#include "oddraw.h"
#include "iddraw.h"
#include "iddrawsurface.h"
#include "weaponid.h"
#include <stdio.h>
#include "hook/etc.h"
#include "hook/hook.h"
#include "UnicodeSupport.h"
#include "AiSeardchMapEntriesLimit.h"
#include "WeaponIDLimit.h"
#include "UnitTypeLimit.h"
#include "IncreaseCompositeSize.h"
#include "MixingBuffers.h"
#include "tamem.h"
#include "tafunctions.h"
#include "ExternQuickKey.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#define TALobby 1
#define TALoading 2
#define TAInGame 3

#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

DataShare_* DataShare;
HANDLE hMemMap;
void *pMapView;

LocalShare_* LocalShare;
HANDLE LocalhMemMap;
void *LocalpMapView;

HANDLE TAHookhMemMap;
void *TAHookpMapView;


typedef HRESULT (*DirectDrawCreateProc)(GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);
//extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawCreate(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);
typedef HRESULT (*DirectDrawCreateExProc)(GUID FAR *, LPVOID ,  REFIID, IUnknown FAR *);
//extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawCreateEx(GUID FAR *arg1, LPVOID *arg2, REFIID arg3, IUnknown FAR *arg4);
typedef HRESULT (*DirectDrawCreateClipperProc)(DWORD, LPDIRECTDRAWCLIPPER FAR * , IUnknown FAR *);
//extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawCreateClipper(DWORD arg1, LPDIRECTDRAWCLIPPER FAR *arg2, IUnknown FAR *arg3);
typedef HRESULT (*DirectDrawEnumerateProc)(LPDDENUMCALLBACK, LPVOID);
//extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawEnumerate(LPDDENUMCALLBACK arg1, LPVOID arg2);
typedef HRESULT (*DirectDrawEnumerateExProc)(LPDDENUMCALLBACKEX, LPVOID, DWORD);
//extern "C" __declspec(dllexport) HRESULT WINAPI DirectDrawEnumerateEx(LPDDENUMCALLBACKEX arg1, LPVOID arg2, DWORD arg3);
typedef HRESULT (*DllGetClassObjectProc)(const CLSID &, const IID &, void **);
//extern "C" __declspec(dllexport) HRESULT WINAPI DllGetClassObject(const CLSID & arg1, const IID & arg2, void ** arg3);
typedef HRESULT (*DllCanUnloadNowProc)();
//extern "C" __declspec(dllexport) HRESULT WINAPI DllCanUnloadNow();

/*typedef int WINAPI (*VidMemLargestFreeProc)(LPVOID);
extern "C" __declspec(dllexport) int WINAPI VidMemLargestFree(LPVOID arg1);
typedef int WINAPI (*VidMemAmountFreeProc)(LPVOID);
extern "C" __declspec(dllexport) int WINAPI VidMemAmountFree(LPVOID arg1);
typedef int WINAPI (*VidMemFiniProc)(LPVOID);
extern "C" __declspec(dllexport) int WINAPI VidMemFini(LPVOID arg1);
typedef int WINAPI (*VidMemFreeProc)(LPVOID, LPVOID);
extern "C" __declspec(dllexport) int WINAPI VidMemFree(LPVOID arg1, LPVOID arg2);
typedef int WINAPI (*VidMemAllocProc)(LPVOID, LPVOID, LPVOID);
extern "C" __declspec(dllexport) int WINAPI VidMemAlloc(LPVOID arg1, LPVOID arg2, LPVOID arg3);
typedef int WINAPI (*VidMemInitProc)(LPVOID, LPVOID, LPVOID, LPVOID, LPVOID);
extern "C" __declspec(dllexport) int WINAPI VidMemInit(LPVOID arg1, LPVOID arg2, LPVOID arg3, LPVOID arg4, LPVOID arg5);
*/


bool SetupFileMap();
void ShutdownFileMap();
bool SetupLocalFileMap();
void ShutdownLocalFileMap();
void GetSysDir();

void SetupTAHookFileMap();
void ShutDownTAHookFileMap();
void EnableSound();

char SystemDDraw[MAX_PATH];

HINSTANCE SDDraw = NULL;

void OutptTxt(char *string);

bool Windowed;
HINSTANCE HInstance;
bool Log = true;

IncreaseAISearchMapEntriesLimit * NowIncreaseAISearchMapEntriesLimit;
IncreaseWeaponTypeLimit * NowIncreaseWeaponTypeLimit;
IncreaseUnitTypeLimit * NowIncreaseUnitTypeLimit;
IncreaseCompositeBuf * NowIncreaseCompositeBuf;
IncreaseMixingBuffers * NowIncreaseMixingBuffers;
InlineSingleHook* HookInCircleSelect;

//---------------------------------------------------------------------------
bool APIENTRY DllMain(HINSTANCE hinst, unsigned long reason, void*)
{
	HInstance = hinst;
	OutptTxt("DLL EntryPoint");

	if(reason==DLL_PROCESS_ATTACH)
	{
		OutptTxt("Process Attached");
		GetSysDir();
		SetupFileMap();
		DataShare = reinterpret_cast<DataSharePTR>(pMapView);
		SetupLocalFileMap();
		LocalShare = reinterpret_cast<LocalSharePTR>(LocalpMapView);

		if(!LocalShare)
			OutptTxt("Error creating shared lmap");
		else
			OutptTxt("lmap success");

		Windowed = false;
		SetupTAHookFileMap();

		DataShare->IsRunning = 5;

		
 		EnableSound();
		NowIncreaseAISearchMapEntriesLimit= new IncreaseAISearchMapEntriesLimit ( GetIniFileInt ( "AISearchMapEntries", 0x50000));
		NowIncreaseWeaponTypeLimit= new IncreaseWeaponTypeLimit (  GetIniFileInt ( "WeaponType", 0x1000) );
		NowIncreaseUnitTypeLimit= new IncreaseUnitTypeLimit ( GetIniFileInt ( "UnitType", 0x2000) );
		NowIncreaseCompositeBuf= new IncreaseCompositeBuf ( GetIniFileInt ( "X_CompositeBuf", 0x500) , GetIniFileInt ( "Y_CompositeBuf", 0x500));
		NowIncreaseMixingBuffers= new IncreaseMixingBuffers ( GetIniFileInt ( "MixingBuffers", 128));
		HookInCircleSelect= new InlineSingleHook ( (unsigned int)AddrAboutCircleSelect, 5, 
			INLINE_5BYTESLAGGERJMP, AddtionRoutine_CircleSelect);

// 	
// 
#ifdef UNICODE_SUPPORT
 		LocalShare->TAUnicodeSupport= (LPVOID) (new UnicodeSupport);
#endif //UNICODE_SUPPORT
	}
	if(reason==DLL_PROCESS_DETACH)
	{

		/* KillTimer(NULL, Timer);
		KillTimer(NULL, DetectTimer); */
#ifdef UNICODE_SUPPORT
		delete (UnicodeSupport *)(LocalShare->TAUnicodeSupport);
#endif //UNICODE_SUPPORT
		FreeLibrary(SDDraw);
		ShutdownLocalFileMap();
		ShutDownTAHookFileMap();

		delete NowIncreaseAISearchMapEntriesLimit;
		delete NowIncreaseWeaponTypeLimit;
		delete NowIncreaseUnitTypeLimit;
		delete NowIncreaseCompositeBuf;
		delete NowIncreaseMixingBuffers;
		delete HookInCircleSelect;
		/*  if(lpTAFrontBuf)
		lpTAFrontBuf->Release();
		if(lpTABackBuf)
		lpTABackBuf->Release();
		if(lpTADD2)
		lpTADD2->Release();
		if(BlitSurf)
		BlitSurf->Release();   */
	}

	return 1;
}
//---------------------------------------------------------------------------

void GetSysDir()
{
	char Buff[MAX_PATH];
	GetSystemDirectory(Buff, MAX_PATH);

	lstrcpyA(SystemDDraw,Buff);
	lstrcatA(SystemDDraw, "\\ddraw.dll");
	SDDraw = LoadLibrary(SystemDDraw);
}

HRESULT WINAPI DirectDrawCreate(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter)
{
	OutptTxt("DirectDrawCreate");
	if(SDDraw == NULL)
	{
		GetSysDir();
		SetupFileMap();
		DataShare = reinterpret_cast<DataSharePTR>(pMapView);
	}

	DirectDrawCreateProc Proc;
	Proc = (DirectDrawCreateProc) GetProcAddress(SDDraw, "DirectDrawCreate");
	HRESULT Result = Proc(lpGUID, lplpDD, pUnkOuter);

	IDDraw *DClass = new IDDraw(*lplpDD, Windowed);
	*lplpDD = (IDirectDraw*)DClass;

	OutptTxt("returning from DirectDrawCreate");
	Windowed = false;

	return Result;
}

void OutptTxt(char *string)
{
#ifdef DEBUG_INFO
	if(!Log)
		return;

	HANDLE file = CreateFileA("C:\\taddrawlog.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS	, 0, NULL);
	DWORD tempWritten;
	SetFilePointer ( file, 0, 0, FILE_END);
	WriteFile ( file, string, strlen(string), &tempWritten, NULL);
	WriteFile ( file, "\r\n", 1, &tempWritten, NULL);

	CloseHandle ( file);
#endif //DEBUG_INFO

}
//---------------------------------------------------------------------------
bool SetupFileMap()
{
	bool bExists;

	//create the mapping to the file
	hMemMap = CreateFileMapping((HANDLE)0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(DataShare_),
		"TADemo-MKChat");

	//see weather this is the first time this file has been mapped to
	bExists = (GetLastError() == ERROR_ALREADY_EXISTS);

	//Map a view into the Mapped file
	pMapView = MapViewOfFile(hMemMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(DataShare_));


	if (!bExists)
	{
		//if it is the first time this map has been made,
		//set all the members of our struct to NULL
		memset (reinterpret_cast<DataSharePTR>(pMapView), NULL, sizeof(DataShare_));
	}

	return true;
}

bool SetupLocalFileMap()
{
	bool bExists;
	LocalhMemMap = CreateFileMapping((HANDLE)0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(LocalShare_),
		"TAOverlayShare");

	bExists = (GetLastError() == ERROR_ALREADY_EXISTS);
	LocalpMapView = MapViewOfFile(LocalhMemMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(LocalShare_));

	if(!LocalpMapView)
		OutptTxt("Error creating shared lmap");


	if (!bExists)
	{
		memset (reinterpret_cast<LocalSharePTR>(LocalpMapView), NULL, sizeof(LocalShare_));
	}
	return true;
}

void ShutdownFileMap()
{
	//cleanup
	if(pMapView!=NULL)
		UnmapViewOfFile(pMapView);

	if(hMemMap != NULL)
		CloseHandle(hMemMap);
}

void ShutdownLocalFileMap()
{
	//cleanup
	if(LocalpMapView!=NULL)
		UnmapViewOfFile(LocalpMapView);

	if(LocalhMemMap != NULL)
		CloseHandle(LocalhMemMap);
}


HRESULT WINAPI DirectDrawCreateEx(GUID FAR *arg1, LPVOID *arg2, REFIID arg3, IUnknown FAR *arg4)
{
	DirectDrawCreateExProc Proc;
	Proc = (DirectDrawCreateExProc)GetProcAddress(SDDraw, "DirectDrawCreateEx");
	return Proc(arg1, arg2, arg3, arg4);
}

HRESULT WINAPI DirectDrawCreateClipper(DWORD arg1, LPDIRECTDRAWCLIPPER FAR *arg2, IUnknown FAR *arg3)
{
	DirectDrawCreateClipperProc Proc;
	Proc = (DirectDrawCreateClipperProc)GetProcAddress(SDDraw, "DirectDrawCreateClipper");
	return Proc(arg1, arg2, arg3);
}

HRESULT WINAPI DirectDrawEnumerate(LPDDENUMCALLBACK arg1, LPVOID arg2)
{
	DirectDrawEnumerateProc Proc;
	Proc = (DirectDrawEnumerateProc)GetProcAddress(SDDraw, "DirectDrawEnumerate");
	return Proc(arg1, arg2);
}

HRESULT WINAPI DirectDrawEnumerateEx(LPDDENUMCALLBACKEX arg1, LPVOID arg2, DWORD arg3)
{
	DirectDrawEnumerateExProc Proc;
	Proc = (DirectDrawEnumerateExProc)GetProcAddress(SDDraw, "DirectDrawEnumerateEx");
	return Proc(arg1, arg2, arg3);
}

HRESULT WINAPI DllGetClassObject(const CLSID & arg1, const IID & arg2, void ** arg3)
{
	DllGetClassObjectProc Proc;
	Proc = (DllGetClassObjectProc)GetProcAddress(SDDraw, "DllGetClassObject");
	return Proc(arg1, arg2, arg3);
}

HRESULT WINAPI DllCanUnloadNow()
{
	DllCanUnloadNowProc Proc;
	Proc = (DllCanUnloadNowProc)GetProcAddress(SDDraw, "DllCanUnloadNow");
	return Proc();
}

void SetupTAHookFileMap()
{
	//initiate a tahook filmap so the recorder detects tahook running
	bool bExists;
	TAHookhMemMap = CreateFileMapping((HANDLE)0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		6084,  //correct size so tahook doesnt krash if yank is running it
		"GlobalMap");

	bExists = (GetLastError() == ERROR_ALREADY_EXISTS);
	TAHookpMapView = MapViewOfFile(TAHookhMemMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		6084);
	if (!bExists)
	{
		//memset (TAHookpMapView, NULL, 6084);
	}
}

void ShutDownTAHookFileMap()
{
	//cleanup
	if(TAHookpMapView!=NULL)
		UnmapViewOfFile(TAHookpMapView);

	if(TAHookhMemMap != NULL)
		CloseHandle(TAHookhMemMap);
}

void EnableSound()
{
	//primary buffer
	int adress = 0x4011;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x4CEFE2, &adress, 4, NULL);

	//secondary buffer
	//adress = 0x4092;
	//WriteProcessMemory(GetCurrentProcess(), (void*)0x4CF3CF, &adress, 4, NULL);
}


/*
int WINAPI VidMemLargestFree(LPVOID arg1)
{
VidMemLargestFreeProc Proc;
Proc = (VidMemLargestFreeProc)GetProcAddress(SDDraw, "VidMemLargestFree");
return Proc(arg1);
}

int WINAPI VidMemAmountFree(LPVOID arg1)
{
VidMemAmountFreeProc Proc;
Proc = (VidMemAmountFreeProc)GetProcAddress(SDDraw, "VidMemAmountFree");
return Proc(arg1);
}

int WINAPI VidMemFini(LPVOID arg1)
{
VidMemFiniProc Proc;
Proc = (VidMemFiniProc)GetProcAddress(SDDraw, "VidMemFini");
return Proc(arg1);
}

int WINAPI VidMemFree(LPVOID arg1, LPVOID arg2)
{
VidMemFreeProc Proc;
Proc = (VidMemFreeProc)GetProcAddress(SDDraw, "VidMemFree");
return Proc(arg1, arg2);
}

int WINAPI VidMemAlloc(LPVOID arg1, LPVOID arg2, LPVOID arg3)
{
VidMemAllocProc Proc;
Proc = (VidMemAllocProc)GetProcAddress(SDDraw, "VidMemAlloc");
return Proc(arg1, arg2, arg3);
}

int WINAPI VidMemInit(LPVOID arg1, LPVOID arg2, LPVOID arg3, LPVOID arg4, LPVOID arg5)
{
VidMemInitProc Proc;
Proc = (VidMemInitProc)GetProcAddress(SDDraw, "VidMemInit");
return Proc(arg1, arg2, arg3, arg4, arg5);
}        */
