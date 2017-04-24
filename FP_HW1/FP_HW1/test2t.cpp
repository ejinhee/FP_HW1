#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")
#define BUFSIZE 4096
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

void DisplayErrorBox(LPTSTR lpszFunction);
void recMovFiles(WIN32_FIND_DATA Fp, LARGE_INTEGER file_size, HANDLE hFindRec, TCHAR *targetFileN, TCHAR *destFileN);
void MoveFileWithTComp(TCHAR *target, TCHAR *dest, TCHAR *fileName);
void MoveDir(WCHAR *target);

TCHAR *targetFile;
TCHAR *destinationFile;
FILE *logfile;

int _tmain(int argc, TCHAR *argv[])
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	targetFile = argv[1];
	destinationFile = argv[2];
	
	fopen_s(&logfile , "backupLog.txt", "w");

	if (argc != 3)
	{
		_tprintf(TEXT("need 2 arguments"), argv[0]);
		return (-1);
	}

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	StringCchLength(argv[1], MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		_tprintf(TEXT("\nDirectory path is too long.\n"));
		return (-1);
	}

	_ftprintf(logfile,TEXT("\nTarget directory is %s\n\n"), argv[1]);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopy(szDir, MAX_PATH, argv[1]);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);
	FindNextFile(hFind, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
		return dwError;
	}

	// List all the files in the directory with some info about them.
	filesize.LowPart = ffd.nFileSizeLow;
	filesize.HighPart = ffd.nFileSizeHigh;
	//init filesize
	recMovFiles(ffd, filesize, hFind, targetFile, destinationFile);


	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
	}

	FindClose(hFind);
	fclose(logfile);
	return dwError;
}


