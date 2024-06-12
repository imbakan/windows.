
// client-server model
// client side
// LAN

#include "framework.h"
#include "clientlan.h"

#define DEFAULT_IP     "192.168.1.1"
#define DEFAULT_PORT         "27015"

const int MAX_LOADSTRING = 100;
const int PADDING = 2;
const int SPLITTER = 6;
const int FONT_HEIGHT = 23;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

RECT rect1;
bool dragging1;

RECT rect2;
bool dragging2;

HIMAGELIST himl;
HWND hEdit1, hEdit2, hEdit3, hTree1, hList1;

WSADATA wsadata;

LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void OnLogMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnLogMessage2(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnConnecting(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnRunning(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnCancelDownload(HWND hWnd, WPARAM wParam, LPARAM lParam);

void OnListViewSetFocus(HWND hWnd, LPARAM lParam);
void OnListViewKillFocus(HWND hWnd, LPARAM lParam);
void OnListViewItemChanged(HWND hWnd, LPARAM lParam);

void OnItemExpanding(HWND hWnd, LPARAM lParam);
void OnItemExpanded(LPARAM lParam);
void OnSelChanged(HWND hWnd, LPARAM lParam);

void OnLButtonDown(HWND hWnd, int x, int y);
void OnLButtonUp(HWND hWnd, int x, int y);
void OnMouseMove(HWND hWnd, int x, int y);
void OnSize(HWND hWnd, int width, int height);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);

void OnClientConnect(HWND hWnd);
void OnClientDisonnect(HWND hWnd);
void OnClientExit(HWND hWnd);

void OnToolsDownload(HWND hWnd);

// main program
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTLAN, szWindowClass, MAX_LOADSTRING);

    // iregister ang window class
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENTLAN));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENTLAN);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    hInst = hInstance;

    // gumawa ng window
    int X, Y, nWidth, nHeight, Cx, Cy;

    // 1080p: 1920 x 1080

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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTLAN));

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
    case WM_LOG_MESSAGE:    OnLogMessage(hWnd, wParam, lParam);     break;
    case WM_LOG_MESSAGE_2:  OnLogMessage2(hWnd, wParam, lParam);     break;

    case WM_CONNECTING:     OnConnecting(hWnd, wParam, lParam);     break;
    case WM_RUNNING:        OnRunning(hWnd, wParam, lParam);        break;
    case WM_SHUTTING_DOWN:  OnShuttingDown(hWnd, wParam, lParam);   break;

    case WM_CANCEL_DOWNLOAD:    OnCancelDownload(hWnd, wParam, lParam);     break;

    case WM_LBUTTONDOWN:OnLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam));	break;
    case WM_LBUTTONUP:	OnLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam));      break;
    case WM_MOUSEMOVE:	OnMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));      break;
    case WM_SIZE:       OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));   break;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case LVN_ITEMCHANGED:
            if (((LPNMHDR)lParam)->idFrom == IDC_LIST1)
                OnListViewItemChanged(hWnd, lParam);
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
            break;

        case NM_SETFOCUS:
            if (((LPNMHDR)lParam)->idFrom == IDC_LIST1)
                OnListViewSetFocus(hWnd, lParam);
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
            break;

        case NM_KILLFOCUS:
            if (((LPNMHDR)lParam)->idFrom == IDC_LIST1)
                OnListViewKillFocus(hWnd, lParam);
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
            break;

        case TVN_ITEMEXPANDING: OnItemExpanding(hWnd, lParam);  break;
        case TVN_ITEMEXPANDED:  OnItemExpanded(lParam);         break;
        case TVN_SELCHANGED:    OnSelChanged(hWnd, lParam);	    break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDM_CONNECT:           OnClientConnect(hWnd);	        break;
        case IDM_DISCONNECT:        OnClientDisonnect(hWnd);        break;
        case IDM_EXIT:		        OnClientExit(hWnd);             break;
        case IDM_DOWNLOAD:	        OnToolsDownload(hWnd);          break;
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
void OnLogMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
 
}

//
void OnLogMessage2(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
 
}

