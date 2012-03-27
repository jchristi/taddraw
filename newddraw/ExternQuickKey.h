#ifndef EXTERNQUICKEY_H_316SDHSD
#define EXTERNQUICKEY_H_316SDHSD


class InlineSingleHook;
enum TAUnitType;

int __stdcall AddtionRoutine_CircleSelect (PInlineX86StackBuffer X86StrackBuffer);

class ExternQuickKey
{
private:
	TAdynmemStruct * TAMainStruct_Ptr;
	HANDLE Semaphore_OnlyInScreenSameType;
	HANDLE Semaphore_FilterSelect;
	HANDLE Semaphore_OnlyInScreenWeapon;
	HANDLE Semaphore_IdleCons;
	HANDLE Semaphore_IdleFac;

	InlineSingleHook * HookInCircleSelect;


	int VirtualKeyCode;


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
	bool SetIDMaskInTypeAry (WORD ID, DWORD SelectedUnitTypeIDAry_Dw[]);
	bool MatchInTypeAry (WORD ID, DWORD SelectedUnitTypeIDAry_Dw[]);

	void FindIdelFactory ();
	void FindIdleConst();
	void ScrollToCenter(int x, int y);

};

#endif //EXTERNQUICKEY_H_316SDHSD