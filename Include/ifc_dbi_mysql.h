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

/// @file ifc_dbi_mysql.h
/// Defines the mysql classes.

#pragma once

#include "ifc_database.h"

#include <errmsg.h>
#include <mysql.h>

/// The namespace of IFC.
namespace ifc
{

/// @addtogroup Database
/// @{

///////////////////////////////////////////////////////////////////////////////
// Classes

class CMySqlConnection;
class CMySqlField;
class CMySqlDataSet;
class CMySqlQuery;
class CMySqlDatabase;

///////////////////////////////////////////////////////////////////////////////
/// CMySqlConnection

class CMySqlConnection : public CDbConnection
{
private:
	MYSQL m_ConnObject;

public:
	CMySqlConnection(CDatabase *pDatabase);
	virtual ~CMySqlConnection();

	virtual void DoConnect();
	virtual void DoDisconnect();

	MYSQL& GetConnObject() { return m_ConnObject; }
};

///////////////////////////////////////////////////////////////////////////////
/// CMySqlField

class CMySqlField : public CDbField
{
private:
	char *m_pDataPtr;
	int m_nDataSize;
public:
	CMySqlField();
	virtual ~CMySqlField() {}

	void SetData(void *pDataPtr, int nDataSize);
	virtual bool IsNull() const { return (m_pDataPtr == NULL); }
	virtual CString AsString() const;
};

///////////////////////////////////////////////////////////////////////////////
/// CMySqlDataSet

class CMySqlDataSet : public CDbDataSet
{
private:
	MYSQL_RES *m_pRes;
	MYSQL_ROW m_pRow;

private:
	MYSQL& GetConnObject();
	void FreeDataSet();

protected:
	virtual void InitDataSet();
	virtual void InitFieldDefs();

public:
	CMySqlDataSet(CDbQuery* pDbQuery);
	virtual ~CMySqlDataSet();

	virtual bool Rewind();
	virtual bool Next();

	virtual UINT64 GetRecordCount();
	virtual bool IsEmpty();
};

///////////////////////////////////////////////////////////////////////////////
/// CMySqlQuery

class CMySqlQuery : public CDbQuery
{
private:
	MYSQL& GetConnObject();

protected:
	virtual void DoExecute(CDbDataSet *pResultDataSet);

public:
	CMySqlQuery(CDatabase *pDatabase);
	virtual ~CMySqlQuery();

	virtual CString EscapeString(LPCTSTR lpszStr);
	virtual UINT GetAffectedRowCount();
	virtual UINT64 GetLastInsertId();
};

///////////////////////////////////////////////////////////////////////////////
/// CMySqlDatabase

class CMySqlDatabase : public CDatabase
{
public:
	virtual CDbConnection* CreateDbConnection() { return new CMySqlConnection(this); }
	virtual CDbDataSet* CreateDbDataSet(CDbQuery* pDbQuery) { return new CMySqlDataSet(pDbQuery); }
	virtual CDbQuery* CreateDbQuery() { return new CMySqlQuery(this); }
};

///////////////////////////////////////////////////////////////////////////////

/// @}

} // namespace ifc
