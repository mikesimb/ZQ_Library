// Dialog_Mysql_V1.cpp : implementation file
//

#include "stdafx.h"
#include "Server_Rebuild_4.h"
#include "Dialog_Mysql_V1.h"
#include "afxdialogex.h"



// CDialog_Mysql_V1 dialog

IMPLEMENT_DYNAMIC(CDialog_Mysql_V1, CDialogEx)

CDialog_Mysql_V1::CDialog_Mysql_V1(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG3, pParent)
	, m_IPAddress(_T(""))
	, m_Port(0)
	, m_DBName(_T(""))
	, m_Username(_T(""))
	, m_password(_T(""))
{
	m_IPAddress = G_Mysql_IPaddress;
	m_Port = G_Mysql_port;
	m_Username = G_Mysql_UserName;
	m_password = G_Mysql_Password;
	mysql = mysql_init(mysql);
	m_DBName = "mirdemos";

}

CDialog_Mysql_V1::~CDialog_Mysql_V1()
{
}

void CDialog_Mysql_V1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_IPAddress);
	DDX_Text(pDX, IDC_EDIT4, m_Port);
	DDX_Text(pDX, IDC_EDIT6, m_DBName);
	DDX_Text(pDX, IDC_EDIT5, m_Username);
	DDX_Text(pDX, IDC_EDIT3, m_password);
	DDX_Control(pDX, IDC_LIST2, m_output_info);
}


BEGIN_MESSAGE_MAP(CDialog_Mysql_V1, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDialog_Mysql_V1::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDialog_Mysql_V1 message handlers


void CDialog_Mysql_V1::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	MYSQL * Handle = mysql_real_connect(mysql, m_IPAddress, m_Username, m_password, NULL, m_Port, 0, 0);
	if (Handle)
	{
		m_output_info.AddString("连接数据库成功");
        bool isExists = mysql_select_db(mysql, m_DBName);
			if (!isExists)
			{
				//这里代表数据库表存在
				m_output_info.AddString("数据库存在");

			}
			else
			{
				m_output_info.AddString("数据库不存在");
			}

	}
	else
	{
		
		m_output_info.AddString("连接数据库失败");
		CString errorcode = "ErrorCode:%s\n";
		errorcode.Format(mysql_error(mysql));
		OutputDebugString(errorcode);
	}
	

	//接下来我们需要检查DB是否存在
	
}
