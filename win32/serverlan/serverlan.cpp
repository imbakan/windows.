
#include "framework.h"
#include "server_lan.h"
#include "server.h"
#include "client.h"

#define DEFAULT_PORT    "54105"

const int MAX_LOADSTRING = 100;

const int PADDING = 2;
const int SPLITTER = 6;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

RECT rect;
bool dragging;

HIMAGELIST himl;
HWND hList1, hEdit1;

WSADATA wsadata;
CServer server(DEFAULT_PORT);

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void GetIPAddress(wchar_t* str, int size);

void OnLogMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnServerRunning(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnServerShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnCreateClient(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnClientRunning(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnClientShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

void OnItemChanged(HWND hWnd, LPARAM lParam);
void OnSetFocus(HWND hWnd, LPARAM lParam);
void OnKillFocus(HWND hWnd, LPARAM lParam);

void OnLButtonDown(HWND hWnd, int x, int y);
void OnLButtonUp(HWND hWnd, int x, int y);
void OnMouseMove(HWND hWnd, int x, int y);
void OnSize(HWND hWnd, int width, int height);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);
void OnClose(HWND hWnd);

void OnServerRun(HWND hWnd);
void OnServerShutdown(HWND hWnd);
void OnServerExit(HWND hWnd);

void OnClientClose(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVERLAN, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVERLAN));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SERVERLAN);
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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVERLAN));

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
    case WM_LOG_MESSAGE:            OnLogMessage(hWnd, wParam, lParam);         break;
    case WM_SERVER_RUNNING:         OnServerRunning(hWnd, wParam, lParam);      break;
    case WM_SERVER_SHUTTING_DOWN:   OnServerShuttingDown(hWnd, wParam, lParam); break;
    case WM_CREATE_CLIENT:          OnCreateClient(hWnd, wParam, lParam);       break;
    case WM_CLIENT_RUNNING:         OnClientRunning(hWnd, wParam, lParam);      break;
    case WM_CLIENT_SHUTTING_DOWN:   OnClientShuttingDown(hWnd, wParam, lParam); break;

    case WM_NOTIFY:
        if (((LPNMHDR)lParam)->idFrom == IDC_LIST1) {
            switch (((LPNMHDR)lParam)->code) {
            case LVN_ITEMCHANGED:   OnItemChanged(hWnd, lParam);    break;
            case NM_SETFOCUS:       OnSetFocus(hWnd, lParam);       break;
            case NM_KILLFOCUS:      OnKillFocus(hWnd, lParam);      break;
            }
        }
        else {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        break;

    case WM_LBUTTONDOWN:    OnLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam)); break;
    case WM_LBUTTONUP:	    OnLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam));	break;
    case WM_MOUSEMOVE:	    OnMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));	break;
    case WM_SIZE:           OnSize(hWnd, LOWORD(lParam), HIWORD(lParam)); break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDM_RUN:       OnServerRun(hWnd);		break;
        case IDM_SHUTDOWN:  OnServerShutdown(hWnd);	break;
        case IDM_EXIT:      OnServerExit(hWnd);		break;
        case IDM_CLOSE:	    OnClientClose(hWnd);	break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_PAINT:          OnPaint(hWnd);                                          break;
    case WM_CREATE:         OnCreate(hWnd);                                         break;
    case WM_DESTROY:        OnDestroy(hWnd);                                        break;
    case WM_CLOSE:          OnClose(hWnd);								            break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void GetIPAddress(wchar_t* str, int size)
{
    char name[100];
    struct addrinfo hints;
    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    sockaddr_in* ipv4;
    sockaddr_in6* ipv6;
    void* addr;
    int errcode;
    char ip[2][INET6_ADDRSTRLEN];

    errcode = gethostname(name, sizeof(name));

    if (errcode != 0) {
        OutputDebugString(L"gethostname error\n");
        return;
    }

    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    errcode = getaddrinfo(name, DEFAULT_PORT, &hints, &result);

    if (errcode != 0) {
        OutputDebugString(L"getaddrinfo error\n");
        return;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        switch (ptr->ai_addr->sa_family) {
        case AF_INET6:
            ipv6 = (sockaddr_in6*)ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
            inet_ntop(AF_INET, addr, ip[0], INET6_ADDRSTRLEN);
            break;
        case AF_INET:
            ipv4 = (sockaddr_in*)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
            inet_ntop(AF_INET, addr, ip[1], INET6_ADDRSTRLEN);
            break;
        }
    }

    freeaddrinfo(result);

    MultiByteToWideChar(CP_UTF8, 0, ip[1], -1, str, size);
}

void OnLogMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    wchar_t* str2 = (wchar_t*)lParam;

    wchar_t* str, * str1;
    int n, n1, n2;

    n1 = GetWindowTextLength(hEdit1);

    if (n1 == 0) {
        SetWindowText(hEdit1, str2);
        return;
    }

    n2 = (int)wcslen(str2);
    n = n1 + n2 + 3;

    str1 = new wchar_t[n1 + 1];
    str = new wchar_t[n];

    GetWindowText(hEdit1, str1, n1 + 1);

    wcscpy_s(str, n, str1);
    wcscat_s(str, n, L"\r\n");
    wcscat_s(str, n, str2);

    SetWindowText(hEdit1, str);

    delete[] str1;
    delete[] str;

    SendMessage(hEdit1, EM_SETSEL, 0, -1);
    SendMessage(hEdit1, EM_SETSEL, -1, -1);
    SendMessage(hEdit1, EM_SCROLLCARET, 0, 0);
}

void OnServerRunning(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - ONLINE");
    SetWindowText(hWnd, str);

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_RUN, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_SHUTDOWN, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_CLOSE, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_DISABLED);
}

//
void OnServerShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - OFFLINE");
    SetWindowText(hWnd, str);

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_RUN, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_SHUTDOWN, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_CLOSE, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_ENABLED);
}

void OnCreateClient(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    SOCKET* socket = (SOCKET*)lParam;

    CClient* client = new CClient(hWnd, hList1, *socket);
    client->Run();
}

void OnClientRunning(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CClient* client = (CClient*)lParam;
    CClient* client1;
    LV_ITEM lvi;
    struct tm Tm;
    time_t lltime;
    int index, i, n;
    wchar_t str1[100], str2[50];

    // ang server ay nag-aasign ng id sa kumonektang client
    // isend 'to sa client
    client->Send(CClient::CONNECTED);
    client->Send(client->GetId());

    n = ListView_GetItemCount(hList1);

    if (n > 0) {

        // isend ang mga pangalan ng client na nasa list view sa kumonektang client
        lvi.mask = LVIF_PARAM;

        for (i = 0; i < n; i++) {

            lvi.iItem = i;
            ListView_GetItem(hList1, &lvi);

            client1 = (CClient*)lvi.lParam;

            client->Send(CClient::ATTRIBUTES);
            client->Send(client1->GetName());
            client->Send(client1->GetId());
        }

        client->Send(CClient::REPLY_DEVICE);

        // isend ang pangalan ng kumonektang client sa mga client na nasa list view
        for (i = 0; i < n; i++) {

            lvi.iItem = i;
            ListView_GetItem(hList1, &lvi);

            client1 = (CClient*)lvi.lParam;

            client1->Send(CClient::ATTRIBUTES);
            client1->Send(client->GetName());
            client1->Send(client->GetId());
            client1->Send(CClient::REPLY_DEVICE);
        }
    }

    // iadd ang kumonektang client sa list view
    wcscpy_s(str1, 100, client->GetName());

    time(&lltime);
    _localtime64_s(&Tm, &lltime);
    swprintf_s(str2, 50, L"%2d-%02d-%04d %2d:%02d:%02d", Tm.tm_mon + 1, Tm.tm_mday, Tm.tm_year + 1900, Tm.tm_hour, Tm.tm_min, Tm.tm_sec);

    index = ListView_GetItemCount(hList1);

    lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
    lvi.state = 0;
    lvi.stateMask = 0;
    lvi.iItem = index;
    lvi.iSubItem = 0;
    lvi.pszText = str1;
    lvi.lParam = (LPARAM)client;

    ListView_InsertItem(hList1, &lvi);
    ListView_SetItemText(hList1, index, 1, str2);

}

void OnClientShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CClient* client1 = (CClient*)lParam;
    CClient* client2;
    LVFINDINFO lvfi;
    LV_ITEM lvi;
    int i, n;
    //wchar_t str[100];

    // iremove ang client na 'to sa list view
    lvfi.flags = LVFI_PARAM;
    lvfi.psz = NULL;
    lvfi.lParam = (LPARAM)client1;
    lvfi.pt.x = 0;
    lvfi.pt.y = 0;
    lvfi.vkDirection = 0;

    i = ListView_FindItem(hList1, -1, &lvfi);
    ListView_DeleteItem(hList1, i);

    if (ListView_GetItemCount(hList1) == 0) {
        HMENU hMenu = GetMenu(hWnd);
        EnableMenuItem(hMenu, IDM_CLOSE, MF_BYCOMMAND | MF_DISABLED);
    }

    // ipaalam sa mga client na nasa list view na wala na ang client na 'to
    n = ListView_GetItemCount(hList1);

    //swprintf_s(str, 100, L"%10d\n", n);
    //OutputDebugString(str);

    for (i = 0; i < n; i++) {

        lvi.iItem = i;
        ListView_GetItem(hList1, &lvi);

        client2 = (CClient*)lvi.lParam;

        //swprintf_s(str, 100, L"%-20s%20lld\n", client2->GetName(), client2->GetId());
        //OutputDebugString(str);

        client2->Send(CClient::LEAVE);
        client2->Send(client1->GetId());
    }

    delete client1;
}

void OnItemChanged(HWND hWnd, LPARAM lParam)
{
    HWND hList = ((LPNMHDR)lParam)->hwndFrom;
    LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;

    UINT uEnabled = (pnmv->uNewState == (LVIS_FOCUSED | LVIS_SELECTED)) ? MF_ENABLED : MF_DISABLED;

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CLOSE, uEnabled);
}

void OnSetFocus(HWND hWnd, LPARAM lParam)
{
    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CLOSE, MF_DISABLED);
}

void OnKillFocus(HWND hWnd, LPARAM lParam)
{
    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CLOSE, MF_DISABLED);
}


void OnLButtonDown(HWND hWnd, int x, int y)
{
    if (rect.left < x && x < rect.right && rect.top < y && y < rect.bottom) {
        dragging = true;
        SetCursor(LoadCursor(NULL, IDC_SIZENS));
        SetCapture(hWnd);
    }
}

void OnLButtonUp(HWND hWnd, int x, int y)
{
    dragging = false;
    ReleaseCapture();
}

void OnMouseMove(HWND hWnd, int x, int y)
{
    RECT cr;
    int x1, y1, y2, width1, height1, height2;

    if (dragging) {

        GetClientRect(hWnd, &cr);
        if (y < 100 || (cr.bottom - 100) < y) return;

        rect.top = y;
        rect.bottom = rect.top + SPLITTER;

        x1 = y1 = PADDING;
        y2 = rect.bottom;

        width1 = rect.right - rect.left;
        height1 = rect.top - y1;
        height2 = cr.bottom - rect.bottom - PADDING;

        MoveWindow(hList1, x1, y1, width1, height1, TRUE);
        MoveWindow(hEdit1, x1, y2, width1, height2, TRUE);

    }
    else {

        if (rect.left < x && x < rect.right && rect.top < y && y < rect.bottom)
            SetCursor(LoadCursor(NULL, IDC_SIZENS));
    }
}

//          x1
//       y1 +-------------------------------------+  ---      ---
//          |                                     |   ^        ^
//          |                                     |   |        |
//          |                                     | height1    |
//          |                                     |   |        |
//          |L                                   R|   v        |
//          +-------------------------------------+T ---       |
//          |                                     |          height
//       y2 +-------------------------------------+B ---       |
//          |                                     |   ^        |
//          |                                     |   |        |
//          |                                     | height2    |
//          |                                     |   |        |
//          |                                     |   v        v
//          +--------------width1-----------------+  ---      ---
// 
//          |<----------------width-------------->|

