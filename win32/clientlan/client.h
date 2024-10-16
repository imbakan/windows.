
#include "queue_i.h"
#include "queue_s.h"
#include "queue_a.h"
#include "uploader.h"

#pragma once

typedef struct {
    long long value;
    bool is_collapse;
}TREE_VIEW_DATA;

class CClient
{
private:

    static const int MAX_PORT_STRING = 10;
    static const int MAX_IP_STRING = 20;
    static const int MAX_NAME_STRING = 100;

    static const int BUFFER_SIZE = 32768;

    static const int NEED_DATA = 3101;
    static const int SET_ORDER = 3102;
    static const int GET_INTEGER = 3103;
    static const int GET_LONGLONG = 3104;
    static const int GET_STRING = 3105;

    static const int ATTRIBUTES = 3201;
    static const int COLLECT_STRING = 3203;
    static const int COLLECT_ATTRIBUTE = 3204;

    static const int JOIN = 3301;
    static const int LEAVE = 3302;
    static const int CLIENT_RUNNING = 3303;
    static const int CONFIRM = 3304;
    static const int ID_RECEIVED = 3305;
    static const int RUN_ONCE = 3306;

    static const int REPLY_DEVICE = 3401;
    static const int REPLY_DEVICES = 3402;
    static const int REPLY_DRIVE = 3403;
    static const int REPLY_DIRECTORY = 3404;
    static const int REPLY_FILE = 3405;

    static const int DEPOPULATE_BY_DEVICE = 3601;
    static const int POPULATE_WITH_DEVICE = 3602;
    static const int POPULATE_WITH_DEVICES = 3603;
    static const int POPULATE_WITH_DRIVES = 3604;
    static const int POPULATE_WITH_DIRECTORIES = 3605;
    static const int POPULATE_WITH_FILES = 3606;

    static const int ENUMERATE_DRIVES = 3701;
    static const int ENUMERATE_DIRECTORIES = 3702;
    static const int ENUMERATE_FILES = 3703;
    static const int ENUMERATE_CONTENTS = 3704;

    HANDLE thread1;
    HWND hWnd, hTree, hList, hEdit;
    HTREEITEM hItem;

    int drive_count;
    long long my_id;
    char ip[MAX_IP_STRING], port[MAX_PORT_STRING];
    wchar_t my_name[MAX_NAME_STRING], ** drive_letter, ** drive_name;
    SOCKET socket;

    CUploader uploader;

    void OutputBuffer(const char* txt, char* buffer, int count);

    void FormatWithCommas(wchar_t* str, int size, unsigned __int64 num);
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

    void OnPopulateNode(CQueue_i* order, int* next, CQueue_s* que, int image, int selectedimage);
    void OnPopulateWithFiles(CQueue_i* order, int* next, CQueue_s* que);

    void OnEnumerateDrives(CQueue_i* order, int* next, long long value);
    void OnEnumerateDirectories(CQueue_i* order, int* next, CQueue_s* que, long long value);
    void OnEnumerateFiles(CQueue_i* order, int* next, CQueue_s* que, long long value);
    void OnEnumerateContents(CQueue_i* order, int* next, CQueue_s* que, long long value);

    static DWORD WINAPI Function1(LPVOID lpParam);

public:
    static const int STRINGS = 3202;
    static const int FORWARD = 3307;
    static const int REQUEST_DRIVE = 3501;
    static const int REQUEST_DIRECTORY = 3502;
    static const int REQUEST_FILE = 3503;
    static const int REQUEST_CONTENT = 3504;

    CClient(const char* port);
    ~CClient();

    void SetHandle(HTREEITEM hItem);

    void Run(HWND hWnd, HWND hTree, HWND hList, HWND hEdit, char* ip, wchar_t* name);
    void Shutdown();

    void Send(int value);
    void Send(long long value);
    void Send(wchar_t* str);

    wchar_t* GetName();
    long long GetId();
};

