
#include "framework.h"
#include "client_lan.h"
#include "connectdialog.h"
#include "client.h"

#define DEFAULT_IP     "192.168.1.5"
#define DEFAULT_PORT         "54105"

const int MAX_LOADSTRING = 100;

const int PADDING = 2;
const int SPLITTER = 6;
const int FONT_HEIGHT = 23;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

WSADATA wsadata;
CConnectDialog dlg1(DEFAULT_IP);
CClient client(DEFAULT_PORT);

RECT rect1;
bool dragging1;

RECT rect2;
bool dragging2;

HIMAGELIST himl;
HWND hEdit1, hEdit2, hEdit3, hTree1, hList1;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void TreeView_Release(HWND hTree, HTREEITEM hItem);
void TreeView_Traversal(HWND hTree, HTREEITEM hItem);
void TreeView_Clear(HWND hTree);

void OnLogMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnConnecting(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnRunning(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

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
void OnClose(HWND hWnd);

void OnClientConnect(HWND hWnd);
void OnClientDisonnect(HWND hWnd);
void OnClientExit(HWND hWnd);

void OnFileDownload(HWND hWnd);

void OnToolsEmergencyExit(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTLAN, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTLAN));

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
    case WM_LOG_MESSAGE:    OnLogMessage(hWnd, wParam, lParam);     break;
    case WM_CONNECTING:     OnConnecting(hWnd, wParam, lParam);     break;
    case WM_RUNNING:        OnRunning(hWnd, wParam, lParam);        break;
    case WM_SHUTTING_DOWN:  OnShuttingDown(hWnd, wParam, lParam);   break;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case LVN_ITEMCHANGED:
            switch (((LPNMHDR)lParam)->idFrom)
            {
            case IDC_LIST1: OnListViewItemChanged(hWnd, lParam); break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case NM_SETFOCUS:
            switch (((LPNMHDR)lParam)->idFrom)
            {
            case IDC_LIST1: OnListViewSetFocus(hWnd, lParam); break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case NM_KILLFOCUS:
            switch (((LPNMHDR)lParam)->idFrom)
            {
            case IDC_LIST1: OnListViewKillFocus(hWnd, lParam); break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
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
        case IDM_DOWNLOAD:          OnFileDownload(hWnd);           break;
        case IDM_EMERGENCY_EXIT:    OnToolsEmergencyExit(hWnd);     break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_LBUTTONDOWN:    OnLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam));	break;
    case WM_LBUTTONUP:	    OnLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam));      break;
    case WM_MOUSEMOVE:	    OnMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));      break;
    case WM_SIZE:           OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));    break;
    case WM_PAINT:          OnPaint(hWnd);                                              break;
    case WM_CREATE:         OnCreate(hWnd);                                             break;
    case WM_DESTROY:        OnDestroy(hWnd);                                            break;
    case WM_CLOSE:          OnClose(hWnd);								                break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK DlgProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return dlg1.Proc(hWnd, message, wParam, lParam);
}

// irelease ang member lParam ng Node hItem
void TreeView_Release(HWND hTree, HTREEITEM hItem)
{
    TV_ITEM tvi;
    TREE_VIEW_DATA* data;

    ZeroMemory(&tvi, sizeof(TV_ITEM));

    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hItem;

    TreeView_GetItem(hTree1, &tvi);

    data = (TREE_VIEW_DATA*)tvi.lParam;

    delete data;
}

// irelease ang bawat node ng tree view control
void TreeView_Traversal(HWND hTree, HTREEITEM hItem)
{
    HTREEITEM hSibling, hChild;
    
    hSibling = hItem;

    while (hSibling != NULL) {

        TreeView_Release(hTree, hSibling);

        hChild = TreeView_GetChild(hTree, hSibling);
        TreeView_Traversal(hTree, hChild);

        hSibling = TreeView_GetNextSibling(hTree, hSibling);
    }
}

// irelease ang lahat ng node ng tree view control
void TreeView_Clear(HWND hTree)
{
    HTREEITEM hItem;

    hItem = TreeView_GetRoot(hTree1);
    TreeView_Traversal(hTree1, hItem);
}

void OnLogMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    wchar_t* str2 = (wchar_t*)lParam;

    wchar_t* str, * str1;
    int n, n1, n2;

    n1 = GetWindowTextLength(hEdit3);

    if (n1 == 0) {
        SetWindowText(hEdit3, str2);
        return;
    }

    n2 = (int)wcslen(str2);
    n = n1 + n2 + 3;

    str1 = new wchar_t[n1 + 1];
    str = new wchar_t[n];

    GetWindowText(hEdit3, str1, n1 + 1);

    wcscpy_s(str, n, str1);
    wcscat_s(str, n, L"\r\n");
    wcscat_s(str, n, str2);

    SetWindowText(hEdit3, str);

    delete[] str1;
    delete[] str;

    SendMessage(hEdit3, EM_SETSEL, 0, -1);
    SendMessage(hEdit3, EM_SETSEL, -1, -1);
    SendMessage(hEdit3, EM_SCROLLCARET, 0, 0);
}

