
// Server_Rebuild_4Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Tab_ControlPanel.h"


// CServer_Rebuild_4Dlg 对话框
class CServer_Rebuild_4Dlg : public CDialogEx
{
// 构造
public:
	CServer_Rebuild_4Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_REBUILD_4_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
// 	afx_msg void OnBnClickedButton1();
// 	afx_msg void OnClose();

// 	afx_msg void OnBnClickedCheck1();
// 	CButton m_output_clientCount;

	void InitializeTabCtrl();//这个函数是MFC的TabCtrl的典型应用
	CTabCtrl m_TabCtrl;

	CTab_ControlPanel m_Tab_ControlPanel;
};
