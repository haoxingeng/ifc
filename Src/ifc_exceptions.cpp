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

/// @file ifc_exceptions.cpp

#include "stdafx.h"
#include "ifc_exceptions.h"
#include "ifc_sysutils.h"

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////
// CIfcException

#ifdef IFC_USE_MFC

//-----------------------------------------------------------------------------

CIfcException::CIfcException()
{
	m_bAutoDelete = TRUE;
}

//-----------------------------------------------------------------------------

BOOL CIfcException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext) const
{
	IFC_ASSERT(lpszError != NULL && AfxIsValidString(lpszError, nMaxError));

	if (lpszError == NULL || nMaxError == 0)
		return FALSE;

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	CString strErrorMsg = GetErrorMessage();
	_tcsncpy_s(lpszError, nMaxError, strErrorMsg, _TRUNCATE);

	return TRUE;
}

IMPLEMENT_DYNAMIC(CIfcException, CException)

#else

//-----------------------------------------------------------------------------

const char* CIfcException::what() const
{
	m_strWhat = GetErrorMessage();
	return m_strWhat;
}

#endif

///////////////////////////////////////////////////////////////////////////////
// CIfcOsException

CIfcOsException::CIfcOsException() :
	m_nOsError(0)
{
	// nothing
}

//-----------------------------------------------------------------------------

CIfcOsException::CIfcOsException(int nOsError) :
	m_nOsError(nOsError)
{
	// nothing
}

//-----------------------------------------------------------------------------

CString CIfcOsException::GetErrorMessage() const
{
	CString strResult;

	if (m_nOsError != 0)
		strResult.Format(SEM_OS_ERROR, m_nOsError, SysErrorMessage(m_nOsError));
	else
		strResult = SEM_UNKNOWN_OS_ERROR;

	return strResult;
}

///////////////////////////////////////////////////////////////////////////////
// CIfcFileException

//-----------------------------------------------------------------------------

CIfcFileException::CIfcFileException() :
	m_nCause(E_NONE),
	m_nOsError(0)
{
	// nothing
}

//-----------------------------------------------------------------------------

CIfcFileException::CIfcFileException(LPCTSTR lpszFileName, int nCause, int nOsError) :
	m_strFileName(lpszFileName),
	m_nCause(nCause),
	m_nOsError(nOsError)
{
	// nothing
}

//-----------------------------------------------------------------------------

int CIfcFileException::OsErrorToCause(int nOsError)
{
	switch ((UINT)nOsError)
	{
	case NO_ERROR:
		return CIfcFileException::E_NONE;
	case ERROR_FILE_NOT_FOUND:
		return CIfcFileException::E_FILE_NOT_FOUND;
	case ERROR_PATH_NOT_FOUND:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_TOO_MANY_OPEN_FILES:
		return CIfcFileException::E_TOO_MANY_OPEN_FILES;
	case ERROR_ACCESS_DENIED:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_INVALID_HANDLE:
		return CIfcFileException::E_FILE_NOT_FOUND;
	case ERROR_BAD_FORMAT:
		return CIfcFileException::E_INVALID_FILE;
	case ERROR_INVALID_ACCESS:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_INVALID_DRIVE:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_CURRENT_DIRECTORY:
		return CIfcFileException::E_REMOVE_CURRENT_DIR;
	case ERROR_NOT_SAME_DEVICE:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_NO_MORE_FILES:
		return CIfcFileException::E_FILE_NOT_FOUND;
	case ERROR_WRITE_PROTECT:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_BAD_UNIT:
		return CIfcFileException::E_HARD_IO;
	case ERROR_NOT_READY:
		return CIfcFileException::E_HARD_IO;
	case ERROR_BAD_COMMAND:
		return CIfcFileException::E_HARD_IO;
	case ERROR_CRC:
		return CIfcFileException::E_HARD_IO;
	case ERROR_BAD_LENGTH:
		return CIfcFileException::E_BAD_SEEK;
	case ERROR_SEEK:
		return CIfcFileException::E_BAD_SEEK;
	case ERROR_NOT_DOS_DISK:
		return CIfcFileException::E_INVALID_FILE;
	case ERROR_SECTOR_NOT_FOUND:
		return CIfcFileException::E_BAD_SEEK;
	case ERROR_WRITE_FAULT:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_READ_FAULT:
		return CIfcFileException::E_BAD_SEEK;
	case ERROR_SHARING_VIOLATION:
		return CIfcFileException::E_SHARING_VIOLATION;
	case ERROR_LOCK_VIOLATION:
		return CIfcFileException::E_LOCK_VIOLATION;
	case ERROR_WRONG_DISK:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_SHARING_BUFFER_EXCEEDED:
		return CIfcFileException::E_TOO_MANY_OPEN_FILES;
	case ERROR_HANDLE_EOF:
		return CIfcFileException::E_END_OF_FILE;
	case ERROR_HANDLE_DISK_FULL:
		return CIfcFileException::E_DISK_FULL;
	case ERROR_DUP_NAME:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_BAD_NETPATH:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_NETWORK_BUSY:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_DEV_NOT_EXIST:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_ADAP_HDW_ERR:
		return CIfcFileException::E_HARD_IO;
	case ERROR_BAD_NET_RESP:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_UNEXP_NET_ERR:
		return CIfcFileException::E_HARD_IO;
	case ERROR_BAD_REM_ADAP:
		return CIfcFileException::E_INVALID_FILE;
	case ERROR_NO_SPOOL_SPACE:
		return CIfcFileException::E_DIRECTORY_FULL;
	case ERROR_NETNAME_DELETED:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_NETWORK_ACCESS_DENIED:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_BAD_DEV_TYPE:
		return CIfcFileException::E_INVALID_FILE;
	case ERROR_BAD_NET_NAME:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_TOO_MANY_NAMES:
		return CIfcFileException::E_TOO_MANY_OPEN_FILES;
	case ERROR_SHARING_PAUSED:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_REQ_NOT_ACCEP:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_FILE_EXISTS:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_CANNOT_MAKE:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_ALREADY_ASSIGNED:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_INVALID_PASSWORD:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_NET_WRITE_FAULT:
		return CIfcFileException::E_HARD_IO;
	case ERROR_DISK_CHANGE:
		return CIfcFileException::E_FILE_NOT_FOUND;
	case ERROR_DRIVE_LOCKED:
		return CIfcFileException::E_LOCK_VIOLATION;
	case ERROR_BUFFER_OVERFLOW:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_DISK_FULL:
		return CIfcFileException::E_DISK_FULL;
	case ERROR_NO_MORE_SEARCH_HANDLES:
		return CIfcFileException::E_TOO_MANY_OPEN_FILES;
	case ERROR_INVALID_TARGET_HANDLE:
		return CIfcFileException::E_INVALID_FILE;
	case ERROR_INVALID_CATEGORY:
		return CIfcFileException::E_HARD_IO;
	case ERROR_INVALID_NAME:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_INVALID_LEVEL:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_NO_VOLUME_LABEL:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_NEGATIVE_SEEK:
		return CIfcFileException::E_BAD_SEEK;
	case ERROR_SEEK_ON_DEVICE:
		return CIfcFileException::E_BAD_SEEK;
	case ERROR_DIR_NOT_ROOT:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_DIR_NOT_EMPTY:
		return CIfcFileException::E_REMOVE_CURRENT_DIR;
	case ERROR_LABEL_TOO_LONG:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_BAD_PATHNAME:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_LOCK_FAILED:
		return CIfcFileException::E_LOCK_VIOLATION;
	case ERROR_BUSY:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_INVALID_ORDINAL:
		return CIfcFileException::E_INVALID_FILE;
	case ERROR_ALREADY_EXISTS:
		return CIfcFileException::E_ACCESS_DENIED;
	case ERROR_INVALID_EXE_SIGNATURE:
		return CIfcFileException::E_INVALID_FILE;
	case ERROR_BAD_EXE_FORMAT:
		return CIfcFileException::E_INVALID_FILE;
	case ERROR_FILENAME_EXCED_RANGE:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_META_EXPANSION_TOO_LONG:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_DIRECTORY:
		return CIfcFileException::E_BAD_PATH;
	case ERROR_OPERATION_ABORTED:
		return CIfcFileException::E_HARD_IO;
	case ERROR_IO_INCOMPLETE:
		return CIfcFileException::E_HARD_IO;
	case ERROR_IO_PENDING:
		return CIfcFileException::E_HARD_IO;
	case ERROR_SWAPERROR:
		return CIfcFileException::E_ACCESS_DENIED;
	default:
		return CIfcFileException::E_GENERIC;
	}
}

