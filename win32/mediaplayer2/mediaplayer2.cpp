

// This is an synchronous

#include "framework.h"
#include "mediaplayer2.h"
#include "player.h"

const int MAX_LOADSTRING = 100;

const int BAR_HEIGHT = 20;
const int FONT_HEIGHT = -15;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

HANDLE thread;
DWORD id;

HWND hText;
CPlayer player;
RECT rect, rect1, rect2;
DWORD interval;
int fx, fy, bar_width;
wchar_t szCurrentTime[50];

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		ThreadFunc(LPVOID lpParam);

void OnPreDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnTopoStatusReady(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSessionStarted(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSessionPaused(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSessionStopped(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSessionEnded(HWND hWnd, WPARAM wParam, LPARAM lParam);

void wcstime(wchar_t* str, size_t count, UINT64 time);

void OnLButtonDown(HWND hWnd, WORD Key, int x, int y);
void OnSize(HWND hWnd, int width, int height);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);
void OnClose(HWND hWnd);

void OnFileOpen(HWND hWnd);
void OnFileExit(HWND hWnd);

void OnPlayerPlay(HWND hWnd);
void OnPlayerPause(HWND hWnd);
void OnPlayerStop(HWND hWnd);

void OnToolsDebug1(HWND hWnd);
void OnToolsDebug2(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MEDIAPLAYER2, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MEDIAPLAYER2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MEDIAPLAYER2);
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

    Cx = 1280;
    Cy = 720;

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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MEDIAPLAYER2));

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
    case WM_PRE_DESTROY:        OnPreDestroy(hWnd, wParam, lParam);         break;
    case WM_TOPO_STATUS_READY:  OnTopoStatusReady(hWnd, wParam, lParam);    break;
    case WM_SESSION_STARTED:    OnSessionStarted(hWnd, wParam, lParam);     break;
    case WM_SESSION_PAUSED:     OnSessionPaused(hWnd, wParam, lParam);      break;
    case WM_SESSION_STOPPED:    OnSessionStopped(hWnd, wParam, lParam);     break;
    case WM_SESSION_ENDED:      OnSessionEnded(hWnd, wParam, lParam);       break;

    case WM_CTLCOLORSTATIC:

        if (GetDlgCtrlID((HWND)lParam) == IDC_TEXT1)
            return (LRESULT)GetStockObject(DKGRAY_BRUSH);
        else
            return DefWindowProc(hWnd, message, wParam, lParam);
        break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDM_OPEN:          OnFileOpen(hWnd);		    break;
        case IDM_EXIT:          OnFileExit(hWnd);           break;
        case IDM_PLAY:          OnPlayerPlay(hWnd);         break;
        case IDM_PAUSE:         OnPlayerPause(hWnd);        break;
        case IDM_STOP:          OnPlayerStop(hWnd);         break;
        case IDM_DEBUG_1:       OnToolsDebug1(hWnd);        break;
        case IDM_DEBUG_2:       OnToolsDebug2(hWnd);        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_LBUTTONDOWN:    OnLButtonDown(hWnd, WORD(wParam), LOWORD(lParam), HIWORD(lParam));  break;
    case WM_SIZE:           OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));                       break;
    case WM_PAINT:          OnPaint(hWnd);                                                      break;
    case WM_CREATE:         OnCreate(hWnd);                                                     break;
    case WM_DESTROY:        OnDestroy(hWnd);                                                    break;
    case WM_CLOSE:          OnClose(hWnd);                                                      break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
    HWND hWnd = *((HWND*)lpParam);
    MFTIME duration, latest;
    wchar_t str[100], str1[50], str2[50];

    // TESTING LANG
    //bool ounce = true;

    swprintf_s(str, 100, L"THE THREAD 0x%x HAS STARTED.\n", id);
    OutputDebugString(str);

    while (true) {

        Sleep(interval);

        player.GetLatestClockTime(&latest);
        wcstime(str1, 50, latest);

        duration = player.GetSourceDuration();
        wcstime(str2, 50, duration);

        swprintf_s(szCurrentTime, 50, L"%s : %s", str1, str2);

        rect1.right = rect2.left = (int)(((double)latest / (double)duration) * (double)bar_width);

        InvalidateRect(hWnd, &rect, TRUE);

        // TESTING LANG Steve Jobs MIT Class.mp4
        //if (ounce && latest >= 10747985272) {
        //    ounce = false;
        //    OnPlayerStop(hWnd);
        //}
    }

    swprintf_s(str, 100, L"THE THREAD 0x%x HAS EXITED.\n", id);
    OutputDebugString(str);

    return 0;
}
void OnPreDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    DestroyWindow(hWnd);
}

