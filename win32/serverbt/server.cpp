
#include "framework.h"
#include "server.h"

CServer::CServer()
{
	thread = NULL;
	hWnd = NULL;
	serversocket = INVALID_SOCKET;
}

CServer::~CServer()
{
	if (thread != NULL) CloseHandle(thread);
}

void CServer::Run(HWND hWnd)
{
	DWORD id;

	this->hWnd = hWnd;

	if (thread != NULL) CloseHandle(thread);

	thread = CreateThread(NULL, 0, Function, this, 0, &id);
}

void CServer::Shutdown()
{
	int errcode = closesocket(serversocket);

	if (errcode == SOCKET_ERROR)
		SendMessage(hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
}

DWORD WINAPI CServer::Function(LPVOID lpParam)
{
	CServer* p = (CServer*)lpParam;

	SOCKET clientsocket = INVALID_SOCKET;
	int errcode, sablen;
	SOCKADDR_BTH sab = { 0 };
	LPCSADDR_INFO lpcsai = NULL;
	WSAQUERYSET wsaqs = { 0 };

	SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)L"The server thread has started.");

	p->serversocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

	if (p->serversocket == INVALID_SOCKET) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
		goto DITO;
	}

	sab.addressFamily = AF_BTH;
	sab.port = BT_PORT_ANY;

	errcode = bind(p->serversocket, (struct sockaddr*)&sab, sizeof(SOCKADDR_BTH));

	if (errcode == SOCKET_ERROR) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
		goto DITO;
	}

	sablen = sizeof(SOCKADDR_BTH);
	errcode = getsockname(p->serversocket, (struct sockaddr*)&sab, &sablen);

	if (errcode == SOCKET_ERROR) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
		goto DITO;
	}

	lpcsai = (LPCSADDR_INFO)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CSADDR_INFO));

	if (lpcsai == NULL) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)L"Di maka allocate ng memory para sa CSADDR_INFO.");
		goto DITO;
	}

	lpcsai[0].LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
	lpcsai[0].LocalAddr.lpSockaddr = (LPSOCKADDR)&sab;

	lpcsai[0].RemoteAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
	lpcsai[0].RemoteAddr.lpSockaddr = (LPSOCKADDR)&sab;

	lpcsai[0].iSocketType = SOCK_STREAM;
	lpcsai[0].iProtocol = BTHPROTO_RFCOMM;

	ZeroMemory(&wsaqs, sizeof(WSAQUERYSET));

	wsaqs.dwSize = sizeof(WSAQUERYSET);
	wsaqs.lpServiceClassId = (LPGUID)&guidServiceClass;
	wsaqs.lpszServiceInstanceName = (LPWSTR)L"MSI Laptop Computer";
	wsaqs.lpszComment = (LPWSTR)L"Ito ay program ng client-server model gamit ang bluetooth";
	wsaqs.dwNameSpace = NS_BTH;
	wsaqs.dwNumberOfCsAddrs = 1;
	wsaqs.lpcsaBuffer = lpcsai;

	errcode = WSASetService(&wsaqs, RNRSERVICE_REGISTER, 0);

	if (lpcsai != NULL)
		HeapFree(GetProcessHeap(), 0, lpcsai);

	if (errcode == SOCKET_ERROR) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
		goto DITO;
	}

	errcode = listen(p->serversocket, SOMAXCONN);

	if (errcode == SOCKET_ERROR) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
		goto DITO;
	}

	SendMessage(p->hWnd, WM_SERVER_RUNNING, 0, 0);

	while (true) {

		clientsocket = accept(p->serversocket, NULL, NULL);
		if (clientsocket == INVALID_SOCKET) break;

		SendMessage(p->hWnd, WM_CREATE_CLIENT, 0, (LPARAM)&clientsocket);
	}

DITO:

	SendMessage(p->hWnd, WM_SERVER_SHUTTING_DOWN, 0, 0);
	SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)L"The server thread has exited.");

	return 0;
}
