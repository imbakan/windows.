
// transcode video file into a m4a audio file

#include "framework.h"
#include "transcode_audio_only.h"
#include "transcoder.h"
#include "circleprogressbar.h"

const int MAX_LOADSTRING = 100;

const wchar_t destination_pathname[] = { L"E:\\transcode_audio_only" };

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

HANDLE thread, hevent[4];
DWORD id;
LONGLONG duration;
CCircleProgressBar bar;
wchar_t szSource[MAX_PATH];

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		ThreadFunc(LPVOID lpParam);

void OnSessionStarted(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSessionPaused(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSessionStopped(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSessionEnded(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSessionClosed(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnTopoStatusReady(HWND hWnd, WPARAM wParam, LPARAM lParam);

bool IsPathnameExist(const wchar_t* pathname);
void GetUniqueName(wchar_t* filename, DWORD size);
void PrintDuration(LONGLONG duration);

void OnSize(HWND hWnd, int width, int height);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);

void OnFileOpen(HWND hWnd);
void OnFileExit(HWND hWnd);

void OnTranscodeStart(HWND hWnd);
void OnTranscodeStop(HWND hWnd);

void OnToolsDebug0(HWND hWnd);
void OnToolsDebug1(HWND hWnd);
void OnToolsDebug2(HWND hWnd);
void OnToolsDebug3(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TRANSCODEAUDIOONLY, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRANSCODEAUDIOONLY));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TRANSCODEAUDIOONLY);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    // Store instance handle in our global variable.
    hInst = hInstance;

    // Creates main window.
    int X, Y, nWidth, nHeight, Cx, Cy;

    // 1080p: 1920 x 1080
    //  720p: 1280 x  720
    //  480p:  854 x  480

    Cx = 854;
    Cy = 480;

    nWidth = Cx + 16;
    nHeight = Cy + 59;

    X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
    Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 4;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        X, Y, nWidth, nHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRANSCODEAUDIOONLY));

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
    case WM_TOPO_STATUS_READY:      OnTopoStatusReady(hWnd, wParam, lParam);        break;
    case WM_SESSION_STARTED:        OnSessionStarted(hWnd, wParam, lParam);         break;
    case WM_SESSION_PAUSED:         OnSessionPaused(hWnd, wParam, lParam);          break;
    case WM_SESSION_STOPPED:        OnSessionStopped(hWnd, wParam, lParam);         break;
    case WM_SESSION_ENDED:          OnSessionEnded(hWnd, wParam, lParam);           break;
    case WM_SESSION_CLOSED:         OnSessionClosed(hWnd, wParam, lParam);          break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDM_OPEN:      OnFileOpen(hWnd);           break;
        case IDM_EXIT:      OnFileExit(hWnd);           break;
        case IDM_START:     OnTranscodeStart(hWnd);     break;
        case IDM_STOP:      OnTranscodeStop(hWnd);      break;
        case IDM_DEBUG_0:   OnToolsDebug0(hWnd);        break;
        case IDM_DEBUG_1:   OnToolsDebug1(hWnd);        break;
        case IDM_DEBUG_2:   OnToolsDebug2(hWnd);        break;
        case IDM_DEBUG_3:   OnToolsDebug3(hWnd);        break;

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
    HWND* p = (HWND*)lpParam;
    HWND hWnd = *p;

    CTranscoder transcoder;
    HMENU hMenu;
    HRESULT hr;
    LARGE_INTEGER freq, t1, t2;
    DWORD result;
    MFTIME time;
    LONGLONG lapse;
    double factor;
    wchar_t str[MAX_PATH], szDest[MAX_PATH];

    if (!IsPathnameExist(destination_pathname)) {
        swprintf_s(str, MAX_PATH, L"Pathname %s does not exist.", destination_pathname);
        MessageBox(hWnd, str, L"Destination Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    swprintf_s(str, MAX_PATH, L"THE THREAD 0x%x HAS STARTED.\n", id);
    OutputDebugString(str);

    GetUniqueName(str, MAX_PATH);

    wcscpy_s(szDest, MAX_PATH, destination_pathname);
    wcscat_s(szDest, MAX_PATH, L"\\");
    wcscat_s(szDest, MAX_PATH, str);

    OutputDebugString(szDest);
    OutputDebugString(L"\n");

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t1);

    hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_START, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_BYCOMMAND | MF_DISABLED);

    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        hr = MFStartup(MF_VERSION);
        if (SUCCEEDED(hr)) {

            transcoder.Create(hWnd);
            transcoder.Transcode(szSource, szDest);

            bar.Reset(hWnd);

            while (true) {

                result = WaitForMultipleObjects(4, hevent, FALSE, 0);

                if (result == WAIT_OBJECT_0) {

                    OutputDebugString(L".......... WAIT_OBJECT_0 + 0\n");

                    ResetEvent(hevent[0]);
                    break;

                }
                else if (result == WAIT_OBJECT_0 + 1) {

                    OutputDebugString(L".......... WAIT_OBJECT_0 + 1\n");

                    transcoder.Play();
                    ResetEvent(hevent[1]);

                }
                else if (result == WAIT_OBJECT_0 + 2) {

                    OutputDebugString(L".......... WAIT_OBJECT_0 + 2\n");

                    transcoder.Stop();
                    ResetEvent(hevent[2]);

                }
                else if (result == WAIT_OBJECT_0 + 3) {

                    OutputDebugString(L".......... WAIT_OBJECT_0 + 3\n");

                    transcoder.Close();
                    ResetEvent(hevent[3]);
                }

                transcoder.GetLatestClockTime(&time);
                factor = (double)time / (double)duration;
                bar.Set(factor);

            }

            transcoder.Destroy();

            MFShutdown();
        }
        CoUninitialize();
    }

    EnableMenuItem(hMenu, IDM_START, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_BYCOMMAND | MF_ENABLED);

    QueryPerformanceCounter(&t2);
    lapse = ((t2.QuadPart - t1.QuadPart) * 1000LL) / freq.QuadPart;

    PrintDuration(lapse);

    swprintf_s(str, MAX_PATH, L"THE THREAD 0x%x HAS EXITED.\n", id);
    OutputDebugString(str);

    return 0;
}

