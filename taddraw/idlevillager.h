#ifndef idlevillagerH
#define idlevillagerH


class CIdleUnits
{
	private:
		HANDLE Semaphore_IdleCons;

  private:
    void FindIdleConst();
    void ScrollToCenter(int x, int y);

  public:
    CIdleUnits();
    ~CIdleUnits();
    void Blit(LPDIRECTDRAWSURFACE DestSurf);
    bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};

#endif