void OnTopoStatusReady(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    MFTIME duration, mspd, mspp;
    wchar_t str[100];

    duration = player.GetSourceDuration();
    mspd = duration / 10000;
    mspp = mspd / bar_width;
    interval = (DWORD)mspp;

    swprintf_s(str, 100, L".......... %d millisecon per pixel\n", interval);
    OutputDebugString(str);

    OnPlayerPlay(hWnd);

    // TESTING LANG Steve Jobs MIT Class.mp4
    //player.Play(8882126865);
}

void OnSessionStarted(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu;

    hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_PLAY, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_PAUSE, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_BYCOMMAND | MF_DISABLED);

    DWORD count = ResumeThread(thread);

    wchar_t str[100];
    swprintf_s(str, 100, L"THREAD COUNT = %d\n", count);
    OutputDebugString(str);
}

void OnSessionPaused(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu;

    hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_PLAY, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_PAUSE, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_BYCOMMAND | MF_DISABLED);

    MFTIME latest;
    wchar_t text[100];
    player.GetLatestClockTime(&latest);
    swprintf_s(text, 100, L"%30lld centnanosecond\n", latest);
    OutputDebugString(text);

    DWORD count = SuspendThread(thread);

    wchar_t str[100];
    swprintf_s(str, 100, L"THREAD COUNT = %d\n", count);
    OutputDebugString(str);
}

void OnSessionStopped(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu;

    hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_PLAY, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_PAUSE, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_BYCOMMAND | MF_ENABLED);

    DWORD count = SuspendThread(thread);

    wchar_t str[100];
    swprintf_s(str, 100, L"THREAD COUNT = %d\n", count);
    OutputDebugString(str);

    if (count > 0) {

        count = ResumeThread(thread);

        wchar_t str[100];
        swprintf_s(str, 100, L"THREAD COUNT = %d\n", count);
        OutputDebugString(str);

    }
}

void OnSessionEnded(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu;

    hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_PLAY, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem(hMenu, IDM_PAUSE, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_BYCOMMAND | MF_ENABLED);

    DWORD count = SuspendThread(thread);

    wchar_t str[100];
    swprintf_s(str, 100, L"THREAD COUNT = %d\n", count);
    OutputDebugString(str);
}

void wcstime(wchar_t* str, size_t count, UINT64 time)
{
    UINT64 hh, mm, tos, sec, tom;

    tos = time / 10000000;  // all seconds
    sec = tos % 60;         // secomd
    tom = tos / 60;
    mm = tom % 60;          // minute
    hh = tom / 60;          // hour

    swprintf_s(str, count, L"%2lld:%02lld:%02lld", hh, mm, sec);
}

void OnLButtonDown(HWND hWnd, WORD Key, int x, int y)
{
    if (!(rect1.top < y && y < rect1.bottom)) return;

    MFTIME seektime, duration;
    double fraction;

    fraction = (double)(x - rect1.left) / (double)bar_width;

    if (fraction < 0.0) fraction = 0.0;
    if (fraction > 1.0) fraction = 1.0;

    duration = player.GetSourceDuration();

    seektime = (MFTIME)(fraction * (double)duration);

    player.SkipToPosition(seektime);
}

