
// Server_Rebuild_4.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CServer_Rebuild_4App: 
// �йش����ʵ�֣������ Server_Rebuild_4.cpp
//

class CServer_Rebuild_4App : public CWinApp
{
public:
	CServer_Rebuild_4App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CServer_Rebuild_4App theApp;