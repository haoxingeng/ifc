/****************************************************************************\
*                                                                            *
*  IFC (Iris Foundation Classes) Project                                     *
*  http://github.com/haoxingeng/ifc                                          *
*                                                                            *
*  Copyright 2008 HaoXinGeng (haoxingeng@gmail.com)                          *
*  All rights reserved.                                                      *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
\****************************************************************************/

/// @file ifc_global_defs.h
/// Defines the global types, consts, etc.

/// @mainpage IFC Documentation
///
/// IFC (Iris Foundation Classes) is a class library for C++ programming
/// under VC2005/VC2008/VC2010. It reimplemented many Delphi VCL classes and
/// utility routines on VC++, therefore it is particularly suitable for VC++ 
/// developers who are also familiar with Delphi.
///
/// @n@n

/// @defgroup Classes Classes
/// @defgroup Utilities Utilities
/// @defgroup GlobalDefs Global Definitions
/// @defgroup Exceptions Exceptions

#pragma once

#include "ifc_options.h"

#include <string>
#include <memory>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <hash_map>

#include <atlstr.h>
#include <atltypes.h>
#include <atltime.h>

using stdext::hash_map;

///////////////////////////////////////////////////////////////////////////////
// IFC_ASSERT

#if defined(ASSERT)
#   define IFC_ASSERT(e)     ASSERT(e)
#elif defined(_ASSERTE)
#   define IFC_ASSERT(e)     _ASSERTE(e)
#else
#   ifdef _DEBUG
#   define IFC_ASSERT(e)     assert(e)
#   else
#   define IFC_ASSERT(e)
#   endif
#endif

///////////////////////////////////////////////////////////////////////////////

/// The namespace of IFC.
namespace ifc
{

/// @addtogroup GlobalDefs
/// @{

///////////////////////////////////////////////////////////////////////////////

/// The default delimiter character used by CStrings.
#define DEFAULT_DELIMITER       TEXT(',')
/// The default quote character used by CStrings.
#define DEFAULT_QUOTE_CHAR      TEXT('\"')
/// The default name-value separator used by CStrings.
#define DEFAULT_NAME_VALUE_SEP  TEXT('=')
/// The default line break string used by CStrings.
#define DEFAULT_LINE_BREAK      TEXT("\r\n")

/// The path delimiter.
#define PATH_DELIM              TEXT('\\')
/// The drive delimiter.
#define DRIVE_DELIM             TEXT(':')

/// The return chars
#define S_CRLF                  TEXT("\r\n")

/// The string of true.
#define TRUE_STR                TEXT("true")
/// The string of false.
#define FALSE_STR               TEXT("false")

/// The maximum characters of path in ifc.
#define IFC_MAX_PATH            (1024*4)

///////////////////////////////////////////////////////////////////////////////

/// Delete and empty the object.
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)          { delete (x); (x) = NULL; }
#endif

/// Release and empty the object.
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)         { if (x) (x)->Release(); (x) = NULL; }
#endif

///////////////////////////////////////////////////////////////////////////////
// Type Definitions

/// The union of INT32
union INT32_REC
{
	INT32 value;
	struct {
		WORD lo;
		WORD hi;
	} words;
	BYTE bytes[4];
};

/// The union of INT64
union INT64_REC
{
	INT64 value;
	struct {
		INT32 lo;
		INT32 hi;
	} ints;
	WORD words[4];
	BYTE bytes[8];
};

/// Indicates where to start a seek operation.
enum SEEK_ORIGIN
{
	SO_BEGINNING    = 0,            ///< Seek from the beginning of the resource.
	SO_CURRENT      = 1,            ///< Seek from the current position in the resource.
	SO_END          = 2             ///< Seek from the end of the resource.
};

/// File opening mode
enum FILE_OPEN_MODE
{
	FM_CREATE           = 0xFF00,   ///< Create a file with the given name. If a file with the given name exists, open the file in write mode.
	FM_OPEN_READ        = 0x0000,   ///< Open the file for reading only.
	FM_OPEN_WRITE       = 0x0001,   ///< Open the file for writing only. Writing to the file completely replaces the current contents.
	FM_OPEN_READ_WRITE  = 0x0002,   ///< Open the file to modify the current contents rather than replace them.

	FM_SHARE_EXCLUSIVE  = 0x0010,   ///< Other applications can not open the file for any reason.
	FM_SHARE_DENY_WRITE = 0x0020,   ///< Other applications can open the file for reading but not for writing.
	FM_SHARE_DENY_NONE  = 0x0040    ///< No attempt is made to prevent other applications from reading from or writing to the file.
};

///////////////////////////////////////////////////////////////////////////////

/// @}

} // namespace ifc

///////////////////////////////////////////////////////////////////////////////

namespace stdext
{
	template<> inline
	size_t hash_value<CString>(const CString& s)
	{
		LPCTSTR p = s;
		return (_Hash_value(p, p + s.GetLength()));
	}
}

///////////////////////////////////////////////////////////////////////////////