void OnSize(HWND hWnd, int width, int height)
{
    const int ct_width = 152;

    int x, y, cx, cy;
    wchar_t str[100];

    x = 2;
    y = 2;

    cx = width - 4;
    cy = height - BAR_HEIGHT - 4;

    rect1.left = 2;
    rect1.right = rect2.left = 5;
    rect2.right = width - 4;

    rect1.top = rect2.top = y + cy;
    rect1.bottom = rect2.bottom = rect2.top + BAR_HEIGHT;

    rect.left = rect1.left;
    rect.right = rect2.right;

    rect.top = rect1.top;
    rect.bottom = rect1.bottom;

    fx = (width - ct_width) / 2;
    fy = rect1.top + (BAR_HEIGHT + FONT_HEIGHT) / 2;

    bar_width = rect2.right - rect1.left;

    x = 10;
    y = 10;

    cx = rect2.right - 2 * x;
    cy = rect1.top - 2 * y;

    MoveWindow(hText, x, y, cx, cy, TRUE);

    player.UpdateVideoWindow();

    if (player.IsNotNullMediSource()) {

        MFTIME duration, mspd, mspp;

        duration = player.GetSourceDuration();
        mspd = duration / 10000;
        mspp = mspd / bar_width;
        interval = (DWORD)mspp;

        swprintf_s(str, 100, L".......... %d millisecon per pixel\n", interval);
        OutputDebugString(str);
    }
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC;
    HBRUSH hBrush1;
    HBRUSH hBrush2;
    HFONT hFont;

    hDC = BeginPaint(hWnd, &ps);

    hBrush1 = CreateSolidBrush(RGB(200, 200, 0));
    hBrush2 = CreateSolidBrush(RGB(40, 40, 40));

    hFont = CreateFont(FONT_HEIGHT, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas");

    SelectObject(hDC, hBrush1);
    Rectangle(hDC, rect1.left, rect1.top, rect1.right, rect1.bottom);

    SelectObject(hDC, hBrush2);
    Rectangle(hDC, rect2.left, rect2.top, rect2.right, rect2.bottom);

    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(128, 128, 128));
    SelectObject(hDC, hFont);
    TextOut(hDC, fx, fy, szCurrentTime, (int)wcslen(szCurrentTime));

    //       152        18
    //SIZE size;
    //wchar_t text[100];
    //GetTextExtentPoint32(hDC, szCurrentTime, (int)wcslen(szCurrentTime), &size);
    //swprintf_s(text, 100, L"%10d%10d\n", size.cx, size.cy);
    //OutputDebugString(text);

    DeleteObject(hFont);
    DeleteObject(hBrush2);
    DeleteObject(hBrush1);

    EndPaint(hWnd, &ps);
}

void OnCreate(HWND hWnd)
{
    interval = 0;

    HMENU hMenu;

    hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_PLAY, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_PAUSE, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | MF_DISABLED);

    wcscpy_s(szCurrentTime, 50, L"00:00:00 - 00:00:00");

    hText = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE,
        0, 0, 1, 1, hWnd, (HMENU)IDC_TEXT1, hInst, NULL);

    // Call CoInitializeEx to initialize the COM library.
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        OutputDebugString(L"COM initialization failed.\n");
    }

    // Initialize Microsoft Media Foundation.
    hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
        OutputDebugString(L"MFC initialization failed.\n");
    }

    hr = player.Create(hWnd, hText);
    if (FAILED(hr)) {
        OutputDebugString(L"player initialization failed.\n");
    }

    static HWND hwnd = hWnd;
    thread = NULL;
    thread = CreateThread(NULL, 0, ThreadFunc, &hwnd, CREATE_SUSPENDED, &id);
}

void OnDestroy(HWND hWnd)
{
    if (thread != NULL) CloseHandle(thread);

    player.Destroy();

    // Call MFShutdown.
    MFShutdown();

    // Call CoUninitialize.
    CoUninitialize();

    PostQuitMessage(0);
}

void OnClose(HWND hWnd)
{
    MessageBox(hWnd, L"Gamitin ang Exit menu.", L"Paalala", MB_ICONINFORMATION);
}

void OnFileOpen(HWND hWnd)
{
    HRESULT hr;
    IFileOpenDialog* pFileOpen;
    FILEOPENDIALOGOPTIONS options;
    COMDLG_FILTERSPEC fs[2];
    IShellItem* pItem;
    PWSTR pszFile;
    wchar_t str[MAX_PATH], name[2][20], spec[2][6], filename[MAX_PATH];
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
                        wcscpy_s(filename, MAX_PATH, pszFile);

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

    swprintf_s(str, MAX_PATH, L"%s - %s", szTitle, filename);
    SetWindowText(hWnd, str);

    player.Open(filename);
}

void OnFileExit(HWND hWnd)
{
    player.Close();
}

void OnPlayerPlay(HWND hWnd)
{
    player.Play();
}

void OnPlayerPause(HWND hWnd)
{
    player.Pause();
}

void OnPlayerStop(HWND hWnd)
{
    player.Stop();
}

void OnToolsDebug1(HWND hWnd)
{

}

void OnToolsDebug2(HWND hWnd)
{

}
