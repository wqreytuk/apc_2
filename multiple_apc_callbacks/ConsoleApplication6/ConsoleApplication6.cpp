#include <windows.h>
#include <stdio.h>
void test() {
	printf("test\n");
}
void Papcfunc(
	ULONG_PTR Parameter
)
{
	wprintf(L"[%u] APC callback has fired with param=%Id\n", ::GetCurrentThreadId(), Parameter);
}BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
DWORD WINAPI ThreadProc(
	_In_ LPVOID lpParameter
)
{
	wprintf(L"[%u] Thread has started\n", ::GetCurrentThreadId());
	
	for (;;)
	{
		DWORD dwR = ::SleepEx(INFINITE, TRUE);
		wprintf(L"SleepEx returned %d\n", dwR);
	}

	return 0;
}
int main()
{
	HANDLE hThread = ::CreateThread(NULL, 0, ThreadProc, 0, 0, NULL);
	if (hThread)
	{
		::Sleep(1000);

		for (int q = 0; q < 1000; q++)
		{
			if (!::QueueUserAPC((PAPCFUNC)Papcfunc, hThread, q))
			{
				wprintf(L"ERROR: (%d) QueueUserAPC with value q=%d\n", ::GetLastError(), q);
				break;
			}
			::Sleep(1);
		}
		::Sleep(1000 * 1000);
		::CloseHandle(hThread);
	}
	else
		wprintf(L"ERROR: (%d) CreateThread\n", ::GetLastError());
}