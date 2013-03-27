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

/// @file ifc_thread.h
/// Defines the thread class.

/// @addtogroup Classes
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////
// Type Definitions

typedef void (*THREAD_EXEC_PROC)(PVOID pParam);

///////////////////////////////////////////////////////////////////////////////
/// CThread - Thread class.
///
/// CThread is an abstract class that enables creation of separate threads of execution in an application.
/// Each new instance of a CThread descendant is a new thread of execution. Multiple instances of a CThread
/// derived class make an application multi-threaded.
///
/// The descendant classes must implement the thread object's Execute() method by inserting the code that
/// should execute when the thread is executed.

#ifdef IFC_USE_MFC

// CThread for MFC.
class CThread
{
public:
	friend UINT __stdcall ThreadExecProc(PVOID pParam);

private:
	HANDLE m_hHandle;               // The handle of the thread.
	int m_nThreadId;                // The ID of the thread.
	int m_nPriority;                // The priority of the thread.
	time_t m_nTermTime;             // The time stamp on Terminated() called.
	bool m_bFreeOnTerminate;        // Determines whether the thread object is automatically destroyed when the thread terminates.
	bool m_bTerminated;             // Indicates whether the thread has been asked to terminate.
	int m_nReturnValue;             // The value returned to other waiting threads when the thread finishes executing.
	bool m_bRunCalled;              // Indicates whether the Run() is called.
	THREAD_EXEC_PROC m_pExecProc;   // Used by CThread::Create().
	void *m_pThreadParam;           // Used by CThread::Create().
	bool m_bFinished;               // Indicates whether the execution of ThreadExecProc() is finished.
protected:
	/// Provides an virtual method to contain the code which executes when the thread is run.
	/// Override Execute and insert the code that should be executed when the thread runs. Execute is
	/// responsible for checking the value of the GetTerminated() to determine if the thread needs to exit.
	/// By default, the function checks the @a m_pExecProc and calls it.
	virtual void Execute() { if (m_pExecProc != NULL) (*m_pExecProc)(m_pThreadParam); }

	/// AfterExecute() is always invoked automatically after Execute().
	virtual void AfterExecute() {}

	/// BeforeTerminate() is invoked automatically by Terminate() before setting terminated flag to true.
	/// Descendant classes can override this  method to perform some important operations before the
	/// thread is asked to terminate.
	virtual void BeforeTerminate() {}

	/// BeforeKill() is invoked automatically by Kill() before the thread is killed.
	/// Descendant classes can override this  method to perform some important cleanup operations before
	/// the thread is killed.
	virtual void BeforeKill() {}
public:
	/// Default constructor.
	CThread();

	/// Destructor.
	virtual ~CThread();

	/// Creates a thread and runs it immediately.
	///
	/// @param[in] pExecProc
	///   The procedure to be executed by the created thread.
	/// @param[in] pParam
	///   The parameter to be passed to @a pExecProc.
	static void Create(THREAD_EXEC_PROC pExecProc, void *pParam = NULL);

	/// Create thread and begin to run the thread.
	/// This method can be called only once for one CThread object.
	void Run();

	/// Signals the thread to terminate by setting the m_bTerminated flag to true.
	/// For Terminate to work, the thread's Execute() method and any methods that Execute calls should
	/// check GetTerminated() periodically and exit when it's true.
	void Terminate();

	/// Kill the thread immediately by calling Windows API TerminateThread.
	///
	/// @remarks
	///   - Kill() calls the BeforeKill() method before killing the thread.
	///   - If GetFreeOnTerminate() is true, Kill() will delete the CThread object.
	void Kill();

	/// Waits for the thread to terminate and then returns the value specified by SetReturnValue().
	///
	/// @return
	///   The return value of the thread.
	/// @remarks
	///   - WaitFor doesn't return until the thread terminates.
	///   - If GetFreeOnTerminate() is true, the return value is undefined.
	int WaitFor();

	/// Suspends the thread for at least the specified number of seconds.
	///
	/// @remarks
	///   - The method must be called from within the thread.
	///   - The sleeping thread can be interrupted by SetTerminated(true).
	void SleepThread(double fSeconds);

	/// Indicates whether the thread is executing.
	bool IsRunning() { return m_nThreadId != 0; }

	/// Returns the thread handle.
	HANDLE GetHandle() const { return m_hHandle; }
	/// Returns the thread ID.
	int GetThreadId() const { return m_nThreadId; }
	/// Returns the priority of the thread.
	int GetPriority() const { return m_nPriority; }
	/// Indicates whether the thread has been asked to terminate.
	int GetTerminated() const { return m_bTerminated; }
	/// Returns the value returned to other waiting threads when the thread finishes executing.
	int GetReturnValue() const { return m_nReturnValue; }
	/// Determines whether the thread object is automatically destroyed when the thread terminates.
	bool GetFreeOnTerminate() const { return m_bFreeOnTerminate; }
	/// Returns the elapsed seconds since Terminate() or SetTerminated(true) was called.
	int GetTermElapsedSecs() const;

