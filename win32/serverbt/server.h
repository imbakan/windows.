
#pragma once

DEFINE_GUID(guidServiceClass, 0x070fed75, 0xdcad, 0x2d4f, 0xbc, 0x39, 0x39, 0x9b, 0xa8, 0x5b, 0x93, 0x7f);

class CServer
{
private:

	HANDLE thread;
	HWND hWnd;
	SOCKET serversocket;

	static DWORD WINAPI Function(LPVOID lpParam);

public:
	CServer();
	~CServer();

	void Run(HWND hWnd);
	void Shutdown();
};

