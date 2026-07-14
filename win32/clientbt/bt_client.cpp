
// Bluetooth - client side

#include "framework.h"
#include "bt_client.h"
#include "connectdialog.h"
#include "client.h"

typedef struct {
    long long id;
    bool collapse;
} TREE_VIEW_LPARAM;

const int MAX_LOADSTRING = 100;

const int PADDING = 2;
const int SPLITTER = 6;
const int FONT_HEIGHT = 23;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // main window class name

HTREEITEM g_hItem;

WSADATA wsadata;
CConnectDialog dlg1;
CClient client;

RECT rect;
bool dragging;

HWND hEdit1, hEdit2, hTree1, hList1;
HIMAGELIST himl;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void OnMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnConnecting(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnRunning(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnShuttingDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

void OnPopulateWithDevice(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnPopulateWithDrive(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnPopulateWithFolder(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnDepopulateByDevice(HWND hWnd, WPARAM wParam, LPARAM lParam);

void TreeView_PrintItem(HWND hTree, HTREEITEM hItem);
void TreeView_FindItem(HWND hTree, long long id, HTREEITEM* hItem);
void TreeView_GetPathName(HWND hTree, HTREEITEM hItem, char** buffer, int* size);
bool TreeView_IsRootItem(HWND hTree, HTREEITEM hItem);
void TreeView_ReleaseItem(HWND hTree, HTREEITEM hItem);
void TreeView_DeepReleaseItem(HWND hTree, HTREEITEM hItem);
void TreeView_Clear(HWND hTree);

void TreeView_SetLParam(HWND hTree, HTREEITEM hItem, TREE_VIEW_LPARAM* lparam);
void TreeView_GetLParam(HWND hTree, HTREEITEM hItem, TREE_VIEW_LPARAM* lparam);

void TreeView_SaveItem(HTREEITEM hItem);
void TreeView_RestoreItem(HTREEITEM* hItem);

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

void OnToolsEmergencyExit(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BTCLIENT, szWindowClass, MAX_LOADSTRING);

    // Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BTCLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BTCLIENT);
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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BTCLIENT));

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
    case WM_MESSAGE:        OnMessage(hWnd, wParam, lParam);        break;
    case WM_CONNECTING:     OnConnecting(hWnd, wParam, lParam);     break;
    case WM_RUNNING:        OnRunning(hWnd, wParam, lParam);        break;
    case WM_SHUTTING_DOWN:  OnShuttingDown(hWnd, wParam, lParam);   break;

    case WM_POPULATE_WITH_DEVICE:   OnPopulateWithDevice(hWnd, wParam, lParam);     break;
    case WM_POPULATE_WITH_DRIVE:    OnPopulateWithDrive(hWnd, wParam, lParam);      break;
    case WM_POPULATE_WITH_FOLDER:   OnPopulateWithFolder(hWnd, wParam, lParam);     break;
    case WM_DEPOPULATE_BY_DEVICE:   OnDepopulateByDevice(hWnd, wParam, lParam);     break;

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDM_CONNECT:       OnClientConnect(hWnd);	        break;
        case IDM_DISCONNECT:    OnClientDisonnect(hWnd);        break;
        case IDM_EXIT:		    OnClientExit(hWnd);             break;
        case IDM_EMERGENCY_EXIT:OnToolsEmergencyExit(hWnd);     break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;

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

    case WM_LBUTTONDOWN:    OnLButtonDown(hWnd, LOWORD(lParam), HIWORD(lParam));	break;
    case WM_LBUTTONUP:	    OnLButtonUp(hWnd, LOWORD(lParam), HIWORD(lParam));      break;
    case WM_MOUSEMOVE:	    OnMouseMove(hWnd, LOWORD(lParam), HIWORD(lParam));      break;
    case WM_SIZE:           OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));   break;
    case WM_PAINT:          OnPaint(hWnd);                                              break;
    case WM_CREATE:         OnCreate(hWnd);                                             break;
    case WM_DESTROY:        OnDestroy(hWnd);                                            break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK DlgProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return dlg1.Proc(hWnd, message, wParam, lParam);
}

void OnMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

void OnConnecting(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CONNECT, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_DISCONNECT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_DISABLED);

    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - CONNECTING ...");
    SetWindowText(hWnd, str);
}

void OnRunning(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CONNECT, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_DISCONNECT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_DISABLED);

    wchar_t str[MAX_LOADSTRING];
    wcscpy_s(str, MAX_LOADSTRING, szTitle);
    wcscat_s(str, MAX_LOADSTRING, L" - ONLINE");
    SetWindowText(hWnd, str);
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

void OnPopulateWithDevice(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CLinkedList<wchar_t*>* names = (CLinkedList<wchar_t*>*)wParam;
    CLinkedList<long long>* ids = (CLinkedList<long long>*)lParam;

    TVINSERTSTRUCT tvinsert;
    TREE_VIEW_LPARAM* lparam;
    int n;
    long long id;
    wchar_t* pszText;
    //wchar_t str[100];

    tvinsert.hParent = NULL;
    tvinsert.hInsertAfter = TVI_ROOT;
    tvinsert.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
    tvinsert.item.hItem = NULL;
    tvinsert.item.state = 0;
    tvinsert.item.stateMask = 0;
    tvinsert.item.iImage = 0;
    tvinsert.item.iSelectedImage = 1;
    tvinsert.item.cChildren = 1;

    n = ids->GetCount();

    while (!ids->IsEmpty()) {

        ids->Remove(&id);
        names->Remove(&pszText);

        //swprintf_s(str, 100, L"%20lld %s\n", id, pszText);
        //OutputDebugString(str);

        lparam = new TREE_VIEW_LPARAM;
        lparam->id = id;
        lparam->collapse = true;

        tvinsert.item.pszText = pszText;
        tvinsert.item.cchTextMax = (int)wcslen(pszText);
        tvinsert.item.lParam = (LPARAM)lparam;

        TreeView_InsertItem(hTree1, &tvinsert);

        delete[] pszText;
    }
}

void OnPopulateWithDrive(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CLinkedList<wchar_t*>* que = (CLinkedList<wchar_t*>*)lParam;
    TVINSERTSTRUCT tvis;
    TREE_VIEW_LPARAM* lparam;
    TREE_VIEW_LPARAM param;
    HTREEITEM hItem;
    wchar_t* pszText;

    TreeView_RestoreItem(&hItem);

    tvis.hParent = hItem;
    tvis.hInsertAfter = TVI_LAST;
    tvis.itemex.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
    tvis.item.hItem = NULL;
    tvis.item.state = 0;
    tvis.item.stateMask = 0;
    tvis.item.iImage = 2;
    tvis.item.iSelectedImage = 3;
    tvis.item.cChildren = 1;

    while (!que->IsEmpty()) {

        que->Remove(&pszText);

        lparam = new TREE_VIEW_LPARAM;
        lparam->id = 0LL;
        lparam->collapse = true;

        tvis.item.pszText = pszText;
        tvis.item.cchTextMax = (int)wcslen(pszText);
        tvis.item.lParam = (LPARAM)lparam;

        TreeView_InsertItem(hTree1, &tvis);

        delete[] pszText;
    }

    TreeView_GetLParam(hTree1, hItem, &param);
    param.collapse = false;
    TreeView_SetLParam(hTree1, hItem, &param);

    TreeView_Expand(hTree1, hItem, TVE_EXPAND);
}