//
void OnConnecting(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

//
void OnRunning(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

//
void OnShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

//
void OnCancelDownload(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

//
void OnListViewSetFocus(HWND hWnd, LPARAM lParam)
{

}

//
void OnListViewKillFocus(HWND hWnd, LPARAM lParam)
{

}

//
void OnListViewItemChanged(HWND hWnd, LPARAM lParam)
{

}

//
void OnItemExpanding(HWND hWnd, LPARAM lParam)
{

}

//
void OnItemExpanded(LPARAM lParam)
{

}

//
void OnSelChanged(HWND hWnd, LPARAM lParam)
{

}

//
void OnLButtonDown(HWND hWnd, int x, int y)
{
    if (rect1.left < x && x < rect1.right && rect1.top < y && y < rect1.bottom) {
        dragging1 = true;
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        SetCapture(hWnd);
    }
    else if (rect2.left < x && x < rect2.right && rect2.top < y && y < rect2.bottom) {
        dragging2 = true;
        SetCursor(LoadCursor(NULL, IDC_SIZENS));
        SetCapture(hWnd);
    }
}

//
void OnLButtonUp(HWND hWnd, int x, int y)
{
    dragging1 = false;
    dragging2 = false;
    ReleaseCapture();
}

//
void OnMouseMove(HWND hWnd, int x, int y)
{
    RECT cr;
    int x1, x2, y1, y2, y3, y4, width1, width2, width3, height1, height2, height3;

    if (dragging1) {

        GetClientRect(hWnd, &cr);
        if (x < 150 || (cr.right - 150) < x) return;

        rect1.left = x;
        rect1.right = rect1.left + SPLITTER;

        x1 = y1 = PADDING;
        x2 = rect1.right;
        y2 = rect1.top;
        y3 = rect1.bottom + PADDING;
        y4 = rect2.bottom;

        width1 = cr.right - 2 * PADDING;
        width2 = rect1.left - PADDING;
        width3 = cr.right - rect1.right - PADDING;

        height1 = FONT_HEIGHT;
        height2 = rect1.bottom - rect1.top;
        height3 = cr.bottom - rect2.bottom - PADDING;

        MoveWindow(hEdit1, x1, y1, width1, height1, TRUE);
        MoveWindow(hEdit2, x1, y3, width1, height1, TRUE);
        MoveWindow(hEdit3, x1, y4, width1, height3, TRUE);

        MoveWindow(hTree1, x1, y2, width2, height2, TRUE);
        MoveWindow(hList1, x2, y2, width3, height2, TRUE);

    }
    else if (dragging2) {

        GetClientRect(hWnd, &cr);
        if (y < 100 || (cr.bottom - 100) < y) return;

        rect2.top = y;
        rect2.bottom = rect2.top + SPLITTER;
        rect1.bottom = rect2.top - FONT_HEIGHT;

        x1 = y1 = PADDING;
        x2 = rect1.right;
        y2 = rect1.top;
        y3 = rect1.bottom + PADDING;
        y4 = rect2.bottom;

        width1 = cr.right - 2 * PADDING;
        width2 = rect1.left - PADDING;
        width3 = cr.right - rect1.right - PADDING;

        height1 = FONT_HEIGHT;
        height2 = rect1.bottom - rect1.top;
        height3 = cr.bottom - rect2.bottom - PADDING;

        MoveWindow(hEdit1, x1, y1, width1, height1, TRUE);
        MoveWindow(hEdit2, x1, y3, width1, height1, TRUE);
        MoveWindow(hEdit3, x1, y4, width1, height3, TRUE);

        MoveWindow(hTree1, x1, y2, width2, height2, TRUE);
        MoveWindow(hList1, x2, y2, width3, height2, TRUE);

    }
    else {

        if (rect1.left < x && x < rect1.right && rect1.top < y && y < rect1.bottom)
            SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        else if (rect2.left < x && x < rect2.right && rect2.top < y && y < rect2.bottom)
            SetCursor(LoadCursor(NULL, IDC_SIZENS));
    }
}

//          x1                 x2
//       y1 +--------------width1-----------------+   ---     ---
//          |                                     |    ^       ^
//          |                                     |    |       |
//          |               EDIT1                 | height1    |
//          |                                     |    |       |
//          |                L R                  |    v       |
//       y2 +-----width2----T+-+------width3------+   ---      |
//          |                | |                  |    ^       |
//          |                | |                  |    |       |
//          |     TREE       |R|      LIST        | height2    |
//          |                |1|                  |    |       |
//          |                | |                  |    v       |
//       y3 +---------------B+-+------------------+   ---    height
//          |                                     |    ^       |
//          |                                     |    |       |
//          |              EDIT2                  | height1    |
//          |                                     |    |       |
//          L                                     R    v       |
//         T+-------------------------------------+   ---      |
//          |               RECT2                 |            |
//      y4 B+-------------------------------------+   ---      |
//          |                                     |    ^       |
//          |                                     |    |       |
//          |             EDIT3                   | height3    |
//          |                                     |    |       |
//          |                                     |    v       v
//          +-------------------------------------+   ---     ---
// 
//          |<----------------width-------------->|

void OnSize(HWND hWnd, int width, int height)
{
    rect2.right = width - rect2.left;

    int x1, x2, y1, y2, y3, y4, width1, width2, width3, height1, height2, height3;

    x1 = y1 = PADDING;
    x2 = rect1.right;
    y2 = rect1.top;
    y3 = rect1.bottom + PADDING;
    y4 = rect2.bottom;

    width1 = width - 2 * PADDING;
    width2 = rect1.left - PADDING;
    width3 = width - rect1.right - PADDING;

    height1 = FONT_HEIGHT;
    height2 = rect1.bottom - rect1.top;
    height3 = height - rect2.bottom - PADDING;

    MoveWindow(hEdit1, x1, y1, width1, height1, FALSE);
    MoveWindow(hEdit2, x1, y3, width1, height1, FALSE);
    MoveWindow(hEdit3, x1, y4, width1, height3, FALSE);

    MoveWindow(hTree1, x1, y2, width2, height2, FALSE);
    MoveWindow(hList1, x2, y2, width3, height2, FALSE);
}

//
void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC;

    hDC = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
}

//
void OnCreate(HWND hWnd)
{
    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - OFFLINE");
    SetWindowText(hWnd, str);

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CONNECT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_DISCONNECT, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_DOWNLOAD, MF_DISABLED);

    // unang posisyon ng horizontal splitter
    rect2.left = PADDING;
    rect2.top = 500;
    rect2.bottom = rect2.top + SPLITTER;

    // unang posisyon ng vertical splitter
    rect1.top = 2 * PADDING + FONT_HEIGHT;
    rect1.bottom = rect2.top - FONT_HEIGHT;
    rect1.left = 300;
    rect1.right = rect1.left + SPLITTER;

    // gumawa ng mga icon
    HICON hicon;

    himl = ImageList_Create(16, 16, ILC_MASK, 7, 0);

    if (himl == NULL)
        return;

    hicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_1));
    ImageList_AddIcon(himl, hicon);

    hicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_2));
    ImageList_AddIcon(himl, hicon);

    hicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_3));
    ImageList_AddIcon(himl, hicon);

    hicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_4));
    ImageList_AddIcon(himl, hicon);

    hicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_5));
    ImageList_AddIcon(himl, hicon);

    hicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_6));
    ImageList_AddIcon(himl, hicon);

    hicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_7));
    ImageList_AddIcon(himl, hicon);

    // gumawa ng font para sa edit control
    HFONT hFont = CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas");

    // gumawa ng edit control
    hEdit1 = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | WS_BORDER,
        0, 0, 1, 1, hWnd, (HMENU)IDC_EDIT1, hInst, NULL);

    // gamitin ang font na 'to
    SendMessage(hEdit1, WM_SETFONT, (WPARAM)hFont, 0);

    // gumawa ng edit control
    hEdit2 = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY | WS_BORDER,
        0, 0, 1, 1, hWnd, (HMENU)IDC_EDIT2, hInst, NULL);

    // gamitin ang font na 'to
    SendMessage(hEdit2, WM_SETFONT, (WPARAM)hFont, 0);

    // gumawa ng edit control
    hEdit3 = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_BORDER,
        0, 0, 1, 1, hWnd, (HMENU)IDC_EDIT3, hInst, NULL);

    // gamitin ang font na 'to
    SendMessage(hEdit3, WM_SETFONT, (WPARAM)hFont, 0);

    // gumawa ng tree control
    hTree1 = CreateWindowEx(0L, WC_TREEVIEW, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
        0, 0, 1, 1, hWnd, (HMENU)IDC_TREE1, hInst, NULL);

    // iassigns ang mga icon sa tree control
    TreeView_SetImageList(hTree1, himl, TVSIL_NORMAL);

    // gumawa ng list control
    hList1 = CreateWindowEx(0L, WC_LISTVIEW, L"", WS_VISIBLE | LVS_SHOWSELALWAYS | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL,
        0, 0, 1, 1, hWnd, (HMENU)IDC_LIST1, hInst, NULL);

    // iassigns ang icon sa list control
    ListView_SetImageList(hList1, himl, LVSIL_SMALL);

    LV_COLUMN lvc;

    // gumawa ng tatlong column
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.pszText = str;

    // column 1
    lvc.iSubItem = 0;
    lvc.cx = 200;
    lvc.fmt = LVCFMT_LEFT;
    wcscpy_s(str, 100, L"Name");
    if (ListView_InsertColumn(hList1, 0, &lvc) == -1) return;

    // column 2
    lvc.iSubItem = 1;
    lvc.cx = 150;
    lvc.fmt = LVCFMT_RIGHT;
    wcscpy_s(str, 100, L"Date");
    if (ListView_InsertColumn(hList1, 1, &lvc) == -1) return;

    // column 3
    lvc.iSubItem = 2;
    lvc.cx = 150;
    lvc.fmt = LVCFMT_RIGHT;
    wcscpy_s(str, 100, L"Size");
    if (ListView_InsertColumn(hList1, 2, &lvc) == -1) return;

    // Ito ang unang tatawagin sa pag-initialize ng winsock.
    int errcode = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (errcode != 0)
        OutputDebugString(GetErrorMessage(WSAGetLastError()));
}

//
void OnDestroy(HWND hWnd)
{
    ImageList_Destroy(himl);
    WSACleanup();
    PostQuitMessage(0);
}

//
void OnClientConnect(HWND hWnd)
{

}

//
void OnClientDisonnect(HWND hWnd)
{

}

//
void OnClientExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

//
void OnToolsDownload(HWND hWnd)
{

}
