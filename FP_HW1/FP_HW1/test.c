#include <windows.h>
#include <stdio.h>
#include <tchar.h>

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hFile;
	HANDLE hAppend;
	DWORD  dwBytesRead, dwBytesWritten, dwPos;
	BYTE   buff[4096];

	// Open the existing file.

	if (argc != 3) {
		printf("more argument needed");
		return -1;
	}

	hFile = CreateFile(argv[1], // open One.txt
		GENERIC_READ,             // open for reading
		0,                        // do not share
		NULL,                     // no security
		OPEN_EXISTING,            // existing file only
		FILE_ATTRIBUTE_NORMAL,    // normal file
		NULL);                    // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Could not open One.txt.");
		
		return;
	}

	// Open the existing file, or if the file does not exist,
	// create a new file
	char s[] = "to.txt";


	if (_unlink(s)) {
		switch (errno) {
		case EACCES:
			fprintf(stderr, "Access is denied: %s\n", s); break;
		case ENOENT:
			fprintf(stderr, "Could not find: %s\n", s); break;
		default:
			fprintf(stderr, "Unknown error.\n"); break;
		}
	}
	else
		printf("Deleted: %s\n", s);

	hAppend = CreateFile(argv[2], // open Two.txt
		FILE_APPEND_DATA,         // open for writing
		FILE_SHARE_READ,          // allow multiple readers
		NULL,                     // no security
		OPEN_ALWAYS,              // open or create
		FILE_ATTRIBUTE_NORMAL,    // normal file
		NULL);                    // no attr. template

	if (hAppend == INVALID_HANDLE_VALUE)
	{
		printf("Could not open Two.txt.");
		return;
	}

	// Append the first file to the end of the second file.
	// Lock the second file to prevent another process from
	// accessing it while writing to it. Unlock the
	// file when writing is complete.

	while (ReadFile(hFile, buff, sizeof(buff), &dwBytesRead, NULL)
		&& dwBytesRead > 0)
	{
		dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END);
		LockFile(hAppend, dwPos, 0, dwBytesRead, 0);
		WriteFile(hAppend, buff, dwBytesRead, &dwBytesWritten, NULL);
		UnlockFile(hAppend, dwPos, 0, dwBytesRead, 0);
	}

	// Close both files.

	CloseHandle(hFile);
	CloseHandle(hAppend);

	return 0;
}