void DisplayErrorBox(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and clean up

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void MoveFileWithTComp(TCHAR *target, TCHAR *dest, TCHAR *fileName) {
	HANDLE source_file;
	HANDLE target_file;
	DWORD  dwBytesRead, dwBytesWritten, dwPos;
	BYTE   buff[4096];


	FILETIME m_creationTime;
	FILETIME m_lpLastAccessTime;
	FILETIME m_lastWriteTIme;

	FILETIME t_creationTime;
	FILETIME t_lpLastAccessTime;
	FILETIME t_lastWriteTIme;

	boolean isNew = FALSE;
	// Open the existing file.

	TCHAR NewDestination[MAX_PATH];
	StringCchCopy(NewDestination, MAX_PATH, dest);
	StringCchCat(NewDestination, MAX_PATH, fileName);

	source_file = CreateFile(target, // open source file
		GENERIC_READ,             // open for reading
		0,                        // do not share
		NULL,                     // no security
		OPEN_EXISTING,            // existing file only
		FILE_ATTRIBUTE_NORMAL,    // normal file
		NULL);                    // no attr. template

	if (source_file == INVALID_HANDLE_VALUE) // not existing
	{
		_tprintf(TEXT(" could not open %s\n"), target);

		return;
	}
	target_file = CreateFile(NewDestination, // open existing Target File
		FILE_APPEND_DATA,         // open for writing
		FILE_SHARE_READ,          // allow multiple readers
		NULL,                     // no security
		OPEN_EXISTING,              // open or create
		FILE_ATTRIBUTE_NORMAL,    // normal file
		NULL);                    // no attr. template
	if (target_file == INVALID_HANDLE_VALUE) // 
	{
		target_file = CreateFile(NewDestination, // Creating a new File
			FILE_APPEND_DATA,
			FILE_SHARE_READ,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		isNew = TRUE;
	}
	boolean t_time_detected_error = GetFileTime(source_file, &m_creationTime, &m_lpLastAccessTime, &m_lastWriteTIme);
	if (!t_time_detected_error) {
		printf("Could not open target File time.\n");
		return;
	}
	boolean m_time_detected_error = GetFileTime(target_file, &t_creationTime, &t_lpLastAccessTime, &t_lastWriteTIme);
	if (!m_time_detected_error) {
		printf("Could not open destination File time.\n");
		return;
	}


	//if (GetFileSize(source_file, NULL) != GetFileSize(target_file, NULL)) {
	if (CompareFileTime(&m_lastWriteTIme, &t_lastWriteTIme) == 1 || isNew) { // source file 의 write 시간이 더 최근 or target file이 새로 만들어진 경우
		isNew = FALSE;
		CloseHandle(target_file);

		DeleteFile(NewDestination); //

		target_file = CreateFile(NewDestination, // open target file
			FILE_APPEND_DATA,         // open for writing
			FILE_SHARE_READ,          // allow multiple readers
			NULL,                     // no security
			OPEN_ALWAYS,              // open or create
			FILE_ATTRIBUTE_NORMAL,    // normal file
			NULL);                    // no attr. template

		if (target_file == INVALID_HANDLE_VALUE)
		{
			_tprintf(TEXT(" could not open %s\n"), NewDestination);
			return;
		}
		printf("file backup is completed.\n");
		while (ReadFile(source_file, buff, sizeof(buff), &dwBytesRead, NULL)
			&& dwBytesRead > 0)
		{
			dwPos = SetFilePointer(target_file, 0, NULL, FILE_BEGIN);
			LockFile(target_file, dwPos, 0, dwBytesRead, 0);
			WriteFile(target_file, buff, dwBytesRead, &dwBytesWritten, NULL);
			UnlockFile(target_file, dwPos, 0, dwBytesRead, 0);
		}

	}
	else
		printf("file is not updated\n");

	// Append the first file to the end of the second file.
	// Lock the second file to prevent another process from
	// accessing it while writing to it. Unlock the
	// file when writing is complete.



	// Close both files.

	CloseHandle(source_file);
	CloseHandle(target_file);
}

void recMovFiles(WIN32_FIND_DATA Fp, LARGE_INTEGER file_size, HANDLE hFindRec, TCHAR *targetFileN, TCHAR *destFileN) {
	TCHAR Dir[MAX_PATH];
	TCHAR destDir[MAX_PATH];
	WIN32_FIND_DATA FpIN;
	HANDLE hFindGo;
	size_t length_of_arg;

	_ftprintf(logfile ,TEXT("now we in :::::::::: %s DIR:::\n"),destFileN);

	file_size.LowPart = Fp.nFileSizeLow;
	file_size.HighPart = Fp.nFileSizeHigh;

	if (FindNextFile(hFindRec, &Fp) == 0) {
		return;
	}

	if (Fp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{

		if (Fp.cFileName == TEXT(".")) {
			return;
		}
		else if (Fp.cFileName == TEXT("..")) {
			return;
		}
		else {
			// 디렉토리인 경우.
			TCHAR *nextDir = new TCHAR[MAX_PATH];
			_ftprintf(logfile, TEXT("********MOVE FROM  %s   <DIR>\n"), Fp.cFileName);


			StringCchLength(Fp.cFileName, MAX_PATH, &length_of_arg);
			StringCchCopy(Dir, MAX_PATH, targetFileN);
			StringCchCat(Dir, MAX_PATH, TEXT("\\"));
			StringCchCat(Dir, MAX_PATH, Fp.cFileName);

			StringCchCopy(nextDir, MAX_PATH, targetFileN);
			StringCchCat(nextDir, MAX_PATH, TEXT("\\"));
			StringCchCat(nextDir, MAX_PATH, Fp.cFileName);

			StringCchCopy(destDir, MAX_PATH, destFileN);
			StringCchCat(destDir, MAX_PATH, TEXT("\\"));
			StringCchCat(destDir, MAX_PATH, Fp.cFileName);
			CreateDirectory(destDir, NULL);

			_ftprintf(logfile, TEXT("********MOVE TO    %s   <DIR>\n"), destDir);

			StringCchCat(Dir, MAX_PATH, TEXT("\\*"));

			hFindGo = FindFirstFile(Dir, &FpIN);
			FindNextFile(hFindGo, &Fp);


			recMovFiles(FpIN, file_size, hFindGo, nextDir, destDir);
		}
	}
	else
	{
		TCHAR targetFileD[MAX_PATH];

		StringCchLength(Fp.cFileName, MAX_PATH, &length_of_arg);
		StringCchCopy(Dir, MAX_PATH, targetFileN);
		StringCchCat(Dir, MAX_PATH, TEXT("\\"));
		StringCchCat(Dir, MAX_PATH, Fp.cFileName);

		StringCchCopy(targetFileD, MAX_PATH, destFileN);
		StringCchCat(targetFileD, MAX_PATH, TEXT("\\"));

		_ftprintf(logfile, TEXT("********MOVE FROM  %s   %ld bytes\n"), Fp.cFileName, file_size.QuadPart);
		_ftprintf(logfile, TEXT("********MOVE TO    %s   %ld bytes\n"), targetFileD, file_size.QuadPart);

		// 파일인 경우.
		file_size.LowPart = Fp.nFileSizeLow;
		file_size.HighPart = Fp.nFileSizeHigh;
		MoveFileWithTComp(Dir, targetFileD, Fp.cFileName);
	}

	recMovFiles(Fp, file_size, hFindRec, targetFileN, destFileN);
}