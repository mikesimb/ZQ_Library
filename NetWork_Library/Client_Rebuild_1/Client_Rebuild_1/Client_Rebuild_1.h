
// Client_Rebuild_1.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CClient_Rebuild_1App: 
// �йش����ʵ�֣������ Client_Rebuild_1.cpp
//

class CClient_Rebuild_1App : public CWinApp
{
public:
	CClient_Rebuild_1App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CClient_Rebuild_1App theApp;