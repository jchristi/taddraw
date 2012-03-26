
// TA 试验品.h : TA 试验品 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CTA试验品App:
// 有关此类的实现，请参阅 TA 试验品.cpp
//

class CTA试验品App : public CWinAppEx
{
public:
	CTA试验品App();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
protected:
	HMENU  m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnFILEOPEN();

	DECLARE_MESSAGE_MAP()
};

extern CTA试验品App theApp;
