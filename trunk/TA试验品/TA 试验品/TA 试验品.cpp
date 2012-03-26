
// TA 试验品.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "TA 试验品.h"
#include "ChildFrm.h"
#include "MainFrm.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTA试验品App

BEGIN_MESSAGE_MAP(CTA试验品App, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CTA试验品App::OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN,&CTA试验品App::OnFILEOPEN)
END_MESSAGE_MAP()
#define ON_COMMAND(id, memberFxn) { WM_COMMAND, CN_COMMAND, (WORD)id, (WORD)id, AfxSigCmd_v, static_cast<AFX_PMSG> (memberFxn) },

// CTA试验品App 构造

CTA试验品App::CTA试验品App()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("TA 试验品.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CTA试验品App 对象

CTA试验品App theApp;


// CTA试验品App 初始化

BOOL CTA试验品App::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMDIFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建主 MDI 框架窗口
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// 试图加载共享 MDI 菜单和快捷键表
	//TODO: 添加附加成员变量，并加载对
	//	应用程序可能需要的附加菜单类型的调用
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_TATYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_TATYPE));


	// 主窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}

int CTA试验品App::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinAppEx::ExitInstance();
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令//
void CTA试验品App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CTA试验品App 自定义加载/保存方法 //

void CTA试验品App::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CTA试验品App::LoadCustomState()
{
}

void CTA试验品App::SaveCustomState()
{
}

// CTA试验品App 消息处理程序//



void CTA试验品App::OnFILEOPEN()
{
	// TODO: 在此添加命令处理程序代码//

	//, _T(""), _T("[S] BOW v2.tnt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("*.tnt;")
	CFileDialog GetTNTFile_OpenDialog(TRUE, _T("tnt"), _T("*.tnt"),
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, _T("TNT Map(*.tnt)|*.tnt||"));
	CString FileName_Cstr;
	GetTNTFile_OpenDialog.GetOFN().lpstrFile = FileName_Cstr.GetBuffer ( MAX_PATH);
	GetTNTFile_OpenDialog.GetOFN().nMaxFile = MAX_PATH;
	GetTNTFile_OpenDialog.DoModal ( );
	FileName_Cstr.ReleaseBuffer();
	if (! FileName_Cstr.IsEmpty())
	{
		CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
		// 创建新的 MDI 子窗口//
		CChildFrame * myChildFrame_PCCF= pFrame->CreateNewChildWithTNTFilePtr(
			RUNTIME_CLASS(CChildFrame), IDR_TATYPE, FileName_Cstr, m_hMDIMenu, m_hMDIAccel);
		//FileName_Cstr.ReleaseBuffer();
		//myChildFrame_PCCF->
	}

 }

// CTA试验品App 消息处理程序
/*

void CTA试验品App::OnFileNew() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	// 创建新的 MDI 子窗口
	pFrame->CreateNewChild(
		RUNTIME_CLASS(CChildFrame), IDR_TATYPE, m_hMDIMenu, m_hMDIAccel);
}*/
