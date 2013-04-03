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

/// @file ifc_database.h
/// Defines the database classes.

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_socket.h"
#include "ifc_exceptions.h"

/// The namespace of IFC.
namespace ifc
{

/// @addtogroup Database
/// @{

///////////////////////////////////////////////////////////////////////////////
// Classes

class CDbConnParams;
class CDbOptions;
class CDbConnection;
class CDbConnectionPool;
class CDbFieldDef;
class CDbFieldDefList;
class CDbField;
class CDbFieldList;
class CDbParam;
class CDbParamList;
class CDbDataSet;
class CDbQuery;
class CDbQueryWrapper;
class CDbDataSetWrapper;
class CDatabase;

///////////////////////////////////////////////////////////////////////////////
/// CDbConnParams - Manages the database connection parameters.

class CDbConnParams
{
private:
	CString m_strHostName;
	CString m_strUserName;
	CString m_strPassword;
	CString m_strDbName;
	int m_nPort;

public:
	CDbConnParams();
	CDbConnParams(const CDbConnParams& src);
	CDbConnParams(LPCTSTR lpszHostName, LPCTSTR lpszUserName,
		LPCTSTR lpszPassword, LPCTSTR lpszDbName, const int nPort);

	CString GetHostName() const { return m_strHostName; }
	CString GetUserName() const { return m_strUserName; }
	CString GetPassword() const { return m_strPassword; }
	CString GetDbName() const { return m_strDbName; }
	int GetPort() const { return m_nPort; }

	void SetHostName(LPCTSTR lpszValue) { m_strHostName = lpszValue; }
	void SetUserName(LPCTSTR lpszValue) { m_strUserName = lpszValue; }
	void SetPassword(LPCTSTR lpszValue) { m_strPassword = lpszValue; }
	void SetDbName(LPCTSTR lpszValue) { m_strDbName = lpszValue; }
	void SetPort(const int nValue) { m_nPort = nValue; }
};

///////////////////////////////////////////////////////////////////////////////
/// CDbOptions - Manages the DBI options.

class CDbOptions
{
public:
	enum { DEF_MAX_DB_CONNECTIONS = 100 };      // The default value of the maximum connections in the pool.

private:
	int m_nMaxDbConnections;                    // The capacity of the pool.
	CStrList m_InitialSqlCmdList;               // The sql commands to be executed when a connection connected.
	CString m_strInitialCharSet;                // The character-set to be set when a connection connected.

public:
	CDbOptions();

	int GetMaxDbConnections() const { return m_nMaxDbConnections; }
	CStrList& InitialSqlCmdList() { return m_InitialSqlCmdList; }
	CString GetInitialCharSet() { return m_strInitialCharSet; }

	void SetMaxDbConnections(int nValue);
	void SetInitialSqlCmd(LPCTSTR lpszValue);
	void SetInitialCharSet(LPCTSTR lpszValue);
};

///////////////////////////////////////////////////////////////////////////////
/// CDbConnection - The base class of database connection.

class CDbConnection
{
public:
	friend class CDbConnectionPool;

protected:
	CDatabase *m_pDatabase;           // Reference to the CDatabase object.
	bool m_bConnected;                // Indicates whether the connection is connected or not.
	bool m_bBusy;                     // Indicates whether the connection is using or not.

protected:
	/// Establish and setup the connection. Throws exception on error.
	void Connect();
	/// Disconnect the connection.
	void Disconnect();
	/// Executes the sql command when the connection connected.
	void ExecCmdOnConnected();

	/// Gets the connection from the pool.
	bool GetDbConnection();
	/// Returns the connection to the pool.
	void ReturnDbConnection();
	/// Indicates whether the connection is got from the pool or not.
	bool IsBusy();

protected:
	/// Connects to the database server. Throws exception on error.
	virtual void DoConnect() {}
	/// Disconnects the connection.
	virtual void DoDisconnect() {}

public:
	CDbConnection(CDatabase *pDatabase);
	virtual ~CDbConnection();

