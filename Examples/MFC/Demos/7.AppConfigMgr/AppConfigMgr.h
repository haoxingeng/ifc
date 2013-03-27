#pragma once

#include "ifc.h"

///////////////////////////////////////////////////////////////////////////////

class CAppConfigMgr : public CBaseConfigMgr
{
private:
    CRect m_WindowRect;
private:
    CString GetIniFileName();
    void LoadWindowRect();
    void SaveWindowRect();
public:
    CAppConfigMgr();
    virtual ~CAppConfigMgr();

    void Load();
    void Save();

    CRect& WindowRect() { return m_WindowRect; }
};

///////////////////////////////////////////////////////////////////////////////

extern CAppConfigMgr ConfigMgr;

///////////////////////////////////////////////////////////////////////////////
