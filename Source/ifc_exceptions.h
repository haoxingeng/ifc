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

/// @file ifc_exceptions.h
/// Defines the exception classes.

/// @addtogroup Exceptions
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global.h"
#include "ifc_errmsgs.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// ∫Í∂®“Â

#ifdef IFC_USE_MFC
#define IFC_EXCEPT_OBJ  new
#else
#define IFC_EXCEPT_OBJ
#endif

///////////////////////////////////////////////////////////////////////////////
/// CIfcException - The base class for all exceptions in IFC.

#ifdef IFC_USE_MFC

class CIfcException : public CException
{
    DECLARE_DYNAMIC(CIfcException)

public:
    CIfcException();
    virtual ~CIfcException() {}

    virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL) const;
    virtual CString GetErrorMessage() const { return TEXT(""); }
};

#else

class CIfcException : std::exception
{
private:
    mutable CStringA m_strWhat;
public:
    CIfcException() {}
    virtual ~CIfcException() {}

    virtual CString GetErrorMessage() const { return TEXT(""); }
    virtual const char* what() const;
};

#endif

///////////////////////////////////////////////////////////////////////////////
/// CIfcSimpleException - Simple exception

class CIfcSimpleException : public CIfcException
{
protected:
    CString m_strErrorMsg;      ///< The error message.
public:
    /// Default constructor
    CIfcSimpleException() {}

    /// Constructor using a specified error message.
    explicit CIfcSimpleException(LPCTSTR lpszErrorMsg) : m_strErrorMsg(lpszErrorMsg) {}

    /// Returns the error message.
    virtual CString GetErrorMessage() const { return m_strErrorMsg; }

    /// Sets the error message.
    void SetErrorMesssage(LPCTSTR lpszMsg) { m_strErrorMsg = lpszMsg; }
};

///////////////////////////////////////////////////////////////////////////////
/// CIfcMemoryException - Memory exception

class CIfcMemoryException : public CIfcSimpleException
{
public:
    CIfcMemoryException() {}
    explicit CIfcMemoryException(LPCTSTR lpszErrorMsg) : CIfcSimpleException(lpszErrorMsg) {}
};

///////////////////////////////////////////////////////////////////////////////
/// CIfcStreamException - Stream exception

class CIfcStreamException : public CIfcSimpleException
{
public:
    CIfcStreamException() {}
    explicit CIfcStreamException(LPCTSTR lpszErrorMsg) : CIfcSimpleException(lpszErrorMsg) {}
};

///////////////////////////////////////////////////////////////////////////////
/// CIfcRegistryException - Registry exception

class CIfcRegistryException : public CIfcSimpleException
{
public:
    CIfcRegistryException() {}
    explicit CIfcRegistryException(LPCTSTR lpszErrorMsg) : CIfcSimpleException(lpszErrorMsg) {}
};

///////////////////////////////////////////////////////////////////////////////
/// CIfcIniFileException - INI file Exception

class CIfcIniFileException : public CIfcSimpleException
{
public:
    CIfcIniFileException() {}
    explicit CIfcIniFileException(LPCTSTR lpszErrorMsg) : CIfcSimpleException(lpszErrorMsg) {}
};

///////////////////////////////////////////////////////////////////////////////
/// CIfcThreadException - Thread exception

class CIfcThreadException : public CIfcSimpleException
{
public:
    CIfcThreadException() {}
    explicit CIfcThreadException(LPCTSTR lpszErrorMsg) : CIfcSimpleException(lpszErrorMsg) {}
};

///////////////////////////////////////////////////////////////////////////////
/// CIfcFileException - File exception

class CIfcFileException : public CIfcException
{
public:
    /// CIfcFileException cause enumerators.
    enum 
    {
        E_NONE,                 ///< No error occurred.
        E_GENERIC,              ///< An unspecified error occurred.
        E_FILE_NOT_FOUND,       ///< The file could not be located.
        E_BAD_PATH,             ///< All or part of the path is invalid.
        E_TOO_MANY_OPEN_FILES,  ///< The permitted number of open files was exceeded.
        E_ACCESS_DENIED,        ///< The file could not be accessed.
        E_INVALID_FILE,         ///< There was an attempt to use an invalid file handle.
        E_REMOVE_CURRENT_DIR,   ///< The current working directory cannot be removed.
        E_DIRECTORY_FULL,       ///< There are no more directory entries.
        E_BAD_SEEK,             ///< There was an error trying to set the file pointer.
        E_HARD_IO,              ///< There was a hardware error.
        E_SHARING_VIOLATION,    ///< SHARE.EXE was not loaded, or a shared region was locked.
        E_LOCK_VIOLATION,       ///< There was an attempt to lock a region that was already locked.
        E_DISK_FULL,            ///< The disk is full.
        E_END_OF_FILE           ///< The end of file was reached.
    };

public:
    CString m_strFileName;      ///< Contains the name of the file for this exception condition.
    int m_nCause;               ///< Contains values defined by a CIfcFileException enumerated type.
    int m_nOsError;             ///< Contains the operating-system error code for this exception.
public:
    /// Default constructor.
    CIfcFileException();

    /// Constructor using file name, cause code and os error code.
    CIfcFileException(LPCTSTR lpszFileName, int nCause, int nOsError);

    /// Returns a cause code corresponding to an operating system error code.
    static int OsErrorToCause(int nOsError);

    /// Returns the error message.
    virtual CString GetErrorMessage() const;
};

///////////////////////////////////////////////////////////////////////////////
// Exception throwing rountines.

/// Throws a CIfcSimpleException exception.
inline void IfcThrowException(LPCTSTR lpszMsg)
{
    throw IFC_EXCEPT_OBJ CIfcSimpleException(lpszMsg);
}

/// Throws a CIfcMemoryException exception.
inline void IfcThrowMemoryException()
{
    throw IFC_EXCEPT_OBJ CIfcMemoryException(IEM_OUT_OF_MEMORY);
}

/// Throws a CIfcStreamException exception.
inline void IfcThrowStreamException(LPCTSTR lpszMsg)
{
    throw IFC_EXCEPT_OBJ CIfcStreamException(lpszMsg);
}

/// Throws a CIfcRegistryException exception.
inline void IfcThrowRegistryException(LPCTSTR lpszMsg)
{
    throw IFC_EXCEPT_OBJ CIfcRegistryException(lpszMsg);
}

/// Throws a CIfcIniFileException exception.
inline void IfcThrowIniFileException(LPCTSTR lpszMsg)
{
    throw IFC_EXCEPT_OBJ CIfcIniFileException(lpszMsg);
}

/// Throws a CIfcThreadException exception.
inline void IfcThrowThreadException(LPCTSTR lpszMsg)
{
    throw IFC_EXCEPT_OBJ CIfcThreadException(lpszMsg);
}

/// Throws a CIfcFileException exception.
inline void IfcThrowFileException(LPCTSTR lpszFileName, int nCause, int nOsError)
{
    throw IFC_EXCEPT_OBJ CIfcFileException(lpszFileName, nCause, nOsError);
}

/// Throws a CIfcFileException exception.
inline void IfcThrowFileException(LPCTSTR lpszFileName, int nOsError)
{
    if (nOsError != 0)
    {
        int nCause = CIfcFileException::OsErrorToCause(nOsError);
        throw IFC_EXCEPT_OBJ CIfcFileException(lpszFileName, nCause, nOsError);
    }
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)

/// @}
