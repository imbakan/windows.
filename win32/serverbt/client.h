
#include "queue_i.h"

#pragma once

class CClient
{
private:
    static const int MAX_NAME_STRING    = 100;

    static const int BUFFER_SIZE = 32768;
    
    static const int NEED_DATA      = 3001;
    static const int SET_ORDER      = 3002;
    static const int GET_INTEGER    = 3003;
    static const int GET_LONGLONG   = 3004;
    static const int GET_STRING     = 3005;

    //static const int ATTRIBUTES         = 4001;
    static const int STRINGS            = 4002;
    static const int COLLECT_STRING     = 4003;
    static const int COLLECT_ATTRIBUTE  = 4004;
    
    static const int FIRST_RUN      = 5001;
    //static const int CONNECTED        = 5002;
    static const int JOIN           = 5003;
    //static const int LEAVE          = 5004;
    static const int FORWARD        = 5005;
    static const int RESEND         = 5006;
    static const int RUNNING        = 5007;

    static const int REQUEST_DRIVE      = 6001;
    static const int REQUEST_DIRECTORY  = 6002;
    static const int REQUEST_FILE       = 6003;
    static const int REQUEST_CONTENT    = 6004;
    
    //static const int REPLY_DEVICE       = 7001;
    static const int REPLY_DRIVE        = 7002;
    static const int REPLY_DIRECTORY    = 7003;
    static const int REPLY_FILE         = 7004;
    static const int REPLY_CONTENT_1    = 7005;
    static const int REPLY_CONTENT_2    = 7006;
    static const int REPLY_CONTENT_3    = 7007;

    static const int ENUMERATE_DRIVE        = 8001;
    static const int ENUMERATE_DIRECTORY    = 8002;
    static const int ENUMERATE_FILES        = 8003;
    static const int ENUMERATE_CONTENTS     = 8004;

    static const int DEPOPULATE_BY_DEVICE       = 9001;
    static const int POPULATE_WITH_DEVICE       = 9002;
    static const int POPULATE_WITH_DRIVE        = 9003;
    static const int POPULATE_WITH_DIRECTORY    = 9004;
    static const int POPULATE_WITH_FILE         = 9005;
    static const int POPULATE_WITH_CONTENT_1    = 9006;
    static const int POPULATE_WITH_CONTENT_2    = 9007;
    static const int POPULATE_WITH_CONTENT_3    = 9008;

    void OnSetOrder(bool* needdata, CQueue_i* order, int* next, int cmd);
    void OnNeedData(char* buffer, int* index, int* buffer_size, bool* needdata, CQueue_i* order, int* next);
    void OnGetInteger(char* buffer, int* index, CQueue_i* order, int* next, int* value);
    void OnGetLongLong(char* buffer, int* index, CQueue_i* order, int* next, long long* value);
    void OnGetString(char* buffer, int* index, CQueue_i* order, int* next, int len, wchar_t** str);

    void OnRunning(CQueue_i* order, int* next, wchar_t* str);

    static DWORD WINAPI Function(LPVOID lpParam);

public:
    static const int ATTRIBUTES = 4001;
    static const int CONNECTED = 5002;
    static const int LEAVE = 5004;
    static const int REPLY_DEVICE = 7001;

    HANDLE thread;
    HWND hWnd, hList;
    SOCKET socket;
    wchar_t name[MAX_NAME_STRING];
    long long id;

    CClient(HWND hWnd, HWND hList, SOCKET socket);
    ~CClient();

    void Run();
    void Shutdown();

    void Send(int value);
    void Send(long long value);
    void Send(wchar_t* str);

    wchar_t* GetName();
    long long GetId();
};

