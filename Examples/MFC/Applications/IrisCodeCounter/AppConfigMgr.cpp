#include "StdAfx.h"
#include "AppConfigMgr.h"
#include "Global.h"

///////////////////////////////////////////////////////////////////////////////

#define SECTION_GENERAL  TEXT("Config")

///////////////////////////////////////////////////////////////////////////////

CAppConfigMgr::CAppConfigMgr()
{
}

CAppConfigMgr::~CAppConfigMgr()
{
}

CString CAppConfigMgr::GetIniFileName()
{
    return GetAppPath() + CONFIG_FILE_NAME;
}

void CAppConfigMgr::LoadRecentSrcPaths()
{
    CString str = GetString(SECTION_GENERAL, TEXT("RecentSrcPaths"));
    m_RecentSrcPaths.SetCommaText(str);
}

void CAppConfigMgr::SaveRecentSrcPaths()
{
    CString str = m_RecentSrcPaths.GetCommaText();
    SetString(SECTION_GENERAL, TEXT("RecentSrcPaths"), str);
}

void CAppConfigMgr::Load()
{
    LoadFromIniFile(GetIniFileName());
    LoadRecentSrcPaths();
}

void CAppConfigMgr::Save()
{
    SaveRecentSrcPaths();
    SaveToIniFile(GetIniFileName());
}

void CAppConfigMgr::GetRecentSrcPaths(CStrList& List)
{
    List = m_RecentSrcPaths;
}

void CAppConfigMgr::SetRecentSrcPaths(const CStrList& List)
{
    CStrList TempList;

    // È¥³ýÖØ¸´
    for (int i = 0; i < List.GetCount() && i < MAX_RECENT_SRC_DIRS; i++)
    {
        CString str = List[i];
        if (!TempList.Exists(str))
            TempList.Add(str);
    }

    m_RecentSrcPaths = TempList;
}

///////////////////////////////////////////////////////////////////////////////

CAppConfigMgr ConfigMgr;

///////////////////////////////////////////////////////////////////////////////
