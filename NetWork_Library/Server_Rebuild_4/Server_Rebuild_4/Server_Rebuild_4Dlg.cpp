
// Server_Rebuild_4Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server_Rebuild_4.h"
#include "Server_Rebuild_4Dlg.h"
#include "afxdialogex.h"
#include "MainThread.h"
#include "Log.h"
#include "ZQString.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_WRITE_LOG WM_USER+9

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CServer_Rebuild_4Dlg 对话框



CServer_Rebuild_4Dlg::CServer_Rebuild_4Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER_REBUILD_4_DIALOG, pParent)
	//, m_Port(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	m_Port = 7100;
}

void CServer_Rebuild_4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON1, m_Btn_Switch);
	//DDX_Text(pDX, IDC_EDIT2, m_Port);
	//DDX_Control(pDX, IDC_CHECK1, m_output_clientCount);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CServer_Rebuild_4Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &CServer_Rebuild_4Dlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	//ON_BN_CLICKED(IDC_CHECK1, &CServer_Rebuild_4Dlg::OnBnClickedCheck1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CServer_Rebuild_4Dlg::OnTcnSelchangeTab1)
	ON_MESSAGE(WM_WRITE_LOG, &CServer_Rebuild_4Dlg::OnWriteLog)
	ON_BN_CLICKED(IDOK2, &CServer_Rebuild_4Dlg::OnBnClickedOk2)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CServer_Rebuild_4Dlg 消息处理程序

BOOL CServer_Rebuild_4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	CLog::getInstance()->InitializeLogFile();


	InitializeTabCtrl();

	//CLog::getInstance()->InitializeLogFile();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServer_Rebuild_4Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServer_Rebuild_4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServer_Rebuild_4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServer_Rebuild_4Dlg::InitializeTabCtrl()
{
	m_TabCtrl.InsertItem(0, "控制面板");
	m_TabCtrl.InsertItem(1, "数据库配置");

	m_Tab_ControlPanel.Create(IDD_DIALOG1, GetDlgItem(IDC_TAB1));
	m_Tab_ControlPanel.ShowWindow(true);
    m_Dialog_Mysql.Create(IDD_DIALOG3, GetDlgItem(IDC_TAB1));
	m_Dialog_Mysql.ShowWindow(FALSE);
	CRect rs;
	m_TabCtrl.GetClientRect(&rs);
	rs.top += 20;
	m_Tab_ControlPanel.MoveWindow(&rs);
	m_TabCtrl.SetCurSel(0);
}

void CServer_Rebuild_4Dlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CRect rs;

	*pResult = 0;
	switch (m_TabCtrl.GetCurSel())
	{
	case 0 :
		m_Tab_ControlPanel.ShowWindow(true);
		m_TabCtrl.GetClientRect(&rs);
		rs.top += 20;
		// 	rs.bottom -= 60;
		// 	rs.left += 1;
		// 	rs.right -= 2;

		m_Tab_ControlPanel.MoveWindow(&rs);
		m_Dialog_Mysql.ShowWindow(false);
		break;
	case 1:
		m_Dialog_Mysql.ShowWindow(true);
		m_TabCtrl.GetClientRect(&rs);
		rs.top += 20;
		// 	rs.bottom -= 60;
		// 	rs.left += 1;
		// 	rs.right -= 2;

		m_Dialog_Mysql.MoveWindow(&rs);
		m_Tab_ControlPanel.ShowWindow(false);
		break;
	default:
		break;
	}
}



afx_msg LRESULT CServer_Rebuild_4Dlg::OnWriteLog(WPARAM wParam, LPARAM lParam)
{
 	char * chr = (char*)wParam;
 	CLog::getInstance()->WriteLog(chr, lParam);
	m_Tab_ControlPanel.AddMessageToListBox(chr, lParam);
	return 0;
}


void CServer_Rebuild_4Dlg::OnBnClickedOk2()
{
 	CZQString str = "1234567890";
// 	// TODO: 在此添加控件通知处理程序代码
	char* chr = str;
	
	SendMessage(WM_WRITE_LOG, (WPARAM)chr, str.GetLength());
}


void CServer_Rebuild_4Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	CLog::FreeInstance();

	// TODO: 在此处添加消息处理程序代码
}
