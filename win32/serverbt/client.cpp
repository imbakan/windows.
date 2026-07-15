
// nareceive ng server
//     +---------------+
//     |   Client Id   |
//     +---------------+
//     |      Size     |
//     +---------------+
//     |      Data     |
//     +---------------+

// sinend ng server
//     +---------------+
//     |   Client Id   |
//     +---------------+
//     |      Data     |
//     +---------------+

#include "framework.h"
#include "client.h"

CClient::CClient(HWND hWnd, HWND hList, SOCKET socket)
{
	thread = NULL;
	wcscpy_s(name, MAX_NAME_STRING, L"");
	this->hWnd = hWnd;
	this->hList = hList;
	this->socket = socket;
}

CClient::~CClient()
{
	if (thread != NULL) CloseHandle(thread);
}

void CClient::Run()
{
	DWORD id;

	if (thread != NULL) CloseHandle(thread);

	thread = CreateThread(NULL, 0, Function, this, 0, &id);
}

void CClient::Shutdown()
{
	int errcode = shutdown(socket, SD_SEND);

	if (errcode == SOCKET_ERROR)
		SendMessage(hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

wchar_t* CClient::GetName()
{
	return name;
}

long long CClient::GetId()
{
	return id;
}

void CClient::OnSetOrder(bool* needdata, CLinkedList<int>* order, int* next, int cmd)
{
	wchar_t str[100];
	swprintf(str, 100, L"%20d\n", cmd);
	OutputDebugString(str);

	*needdata = false;

	switch (cmd) {

	case JOIN:
		order->Add(GET_INTEGER);
		order->Add(GET_STRING);
		order->Add(RUNNING);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case FORWARD:
		order->Add(GET_LONGLONG);
		order->Add(GET_LONGLONG);
		order->Add(GET_ATTRIBUTE);
		order->Add(SEND_ATTRIBUTE);
		order->Add(FORWARD);
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

void CClient::OnNeedData(char* buffer, int* index, int* buffer_size, bool* needdata, CLinkedList<int>* order, int* next)
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

void CClient::OnGetInteger(char* buffer, int* index, CLinkedList<int>* order, int* next, int* value)
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

void CClient::OnGetLongLong(char* buffer, int* index, CLinkedList<int>* order, int* next, CLinkedList<long long>* arrayll)
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
	arrayll->Add(*p);

	index[0] += req_size;

	order->Remove(next);
}

void CClient::OnGetString(char* buffer, int* index, CLinkedList<int>* order, int* next, int len, wchar_t** str)
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

	//OutputDebugString(L"          ");
	//OutputDebugString(*str);
	//OutputDebugString(L"\n");

	order->Remove(next);
}

void CClient::OnGetAttribute(CLinkedList<int>* order, int* next, CLinkedList <long long>* arrayll, long long* size, CClient** client)
{
	CClient* item;
	LV_ITEM lvi;
	int i, n;
	long long id;

	arrayll->Remove(&id);
	arrayll->Remove(size);

	// kunin ang destination client base sa id number
	n = ListView_GetItemCount(hList);

	lvi.mask = LVIF_PARAM;

	for (i = 0; i < n; i++) {

		lvi.iItem = i;
		ListView_GetItem(hList, &lvi);

		item = (CClient*)lvi.lParam;

		if (id == item->GetId()) {
			*client = item;
			break;
		}
	}

	order->Remove(next);
}

void CClient::OnRunning(CLinkedList<int>* order, int* next, wchar_t* str)
{
	wcscpy_s(name, MAX_NAME_STRING, str);

	SendMessage(hWnd, WM_CLIENT_RUNNING, 0, (LPARAM)this);

	order->Remove(next);
}

void CClient::OnForward(char* buffer, int* index, int* buffer_size, bool* needdata, CLinkedList<int>* order, int* next, CClient* client, long long* data_size)
{
	int avail_size;

	avail_size = index[1] - index[0];

	if (avail_size < *data_size) {

		if (avail_size > 0)
			client->Send(&buffer[index[0]], avail_size);

		index[0] = index[1] = 0;
		*buffer_size = BUFFER_SIZE;
		*data_size -= avail_size;
		*needdata = true;
	}
	else {

		client->Send(&buffer[index[0]], (int) *data_size);

		index[0] += (int)*data_size;
		order->Remove(next);
	}
}

void CClient::OnSendAttribute(CLinkedList<int>* order, int* next, CClient* client)
{
	client->Send(CClient::CLIENT_ID);
	client->Send(id);

	order->Remove(next);
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
		SendMessage(hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
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
		SendMessage(hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
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
		SendMessage(hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

void CClient::Send(char* buffer, int size)
{
	int count;

	if (size == 0) return;

	Send(size);
	count = send(socket, buffer, size, 0);

	if (count == SOCKET_ERROR)
		SendMessage(hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

void CClient::PrintOutput(int cmd)
{
	switch (cmd) {
	case SET_ORDER:		OutputDebugString(L"SET_ORDER\n");		break;
	case NEED_DATA:		OutputDebugString(L"NEED_DATA\n");		break;
	case GET_INTEGER:	OutputDebugString(L"GET_INTEGER\n");	break;
	case GET_LONGLONG:	OutputDebugString(L"GET_LONGLONG\n");	break;
	case GET_STRING:	OutputDebugString(L"GET_STRING\n");		break;
	case GET_ATTRIBUTE:	OutputDebugString(L"GET_ATTRIBUTE\n");	break;
	case RUNNING:		OutputDebugString(L"RUNNING\n");		break;
	case FORWARD:		OutputDebugString(L"FORWARD\n");		break;
	case SEND_ATTRIBUTE:OutputDebugString(L"SEND_ATTRIBUTE\n"); break;
	default:			OutputDebugString(L"**************\n"); break;
	}
}

DWORD WINAPI CClient::Function(LPVOID lpParam)
{
	CClient* p = (CClient*)lpParam;
	CClient* client;
	CLinkedList<int> order;
	CLinkedList<long long> array_ll;
	CLinkedList<wchar_t*> array_s;
	int errcode, count, buffer_size, index[2], next;
	char* buffer;
	bool need_data;
	int ivalue;
	long long size;
	wchar_t* string;

	SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)L"The client thread has started.");

	client = NULL;
	buffer_size = BUFFER_SIZE;
	buffer = new char[buffer_size];
	count = 0;
	index[0] = index[1] = 0;
	need_data = true;
	string = NULL;
	next = GET_INTEGER;
	order.Add(SET_ORDER);

	p->id = (long long)GetTickCount64();

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

		p->PrintOutput(next);

		switch (next) {
		case SET_ORDER:						p->OnSetOrder(&need_data, &order, &next, ivalue);										break;
		case NEED_DATA:						p->OnNeedData(buffer, index, &buffer_size, &need_data, &order, &next);					break;
		case GET_INTEGER:					p->OnGetInteger(buffer, index, &order, &next, &ivalue);									break;
		case GET_LONGLONG:					p->OnGetLongLong(buffer, index, &order, &next, &array_ll);								break;
		case GET_STRING:					p->OnGetString(buffer, index, &order, &next, ivalue, &string);							break;
		case GET_ATTRIBUTE:					p->OnGetAttribute(&order, &next, &array_ll, &size, &client);							break;
		case RUNNING:						p->OnRunning(&order, &next, string);													break;
		case FORWARD:						p->OnForward(buffer, index, &buffer_size, &need_data, &order, &next, client, &size);	break;
		case SEND_ATTRIBUTE:                p->OnSendAttribute(&order, &next, client);												break;
		}

	}

	if (string != NULL) delete[] string;
	delete[] buffer;

	errcode = shutdown(p->socket, SD_SEND);
	if (count == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	errcode = closesocket(p->socket);
	if (count == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	PostMessage(p->hWnd, WM_CLIENT_SHUTTING_DOWN, 0, (LPARAM)p);
	SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)L"The client thread has exited.");

	return 0;
}
