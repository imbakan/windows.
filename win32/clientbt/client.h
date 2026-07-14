

#include "linkedlist.h"

#pragma once

DEFINE_GUID(guidServiceClass, 0x070fed75, 0xdcad, 0x2d4f, 0xbc, 0x39, 0x39, 0x9b, 0xa8, 0x5b, 0x93, 0x7f);

class CClient
{
private:

    static const int MAX_ADDRESS_SIZE = 20;
    static const int MAX_NAME_STRING = 100;

    static const int BUFFER_SIZE = 32768;

    static const int NEED_DATA = 3001;
    static const int SET_ORDER = 3002;
    static const int GET_INTEGER = 3003;
    static const int GET_LONGLONG = 3004;
    static const int GET_STRING = 3005;
    static const int GET_CLIENT_ID = 3006;
    static const int GET_BUFFERS = 3007;

    static const int BUFFERS = 4001;
    static const int STRINGS = 4002;
    static const int COLLECT_STRING = 4003;
    static const int COLLECT_ATTRIBUTE = 4004;

    static const int FIRST_RUN = 5001;
    static const int JOIN = 5003;
    static const int CLIENT_ID = 5004;
    static const int LEAVE = 5005;

    static const int ATTRIBUTE = 6001;
    static const int DEVICE = 6002;
    static const int DRIVE = 6003;
    static const int FOLDER = 6004;

public:

    static const int FORWARD = 7001;

    static const int REQ_DRIVE = 8001;
    static const int REQ_FOLDER = 8002;

private:

    static const int ENUMERATE_DRIVE = 9001;
    static const int ENUMERATE_FOLDER = 9002;

    static const int POPULATE_WITH_DEVICE = 9006;
    static const int POPULATE_WITH_DRIVE = 9007;
    static const int POPULATE_WITH_FOLDER = 9008;
    static const int DEPOPULATE_BY_DEVICE = 9009;

    HANDLE thread;
    HWND hWnd;
    SOCKET socket;
    DWORD thread_id;
    wchar_t name[MAX_NAME_STRING], address[MAX_ADDRESS_SIZE];

    void PrintOutput(int cmd);

    void BufferToPathName(wchar_t* pathname, size_t size, char* buffer);

    void GetStorage(CLinkedList<wchar_t*>* que, long long* size);
    void GetDirectory(CLinkedList<wchar_t*>* que, wchar_t* str, long long* size);

    void OnSetOrder(bool* needdata, CLinkedList<int>* order, int* next, int cmd);
    void OnNeedData(char* buffer, int* index, int* buffer_size, bool* needdata, CLinkedList<int>* order, int* next);
    void OnGetInteger(char* buffer, int* index, CLinkedList<int>* order, int* next, int* value);
    void OnGetLongLong(char* buffer, int* index, CLinkedList<int>* order, int* next, long long* value);
    void OnGetString(char* buffer, int* index, CLinkedList<int>* order, int* next, int len, wchar_t** str);
    void OnGetClientId(CLinkedList<int>* order, int* next, long long value, long long* id);
    void OnGetBuffer(char* buffer, int* index, CLinkedList<int>* order, int* next, int size, char** value);

    void OnFirstRun(CLinkedList<int>* order, int* next);
    void OnCollectAttribute(CLinkedList<int>* order, int* next, wchar_t* str, long long value, CLinkedList<long long>* ids, CLinkedList<wchar_t*>* names);
    void OnCollectString(CLinkedList<int>* order, int* next, CLinkedList<wchar_t*>* que, wchar_t* str);

    void OnEnumerateDrive(CLinkedList<int>* order, int* next, long long id);
    void OnEnumerateFolder(CLinkedList<int>* order, int* next, long long id, char* value);

    void OnPopulateWithDevice(CLinkedList<int>* order, int* next, CLinkedList<long long>* ids, CLinkedList<wchar_t*>* names);
    void OnPopulateWithDrive(CLinkedList<int>* order, int* next, CLinkedList<wchar_t*>* names);
    void OnPopulateWithFolder(CLinkedList<int>* order, int* next, CLinkedList<wchar_t*>* names);
    void OnDepopulateByDevice(CLinkedList<int>* order, int* next, long long value);

    static DWORD WINAPI Function(LPVOID lpParam);

public:
    CClient();
    ~CClient();

    void Run(HWND hWnd, wchar_t* name, wchar_t* address);
    void Shutdown();

    void Send(int value);
    void Send(long long value);
    void Send(wchar_t* str);
    void Send(char* value, int szie);

};

