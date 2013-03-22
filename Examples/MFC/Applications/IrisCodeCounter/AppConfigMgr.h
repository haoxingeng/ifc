#pragma once

#include "ifc.h"

///////////////////////////////////////////////////////////////////////////////

class CAppConfigMgr : public CBaseConfigMgr
{
private:
    CStrList m_RecentSrcPaths;
private:
    CString GetIniFileName();
    void LoadRecentSrcPaths();
    void SaveRecentSrcPaths();
public:
    CAppConfigMgr();
    virtual ~CAppConfigMgr();

    void Load();
    void Save();

    void GetRecentSrcPaths(CStrList& List);
    void SetRecentSrcPaths(const CStrList& List);
};

///////////////////////////////////////////////////////////////////////////////

extern CAppConfigMgr ConfigMgr;

///////////////////////////////////////////////////////////////////////////////
