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

/// @file ifc_xml_doc.h
/// Defines the xml classes.

/// @addtogroup Classes
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_exceptions.h"

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////
// Classes

class CXmlNode;
class CXmlNodeList;
class CXmlNodeAttrs;
class CXmlValue;
class CXmlDocument;
class CXmlDocReader;

///////////////////////////////////////////////////////////////////////////////
// Type Definitions

struct CXmlNodeAttrItem
{
	CString strName;
	CString strValue;
};

///////////////////////////////////////////////////////////////////////////////
/// CXmlNode - XML node class.

class CXmlNode
{
public:
	friend class CXmlNodeList;
	friend class CXmlNodeAttrs;
	friend class CXmlDocument;
private:
	CComPtr<IXMLDOMNode> m_pDomNode;
	CXmlNodeAttrs *m_pAttrs;
private:
	void Init();
public:
	/// Constructor.
	CXmlNode();
	/// Copy constructor.
	CXmlNode(const CXmlNode& src);
	/// Destructor
	virtual ~CXmlNode();

	/// Assignment operator.
	CXmlNode& operator = (const CXmlNode& rhs);

	/// Returns the node name.
	CString GetName() const;
	/// Returns the node string.
	CString GetString() const;
	/// Sets the string of the node.
	bool SetString(LPCTSTR lpszStr);
	/// Returns the number of child nodes.
	int GetChildNodeCount() const;
	/// Determines whether the node has children.
	bool HasChildNodes() const;
	/// Determines whether the child node with the specified name exists or not.
	bool ChildNodeExists(LPCTSTR lpszNodeName) const;
	/// Returns the child node by 0-based index.
	CXmlNode GetChildNode(int nIndex) const;
	/// Returns the child node by node name.
	CXmlNode GetChildNode(LPCTSTR lpszNodeName) const;
	/// Returns the child node by node name, and adds it if not found.
	CXmlNode GetChildNode(LPCTSTR lpszNodeName, bool bAddIfNotExists);
	/// Returns the parent node.
	CXmlNode GetParentNode() const;

	/// Adds a new node under the node and returns the added node.
	CXmlNode AddChildNode(LPCTSTR lpszName, LPCTSTR lpszString = TEXT(""));
	/// Removes all child nodes.
	void RemoveChildNodes();
	/// Removes this node, and sets node object to null.
	void Remove();

	/// Gets the list of matching nodes filtered by XPath expression.
	bool SelectNodes(LPCTSTR lpszXPathExpr, CXmlNodeList& List);
	/// Gets the single matching node filtered by XPath expression.
	CXmlNode SelectNode(LPCTSTR lpszXPathExpr);

	/// Returns the CXmlNodeAttrs object.
	CXmlNodeAttrs& Attributes() { return *m_pAttrs; }

	/// Indicates whether the node object is a null node or not.
	bool IsNull() const { return (m_pDomNode == NULL); }
};

///////////////////////////////////////////////////////////////////////////////
/// CXmlNodeList - XML node list class.

class CXmlNodeList
{
public:
	friend class CXmlNode;
private:
	CComPtr<IXMLDOMNodeList> m_pDomNodeList;
public:
	CXmlNodeList();
	virtual ~CXmlNodeList();

	/// Returns the number of items.
	int GetCount() const;
	/// Returns the item at the specified position.
	CXmlNode GetItem(int nIndex) const;

	/// Indicates whether the list is empty or not.
	bool IsEmpty() const { return GetCount() <= 0; }
	/// Indicates whether the node list object is a null or not.
	bool IsNull() const { return (m_pDomNodeList == NULL); }

	/// Returns the item at the specified position.
	CXmlNode operator[] (int nIndex) const { return GetItem(nIndex); }
};

///////////////////////////////////////////////////////////////////////////////
/// CXmlValue - XML value class.

class CXmlValue
{
private:
	CString m_strValue;
public:
	/// Constructor.
	CXmlValue(LPCTSTR lpszValue) : m_strValue(lpszValue) {}

	/// Sets the value.
	void SetValue(LPCTSTR lpszValue) { m_strValue = lpszValue; }
	/// Gets the value as a string pointer.
	operator LPCTSTR() const { return m_strValue; }

	/// Returns the value as a string;
	const CString& AsString() const { return m_strValue; }
	/// Returns the value as an integer.
	int AsInteger(int nDefault = 0) const;
	/// Returns the value as a 64-bit integer.
	INT64 AsInt64(INT64 nDefault = 0) const;
	/// Returns the value as an unsigned integer.
	UINT AsUInteger(UINT nDefault = 0) const;
	/// Returns the value as a 64-bit unsigned integer.
	UINT64 AsUInt64(UINT64 nDefault = 0) const;
	/// Returns the value as a float.
	double AsFloat(double fDefault = 0) const;
	/// Returns the value as a boolean.
	bool AsBoolean(bool bDefault = false) const;
};

///////////////////////////////////////////////////////////////////////////////
/// CXmlNodeAttrs - XML attribute class.

class CXmlNodeAttrs
{
private:
	CXmlNode& m_Node;
private:
	CComPtr<IXMLDOMNode>& GetDomNode() const { return m_Node.m_pDomNode; }
private:
	CXmlNodeAttrs(const CXmlNodeAttrs& src);
	CXmlNodeAttrs& operator = (const CXmlNodeAttrs& rhs);
public:
	/// Constructor.
	CXmlNodeAttrs(CXmlNode& Node);
	/// Destructor
	virtual ~CXmlNodeAttrs();

	/// Adds or updates an attribute item with specified name and value, returns false if error occured.
	bool Add(LPCTSTR lpszName, LPCTSTR lpszValue);
	/// Removes an attribute item by name.
	bool Remove(LPCTSTR lpszName);
	/// Clears all attributes.
	void Clear();

	/// Returns the number of attribute items.
	int GetCount() const;
	/// Returns the attribute item at the specified position.
	CXmlNodeAttrItem GetItem(int nIndex) const;
	/// Returns the value of a attribute item associated with a given name.
	CXmlValue ValueOf(LPCTSTR lpszName) const;
	/// Determines whether the attribute name exists or not.
	bool Exists(LPCTSTR lpszName) const;

	/// Returns the attribute item at the specified position.
	CXmlNodeAttrItem operator[] (int nIndex) const { return GetItem(nIndex); }
	/// Returns the value of a attribute item associated with a given name.
	CString operator[] (LPCTSTR lpszName) const { return ValueOf(lpszName).AsString(); }
};

///////////////////////////////////////////////////////////////////////////////
/// CXmlDocument - XML document class.

class CXmlDocument
{
private:
	CComPtr<IXMLDOMDocument> m_pDoc;
	CXmlDocReader *m_pReader;
	bool m_bComInited;
private:
	bool CreateRootNode(LPCTSTR lpszNodeName);
private:
	CXmlDocument(const CXmlDocument& src);
	CXmlDocument& operator = (const CXmlDocument& rhs);
public:
	/// Default Constructor.
	CXmlDocument();
	/// Destructor
	virtual ~CXmlDocument();

	/// Saves the XML document to the stream.
	bool SaveToStream(CStream& Stream);
	/// Loads the XML document from the stream.
	bool LoadFromStream(CStream& Stream);
	/// Saves the XML document to the std::string.
	bool SaveToString(std::string& str);
	/// Loads the XML document from the string.
	bool LoadFromString(const char *pStr, int nBytes);
	/// Saves the XML document to the buffer.
	bool SaveToBuffer(CBuffer& Buffer);
	/// Loads the XML document from the buffer.
	bool LoadFromBuffer(const CBuffer& Buffer);
	/// Saves the XML document to the file.
	bool SaveToFile(LPCTSTR lpszFileName);
	/// Loads the XML document from the file.
	bool LoadFromFile(LPCTSTR lpszFileName);

	/// Clears all data from the XML document.
	void Clear();
	/// Creates a new XML document.
	bool CreateNew(LPCTSTR lpszEncoding = TEXT("utf-8"), LPCTSTR lpszRootNodeName = TEXT("root"));

	/// Returns the root node.
	CXmlNode GetRootNode();
	/// Returns the CXmlDocReader object.
	CXmlDocReader& Reader() { return *m_pReader; }
	/// Indicates whether the xml doc object is a null or not.
	bool IsNull() const { return (m_pDoc == NULL); }
};

///////////////////////////////////////////////////////////////////////////////
/// CXmlDocReader - XML document reader class.
///
/// CXmlDocReader is a helper class to read and parse the xml content. The class
/// use a string named "NamePath" to access the xml nodes and attributes.
/// NamePath is a string of a xml path, it consists of several xml node names or attribute names,
/// and separates with period. The xml root node name should be omitted in the NamePath.
//  Especially, an empty NamePath string can be use to read the root node string.
/// The NamePath string is case-sensitive.
/// For example:
/** @verbatim
	<?xml version="1.0"?>
	<config>
	   <test1>
		   <test2>abc</test2>
	   </test1>
	   <test3 value="123"/>
	</config>
@endverbatim */
/// You can read the xml like this:
/** @code
	s = XmlDocReader.GetString("test1.test2").AsString();
	i = XmlDocReader.GetInteger("test3.value").AsInteger();
@endcode */

class CXmlDocReader
{
private:
	CXmlDocument& m_XmlDoc;
private:
	void SplitNamePath(LPCTSTR lpszNamePath, CStrList& List);
public:
	CXmlDocReader(CXmlDocument& XmlDoc) : m_XmlDoc(XmlDoc) {}
	virtual ~CXmlDocReader() {}

	/// Gets the value of the specified NamePath from the XML document.
	CXmlValue GetValue(LPCTSTR lpszNamePath);
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc

/// @}