	/// Activates the connection.
	void ActivateConnection(bool bForce = false);
};

///////////////////////////////////////////////////////////////////////////////
/// CDbConnectionPool - The base class of db connection pool.

class CDbConnectionPool
{
protected:
	CDatabase *m_pDatabase;           // Reference to the CDatabase object.
	CPointerList m_DbConnectionList;  // The connection list including idle connections and busy connections.
	CCriticalSection m_Lock;

protected:
	void ClearPool();

public:
	CDbConnectionPool(CDatabase *pDatabase);
	virtual ~CDbConnectionPool();

	/// Gets a connection from pool. Throws exception on error.
	virtual CDbConnection* GetConnection();
	/// Returns a connection to pool.
	virtual void ReturnConnection(CDbConnection *pDbConnection);
};

///////////////////////////////////////////////////////////////////////////////
/// CDbFieldDef - The field definition class.

class CDbFieldDef
{
protected:
	CString m_strName;                // The field name.
	int m_nType;                      // The field type.

public:
	CDbFieldDef() {}
	CDbFieldDef(LPCTSTR lpszName, int nType);
	CDbFieldDef(const CDbFieldDef& src);
	virtual ~CDbFieldDef() {}

	/// Sets the field info.
	void SetData(char *sName, int nType) { m_strName = sName; m_nType = nType; }

	/// Returns the field name.
	CString GetName() const { return m_strName; }
	/// Returns the field type.
	int GetType() const { return m_nType; }
};

///////////////////////////////////////////////////////////////////////////////
/// CDbFieldDefList - Manages the list of CDbFieldDef object.

class CDbFieldDefList
{
private:
	CPointerList m_Items;             // (CDbFieldDef* [])

public:
	CDbFieldDefList();
	virtual ~CDbFieldDefList();

	/// Adds a CDbFieldDef object to the list.
	void Add(CDbFieldDef *pFieldDef);
	/// Clears the list.
	void Clear();
	/// Returns the 0-based index of the item with the specified name.
	int IndexOfName(LPCTSTR lpszName);
	/// Gets the list of the field names.
	void GetFieldNameList(CStrList& List);

	/// Gets the item at the specified index (0-based).
	CDbFieldDef* operator[] (int nIndex);
	/// Returns the number of items in the list.
	int GetCount() const { return m_Items.GetCount(); }
};

///////////////////////////////////////////////////////////////////////////////
/// CDbField - The base class of field.

class CDbField
{
public:
	CDbField();
	virtual ~CDbField() {}

	/// Indicates whether the field value is null or not.
	virtual bool IsNull() const { return false; }
	/// Returns the field value as an integer.
	virtual int AsInteger(int nDefault = 0) const;
	/// Returns the field value as an 64-bit integer.
	virtual INT64 AsInt64(INT64 nDefault = 0) const;
	/// Returns the field value as a float.
	virtual double AsFloat(double fDefault = 0) const;
	/// Returns the field value as a boolean.
	virtual bool AsBoolean(bool bDefault = false) const;
	/// Returns the field value as a string.
	virtual CString AsString() const { return TEXT(""); };
};

///////////////////////////////////////////////////////////////////////////////
/// CDbFieldList - Manages the list of CDbField object.

class CDbFieldList
{
private:
	CPointerList m_Items;             // (CDbField* [])

public:
	CDbFieldList();
	virtual ~CDbFieldList();

	/// Adds a CDbField object to the list.
	void Add(CDbField *pField);
	/// Clears the list.
	void Clear();

	/// Gets the item at the specified index (0-based).
	CDbField* operator[] (int nIndex);
	/// Returns the number of items in the list.
	int GetCount() const { return m_Items.GetCount(); }
};

///////////////////////////////////////////////////////////////////////////////
/// CDbParam - The SQL param class.

class CDbParam
{
public:
	friend class CDbParamList;

protected:
	CDbQuery *m_pDbQuery;             // Reference to the CDbQuery object.
	CString m_strName;                // The param name.
	int m_nNumber;                    // The param number (1-based).

public:
	CDbParam() : m_pDbQuery(NULL), m_nNumber(0) {}
	virtual ~CDbParam() {}

