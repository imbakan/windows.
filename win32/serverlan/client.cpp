
#include "framework.h"
#include "client.h"

// contructor
CClient::CClient(HWND hWnd, HWND hList, SOCKET socket)
{
	thread = NULL;
	wcscpy_s(my_name, MAX_NAME_STRING, L"");
	this->hWnd = hWnd;
	this->hList = hList;
	this->socket = socket;
}

// destructor
CClient::~CClient()
{
	if (thread != NULL) CloseHandle(thread);
}

//
void CClient::Run()
{
	DWORD id;

	if (thread != NULL) CloseHandle(thread);

	thread = CreateThread(NULL, 0, Function, this, 0, &id);
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
void CClient::OutputBuffer(const char* txt, int value)
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
void CClient::OutputBuffer(const char* txt, long long value)
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

//
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

//
void CClient::OnSetOrder(bool* needdata, CQueue_i* order, int* next, int msg)
{
	*needdata = false;

	switch (msg) {
	case JOIN:
		order->Add(GET_INTEGER);
		order->Add(GET_STRING);
		order->Add(CLIENT_RUNNING);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;
	case FORWARD:
		order->Add(GET_LONGLONG);
		order->Add(GET_LONGLONG);
		order->Add(GET_CLIENT);
		order->Add(RESEND);
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
void CClient::OnGetLongLong(char* buffer, int* index, CQueue_i* order, int* next, long long* value, int* idx)
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
	value[*idx] = *p;

	index[0] += req_size;

	++(*idx);
	if ((*idx) == 2) *idx = 0;

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

	if (*str != NULL) {
		delete[] * str;
	}
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
void CClient::OnGetClient(CQueue_i* order, int* next, long long* llvalue, CClient** client)
{
	LV_ITEM lvi;
	int i, n;

	n = ListView_GetItemCount(hList);

	if (n > 0) {

		lvi.mask = LVIF_PARAM;

		for (i = 0; i < n; i++) {

			lvi.iItem = i;
			ListView_GetItem(hList, &lvi);

			*client = (CClient*)lvi.lParam;

			if (llvalue[0] == (*client)->GetId()) break;
		}
	}

	order->Remove(next);
}

//
void CClient::OnClientRunning(CQueue_i* order, int* next, wchar_t* str)
{
	wcscpy_s(my_name, MAX_NAME_STRING, str);

	PostMessage(hWnd, WM_CLIENT_RUNNING, (WPARAM)0, (LPARAM)this);

	order->Remove(next);
}

//
void CClient::OnResend(char* buffer, int* index, int* buffer_size, bool* needdata, CQueue_i* order, int* next, CClient* client, long long* llvalue)
{
	long long avail_size, req_size;

	avail_size = index[1] - index[0];
	req_size = avail_size > llvalue[1] ? llvalue[1] : avail_size;

	if (req_size > 0) {

		OutputBuffer("Send", &buffer[index[0]], (int)req_size);

		client->Send(&buffer[index[0]], (int)req_size);
	}

	llvalue[1] -= req_size;
	index[0] += (int)req_size;

	if (llvalue[1] > 0LL) {

		order->Return(RESEND);
		*next = NEED_DATA;
		return;
	}

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
void CClient::Send(char* value, int size)
{
	int count;

	count = send(socket, value, size, 0);

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
DWORD WINAPI CClient::Function(LPVOID lpParam)
{
	CClient* p = (CClient*)lpParam;

	CClient* client;
	CQueue_i order;
	int errcode, count, buffer_size, index[2], next;
	char* buffer;
	bool need_data;
	int ivalue, idx;
	long long llvalue[2];
	wchar_t* string;

	buffer_size = BUFFER_SIZE;
	buffer = new char[buffer_size];
	count = 0;
	index[0] = index[1] = 0;
	string = NULL;
	need_data = true;
	next = GET_INTEGER;
	order.Add(SET_ORDER);

	client = NULL;
	llvalue[0] = llvalue[1] = 0LL;
	idx = 0;

	// ang server ay nag-aasign ng id number sa bawat kumokonektang client
	p->my_id = (long long)GetTickCount64();

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

			p->OutputBuffer("Recv", &buffer[index[1]], count);

			index[1] += count;
			need_data = false;
		}

		switch (next) {
		case SET_ORDER:			p->OnSetOrder(&need_data, &order, &next, ivalue);										break;
		case NEED_DATA:			p->OnNeedData(buffer, index, &buffer_size, &need_data, &order, &next);					break;
		case GET_INTEGER:		p->OnGetInteger(buffer, index, &order, &next, &ivalue);											break;
		case GET_LONGLONG:		p->OnGetLongLong(buffer, index, &order, &next, llvalue, &idx);									break;
		case GET_STRING:		p->OnGetString(buffer, index, &order, &next, ivalue, &string);								break;
		case GET_CLIENT:		p->OnGetClient(&order, &next, llvalue, &client);												break;
		case CLIENT_RUNNING:	p->OnClientRunning(&order, &next, string);														break;
		case RESEND:			p->OnResend(buffer, index, &buffer_size, &need_data, &order, &next, client, llvalue);	break;
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

	PostMessage(p->hWnd, WM_CLIENT_SHUTTING_DOWN, 0, (LPARAM)p);

	return 0;
}
