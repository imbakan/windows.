
// client-server model
// client side
// bluetooth

#include "framework.h"
#include "clientbt.h"
#include "connectdialog.h"
#include "client.h"
#include "stack.h"

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
CConnectDialog dlg1;
CClient client;

LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK	DlgProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void OutputBuffer(const char* txt, char* buffer, int count);
void OutputBuffer(const char* txt, int value);
void OutputBuffer(const char* txt, long long value);

void ReleaseTreeView(HWND hTree, HTREEITEM hItem);
void ReleaseTreeViewChildren(HWND hTree, HTREEITEM hItem);

HTREEITEM TreeView_GetHierarchicalItem(CStack* stack, long long* count, HWND hTree, HTREEITEM hItem);

void OnLogMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnConnecting(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnRunning(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

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

void OnToolsOutput1(HWND hWnd);

// main program
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTBT, szWindowClass, MAX_LOADSTRING);

    // iregister ang window class
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENTBT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENTBT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    hInst = hInstance;

    // gumawa ng window
    int X, Y, nWidth, nHeight, Cx, Cy;

    // 1080p: 1920 x 1080
    //  480p:  854 x  480

    Cx = 854;
    Cy = 480;

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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTBT));

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
    case WM_LOG_MESSAGE:  OnLogMessage(hWnd, wParam, lParam);     break;

    case WM_CONNECTING:     OnConnecting(hWnd, wParam, lParam);     break;
    case WM_RUNNING:        OnRunning(hWnd, wParam, lParam);        break;
    case WM_SHUTTING_DOWN:  OnShuttingDown(hWnd, wParam, lParam);   break;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case TVN_ITEMEXPANDING: OnItemExpanding(hWnd, lParam);  break;
        case TVN_ITEMEXPANDED:  OnItemExpanded(lParam);         break;
        case TVN_SELCHANGED:    OnSelChanged(hWnd, lParam);	    break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_LBUTTONDOWN:OnLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam));	break;
    case WM_LBUTTONUP:	OnLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam));      break;
    case WM_MOUSEMOVE:	OnMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));      break;
    case WM_SIZE:       OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));   break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDM_CONNECT:           OnClientConnect(hWnd);	        break;
        case IDM_DISCONNECT:        OnClientDisonnect(hWnd);        break;
        case IDM_EXIT:		        OnClientExit(hWnd);             break;
        case IDM_OUTPUT_1:          OnToolsOutput1(hWnd);           break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

    case WM_PAINT:      OnPaint(hWnd);								break;
    case WM_CREATE:     OnCreate(hWnd);								break;
    case WM_DESTROY:    OnDestroy(hWnd);							break;
        /*case WM_CLOSE:      OnClose(hWnd);								break; */

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//
INT_PTR CALLBACK DlgProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return dlg1.Proc(hWnd, message, wParam, lParam);
}

//
void OutputBuffer(const char* txt, char* buffer, int count)
{
    int i;
    char str[10];

    OutputDebugStringA(txt);

    for (i = 0; i < count; i++) {

        sprintf_s(str, 10, " %02x", (unsigned char)buffer[i]);
        OutputDebugStringA(str);
    }

    OutputDebugStringA("\n");
}

//
void OutputBuffer(const char* txt, int value)
{
    const int n = sizeof(int);
    int* p;
    int i;
    char buffer[n];
    char str[10];

    p = (int*)buffer;
    *p = value;

    OutputDebugStringA(txt);

    for (i = 0; i < n; i++) {

        sprintf_s(str, 10, " %02x", (unsigned char)buffer[i]);
        OutputDebugStringA(str);
    }

    OutputDebugStringA("\n");
}

//
void OutputBuffer(const char* txt, long long value)
{
    const int n = sizeof(long long);
    long long* p;
    int i;
    char buffer[n];
    char str[10];

    p = (long long*)buffer;
    *p = value;

    OutputDebugStringA(txt);

    for (i = 0; i < n; i++) {

        sprintf_s(str, 10, " %02x", (unsigned char)buffer[i]);
        OutputDebugStringA(str);
    }

    OutputDebugStringA("\n");
}

