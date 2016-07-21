#pragma once

#include "mysql.h"
#include "afxwin.h"
// CDialog_Mysql dialog

class CDialog_Mysql : public CDialogEx
{
	DECLARE_DYNAMIC(CDialog_Mysql)

public:
	CDialog_Mysql(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialog_Mysql();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_IPaddress;
	CString m_DBName;
	CString m_UserName;
	CString m_Password;
	int m_port;

	MYSQL  mysql;

	afx_msg void OnBnClickedButton1();
	CListBox m_output_info;
};
