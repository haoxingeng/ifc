#include "StdAfx.h"
#include "AppConfigMgr.h"

///////////////////////////////////////////////////////////////////////////////

#define SECTION_GENERAL  TEXT("Config")

///////////////////////////////////////////////////////////////////////////////

CAppConfigMgr::CAppConfigMgr()
{
    memset(&m_WindowRect, 0, sizeof(m_WindowRect));
}

CAppConfigMgr::~CAppConfigMgr()
{
}

CString CAppConfigMgr::GetIniFileName()
{
    return GetAppPath() + TEXT("Config.ini");
}

void CAppConfigMgr::LoadWindowRect()
{
    m_WindowRect.left = GetInteger(SECTION_GENERAL, TEXT("WindowLeft"));
    m_WindowRect.top = GetInteger(SECTION_GENERAL, TEXT("WindowTop"));
    m_WindowRect.right = GetInteger(SECTION_GENERAL, TEXT("WindowRight"));
    m_WindowRect.bottom = GetInteger(SECTION_GENERAL, TEXT("WindowBottom"));
}

void CAppConfigMgr::SaveWindowRect()
{
    SetInteger(SECTION_GENERAL, TEXT("WindowLeft"), m_WindowRect.left);
    SetInteger(SECTION_GENERAL, TEXT("WindowTop"), m_WindowRect.top);
    SetInteger(SECTION_GENERAL, TEXT("WindowRight"), m_WindowRect.right);
    SetInteger(SECTION_GENERAL, TEXT("WindowBottom"), m_WindowRect.bottom);
}

void CAppConfigMgr::Load()
{
    LoadFromIniFile(GetIniFileName());
    LoadWindowRect();
}

void CAppConfigMgr::Save()
{
    SaveWindowRect();
    SaveToIniFile(GetIniFileName());
}

///////////////////////////////////////////////////////////////////////////////

CAppConfigMgr ConfigMgr;

///////////////////////////////////////////////////////////////////////////////
