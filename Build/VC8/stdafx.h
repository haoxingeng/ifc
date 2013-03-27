
#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif						

#define WIN32_LEAN_AND_MEAN

// some CString constructors will be explicit
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#ifdef IFC_USE_MFC

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#else

#include <windows.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <shellapi.h>
#include <winsvc.h>

#endif