void OnConnecting(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - CONNECTING ...");
    SetWindowText(hWnd, str);

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CONNECT, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_DISCONNECT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_DISABLED);
}

void OnRunning(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - ONLINE");
    SetWindowText(hWnd, str);

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CONNECT, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_DISCONNECT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_DISABLED);
}

void OnShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - OFFLINE");
    SetWindowText(hWnd, str);

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CONNECT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_DISCONNECT, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_ENABLED);

    TreeView_Clear(hTree1);

    TreeView_DeleteAllItems(hTree1);
    ListView_DeleteAllItems(hList1);
    SetWindowText(hEdit1, L"");
    SetWindowText(hEdit2, L"");
}

void OnListViewSetFocus(HWND hWnd, LPARAM lParam)
{
    /*HWND hList = ((LPNMHDR)lParam)->hwndFrom;
    UINT enable = ListView_GetNextItem(hList, -1, LVNI_SELECTED) == -1 ? MF_DISABLED : MF_ENABLED;

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_DOWNLOAD, MF_BYCOMMAND | enable);*/
}

void OnListViewKillFocus(HWND hWnd, LPARAM lParam)
{
    /*HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_DOWNLOAD, MF_BYCOMMAND | MF_DISABLED);*/
}

void OnListViewItemChanged(HWND hWnd, LPARAM lParam)
{
    /*HWND hList = ((LPNMHDR)lParam)->hwndFrom;
    UINT enable = ListView_GetNextItem(hList, -1, LVNI_SELECTED) == -1 ? MF_DISABLED : MF_ENABLED;

    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_DOWNLOAD, MF_BYCOMMAND | enable);*/
}

void OnItemExpanding(HWND hWnd, LPARAM lParam)
{
    /*HWND hTree = ((LPNMHDR)lParam)->hwndFrom;
    LPNM_TREEVIEW lpnmtv = (LPNM_TREEVIEW)lParam;
    HTREEITEM hItem = lpnmtv->itemNew.hItem;
    TV_ITEM tvi;
    TREE_VIEW_DATA* data;
    CStack stack;
    long long count, size, value;
    bool is_collapse;
    wchar_t str[MAX_PATH];
    wchar_t* s;

    // dito ang tree node ay inexpand
    if (lpnmtv->action == TVE_EXPAND) {

        // alamin kung nakacollaspse ang node
        ZeroMemory(&tvi, sizeof(TV_ITEM));

        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = hItem;

        TreeView_GetItem(hTree1, &tvi);

        data = (TREE_VIEW_DATA*)tvi.lParam;
        is_collapse = data->is_collapse;

        // kung nakacollapse iexpand
        if (is_collapse) {

            // isave ang handle ng node
            client.SetHandle(hItem);

            // kunin ang path name
            TreeView_GetHierarchicalItem(&stack, hTree, hItem);

            // alisin ang pangalan ng device sa path name
            stack.Pop(str, MAX_PATH);

            // equal zero, ito ay drive
            // not equal zero, ito ay directory
            if (stack.GetCount() == 0) {

                // kompyutin ang size na isesend sa destination client
                size = sizeof(int) + sizeof(long long);

                // isend ang mga ito sa server
                // ang data->value ay id number ng destination client
                // ang client.GetId() ay id number ng source client
                client.Send(CClient::FORWARD);
                client.Send(data->value);
                client.Send(size);

                client.Send(CClient::REQUEST_DRIVE);
                client.Send(client.GetId());

            }
            else {

                // kompyutin ang size na isesend sa destination client
                count = 0LL;
                stack.Reset();
                while (!stack.IsEnd()) {
                    stack.Read(&s);
                    count += wcslen(s);
                }

                size = 2LL * stack.GetCount() * sizeof(int) + count * sizeof(wchar_t) + sizeof(int) + sizeof(long long);

                // isend ang mga ito sa server
                // ang data->value ay id number ng destination client
                // ang client.GetId() ay id number ng source client
                client.Send(CClient::FORWARD);
                client.Send(data->value);
                client.Send(size);

                while (!stack.IsEmpty()) {

                    stack.Pop(str, MAX_PATH);

                    client.Send(CClient::STRINGS);
                    client.Send(str);
                }

                client.Send(CClient::REQUEST_DIRECTORY);
                client.Send(client.GetId());
            }
        }
    }

    // dito ang tree node ay cinollapse
    if (lpnmtv->action == TVE_COLLAPSE) {

        // iset sa false ang collapse indicator ng node ni 'to
        ZeroMemory(&tvi, sizeof(TV_ITEM));

        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = hItem;

        TreeView_GetItem(hTree1, &tvi);

        data = (TREE_VIEW_DATA*)tvi.lParam;
        value = data->value;

        delete data;

        data = new TREE_VIEW_DATA;
        data->value = value;
        data->is_collapse = true;

        ZeroMemory(&tvi, sizeof(TV_ITEM));

        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = hItem;
        tvi.lParam = (LPARAM)data;

        TreeView_SetItem(hTree1, &tvi);

        // irelease ang mga child node nito
        ReleaseTreeViewChildren(hTree, hItem);
    }*/
}

