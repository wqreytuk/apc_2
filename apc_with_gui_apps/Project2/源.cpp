#if       _WIN32_WINNT < 0x0500
  #undef  _WIN32_WINNT
  #define _WIN32_WINNT   0x0500
#endif

#define _UNICODE
#define UNICODE

#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <cassert>
#define WM_DISKCHANGE   WM_USER+123

using namespace std;

BOOL FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
DWORD WINAPI message_notifier(PVOID p) {
    while (1) {
        if (FileExists(TEXT("C:\\1.txt"))) {

            SendMessageW(*(HWND*)p, WM_DISKCHANGE, 0, 0);
            break;
        }
    }
    return 0;
}

void Papcfunc(
    ULONG_PTR Parameter
)
{
    HWND hWnd = (HWND)Parameter;
    ::MessageBox(hWnd, L"APC callback fired OK", L"Success", MB_ICONINFORMATION);
}

void set_test_APC(HWND hWnd)
{
    ::MessageBox(hWnd, L"set_test_APC called", L"ERROR", MB_ICONERROR);

    if (!::QueueUserAPC((PAPCFUNC)Papcfunc, ::GetCurrentThread(), (ULONG_PTR)hWnd))
    {
        ::MessageBox(hWnd, L"ERROR: QueueUserAPC failed", L"ERROR", MB_ICONERROR);
    }
}


LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);
void test_function();
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow){

    HWND hWnd;

    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"WindowClass1";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(NULL,
                          L"WindowCLass1",
                          L"Window Program",
                          WS_OVERLAPPEDWINDOW,
                          300,
                          300,
                          500,
                          400,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

    //ShowWindow( GetConsoleWindow(), SW_HIDE );

    // 创建一个线程用于向窗口发送消息
    DWORD id = 0;
    HANDLE thread_handle = CreateThread(NULL, 0, message_notifier, (PVOID)&hWnd, 0, &id);
    for (;;)
    {
        // dwWakeMask: QS_ALLINPUT
        // QS_ALLINPUT的意思是Any message is in the queue
        // 结合第五个参数dwFlags中的MWMO_INPUTAVAILABLE
        // MWMO_INPUTAVAILABLE意味着如果在队列中存在输入类型的句柄，该函数就会返回
        // 而输入类型就是由dwWakeMask指定的，那么QS_ALLINPUT就会导致该函数接收到任何消息后都会返回
        // MWMO_ALERTABLE表示线程进入alertable状态，在APC调用完成后也会导致该函数立即返回
        DWORD dwR = ::MsgWaitForMultipleObjectsEx(0, NULL, INFINITE, QS_ALLINPUT, MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);
        if (dwR == WAIT_FAILED)
        {
            //Error
            assert(false);
            break;
        }
        MSG msg;

        BOOL bRet;
        // 原来这个地方写的是GetMessage，之所以换成PeekMessage是因为后者并不会阻塞
        // 他只会看一下消息队列，有消息就取出来并从队列中删除该消息，如果没有消息就直接返回
        // 该函数的返回值是BOOL，有消息就是TRUE，没有消息就是FALSE
        // 之所以选择不阻塞的PeekMessage函数，是因为上面还有一层循环
        // 要重复执行MsgWaitForMultipleObjectsEx函数来使当前线程进入alertable状态
        // 因为有消息进来的话MsgWaitForMultipleObjectsEx也会返回，否则的话当前线程就会一直阻塞在MsgWaitForMultipleObjectsEx函数
        // 这样将导致set_test_APC函数也无法执行
        while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    }
    return 0;

}

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam)
                            {
                                switch(message)
                                {
                                case WM_DESTROY:
                                    {
                                        //ShowWindow( GetConsoleWindow(), SW_RESTORE );
                                        // test_function();
                                        // set_test_APC(hWnd);
                                        // ::Sleep(1000 * 1000);    // becomes redundant

                                    ::MessageBox(hWnd, L"Destroyed?", L"ERROR", MB_ICONERROR);
                                        return 0;
                                    }break;
                                case WM_DISKCHANGE:
                                {
                                    // 接收线程中发来的消息，插入APC
                                    set_test_APC(hWnd);
                                    return 0;
                                }
                                break;
                                
                                }

                                return DefWindowProc  (hWnd, message, wParam, lParam);
                            }

void test_function() {
	MessageBox(NULL, TEXT("whoami"), TEXT(""), 0);
}