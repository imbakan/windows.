
#include "queue_i.h"

#pragma once

class CClient
{
private:

    static const int MAX_PORT_STRING    = 10;
    static const int MAX_IP_STRING      = 20;
    static const int MAX_NAME_STRING    = 100;

    static const int BUFFER_SIZE = 32768;

    static const int NEED_DATA      = 3101;
    static const int SET_ORDER      = 3102;
    static const int GET_INTEGER    = 3103;
    static const int GET_LONGLONG   = 3104;
    static const int GET_STRING     = 3105;
    static const int GET_CLIENT     = 3106;

    static const int STRINGS        = 3202;

    static const int JOIN               = 3301;
    static const int CLIENT_RUNNING     = 3303;
    static const int FORWARD            = 3307;
    static const int RESEND             = 3308;

    HANDLE thread;
    HWND hWnd, hList;
    SOCKET socket;

    wchar_t my_name[MAX_NAME_STRING];
    long long my_id;

    void OutputBuffer(const char* txt, char* buffer, int count);
    void OutputBuffer(const char* txt, int value);
    void OutputBuffer(const char* txt, long long value);

    void OnNeedData(char* buffer, int* index, int* buffer_size, bool* needdata, CQueue_i* order, int* next);
    void OnSetOrder(bool* needdata, CQueue_i* order, int* next, int msg);
    void OnGetInteger(char* buffer, int* index, CQueue_i* order, int* next, int* value);
    void OnGetLongLong(char* buffer, int* index, CQueue_i* order, int* next, long long* value, int* idx);
    void OnGetString(char* buffer, int* index, CQueue_i* order, int* next, int len, wchar_t** str);
    void OnGetClient(CQueue_i* order, int* next, long long* llvalue, CClient** client);

    void OnClientRunning(CQueue_i* order, int* next, wchar_t* str);
    void OnResend(char* buffer, int* index, int* buffer_size, bool* needdata, CQueue_i* order, int* next, CClient* client, long long* llvalue);

    static DWORD WINAPI Function(LPVOID lpParam);

public:

    static const int ATTRIBUTES = 3201;

    static const int CONFIRM = 3304;
    static const int LEAVE  = 3302;

    static const int REPLY_DEVICE = 3401;
    static const int REPLY_DEVICES = 3402;

    CClient(HWND hWnd, HWND hList, SOCKET socket);
    ~CClient();

    void Run();
    void Shutdown();

    void Send(int value);
    void Send(long long value);
    void Send(wchar_t* str);
    void Send(char* value, int size);

    wchar_t* GetName();
    long long GetId();
};