//-----------------------------------------------------------------------------

CString CIfcFileException::GetErrorMessage() const
{
	CString strFmt, strResult;

	switch (m_nCause)
	{
	case E_NONE:                strFmt = SEM_FILE_E_NONE;                break;
	case E_GENERIC:             strFmt = SEM_FILE_E_GENERIC;             break;
	case E_FILE_NOT_FOUND:      strFmt = SEM_FILE_E_FILE_NOT_FOUND;      break;
	case E_BAD_PATH:            strFmt = SEM_FILE_E_BAD_PATH;            break;
	case E_TOO_MANY_OPEN_FILES: strFmt = SEM_FILE_E_TOO_MANY_OPEN_FILES; break;
	case E_ACCESS_DENIED:       strFmt = SEM_FILE_E_ACCESS_DENIED;       break;
	case E_INVALID_FILE:        strFmt = SEM_FILE_E_INVALID_FILE;        break;
	case E_REMOVE_CURRENT_DIR:  strFmt = SEM_FILE_E_REMOVE_CURRENT_DIR;  break;
	case E_DIRECTORY_FULL:      strFmt = SEM_FILE_E_DIRECTORY_FULL;      break;
	case E_BAD_SEEK:            strFmt = SEM_FILE_E_BAD_SEEK;            break;
	case E_HARD_IO:             strFmt = SEM_FILE_E_HARD_IO;             break;
	case E_SHARING_VIOLATION:   strFmt = SEM_FILE_E_SHARING_VIOLATION;   break;
	case E_LOCK_VIOLATION:      strFmt = SEM_FILE_E_LOCK_VIOLATION;      break;
	case E_DISK_FULL:           strFmt = SEM_FILE_E_DISK_FULL;           break;
	case E_END_OF_FILE:         strFmt = SEM_FILE_E_END_OF_FILE;         break;
	}

	strResult.Format(strFmt, m_strFileName);
	return strResult;
}

///////////////////////////////////////////////////////////////////////////////
// Misc routines

CString GetExceptionErrMsg(CException *e)
{
#ifdef IFC_USE_MFC
	const int MAX_CHARS = 1024*4;

	CString strResult;
	PTSTR pBuffer = strResult.GetBuffer(MAX_CHARS);
	e->GetErrorMessage(pBuffer, MAX_CHARS);
	strResult.ReleaseBuffer();

	return strResult;
#else
	const char *s = e->what();
	return s ? CString(CA2T(s)) : TEXT("");
#endif
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc
