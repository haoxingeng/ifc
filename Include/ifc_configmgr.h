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

/// @file ifc_configmgr.h
/// Defines the configuration manager classes.

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"

/// The namespace of IFC.
namespace ifc
{

/// @addtogroup Classes
/// @{

///////////////////////////////////////////////////////////////////////////////
// classes:

class CBaseConfigMgr;
class CConfigIO;
class CIniConfigIO;
class CRegConfigIO;

///////////////////////////////////////////////////////////////////////////////
/// CBaseConfigMgr - The base class for application configuration management.
///
/// CBaseConfigMgr is the base class that encapsulates access to the configuration data in an application.
/// Do not create instance of CBaseConfigMgr. Instead, create descendant class to implement your particular
/// configuration manager, such as CAppConfigMgr. As a base class, CBaseConfigMgr provides many methods to
/// read or write the configuration data.
///
/// The classic form of the descendant class is:
/** @code
	class CAppConfigMgr : public CBaseConfigMgr
	{
	private:
		int m_nTestInteger;
		CString m_strTestString;
	private:
		CString GetIniFileName() { return TEXT("c:\\test.ini"); }
	public:
		CAppConfigMgr() {}
		virtual ~CAppConfigMgr() {}

		// Call Load() to load config on application startup.
		void Load()
		{
			// Load all config data into manager from file
			LoadFromIniFile(GetIniFileName());

			// Load application's config data from manager by using GetXXX member functions:
			m_nTestInteger = GetInteger(TEXT("section"), TEXT("TestInteger"));
			m_strTestString = GetString(TEXT("section"), TEXT("TestString"));
		}

		// Call Save() to save config before application terminate.
		void Save()
		{
			// Save application's config data to manager by using SetXXX member functions:
			SetInteger(TEXT("section"), TEXT("TestInteger"), m_nTestInteger);
			SetString(TEXT("section"), TEXT("TestString"), m_strTestString);

			// Save all config data to file
			SaveToIniFile(GetIniFileName());
		}

		// The config data interface for application:
		int& GetTestInteger() { return m_nTestInteger; }
		CString& GetTestString() { return m_strTestString; }
	};
	@endcode
*/

class CBaseConfigMgr
{
private:
	CStrList m_Sections;   // <string:SectionName, data:CNameValueList*>
private:
	CNameValueList* FindNameValueList(LPCTSTR lpszSection);
	CNameValueList* AddSection(LPCTSTR lpszSection);
	bool GetValueStr(LPCTSTR lpszSection, LPCTSTR lpszName, CString& strValue);
	void Load(CConfigIO& IO);
	void Save(CConfigIO& IO);
protected:
	/// Retrieves a string value from the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] lpszDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	CString GetString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault = TEXT(""));

	/// Retrieves an integer value from the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] nDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	int GetInteger(LPCTSTR lpszSection, LPCTSTR lpszName, int nDefault = 0);

	/// Retrieves a bool value from the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] bDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	bool GetBool(LPCTSTR lpszSection, LPCTSTR lpszName, bool bDefault = false);

	/// Retrieves a double-precision value from the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] fDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	double GetFloat(LPCTSTR lpszSection, LPCTSTR lpszName, double fDefault = 0);

	/// Retrieves a binary value from the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[out] Value
	///   Stores the retrieved value.
	/// @return
	///   The number of bytes written to the buffer @a Value.
	int GetBinaryData(LPCTSTR lpszSection, LPCTSTR lpszName, CBuffer& Value);

	/// Writes a string value to the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] lpszValue
	///   The value to write.
	void SetString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Writes an integer value to the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] nValue
	///   The value to write.
	void SetInteger(LPCTSTR lpszSection, LPCTSTR lpszName, int nValue);

	/// Writes a bool value to the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] bValue
	///   The value to write.
	void SetBool(LPCTSTR lpszSection, LPCTSTR lpszName, bool bValue);

	/// Writes a double-precision value to the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] fValue
	///   The value to write.
	void SetFloat(LPCTSTR lpszSection, LPCTSTR lpszName, double fValue);

	/// Writes a binary value to the manager.
	///
	/// @param[in] lpszSection
	///   The section in the manager that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] pDataBuf
	///   Pointer to the data to be written.
	/// @param[in] nDataSize
	///   The size in bytes of the buffer @a pDataBuf.
	void SetBinaryData(LPCTSTR lpszSection, LPCTSTR lpszName, PVOID pDataBuf, int nDataSize);

	/// Deletes an entire section from the manager.
	void DeleteSection(LPCTSTR lpszSection);

	/// Reads all items from a specified section from the manager into a CNameValueList object.
	void GetSection(LPCTSTR lpszSection, CNameValueList& List);

	/// Loads all configuration data from a specified file into the manager.
	void LoadFromIniFile(LPCTSTR lpszFileName);

	/// Saves all configuration data managed by the manager to a specified file.
	void SaveToIniFile(LPCTSTR lpszFileName);

	/// Loads all configuration data from the Windows registry into the manager.
	void LoadFromRegistry(HKEY hRootKey, LPCTSTR lpszPath);

	/// Loads all configuration data managed by the manager to the Windows registry.
	void SaveToRegistry(HKEY hRootKey, LPCTSTR lpszPath);
public:
	/// Default constructor
	CBaseConfigMgr();

	/// Destructor
	virtual ~CBaseConfigMgr();

	/// Deletes all data from the manager.
	void Clear();
};

///////////////////////////////////////////////////////////////////////////////
// CConfigIO

class CConfigIO
{
public:
	struct CAutoUpdater
	{
		CConfigIO& m_Owner;
		CAutoUpdater(CConfigIO& Owner) : m_Owner(Owner)
			{ m_Owner.BeginUpdate(); }
		~CAutoUpdater()
			{ m_Owner.EndUpdate(); }
	};

public:
	virtual ~CConfigIO() {}

	virtual void BeginUpdate() {}
	virtual void EndUpdate() {}
	virtual void GetSectionList(CStrList& List) = 0;
	virtual void GetKeyList(LPCTSTR lpszSection, CStrList& List) = 0;
	virtual CString Read(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault) = 0;
	virtual void Write(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue) = 0;
	virtual void DeleteSection(LPCTSTR lpszSection) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// CIniConfigIO

class CIniConfigIO : public CConfigIO
{
private:
	CString m_strFileName;
	CMemIniFile *m_pIniFile;
public:
	CIniConfigIO(LPCTSTR lpszFileName);
	virtual ~CIniConfigIO() {}

	virtual void BeginUpdate();
	virtual void EndUpdate();
	virtual void GetSectionList(CStrList& List);
	virtual void GetKeyList(LPCTSTR lpszSection, CStrList& List);
	virtual CString Read(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault);
	virtual void Write(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void DeleteSection(LPCTSTR lpszSection);
};

///////////////////////////////////////////////////////////////////////////////
// CRegConfigIO

class CRegConfigIO : public CConfigIO
{
private:
	HKEY m_hRootKey;
	CString m_strPath;
	CRegistry *m_pRegistry;
public:
	CRegConfigIO(HKEY hRootKey, LPCTSTR lpszPath);
	virtual ~CRegConfigIO() {}

	virtual void BeginUpdate();
	virtual void EndUpdate();
	virtual void GetSectionList(CStrList& List);
	virtual void GetKeyList(LPCTSTR lpszSection, CStrList& List);
	virtual CString Read(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault);
	virtual void Write(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void DeleteSection(LPCTSTR lpszSection);
};

///////////////////////////////////////////////////////////////////////////////

/// @}

} // namespace ifc
