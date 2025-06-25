
#include "framework.h"
#include "client.h"

CClient::CClient()
{
	thread = NULL;
	hWnd = NULL;
}

CClient::~CClient()
{
	if (thread != NULL) CloseHandle(thread);
}

void CClient::Run(HWND hWnd, HWND hTree, HWND hList, HWND hEdit, wchar_t* address, wchar_t* name)
{
	DWORD thread_id;

	this->hWnd = hWnd;
	this->hTree = hTree;
	this->hList = hList;
	this->hEdit = hEdit;

	wcscpy_s(this->address, MAX_ADDRESS_SIZE, address);
	wcscpy_s(this->name, MAX_NAME_STRING, name);

	if (thread != NULL) CloseHandle(thread);

	thread = CreateThread(NULL, 0, Function, this, 0, &thread_id);
}

void CClient::Shutdown()
{
	int errcode = shutdown(socket, SD_SEND);

	if (errcode == SOCKET_ERROR)
		SendMessage(hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

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

	case CONNECTED:
		order->Add(GET_LONGLONG);
		order->Add(CONNECTED);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REPLY_DEVICE:
		order->Add(POPULATE_WITH_DEVICE);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REPLY_DRIVE:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REPLY_DIRECTORY:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REPLY_FILE:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REPLY_CONTENT_1:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REPLY_CONTENT_2:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case REPLY_CONTENT_3:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REQUEST_DRIVE:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REQUEST_DIRECTORY:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REQUEST_FILE:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REQUEST_CONTENT:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	default:
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
	}

	order->Remove(next);
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

void CClient::OnGetInteger(char* buffer, int* index, CQueue_i* order, int* next, int* value)
{
	int avail_size, req_size;
	int* p;

	avail_size = index[1] - index[0];
	req_size = sizeof(int);

	if (avail_size < req_size) {
		order->Push(GET_INTEGER);
		*next = NEED_DATA;
		return;
	}

	p = (int*)&buffer[index[0]];
	*value = *p;

	index[0] += req_size;

	order->Remove(next);
}

void CClient::OnGetLongLong(char* buffer, int* index, CQueue_i* order, int* next, long long* value)
{
	int avail_size, req_size;
	long long* p;

	avail_size = index[1] - index[0];
	req_size = sizeof(long long);

	if (avail_size < req_size) {
		order->Push(GET_LONGLONG);
		*next = NEED_DATA;
		return;
	}

	p = (long long*)&buffer[index[0]];
	*value = *p;

	index[0] += req_size;

	order->Remove(next);
}

void CClient::OnGetString(char* buffer, int* index, CQueue_i* order, int* next, int len, wchar_t** str)
{
	int avail_size, req_size, i, k, size;

	size = sizeof(wchar_t);
	avail_size = index[1] - index[0];
	req_size = len * size;

	if (avail_size < req_size) {
		order->Push(GET_STRING);
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

void CClient::OnCollectAttribute(CQueue_i* order, int* next, CQueue_a* que, wchar_t* str, long long value)
{
	que->Add(str, value);
	order->Remove(next);
}

void CClient::OnFirstRun(CQueue_i* order, int* next)
{
	Send(JOIN);
	Send(name);

	order->Remove(next);
}

void CClient::OnConnected(CQueue_i* order, int* next, long long value)
{
	id = value;
	order->Remove(next);
}

void CClient::OnDepopulateByDevice(CQueue_i* order, int* next, long long value)
{
	HTREEITEM hItem;
	TV_ITEM tvi;
	TREE_VIEW_DATA* data = NULL;

	//wchar_t str[100];
	//swprintf_s(str, 100, L"%10lld\n", value);
	//OutputDebugString(str);

	// hanapin ang tree node na ang id ay equal sa value
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;

	hItem = TreeView_GetRoot(hTree);

	while (hItem != NULL) {

		tvi.hItem = hItem;

		TreeView_GetItem(hTree, &tvi);
		data = (TREE_VIEW_DATA*)tvi.lParam;

		if (data->value == value) break;

		hItem = TreeView_GetNextSibling(hTree, hItem);
	}

	// idelete ang allocated memmory at ang tree node nito
	delete data;

	TreeView_DeleteItem(hTree, hItem);
	ListView_DeleteAllItems(hList);
	SetWindowText(hEdit, L"");

	order->Remove(next);
}

void CClient::OnPopulateWithDevice(CQueue_i* order, int* next, CQueue_a* que)
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

		que->Remove(str, 100, &value);

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

DWORD WINAPI CClient::Function(LPVOID lpParam)
{
	CClient* p = (CClient*)lpParam;
	SOCKADDR_BTH sab;
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	CQueue_i order;
	CQueue_a attribute;
	int errcode, count, buffer_size, index[2], next, addrlen;
	char* buffer;
	char* pbuffer;
	bool need_data;
	int ivalue;
	long long llvalue;
	wchar_t* string;

	SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)L"The client thread has started.");
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

	buffer_size = BUFFER_SIZE;
	buffer = new char[buffer_size];
	ivalue = count = 0;
	index[0] = index[1] = 0;
	string = NULL;
	pbuffer = NULL;
	need_data = false;
	next = FIRST_RUN;
	order.Add(GET_INTEGER);
	order.Add(SET_ORDER);

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

			index[1] += count;
			need_data = false;
		}

		switch (next) {
		case SET_ORDER:						p->OnSetOrder(&need_data, &order, &next, ivalue);						break;
		case NEED_DATA:						p->OnNeedData(buffer, index, &buffer_size, &need_data, &order, &next);	break;
		case GET_INTEGER:					p->OnGetInteger(buffer, index, &order, &next, &ivalue);							break;
		case GET_LONGLONG:					p->OnGetLongLong(buffer, index, &order, &next, &llvalue);						break;
		case GET_STRING:					p->OnGetString(buffer, index, &order, &next, ivalue, &string);				break;
		case COLLECT_ATTRIBUTE:				p->OnCollectAttribute(&order, &next, &attribute, string, llvalue);			break;
		case FIRST_RUN:						p->OnFirstRun(&order, &next);													break;
		case CONNECTED:						p->OnConnected(&order, &next, llvalue);											break;
		case DEPOPULATE_BY_DEVICE:			p->OnDepopulateByDevice(&order, &next, llvalue);							break;
		case POPULATE_WITH_DEVICE:			p->OnPopulateWithDevice(&order, &next, &attribute);							break;
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

	SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)L"The client thread has exited.");
	SendMessage(p->hWnd, WM_SHUTTING_DOWN, 0, 0);

	return 0;
}
