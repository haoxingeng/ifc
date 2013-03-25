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

/// @file ifc_dbi_mysql.cpp

#include "stdafx.h"
#include "ifc_dbi_mysql.h"
#include "ifc_sysutils.h"
#include "ifc_errmsgs.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// CMySqlConnection

CMySqlConnection::CMySqlConnection(CDatabase* pDatabase) :
	CDbConnection(pDatabase)
{
	// nothing
}

CMySqlConnection::~CMySqlConnection()
{
	// nothing
}

//-----------------------------------------------------------------------------

void CMySqlConnection::DoConnect()
{
	static CCriticalSection s_Lock;
	CAutoLocker Locker(s_Lock);

	if (mysql_init(&m_ConnObject) == NULL)
		IfcThrowDbException(SEM_MYSQL_INIT_ERROR);

	int nValue = 0;
	mysql_options(&m_ConnObject, MYSQL_OPT_RECONNECT, (char*)&nValue);

	if (mysql_real_connect(&m_ConnObject,
		CT2A(m_pDatabase->GetDbConnParams()->GetHostName()),
		CT2A(m_pDatabase->GetDbConnParams()->GetUserName()),
		CT2A(m_pDatabase->GetDbConnParams()->GetPassword()),
		CT2A(m_pDatabase->GetDbConnParams()->GetDbName()),
		m_pDatabase->GetDbConnParams()->GetPort(), NULL, 0) == NULL)
	{
		mysql_close(&m_ConnObject);
		IfcThrowDbException(CA2T(mysql_error(&m_ConnObject)));
	}

	// for MYSQL 5.0.7 or higher
	CString strInitialCharSet = m_pDatabase->GetDbOptions()->GetInitialCharSet();
	if (!strInitialCharSet.IsEmpty())
		mysql_set_character_set(&m_ConnObject, CT2A(strInitialCharSet));
}

//-----------------------------------------------------------------------------

void CMySqlConnection::DoDisconnect()
{
	mysql_close(&m_ConnObject);
}

///////////////////////////////////////////////////////////////////////////////
// CMySqlField

CMySqlField::CMySqlField()
{
	m_pDataPtr = NULL;
	m_nDataSize = 0;
}

//-----------------------------------------------------------------------------

void CMySqlField::SetData(void *pDataPtr, int nDataSize)
{
	m_pDataPtr = (char*)pDataPtr;
	m_nDataSize = nDataSize;
}

//-----------------------------------------------------------------------------

CString CMySqlField::AsString() const
{
	CString strResult;

	if (m_pDataPtr && m_nDataSize > 0)
	{
		CStringA s(m_pDataPtr, m_nDataSize);
		strResult = s;
	}

	return strResult;
}

///////////////////////////////////////////////////////////////////////////////
// CMySqlDataSet

CMySqlDataSet::CMySqlDataSet(CDbQuery* pDbQuery) :
	CDbDataSet(pDbQuery),
	m_pRes(NULL),
	m_pRow(NULL)
{
	// nothing
}

CMySqlDataSet::~CMySqlDataSet()
{
	FreeDataSet();
}

//-----------------------------------------------------------------------------

MYSQL& CMySqlDataSet::GetConnObject()
{
	return ((CMySqlConnection*)m_pDbQuery->GetDbConnection())->GetConnObject();
}

//-----------------------------------------------------------------------------

void CMySqlDataSet::FreeDataSet()
{
	if (m_pRes)
		mysql_free_result(m_pRes);
	m_pRes = NULL;
}

//-----------------------------------------------------------------------------

void CMySqlDataSet::InitDataSet()
{
	m_pRes = mysql_store_result(&GetConnObject());

	// if error
	if (!m_pRes)
	{
		if (mysql_errno(&GetConnObject()) != 0)
			IfcThrowDbException(CA2T(mysql_error(&GetConnObject())));
		else
			IfcThrowDbException(SEM_MYSQL_STORE_RESULT_ERROR);
	}
}

//-----------------------------------------------------------------------------

void CMySqlDataSet::InitFieldDefs()
{
	MYSQL_FIELD *pMySqlField;
	CDbFieldDef* pFieldDef;
	int nFieldCount;

	m_DbFieldDefList.Clear();
	nFieldCount = mysql_num_fields(m_pRes);
	for (int i = 0; i < nFieldCount; i++)
	{
		pMySqlField = mysql_fetch_field(m_pRes);
		pFieldDef = new CDbFieldDef();
		pFieldDef->SetData(pMySqlField->name, pMySqlField->type);
		m_DbFieldDefList.Add(pFieldDef);
	}
}

