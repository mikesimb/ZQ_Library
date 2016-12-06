#include "stdafx.h"
#include "IOCPTypeDef.h"

bool WSAStartupSocket()
{
	WSADATA  data;
	if (WSAStartup(0x0202, &data) != 0)
		return false;
		//这里创建失败了
	else
		return true;

}

bool WSACleanupSocket()
{
	if (WSACleanup() != 0)
		return false;
	else
		return true;
}

int inet_ntop4(const unsigned char *src, char *dst, size_t size)
{
	static const char fmt[] = "%u.%u.%u.%u";
	char tmp[16];
	int l;

#ifndef _WIN32
	l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
#else
	l = _snprintf_s(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
#endif
	if (l <= 0 || (size_t)l >= size) {
		return INADDR_NONE;
	}
	memcpy(dst, tmp, size);
	//strncpy(dst, tmp, size);
	dst[size - 1] = '\0';
	return 0;
}
CZQString intToStr(int value)
{
	char buf[10];
	_itoa_s(value, buf, 10);
	CZQString ret;
	ret += buf;
	return ret;
}

int strToInt(CZQString s)
{
	return atoi(s);
}