	/// Changes the priority of the thread.
	void SetPriority(int nValue);
	/// Changes the terminated flag. SetTerminated(true) is identical to Terminate().
	void SetTerminated(bool bValue);
	/// Specifies the value returned to other waiting threads when the thread finishes executing.
	void SetReturnValue(int nValue) { m_nReturnValue = nValue; }
	/// Specifies whether the thread object is automatically destroyed when the thread terminates.
	void SetFreeOnTerminate(bool bValue) { m_bFreeOnTerminate = bValue; }
};

#else

///////////////////////////////////////////////////////////////////////////////

// CThread for non-MFC.
class CThread
{
	friend UINT __stdcall ThreadExecProc(LPVOID pParam);

private:
	HANDLE m_hHandle;               // The handle of the thread.
	int m_nThreadId;                // The ID of the thread.
	int m_nPriority;                // The priority of the thread.
	time_t m_nTermTime;             // The time stamp on Terminated() called.
	bool m_bFreeOnTerminate;        // Determines whether the thread object is automatically destroyed when the thread terminates.
	bool m_bTerminated;             // Indicates whether the thread has been asked to terminate.
	int m_nReturnValue;             // The value returned to other waiting threads when the thread finishes executing.
	bool m_bRunCalled;              // Indicates whether the Run() is called.
	THREAD_EXEC_PROC m_pExecProc;   // Used by CThread::Create().
	void *m_pThreadParam;           // Used by CThread::Create().
	bool m_bFinished;               // Indicates whether the execution of ThreadExecProc() is finished.
protected:
	/// Provides an abstract virtual method to contain the code which executes when the thread is run.
	/// Override Execute and insert the code that should be executed when the thread runs. Execute is 
	/// responsible for checking the value of the GetTerminated() to determine if the thread needs to exit.
	/// By default, the function checks the @a m_pExecProc and calls it.
	virtual void Execute() { if (m_pExecProc != NULL) (*m_pExecProc)(m_pThreadParam); }

	/// AfterExecute() is always invoked automatically after Execute().
	virtual void AfterExecute() {}

	/// BeforeTerminate() is invoked automatically by Terminate() before setting terminated flag to true.
	/// Descendant classes can override this  method to perform some important operations before the 
	/// thread is asked to terminate.
	virtual void BeforeTerminate() {}

	/// BeforeKill() is invoked automatically by Kill() before the thread is killed.
	/// Descendant classes can override this  method to perform some important cleanup operations before 
	/// the thread is killed.
	virtual void BeforeKill() {}
public:
	/// Default constructor.
	CThread();

	/// Destructor.
	virtual ~CThread();

	/// Creates a thread and runs it immediately.
	///
	/// @param[in] pExecProc
	///   The procedure to be executed by the created thread.
	/// @param[in] pParam
	///   The parameter to be passed to @a pExecProc.
	static void Create(THREAD_EXEC_PROC pExecProc, void *pParam = NULL);

	/// Create thread and begin to run the thread.
	/// This method can be called only once for one CThread object.
	void Run();

	/// Signals the thread to terminate by setting the m_bTerminated flag to true.
	/// For Terminate to work, the thread's Execute() method and any methods that Execute calls should 
	/// check GetTerminated() periodically and exit when it's true.
	void Terminate();

	/// Kill the thread immediately by calling Windows API TerminateThread.
	///
	/// @remarks
	///   - Kill() calls the BeforeKill() method before killing the thread.
	///   - If GetFreeOnTerminate() is true, Kill() will delete the CThread object.
	void Kill();

	/// Waits for the thread to terminate and then returns the value specified by SetReturnValue().
	///
	/// @return
	///   The return value of the thread.
	/// @remarks
	///   - WaitFor doesn't return until the thread terminates.
	///   - If GetFreeOnTerminate() is true, the return value is undefined.
	int WaitFor();

	/// Suspends the thread for at least the specified number of seconds.
	///
	/// @remarks
	///   - The method must be called from within the thread.
	///   - The sleeping thread can be interrupted by SetTerminated(true).
	void SleepThread(double fSeconds);

	/// Returns the thread handle.
	HANDLE GetHandle() const { return m_hHandle; }
	/// Returns the thread ID.
	int GetThreadId() const { return m_nThreadId; }
	/// Returns the priority of the thread.
	int GetPriority() const { return m_nPriority; }
	/// Indicates whether the thread has been asked to terminate.
	int GetTerminated() const { return m_bTerminated; }
	/// Returns the value returned to other waiting threads when the thread finishes executing.
	int GetReturnValue() const { return m_nReturnValue; }
	/// Determines whether the thread object is automatically destroyed when the thread terminates. 
	bool GetFreeOnTerminate() const { return m_bFreeOnTerminate; }
	/// Returns the elapsed seconds since Terminate() or SetTerminated(true) was called.
	int GetTermElapsedSecs() const;

	/// Changes the priority of the thread.
	void SetPriority(int nValue);
	/// Changes the terminated flag. SetTerminated(true) is identical to Terminate().
	void SetTerminated(bool bValue);
	/// Specifies the value returned to other waiting threads when the thread finishes executing.
	void SetReturnValue(int nValue) { m_nReturnValue = nValue; }
	/// Specifies whether the thread object is automatically destroyed when the thread terminates. 
	void SetFreeOnTerminate(bool bValue) { m_bFreeOnTerminate = bValue; }
};

#endif

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc

/// @}