//-----------------------------------------------------------------------------

bool CMySqlDataSet::Rewind()
{
	if (GetRecordCount() > 0)
	{
		mysql_data_seek(m_pRes, 0);
		return true;
	}
	else
		return false;
}

//-----------------------------------------------------------------------------

bool CMySqlDataSet::Next()
{
	m_pRow = mysql_fetch_row(m_pRes);
	if (m_pRow)
	{
		CMySqlField* pField;
		int nFieldCount;
		UINT* pLengths;

		nFieldCount = mysql_num_fields(m_pRes);
		pLengths = (UINT*)mysql_fetch_lengths(m_pRes);

		for (int i = 0; i < nFieldCount; i++)
		{
			if (i < m_DbFieldList.GetCount())
			{
				pField = (CMySqlField*)m_DbFieldList[i];
			}
			else
			{
				pField = new CMySqlField();
				m_DbFieldList.Add(pField);
			}

			pField->SetData(m_pRow[i], pLengths[i]);
		}
	}

	return (m_pRow != NULL);
}

//-----------------------------------------------------------------------------

UINT64 CMySqlDataSet::GetRecordCount()
{
	if (m_pRes)
		return (UINT64)mysql_num_rows(m_pRes);
	else
		return 0;
}

//-----------------------------------------------------------------------------

bool CMySqlDataSet::IsEmpty()
{
	return (GetRecordCount() == 0);
}

///////////////////////////////////////////////////////////////////////////////
// CMySqlQuery

CMySqlQuery::CMySqlQuery(CDatabase* pDatabase) :
	CDbQuery(pDatabase)
{
	// nothing
}

CMySqlQuery::~CMySqlQuery()
{
	// nothing
}

//-----------------------------------------------------------------------------

MYSQL& CMySqlQuery::GetConnObject()
{
	return ((CMySqlConnection*)m_pDbConnection)->GetConnObject();
}

//-----------------------------------------------------------------------------

void CMySqlQuery::DoExecute(CDbDataSet *pResultDataSet)
{
	/*
	Upon connection, mysql_real_connect() sets the reconnect flag (part of the
	MYSQL structure) to a value of 1 in versions of the API older than 5.0.3,
	or 0 in newer versions. A value of 1 for this flag indicates that if a
	statement cannot be performed because of a lost connection, to try reconnecting
	to the server before giving up. You can use the MYSQL_OPT_RECONNECT option
	to mysql_options() to control reconnection behavior.
	*/

	for (int nTimes = 0; nTimes < 2; nTimes++)
	{
		CStringA strSqlToExec(m_strSql);
		int r = mysql_real_query(&GetConnObject(), strSqlToExec, (int)strSqlToExec.GetLength());

		// if error
		if (r != 0)
		{
			if (nTimes == 0)
			{
				int nErrNo = mysql_errno(&GetConnObject());
				if (nErrNo == CR_SERVER_GONE_ERROR || nErrNo == CR_SERVER_LOST)
				{
					GetDbConnection()->ActivateConnection(true);
					continue;
				}
			}

			CString strSql(m_strSql);
			if (strSql.GetLength() > 1024*2)
			{
				strSql.Truncate(100);
				strSql += TEXT("...");
			}

			CString strErrMsg = FormatString(TEXT("%s; Error: %s"), strSql, CA2T(mysql_error(&GetConnObject())));
			IfcThrowDbException(strErrMsg);
		}
		else break;
	}
}

//-----------------------------------------------------------------------------

CString CMySqlQuery::EscapeString(LPCTSTR lpszStr)
{
	if (lpszStr == NULL || *lpszStr == '\0')
		return TEXT("");

	EnsureConnected();

	CStringA str(lpszStr);
	int nSrcLen = str.GetLength();
	CBuffer Buffer(nSrcLen * 2 + 1);
	char *pEnd;

	pEnd = (char*)Buffer.Data();
	pEnd += mysql_real_escape_string(&GetConnObject(), pEnd, str, nSrcLen);
	*pEnd = '\0';

	return CString(Buffer.Data());
}

//-----------------------------------------------------------------------------

UINT CMySqlQuery::GetAffectedRowCount()
{
	UINT nResult = 0;

	if (m_pDbConnection)
		nResult = (UINT)mysql_affected_rows(&GetConnObject());

	return nResult;
}

//-----------------------------------------------------------------------------

UINT64 CMySqlQuery::GetLastInsertId()
{
	UINT64 nResult = 0;

	if (m_pDbConnection)
		nResult = (UINT64)mysql_insert_id(&GetConnObject());

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
