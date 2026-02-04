
// Ito ay pangcapture ng audio na galing sa speaker ng computer.
// Ito ay isesave sa m4a file.

#include "framework.h"
#include "speakersoundrecorder.h"
#include "capture.h"
#include "mediafile.h"

typedef struct {
    HWND hWnd;
    HANDLE thread, stopevent;
    DWORD id;
}THREAD_STRUCT;

const int MAX_LOADSTRING = 100;

const wchar_t destination_pathname[] = { L"E:\\data\\speakersoundrecorder" };

// Global Variables:
HINSTANCE hInstance;                             // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

THREAD_STRUCT data;
HWND hButton1, hButton2, hButton3;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		ThreadFunc(LPVOID lpParam);

void OutputDuration(LONGLONG duration);
bool IsPathnameExist(const wchar_t* pathname);
void GetUniqueName(wchar_t* filename, DWORD size);

void OnSize(HWND hWnd, int width, int height);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);

void OnButtonRecord(HWND hWnd);
void OnButtonStop(HWND hWnd);
void OnButtonExit(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SPEAKERSOUNDRECORDER, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPEAKERSOUNDRECORDER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    // Store instance handle in our global variable.
    ::hInstance = hInstance;

    int X, Y, nWidth, nHeight;

    nWidth = 170;
    nHeight = 90;

    X = GetSystemMetrics(SM_CXSCREEN) - nWidth - 32;
    Y = GetSystemMetrics(SM_CYSCREEN) - nHeight - 64;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_DLGFRAME,
        X, Y, nWidth, nHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPEAKERSOUNDRECORDER));

    MSG msg;

    // main message loop
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

// Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDC_BUTTON1:   OnButtonRecord(hWnd);	break;
        case IDC_BUTTON2:   OnButtonStop(hWnd);		break;
        case IDC_BUTTON3:   OnButtonExit(hWnd);		break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_SIZE:           OnSize(hWnd, LOWORD(lParam), HIWORD(lParam)); break;
    case WM_PAINT:          OnPaint(hWnd);                                          break;
    case WM_CREATE:         OnCreate(hWnd);                                         break;
    case WM_DESTROY:        OnDestroy(hWnd);                                        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
    THREAD_STRUCT* p = (THREAD_STRUCT*)lpParam;

    CCapture capture;
    CMediaFile file;
    WAVEFORMATEX wfx;
    HRESULT hr;
    LARGE_INTEGER freq, t1, t2;
    LONGLONG timestamp, duration;
    DWORD result;
    wchar_t str[MAX_PATH], filename[MAX_PATH];

    if (!IsPathnameExist(destination_pathname)) {
        swprintf_s(str, MAX_PATH, L"Pathname %s does not exist.", destination_pathname);
        MessageBox(p->hWnd, str, L"Destination Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    swprintf_s(str, MAX_PATH, L"THE THREAD 0x%x HAS STARTED.\n", p->id);
    OutputDebugString(str);

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t1);

    EnableWindow(hButton1, FALSE); // record
    EnableWindow(hButton2, TRUE);  // stop
    EnableWindow(hButton3, FALSE); // close

    CoInitialize(NULL);
    MFStartup(MF_VERSION);

    GetUniqueName(str, MAX_PATH);

    wcscpy_s(filename, MAX_PATH, destination_pathname);
    wcscat_s(filename, MAX_PATH, L"\\");
    wcscat_s(filename, MAX_PATH, str);

    OutputDebugString(filename);
    OutputDebugString(L"\n");

    hr = capture.Open(&wfx);

    if (SUCCEEDED(hr)) {

        timestamp = 0LL;

        hr = file.Open(filename, wfx.nChannels, wfx.nSamplesPerSec, wfx.wBitsPerSample);

        QueryPerformanceCounter(&t1);

        while (true) {

            result = WaitForSingleObject(p->stopevent, 0);
            if (result == WAIT_OBJECT_0) break;

            capture.Read(&file, &timestamp);
        }

        file.Close();
    }

    capture.Close();

    MFShutdown();
    CoUninitialize();

    EnableWindow(hButton1, TRUE); // record
    EnableWindow(hButton2, FALSE);  // stop
    EnableWindow(hButton3, TRUE); // close

    QueryPerformanceCounter(&t2);
    duration = ((t2.QuadPart - t1.QuadPart) * 1000LL) / freq.QuadPart;

    OutputDuration(duration);

    swprintf_s(str, MAX_PATH, L"THE THREAD 0x%x HAS EXITED.\n", p->id);
    OutputDebugString(str);

    return 0;
}

