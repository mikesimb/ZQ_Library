#pragma once
#include "afxwin.h"


// CTab_ControlPanel dialog

class CTab_ControlPanel : public CDialogEx
{
	DECLARE_DYNAMIC(CTab_ControlPanel)

public:
	CTab_ControlPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTab_ControlPanel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	
	int m_port;
	CButton m_Btn_Switch;
};
