#pragma once

#define GD_SERVICE_NAME_SHORT L"STSvc"
#define GD_SERVICE_NAME_LONG L"Smile for Tomorrow Service"
#define GD_SERVICE_DESCRIPTION L"Service for assistant application"

class ClaService
{
public:
	ClaService() {}
	~ClaService() {}

public:
	unsigned long run();
};