void OnPopulateWithFolder(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    CLinkedList<wchar_t*>* que = (CLinkedList<wchar_t*>*)lParam;
    TVINSERTSTRUCT tvis;
    TREE_VIEW_LPARAM* lparam;
    TREE_VIEW_LPARAM param;
    HTREEITEM hItem;
    wchar_t* pszText;

    TreeView_RestoreItem(&hItem);

    tvis.hParent = hItem;
    tvis.hInsertAfter = TVI_LAST;
    tvis.itemex.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
    tvis.item.hItem = NULL;
    tvis.item.state = 0;
    tvis.item.stateMask = 0;
    tvis.item.iImage = 4;
    tvis.item.iSelectedImage = 5;
    tvis.item.cChildren = 1;

    while (!que->IsEmpty()) {

        que->Remove(&pszText);

        lparam = new TREE_VIEW_LPARAM;
        lparam->id = 0LL;
        lparam->collapse = true;

        tvis.item.pszText = pszText;
        tvis.item.cchTextMax = (int)wcslen(pszText);
        tvis.item.lParam = (LPARAM)lparam;

        TreeView_InsertItem(hTree1, &tvis);

        delete[] pszText;
    }

    TreeView_GetLParam(hTree1, hItem, &param);
    param.collapse = false;
    TreeView_SetLParam(hTree1, hItem, &param);

    TreeView_Expand(hTree1, hItem, TVE_EXPAND);
}

void OnDepopulateByDevice(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HTREEITEM hItem1, hItem2;
    long long id = *((long long*)lParam);
    TREE_VIEW_LPARAM lparam;

    TreeView_FindItem(hTree1, id, &hItem1);

    //TreeView_PrintItem(hTree1, hItem1);

    TreeView_GetLParam(hTree1, hItem1, &lparam);

    if (!lparam.collapse) {

        hItem2 = TreeView_GetChild(hTree1, hItem1);
        TreeView_DeepReleaseItem(hTree1, hItem2);

        //TreeView_PrintItem(hTree1, hItem2);
    }

    TreeView_ReleaseItem(hTree1, hItem1);
    TreeView_DeleteItem(hTree1, hItem1);
}

void TreeView_PrintItem(HWND hTree, HTREEITEM hItem)
{
    TV_ITEM tvi;
    wchar_t str[MAX_PATH];

    tvi.mask = TVIF_HANDLE | TVIF_TEXT;
    tvi.hItem = hItem;
    tvi.pszText = str;
    tvi.cchTextMax = MAX_PATH;

    TreeView_GetItem(hTree, &tvi);

    OutputDebugString(str);
    OutputDebugString(L"\n");
}

void TreeView_FindItem(HWND hTree, long long id, HTREEITEM* hItem)
{
    HTREEITEM hItem1;
    TV_ITEM tvi;
    TREE_VIEW_LPARAM* lParam;
    //wchar_t str[100];

    *hItem = NULL;

    tvi.mask = TVIF_HANDLE | TVIF_PARAM;

    hItem1 = TreeView_GetRoot(hTree1);

    while (hItem1 != NULL) {

        tvi.hItem = hItem1;

        TreeView_GetItem(hTree1, &tvi);
        lParam = (TREE_VIEW_LPARAM*)tvi.lParam;

        //swprintf_s(str, 100, L"%20lld%20lld\n", lParam->id, id);
        //OutputDebugString(str);

        if (lParam->id == id) {
            *hItem = hItem1;
            break;
        }

        hItem1 = TreeView_GetNextSibling(hTree1, hItem1);
    }
}

void TreeView_GetPathName(HWND hTree, HTREEITEM hItem, char** buffer, int* size)
{
    CLinkedList<wchar_t*> stack;
    HTREEITEM prev;
    TV_ITEM tvi;
    wchar_t str[MAX_PATH];

    // ilagay sa stack ang mga directory

    prev = hItem;

    while (prev != NULL) {

        tvi.mask = TVIF_HANDLE | TVIF_TEXT;
        tvi.hItem = prev;
        tvi.pszText = str;
        tvi.cchTextMax = MAX_PATH;

        TreeView_GetItem(hTree, &tvi);

        stack.Push(str);

        prev = TreeView_GetParent(hTree, prev);
    }

    // ilagay sa buffer ang nasa stack
    // 
    //                 +--------------------+
    //                 |    string count    |
    //                 +--------------------+
    //                 |  string length 1   |
    //                 +--------------------+
    //                 |      string 1      |
    //                 +--------------------+
    //                 |  string length 2   |
    //                 +--------------------+
    //                 |      string 2      |
    //                 +--------------------+
    //                 |  string length 3   |
    //                 +--------------------+
    //                 |      string 3      |
    //                 +--------------------+
    //                           .
    //                           .
    //                           .
    wchar_t* pStr;
    wchar_t* pCh;
    int k, n;
    //wchar_t txt[100];

    // directory count
    *size = (int)sizeof(short);

    stack.Reset();

    while (stack.Read(&pStr)) {

        // size ng bawat directory
        n = (int)wcslen(pStr) + 1;
        *size += (n * (int)sizeof(wchar_t));

        //swprintf(txt, 100, L"%-20s%10lld%10lld%10lld\n", str, n, n * sizeof(wchar_t), size);
        //OutputDebugString(txt);

    }

    // ang  + sizeof(short) ay para sa null terminated sa huling wcscpy_s
    *buffer = new char[*size + sizeof(short)];

    k = 0;

    n = stack.GetCount();
    pCh = (wchar_t*)&(*buffer)[k];
    *pCh = (wchar_t)n;

    //swprintf(txt, 100, L"%04x %d\n", (*buffer)[k], (*buffer)[k]);
    //OutputDebugString(txt);

    k += sizeof(wchar_t);

    while (!stack.IsEmpty()) {

        stack.Pop(&pStr);

        n = (int)wcslen(pStr);
        pCh = (wchar_t*)&(*buffer)[k];
        *pCh = (wchar_t)n;

        //swprintf(txt, 100, L"%04x %d\n", (*buffer)[k], (*buffer)[k]);
        //OutputDebugString(txt);

        k += sizeof(wchar_t);

        pCh = (wchar_t*)&(*buffer)[k];
        wcscpy_s(pCh, n + 1, pStr);
        k += (n * sizeof(wchar_t));

        delete[] pStr;
    }
}

bool TreeView_IsRootItem(HWND hTree, HTREEITEM hItem)
{
    return (TreeView_GetParent(hTree, hItem) == NULL);
}

void TreeView_ReleaseItem(HWND hTree, HTREEITEM hItem)
{
    TV_ITEM tvi;
    TREE_VIEW_LPARAM* lParam;

    ZeroMemory(&tvi, sizeof(TV_ITEM));

    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hItem;
    tvi.cchTextMax = MAX_PATH;

    TreeView_GetItem(hTree, &tvi);

    lParam = (TREE_VIEW_LPARAM*)tvi.lParam;

    delete lParam;
}

// ang hItme ay ang unang sibling
// ang TreeView_GetNextSibling ay ang susunod na child item sa parehong parent item
void TreeView_DeepReleaseItem(HWND hTree, HTREEITEM hItem)
{
    HTREEITEM hSibling, hChild;

    hSibling = hItem;

    while (hSibling != NULL) {

        //TreeView_PrintItem(hTree, hSibling);

        TreeView_ReleaseItem(hTree, hSibling);

        hChild = TreeView_GetChild(hTree, hSibling);
        TreeView_DeepReleaseItem(hTree, hChild);

        hSibling = TreeView_GetNextSibling(hTree, hSibling);
    }
}

// ang TreeView_GetRoot ay ang unang item sa root
void TreeView_Clear(HWND hTree)
{
    HTREEITEM hItem;

    hItem = TreeView_GetRoot(hTree);
    TreeView_ReleaseItem(hTree, hItem);
}

void TreeView_SetLParam(HWND hTree, HTREEITEM hItem, TREE_VIEW_LPARAM* lparam)
{
    TV_ITEM tvi;
    TREE_VIEW_LPARAM* lParam;

    // kunin ang tvi.lParam ng hItem
    ZeroMemory(&tvi, sizeof(TV_ITEM));

    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hItem;

    TreeView_GetItem(hTree, &tvi);

    lParam = (TREE_VIEW_LPARAM*)tvi.lParam;

    delete lParam;

    // ilagay ang bagong value ng tvi.lParam ng hItem
    lParam = new TREE_VIEW_LPARAM;

    lParam->collapse = lparam->collapse;
    lParam->id = lparam->id;

    ZeroMemory(&tvi, sizeof(TV_ITEM));

    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hItem;
    tvi.lParam = (LPARAM)lParam;

    TreeView_SetItem(hTree, &tvi);
}

void TreeView_GetLParam(HWND hTree, HTREEITEM hItem, TREE_VIEW_LPARAM* lparam)
{
    TV_ITEM tvi;
    TREE_VIEW_LPARAM* lParam;

    ZeroMemory(&tvi, sizeof(TV_ITEM));

    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hItem;

    TreeView_GetItem(hTree, &tvi);

    lParam = (TREE_VIEW_LPARAM*)tvi.lParam;

    lparam->collapse = lParam->collapse;
    lparam->id = lParam->id;
}

void TreeView_SaveItem(HTREEITEM hItem)
{
    g_hItem = hItem;
}

void TreeView_RestoreItem(HTREEITEM* hItem)
{
    *hItem = g_hItem;
}

void OnItemExpanding(HWND hWnd, LPARAM lParam)
{
    HWND hTree = ((LPNMHDR)lParam)->hwndFrom;
    LPNM_TREEVIEW lpnmtv = (LPNM_TREEVIEW)lParam;
    HTREEITEM hItem = lpnmtv->itemNew.hItem;
    HTREEITEM hItem2;
    TREE_VIEW_LPARAM param;
    char* buffer;
    long long id, size2;
    int size1;
    //wchar_t str[100];

    // ang node ay inexpand
    if (lpnmtv->action == TVE_EXPAND) {

        TreeView_GetLParam(hTree1, hItem, &param);
        id = param.id;

        if (param.collapse) {

            TreeView_SaveItem(hItem);

            if (TreeView_IsRootItem(hTree, hItem)) {

                //swprintf_s(str, 100, L"%30lld expand\n", id);
                //OutputDebugString(str);

                size2 = sizeof(int);

                client.Send(CClient::FORWARD);
                client.Send(id);
                client.Send(size2);

                client.Send(CClient::REQ_DRIVE);
            }
            else {

                TreeView_GetPathName(hTree, hItem, &buffer, &size1);

                size2 = 2LL * sizeof(int) + size1;

                client.Send(CClient::FORWARD);
                client.Send(id);
                client.Send(size2);

                client.Send(CClient::REQ_FOLDER);
                client.Send(buffer, size1);

                delete[] buffer;
            }

        }
    }

    // bago ang node ay icollapse
    if (lpnmtv->action == TVE_COLLAPSE) {

        TreeView_GetLParam(hTree1, hItem, &param);
        param.collapse = true;
        TreeView_SetLParam(hTree1, hItem, &param);

        hItem2 = TreeView_GetChild(hTree1, hItem);
        TreeView_ReleaseItem(hTree1, hItem2);
    }
}

void OnItemExpanded(LPARAM lParam)
{
    HWND hTree = ((LPNMHDR)lParam)->hwndFrom;
    LPNM_TREEVIEW lpnmtv = (LPNM_TREEVIEW)lParam;
    TV_ITEM tvi = lpnmtv->itemNew;

    if (lpnmtv->action == TVE_COLLAPSE)
        TreeView_Expand(hTree, tvi.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
}

void OnSelChanged(HWND hWnd, LPARAM lParam)
{

}

void OnLButtonDown(HWND hWnd, int x, int y)
{
    if (rect.left < x && x < rect.right && rect.top < y && y < rect.bottom) {
        dragging = true;
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
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
    int x1, x2, y2, width2, width3, height2;

    if (dragging) {

        GetClientRect(hWnd, &cr);
        if (x < 150 || (cr.right - 150) < x) return;

        rect.left = x;
        rect.right = rect.left + SPLITTER;

        x1 = PADDING;
        x2 = rect.right;

        y2 = rect.top;

        width2 = rect.left - PADDING;
        width3 = cr.right - rect.right - PADDING;

        height2 = rect.bottom - rect.top;

        MoveWindow(hTree1, x1, y2, width2, height2, TRUE);
        MoveWindow(hList1, x2, y2, width3, height2, TRUE);
    }
    else {

        if (rect.left < x && x < rect.right && rect.top < y && y < rect.bottom) SetCursor(LoadCursor(NULL, IDC_SIZEWE));
    }
}

//       y1 +--------------width1-----------------+              ---
//          |                                     |               ^
//          |               EDIT 1                | height1       |
//          |                L R                  |               |
//       y2 +-----width2----T+-+------width3------+               |
//          |                | |                  |               |
//          |                | |                  |               |
//          |     TREE       | |      LIST        | height2     height
//          |                | |                  |               |
//          |                | |                  |               |
//       y3 +---------------B+-+------------------+               |
//          |                                     |               ^
//          |              EDIT 2                 | height1       |
//          |                                     |               v
//          +-------------------------------------+              ---
//          x1                 x2
// 
//          |<----------------width-------------->|

void OnSize(HWND hWnd, int width, int height)
{
    rect.bottom = height - rect.top;

    int x1, x2, y1, y2, y3, width1, width2, width3, height1, height2;

    x1 = PADDING;
    x2 = rect.right;

    y1 = PADDING;
    y2 = rect.top;
    y3 = rect.bottom + PADDING;

    width1 = width - 2 * PADDING;
    width2 = rect.left - PADDING;
    width3 = width - rect.right - PADDING;

    height1 = FONT_HEIGHT;
    height2 = rect.bottom - rect.top;

    MoveWindow(hEdit1, x1, y1, width1, height1, FALSE);
    MoveWindow(hEdit2, x1, y3, width1, height1, FALSE);

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
    HMENU hMenu = GetMenu(hWnd);
    EnableMenuItem(hMenu, IDM_CONNECT, MF_ENABLED);
    EnableMenuItem(hMenu, IDM_DISCONNECT, MF_DISABLED);
    EnableMenuItem(hMenu, IDM_EXIT, MF_ENABLED);

    rect.top = 2 * PADDING + FONT_HEIGHT;
    rect.left = 300;
    rect.right = rect.left + SPLITTER;

    wchar_t text[MAX_LOADSTRING];
    wcscpy_s(text, MAX_LOADSTRING, szTitle);
    wcscat_s(text, MAX_LOADSTRING, L" - OFFLINE");
    SetWindowText(hWnd, text);

    HICON hicon;

    // icon for tree and list control
    himl = ImageList_Create(16, 16, ILC_MASK, 5, 0);

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

    // gumawa ng tree control
    hTree1 = CreateWindowEx(0L, WC_TREEVIEW, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
        0, 0, 1, 1, hWnd, (HMENU)IDC_TREE1, hInst, NULL);

    TreeView_SetImageList(hTree1, himl, TVSIL_NORMAL);

    // gumawa ng list control
    hList1 = CreateWindowEx(0L, WC_LISTVIEW, L"", WS_VISIBLE | LVS_SHOWSELALWAYS | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL,
        0, 0, 1, 1, hWnd, (HMENU)IDC_LIST1, hInst, NULL);

    ListView_SetImageList(hList1, himl, LVSIL_SMALL);

    LV_COLUMN lvc;
    wchar_t str[100];

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

void OnClientConnect(HWND hWnd)
{
    wchar_t* address = dlg1.Show(hWnd, hInst, DlgProc1);
    if (address == NULL) return;

    wchar_t name[MAX_LOADSTRING];
    DWORD n = sizeof(name);
    GetComputerName(name, &n);

    client.Run(hWnd, name, address);
}

void OnClientDisonnect(HWND hWnd)
{
    client.Shutdown();
}

void OnClientExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}

void OnToolsEmergencyExit(HWND hWnd)
{
    DestroyWindow(hWnd);
}
