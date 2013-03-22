/****************************************************************************\
*                                                                            *
*  IFC (Iris Foundation Classes) Project                                     *
*  http://ifc.googlecode.com                                                 *
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

/// @file ifc_errmsgs.h
/// Defines the error messages.

#pragma once

#include "ifc_options.h"
#include "ifc_global.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// IFC Error Message

const TCHAR* const IEM_OUT_OF_MEMORY                = TEXT("Out of memory.");
const TCHAR* const IEM_STREAM_READ_ERROR            = TEXT("Stream read error.");
const TCHAR* const IEM_STREAM_WRITE_ERROR           = TEXT("Stream write error.");
const TCHAR* const IEM_CANNOT_WRITE_RES_STREAM      = TEXT("Cannot write to a read-only resource stream.");
const TCHAR* const IEM_THREAD_RUN_ONCE              = TEXT("CThread::Run() can be call only once.");
const TCHAR* const IEM_THREAD_CREATE_ERROR          = TEXT("Error occurred while creating thread.");
const TCHAR* const IEM_STRINGS_NAME_ERROR           = TEXT("Invalid name in strings.");
const TCHAR* const IEM_DUPLICATE_STRING             = TEXT("String list does not allow duplicates.");
const TCHAR* const IEM_LIST_INDEX_ERROR             = TEXT("List index out of bounds (%d).");
const TCHAR* const IEM_LIST_CAPACITY_ERROR          = TEXT("List capacity error.");
const TCHAR* const IEM_LIST_COUNT_ERROR             = TEXT("List count error.");
const TCHAR* const IEM_SORTED_LIST_ERROR            = TEXT("Operation not allowed on sorted list.");
const TCHAR* const IEM_DUPLICATE_NAME               = TEXT("Name \"%s\" duplicated in list.");
const TCHAR* const IEM_REG_GET_DATA_FAILED          = TEXT("Failed to get data for \"%s\".");
const TCHAR* const IEM_REG_SET_DATA_FAILED          = TEXT("Failed to set data for \"%s\".");
const TCHAR* const IEM_REG_CREATE_FAILED            = TEXT("Failed to create key %s.");
const TCHAR* const IEM_INVALID_REG_TYPE             = TEXT("Invalid data type for \"%s\".");
const TCHAR* const IEM_INI_FILE_WRITE_ERROR         = TEXT("Unable to write to \"%s\".");
const TCHAR* const IEM_RESOURCE_ERROR               = TEXT("A required resource was unavailable.");
const TCHAR* const IEM_OPEN_CLIPBOARD_ERROR         = TEXT("Cannot open clipboard.");

const TCHAR* const IEM_FILE_E_NONE                  = TEXT("No error occurred.");
const TCHAR* const IEM_FILE_E_GENERIC               = TEXT("An unknown error occurred while accessing \"%s\".");
const TCHAR* const IEM_FILE_E_FILE_NOT_FOUND        = TEXT("\"%s\" not found.");
const TCHAR* const IEM_FILE_E_BAD_PATH              = TEXT("\"%s\" contains an incorrect path.");
const TCHAR* const IEM_FILE_E_TOO_MANY_OPEN_FILES   = TEXT("Could not open \"%s\" because there are too many open files.");
const TCHAR* const IEM_FILE_E_ACCESS_DENIED         = TEXT("Access to \"%s\" was denied.");
const TCHAR* const IEM_FILE_E_INVALID_FILE          = TEXT("An incorrect file handle was associated with \"%s\".");
const TCHAR* const IEM_FILE_E_REMOVE_CURRENT_DIR    = TEXT("Could not remove \"%s\" because it is the current directory.");
const TCHAR* const IEM_FILE_E_DIRECTORY_FULL        = TEXT("Could not create \"%s\" because the directory is full.");
const TCHAR* const IEM_FILE_E_BAD_SEEK              = TEXT("Seek failed on \"%s\".");
const TCHAR* const IEM_FILE_E_HARD_IO               = TEXT("Encountered a hardware I/O error while accessing \"%s\".");
const TCHAR* const IEM_FILE_E_SHARING_VIOLATION     = TEXT("Encountered a sharing violation while accessing \"%s\".");
const TCHAR* const IEM_FILE_E_LOCK_VIOLATION        = TEXT("Encountered a locking violation while accessing \"%s\".");
const TCHAR* const IEM_FILE_E_DISK_FULL             = TEXT("Disk full while accessing \"%s\".");
const TCHAR* const IEM_FILE_E_END_OF_FILE           = TEXT("Attempted to access \"%s\" past its end.");

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
