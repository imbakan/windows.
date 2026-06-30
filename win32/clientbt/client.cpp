
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

void CClient::PrintOutput(int cmd)
{
	wchar_t str[100];

	switch (cmd) {
	case NEED_DATA: wcscpy_s(str, 100, L"NEED_DATA\n");  break;
	case SET_ORDER: wcscpy_s(str, 100, L"SET_ORDER\n");  break;
	case GET_INTEGER: wcscpy_s(str, 100, L"GET_INTEGER\n");  break;
	case GET_LONGLONG: wcscpy_s(str, 100, L"GET_LONGLONG\n");  break;
	case GET_STRING: wcscpy_s(str, 100, L"GET_STRING\n");  break;
	case GET_BUFFERS: wcscpy_s(str, 100, L"GET_BUFFERS\n");  break;
	case FIRST_RUN: wcscpy_s(str, 100, L"FIRST_RUN\n");  break;
	case COLLECT_ATTRIBUTE: wcscpy_s(str, 100, L"COLLECT_ATTRIBUTE\n");  break;
	case COLLECT_STRING: wcscpy_s(str, 100, L"COLLECT_STRING\n");  break;
	case GET_CLIENT_ID: wcscpy_s(str, 100, L"GET_CLIENT_ID\n");  break;
	case ENUMERATE_DRIVE: wcscpy_s(str, 100, L"ENUMERATE_DRIVE\n");  break;
	case ENUMERATE_FOLDER: wcscpy_s(str, 100, L"ENUMERATE_FOLDER\n");  break;
	case POPULATE_WITH_DEVICE: wcscpy_s(str, 100, L"POPULATE_WITH_DEVICE\n");  break;
	case POPULATE_WITH_DRIVE: wcscpy_s(str, 100, L"POPULATE_WITH_DRIVE\n");  break;
	case POPULATE_WITH_FOLDER: wcscpy_s(str, 100, L"POPULATE_WITH_FOLDER\n");  break;
	case DEPOPULATE_BY_DEVICE: wcscpy_s(str, 100, L"DEPOPULATE_BY_DEVICE\n");  break;
	default: wcscpy_s(str, 100, L"**********\n");
	}

	OutputDebugString(str);
}


void CClient::BufferToPathName(wchar_t* pathname, size_t size, char* buffer)
{
	int i, j, k, m, n;
	wchar_t* p;
	wchar_t str[MAX_PATH], tmp[MAX_PATH];

	k = 0;

	p = (wchar_t*)&buffer[k];
	n = *p;
	k += sizeof(wchar_t);

	//swprintf(text, 100, L"%d\n", n);
	//OutputDebugString(text);

	wcscpy_s(pathname, size, L"");

	for (i = 0; i < n; i++) {

		p = (wchar_t*)&buffer[k];
		m = *p;
		k += sizeof(wchar_t);

		//swprintf(text, 100, L"%d\n", m);
		//OutputDebugString(text);

		for (j = 0; j < m; j++) {

			str[j] = buffer[k];
			k += sizeof(wchar_t);
		}

		str[m] = L'\0';

		//OutputDebugString(str);
		//OutputDebugString(L"\n");

		if (i == 0) continue;

		if (i == 1) {
			wcscpy_s(tmp, MAX_PATH, str);
			p = wcschr(tmp, ':');
			if (p != NULL) {
				*(p + 1) = '\0';
				wcscpy_s(tmp, MAX_PATH, (p - 1));
				wcscpy_s(str, MAX_PATH, tmp);
			}
		}

		wcscat_s(pathname, size, str);
		wcscat_s(pathname, size, L"\\");
	}
}

