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

/// @file ifc_thread.cpp

#include "stdafx.h"
#include "ifc_thread.h"
#include "ifc_exceptions.h"
#include "ifc_errmsgs.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// CThread

#ifdef IFC_USE_MFC

//-----------------------------------------------------------------------------

CThread::CThread() :
    m_pWinThread(NULL),
    m_nPriority(THREAD_PRIORITY_NORMAL),
    m_nTermTime(0),
    m_bFreeOnTerminate(false),
    m_bTerminated(false),
    m_nReturnValue(0),
    m_bRunCalled(false)
{
}

//-----------------------------------------------------------------------------

CThread::~CThread()
{
    // If the thread is still running.
    if (m_pWinThread != NULL)
    {
        Terminate();
        WaitFor();
    }
}

//-----------------------------------------------------------------------------
// The thread execution procedure.
// Parameters:
//   pParam: Points to the CThread object.
//-----------------------------------------------------------------------------
UINT __cdecl ThreadExecProc(LPVOID pParam)
{
    CThread *pThread = (CThread*)pParam;
    int nReturnValue = 0;

    if (!pThread->m_bTerminated)
    {
        struct CAutoFinalizer
        {
            CThread *m_pThread;
            CAutoFinalizer(CThread *pThread) { m_pThread = pThread; }
            ~CAutoFinalizer()
            {
                m_pThread->m_pWinThread = NULL;
                if (m_pThread->GetFreeOnTerminate())
                    delete m_pThread;
            }
        } AutoFinalizer(pThread);

        try { pThread->Execute(); } catch (CException* e) { e->Delete(); } catch (...) {}
        try { pThread->AfterExecute(); } catch (CException* e) { e->Delete(); } catch (...) {}

        nReturnValue = pThread->m_nReturnValue;
    }
    
    return nReturnValue;
}

//-----------------------------------------------------------------------------

void CThread::Run()
{
    if (m_bRunCalled)
        IfcThrowThreadException(IEM_THREAD_RUN_ONCE);
    m_bRunCalled = true;

    m_pWinThread = ::AfxBeginThread(ThreadExecProc, (LPVOID)this, 
        m_nPriority, 0, CREATE_SUSPENDED, NULL);

    // The default value of m_bAutoDelete is TRUE, but it still be set to true here explicitly.
    m_pWinThread->m_bAutoDelete = TRUE;
    m_pWinThread->ResumeThread();
}

//-----------------------------------------------------------------------------

void CThread::Terminate()
{
    if (!m_bTerminated)
    {
        BeforeTerminate();
        m_nTermTime = time(NULL);
        m_bTerminated = true;
    }
}

//-----------------------------------------------------------------------------

void CThread::Kill()
{
    if (m_pWinThread != NULL)
    {
        BeforeKill();

        ::TerminateThread(GetHandle(), m_nReturnValue);

        delete m_pWinThread;
        m_pWinThread = NULL;

        if (m_bFreeOnTerminate)
            delete this;
    }
}

//-----------------------------------------------------------------------------

int CThread::WaitFor()
{
    int nReturnValue = 0;

    if (m_pWinThread != NULL)
    {
        HANDLE hThread = GetHandle();
        bool bFreeOnTerminate = m_bFreeOnTerminate;

        ::WaitForSingleObject(hThread, INFINITE);

        if (!bFreeOnTerminate)
            nReturnValue = m_nReturnValue;
    }

    return nReturnValue;
}

//-----------------------------------------------------------------------------

void CThread::SleepThread(double fSeconds)
{
    const double SLEEP_INTERVAL = 0.5;      // in seconds
    double fOnceSecs;

    while (!GetTerminated() && fSeconds > 0)
    {
        fOnceSecs = (fSeconds >= SLEEP_INTERVAL ? SLEEP_INTERVAL : fSeconds);
        fSeconds -= fOnceSecs;

        ::Sleep((DWORD)(fOnceSecs * 1000));
    }
}

//-----------------------------------------------------------------------------

