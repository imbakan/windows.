
#pragma once

class CServer
{
private:

	static const int MAX_PORT_STRING = 10;

	HANDLE thread;
	HWND hWnd;
	SOCKET serversocket;
	char port[MAX_PORT_STRING];

	static DWORD WINAPI Function(LPVOID lpParam);

public:
	CServer(const char* port);
	~CServer();

	void Run(HWND hWnd);
	void Shutdown();
};

