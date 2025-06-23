
#include "framework.h"
#include "server.h"

CServer::CServer(const char* port)
{
	strcpy_s(this->port, MAX_PORT_STRING, port);
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
	int errcode;
	struct addrinfo hints;
	struct addrinfo* result = NULL;

	SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)L"The server thread has started.");

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	errcode = getaddrinfo(NULL, p->port, &hints, &result);
	if (errcode != 0) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
		goto DITO;
	}

	p->serversocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (p->serversocket == INVALID_SOCKET) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
		freeaddrinfo(result);
		goto DITO;
	}

	errcode = bind(p->serversocket, result->ai_addr, (int)result->ai_addrlen);
	if (errcode == SOCKET_ERROR) {
		SendMessage(p->hWnd, WM_LOG_MESSAGE, 0, (LPARAM)GetErrorMessage(WSAGetLastError()));
		freeaddrinfo(result);
		closesocket(p->serversocket);
		goto DITO;
	}

	freeaddrinfo(result);

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
