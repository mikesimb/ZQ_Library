
// Server_Rebuild_4Dlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CServer_Rebuild_4Dlg �Ի���



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


// CServer_Rebuild_4Dlg ��Ϣ�������

BOOL CServer_Rebuild_4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	CLog::getInstance()->InitializeLogFile();


	InitializeTabCtrl();

	//CLog::getInstance()->InitializeLogFile();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServer_Rebuild_4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CServer_Rebuild_4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServer_Rebuild_4Dlg::InitializeTabCtrl()
{
	m_TabCtrl.InsertItem(0, "�������");
	m_TabCtrl.InsertItem(1, "���ݿ�����");

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
// 	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char* chr = str;
	
	SendMessage(WM_WRITE_LOG, (WPARAM)chr, str.GetLength());
}


void CServer_Rebuild_4Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	CLog::FreeInstance();

	// TODO: �ڴ˴������Ϣ����������
}
