
// Server_Rebuild_5.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CServer_Rebuild_5App: 
// �йش����ʵ�֣������ Server_Rebuild_5.cpp
//

class CServer_Rebuild_5App : public CWinApp
{
public:
	CServer_Rebuild_5App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CServer_Rebuild_5App theApp;