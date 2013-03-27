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

/// @file ifc_sync_objs.cpp

#include "stdafx.h"
#include "ifc_sync_objs.h"
#include "ifc_errmsgs.h"
#include "ifc_sysutils.h"
#include "ifc_classes.h"

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////
/// CSyncObject

CSyncHandleObject::CSyncHandleObject() :
	m_hObject(NULL)
{
	// nothing
}

//-----------------------------------------------------------------------------

CSyncHandleObject::~CSyncHandleObject()
{
	if (m_hObject != NULL)
	{
		::CloseHandle(m_hObject);
		m_hObject = NULL;
	}
}

//-----------------------------------------------------------------------------

bool CSyncHandleObject::WaitFor(DWORD nTimeOut)
{
	DWORD r = WaitForSingleObjectEx(m_hObject, nTimeOut, FALSE);
	return (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED);
}

///////////////////////////////////////////////////////////////////////////////
// CCriticalSection

CCriticalSection::CCriticalSection()
{
	Init();
}

//-----------------------------------------------------------------------------

CCriticalSection::~CCriticalSection()
{
	DeleteCriticalSection(&m_Lock);
}

//-----------------------------------------------------------------------------

void CCriticalSection::Init()
{
	__try
	{
		InitializeCriticalSection(&m_Lock);
	}
	__except(STATUS_NO_MEMORY == GetExceptionCode())
	{
		IfcThrowMemoryException();
	}
}

//-----------------------------------------------------------------------------

bool CCriticalSection::Lock()
{
	__try
	{
		EnterCriticalSection(&m_Lock);
	}
	__except(STATUS_NO_MEMORY == GetExceptionCode())
	{
		IfcThrowMemoryException();
	}

	return true;
}

//-----------------------------------------------------------------------------

bool CCriticalSection::Unlock()
{
	LeaveCriticalSection(&m_Lock);
	return true;
}

//-----------------------------------------------------------------------------

bool CCriticalSection::TryLock()
{
	return TryEnterCriticalSection(&m_Lock) != 0;
}

///////////////////////////////////////////////////////////////////////////////
// CMutexObject

CMutexObject::CMutexObject(LPCTSTR lpszName, bool bInitialOwner, LPSECURITY_ATTRIBUTES pAttribute)
{
	if (lpszName && !lpszName[0]) lpszName = NULL;
	m_hObject = ::CreateMutex(pAttribute, bInitialOwner, lpszName);
	if (m_hObject == NULL)
		IfcThrowOsException();
}

//-----------------------------------------------------------------------------

CMutexObject::CMutexObject(LPCTSTR lpszName, DWORD nDesiredAccess, bool bInheritHandle)
{
	m_hObject = ::OpenMutex(nDesiredAccess, bInheritHandle, lpszName);
	if (m_hObject == NULL)
		IfcThrowOsException();
}

//-----------------------------------------------------------------------------

bool CMutexObject::Unlock()
{
	return (::ReleaseMutex(m_hObject) != FALSE);
}

///////////////////////////////////////////////////////////////////////////////
/// CEventObject

CEventObject::CEventObject(LPCTSTR lpszName, bool bManualReset, bool bInitialState,
	LPSECURITY_ATTRIBUTES pAttribute)
{
	if (lpszName && !lpszName[0]) lpszName = NULL;
	m_hObject = ::CreateEvent(pAttribute, bManualReset, bInitialState, lpszName);
	if (m_hObject == NULL)
		IfcThrowOsException();
}

//-----------------------------------------------------------------------------

bool CEventObject::SetEvent()
{
	return (::SetEvent(m_hObject) != FALSE);
}

//-----------------------------------------------------------------------------

bool CEventObject::ResetEvent()
{
	return (::ResetEvent(m_hObject) != FALSE);
}

//-----------------------------------------------------------------------------

bool CEventObject::PulseEvent()
{
	return (::PulseEvent(m_hObject) != FALSE);
}

///////////////////////////////////////////////////////////////////////////////
/// CSemaphoreObject

CSemaphoreObject::CSemaphoreObject(LPCTSTR lpszName, int nInitialCount, int nMaxCount,
	LPSECURITY_ATTRIBUTES pAttribute)
{
	IFC_ASSERT(nMaxCount > 0);
	IFC_ASSERT(nInitialCount <= nMaxCount);

	if (lpszName && !lpszName[0]) lpszName = NULL;
	m_hObject = ::CreateSemaphore(pAttribute, nInitialCount, nMaxCount, lpszName);
	if (m_hObject == NULL)
		IfcThrowOsException();
}

//-----------------------------------------------------------------------------

CSemaphoreObject::CSemaphoreObject(LPCTSTR lpszName, DWORD nDesiredAccess, bool bInheritHandle)
{
	m_hObject = ::OpenSemaphore(nDesiredAccess, bInheritHandle, lpszName);
	if (m_hObject == NULL)
		IfcThrowOsException();
}

//-----------------------------------------------------------------------------

bool CSemaphoreObject::Unlock(int nCount)
{
	return (::ReleaseSemaphore(m_hObject, nCount, NULL) != FALSE);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc
