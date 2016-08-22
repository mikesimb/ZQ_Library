// Tab_ControlPanel.cpp : implementation file
//

#include "stdafx.h"
#include "Server_Rebuild_4.h"
#include "Tab_ControlPanel.h"
#include "afxdialogex.h"
#include "MainThread.h"

// CTab_ControlPanel dialog

IMPLEMENT_DYNAMIC(CTab_ControlPanel, CDialogEx)

CTab_ControlPanel::CTab_ControlPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_port(0)
{
	m_port = 7100;
}

CTab_ControlPanel::~CTab_ControlPanel()
{
}

void CTab_ControlPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_port);
	DDX_Control(pDX, IDC_BUTTON1, m_Btn_Switch);
}


BEGIN_MESSAGE_MAP(CTab_ControlPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CTab_ControlPanel::OnBnClickedButton1)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTab_ControlPanel message handlers


void CTab_ControlPanel::OnBnClickedButton1()
{
    //TODO: Add your control notification handler code here
	  if(!CMainThread::getInstance()->isRunning())
	  {
		CMainThread::getInstance()->Initialize(false);
		CMainThread::getInstance()->setPort(m_port);
		CMainThread::getInstance()->Resume();
		m_Btn_Switch.SetWindowText("ֹͣ����");
	  }
	  else
	  {
		  CMainThread::getInstance()->Terminate();
		  WaitForSingleObject(CMainThread::getInstance()->getThread(), 3000);
		  delete(CMainThread::getInstance());
		  m_Btn_Switch.SetWindowText("��������");
	  }
}


void CTab_ControlPanel::OnDestroy()
{
	CDialogEx::OnDestroy();
	if (CMainThread::getInstance()->isRunning())
	{
		CMainThread::getInstance()->Terminate();
		WaitForSingleObject(CMainThread::getInstance()->getThread(), 3000);
		delete(CMainThread::getInstance());
	}

	// TODO: �ڴ˴������Ϣ����������
}
