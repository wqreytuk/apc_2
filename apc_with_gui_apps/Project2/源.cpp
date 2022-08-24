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

    // ����һ���߳������򴰿ڷ�����Ϣ
    DWORD id = 0;
    HANDLE thread_handle = CreateThread(NULL, 0, message_notifier, (PVOID)&hWnd, 0, &id);
    for (;;)
    {
        // dwWakeMask: QS_ALLINPUT
        // QS_ALLINPUT����˼��Any message is in the queue
        // ��ϵ��������dwFlags�е�MWMO_INPUTAVAILABLE
        // MWMO_INPUTAVAILABLE��ζ������ڶ����д����������͵ľ�����ú����ͻ᷵��
        // ���������;�����dwWakeMaskָ���ģ���ôQS_ALLINPUT�ͻᵼ�¸ú������յ��κ���Ϣ�󶼻᷵��
        // MWMO_ALERTABLE��ʾ�߳̽���alertable״̬����APC������ɺ�Ҳ�ᵼ�¸ú�����������
        DWORD dwR = ::MsgWaitForMultipleObjectsEx(0, NULL, INFINITE, QS_ALLINPUT, MWMO_ALERTABLE | MWMO_INPUTAVAILABLE);
        if (dwR == WAIT_FAILED)
        {
            //Error
            assert(false);
            break;
        }
        MSG msg;

        BOOL bRet;
        // ԭ������ط�д����GetMessage��֮���Ի���PeekMessage����Ϊ���߲���������
        // ��ֻ�ῴһ����Ϣ���У�����Ϣ��ȡ�������Ӷ�����ɾ������Ϣ�����û����Ϣ��ֱ�ӷ���
        // �ú����ķ���ֵ��BOOL������Ϣ����TRUE��û����Ϣ����FALSE
        // ֮����ѡ��������PeekMessage����������Ϊ���滹��һ��ѭ��
        // Ҫ�ظ�ִ��MsgWaitForMultipleObjectsEx������ʹ��ǰ�߳̽���alertable״̬
        // ��Ϊ����Ϣ�����Ļ�MsgWaitForMultipleObjectsExҲ�᷵�أ�����Ļ���ǰ�߳̾ͻ�һֱ������MsgWaitForMultipleObjectsEx����
        // ����������set_test_APC����Ҳ�޷�ִ��
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
                                    // �����߳��з�������Ϣ������APC
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