void CClient::GetStorage(CLinkedList<wchar_t*>* que, long long* size)
{
	wchar_t root[MAX_PATH], volume[MAX_PATH], drive[MAX_PATH], str[MAX_PATH];
	wchar_t* buffer;
	int count;
	size_t n;

	//  root[0]     C
	//  root[1]     :
	//  root[2]     \
    //  root[3]     \0
	//  root[4]     D
	//  root[5]     :
	//  root[6]     \
    //  root[7]     \0
	//  root[8]     \0

	*size = 0LL;

	if (GetLogicalDriveStrings(MAX_PATH, root)) {

		buffer = root;

		while (*buffer) {

			GetVolumeInformation(buffer, volume, MAX_PATH, NULL, NULL, NULL, NULL, 0);

			if (wcslen(volume) == 0)
				wcscpy_s(volume, MAX_PATH, L"Local Disk");

			wcscpy_s(drive, MAX_PATH, buffer);
			n = wcslen(drive);
			drive[n - 1] = '\0';

			swprintf_s(str, MAX_PATH, L"%s (%s)", volume, drive);

			que->Add(str);

			*size += (wcslen(str) * sizeof(wchar_t) + sizeof(int));

			buffer += (n + 1);
		}
	}

	count = que->GetCount();
	*size += ((count + 1) * sizeof(int));
}

void CClient::GetDirectory(CLinkedList<wchar_t*>* que, wchar_t* str, long long* size)
{
	HANDLE handle;
	WIN32_FIND_DATA data;
	int count;
	wchar_t pathname[MAX_PATH];

	wcscpy_s(pathname, MAX_PATH, str);
	wcscat_s(pathname, MAX_PATH, L"*.*");

	handle = FindFirstFile(pathname, &data);
	if (handle == INVALID_HANDLE_VALUE) return;

	*size = 0LL;

	do {
		if (!wcscmp(data.cFileName, L"..") || !wcscmp(data.cFileName, L".")) continue;
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

			que->Add(data.cFileName);

			*size += (wcslen(data.cFileName) * sizeof(wchar_t) + sizeof(int));
		}
	} while (FindNextFile(handle, &data));

	count = que->GetCount();
	*size += ((count + 1) * sizeof(int));

	FindClose(handle);
}

