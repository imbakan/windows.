// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define WM_MESSAGE			WM_USER + 5
#define WM_CONNECTING		WM_USER + 6
#define WM_RUNNING			WM_USER + 7
#define WM_SHUTTING_DOWN	WM_USER + 8

#define WM_POPULATE_WITH_DEVICE	WM_USER + 9
#define WM_POPULATE_WITH_DRIVE	WM_USER + 10
#define WM_POPULATE_WITH_FOLDER	WM_USER + 11
#define WM_DEPOPULATE_BY_DEVICE	WM_USER + 12

// Windows Header Files
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <initguid.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

wchar_t* GetErrorMessage(int err);

/*
comctl32.lib
ws2_32.lib
*/