void OutputDuration(LONGLONG duration)
{
    LONGLONG tm, quo, ms, ss, mm, hh;

    tm = duration;

    quo = tm / 1000;
    ms = tm % 1000;

    tm = quo;

    quo = tm / 60;
    ss = tm % 60; // second

    tm = quo;

    hh = tm / 60; // hour
    mm = tm % 60; // minute

    wchar_t str[50];
    swprintf_s(str, 50, L"%lld:%02lld:%02lld:%03lld\n", hh, mm, ss, ms);
    OutputDebugString(str);
}

void GetUniqueName(wchar_t* filename, DWORD size)
{
    time_t ltime;
    struct tm a;

    time(&ltime);
    _localtime64_s(&a, &ltime);

    swprintf_s(filename, size, L"%d%02d%02d%02d%02d%02d.mp4", a.tm_year + 1900, a.tm_mon + 1, a.tm_mday, a.tm_hour, a.tm_min, a.tm_sec);
}

bool IsPathnameExist(const wchar_t* pathname)
{
    HANDLE h;
    WIN32_FIND_DATA data;
    wchar_t destination[MAX_PATH];
    bool result;

    wcscpy_s(destination, MAX_PATH, pathname);
    wcscat_s(destination, MAX_PATH, L"\\*.*");

    result = true;

    h = FindFirstFile(destination, &data);
    if (h == INVALID_HANDLE_VALUE) result = false;
    FindClose(h);

    return result;
}

void OnSize(HWND hWnd, int width, int height)
{

}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC;

    hDC = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
}

void OnCreate(HWND hWnd)
{
    // record
    hButton1 = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD | BS_BITMAP | WS_VISIBLE, 10, 10, 40, 30, hWnd, (HMENU)IDC_BUTTON1, hInstance, NULL);
    SendMessageW(hButton1, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1)));

    // stop
    hButton2 = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD | BS_BITMAP | WS_VISIBLE, 55, 10, 40, 30, hWnd, (HMENU)IDC_BUTTON2, hInstance, NULL);
    SendMessageW(hButton2, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2)));

    // close
    hButton3 = CreateWindowEx(0, L"BUTTON", L"", WS_CHILD | BS_BITMAP | WS_VISIBLE, 100, 10, 40, 30, hWnd, (HMENU)IDC_BUTTON3, hInstance, NULL);
    SendMessageW(hButton3, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP3)));

    EnableWindow(hButton1, TRUE); // record
    EnableWindow(hButton2, FALSE);  // stop
    EnableWindow(hButton3, TRUE); // close

    // thread at event
    data.thread = NULL;
    data.stopevent = NULL;

    data.stopevent = CreateEvent(NULL, TRUE, FALSE, L"screerecorder_stopevent");

    if (data.stopevent == NULL)
        OutputDebugString(L"EVENT CREATION FAILED.\n");
}

void OnDestroy(HWND hWnd)
{
    if (data.thread != NULL) CloseHandle(data.thread);
    if (data.stopevent != NULL) CloseHandle(data.stopevent);

    PostQuitMessage(0);
}

void OnButtonRecord(HWND hWnd)
{
    data.hWnd = hWnd;

    if (data.thread != NULL) CloseHandle(data.thread);

    ResetEvent(data.stopevent);

    data.thread = CreateThread(NULL, 0, ThreadFunc, &data, 0, &data.id);

    if (data.thread == NULL)
        OutputDebugString(L"THREAD CREATION FAILED\n");
}

void OnButtonStop(HWND hWnd)
{
    SetEvent(data.stopevent);
}

void OnButtonExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

