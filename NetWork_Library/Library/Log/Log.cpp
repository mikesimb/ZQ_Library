#include "stdafx.h"
#include "Log.h"


static CLog* g_log;
CLog::CLog()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_szFileName = new char[MAX_PATH];
}


CLog::~CLog()
{
	delete m_szFileName;
}

CLog* CLog::getInstance()
{
	if (g_log == NULL)
		g_log = new CLog();
	return g_log;
}

void CLog::FreeInstance()
{
	if (g_log != NULL)
		delete g_log;
}

void CLog::InitializeLogFile()
{
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	//����Ҫ��ȡ����ǰ��ģ���ļ�·��
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_splitpath_s(path, drive, dir, fname, ext);
	char curpath[MAX_PATH];
	sprintf_s(curpath, MAX_PATH, "%s%s", drive, dir);
	SetCurrentDirectory(curpath);
	char filename[MAX_PATH];
	//�ļ�����һ��������Ϊ���ĵ��ļ���
	time_t rawtime;
	struct tm* timeinfo =new tm;
	char timE[MAX_PATH];
	time(&rawtime);
	localtime_s(timeinfo,&rawtime);
	strftime(timE, 80, "Log%Y-%m-%d.log", timeinfo);
	sprintf_s(filename, MAX_PATH, "%s%s", curpath, timE);

	setLogFilename(filename);
	
	OpenFile();
	delete timeinfo;
}

void CLog::setLogFilename(char* pFilename)
{
	memcpy_s(m_szFileName, MAX_PATH, pFilename, MAX_PATH);
}

bool CLog::OpenFile()
{
  if (FileIsOpen())
			return true;

		if (!m_szFileName)
			return false;

		m_hFile = CreateFile(
			m_szFileName,
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if (!FileIsOpen() && GetLastError() == 2)//�򿪲��ɹ�������Ϊ�ļ������ڣ� �����ļ�
			m_hFile = CreateFile(
				m_szFileName,
				GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);

		if (FileIsOpen())
			SetFilePointer(m_hFile, 0, NULL, FILE_END);

		return FileIsOpen();
}

bool CLog::FileIsOpen()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		return true;
	}
	else
		return false;
}

void CLog::WriteLog(LPVOID lpBuffer, DWORD dwLength)
{
	time_t now;
	if (FileIsOpen())
	{
		struct tm* timeinfo =new tm;
		char timE[80];
		time(&now);
		localtime_s(timeinfo,&now);
		strftime(timE, 80, "%Y-%m-%d %I:%M:%S", timeinfo);
		//����ȷ��LOG�ĸ�ʽ ��ʱ�䡿����־�ȼ�����־����
		//�������޶�ÿ����־�ĳ��Ȳ��ܴ���255;
		char strLevel[10] = "Warning";
		char Buffer[255];
		ZeroMemory(Buffer, 255);
		dwLength =sprintf_s(Buffer, 255, "[%s]  [%s]  %s\r\n ",timE,strLevel,lpBuffer);
		WriteFile(m_hFile, Buffer, dwLength, &dwLength, NULL);
		FlushFileBuffers(m_hFile);
		delete timeinfo;
	}
}

