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

/// @file ifc_database.cpp

#include "stdafx.h"
#include "ifc_database.h"
#include "ifc_sysutils.h"
#include "ifc_errmsgs.h"

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////
// CDbConnParams

CDbConnParams::CDbConnParams() :
	m_nPort(0)
{
	// nothing
}

//-----------------------------------------------------------------------------

CDbConnParams::CDbConnParams(const CDbConnParams& src)
{
	m_strHostName = src.m_strHostName;
	m_strUserName = src.m_strUserName;
	m_strPassword = src.m_strPassword;
	m_strDbName = src.m_strDbName;
	m_nPort = src.m_nPort;
}

//-----------------------------------------------------------------------------

CDbConnParams::CDbConnParams(LPCTSTR lpszHostName, LPCTSTR lpszUserName,
	LPCTSTR lpszPassword, LPCTSTR lpszDbName, int nPort)
{
	m_strHostName = lpszHostName;
	m_strUserName = lpszUserName;
	m_strPassword = lpszPassword;
	m_strDbName = lpszDbName;
	m_nPort = nPort;
}

///////////////////////////////////////////////////////////////////////////////
// CDbOptions

CDbOptions::CDbOptions()
{
	SetMaxDbConnections(DEF_MAX_DB_CONNECTIONS);
}

//-----------------------------------------------------------------------------

void CDbOptions::SetMaxDbConnections(int nValue)
{
	if (nValue < 1) nValue = 1;
	m_nMaxDbConnections = nValue;
}

//-----------------------------------------------------------------------------

void CDbOptions::SetInitialSqlCmd(LPCTSTR lpszValue)
{
	m_InitialSqlCmdList.Clear();
	m_InitialSqlCmdList.Add(lpszValue);
}

//-----------------------------------------------------------------------------

void CDbOptions::SetInitialCharSet(LPCTSTR lpszValue)
{
	m_strInitialCharSet = lpszValue;
}

///////////////////////////////////////////////////////////////////////////////
// CDbConnection

CDbConnection::CDbConnection(CDatabase *pDatabase)
{
	m_pDatabase = pDatabase;
	m_bConnected = false;
	m_bBusy = false;
}

//-----------------------------------------------------------------------------

CDbConnection::~CDbConnection()
{
	// nothing
}

//-----------------------------------------------------------------------------

void CDbConnection::Connect()
{
	if (!m_bConnected)
	{
		DoConnect();
		ExecCmdOnConnected();
		m_bConnected = true;
	}
}

//-----------------------------------------------------------------------------

void CDbConnection::Disconnect()
{
	if (m_bConnected)
	{
		DoDisconnect();
		m_bConnected = false;
	}
}

//-----------------------------------------------------------------------------

void CDbConnection::ExecCmdOnConnected()
{
	try
	{
		CStrList& CmdList = m_pDatabase->GetDbOptions()->InitialSqlCmdList();
		if (!CmdList.IsEmpty())
		{
			CDbQueryWrapper Query(m_pDatabase->CreateDbQuery());
			Query->m_pDbConnection = this;

			for (int i = 0; i < CmdList.GetCount(); ++i)
			{
				try
				{
					Query->SetSql(CmdList[i]);
					Query->Execute();
				}
				catch (IFC_EXCEPT_OBJ e)
				{
					IFC_DELETE_MFC_EXCEPT_OBJ(e);
				}
				catch (...)
				{}
			}

			Query->m_pDbConnection = NULL;
		}
	}
	catch (IFC_EXCEPT_OBJ e)
	{
		IFC_DELETE_MFC_EXCEPT_OBJ(e);
	}
	catch (...)
	{}
}

//-----------------------------------------------------------------------------