void CClient::OnSetOrder(bool* needdata, CLinkedList<int>* order, int* next, int cmd)
{
	wchar_t str[100];
	swprintf(str, 100, L"%20d\n", cmd);
	OutputDebugString(str);

	*needdata = false;

	switch (cmd) {

	case STRINGS:
		order->Add(GET_INTEGER);
		order->Add(GET_STRING);
		order->Add(COLLECT_STRING);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case CLIENT_ID:
		order->Add(GET_LONGLONG);
		order->Add(GET_CLIENT_ID);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REQ_DRIVE:
		order->Add(ENUMERATE_DRIVE);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case REQ_FOLDER:
		order->Add(GET_INTEGER);
		order->Add(GET_BUFFERS);
		order->Add(ENUMERATE_FOLDER);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case ATTRIBUTE:
		order->Add(GET_LONGLONG);
		order->Add(GET_INTEGER);
		order->Add(GET_STRING);
		order->Add(COLLECT_ATTRIBUTE);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case DEVICE:
		order->Add(POPULATE_WITH_DEVICE);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case DRIVE:
		order->Add(POPULATE_WITH_DRIVE);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case FOLDER:
		order->Add(POPULATE_WITH_FOLDER);
		order->Add(GET_INTEGER);
		order->Add(SET_ORDER);
		break;

	case LEAVE:
		order->Add(GET_LONGLONG);
		order->Add(DEPOPULATE_BY_DEVICE);
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

void CClient::OnGetLongLong(char* buffer, int* index, CLinkedList<int>* order, int* next, long long* value)
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

void CClient::OnGetClientId(CLinkedList<int>* order, int* next, long long value, long long* id)
{
	*id = value;
	order->Remove(next);

	//wchar_t str[100];
	//swprintf(str, 100, L"%20lld\n", *id);
	//OutputDebugString(str);
}

void CClient::OnGetBuffer(char* buffer, int* index, CLinkedList<int>* order, int* next, int size, char** value)
{
	int avail_size, req_size, i, k;

	avail_size = index[1] - index[0];
	req_size = size;

	if (avail_size < req_size) {
		order->Push(GET_BUFFERS);
		*next = NEED_DATA;
		return;
	}

	if (*value != NULL) delete[] * value;
	*value = new char[size];

	k = index[0];

	for (i = 0; i < size; i++)
		*(*value + i) = buffer[k++];

	/*//////////////////////////////////////////////////////////////////////
	wchar_t str[100];
	wchar_t* p = (wchar_t*)*value;

	swprintf(str, 100, L"%x\n", *p);
	OutputDebugString(str);

	p = (wchar_t*)&(*value)[sizeof(wchar_t)];

	swprintf(str, 100, L"%x\n", *p);
	OutputDebugString(str);
	//////////////////////////////////////////////////////////////////////*/

	index[0] += req_size;

	order->Remove(next);
}

void CClient::OnFirstRun(CLinkedList<int>* order, int* next)
{
	Send(JOIN);
	Send(name);

	order->Remove(next);
}

void CClient::OnCollectAttribute(CLinkedList<int>* order, int* next, wchar_t* str, long long value, CLinkedList<long long>* ids, CLinkedList<wchar_t*>* names)
{
	ids->Add(value);
	names->Add(str);

	order->Remove(next);
}

void CClient::OnCollectString(CLinkedList<int>* order, int* next, CLinkedList<wchar_t*>* que, wchar_t* str)
{
	que->Add(str);
	order->Remove(next);
}

void CClient::OnEnumerateDrive(CLinkedList<int>* order, int* next, long long id)
{
	CLinkedList<wchar_t*> que;
	wchar_t* str;
	long long size;

	GetStorage(&que, &size);

	Send(FORWARD);
	Send(id);
	Send(size);

	while (!que.IsEmpty()) {

		que.Remove(&str);

		//OutputDebugString(L"          ");
		//OutputDebugString(str);
		//OutputDebugString(L"\n");

		Send(STRINGS);
		Send(str);

		delete[] str;
	}

	Send(DRIVE);

	order->Remove(next);
}

void CClient::OnEnumerateFolder(CLinkedList<int>* order, int* next, long long id, char* value)
{
	CLinkedList<wchar_t*> que;
	wchar_t* str;
	long long size;
	wchar_t pathname[MAX_PATH];

	BufferToPathName(pathname, MAX_PATH, value);

	OutputDebugString(L"                  ");
	OutputDebugString(pathname);
	OutputDebugString(L"\n");

	GetDirectory(&que, pathname, &size);

	Send(FORWARD);
	Send(id);
	Send(size);

	while (!que.IsEmpty()) {

		que.Remove(&str);

		//OutputDebugString(L"          ");
		//OutputDebugString(str);
		//OutputDebugString(L"\n");

		Send(STRINGS);
		Send(str);

		delete[] str;
	}

	Send(FOLDER);

	order->Remove(next);
}

void CClient::OnPopulateWithDevice(CLinkedList<int>* order, int* next, CLinkedList<long long>* ids, CLinkedList<wchar_t*>* names)
{
	SendMessage(hWnd, WM_POPULATE_WITH_DEVICE, (WPARAM)names, (LPARAM)ids);

	order->Remove(next);
}

void CClient::OnPopulateWithDrive(CLinkedList<int>* order, int* next, CLinkedList<wchar_t*>* que)
{
	SendMessage(hWnd, WM_POPULATE_WITH_DRIVE, 0, (LPARAM)que);

	order->Remove(next);
}

void CClient::OnPopulateWithFolder(CLinkedList<int>* order, int* next, CLinkedList<wchar_t*>* que)
{
	SendMessage(hWnd, WM_POPULATE_WITH_FOLDER, 0, (LPARAM)que);

	order->Remove(next);
}

void CClient::OnDepopulateByDevice(CLinkedList<int>* order, int* next, long long value)
{
	SendMessage(hWnd, WM_DEPOPULATE_BY_DEVICE, 0, (LPARAM)&value);

	order->Remove(next);
}

void CClient::Run(HWND hWnd, wchar_t* name, wchar_t* address)
{
	this->hWnd = hWnd;
	wcscpy_s(this->address, MAX_ADDRESS_SIZE, address);
	wcscpy_s(this->name, MAX_NAME_STRING, name);

	if (thread != NULL) CloseHandle(thread);

	thread = CreateThread(NULL, 0, Function, this, 0, &thread_id);
}

void CClient::Shutdown()
{
	int errcode = shutdown(socket, SD_SEND);

	if (errcode == SOCKET_ERROR)
		SendMessage(hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
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

DWORD WINAPI CClient::Function(LPVOID lpParam)
{
	CClient* p = (CClient*)lpParam;
	CLinkedList<int> order;
	CLinkedList<wchar_t*> names;
	CLinkedList<long long> ids;
	SOCKADDR_BTH sab;
	int errcode, count, buffer_size, index[2], next, addrlen;
	char* buffer;
	char* pbuffer;
	bool need_data;
	int ivalue;
	long long llvalue, id;
	wchar_t* string;
	wchar_t str[100];

	swprintf_s(str, 100, L"The client thread 0x%x has started.\n", p->thread_id);
	OutputDebugString(str);

	SendMessage(p->hWnd, WM_CONNECTING, 0, 0);

	addrlen = sizeof(sab);
	errcode = WSAStringToAddress(p->address, AF_BTH, NULL, (LPSOCKADDR)&sab, &addrlen);
	if (errcode == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	sab.addressFamily = AF_BTH;
	sab.serviceClassId = guidServiceClass;
	sab.port = 0;

	p->socket = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (p->socket == INVALID_SOCKET)
		SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	errcode = connect(p->socket, (struct sockaddr*)&sab, sizeof(SOCKADDR_BTH));
	if (errcode == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

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

		p->PrintOutput(next);

		switch (next) {
		case SET_ORDER:						p->OnSetOrder(&need_data, &order, &next, ivalue);						break;
		case NEED_DATA:						p->OnNeedData(buffer, index, &buffer_size, &need_data, &order, &next);	break;
		case GET_INTEGER:					p->OnGetInteger(buffer, index, &order, &next, &ivalue);							break;
		case GET_LONGLONG:					p->OnGetLongLong(buffer, index, &order, &next, &llvalue);						break;
		case GET_STRING:					p->OnGetString(buffer, index, &order, &next, ivalue, &string);				break;
		case GET_CLIENT_ID:					p->OnGetClientId(&order, &next, llvalue, &id);									break;
		case GET_BUFFERS:					p->OnGetBuffer(buffer, index, &order, &next, ivalue, &pbuffer);		break;
		case FIRST_RUN:						p->OnFirstRun(&order, &next);													break;
		case COLLECT_ATTRIBUTE:				p->OnCollectAttribute(&order, &next, string, llvalue, &ids, &names);			break;
		case COLLECT_STRING:				p->OnCollectString(&order, &next, &names, string);							break;
		case ENUMERATE_DRIVE:				p->OnEnumerateDrive(&order, &next, id);											break;
		case ENUMERATE_FOLDER:				p->OnEnumerateFolder(&order, &next, id, pbuffer);							break;
		case POPULATE_WITH_DEVICE:			p->OnPopulateWithDevice(&order, &next, &ids, &names);							break;
		case POPULATE_WITH_DRIVE:			p->OnPopulateWithDrive(&order, &next, &names);								break;
		case POPULATE_WITH_FOLDER:			p->OnPopulateWithFolder(&order, &next, &names);								break;
		case DEPOPULATE_BY_DEVICE:			p->OnDepopulateByDevice(&order, &next, llvalue);								break;
		}
	}

	if (string != NULL) delete[] string;
	if (pbuffer != NULL) delete[] pbuffer;
	delete[] buffer;

	errcode = shutdown(p->socket, SD_SEND);
	if (count == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	errcode = closesocket(p->socket);
	if (count == SOCKET_ERROR)
		SendMessage(p->hWnd, WM_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));

	SendMessage(p->hWnd, WM_SHUTTING_DOWN, 0, 0);

	return 0;
}