	/// Sets the param value as an integer.
	virtual void SetInt(int nValue) {}
	/// Sets the param value as a float.
	virtual void SetFloat(double fValue) {}
	/// Sets the param value as a string.
	virtual void SetString(LPCTSTR lpszValue) {}
};

///////////////////////////////////////////////////////////////////////////////
/// CDbParamList - Manages the list of CDbParam object.

class CDbParamList
{
protected:
	CDbQuery *m_pDbQuery;             // Reference to the CDbQuery object.
	CPointerList m_Items;             // (CDbParam* [])

protected:
	virtual CDbParam* FindParam(LPCTSTR lpszName);
	virtual CDbParam* FindParam(int nNumber);
	virtual CDbParam* CreateParam(LPCTSTR lpszName, int nNumber);

public:
	CDbParamList(CDbQuery *pDbQuery);
	virtual ~CDbParamList();

	/// Gets the CDbParam object pointer with the specified name. Returns NULL if not found.
	virtual CDbParam* ParamByName(LPCTSTR lpszName);
	/// Gets the CDbParam object pointer with the specified number. Returns NULL if not found.
	virtual CDbParam* ParamByNumber(int nNumber);

	/// Clears the list.
	void Clear();
};

///////////////////////////////////////////////////////////////////////////////
/// CDbDataSet - The base class of dataset.
///
/// A CDbDataSet object maintains a cursor pointing to its current row of data.
/// Initially the cursor is positioned before the first row. The @a Next method
/// moves the cursor to the next row, and because it returns false when there are
/// no more rows in the dataset object, it can be used in a while loop to iterate
/// through the dataset.

class CDbDataSet
{
public:
	friend class CDbQuery;

protected:
	CDbQuery *m_pDbQuery;             // Reference to the CDbQuery object.
	CDbFieldDefList m_DbFieldDefList;
	CDbFieldList m_DbFieldList;

protected:
	/// Initializes the dataset, throws exception on error.
	virtual void InitDataSet() = 0;
	/// Initializes the field definitions.
	virtual void InitFieldDefs() = 0;

public:
	CDbDataSet(CDbQuery *pDbQuery);
	virtual ~CDbDataSet();

	/// Moves the cursor to the front of the dataset, just before the first row.
	virtual bool Rewind() = 0;
	/// Moves the cursor down one row from its current position. A dataset cursor is
	/// initially positioned before the first row. The first call to the @a Next method
	/// makes the first row the current row.
	/// Returns true if the new current row is valid, false if there are no more rows.
	virtual bool Next() = 0;

	/// Returns the total number of records in the dataset.
	virtual UINT64 GetRecordCount();
	/// Indicates whether the dataset is empty or not.
	virtual bool IsEmpty();

	/// Returns the number of fields.
	int GetFieldCount();
	/// Gets the CDbFieldDef object at the specified 0-based field index.
	CDbFieldDef* GetFieldDefs(int nIndex);
	/// Gets the CDbField object at the specified 0-based field index.
	CDbField* GetFields(int nIndex);
	/// Gets the CDbField object with the specified field name.
	CDbField* GetFields(LPCTSTR lpszName);
};

///////////////////////////////////////////////////////////////////////////////
/// CDbQuery - The base class of db querier.

class CDbQuery
{
public:
	friend class CDbConnection;

protected:
	CDatabase *m_pDatabase;           // Reference to the CDatabase object.
	CDbConnection *m_pDbConnection;   // Reference to the CDbConnection object.
	CDbParamList *m_pDbParamList;     // The SQL param list.
	CString m_strSql;                 // The SQL to be executed.

protected:
	void EnsureConnected();

protected:
	/// Sets the SQL Statement.
	virtual void DoSetSql(LPCTSTR lpszSql) {}

	/// Executes the SQL statement, throws exception on error.
	/// The @a pResultDataSet can be NULL.
	virtual void DoExecute(CDbDataSet *pResultDataSet) {}

public:
	CDbQuery(CDatabase *pDatabase);
	virtual ~CDbQuery();

	/// Sets the SQL Statement.
	void SetSql(LPCTSTR lpszSql);

	/// Gets the CDbParam object with the specified param name.
	virtual CDbParam* ParamByName(LPCTSTR lpszName);
	/// Gets the CDbParam object with the specified param number (1-based).
	virtual CDbParam* ParamByNumber(int nNumber);

	/// Executes the SQL statement that do not return the dataset (such as INSERT, UPDATE).
	/// Throws exception on error.
	void Execute();
	/// Executes the SQL statement that may return the dataset (such as SELECT).
	/// Throws exception on error.
	CDbDataSet* Query();