bool CDbConnection::GetDbConnection()
{
	if (!m_bBusy)
	{
		ActivateConnection();
		m_bBusy = true;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------

void CDbConnection::ReturnDbConnection()
{
	m_bBusy = false;
}

//-----------------------------------------------------------------------------

bool CDbConnection::IsBusy()
{
	return m_bBusy;
}

//-----------------------------------------------------------------------------

void CDbConnection::ActivateConnection(bool bForce)
{
	if (!m_bConnected || bForce)
	{
		Disconnect();
		Connect();
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////
// CDbConnectionPool

CDbConnectionPool::CDbConnectionPool(CDatabase *pDatabase) :
	m_pDatabase(pDatabase)
{
	// nothing
}

CDbConnectionPool::~CDbConnectionPool()
{
	ClearPool();
}

//-----------------------------------------------------------------------------

void CDbConnectionPool::ClearPool()
{
	CAutoLocker Locker(m_Lock);

	for (int i = 0; i < m_DbConnectionList.GetCount(); i++)
	{
		CDbConnection *pDbConnection;
		pDbConnection = (CDbConnection*)m_DbConnectionList[i];
		pDbConnection->DoDisconnect();
		delete pDbConnection;
	}

	m_DbConnectionList.Clear();
}

//-----------------------------------------------------------------------------

CDbConnection* CDbConnectionPool::GetConnection()
{
	CDbConnection *pDbConnection = NULL;
	bool bResult = false;

	{
		CAutoLocker Locker(m_Lock);

		for (int i = 0; i < m_DbConnectionList.GetCount(); i++)
		{
			pDbConnection = (CDbConnection*)m_DbConnectionList[i];
			bResult = pDbConnection->GetDbConnection();
			if (bResult) break;
		}

		if (!bResult && (m_DbConnectionList.GetCount() < m_pDatabase->GetDbOptions()->GetMaxDbConnections()))
		{
			pDbConnection = m_pDatabase->CreateDbConnection();
			m_DbConnectionList.Add(pDbConnection);
			bResult = pDbConnection->GetDbConnection();
		}
	}

	if (!bResult)
		IfcThrowDbException(SEM_GET_CONN_FROM_POOL_ERROR);

	return pDbConnection;
}

//-----------------------------------------------------------------------------

void CDbConnectionPool::ReturnConnection(CDbConnection *pDbConnection)
{
	CAutoLocker Locker(m_Lock);
	pDbConnection->ReturnDbConnection();
}

///////////////////////////////////////////////////////////////////////////////
// CDbFieldDef

CDbFieldDef::CDbFieldDef(LPCTSTR lpszName, int nType)
{
	m_strName = lpszName;
	m_nType = nType;
}

CDbFieldDef::CDbFieldDef(const CDbFieldDef& src)
{
	m_strName = src.m_strName;
	m_nType = src.m_nType;
}

///////////////////////////////////////////////////////////////////////////////
// CDbFieldDefList

CDbFieldDefList::CDbFieldDefList()
{
	// nothing
}

CDbFieldDefList::~CDbFieldDefList()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CDbFieldDefList::Add(CDbFieldDef *pFieldDef)
{
	if (pFieldDef != NULL)
		m_Items.Add(pFieldDef);
}

//-----------------------------------------------------------------------------

void CDbFieldDefList::Clear()
{
	for (int i = 0; i < m_Items.GetCount(); i++)
		delete (CDbFieldDef*)m_Items[i];
	m_Items.Clear();
}

//-----------------------------------------------------------------------------

int CDbFieldDefList::IndexOfName(LPCTSTR lpszName)
{
	int nIndex = -1;

	for (int i = 0; i < m_Items.GetCount(); i++)
	{
		if (SameText(((CDbFieldDef*)m_Items[i])->GetName(), lpszName))
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

//-----------------------------------------------------------------------------

void CDbFieldDefList::GetFieldNameList(CStrList& List)
{
	List.Clear();
	for (int i = 0; i < m_Items.GetCount(); i++)
		List.Add(((CDbFieldDef*)m_Items[i])->GetName());
}

//-----------------------------------------------------------------------------

CDbFieldDef* CDbFieldDefList::operator[] (int nIndex)
{
	if (nIndex >= 0 && nIndex < m_Items.GetCount())
		return (CDbFieldDef*)m_Items[nIndex];
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// CDbField

CDbField::CDbField()
{
	// nothing
}

//-----------------------------------------------------------------------------

int CDbField::AsInteger(int nDefault) const
{
	return StrToInt(AsString(), nDefault);
}

//-----------------------------------------------------------------------------

INT64 CDbField::AsInt64(INT64 nDefault) const
{
	return StrToInt64(AsString(), nDefault);
}

//-----------------------------------------------------------------------------

double CDbField::AsFloat(double fDefault) const
{
	return StrToFloat(AsString(), fDefault);
}

//-----------------------------------------------------------------------------

bool CDbField::AsBoolean(bool bDefault) const
{
	return AsInteger(bDefault? 1 : 0) != 0;
}

///////////////////////////////////////////////////////////////////////////////
// CDbFieldList

CDbFieldList::CDbFieldList()
{
	// nothing
}

CDbFieldList::~CDbFieldList()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CDbFieldList::Add(CDbField *pField)
{
	m_Items.Add(pField);
}

//-----------------------------------------------------------------------------

void CDbFieldList::Clear()
{
	for (int i = 0; i < m_Items.GetCount(); i++)
		delete (CDbField*)m_Items[i];
	m_Items.Clear();
}

//-----------------------------------------------------------------------------

CDbField* CDbFieldList::operator[] (int nIndex)
{
	if (nIndex >= 0 && nIndex < m_Items.GetCount())
		return (CDbField*)m_Items[nIndex];
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// CDbParamList

CDbParamList::CDbParamList(CDbQuery *pDbQuery) :
	m_pDbQuery(pDbQuery)
{
	// nothing
}

CDbParamList::~CDbParamList()
{
	Clear();
}

//-----------------------------------------------------------------------------

CDbParam* CDbParamList::FindParam(LPCTSTR lpszName)
{
	CDbParam *pResult = NULL;

	for (int i = 0; i < m_Items.GetCount(); i++)
		if (SameText(((CDbParam*)m_Items[i])->m_strName, lpszName))
		{
			pResult = (CDbParam*)m_Items[i];
			break;
		}

	return pResult;
}

//-----------------------------------------------------------------------------

CDbParam* CDbParamList::FindParam(int nNumber)
{
	CDbParam *pResult = NULL;

	for (int i = 0; i < m_Items.GetCount(); i++)
		if (((CDbParam*)m_Items[i])->m_nNumber == nNumber)
		{
			pResult = (CDbParam*)m_Items[i];
			break;
		}

	return pResult;
}

//-----------------------------------------------------------------------------

CDbParam* CDbParamList::CreateParam(LPCTSTR lpszName, int nNumber)
{
	CDbParam *pResult = m_pDbQuery->GetDatabase()->CreateDbParam();

	pResult->m_pDbQuery = m_pDbQuery;
	pResult->m_strName = lpszName;
	pResult->m_nNumber = nNumber;

	return pResult;
}

//-----------------------------------------------------------------------------

CDbParam* CDbParamList::ParamByName(LPCTSTR lpszName)
{
	CDbParam *pResult = FindParam(lpszName);
	if (!pResult)
	{
		pResult = CreateParam(lpszName, 0);
		m_Items.Add(pResult);
	}

	return pResult;
}

//-----------------------------------------------------------------------------

CDbParam* CDbParamList::ParamByNumber(int nNumber)
{
	CDbParam *pResult = FindParam(nNumber);
	if (!pResult)
	{
		pResult = CreateParam(TEXT(""), nNumber);
		m_Items.Add(pResult);
	}

	return pResult;
}

//-----------------------------------------------------------------------------

void CDbParamList::Clear()
{
	for (int i = 0; i < m_Items.GetCount(); i++)
		delete (CDbParam*)m_Items[i];
	m_Items.Clear();
}

///////////////////////////////////////////////////////////////////////////////
// CDbDataSet

CDbDataSet::CDbDataSet(CDbQuery *pDbQuery) :
	m_pDbQuery(pDbQuery)
{
	// nothing
}

CDbDataSet::~CDbDataSet()
{
	// nothing
}

//-----------------------------------------------------------------------------

UINT64 CDbDataSet::GetRecordCount()
{
	IfcThrowDbException(SEM_FEATURE_NOT_SUPPORTED);
	return 0;
}

//-----------------------------------------------------------------------------

bool CDbDataSet::IsEmpty()
{
	IfcThrowDbException(SEM_FEATURE_NOT_SUPPORTED);
	return true;
}

//-----------------------------------------------------------------------------

int CDbDataSet::GetFieldCount()
{
	return m_DbFieldDefList.GetCount();
}

//-----------------------------------------------------------------------------

CDbFieldDef* CDbDataSet::GetFieldDefs(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_DbFieldDefList.GetCount())
		return m_DbFieldDefList[nIndex];
	else
		IfcThrowDbException(SEM_INDEX_ERROR);

	return NULL;
}

//-----------------------------------------------------------------------------

CDbField* CDbDataSet::GetFields(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_DbFieldList.GetCount())
		return m_DbFieldList[nIndex];
	else
		IfcThrowDbException(SEM_INDEX_ERROR);

	return NULL;
}

//-----------------------------------------------------------------------------

CDbField* CDbDataSet::GetFields(LPCTSTR lpszName)
{
	int nIndex = m_DbFieldDefList.IndexOfName(lpszName);

	if (nIndex >= 0)
		return GetFields(nIndex);
	else
		IfcThrowDbException(FormatString(SEM_FIELD_NAME_ERROR, lpszName));

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// CDbQuery

CDbQuery::CDbQuery(CDatabase *pDatabase) :
	m_pDatabase(pDatabase),
	m_pDbConnection(NULL),
	m_pDbParamList(NULL)
{
	m_pDbParamList = pDatabase->CreateDbParamList(this);
}

CDbQuery::~CDbQuery()
{
	delete m_pDbParamList;

	if (m_pDbConnection)
		m_pDatabase->GetDbConnectionPool()->ReturnConnection(m_pDbConnection);
}

//-----------------------------------------------------------------------------

void CDbQuery::EnsureConnected()
{
	if (!m_pDbConnection)
		m_pDbConnection = m_pDatabase->GetDbConnectionPool()->GetConnection();
}

//-----------------------------------------------------------------------------

void CDbQuery::SetSql(LPCTSTR lpszSql)
{
	m_strSql = lpszSql;
	m_pDbParamList->Clear();

	DoSetSql(lpszSql);
}

//-----------------------------------------------------------------------------

CDbParam* CDbQuery::ParamByName(LPCTSTR lpszName)
{
	IfcThrowDbException(SEM_FEATURE_NOT_SUPPORTED);
	return NULL;
}

//-----------------------------------------------------------------------------

CDbParam* CDbQuery::ParamByNumber(int nNumber)
{
	IfcThrowDbException(SEM_FEATURE_NOT_SUPPORTED);
	return NULL;
}

//-----------------------------------------------------------------------------

void CDbQuery::Execute()
{
	EnsureConnected();
	DoExecute(NULL);
}

//-----------------------------------------------------------------------------

CDbDataSet* CDbQuery::Query()
{
	EnsureConnected();

	CDbDataSet *pDataSet = m_pDatabase->CreateDbDataSet(this);
	try
	{
		DoExecute(pDataSet);
		pDataSet->InitDataSet();
		pDataSet->InitFieldDefs();
	}
	catch (...)
	{
		delete pDataSet;
		pDataSet = NULL;
		throw;
	}

	return pDataSet;
}

//-----------------------------------------------------------------------------

CString CDbQuery::EscapeString(LPCTSTR lpszStr)
{
	IfcThrowDbException(SEM_FEATURE_NOT_SUPPORTED);
	return TEXT("");
}

//-----------------------------------------------------------------------------

UINT CDbQuery::GetAffectedRowCount()
{
	IfcThrowDbException(SEM_FEATURE_NOT_SUPPORTED);
	return 0;
}

//-----------------------------------------------------------------------------

UINT64 CDbQuery::GetLastInsertId()
{
	IfcThrowDbException(SEM_FEATURE_NOT_SUPPORTED);
	return 0;
}

//-----------------------------------------------------------------------------

CDbConnection* CDbQuery::GetDbConnection()
{
	EnsureConnected();
	return m_pDbConnection;
}

///////////////////////////////////////////////////////////////////////////////
// CDatabase

CDatabase::CDatabase()
{
	m_pDbConnParams = NULL;
	m_pDbOptions = NULL;
	m_pDbConnectionPool = NULL;
}

CDatabase::~CDatabase()
{
	delete m_pDbConnParams;
	delete m_pDbOptions;
	delete m_pDbConnectionPool;
}

//-----------------------------------------------------------------------------

void CDatabase::EnsureInited()
{
	if (!m_pDbConnParams)
	{
		m_pDbConnParams = CreateDbConnParams();
		m_pDbOptions = CreateDbOptions();
		m_pDbConnectionPool = CreateDbConnectionPool();
	}
}

//-----------------------------------------------------------------------------

CDbConnParams* CDatabase::GetDbConnParams()
{
	EnsureInited();
	return m_pDbConnParams;
}

//-----------------------------------------------------------------------------

CDbOptions* CDatabase::GetDbOptions()
{
	EnsureInited();
	return m_pDbOptions;
}

//-----------------------------------------------------------------------------

CDbConnectionPool* CDatabase::GetDbConnectionPool()
{
	EnsureInited();
	return m_pDbConnectionPool;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc
