
#include "framework.h"
#include "connectdialog.h"

CConnectDialog::CConnectDialog()
{
	p = NULL;
}

CConnectDialog::~CConnectDialog()
{

}

void CConnectDialog::GetBluetoothDevices(CQueue_s* que)
{
	HANDLE handle;
	PWSAQUERYSET queryset;
	SOCKADDR_BTH address;
	DWORD flags, size;
	int errcode, result;
	bool loop;
	wchar_t str1[BTH_MAX_NAME_SIZE], str2[20];
	unsigned char* btaddr;

	// mag-allocate ng memory para sa PWSAQUERYSET
	size = sizeof(WSAQUERYSET);
	queryset = NULL;
	queryset = (PWSAQUERYSET)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
	if (queryset == NULL) return;

	ZeroMemory(queryset, size);
	queryset->dwNameSpace = NS_BTH;
	queryset->dwSize = sizeof(WSAQUERYSET);

	handle = NULL;
	flags = LUP_CONTAINERS | LUP_RETURN_NAME | LUP_RETURN_ADDR;

	// simulan na
	errcode = WSALookupServiceBegin(queryset, flags, &handle);

	if (errcode != 0) return;

	loop = true;

	while (loop) {

		// kunin ang address at pangalan ng bluetooth device
		errcode = WSALookupServiceNext(handle, flags, &size, queryset);

		if (errcode == 0) {

			CopyMemory(&address, (PSOCKADDR_BTH)queryset->lpcsaBuffer->RemoteAddr.lpSockaddr, sizeof(address));

			wcscpy_s(str1, BTH_MAX_NAME_SIZE, queryset->lpszServiceInstanceName);
			btaddr = (unsigned char*)queryset->lpcsaBuffer->RemoteAddr.lpSockaddr->sa_data;
			swprintf_s(str2, 20, L"%02x:%02x:%02x:%02x:%02x:%02x", btaddr[5], btaddr[4], btaddr[3], btaddr[2], btaddr[1], btaddr[0]);

			que->Add(str1);
			que->Add(str2);
		}
		else {

			// iassign ang return nito sa variable para di mag error 5 (ERROR_ACCESS_DENIED)
			result = WSAGetLastError();

			switch (result)
			{
			case WSAEFAULT:

				// maling size para sa PWSAQUERYSET, iallocate uli gamit ang bagong size
				HeapFree(GetProcessHeap(), 0, queryset);
				queryset = (PWSAQUERYSET)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
				if (queryset == NULL) loop = false;
				break;

			case WSA_E_NO_MORE:

				// nakuha na lahat, alis na sa loop
				loop = false;
				break;

			default:

				// alis na rin sa loop
				loop = false;
			}

		}
	}

	// tapos na
	WSALookupServiceEnd(handle);

	// irelease ang memory na inallocate
	HeapFree(GetProcessHeap(), 0, queryset);
}

void CConnectDialog::OnInitDialog(HWND hWnd)
{
	HWND hwnd;
	RECT rect1, rect2;
	int x, y;
	CQueue_s que;

	hwnd = GetParent(hWnd);
	GetWindowRect(hwnd, &rect1);
	GetWindowRect(hWnd, &rect2);

	x = rect1.left + ((rect1.right - rect1.left) - (rect2.right - rect2.left)) / 2;
	y = rect1.top + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)) / 2;

	SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

	// list view
	HWND hList = GetDlgItem(hWnd, IDC_LIST2);

	LV_COLUMN lvc;
	LV_ITEM lvi;
	int i;
	wchar_t str1[BTH_MAX_NAME_SIZE], str2[20];

	// header
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 150;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = str1;

	// column 1
	lvc.iSubItem = 0;
	wcscpy_s(str1, BTH_MAX_NAME_SIZE, L"Device");
	if (ListView_InsertColumn(hList, 0, &lvc) == -1) return;

	// column 2
	lvc.iSubItem = 1;
	wcscpy_s(str1, BTH_MAX_NAME_SIZE, L"Address");
	if (ListView_InsertColumn(hList, 1, &lvc) == -1) return;

	// populate
	GetBluetoothDevices(&que);

	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = 0;
	lvi.stateMask = 0;
	lvi.iImage = 0;
	lvi.iSubItem = 0;

	i = 0;

	while (!que.IsEmpty()) {

		que.Remove(str1, BTH_MAX_NAME_SIZE);
		que.Remove(str2, 20);

		lvi.iItem = i;
		lvi.pszText = str1;

		ListView_InsertItem(hList, &lvi);            // column 1
		ListView_SetItemText(hList, i, 1, str2);     // column 2
	}

	// idisable ang ok button
	HWND hOK = GetDlgItem(hWnd, IDOK);
	EnableWindow(hOK, false);

	// iresize kasi di to nag reresize sa unang run
	RECT rect;
	WPARAM wParam;
	LPARAM lParam;
	GetClientRect(hWnd, &rect);
	wParam = 0;
	lParam = rect.bottom * 0x00010000 + rect.right;
	SendMessage(hWnd, WM_SIZE, wParam, lParam);
}

