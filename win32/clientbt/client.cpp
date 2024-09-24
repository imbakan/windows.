
#include "framework.h"
#include "client.h"

// contructor
CClient::CClient()
{
	thread1 = NULL;
	hWnd = NULL;
}

// destructor
CClient::~CClient()
{
	if (thread1 != NULL) CloseHandle(thread1);

	int i;

	for (i = 0; i < drive_count; i++)
		delete[] drive_letter[i];

	delete[] drive_letter;

	for (i = 0; i < drive_count; i++)
		delete[] drive_name[i];

	delete[] drive_name;

}

//
void CClient::SetHandle(HTREEITEM hItem)
{
	this->hItem = hItem;
}

//
void CClient::Run(HWND hWnd, HWND hTree, HWND hList, HWND hEdit, wchar_t* address, wchar_t* name)
{
	DWORD id;

	this->hWnd = hWnd;
	this->hTree = hTree;
	this->hList = hList;
	this->hEdit = hEdit;
	wcscpy_s(this->address, MAX_ADDRESS_SIZE, address);
	wcscpy_s(this->my_name, MAX_NAME_STRING, name);

	if (thread1 != NULL) CloseHandle(thread1);
	thread1 = CreateThread(NULL, 0, Function1, this, 0, &id);
}

//
void CClient::Shutdown()
{
	int errcode = shutdown(socket, SD_SEND);

	if (errcode == SOCKET_ERROR)
		SendMessage(hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

//
void CClient::OutputBuffer(const char* txt, char* buffer, int count)
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
void CClient::FormatWithCommas(wchar_t* str, int size, unsigned __int64 num)
{
	unsigned __int64 quo, rem, dividend;
	wchar_t* str1, * str2, * str3;

	str1 = new wchar_t[size];
	str2 = new wchar_t[size];
	str3 = new wchar_t[size];

	quo = num;

	wcscpy_s(str2, size, L"");

	do {

		dividend = quo;

		quo = dividend / 1000LL;
		rem = dividend % 1000LL;

		if (quo == 0)
			swprintf_s(str1, size, L",%lld", rem);
		else
			swprintf_s(str1, size, L",%03lld", rem);

		wcscpy_s(str3, size, str1);
		wcscat_s(str3, size, str2);
		wcscpy_s(str2, size, str3);

	} while (quo > 0);

	wcscpy_s(str, size, &str3[1]);

	delete[] str1;
	delete[] str2;
	delete[] str3;
}

//
void CClient::GetDrives(wchar_t*** letter, wchar_t*** name, int* count)
{
	wchar_t buffer[MAX_PATH], drive[MAX_PATH], volume[MAX_PATH], str1[MAX_PATH], str2[MAX_PATH];
	wchar_t* pbuffer;
	size_t i, n;
	CQueue_s que1, que2;

	if (GetLogicalDriveStrings(MAX_PATH, buffer)) {

		// kunin ang unang drive
		pbuffer = buffer;

		// kung ito ay null, wala nang mga drive
		while (*pbuffer) {

			// ito ay ang drive volume (Windows)
			GetVolumeInformation(pbuffer, volume, MAX_PATH, NULL, NULL, NULL, NULL, 0);

			// ito ay ang drive letter (C:)
			wcscpy_s(drive, MAX_PATH, pbuffer);
			n = wcslen(drive);
			drive[n - 1] = '\0';

			// kung walang volume, ilagay Local Disk
			if (wcslen(volume) == 0)
				wcscpy_s(volume, MAX_PATH, L"Local Disk");

			// pagsamahin ang drive letter at volume, ito ang drive name
			swprintf_s(str1, MAX_PATH, L"%s (%s)", volume, drive);

			// iadd 'to sa list
			que1.Add(str1);
			que2.Add(drive);

			// kukunin ang susunod na drive
			pbuffer += n + 1;
		}

		// ilagay sa array ang nasa list
		*count = que1.GetCount();
		*name = new wchar_t* [*count];
		*letter = new wchar_t* [*count];

		i = 0;

		while (!que1.IsEmpty()) {

			que1.Remove(str1, MAX_PATH);
			que2.Remove(str2, MAX_PATH);

			n = wcslen(str1) + 1;
			(*name)[i] = new wchar_t[n];
			wcscpy_s((*name)[i], n, str1);

			n = wcslen(str2) + 1;
			(*letter)[i] = new wchar_t[n];
			wcscpy_s((*letter)[i], n, str2);

			++i;
		}
	}
}


//   |<--------------------------------- BUFFER_SIZE ------------------------------->|
//             |<------------ on hand ----------->|<---------- buffer size --------->|
//   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//   |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 |
//   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//                ^                                  ^
//                |                                  |
//             index[0]                           index[1]

void CClient::OnNeedData(char* buffer, int* index, int* buffer_size, bool* needdata, CQueue_i* order, int* next)
{
	int i, k, n;

	n = index[1] - index[0];
	k = index[0];
	for (i = 0; i < n; i++) {
		buffer[i] = buffer[k++];
	}

	index[0] = 0;
	index[1] = n;

	*buffer_size = BUFFER_SIZE - index[1];

	*needdata = true;

	order->Remove(next);
}

// iset dito ang order ng execution
void CClient::OnSetOrder(bool* needdata, CQueue_i* order, int* next, int cmd)
{
	*needdata = false;

	switch (cmd) {
	case STRINGS:
		order->Add(GET_INTEGER);
		order->Add(GET_STRING);
		order->Add(COLLECT_STRING);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case ATTRIBUTES:
		order->Add(GET_INTEGER);
		order->Add(GET_STRING);
		order->Add(GET_LONGLONG);
		order->Add(COLLECT_ATTRIBUTE);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case LEAVE:
		order->Add(GET_LONGLONG);
		order->Add(DEPOPULATE_BY_DEVICE);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case CONFIRM:
		order->Add(GET_LONGLONG);
		order->Add(ID_RECEIVED);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REPLY_DEVICE:
		order->Add(GET_INTEGER);
		order->Add(GET_STRING);
		order->Add(GET_LONGLONG);
		order->Add(POPULATE_WITH_DEVICE);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REPLY_DEVICES:
		order->Add(POPULATE_WITH_DEVICES);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REPLY_DRIVE:
		order->Add(POPULATE_WITH_DRIVES);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REPLY_DIRECTORY:
		order->Add(POPULATE_WITH_DIRECTORIES);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REPLY_FILE:
		order->Add(POPULATE_WITH_FILES);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REQUEST_DRIVE:
		order->Add(GET_LONGLONG);
		order->Add(ENUMERATE_DRIVES);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REQUEST_DIRECTORY:
		order->Add(GET_LONGLONG);
		order->Add(ENUMERATE_DIRECTORIES);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REQUEST_FILE:
		order->Add(GET_LONGLONG);
		order->Add(ENUMERATE_FILES);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REQUEST_CONTENT:
		order->Add(GET_LONGLONG);
		order->Add(ENUMERATE_CONTENTS);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	default:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
	}

	order->Remove(next);
}

//
void CClient::OnGetInteger(char* buffer, int* index, CQueue_i* order, int* next, int* value)
{
	int avail_size, req_size;
	int* p;

	avail_size = index[1] - index[0];
	req_size = sizeof(int);

	if (avail_size < req_size) {
		order->Return(GET_INTEGER);
		*next = NEED_DATA;
		return;
	}

	p = (int*)&buffer[index[0]];
	*value = *p;

	index[0] += req_size;

	order->Remove(next);
}

//
void CClient::OnGetLongLong(char* buffer, int* index, CQueue_i* order, int* next, long long* value)
{
	int avail_size, req_size;
	long long* p;

	avail_size = index[1] - index[0];
	req_size = sizeof(long long);

	if (avail_size < req_size) {
		order->Return(GET_LONGLONG);
		*next = NEED_DATA;
		return;
	}

	p = (long long*)&buffer[index[0]];
	*value = *p;

	index[0] += req_size;

	order->Remove(next);
}

//
void CClient::OnGetString(char* buffer, int* index, CQueue_i* order, int* next, int len, wchar_t** str)
{
	int avail_size, req_size, i, k, size;

	size = sizeof(wchar_t);
	avail_size = index[1] - index[0];
	req_size = len * size;

	if (avail_size < req_size) {
		order->Return(GET_STRING);
		*next = NEED_DATA;
		return;
	}

	if (*str != NULL) delete[] * str;
	*str = new wchar_t[len + 1];

	k = index[0];

	for (i = 0; i < len; i++) {
		*(*str + i) = *((wchar_t*)&buffer[k]);
		k += size;
	}

	*(*str + len) = '\0';

	index[0] += req_size;

	order->Remove(next);
}

//
void CClient::OnCollectString(CQueue_i* order, int* next, CQueue_s* que, wchar_t* str)
{
	que->Add(str);
	order->Remove(next);
}

//
void CClient::OnCollectAttribute(CQueue_i* order, int* next, CQueue_a* que, wchar_t* str, long long value)
{
	que->Add(str, value);
	order->Remove(next);
}

//
void CClient::OnRunOnce(CQueue_i* order, int* next)
{
	Send(JOIN);
	Send(my_name);

	order->Remove(next);
}

//
void CClient::OnIdReceived(CQueue_i* order, int* next, long long value)
{
	my_id = value;

	wchar_t str[100];
	swprintf_s(str, 100, L"The client thread 0x%llx has started.", GetId());
	SendMessage(hWnd, WM_LOG_MESSAGE, 0, (LPARAM)str);

	order->Remove(next);
}

// 
void CClient::OnDepopulateByDevice(CQueue_i* order, int* next, long long value)
{
	HTREEITEM hItem;
	TV_ITEM tvi;
	LPARAM lParam1, lParam2;
	TREE_VIEW_DATA* data = NULL;

	// hanapin ang tree node na ang id ay equal sa value
	lParam1 = 0;
	hItem = TreeView_GetRoot(hTree);

	while (hItem != NULL) {

		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
		tvi.hItem = hItem;

		TreeView_GetItem(hTree, &tvi);
		lParam2 = tvi.lParam;

		if (lParam1 == lParam2) break;

		data = (TREE_VIEW_DATA*)lParam2;
		if (data->value == value) break;

		lParam1 = lParam2;

		hItem = TreeView_GetNextSibling(hTree, hItem);
	}

	// idelete ang allocated memmory at ang tree node nato
	delete data;

	TreeView_DeleteItem(hTree, hItem);
	ListView_DeleteAllItems(hList);
	SetWindowText(hEdit, L"");

	order->Remove(next);
}

//
void CClient::OnPopulateWithDevice(CQueue_i* order, int* next, wchar_t* str, long long value)
{
	TVINSERTSTRUCT tvinsert;
	TREE_VIEW_DATA* data;
	int n;
	wchar_t text[100];

	tvinsert.hParent = NULL;
	tvinsert.hInsertAfter = TVI_ROOT;
	tvinsert.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	tvinsert.item.hItem = NULL;
	tvinsert.item.state = 0;
	tvinsert.item.stateMask = 0;
	tvinsert.item.iImage = 0;
	tvinsert.item.iSelectedImage = 1;
	tvinsert.item.cChildren = 1;

	data = new TREE_VIEW_DATA;
	data->value = value;
	data->is_collapse = true;

	tvinsert.item.pszText = str;
	tvinsert.item.cchTextMax = (int)wcslen(str);
	tvinsert.item.lParam = (LPARAM)data;
	TreeView_InsertItem(hTree, &tvinsert);

	n = TreeView_GetCount(hTree);

	swprintf_s(text, 100, L"%d devices", n);
	SetWindowText(hEdit, text);

	order->Remove(next);
}

//
void CClient::OnPopulateWithDevices(CQueue_i* order, int* next, CQueue_a* que)
{
	TVINSERTSTRUCT tvinsert;
	TREE_VIEW_DATA* data;
	long long value;
	int n;
	wchar_t str[MAX_PATH];

	tvinsert.hParent = NULL;
	tvinsert.hInsertAfter = TVI_ROOT;
	tvinsert.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	tvinsert.item.hItem = NULL;
	tvinsert.item.state = 0;
	tvinsert.item.stateMask = 0;
	tvinsert.item.iImage = 0;
	tvinsert.item.iSelectedImage = 1;
	tvinsert.item.cChildren = 1;

	while (!que->IsEmpty()) {

		que->Remove(str, MAX_PATH, &value);

		data = new TREE_VIEW_DATA;
		data->value = value;
		data->is_collapse = true;

		tvinsert.item.pszText = str;
		tvinsert.item.cchTextMax = (int)wcslen(str);
		tvinsert.item.lParam = (LPARAM)data;

		TreeView_InsertItem(hTree, &tvinsert);
	}

	n = TreeView_GetCount(hTree);

	swprintf_s(str, MAX_PATH, L"%d devices", n);
	SetWindowText(hEdit, str);

	order->Remove(next);
}

//
void CClient::OnPopulateNode(CQueue_i* order, int* next, CQueue_s* que, int image, int selectedimage)
{
	TVINSERTSTRUCT tvis;
	TV_ITEM tvi;
	TREE_VIEW_DATA* data;
	long long value;
	wchar_t str[MAX_PATH];

	// kunin ang id number ng node na 'to
	ZeroMemory(&tvi, sizeof(TV_ITEM));

	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;

	TreeView_GetItem(hTree, &tvi);

	data = (TREE_VIEW_DATA*)tvi.lParam;
	value = data->value;

	// idagdag ang mga child sa node na 'to
	// iset ang id number na nakuha sa itaas sa bawat child na idadagdag
	// iset din ang collapse indicator sa bawat child na idadagdag
	tvis.hParent = hItem;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	tvis.item.hItem = NULL;
	tvis.item.state = 0;
	tvis.item.stateMask = 0;
	tvis.item.iImage = image;
	tvis.item.iSelectedImage = selectedimage;
	tvis.item.cChildren = 1;

	while (!que->IsEmpty()) {

		que->Remove(str, MAX_PATH);

		data = new TREE_VIEW_DATA;
		data->value = value;
		data->is_collapse = true;

		tvis.item.pszText = str;
		tvis.item.cchTextMax = (int)wcslen(str);
		tvis.item.lParam = (LPARAM)data;

		TreeView_InsertItem(hTree, &tvis);
	}

	// baguhin ang collapse indicator ng node na 'to sa false
	// kasi ito ay expand na
	ZeroMemory(&tvi, sizeof(TV_ITEM));

	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;

	TreeView_GetItem(hTree, &tvi);

	data = (TREE_VIEW_DATA*)tvi.lParam;
	value = data->value;

	delete data;

	data = new TREE_VIEW_DATA;
	data->value = value;
	data->is_collapse = false;

	ZeroMemory(&tvi, sizeof(TV_ITEM));

	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;
	tvi.lParam = (LPARAM)data;

	TreeView_SetItem(hTree, &tvi);

	// iexpand ang node
	TreeView_Expand(hTree, hItem, TVE_EXPAND);

	order->Remove(next);
}

//
void CClient::OnPopulateWithFiles(CQueue_i* order, int* next, CQueue_s* que)
{
	LV_ITEM lvi;
	int index1, index2;
	wchar_t str1[MAX_PATH], str2[100], str3[100];

	que->Remove(str1, MAX_PATH);

	SetWindowText(hEdit, str1);

	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = 0;
	lvi.stateMask = 0;
	lvi.iImage = 6;

	index1 = index2 = 0;

	while (!que->IsEmpty()) {

		que->Remove(str1, MAX_PATH);
		que->Remove(str2, 100);
		que->Remove(str3, 100);

		lvi.iItem = index2;
		lvi.iSubItem = 0;
		lvi.pszText = str1;

		ListView_InsertItem(hList, &lvi);					// column 1
		ListView_SetItemText(hList, index2, 1, str2);		// column 2
		ListView_SetItemText(hList, index2, 2, str3);		// column 3

		++index2;
	}

	order->Remove(next);
}

//
void CClient::OnEnumerateDrives(CQueue_i* order, int* next, long long value)
{
	int i;
	long long size, count;

	// kompyutin ang size na isesend sa destination client
	count = 0LL;

	for (i = 0; i < drive_count; i++)
		count += wcslen(drive_name[i]);

	size = count * sizeof(wchar_t) + 2LL * drive_count * sizeof(int) + sizeof(int);

	// isend ang mga ito sa server
	// ang value ay id number ng destination client
	Send(FORWARD);
	Send(value);
	Send(size);

	for (i = 0; i < drive_count; i++) {

		Send(STRINGS);
		Send(drive_name[i]);
	}

	Send(REPLY_DRIVE);

	order->Remove(next);
}

//
void CClient::OnEnumerateDirectories(CQueue_i* order, int* next, CQueue_s* que, long long value)
{
	HANDLE handle;
	WIN32_FIND_DATA data;
	CQueue_s array_s;
	int i;
	long long count, size;
	wchar_t str[MAX_PATH], path[MAX_PATH];

	// buuin ang path name
	que->Remove(str, MAX_PATH);

	for (i = 0; i < drive_count; i++)
		if (wcscmp(str, drive_name[i]) == 0)
			break;

	wcscpy_s(path, MAX_PATH, drive_letter[i]);

	while (!que->IsEmpty()) {
		que->Remove(str, MAX_PATH);
		wcscat_s(path, MAX_PATH, L"\\");
		wcscat_s(path, MAX_PATH, str);
	}

	wcscat_s(path, MAX_PATH, L"\\*.*");

	// kunin ang mga directory at
	// kompyutin ang size na isesend sa destination client
	handle = FindFirstFile(path, &data);
	if (handle != INVALID_HANDLE_VALUE) {

		count = 0LL;

		do {
			if (!wcscmp(data.cFileName, L"..") || !wcscmp(data.cFileName, L".")) continue;
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				count += wcslen(data.cFileName);
				array_s.Add(data.cFileName);
			}
		} while (FindNextFile(handle, &data));

		FindClose(handle);

		size = 2LL * array_s.GetCount() * sizeof(int) + count * sizeof(wchar_t) + sizeof(int);
	}

	// isend ang mga ito sa server kung meron
	// ang value ay id number ng destination client
	if (!array_s.IsEmpty()) {

		Send(FORWARD);
		Send(value);
		Send(size);

		while (!array_s.IsEmpty()) {

			array_s.Remove(str, MAX_PATH);

			Send(STRINGS);
			Send(str);
		}

		Send(REPLY_DIRECTORY);
	}

	order->Remove(next);
}

//
void CClient::OnEnumerateFiles(CQueue_i* order, int* next, CQueue_s* que, long long value) {
	HANDLE handle;
	WIN32_FIND_DATA data;
	SYSTEMTIME t;
	CQueue_s array_s;
	int i, n1, n2, hh;
	long long count, size;
	wchar_t str1[MAX_PATH], str2[50], str3[100], str4[3];
	wchar_t str[MAX_PATH], path[MAX_PATH];

	// buuin ang path name
	que->Remove(str, MAX_PATH);

	for (i = 0; i < drive_count; i++)
		if (wcscmp(str, drive_name[i]) == 0)
			break;

	wcscpy_s(path, MAX_PATH, drive_letter[i]);

	while (!que->IsEmpty()) {
		que->Remove(str, MAX_PATH);
		wcscat_s(path, MAX_PATH, L"\\");
		wcscat_s(path, MAX_PATH, str);
	}

	wcscat_s(path, MAX_PATH, L"\\*.*");

	// kunin ang mga files at
	// kompyutin ang size na isesend sa destination client
	handle = FindFirstFile(path, &data);
	if (handle != INVALID_HANDLE_VALUE) {

		count = 0LL;
		n1 = n2 = 0;

		do {
			if (!wcscmp(data.cFileName, L"..") || !wcscmp(data.cFileName, L".")) continue;
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				++n1;
			}
			else {
				FileTimeToSystemTime(&data.ftLastWriteTime, &t);

				if (t.wHour == 0) {
					hh = 12;
					wcscpy_s(str4, 3, L"AM");
				}
				else if (t.wHour < 12) {
					hh = t.wHour;
					wcscpy_s(str4, 3, L"AM");

				}
				else if (t.wHour == 12) {
					hh = t.wHour;
					wcscpy_s(str4, 3, L"PM");
				}
				else {
					hh = t.wHour - 12;
					wcscpy_s(str4, 3, L"PM");
				}

				wcscpy_s(str1, MAX_PATH, data.cFileName);
				swprintf_s(str2, 50, L"%2d/%02d/%04d %2d:%02d:%02d %s", t.wMonth, t.wDay, t.wYear, hh, t.wMinute, t.wSecond, str4);

				size = (unsigned __int64)data.nFileSizeHigh * ((unsigned __int64)MAXDWORD + 1) + (unsigned __int64)data.nFileSizeLow;
				FormatWithCommas(str3, 100, size);

				count += wcslen(str1);
				count += wcslen(str2);
				count += wcslen(str3);

				array_s.Add(str1);
				array_s.Add(str2);
				array_s.Add(str3);

				++n2;
			}
		} while (FindNextFile(handle, &data));

		FindClose(handle);

		swprintf_s(str, MAX_PATH, L"%d files %d folders", n2, n1);

		size = 2LL * array_s.GetCount() * sizeof(int) + count * sizeof(wchar_t) + sizeof(int);
		size += (2LL * sizeof(int) + wcslen(str) * sizeof(wchar_t));
	}

	// isend ang mga ito sa server kung meron
	// ang value ay id number ng destination client
	if (!array_s.IsEmpty()) {

		Send(FORWARD);
		Send(value);
		Send(size);

		Send(STRINGS);
		Send(str);

		while (!array_s.IsEmpty()) {

			array_s.Remove(str, MAX_PATH);

			Send(STRINGS);
			Send(str);
		}

		Send(REPLY_FILE);
	}

	order->Remove(next);
}

