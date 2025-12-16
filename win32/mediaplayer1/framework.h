// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define WM_PRE_DESTROY			WM_USER + 5
#define WM_TOPO_STATUS_READY	WM_USER + 6
#define WM_SESSION_STARTED		WM_USER + 7
#define WM_SESSION_PAUSED		WM_USER + 8
#define WM_SESSION_STOPPED		WM_USER + 9
#define WM_SESSION_ENDED		WM_USER + 10

// Windows Header Files
#include <windows.h>

// Media Foundation
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <Evr.h>
#include <shlwapi.h>
#include <shobjidl.h> 
#include <codecapi.h> 

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

/*
mfplat.lib
mf.lib
mfuuid.lib
shlwapi.lib
strmiids.lib
*/
