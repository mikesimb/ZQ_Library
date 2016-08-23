
// Server_Rebuild_4Dlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Tab_ControlPanel.h"
#include "Dialog_Mysql_V1.h"


// CServer_Rebuild_4Dlg �Ի���
class CServer_Rebuild_4Dlg : public CDialogEx
{
// ����
public:
	CServer_Rebuild_4Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_REBUILD_4_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
// 	afx_msg void OnBnClickedButton1();
// 	afx_msg void OnClose();
// 	CButton m_Btn_Switch;
	//int m_Port;
// 	afx_msg void OnBnClickedCheck1();
// 	CButton m_output_clientCount;

	void InitializeTabCtrl();//���������MFC��TabCtrl�ĵ���Ӧ��
	CTabCtrl m_TabCtrl;
	

	CTab_ControlPanel m_Tab_ControlPanel;
	CDialog_Mysql_V1 m_Dialog_Mysql;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	afx_msg LRESULT OnWriteLog(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedOk2();
	afx_msg void OnDestroy();
};
