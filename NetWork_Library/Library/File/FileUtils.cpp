#include "stdafx.h"
#include "FileUtils.h"


#define faInvalid    -1
#define faReadOnly   0x0000001
#define faHidden     0x00000002  // only a convention on POSIX
#define faSysFile    0x00000004  // on POSIX system files are not regular files and not directories
#define faVolumeID   0x00000008  // not used in Win32
#define faDirectory  0x00000010
#define faArchive    0x00000020 
#define faNormal     0x00000080
#define faTemporary  0x00000100
#define faSymLink	 0x00000400 // Available on POSIX and Vista and above
#define faCompressed 0x00000800 
#define faEncrypted  0x00004000 
#define faVirtual    0x00010000
#define faAnyFile    0x000001FF;

bool FileExists(char* FileName)
{
	int Flags = GetFileAttributes(FileName);
	if (Flags != INVALID_FILE_ATTRIBUTES)
	{
		return (Flags & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}
#ifdef _DEBUG
	unsigned long lastError = GetLastError();
	char Outputinfo[255];
	sprintf_s(Outputinfo, 255, "GetlastErrorCode is : %d", lastError);
	OutputDebugString(Outputinfo);
#endif
	return false;

}