void OnItemExpanded(LPARAM lParam)
{
    /*HWND hTree = ((LPNMHDR)lParam)->hwndFrom;
    LPNM_TREEVIEW lpnmtv = (LPNM_TREEVIEW)lParam;
    TV_ITEM tvi = lpnmtv->itemNew;

    if (lpnmtv->action == TVE_COLLAPSE)
        TreeView_Expand(hTree, tvi.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);*/
}

void OnSelChanged(HWND hWnd, LPARAM lParam)
{
    /*HWND hTree = ((LPNMHDR)lParam)->hwndFrom;
    LPNM_TREEVIEW lpnmtv = (LPNM_TREEVIEW)lParam;
    HTREEITEM hItem = lpnmtv->itemNew.hItem;
    TV_ITEM tvi;
    TREE_VIEW_DATA* data;
    CStack stack;
    long long count, size;
    wchar_t str1[MAX_PATH];
    wchar_t* str2;

    // kunin ang path name
    TreeView_GetHierarchicalItem(&stack, hTree, hItem);

    // idisplay sa edit control
    wcscpy_s(str1, MAX_PATH, L"");
    stack.Reset();
    while (!stack.IsEnd()) {
        stack.Read(&str2);
        wcscat_s(str1, MAX_PATH, str2);
        wcscat_s(str1, MAX_PATH, L" \u25ba ");
    }

    count = wcslen(str1);
    str1[count - 3] = '\0';
    SetWindowText(hEdit1, str1);

    // iclear ang mga ito
    ListView_DeleteAllItems(hList1);
    SetWindowText(hEdit2, L"");

    // alisin ang pangalan ng device sa path name
    stack.Pop(str1, MAX_PATH);

    if (!stack.IsEmpty()) {

        // kunin ang id number ng destination client
        ZeroMemory(&tvi, sizeof(TV_ITEM));

        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = hItem;

        TreeView_GetItem(hTree1, &tvi);

        data = (TREE_VIEW_DATA*)tvi.lParam;

        // kompyutin ang size na isesend sa destination client
        count = 0LL;
        stack.Reset();
        while (!stack.IsEnd()) {
            stack.Read(&str2);
            count += wcslen(str2);
        }

        size = 2LL * stack.GetCount() * sizeof(int) + count * sizeof(wchar_t) + sizeof(int) + sizeof(long long);

        // isend ang mga ito sa server
        // ang data->value ay ang id number ng destination client
        // ang client.GetId() ay ang id number ng source client
        client.Send(CClient::FORWARD);
        client.Send(data->value);
        client.Send(size);

        while (!stack.IsEmpty()) {

            stack.Pop(str1, MAX_PATH);

            client.Send(CClient::STRINGS);
            client.Send(str1);
        }

        client.Send(CClient::REQUEST_FILE);
        client.Send(client.GetId());
    }*/
}

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

void OnLButtonUp(HWND hWnd, int x, int y)
{
    dragging1 = false;
    dragging2 = false;
    ReleaseCapture();
}

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
    EnableMenuItem(hMenu, IDM_CONNECT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_DISCONNECT, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_DOWNLOAD, MF_BYCOMMAND | MF_DISABLED);

    // unang posisyon ng horizontal splitter
    rect2.left = PADDING;
    rect2.top = 300;
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

    // initialize ang winsock
    int errcode = WSAStartup(MAKEWORD(2, 2), &wsadata);

    if (errcode != 0)
        OutputDebugString(GetErrorMessage(WSAGetLastError()));
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

void OnClientConnect(HWND hWnd)
{
    char* ip = dlg1.Show(hWnd, hInst, DlgProc1);
    if (ip == NULL) return;

    wchar_t str[MAX_LOADSTRING];
    DWORD n = sizeof(str);
    GetComputerName(str, &n);

    //wcscpy_s(str, MAX_LOADSTRING, L"Gateway");

    client.Run(hWnd, hTree1, hList1, hEdit2, ip, str);
}

void OnClientDisonnect(HWND hWnd)
{
    client.Shutdown();
}

void OnClientExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

void OnFileDownload(HWND hWnd)
{
 
}

void OnToolsEmergencyExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}
