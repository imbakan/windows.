
#pragma once

DEFINE_GUID(guidServiceClass, 0x123e4567, 0xe89b, 0x12d3, 0xa4, 0x56, 0x42, 0x66, 0x14, 0x17, 0x40, 0x00);

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