//
void CClient::OnEnumerateContents(CQueue_i* order, int* next, CQueue_s* que, long long value)
{
	int i;
	wchar_t str[MAX_PATH], path[MAX_PATH];

	// buuin ang path name
	que->Remove(str, MAX_PATH);

	for (i = 0; i < drive_count; i++)
		if (wcscmp(str, drive_name[i]) == 0)
			break;

	wcscpy_s(path, MAX_PATH, drive_letter[i]);

	while (!que->IsEmpty()) {
		que->Remove(str, MAX_PATH);
		wcscat_s(path, MAX_PATH, L"\\");
		wcscat_s(path, MAX_PATH, str);
	}

	OutputDebugString(path);
	OutputDebugString(L"\n");

	order->Remove(next);
}

//
void CClient::Send(int value)
{
	const int n = sizeof(int);
	int* p;
	int count;
	char buffer[n];

	p = (int*)buffer;
	*p = value;

	count = send(socket, buffer, n, 0);

	if (count == SOCKET_ERROR)
		SendMessage(hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

//
void CClient::Send(long long value)
{
	const int n = sizeof(long long);
	long long* p;
	int count;
	char buffer[n];

	p = (long long*)buffer;
	*p = value;

	count = send(socket, buffer, n, 0);

	if (count == SOCKET_ERROR)
		SendMessage(hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

//
void CClient::Send(wchar_t* str)
{
	int count, len, n;

	len = (int)wcslen(str);
	if (len == 0) return;

	Send(len);

	n = len * sizeof(wchar_t);
	count = send(socket, (char*)str, n, 0);

	if (count == SOCKET_ERROR)
		SendMessage(hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

//
wchar_t* CClient::GetName()
{
	return my_name;
}

long long CClient::GetId()
{
	return my_id;
}

//
DWORD WINAPI CClient::Function1(LPVOID lpParam)
{
	CClient* p = (CClient*)lpParam;

	SOCKADDR_BTH sab;
	CQueue_i order;
	CQueue_s que;
	CQueue_a attribute;
	int errcode, count, buffer_size, index[2], next, addrlen;
	char* buffer;
	char* pbuffer;
	bool need_data;
	int ivalue;
	long long llvalue;
	wchar_t* string;

	SendMessage(p->hWnd, WM_CONNECTING, 0, 0);

	addrlen = sizeof(sab);
	errcode = WSAStringToAddress(p->address, AF_BTH, NULL, (LPSOCKADDR)&sab, &addrlen);
	if (errcode == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	sab.addressFamily = AF_BTH;
	sab.serviceClassId = guidServiceClass;
	sab.port = 0;

	p->socket = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (p->socket == INVALID_SOCKET)
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	errcode = connect(p->socket, (struct sockaddr*)&sab, sizeof(SOCKADDR_BTH));
	if (errcode == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	SendMessage(p->hWnd, WM_RUNNING, 0, 0);

	p->GetDrives(&p->drive_letter, &p->drive_name, &p->drive_count);

	buffer_size = BUFFER_SIZE;
	buffer = new char[buffer_size];
	count = 0;
	index[0] = index[1] = 0;
	string = NULL;
	pbuffer = NULL;
	need_data = false;
	next = RUN_ONCE;
	order.Add(GET_INTEGER);
	order.Add(SET_ORDER);

	// loop
	while (true) {

		//   |<--------------------------------- BUFFER_SIZE ------------------------------->|
		//                                                |<---------- buffer size --------->|
		//             |<------------ on hand ----------->|<------ received------->|
		//   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		//   |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 |
		//   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
		//                ^                                  ^
		//                |                                  |
		//             index[0]                           index[1]

		if (need_data) {

			count = recv(p->socket, &buffer[index[1]], buffer_size, 0);

			if (count <= 0) break;

			//p->OutputBuffer("R:", &buffer[index[1]], count);

			index[1] += count;
			need_data = false;
		}

		switch (next) {
		case SET_ORDER:						p->OnSetOrder(&need_data, &order, &next, ivalue);						break;
		case NEED_DATA:						p->OnNeedData(buffer, index, &buffer_size, &need_data, &order, &next);	break;
		case GET_INTEGER:					p->OnGetInteger(buffer, index, &order, &next, &ivalue);							break;
		case GET_LONGLONG:					p->OnGetLongLong(buffer, index, &order, &next, &llvalue);						break;
		case GET_STRING:					p->OnGetString(buffer, index, &order, &next, ivalue, &string);				break;
		case COLLECT_STRING:				p->OnCollectString(&order, &next, &que, string);								break;
		case COLLECT_ATTRIBUTE:				p->OnCollectAttribute(&order, &next, &attribute, string, llvalue);			break;
		case RUN_ONCE:						p->OnRunOnce(&order, &next);													break;
		case ID_RECEIVED:					p->OnIdReceived(&order, &next, llvalue);										break;
		case DEPOPULATE_BY_DEVICE:			p->OnDepopulateByDevice(&order, &next, llvalue);								break;
		case POPULATE_WITH_DEVICE:			p->OnPopulateWithDevice(&order, &next, string, llvalue);						break;
		case POPULATE_WITH_DEVICES:			p->OnPopulateWithDevices(&order, &next, &attribute);							break;

		case POPULATE_WITH_DRIVES:			p->OnPopulateNode(&order, &next, &que, 2, 3);					break;
		case POPULATE_WITH_DIRECTORIES:		p->OnPopulateNode(&order, &next, &que, 4, 5);					break;
		case POPULATE_WITH_FILES:			p->OnPopulateWithFiles(&order, &next, &que);									break;

		case ENUMERATE_DRIVES:				p->OnEnumerateDrives(&order, &next, llvalue);									break;
		case ENUMERATE_DIRECTORIES:			p->OnEnumerateDirectories(&order, &next, &que, llvalue);						break;
		case ENUMERATE_FILES:				p->OnEnumerateFiles(&order, &next, &que, llvalue);								break;
		case ENUMERATE_CONTENTS:			p->OnEnumerateContents(&order, &next, &que, llvalue);							break;
		}
	}

	if (string != NULL) delete[] string;
	delete[] buffer;

	errcode = shutdown(p->socket, SD_SEND);
	if (count == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	errcode = closesocket(p->socket);
	if (count == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	SendMessage(p->hWnd, WM_SHUTTING_DOWN, 0, 0);

	return 0;
}
