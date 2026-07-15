
#include "linkedlist.h"

#pragma once

class CClient
{
private:

    static const int MAX_NAME_STRING = 100;
    static const int BUFFER_SIZE = 32768;

    static const int NEED_DATA      = 3001;
    static const int SET_ORDER      = 3002;
    static const int GET_INTEGER    = 3003;
    static const int GET_LONGLONG   = 3004;
    static const int GET_STRING     = 3005;
    static const int GET_ATTRIBUTE  = 3006;

    static const int JOIN       = 5003;
    static const int CLIENT_ID  = 5004;

public:
    static const int LEAVE      = 5005;

    static const int ATTRIBUTE  = 6001;
    static const int DEVICE     = 6002;

private:
    static const int FORWARD        = 7001;
    static const int SEND_ATTRIBUTE = 7003;

    static const int RUNNING = 8001;

    HANDLE thread;
    HWND hWnd, hList;
    SOCKET socket;
    wchar_t name[MAX_NAME_STRING];
    long long id;

    void OnSetOrder(bool* needdata, CLinkedList<int>* order, int* next, int cmd);
    void OnNeedData(char* buffer, int* index, int* buffer_size, bool* needdata, CLinkedList<int>* order, int* next);
    void OnGetInteger(char* buffer, int* index, CLinkedList<int>* order, int* next, int* value);
    void OnGetLongLong(char* buffer, int* index, CLinkedList<int>* order, int* next, CLinkedList<long long>* arrayll);
    void OnGetString(char* buffer, int* index, CLinkedList<int>* order, int* next, int len, wchar_t** str);
    void OnGetAttribute(CLinkedList<int>* order, int* next, CLinkedList <long long>* arrayll, long long* size, CClient** client);
    void OnRunning(CLinkedList<int>* order, int* next, wchar_t* str);
    void OnForward(char* buffer, int* index, int* buffer_size, bool* needdata, CLinkedList<int>* order, int* next, CClient* client, long long* data_size);
    void OnSendAttribute(CLinkedList<int>* order, int* next, CClient* client);

    void PrintOutput(int cmd);

	static DWORD WINAPI Function(LPVOID lpParam);

public:

	CClient(HWND hWnd, HWND hList, SOCKET socket);
	~CClient();

    wchar_t* GetName();
    long long GetId();

    void Run();
    void Shutdown();

    void Send(int value);
    void Send(long long value);
    void Send(wchar_t* str);
    void Send(char* value, int size);
};

