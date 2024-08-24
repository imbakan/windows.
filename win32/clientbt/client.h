
#include "queue_i.h"
#include "queue_s.h"
#include "queue_a.h"

#pragma once

DEFINE_GUID(guidServiceClass, 0x123e4567, 0xe89b, 0x12d3, 0xa4, 0x56, 0x42, 0x66, 0x14, 0x17, 0x40, 0x00);

typedef struct {
    long long value;
    bool is_collapse;
}TREE_VIEW_DATA;

class CClient
{
private:

    static const int MAX_ADDRESS_SIZE = 20;
    static const int MAX_NAME_STRING = 100;

    static const int BUFFER_SIZE = 32768;

    static const int NEED_DATA = 3101;
    static const int SET_ORDER = 3102;
    static const int GET_INTEGER = 3103;
    static const int GET_LONGLONG = 3104;
    static const int GET_STRING = 3105;

    static const int ATTRIBUTES = 3201;
    static const int STRINGS = 3202;
    static const int COLLECT_STRING = 3203;
    static const int COLLECT_ATTRIBUTE = 3204;

    static const int JOIN = 3301;
    static const int LEAVE = 3302;
    static const int CLIENT_RUNNING = 3303;
    static const int CONFIRM = 3304;
    static const int ID_RECEIVED = 3305;
    static const int RUN_ONCE = 3306;
    //    static const int FORWARD            = 3307;

    static const int REPLY_DEVICE = 3401;
    static const int REPLY_DEVICES = 3402;
    static const int REPLY_DRIVE = 3403;

    //    static const int REQUEST_DRIVE      = 3501;
    static const int REQUEST_DIRECTORY = 3502;
    static const int REQUEST_FILE = 3503;
    static const int REQUEST_CONTENT = 3504;

    static const int DEPOPULATE_BY_DEVICE = 3601;
    static const int POPULATE_WITH_DEVICE = 3602;
    static const int POPULATE_WITH_DEVICES = 3603;
    static const int POPULATE_WITH_DRIVES = 3604;

    static const int ENUMERATE_DRIVES = 3701;

    HANDLE thread1;
    HWND hWnd, hTree, hList, hEdit;
    HTREEITEM hItem;

    int drive_count;
    long long my_id;
    wchar_t my_name[MAX_NAME_STRING], ** drive_letter, ** drive_name, address[MAX_ADDRESS_SIZE];
    SOCKET socket;

    void OutputBuffer(const char* txt, char* buffer, int count);
    void OutputBuffer(const char* txt, int value);
    void OutputBuffer(const char* txt, long long value);

    void GetDrives(wchar_t*** letter, wchar_t*** name, int* count);

    void OnNeedData(char* buffer, int* index, int* buffer_size, bool* needdata, CQueue_i* order, int* next);
    void OnSetOrder(bool* needdata, CQueue_i* order, int* next, int cmd);
    void OnGetInteger(char* buffer, int* index, CQueue_i* order, int* next, int* value);
    void OnGetLongLong(char* buffer, int* index, CQueue_i* order, int* next, long long* value);
    void OnGetString(char* buffer, int* index, CQueue_i* order, int* next, int len, wchar_t** str);

    void OnCollectString(CQueue_i* order, int* next, CQueue_s* que, wchar_t* str);
    void OnCollectAttribute(CQueue_i* order, int* next, CQueue_a* que, wchar_t* str, long long value);

    void OnRunOnce(CQueue_i* order, int* next);
    void OnIdReceived(CQueue_i* order, int* next, long long value);

    void OnDepopulateByDevice(CQueue_i* order, int* next, long long value);
    void OnPopulateWithDevice(CQueue_i* order, int* next, wchar_t* str, long long value);
    void OnPopulateWithDevices(CQueue_i* order, int* next, CQueue_a* que);
    void OnPopulateWithDrives(CQueue_i* order, int* next, CQueue_s* que);

    void OnEnumerateDrives(CQueue_i* order, int* next, long long value);

    static DWORD WINAPI Function1(LPVOID lpParam);

public:

    static const int FORWARD = 3307;

    static const int REQUEST_DRIVE = 3501;

    CClient();
    ~CClient();

    void SetHandle(HTREEITEM hItem);

    void Run(HWND hWnd, HWND hTree, HWND hList, HWND hEdit, wchar_t* address, wchar_t* name);
    void Shutdown();

    void Send(int value);
    void Send(long long value);
    void Send(wchar_t* str);

    wchar_t* GetName();
    long long GetId();
};

