#pragma once

#include "Mysql.h"
#include "Mysql_Common.h"
#include "afxwin.h"
// CDialog_Mysql_V1 dialog

class CDialog_Mysql_V1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDialog_Mysql_V1)

public:
	CDialog_Mysql_V1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialog_Mysql_V1();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_IPAddress;
	int m_Port;
	CString m_DBName;
	CString m_Username;

	CString m_password;

	MYSQL*  mysql;
	CListBox m_output_info;
	afx_msg void OnBnClickedButton1();
};
