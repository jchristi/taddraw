#ifndef EXTERNQUICKEY_H_316SDHSD
#define EXTERNQUICKEY_H_316SDHSD


class InlineSingleHook;
enum TAUnitType;



int __stdcall AddtionRoutine_CircleSelect (PInlineX86StackBuffer X86StrackBuffer);

class ExternQuickKey
{
public:
	LPDWORD CommanderMask;
	LPDWORD MobileWeaponMask;
	LPDWORD ConstructorMask;
	LPDWORD FactoryMask;
	LPDWORD BuildingMask;
	LPDWORD AirWeaponMask;
	LPDWORD AirConMask;

private:
	TAdynmemStruct * TAMainStruct_Ptr;
	HANDLE Semaphore_OnlyInScreenSameType;
	HANDLE Semaphore_FilterSelect;
	HANDLE Semaphore_OnlyInScreenWeapon;
	HANDLE Semaphore_IdleCons;
	HANDLE Semaphore_IdleFac;

	BOOL DoubleClick;

	InlineSingleHook * HookInCircleSelect;


	int VirtualKeyCode;
	int CategroyMaskSize;

	char *Add;
	char *Sub;
	char OldAdd;
	char OldSub;
	char NumAdd;
	char NumSub;
public:
	ExternQuickKey ();
	~ExternQuickKey ();
public:
	bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	int FilterSelectedUnit (TAUnitType NeededType);

	void AddtionInit (void);
	void AddtionRelease (void);


private:
	int SelectOnlyInScreenSameTypeUnit (BOOL FirstSelect_Flag);
	int SelectOnlyInScreenWeaponUnit (unsigned int SelectWay_Mask);

	void FindIdelFactory ();
	void FindIdleConst();
	void ScrollToCenter(int x, int y);

	int InitExternTypeMask (void);
	void DestroyExternTypeMask (void);
};

extern ExternQuickKey * myExternQuickKey;

#endif //EXTERNQUICKEY_H_316SDHSD