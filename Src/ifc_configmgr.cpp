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

/// @file ifc_configmgr.cpp

#include "stdafx.h"
#include "ifc_configmgr.h"
#include "ifc_sysutils.h"

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////
// CBaseConfigMgr

//-----------------------------------------------------------------------------

CBaseConfigMgr::CBaseConfigMgr()
{
	m_Sections.SetCaseSensitive(false);
}

//-----------------------------------------------------------------------------

CBaseConfigMgr::~CBaseConfigMgr()
{
	Clear();
}

//-----------------------------------------------------------------------------

CNameValueList* CBaseConfigMgr::FindNameValueList(LPCTSTR lpszSection)
{
	CNameValueList *pResult = NULL;

	int i = m_Sections.IndexOf(lpszSection);
	if (i >= 0)
		pResult = (CNameValueList*)m_Sections.GetData(i);

	return pResult;
}

//-----------------------------------------------------------------------------

CNameValueList* CBaseConfigMgr::AddSection(LPCTSTR lpszSection)
{
	CNameValueList *pResult;

	pResult = FindNameValueList(lpszSection);
	if (pResult == NULL)
	{
		pResult = new CNameValueList();
		pResult->SetCaseSensitive(false);

		m_Sections.Add(lpszSection, pResult);
	}

	return pResult;
}

//-----------------------------------------------------------------------------

