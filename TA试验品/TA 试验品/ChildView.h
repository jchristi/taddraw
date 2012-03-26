
// ChildView.h : CChildView 类的接口
//


#pragma once

class TNTtoMiniMap;
// CChildView 窗口

class CChildView : public CWnd
{
// 构造
public:
	CChildView();

// 特性
public:
	//CFile * myTNTFile_CFile;
	
private:
	CPalette TAPalette_CPal;
	HBITMAP myBitmap_H;
// 操作
public:
	TNTtoMiniMap * myTNTMiniMap_PTNTMM;
// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

private:
	CBitmap * MyView_CBitmapPTR;
	LPVOID ColorsBit;
	CStringA TNTFileName_CstrA;
};

