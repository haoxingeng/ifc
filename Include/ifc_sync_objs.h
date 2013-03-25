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

/// @file ifc_sync_objs.h
/// Defines the synchronized object classes.

/// @addtogroup Classes
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_exceptions.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Classes

class CAutoLocker;
class CSyncObject;
class CCriticalSection;
class CMutexObject;
class CEventObject;
class CSemaphoreObject;

///////////////////////////////////////////////////////////////////////////////
/// CAutoLocker - The auto locker class.
///
/// The classic form of the descendant class is:
/** @code
	CCriticalSection m_Lock;

	void foo()
	{
		CAutoLocker Locker(m_Lock);
		//...
	}
	@endcode
*/

class CAutoLocker : public CAutoInvoker
{
public:
	explicit CAutoLocker(CAutoInvokable& Object) : CAutoInvoker(Object) {}
	explicit CAutoLocker(CAutoInvokable *pObject) : CAutoInvoker(pObject) {}
};

///////////////////////////////////////////////////////////////////////////////
/// CSyncObject - The base class of synchronized object.

class CSyncObject : public CAutoInvokable
{
protected:
	virtual void InvokeInitialize() { Lock(); }
	virtual void InvokeFinalize() { Unlock(); }
public:
	/// Locks the object.
	virtual bool Lock() { return true; }
	/// Unlocks the object.
	virtual bool Unlock() { return true; }
};

///////////////////////////////////////////////////////////////////////////////
/// CSyncHandleObject - The base class of synchronized object with a handle.

class CSyncHandleObject : public CSyncObject
{
protected:
	HANDLE m_hObject;
public:
	CSyncHandleObject();
	virtual ~CSyncHandleObject();

	/// Locks the object.
	virtual bool Lock() { return WaitFor(INFINITE); }

	/// Wait for the object.
	bool WaitFor(DWORD nTimeOut = INFINITE);

	/// Returns the object handle.
	operator HANDLE() const { return m_hObject; }
};

///////////////////////////////////////////////////////////////////////////////
/// CCriticalSection - The critical section class.

class CCriticalSection : public CSyncObject
{
private:
	CRITICAL_SECTION m_Lock;
private:
	void Init();
public:
	/// Constructor.
	CCriticalSection();
	/// Destructor.
	virtual ~CCriticalSection();

	/// Locks the critical section.
	virtual bool Lock();
	/// Unlocks the critical section.
	virtual bool Unlock();
	/// Try to perform the lock operation, returns false if already locked.
	bool TryLock();
};

///////////////////////////////////////////////////////////////////////////////
/// CMutexObject - The mutex class.

class CMutexObject : public CSyncHandleObject
{
public:
	/// Constructor.
	CMutexObject(LPCTSTR lpszName = NULL, bool bInitialOwner = false, LPSECURITY_ATTRIBUTES pAttribute = NULL);
	/// Constructor.
	CMutexObject(LPCTSTR lpszName, DWORD nDesiredAccess, bool bInheritHandle);

	/// Unlocks the object.
	virtual bool Unlock();
};

///////////////////////////////////////////////////////////////////////////////
/// CEventObject - The event class.

class CEventObject : public CSyncHandleObject
{
public:
	/// Constructor.
	CEventObject(LPCTSTR lpszName = NULL, bool bManualReset = false, bool bInitialState = false,
		LPSECURITY_ATTRIBUTES pAttribute = NULL);

	/// Sets the specified event object to the signaled state.
	bool SetEvent();
	/// Sets the specified event object to the nonsignaled state.
	bool ResetEvent();
	/// Sets the specified event object to the signaled state and then resets it to the nonsignaled state
	/// after releasing the appropriate number of waiting threads.
	bool PulseEvent();
};

///////////////////////////////////////////////////////////////////////////////
/// CSemaphoreObject - the semaphore class.

class CSemaphoreObject : public CSyncHandleObject
{
public:
	/// Constructor.
	CSemaphoreObject(LPCTSTR lpszName = NULL, int nInitialCount = 1, int nMaxCount = 1,
		LPSECURITY_ATTRIBUTES pAttribute = NULL);
	/// Constructor.
	CSemaphoreObject(LPCTSTR lpszName, DWORD nDesiredAccess, bool bInheritHandle);

	/// Unlocks the object.
	virtual bool Unlock() { return Unlock(1); }
	/// Unlocks the object.
	virtual bool Unlock(int nCount);
};

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)

/// @}