void CConnectDialog::OnOk(HWND hWnd, WPARAM wParam)
{
	HWND hList = GetDlgItem(hWnd, IDC_LIST2);
	int i = ListView_GetNextItem(hList, -1, LVNI_SELECTED);

	LV_ITEM lvi;
	wchar_t text[BTH_MAX_NAME_SIZE];

	ZeroMemory(&lvi, sizeof(LV_ITEM));

	lvi.mask = LVIF_TEXT;
	lvi.pszText = text;
	lvi.cchTextMax = BTH_MAX_NAME_SIZE;
	lvi.iItem = i;						// row index
	lvi.iSubItem = 1;					// column index 1

	ListView_GetItem(hList, &lvi);

	wcscpy_s(str, BTH_MAX_NAME_SIZE, text);
	p = str;

	EndDialog(hWnd, LOWORD(wParam));
}

void CConnectDialog::OnCancel(HWND hWnd, WPARAM wParam)
{
	p = NULL;

	EndDialog(hWnd, LOWORD(wParam));
}

void CConnectDialog::OnItemChanged(HWND hWnd, LPARAM lParam)
{
	LPNM_LISTVIEW lpnmlv = (LPNM_LISTVIEW)lParam;

	HWND hOK = GetDlgItem(hWnd, IDOK);
	bool enable = ((lpnmlv->uNewState & LVIS_SELECTED) == LVIS_SELECTED) ? true : false;

	EnableWindow(hOK, enable);
}

//              x1      x2        x3
//           y1 +---------------------+
//              |                     |
//              |                     | h1
//              |        w1           |
//              +---------------------+
//           y2         +----+   +----+
//                      |    |   |    | h2
//                      +----+   +----+
//                        w2

void CConnectDialog::OnSize(HWND hWnd, int width, int height)
{
	HWND hOK, hCancel, hList;
	RECT rect;
	int w1, w2, h1, h2, x1, x2, x3, y1, y2;

	hList = GetDlgItem(hWnd, IDC_LIST2);
	hOK = GetDlgItem(hWnd, IDOK);
	hCancel = GetDlgItem(hWnd, IDCANCEL);

	GetWindowRect(hOK, &rect);

	w2 = rect.right - rect.left;
	h2 = rect.bottom - rect.top;

	w1 = width - 16;
	h1 = height - h2 - 24;

	x1 = 8;
	x2 = width - 2 * w2 - 16;
	x3 = width - w2 - 8;

	y1 = 8;
	y2 = height - h2 - 8;


	MoveWindow(hList, x1, y1, w1, h1, TRUE);
	MoveWindow(hCancel, x3, y2, w2, h2, TRUE);
	MoveWindow(hOK, x2, y2, w2, h2, TRUE);
}

wchar_t* CConnectDialog::Show(HWND hWnd, HINSTANCE hInstance, DLGPROC pProc)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, pProc);

	return p;
}

INT_PTR CConnectDialog::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message) {
	case WM_INITDIALOG:
		OnInitDialog(hWnd);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			OnOk(hWnd, wParam);
			return (INT_PTR)TRUE;

		case IDCANCEL:
			OnCancel(hWnd, wParam);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case LVN_ITEMCHANGED:
			OnItemChanged(hWnd, lParam);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_SIZE:
		OnSize(hWnd, LOWORD(lParam), HIWORD(lParam));
		return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}