// irelease ang memory naka-associate sa item pagdinestroy
void ReleaseTreeView(HWND hTree, HTREEITEM hItem)
{
    HTREEITEM hItem1, hItem2;
    TV_ITEM tvi;
    wchar_t str[MAX_PATH], str1[MAX_PATH], str2[MAX_PATH];
    TREE_VIEW_DATA* data;
    long long value;

    hItem1 = hItem;

    wcscpy_s(str1, MAX_PATH, L"????@****");

    while (hItem1 != NULL) {

        ZeroMemory(&tvi, sizeof(TV_ITEM));

        tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
        tvi.hItem = hItem1;
        tvi.pszText = str2;
        tvi.cchTextMax = MAX_PATH;

        TreeView_GetItem(hTree, &tvi);

        data = (TREE_VIEW_DATA*)tvi.lParam;
        value = data->value;

        delete data;

        // huling sibling
        if (wcscmp(str1, str2) == 0) break;

        // ibrowse ang mga child
        hItem2 = TreeView_GetChild(hTree, hItem1);

        if (hItem2 != NULL) ReleaseTreeView(hTree, hItem2);

        // pagpalitin
        wcscpy_s(str1, MAX_PATH, str2);

        hItem1 = TreeView_GetNextSibling(hTree, hItem1);
    }
}

// irelease ang memory naka-associate sa item pagkinolapse
void ReleaseTreeViewChildren(HWND hTree, HTREEITEM hItem)
{
    HTREEITEM hNext, hChild;
    TREE_VIEW_DATA* data;
    TV_ITEM tvi;

    // ang hNext ay ang unang child ng parent hItem
    hNext = TreeView_GetChild(hTree, hItem);

    while (hNext != NULL) {

        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = hNext;

        TreeView_GetItem(hTree, &tvi);

        data = (TREE_VIEW_DATA*)tvi.lParam;

        delete data;

        // alamin kung may child ang parent hNext
        hChild = TreeView_GetChild(hTree, hNext);

        // kung meron ireleale ang mag ito
        if (hChild != NULL)
            ReleaseTreeViewChildren(hTree, hNext);

        // ang hNext ay ang susunod na child ng parent hItem
        hNext = TreeView_GetNextSibling(hTree, hNext);
    }
}

//
HTREEITEM TreeView_GetHierarchicalItem(CStack* stack, long long* count, HWND hTree, HTREEITEM hItem)
{
    HTREEITEM hPrevItem;
    TV_ITEM tvi;
    wchar_t str[MAX_PATH];

    *count = 0LL;

    hPrevItem = hItem;

    // ang order ng pagkuha ng mga item ay mula sa child papuntang parent
    while (TreeView_GetParent(hTree, hPrevItem) != NULL)
    {
        // kunin ang pangalan ng item
        tvi.mask = TVIF_HANDLE | TVIF_TEXT;
        tvi.hItem = hPrevItem;
        tvi.pszText = str;
        tvi.cchTextMax = MAX_PATH;

        TreeView_GetItem(hTree, &tvi);

        *count += wcslen(str);

        stack->Push(str);

        // kunin ang susunod na parent item
        hPrevItem = TreeView_GetParent(hTree, hPrevItem);
    }

    return hItem;
}

//
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

//
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

//
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

//
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

    TreeView_DeleteAllItems(hTree1);
    ListView_DeleteAllItems(hList1);
    SetWindowText(hEdit1, L"");
    SetWindowText(hEdit2, L"");

    swprintf_s(str, MAX_LOADSTRING, L"The client thread 0x%llx has exited.", client.GetId());
    SendMessage(hWnd, WM_LOG_MESSAGE, 0, (LPARAM)str);

}