HANDLE CThread::GetHandle() const
{
    if (m_pWinThread != NULL)
        return m_pWinThread->m_hThread;
    else
        return INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------

int CThread::GetThreadId() const
{
    if (m_pWinThread != NULL)
        return m_pWinThread->m_nThreadID;
    else
        return 0;
}

//-----------------------------------------------------------------------------

int CThread::GetTermElapsedSecs() const
{
    int nResult = 0;

    // If the thread has been asked to terminate, but it is still running.
    if (m_bTerminated && m_pWinThread != NULL)
    {
        nResult = (int)(time(NULL) - m_nTermTime);
    }

    return nResult;
}

//-----------------------------------------------------------------------------

void CThread::SetPriority(int nValue)
{
    switch (nValue)
    {
    case THREAD_PRIORITY_IDLE:
    case THREAD_PRIORITY_LOWEST:
    case THREAD_PRIORITY_BELOW_NORMAL:
    case THREAD_PRIORITY_NORMAL:
    case THREAD_PRIORITY_ABOVE_NORMAL:
    case THREAD_PRIORITY_HIGHEST:
    case THREAD_PRIORITY_TIME_CRITICAL:
        m_nPriority = nValue;
        break;
    default:
        m_nPriority = THREAD_PRIORITY_NORMAL;
        break;
    }

    if (m_pWinThread != NULL)
        m_pWinThread->SetThreadPriority(m_nPriority);
}

//-----------------------------------------------------------------------------

void CThread::SetTerminated(bool bValue)
{
    if (bValue != m_bTerminated)
    {
        if (bValue)
            Terminate();
        else
        {
            m_bTerminated = false;
            m_nTermTime = 0;
        }
    }
}

#else

///////////////////////////////////////////////////////////////////////////////
// CThread

//-----------------------------------------------------------------------------

CThread::CThread() :
    m_hHandle(0),
    m_nThreadId(0),
    m_nPriority(THREAD_PRIORITY_NORMAL),
    m_nTermTime(0),
    m_bFreeOnTerminate(false),
    m_bTerminated(false),
    m_nReturnValue(0),
    m_bRunCalled(false),
    m_bFinished(false)
{
}

//-----------------------------------------------------------------------------

CThread::~CThread()
{
    // If the thread is still running.
    if (m_nThreadId != 0 && !m_bFinished)
    {
        Terminate();
        WaitFor();
    }

    if (m_nThreadId != 0)
        CloseHandle(m_hHandle);
}

//-----------------------------------------------------------------------------
// The thread execution procedure.
// Parameters:
//   pParam: Points to the CThread object.
//-----------------------------------------------------------------------------
UINT __stdcall ThreadExecProc(LPVOID pParam)
{
    CThread *pThread = (CThread*)pParam;
    int nReturnValue = 0;

    if (!pThread->m_bTerminated)
    {
        struct CAutoFinalizer
        {
            CThread *m_pThread;
            CAutoFinalizer(CThread *pThread) { m_pThread = pThread; }
            ~CAutoFinalizer()
            {
                m_pThread->m_bFinished = true;
                m_pThread->m_nThreadId = 0;
                if (m_pThread->GetFreeOnTerminate())
                    delete m_pThread;
            }
        } AutoFinalizer(pThread);

        try { pThread->Execute(); } catch (...) {}
        try { pThread->AfterExecute(); } catch (...) {}

        nReturnValue = pThread->m_nReturnValue;
    }
    
    _endthreadex(nReturnValue);
    return nReturnValue;
}

//-----------------------------------------------------------------------------

void CThread::Run()
{
    if (m_bRunCalled)
        IfcThrowThreadException(IEM_THREAD_RUN_ONCE);
    m_bRunCalled = true;

    m_hHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadExecProc, (LPVOID)this, 
        CREATE_SUSPENDED, (UINT*)&m_nThreadId);

    if (m_hHandle == 0)
        IfcThrowThreadException(IEM_THREAD_CREATE_ERROR);

    SetPriority(m_nPriority);
    ::ResumeThread(m_hHandle);
}

//-----------------------------------------------------------------------------

void CThread::Terminate()
{
    if (!m_bTerminated)
    {
        BeforeTerminate();
        m_nTermTime = time(NULL);
        m_bTerminated = true;
    }
}

//-----------------------------------------------------------------------------

void CThread::Kill()
{
    if (m_nThreadId != 0)
    {
        BeforeKill();

        m_nThreadId = 0;
        ::TerminateThread(m_hHandle, m_nReturnValue);
        ::CloseHandle(m_hHandle);

        if (m_bFreeOnTerminate)
            delete this;
    }
}

//-----------------------------------------------------------------------------

int CThread::WaitFor()
{
    int nReturnValue = 0;

    if (m_nThreadId != 0)
    {
        HANDLE hHandle = m_hHandle;
        bool bFreeOnTerminate = m_bFreeOnTerminate;

        ::WaitForSingleObject(hHandle, INFINITE);

        if (!bFreeOnTerminate)
            nReturnValue = m_nReturnValue;
    }

    return nReturnValue;
}

//-----------------------------------------------------------------------------

void CThread::SleepThread(double fSeconds)
{
    const double SLEEP_INTERVAL = 0.5;      // in seconds
    double fOnceSecs;

    while (!GetTerminated() && fSeconds > 0)
    {
        fOnceSecs = (fSeconds >= SLEEP_INTERVAL ? SLEEP_INTERVAL : fSeconds);
        fSeconds -= fOnceSecs;

        ::Sleep((DWORD)(fOnceSecs * 1000));
    }
}

//-----------------------------------------------------------------------------

int CThread::GetTermElapsedSecs() const
{
    int nResult = 0;

    // If the thread has been asked to terminate, but it is still running.
    if (m_bTerminated && m_nThreadId != 0)
    {
        nResult = (int)(time(NULL) - m_nTermTime);
    }

    return nResult;
}

//-----------------------------------------------------------------------------

void CThread::SetPriority(int nValue)
{
    switch (nValue)
    {
    case THREAD_PRIORITY_IDLE:
    case THREAD_PRIORITY_LOWEST:
    case THREAD_PRIORITY_BELOW_NORMAL:
    case THREAD_PRIORITY_NORMAL:
    case THREAD_PRIORITY_ABOVE_NORMAL:
    case THREAD_PRIORITY_HIGHEST:
    case THREAD_PRIORITY_TIME_CRITICAL:
        m_nPriority = nValue;
        break;
    default:
        m_nPriority = THREAD_PRIORITY_NORMAL;
        break;
    }

    if (m_nThreadId != 0)
        SetThreadPriority(m_hHandle, m_nPriority);
}

//-----------------------------------------------------------------------------

void CThread::SetTerminated(bool bValue)
{
    if (bValue != m_bTerminated)
    {
        if (bValue)
            Terminate();
        else
        {
            m_bTerminated = false;
            m_nTermTime = 0;
        }
    }
}

#endif

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
