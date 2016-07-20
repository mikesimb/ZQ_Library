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
	, m_isOutput_ClientCount(FALSE)
	, m_port(0)
{
	m_port = 7100;
	
}

CTab_ControlPanel::~CTab_ControlPanel()
{
}

void CTab_ControlPanel::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ 
	// �˴��������´���ͺ��ˣ� 
// 	switch ()
// 	{
// 	case 1:
// 		//������������ 
// 		//������Ҫ��ʱ���ڴ˴�����KillTimer(1); 
// 		break;
// 	default:
// 		break;
// 	}
	
	CDialog::OnTimer(nIDEvent);
	switch (nIDEvent)
	{ 
	case 1:
			char info[20];
			sprintf_s(info, 20, "client count is :%d", CMainThread::getInstance()->getServerClientCount());
			m_listBox.AddString(info);
			break;
	default:
			break;
	
	}
}

void CTab_ControlPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_Btn_Switch);
	DDX_Check(pDX, IDC_CHECK1, m_isOutput_ClientCount);
	DDX_Text(pDX, IDC_EDIT2, m_port);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
}


BEGIN_MESSAGE_MAP(CTab_ControlPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CTab_ControlPanel::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK1, &CTab_ControlPanel::OnBnClickedCheck1)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTab_ControlPanel message handlers


void CTab_ControlPanel::OnBnClickedButton1()
{
// 	TODO: Add your control notification handler code here
 	// TODO: �ڴ���ӿؼ�֪ͨ����������
 	  if(!CMainThread::getInstance()->isRunning())
 	  {
 		CMainThread::getInstance()->Initialize(false);
 		CMainThread::getInstance()->setPort(m_port);
 		CMainThread::getInstance()->Resume();
 		m_Btn_Switch.SetWindowText("ֹͣ����");
 	  }else
 	  { 
 		  CMainThread::getInstance()->Terminate();
 		  WaitForSingleObject(CMainThread::getInstance()->getThread(),3000);
 		  delete(CMainThread::getInstance());
 		  m_Btn_Switch.SetWindowText("��������");
 
 
 	  }
}


void CTab_ControlPanel::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	m_isOutput_ClientCount = !m_isOutput_ClientCount;

	if (m_isOutput_ClientCount)
	{
		m_listBox.AddString("���outputClientCount");
		SetTimer(1, 1000, NULL);
	}
		//m_listBox.InsertString(m_listBox.GetCount(),"���outputClientCount");
	
	else
		m_listBox.AddString("�����outputClientCount");
		//m_listBox.InsertString(m_listBox.GetCount(),"�����outputClientCount");
}