bool CBaseConfigMgr::GetValueStr(LPCTSTR lpszSection, LPCTSTR lpszName, CString& strValue)
{
	CNameValueList *pNVList = FindNameValueList(lpszSection);
	return (pNVList != NULL && pNVList->GetValue(lpszName, strValue));
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::Load(CConfigIO& IO)
{
	CConfigIO::CAutoUpdater AutoUpdater(IO);

	CStrList SectionList, KeyList;
	CString strSection, strKey, strValue;

	Clear();
	IO.GetSectionList(SectionList);
	for (int nSecIndex = 0; nSecIndex < SectionList.GetCount(); nSecIndex++)
	{
		strSection = SectionList[nSecIndex];
		KeyList.Clear();
		IO.GetKeyList(strSection, KeyList);
		for (int nKeyIndex = 0; nKeyIndex < KeyList.GetCount(); nKeyIndex++)
		{
			strKey = KeyList[nKeyIndex];
			strValue = IO.Read(strSection, strKey, TEXT(""));
			SetString(strSection, strKey, strValue);
		}
	}
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::Save(CConfigIO& IO)
{
	CConfigIO::CAutoUpdater AutoUpdater(IO);

	CString strSection, strKey, strValue;
	CNameValueList *pNVList;

	for (int nSecIndex = 0; nSecIndex < m_Sections.GetCount(); nSecIndex++)
	{
		strSection = m_Sections[nSecIndex];
		pNVList = (CNameValueList*)m_Sections.GetData(nSecIndex);
		IO.DeleteSection(strSection);
		for (int nKeyIndex = 0; nKeyIndex < pNVList->GetCount(); nKeyIndex++)
		{
			pNVList->GetItems(nKeyIndex, strKey, strValue);
			IO.Write(strSection, strKey, strValue);
		}
	}
}

//-----------------------------------------------------------------------------

CString CBaseConfigMgr::GetString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault)
{
	CString strResult;

	if (!GetValueStr(lpszSection, lpszName, strResult))
		strResult = lpszDefault;

	return strResult;
}

//-----------------------------------------------------------------------------

int CBaseConfigMgr::GetInteger(LPCTSTR lpszSection, LPCTSTR lpszName, int nDefault)
{
	int nResult;
	CString strValue;

	if (GetValueStr(lpszSection, lpszName, strValue))
		nResult = StrToInt(strValue, nDefault);
	else
		nResult = nDefault;

	return nResult;
}

//-----------------------------------------------------------------------------

bool CBaseConfigMgr::GetBool(LPCTSTR lpszSection, LPCTSTR lpszName, bool bDefault)
{
	bool bResult;
	CString strValue;

	if (GetValueStr(lpszSection, lpszName, strValue))
		bResult = StrToBool(strValue, bDefault);
	else
		bResult = bDefault;

	return bResult;
}

//-----------------------------------------------------------------------------

double CBaseConfigMgr::GetFloat(LPCTSTR lpszSection, LPCTSTR lpszName, double fDefault)
{
	double fResult;
	CString strValue;

	if (GetValueStr(lpszSection, lpszName, strValue))
		fResult = StrToFloat(strValue, fDefault);
	else
		fResult = fDefault;

	return fResult;
}

//-----------------------------------------------------------------------------

int CBaseConfigMgr::GetBinaryData(LPCTSTR lpszSection, LPCTSTR lpszName, CBuffer& Value)
{
	CString strValue = GetString(lpszSection, lpszName, TEXT(""));

	Value.Clear();
	if (!strValue.IsEmpty())
		DecodeBase16(strValue, Value);

	return Value.GetSize();
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::SetString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CNameValueList *pNVList = AddSection(lpszSection);
	pNVList->Add(lpszName, lpszValue);
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::SetInteger(LPCTSTR lpszSection, LPCTSTR lpszName, int nValue)
{
	CNameValueList *pNVList = AddSection(lpszSection);
	pNVList->Add(lpszName, IntToStr(nValue));
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::SetBool(LPCTSTR lpszSection, LPCTSTR lpszName, bool bValue)
{
	CNameValueList *pNVList = AddSection(lpszSection);
	pNVList->Add(lpszName, BoolToStr(bValue));
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::SetFloat(LPCTSTR lpszSection, LPCTSTR lpszName, double fValue)
{
	CNameValueList *pNVList = AddSection(lpszSection);
	pNVList->Add(lpszName, FloatToStr(fValue));
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::SetBinaryData(LPCTSTR lpszSection, LPCTSTR lpszName, PVOID pDataBuf, int nDataSize)
{
	CString strText;
	if (nDataSize > 0 && pDataBuf != NULL)
		strText = EncodeBase16((char*)pDataBuf, nDataSize);
	SetString(lpszSection, lpszName, strText);
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::DeleteSection(LPCTSTR lpszSection)
{
	// The items in m_Sections should not be removed, so that the Save() can erase
	// the sections from the file correctly.
	CNameValueList *pNVList = FindNameValueList(lpszSection);
	if (pNVList != NULL)
		pNVList->Clear();
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::GetSection(LPCTSTR lpszSection, CNameValueList& List)
{
	List.Clear();
	CNameValueList *pNVList = FindNameValueList(lpszSection);
	if (pNVList != NULL)
		List = *pNVList;
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::LoadFromIniFile(LPCTSTR lpszFileName)
{
	CIniConfigIO IO(lpszFileName);
	Load(IO);
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::SaveToIniFile(LPCTSTR lpszFileName)
{
	CIniConfigIO IO(lpszFileName);
	Save(IO);
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::LoadFromRegistry(HKEY hRootKey, LPCTSTR lpszPath)
{
	CRegConfigIO IO(hRootKey, lpszPath);
	Load(IO);
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::SaveToRegistry(HKEY hRootKey, LPCTSTR lpszPath)
{
	CRegConfigIO IO(hRootKey, lpszPath);
	Save(IO);
}

//-----------------------------------------------------------------------------

void CBaseConfigMgr::Clear()
{
	for (int i = 0; i < m_Sections.GetCount(); i++)
		delete (CNameValueList*)m_Sections.GetData(i);
	m_Sections.Clear();
}

///////////////////////////////////////////////////////////////////////////////
// CIniConfigIO

//-----------------------------------------------------------------------------

CIniConfigIO::CIniConfigIO(LPCTSTR lpszFileName) :
	m_strFileName(lpszFileName),
	m_pIniFile(NULL)
{
	// nothing
}

//-----------------------------------------------------------------------------

void CIniConfigIO::BeginUpdate()
{
	m_pIniFile = new CMemIniFile(m_strFileName);
}

//-----------------------------------------------------------------------------

void CIniConfigIO::EndUpdate()
{
	m_pIniFile->UpdateFile();

	delete m_pIniFile;
	m_pIniFile = NULL;
}

//-----------------------------------------------------------------------------

void CIniConfigIO::GetSectionList(CStrList& List)
{
	m_pIniFile->ReadSectionNames(List);
}

//-----------------------------------------------------------------------------

void CIniConfigIO::GetKeyList(LPCTSTR lpszSection, CStrList& List)
{
	m_pIniFile->ReadKeyNames(lpszSection, List);
}

//-----------------------------------------------------------------------------

CString CIniConfigIO::Read(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault)
{
	return m_pIniFile->ReadString(lpszSection, lpszName, lpszDefault);
}

//-----------------------------------------------------------------------------

void CIniConfigIO::Write(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	m_pIniFile->WriteString(lpszSection, lpszName, lpszValue);
}

//-----------------------------------------------------------------------------

void CIniConfigIO::DeleteSection(LPCTSTR lpszSection)
{
	m_pIniFile->EraseSection(lpszSection);
}

///////////////////////////////////////////////////////////////////////////////
// CRegConfigIO

//-----------------------------------------------------------------------------

CRegConfigIO::CRegConfigIO(HKEY hRootKey, LPCTSTR lpszPath) :
	m_hRootKey(hRootKey),
	m_strPath(lpszPath),
	m_pRegistry(NULL)
{
	m_strPath = PathWithSlash(m_strPath);
}

//-----------------------------------------------------------------------------

void CRegConfigIO::BeginUpdate()
{
	m_pRegistry = new CRegistry();
	m_pRegistry->SetRootKey(m_hRootKey);
}

//-----------------------------------------------------------------------------

void CRegConfigIO::EndUpdate()
{
	delete m_pRegistry;
	m_pRegistry = NULL;
}

//-----------------------------------------------------------------------------

void CRegConfigIO::GetSectionList(CStrList& List)
{
	List.Clear();
	if (m_pRegistry->OpenKey(m_strPath, false))
		m_pRegistry->GetKeyNames(List);
}

//-----------------------------------------------------------------------------

void CRegConfigIO::GetKeyList(LPCTSTR lpszSection, CStrList& List)
{
	List.Clear();
	if (m_pRegistry->OpenKey(m_strPath + lpszSection, false))
		m_pRegistry->GetValueNames(List);
}

//-----------------------------------------------------------------------------

CString CRegConfigIO::Read(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault)
{
	if (m_pRegistry->OpenKey(m_strPath + lpszSection, false))
		return m_pRegistry->ReadString(lpszName);
	else
		return lpszDefault;
}

//-----------------------------------------------------------------------------

void CRegConfigIO::Write(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (m_pRegistry->OpenKey(m_strPath + lpszSection, true))
		m_pRegistry->WriteString(lpszName, lpszValue);
}

//-----------------------------------------------------------------------------

void CRegConfigIO::DeleteSection(LPCTSTR lpszSection)
{
	if (m_pRegistry->OpenKey(m_strPath, false))
		m_pRegistry->DeleteKey(lpszSection);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc
