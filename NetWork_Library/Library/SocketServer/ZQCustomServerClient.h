#pragma once
#include "ZQCustomClient.h"

class CZQCustomServerClient :public CZQCustomClient
{
	friend class CZQCustomIOCPServer;
public:
	CZQCustomServerClient();
	~CZQCustomServerClient();
};

