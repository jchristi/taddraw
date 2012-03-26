#ifndef tafunctionsH
#define tafunctionsH

class InlineSingleHook;
struct msgstruct{
	int xpos;
	int ypos;
	int shiftstatus; //should be 5 for shiftclick
};

struct posstruct{
	int x;
	int y;
};


//////////////////////////////////////////////////////////////////////////////////////////
/// Working.
//////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned short (__stdcall *_FindMouseUnit)(void); //find OwnUnitBegin under mousepointer
extern _FindMouseUnit FindMouseUnit;
//fill TAdynmem->MouseMapPosX & TAdynmem->MouseMapPosY first
typedef void (__stdcall *_TAMapClick)(void *msgstruct);
extern _TAMapClick TAMapClick;
typedef void (__stdcall *_TestBuildSpot)(void);
extern _TestBuildSpot TestBuildSpot;
//Type - 0 = chat, 1 = popup
typedef int (__stdcall *_SendText)(char *Text, int Type);
extern _SendText SendText;
typedef void (__stdcall *_ShowText)(PlayerStruct *Player, char *Text, int Unk1, int Unk2);
extern _ShowText ShowText;
typedef void (__stdcall *_TADrawRect)(char *Context, tagRECT *rect, int color);
extern _TADrawRect TADrawRect;
typedef void (__cdecl *_TADrawLine)(char *Context, int x1,int y1,int x2,int y2,int color);
extern _TADrawLine TADrawLine; 

typedef int (__stdcall *_GetContext)(char *ptr);
extern _GetContext GetContext;
//CirclePointer = CirclePointer in tadynmemstruct
typedef void (__stdcall *_TADrawCircle)(char *context, void *CirclePointer, posstruct *pos, int radius, int color, char *text, int unk);
extern _TADrawCircle TADrawCircle;

typedef void (__stdcall * _ApplySelectUnitMenu) (void);
extern _ApplySelectUnitMenu ApplySelectUnitMenu;
typedef void (__cdecl * _free_SafeWay) (LPVOID MemAddress);
extern _free_SafeWay free_SafeWay;
typedef void (__cdecl * _malloc_SafeWay) (int MemSize_I);
extern _malloc_SafeWay malloc_SafeWay;
typedef void (__stdcall * _UpdateLOSState) (int Update_Type);
extern _UpdateLOSState UpdateLosState;

typedef BOOL (__stdcall * _LoadTARegConfig) (void);
extern _LoadTARegConfig LoadTARegConfig;

typedef int (__stdcall * _ViewCommandProc) (char * );//this is spec struct, but I'm lazy to define it.
extern _ViewCommandProc ViewCommandProc;
//////////////////////////////////////////////////////////////////////////////////////////
/// Not working.
//////////////////////////////////////////////////////////////////////////////////////////
typedef void (__stdcall *_TestGridSpot)(UnitStruct *BuildUnit, int pos, int unk, PlayerStruct *Player); //unk=zero
extern _TestGridSpot TestGridSpot;


////-----------
typedef int (__stdcall * _GetIniFileInt) (LPCSTR lpKeyName, INT nDefault);
extern _GetIniFileInt GetIniFileInt;

typedef void (__stdcall * _ApplySelectUnitGUI) (void);
typedef void (__cdecl * _free_SafeWay) (LPVOID MemAddress);
typedef LPDWORD (__stdcall * __GetUnitIDMaskAryByCategory) (LPSTR);

int ViewPlayerLos_Replay (int PlayerAryIndex);
int UpdateTAProcess (void);
BOOL AntiCheat (void);
void DeselectUnits(void);
void UpdateSelectUnitEffect (void);
void freeTAMem (LPVOID MemAddress);
LPDWORD GetUnitIDMaskAryByCategory (LPSTR CategoryName_cstrp);
void UpdateSelectUnitEffect (void);
void ApplySelectUnitMenu_Wapper (void);
extern LPBYTE AddrAboutCircleSelect;

extern LPDWORD AISearchMapEntriesLimit;
extern DWORD Sfx_mallocBufSizeAddr;

extern TAdynmemStruct * * TAmainStruct_PtrPtr;
extern LPCSTR CompanyName_CCSTR;
extern LPCSTR GameName_CCSTR;

extern unsigned int EnterOption_Address;
extern unsigned int PressInOption_Address;
extern unsigned int AddtionInitAddr;

extern unsigned int Blt_BottomState0_TextRtn;
extern unsigned int Blt_BottomState0_TextAddr;

extern unsigned int Blt_BottomState1_TextRtn;
extern unsigned int Blt_BottomState1_TextAddr;

extern unsigned int PopadStateAddr;
//////////////////////////////////////////////////////////////////////////
extern InlineSingleHook * AddtionInitHook;
extern unsigned int GetTextExtent_AssignCharLenAddr;
extern unsigned int GetStrExtentAddr;

extern unsigned int Addr_0049E91C;
extern unsigned int Addr_0049E93B;
extern unsigned int Addr_00491A75;
extern unsigned int Addr_00491B01;
extern unsigned int Addr_0049802B;
extern unsigned int Addr_004980AD;

extern unsigned int TADontInit_Addr;

extern unsigned int MPUnitLimitAddr;
extern unsigned int UnitLimit0Addr;
extern unsigned int UnitLimit1Addr;
extern unsigned int UnitLimit2Addr;

extern BOOL * IsCheating;                        
extern WNDPROC TAWndProc_Addr;
extern unsigned int TAWndProcSH_Addr;

#endif