void OnSize(HWND hWnd, int width, int height)
{
    rect.right = width - rect.left;

    int x1, y1, y2, width1, height1, height2;

    x1 = y1 = PADDING;
    y2 = rect.bottom;

    width1 = rect.right - rect.left;
    height1 = rect.top - y1;
    height2 = height - rect.bottom - PADDING;

    MoveWindow(hList1, x1, y1, width1, height1, FALSE);
    MoveWindow(hEdit1, x1, y2, width1, height2, FALSE);
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
    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - OFFLINE");
    SetWindowText(hWnd, str);

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_RUN, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_SHUTDOWN, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_CLOSE, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_ENABLED);

    rect.left = PADDING;
    rect.top = 175;
    rect.bottom = rect.top + SPLITTER;

    HICON hicon;

    // gumawa ng icon
    himl = ImageList_Create(16, 16, ILC_MASK, 1, 0);

    if (himl == NULL)
        return;

    hicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_1));
    ImageList_AddIcon(himl, hicon);

    // gumawa ng list view control
    hList1 = CreateWindowEx(0L, WC_LISTVIEW, L"", WS_VISIBLE | LVS_SHOWSELALWAYS | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL,
        0, 0, 1, 1, hWnd, (HMENU)IDC_LIST1, hInst, NULL);

    // iassigns ang icon sa list view control
    ListView_SetImageList(hList1, himl, LVSIL_SMALL);

    LV_COLUMN lvc;

    // gumawa ng dalawang column
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.pszText = str;

    // column 1
    lvc.iSubItem = 0;
    lvc.cx = 300;
    lvc.fmt = LVCFMT_LEFT;
    wcscpy_s(str, 100, L"Name");
    if (ListView_InsertColumn(hList1, 0, &lvc) == -1) return;

    // column 2
    lvc.iSubItem = 1;
    lvc.cx = 300;
    lvc.fmt = LVCFMT_LEFT;
    wcscpy_s(str, 100, L"Date");
    if (ListView_InsertColumn(hList1, 1, &lvc) == -1) return;

    // gumawa ng font
    HFONT hFont = CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas");

    // gumawa ng edit control
    hEdit1 = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_BORDER,
        0, 0, 1, 1, hWnd, (HMENU)IDC_EDIT1, hInst, NULL);

    // gamitin ang font na 'to
    SendMessage(hEdit1, WM_SETFONT, (WPARAM)hFont, 0);

    // initialize ang winsock
    int errcode = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (errcode != 0)
        OutputDebugString(GetErrorMessage(WSAGetLastError()));

    wchar_t ip[INET6_ADDRSTRLEN];
    GetIPAddress(ip, INET6_ADDRSTRLEN);

    swprintf_s(str, MAX_LOADSTRING, L"Ang IP Address ng server ay %s", ip);
    SetWindowText(hEdit1, str);
}

void OnDestroy(HWND hWnd)
{
    ImageList_Destroy(himl);
    WSACleanup();
    PostQuitMessage(0);
}

void OnClose(HWND hWnd)
{
    MessageBox(hWnd, L"Gamitin ang Exit menu.", L"Paalala", MB_ICONINFORMATION);
}

void OnServerRun(HWND hWnd)
{
    server.Run(hWnd);
}

void OnServerShutdown(HWND hWnd)
{
    server.Shutdown();
}

void OnServerExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

void OnClientClose(HWND hWnd)
{
    LV_ITEM lvi;
    int i;

    i = ListView_GetNextItem(hList1, -1, LVNI_SELECTED);

    if (i == -1) return;

    ZeroMemory(&lvi, sizeof(LV_ITEM));

    lvi.mask = LVIF_PARAM;;
    lvi.iItem = i;
    ListView_GetItem(hList1, &lvi);

    CClient* client = (CClient*)lvi.lParam;

    client->Shutdown();
}
