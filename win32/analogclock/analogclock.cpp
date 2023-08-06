
// Ito ay program para idisplay ang analog clock.

#include "framework.h"
#include "analogclock.h"
#include "clock.h"

const int MAX_LOADSTRING = 100;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

CClock clock;
int cx, cy;

LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void OnSize(HWND hWnd, int width, int height);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);

void OnClockStart(HWND hWnd);
void OnClockStop(HWND hWnd);
void OnClockExit(HWND hWnd);

// main program
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ANALOGCLOCK, szWindowClass, MAX_LOADSTRING);

    // iregister ang window class
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANALOGCLOCK));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ANALOGCLOCK);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    hInst = hInstance;

    // gumawa ng window
    int X, Y, nWidth, nHeight, Cx, Cy;

    // 1080p: 1920 x 1080
    //  720p: 1280 x  720
    //  480p:  854 x  480

    Cx = 1280;
    Cy = 720;

    nWidth = Cx + 16;
    nHeight = Cy + 59;

    X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
    Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 3;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        X, Y,
        nWidth, nHeight,
        nullptr, nullptr, hInstance, nullptr);

    // kapag may error, wag irun
    if (!hWnd)
        return FALSE;

    // idisplay ang window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ANALOGCLOCK));

    // message loop
    MSG msg;

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

// window callback function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:       OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));   break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDM_START:		OnClockStart(hWnd);				break;
        case IDM_STOP:		OnClockStop(hWnd);				break;
        case IDM_EXIT:		OnClockExit(hWnd);				break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_PAINT:   OnPaint(hWnd);									break;
    case WM_CREATE:  OnCreate(hWnd);								break;
    case WM_DESTROY: OnDestroy(hWnd);								break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//
void OnSize(HWND hWnd, int width, int height)
{
    cx = width;
    cy = height;

    wchar_t str[100];

    swprintf_s(str, 100, L"%10d%10d%10.0f\n", width/2, height/2, 0.85 * (double)min(width, height) / 2.0);
    OutputDebugString(str);
}

// gumamit ako rito ng memmory device context para di magflicker
void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC, hMDC;
    HBITMAP hBitmap;

    hDC = BeginPaint(hWnd, &ps);

    hMDC = CreateCompatibleDC(hDC);

    hBitmap = CreateCompatibleBitmap(hDC, cx, cy);
   
    SelectObject(hMDC, hBitmap);

    PatBlt(hMDC, 0, 0, cx, cy, WHITENESS);

    clock.Draw(hMDC);

    BitBlt(hDC, 0, 0, cx, cy, hMDC, 0 , 0, SRCCOPY);

    DeleteObject(hBitmap);

    DeleteDC(hMDC);

    EndPaint(hWnd, &ps);
}

//
void OnCreate(HWND hWnd)
{
    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_START, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_STOP, MF_DISABLED);

    clock.Create(640, 360, 306);
}

//
void OnDestroy(HWND hWnd)
{
    PostQuitMessage(0);
}

//
void OnClockStart(HWND hWnd)
{
    clock.Start(hWnd);
}

//
void OnClockStop(HWND hWnd)
{
    clock.Stop();
}

//
void OnClockExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}