	/// Escapes the string for SQL statement.
	/// The @a lpszStr can contain the '\\0' character.
	virtual CString EscapeString(LPCTSTR lpszStr);
	/// Returns the number of rows changed/deleted/inserted by the last UPDATE, DELETE, or INSERT query.
	virtual UINT GetAffectedRowCount();
	/// Returns the ID generated for an auto increment column by the previous query.
	virtual UINT64 GetLastInsertId();

	/// Returns the CDbConnection object.
	CDbConnection* GetDbConnection();
	/// Returns the CDatabase object.
	CDatabase* GetDatabase() { return m_pDatabase; }
};

///////////////////////////////////////////////////////////////////////////////
/// CDbQueryWrapper - The wrapper class of CDbQuery.
///
/// The classic form of the usage is:
/** @code
   int main()
   {
	   CDbQueryWrapper qry(MyDatabase.CreateDbQuery());
	   qry->SetSql("select * from users");
	   // ...
   }
	@endcode
*/

class CDbQueryWrapper
{
private:
	CDbQuery *m_pDbQuery;

public:
	CDbQueryWrapper(CDbQuery *pDbQuery) : m_pDbQuery(pDbQuery) {}
	virtual ~CDbQueryWrapper() { delete m_pDbQuery; }

	CDbQuery* operator -> () { return m_pDbQuery; }
};

///////////////////////////////////////////////////////////////////////////////
/// CDbDataSetWrapper - The wrapper class of CDbDataSet.
///
/// The classic form of the usage is:
/** @code
   int main()
   {
	   CDbQueryWrapper qry(MyDatabase.CreateDbQuery());
	   CDbDataSetWrapper ds;

	   qry->SetSql("select * from users");
	   ds = qry->Query();

	   // ...
   }
	@endcode
*/

class CDbDataSetWrapper
{
private:
	CDbDataSet *m_pDbDataSet;

public:
	CDbDataSetWrapper() : m_pDbDataSet(NULL) {}
	virtual ~CDbDataSetWrapper() { delete m_pDbDataSet; }

	CDbDataSetWrapper& operator = (CDbDataSet *pDataSet)
	{
		if (m_pDbDataSet) delete m_pDbDataSet;
		m_pDbDataSet = pDataSet;
		return *this;
	}

	CDbDataSet* operator -> () { return m_pDbDataSet; }
};

///////////////////////////////////////////////////////////////////////////////
/// CDatabase - The database factory class.

class CDatabase
{
protected:
	CDbConnParams *m_pDbConnParams;
	CDbOptions *m_pDbOptions;
	CDbConnectionPool *m_pDbConnectionPool;
private:
	void EnsureInited();
public:
	CDatabase();
	virtual ~CDatabase();

	/// Creates a CDbConnParams object.
	virtual CDbConnParams* CreateDbConnParams() { return new CDbConnParams(); }
	/// Creates a CDbOptions object.
	virtual CDbOptions* CreateDbOptions() { return new CDbOptions(); }
	/// Creates a CDbConnection object.
	virtual CDbConnection* CreateDbConnection() = 0;
	/// Creates a CDbConnectionPool object.
	virtual CDbConnectionPool* CreateDbConnectionPool() { return new CDbConnectionPool(this); }
	/// Creates a CDbParam object.
	virtual CDbParam* CreateDbParam() { return new CDbParam(); }
	/// Creates a CDbParamList object.
	virtual CDbParamList* CreateDbParamList(CDbQuery* pDbQuery) { return new CDbParamList(pDbQuery); }
	/// Creates a CDbDataSet object.
	virtual CDbDataSet* CreateDbDataSet(CDbQuery* pDbQuery) = 0;
	/// Creates a CDbQuery object.
	virtual CDbQuery* CreateDbQuery() = 0;

	/// Returns the CDbConnParams object.
	CDbConnParams* GetDbConnParams();
	/// Returns the CDbOptions object.
	CDbOptions* GetDbOptions();
	/// Returns the CDbConnectionPool object.
	CDbConnectionPool* GetDbConnectionPool();
};

///////////////////////////////////////////////////////////////////////////////

/// @}

} // namespace ifc
