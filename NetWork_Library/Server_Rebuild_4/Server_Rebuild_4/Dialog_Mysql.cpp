// Dialog_Mysql.cpp : implementation file
//

#include "stdafx.h"
#include "Server_Rebuild_4.h"
#include "Dialog_Mysql.h"
#include "afxdialogex.h"
#include "Mysql_Common.h"



// CDialog_Mysql dialog

IMPLEMENT_DYNAMIC(CDialog_Mysql, CDialogEx)

CDialog_Mysql::CDialog_Mysql(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, m_IPaddress(_T(""))
	, m_DBName(_T(""))
	, m_UserName(_T(""))
	, m_Password(_T(""))
	, m_port(0)
{
	m_IPaddress = G_Mysql_IPaddress;
	m_port = G_Mysql_port;
	m_UserName = G_Mysql_UserName;
	m_Password = G_Mysql_Password;
	MYSQL* A = mysql_init(&mysql);


}

CDialog_Mysql::~CDialog_Mysql()
{
}

void CDialog_Mysql::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_IPaddress);
	DDX_Text(pDX, IDC_EDIT5, m_DBName);
	DDX_Text(pDX, IDC_EDIT2, m_UserName);
	DDX_Text(pDX, IDC_EDIT3, m_Password);
	DDX_Text(pDX, IDC_EDIT4, m_port);
	DDX_Control(pDX, IDC_LIST1, m_output_info);
}


BEGIN_MESSAGE_MAP(CDialog_Mysql, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDialog_Mysql::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDialog_Mysql message handlers


void CDialog_Mysql::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	bool isConnect = mysql_real_connect(&mysql, m_IPaddress, m_UserName, m_Password, NULL, m_port,0,0);
	if (isConnect)
	{
		m_output_info.AddString("连接数据库成功");
	}
	//接下来我们需要检查DB是否存在
	bool isExistDB = mysql_select_db(&mysql, m_DBName);
	if (!isExistDB)
	{
		m_output_info.AddString("数据库不存在");
	}

}
