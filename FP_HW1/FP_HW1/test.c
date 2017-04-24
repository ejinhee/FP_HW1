#include <windows.h>
#include <stdio.h>
#include <tchar.h>

int _tmain(int argc, TCHAR *argv[])
{
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

	if (argc != 3) {
		printf("more argument needed");
		return -1;
	}

	source_file = CreateDirectory(argv[1], NULL); // open source file
		                  // no attr. template

	if (source_file == INVALID_HANDLE_VALUE) // not existing
	{
		printf("Could not open One.txt.z");

		return;
	}

	/*
	target_file = CreateFile(argv[2], // open existing Target File
		FILE_APPEND_DATA,         // open for writing
		FILE_SHARE_READ,          // allow multiple readers
		NULL,                     // no security
		OPEN_EXISTING,              // open or create
		FILE_ATTRIBUTE_NORMAL,    // normal file
		NULL);                    // no attr. template
	if (target_file == INVALID_HANDLE_VALUE) // 
	{
		target_file = CreateFile(argv[2], // Creating a new File
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
		printf("Could not open target File time.");
		return;
	}
	boolean m_time_detected_error = GetFileTime(target_file, &t_creationTime, &t_lpLastAccessTime, &t_lastWriteTIme);
	if (!m_time_detected_error) {
		printf("Could not open target File time.");
		return;
	}


	//if (GetFileSize(source_file, NULL) != GetFileSize(target_file, NULL)) {
	if (CompareFileTime(&m_lastWriteTIme, &t_lastWriteTIme) == 1 || isNew) { // source file 의 write 시간이 더 최근 or target file이 새로 만들어진 경우
		isNew = FALSE;
		CloseHandle(target_file);

		DeleteFile(argv[2]); //

		target_file = CreateFile(argv[2], // open target file
			FILE_APPEND_DATA,         // open for writing
			FILE_SHARE_READ,          // allow multiple readers
			NULL,                     // no security
			OPEN_ALWAYS,              // open or create
			FILE_ATTRIBUTE_NORMAL,    // normal file
			NULL);                    // no attr. template

		if (target_file == INVALID_HANDLE_VALUE)
		{
			printf("Could not open Two.txt.");
			return;
		}
		printf("file backup is completed.");
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
		printf("file is not updated");

	// Append the first file to the end of the second file.
	// Lock the second file to prevent another process from
	// accessing it while writing to it. Unlock the
	// file when writing is complete.



	// Close both files.
	*/
	CloseHandle(source_file);
//	CloseHandle(target_file);
	return 0;
}