void OnSessionStarted(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    OutputDebugString(L".......... MAIN OnSessionStarted\n");
}

void OnSessionPaused(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    OutputDebugString(L".......... MAIN OnSessionPaused\n");
}


// ang event hevent[3] ay para iclose ang session
void OnSessionStopped(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    OutputDebugString(L".......... MAIN OnSessionStopped\n");

    SetEvent(hevent[3]);
}

// ang event hevent[3] ay para iclose ang session
void OnSessionEnded(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    OutputDebugString(L".......... MAIN OnSessionEnded\n");

    SetEvent(hevent[3]);
}

// ang event hevent[0] ay para umexit sa loop at idestroy ang session
void OnSessionClosed(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    OutputDebugString(L".......... MAIN OnSessionClosed\n");

    SetEvent(hevent[0]);
}

// ang event hevent[1] ay para iplay ang session
void OnTopoStatusReady(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    duration = *((LONGLONG*)lParam);

    OutputDebugString(L".......... MAIN OnTopoStatusReady\n");

    SetEvent(hevent[1]);
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

void GetUniqueName(wchar_t* filename, DWORD size)
{
    time_t ltime;
    struct tm a;

    time(&ltime);
    _localtime64_s(&a, &ltime);

    swprintf_s(filename, size, L"%d%02d%02d%02d%02d%02d.m4a", a.tm_year + 1900, a.tm_mon + 1, a.tm_mday, a.tm_hour, a.tm_min, a.tm_sec);
}

void PrintDuration(LONGLONG duration)
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

void OnSize(HWND hWnd, int width, int height)
{
    bar.Resize(width, height);
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC;

    hDC = BeginPaint(hWnd, &ps);
    bar.Draw(hDC);
    EndPaint(hWnd, &ps);
}

void OnCreate(HWND hWnd)
{
    hevent[0] = CreateEvent(NULL, TRUE, FALSE, L"M4ATranscoderStopEvent0");
    hevent[1] = CreateEvent(NULL, TRUE, FALSE, L"M4ATranscoderStopEvent1");
    hevent[2] = CreateEvent(NULL, TRUE, FALSE, L"M4ATranscoderStopEvent2");
    hevent[3] = CreateEvent(NULL, TRUE, FALSE, L"M4ATranscoderStopEvent3");

    thread = NULL;
    duration = 0;

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_START, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_DISABLED);
}

void OnDestroy(HWND hWnd)
{
    if (thread != NULL) CloseHandle(thread);
    if (hevent[0] != NULL) CloseHandle(hevent[0]);
    if (hevent[1] != NULL) CloseHandle(hevent[1]);
    if (hevent[2] != NULL) CloseHandle(hevent[2]);
    if (hevent[3] != NULL) CloseHandle(hevent[3]);

    PostQuitMessage(0);
}

void OnFileOpen(HWND hWnd)
{
    HRESULT hr;
    IFileOpenDialog* pFileOpen;
    FILEOPENDIALOGOPTIONS options;
    COMDLG_FILTERSPEC fs[2];
    IShellItem* pItem;
    PWSTR pszFile;
    wchar_t str[MAX_PATH], name[2][20], spec[2][6];
    bool cancel;

    cancel = true;

    // Initialize COM.
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr)) {

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr)) {

            // Filter black box files only.
            wcscpy_s(name[0], 20, L"Video Files");
            wcscpy_s(spec[0], 6, L"*.mp4");

            fs[0].pszName = name[0];
            fs[0].pszSpec = spec[0];

            wcscpy_s(name[1], 10, L"All Files");
            wcscpy_s(spec[1], 6, L"*.*");

            fs[1].pszName = name[1];
            fs[1].pszSpec = spec[1];

            pFileOpen->SetFileTypes(2, fs);
            hr = pFileOpen->GetOptions(&options);

            if (SUCCEEDED(hr)) {
                options |= FOS_STRICTFILETYPES;
                pFileOpen->SetOptions(options);
            }

            // Show the Save dialog box.
            hr = pFileOpen->Show(hWnd);

            if (SUCCEEDED(hr)) {

                // Get the result object.
                hr = pFileOpen->GetResult(&pItem);

                if (SUCCEEDED(hr)) {

                    // Gets the filename that the user made in the dialog.
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFile);

                    // Copy the file name.
                    if (SUCCEEDED(hr)) {

                        cancel = false;

                        // copy filename
                        wcscpy_s(szSource, MAX_PATH, pszFile);

                        // Release memory.
                        CoTaskMemFree(pszFile);
                    }

                    // Release result object.
                    pItem->Release();
                }
            }

            // Release FileOpenDialog object.
            pFileOpen->Release();
        }

        // Release COM.
        CoUninitialize();
    }

    if (cancel) return;

    swprintf_s(str, MAX_PATH, L"%s - %s", szTitle, szSource);
    SetWindowText(hWnd, str);

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_START, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_DISABLED);
}

void OnFileExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

void OnTranscodeStart(HWND hWnd)
{
    static HWND hwnd = hWnd;

    thread = NULL;
    thread = CreateThread(NULL, 0, ThreadFunc, &hwnd, 0, &id);
    if (thread == NULL)
        OutputDebugString(L"-------------------CreateThread failed.-------------------\n");
}

// ang event hevent[2] ay para istop ang session
void OnTranscodeStop(HWND hWnd)
{
    SetEvent(hevent[2]);
}

void OnToolsDebug0(HWND hWnd)
{

}

void OnToolsDebug1(HWND hWnd)
{

}

void OnToolsDebug2(HWND hWnd)
{

}

void OnToolsDebug3(HWND hWnd)
{

}
