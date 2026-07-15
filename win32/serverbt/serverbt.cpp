
// Bluetooth - server side

#include "framework.h"
#include "bt_server.h"
#include "server.h"
#include "client.h"

const int MAX_LOADSTRING = 100;
const int PADDING = 2;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

HIMAGELIST himl;
HWND hList1;

WSADATA wsadata;
CServer server;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void OnAfterWindowDisplayed(HWND hWnd, WPARAM wParam, LPARAM lParam);

void OnMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnServerRunning(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnServerShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnCreateClient(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnClientRunning(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnClientShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

void OnSize(HWND hWnd, int width, int height);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);

void OnItemChanged(HWND hWnd, LPARAM lParam);

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
    LoadStringW(hInstance, IDC_BTSERVER, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BTSERVER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BTSERVER);
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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BTSERVER));

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
    case WM_AFTERWINDOWDISPLAYED: OnAfterWindowDisplayed(hWnd, wParam, lParam);  break;

    case WM_MESSAGE:                OnMessage(hWnd, wParam, lParam);            break;
    case WM_SERVER_RUNNING:         OnServerRunning(hWnd, wParam, lParam);      break;
    case WM_SERVER_SHUTTING_DOWN:   OnServerShuttingDown(hWnd, wParam, lParam); break;
    case WM_CREATE_CLIENT:          OnCreateClient(hWnd, wParam, lParam);       break;
    case WM_CLIENT_RUNNING:         OnClientRunning(hWnd, wParam, lParam);      break;
    case WM_CLIENT_SHUTTING_DOWN:   OnClientShuttingDown(hWnd, wParam, lParam); break;

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

    case WM_SIZE:           OnSize(hWnd, LOWORD(lParam), HIWORD(lParam)); break;
    case WM_PAINT:          OnPaint(hWnd);                                          break;
    case WM_CREATE:         OnCreate(hWnd);                                         break;
    case WM_DESTROY:        OnDestroy(hWnd);                                        break;

    case WM_NOTIFY:

        if (((LPNMHDR)lParam)->idFrom == IDC_LIST1) {

            if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) {
                OnItemChanged(hWnd, lParam);
            }
            else {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }

        }
        else {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void OnAfterWindowDisplayed(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    OnServerRun(hWnd);
}

void OnMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    wchar_t* str = (wchar_t*)lParam;

    OutputDebugString(str);
    OutputDebugString(L"\n");
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
    CClient* item;
    LV_ITEM lvi;
    time_t ltime;
    struct tm ttime;
    int index, i, n;
    wchar_t str1[100], str2[50], str3[50], str4[50];

    wcscpy_s(str1, 100, client->GetName());

    time(&ltime);
    _localtime64_s(&ttime, &ltime);
    wcsftime(str2, 50, L"%b %#d, %Y", &ttime);
    wcsftime(str3, 50, L"%r", &ttime);
    swprintf_s(str4, 50, L"%s %s", str2, str3);

    // idagdag ang bagong client sa list view
    index = ListView_GetItemCount(hList1);

    lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
    lvi.state = 0;
    lvi.stateMask = 0;
    lvi.iItem = index;
    lvi.iSubItem = 0;
    lvi.pszText = str1;
    lvi.lParam = (LPARAM)client;

    ListView_InsertItem(hList1, &lvi);
    ListView_SetItemText(hList1, index, 1, str4);
 
    n = ListView_GetItemCount(hList1);

    // isend ang mga nasa list view sa kumonektang client
    lvi.mask = LVIF_PARAM;

    for (i = 0; i < n; i++) {

        lvi.iItem = i;
        ListView_GetItem(hList1, &lvi);

        item = (CClient*)lvi.lParam;

        client->Send(CClient::ATTRIBUTE);
        client->Send(item->GetId());
        client->Send(item->GetName());
    }

    client->Send(CClient::DEVICE);

    // isend ang kumonektang client sa mga nasa list view
    lvi.mask = LVIF_PARAM;

    for (i = 0; i < n; i++) {

        lvi.iItem = i;
        ListView_GetItem(hList1, &lvi);

        item = (CClient*)lvi.lParam;

        if (item->GetId() == client->GetId()) continue;

        item->Send(CClient::ATTRIBUTE);
        item->Send(client->GetId());
        item->Send(client->GetName());

        item->Send(CClient::DEVICE);
    }
}

void OnClientShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CClient* client = (CClient*)lParam;
    CClient* item;
    LVFINDINFO lvfi;
    LV_ITEM lvi;
    int i, n;

    // alisin ang client nito sa list view
    lvfi.flags = LVFI_PARAM;
    lvfi.psz = NULL;
    lvfi.lParam = (LPARAM)client;
    lvfi.pt.x = 0;
    lvfi.pt.y = 0;
    lvfi.vkDirection = 0;

    i = ListView_FindItem(hList1, -1, &lvfi);
    ListView_DeleteItem(hList1, i);

    // isend ang nagleave na client sa mga nasa list view para iremove
    n = ListView_GetItemCount(hList1);

    lvi.mask = LVIF_PARAM;

    for (i = 0; i < n; i++) {

        lvi.iItem = i;
        ListView_GetItem(hList1, &lvi);

        item = (CClient*)lvi.lParam;

        item->Send(CClient::LEAVE);
        item->Send(client->GetId());
    }

}

void OnSize(HWND hWnd, int width, int height)
{
    MoveWindow(hList1, PADDING, PADDING, width - 2 * PADDING, height - 2 * PADDING, FALSE);
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

    // initialize ang winsock
    int errcode = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (errcode != 0)
        OutputDebugString(GetErrorMessage(WSAGetLastError()));

    PostMessage(hWnd, WM_AFTERWINDOWDISPLAYED, 0, (LPARAM)0);
}

void OnDestroy(HWND hWnd)
{
    ImageList_Destroy(himl);
    WSACleanup();
    PostQuitMessage(0);
}

void OnItemChanged(HWND hWnd, LPARAM lParam)
{
    HWND hList = ((LPNMHDR)lParam)->hwndFrom;
    LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;

    UINT uEnabled = (pnmv->uNewState == (LVIS_FOCUSED | LVIS_SELECTED)) ? MF_ENABLED : MF_DISABLED;

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CLOSE, uEnabled);
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

    ZeroMemory(&lvi, sizeof(LV_ITEM));

    lvi.mask = LVIF_PARAM;;
    lvi.iItem = i;
    ListView_GetItem(hList1, &lvi);

    CClient* client = (CClient*)lvi.lParam;

    client->Shutdown();
}
