#pragma 

class InlineSingleHook;
struct tagInlineX86StackBuffer;
typedef struct tagInlineX86StackBuffer * PInlineX86StackBuffer;
int  __stdcall sub_0049E91C (PInlineX86StackBuffer X86StrackBuffer);
int  __stdcall sub_0049E93B (PInlineX86StackBuffer X86StrackBuffer);
int  __stdcall sub_00491A75 (PInlineX86StackBuffer X86StrackBuffer);
int __stdcall sub_00491B01 (PInlineX86StackBuffer X86StrackBuffer);

class MenuResolution
{
private:
	InlineSingleHook * Hook_0049E91C;
	InlineSingleHook * Hook_0049E93B;
	InlineSingleHook * Hook_00491A75;
	InlineSingleHook * Hook_00491B01;
	InlineSingleHook * Hook_0049802B;
	InlineSingleHook * Hook_004980AD;
public:
	MenuResolution ();
	MenuResolution (BOOL EqualIt_b);
	~MenuResolution ();
private:
	void InstallHook (void);

	void UninstallHook (void);
};

extern MenuResolution* SyncMenuResolution;