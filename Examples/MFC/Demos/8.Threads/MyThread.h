#pragma once

#include "ifc.h"

///////////////////////////////////////////////////////////////////////////////

#define WM_THREAD_WORKING       (WM_USER + 100)
#define WM_THREAD_BEFORE_TERM   (WM_USER + 101)
#define WM_THREAD_BEFORE_KILL   (WM_USER + 102)
#define WM_THREAD_DELETED       (WM_USER + 103)

///////////////////////////////////////////////////////////////////////////////

class CMyThread : public CThread
{
private:
    HWND m_hWnd;
protected:
    virtual void Execute();
    virtual void BeforeTerminate();
    virtual void BeforeKill();
public:
    CMyThread(HWND hWnd);
    virtual ~CMyThread();
};
