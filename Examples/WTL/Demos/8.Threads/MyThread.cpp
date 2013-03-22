#include "StdAfx.h"
#include "MyThread.h"

///////////////////////////////////////////////////////////////////////////////

CMyThread::CMyThread(HWND hWnd) :
    m_hWnd(hWnd)
{
    SetFreeOnTerminate(true);
}

CMyThread::~CMyThread()
{
    PostMessage(m_hWnd, WM_THREAD_DELETED, 0, 0);
}

void CMyThread::Execute()
{
    int nCount = 0;

    while (!GetTerminated())
    {
        PostMessage(m_hWnd, WM_THREAD_WORKING, nCount++, 0);
        SleepThread(1);
    }
}

void CMyThread::BeforeTerminate()
{
    PostMessage(m_hWnd, WM_THREAD_BEFORE_TERM, 0, 0);
}

void CMyThread::BeforeKill()
{
    PostMessage(m_hWnd, WM_THREAD_BEFORE_KILL, 0, 0);
}