//
void OnItemExpanding(HWND hWnd, LPARAM lParam)
{
    HWND hTree = ((LPNMHDR)lParam)->hwndFrom;
    LPNM_TREEVIEW lpnmtv = (LPNM_TREEVIEW)lParam;
    HTREEITEM hItem = lpnmtv->itemNew.hItem;
    TV_ITEM tvi;
    TREE_VIEW_DATA* data;
    CStack stack;
    int n;
    long long count, size, value;
    bool is_collapse;
    wchar_t str[MAX_PATH];

    // dito ang tree item ay expand
    if (lpnmtv->action == TVE_EXPAND) {

        // kunin ang param ng tree item na 'to
        ZeroMemory(&tvi, sizeof(TV_ITEM));

        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = hItem;

        TreeView_GetItem(hTree1, &tvi);

        data = (TREE_VIEW_DATA*)tvi.lParam;
        is_collapse = data->is_collapse;

        swprintf_s(str, MAX_PATH, L"OnItemExpanding TVE_EXPAND %s\n", is_collapse ? L"true" : L"false");
        OutputDebugString(str);

        // kung nakacollapse iexpand
        if (is_collapse) {

            // iset ang tree item handle
            // dito idadagdag ang mga makukuha drive o directory
            client.SetHandle(hItem);

            // kunin ang path
            TreeView_GetHierarchicalItem(&stack, &count, hTree, hItem);
            n = stack.GetCount();

            // n equal zero, ito ay drive
            if (n == 0) {

                size = sizeof(int) + sizeof(long long);

                OutputBuffer("Send", CClient::FORWARD);
                OutputBuffer("Send", data->value);
                OutputBuffer("Send", size);

                OutputBuffer("Send", CClient::REQUEST_DRIVE);
                OutputBuffer("Send", client.GetId());

                // para sa server
                client.Send(CClient::FORWARD);
                client.Send(data->value);             // ito ang destination client para sa request
                client.Send(size);                    // size ng data na isesend, ito yung nasa baba

                // para sa client
                client.Send(CClient::REQUEST_DRIVE);
                client.Send(client.GetId());          // ito ang destination client para sa reply

            }
            else {

                while (!stack.IsEmpty()) {

                    stack.Pop(str, MAX_PATH);

                    OutputDebugString(str);
                    OutputDebugString(L"\n");
                }

            }
        }
    }

    // dito ang tree item ay collapse
    if (lpnmtv->action == TVE_COLLAPSE) {

        OutputDebugString(L"OnItemExpanding TVE_COLLAPSE\n");

        // kunin ang param ng tree item na 'to
        ZeroMemory(&tvi, sizeof(TV_ITEM));

        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = hItem;

        TreeView_GetItem(hTree1, &tvi);

        data = (TREE_VIEW_DATA*)tvi.lParam;
        value = data->value;

        delete data;

        // gawing true ang member variable is_collapse dahil ito ay nakacollapse na
        // ilagay ang param sa tree item na 'to
        data = new TREE_VIEW_DATA;
        data->value = value;
        data->is_collapse = true;

        ZeroMemory(&tvi, sizeof(TV_ITEM));

        tvi.mask = TVIF_HANDLE | TVIF_PARAM;
        tvi.hItem = hItem;
        tvi.lParam = (LPARAM)data;

        TreeView_SetItem(hTree1, &tvi);

        // irelease ang param ng bawat item
        ReleaseTreeViewChildren(hTree, hItem);
    }
}

//
void OnItemExpanded(LPARAM lParam)
{
    HWND hTree = ((LPNMHDR)lParam)->hwndFrom;
    LPNM_TREEVIEW lpnmtv = (LPNM_TREEVIEW)lParam;
    TV_ITEM tvi = lpnmtv->itemNew;

    if (lpnmtv->action == TVE_COLLAPSE)
        TreeView_Expand(hTree, tvi.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
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

// irelease lahat bago mag-exit
void OnDestroy(HWND hWnd)
{
    HTREEITEM hItem;

    hItem = TreeView_GetRoot(hTree1);
    ReleaseTreeView(hTree1, hItem);

    ImageList_Destroy(himl);
    WSACleanup();
    PostQuitMessage(0);
}

//
void OnClose(HWND hWnd)
{
    MessageBox(hWnd, L"Gamitin ang Exit menu.", L"Paalala", MB_ICONINFORMATION);
}

//
void OnClientConnect(HWND hWnd)
{
    wchar_t* address = dlg1.Show(hWnd, hInst, DlgProc1);
    if (address == NULL) return;

    wchar_t name[MAX_LOADSTRING];
    DWORD size = sizeof(name);
    GetComputerName(name, &size);

    client.Run(hWnd, hTree1, hList1, hEdit2, address, name);
}

//
void OnClientDisonnect(HWND hWnd)
{
    client.Shutdown();
}

//
void OnClientExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

//
void OnToolsOutput1(HWND hWnd)
{
}