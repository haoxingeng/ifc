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

/// @file ifc_xml_doc.cpp

#include "stdafx.h"
#include "ifc_xml_doc.h"
#include "ifc_sysutils.h"

#include <atlsafe.h>

using namespace std;

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Macro Defines

#define IS_S_OK(hr) (((HRESULT)(hr)) == S_OK)

///////////////////////////////////////////////////////////////////////////////
// CXmlNode

CXmlNode::CXmlNode()
{
	Init();
}

CXmlNode::CXmlNode(const CXmlNode& src)
{
	Init();
	*this = src;
}

CXmlNode::~CXmlNode()
{
	delete m_pAttrs;
	m_pAttrs = NULL;
}

//-----------------------------------------------------------------------------

void CXmlNode::Init()
{
	m_pAttrs = new CXmlNodeAttrs(*this);
}

//-----------------------------------------------------------------------------

CXmlNode& CXmlNode::operator = (const CXmlNode& rhs)
{
	if (this == &rhs) return *this;

	m_pDomNode = rhs.m_pDomNode;
	return *this;
}

//-----------------------------------------------------------------------------

CString CXmlNode::GetName() const
{
	IFC_ASSERT(!IsNull());

	CComBSTR bstr;
	HRESULT hr = m_pDomNode->get_nodeName(&bstr);

	return CString(bstr);
}

//-----------------------------------------------------------------------------

CString CXmlNode::GetString() const
{
	IFC_ASSERT(!IsNull());

	CComBSTR bstr;
	HRESULT hr = m_pDomNode->get_text(&bstr);

	return CString(bstr);
}

//-----------------------------------------------------------------------------

bool CXmlNode::SetString(LPCTSTR lpszStr)
{
	IFC_ASSERT(!IsNull());
	return IS_S_OK(m_pDomNode->put_text(CComBSTR(lpszStr)));
}

//-----------------------------------------------------------------------------

int CXmlNode::GetChildNodeCount() const
{
	IFC_ASSERT(!IsNull());

	int nResult = 0;
	CComPtr<IXMLDOMNodeList> pDomNodeList;
	CComPtr<IXMLDOMNode> pDomNode;
	DOMNodeType nNodeType;

	HRESULT hr = m_pDomNode->get_childNodes(&pDomNodeList);
	if (IS_S_OK(hr))
	{
		long nDomNodeCount = 0;
		pDomNodeList->get_length(&nDomNodeCount);

		for (int i = 0; i < nDomNodeCount; i++)
		{
			pDomNode = NULL;
			if (IS_S_OK(pDomNodeList->get_item(i, &pDomNode)) &&
				IS_S_OK(pDomNode->get_nodeType(&nNodeType)) &&
				nNodeType == NODE_ELEMENT)
			{
				nResult++;
			}
		}
	}

	return nResult;
}

//-----------------------------------------------------------------------------

bool CXmlNode::HasChildNodes() const
{
	return (GetChildNodeCount() > 0);
}

//-----------------------------------------------------------------------------

bool CXmlNode::ChildNodeExists(LPCTSTR lpszNodeName) const
{
	IFC_ASSERT(!IsNull());

	bool bResult = false;
	CComPtr<IXMLDOMNode> pDomNode;
	HRESULT hr = m_pDomNode->selectSingleNode(CComBSTR(lpszNodeName), &pDomNode);

	if (IS_S_OK(hr) && pDomNode != NULL)
		bResult = true;

	return bResult;
}

//-----------------------------------------------------------------------------

CXmlNode CXmlNode::GetChildNode(int nIndex) const
{
	IFC_ASSERT(!IsNull());

	CXmlNode Result;
	CComPtr<IXMLDOMNodeList> pDomNodeList;
	CComPtr<IXMLDOMNode> pDomNode;
	DOMNodeType nNodeType;

	HRESULT hr = m_pDomNode->get_childNodes(&pDomNodeList);
	if (IS_S_OK(hr))
	{
		long nDomNodeCount = 0;
		pDomNodeList->get_length(&nDomNodeCount);

		for (int i = 0; i < nDomNodeCount; i++)
		{
			pDomNode = NULL;
			if (IS_S_OK(pDomNodeList->get_item(i, &pDomNode)) &&
				IS_S_OK(pDomNode->get_nodeType(&nNodeType)) &&
				nNodeType == NODE_ELEMENT)
			{
				if (nIndex == i)
				{
					Result.m_pDomNode = pDomNode;
					break;
				}
			}
		}
	}

	return Result;
}

//-----------------------------------------------------------------------------

CXmlNode CXmlNode::GetChildNode(LPCTSTR lpszNodeName) const
{
	IFC_ASSERT(!IsNull());

	CXmlNode Result;
	CComPtr<IXMLDOMNode> pDomNode;
	HRESULT hr = m_pDomNode->selectSingleNode(CComBSTR(lpszNodeName), &pDomNode);

	if (IS_S_OK(hr) && pDomNode != NULL)
		Result.m_pDomNode = pDomNode;

	return Result;
}

//-----------------------------------------------------------------------------

CXmlNode CXmlNode::GetChildNode(LPCTSTR lpszNodeName, bool bAddIfNotExists)
{
	CXmlNode Result = GetChildNode(lpszNodeName);

	if (Result.IsNull() && bAddIfNotExists)
		Result = AddChildNode(lpszNodeName);

	return Result;
}

//-----------------------------------------------------------------------------

CXmlNode CXmlNode::GetParentNode() const
{
	IFC_ASSERT(!IsNull());

	CXmlNode Result;
	CComPtr<IXMLDOMNode> pDomNode;
	HRESULT hr = m_pDomNode->get_parentNode(&pDomNode);

	if (IS_S_OK(hr) && pDomNode != NULL)
		Result.m_pDomNode = pDomNode;

	return Result;
}

//-----------------------------------------------------------------------------

CXmlNode CXmlNode::AddChildNode(LPCTSTR lpszName, LPCTSTR lpszString)
{
	IFC_ASSERT(!IsNull());

	CXmlNode Result;
	CComPtr<IXMLDOMDocument> pDomDoc;
	CComPtr<IXMLDOMElement> pDomElement;
	CComPtr<IXMLDOMNode> pDomNode;

	if (IS_S_OK(m_pDomNode->get_ownerDocument(&pDomDoc)) &&
		IS_S_OK(pDomDoc->createElement(CComBSTR(lpszName), &pDomElement)) &&
		IS_S_OK(m_pDomNode->appendChild(pDomElement, &pDomNode)))
	{
		if (lpszString && !CString(lpszString).IsEmpty())
			pDomNode->put_text(CComBSTR(lpszString));
		Result.m_pDomNode = pDomNode;
	}

	return Result;
}

//-----------------------------------------------------------------------------

void CXmlNode::RemoveChildNodes()
{
	IFC_ASSERT(!IsNull());

	CComPtr<IXMLDOMNodeList> pDomNodeList;
	CComPtr<IXMLDOMNode> pDomNode;
	DOMNodeType nNodeType;

	HRESULT hr = m_pDomNode->get_childNodes(&pDomNodeList);
	if (IS_S_OK(hr))
	{
		long nDomNodeCount = 0;
		pDomNodeList->get_length(&nDomNodeCount);

		for (int i = nDomNodeCount - 1; i >= 0; i--)
		{
			pDomNode = NULL;
			if (IS_S_OK(pDomNodeList->get_item(i, &pDomNode)) &&
				IS_S_OK(pDomNode->get_nodeType(&nNodeType)) &&
				nNodeType == NODE_ELEMENT)
			{
				m_pDomNode->removeChild(pDomNode, NULL);
			}
		}
	}
}

//-----------------------------------------------------------------------------

void CXmlNode::Remove()
{
	if (IsNull()) return;

	CComPtr<IXMLDOMNode> pDomParentNode;

	m_pDomNode->get_parentNode(&pDomParentNode);
	if (pDomParentNode != NULL)
		pDomParentNode->removeChild(m_pDomNode, NULL);

	m_pDomNode = NULL;
}

//-----------------------------------------------------------------------------

bool CXmlNode::SelectNodes(LPCTSTR lpszXPathExpr, CXmlNodeList& List)
{
	if (IsNull()) return false;

	bool bResult = false;
	CComPtr<IXMLDOMNodeList> pDomNodeList;

	HRESULT hr = m_pDomNode->selectNodes(CComBSTR(lpszXPathExpr), &pDomNodeList);
	if (IS_S_OK(hr) && pDomNodeList != NULL)
	{
		List.m_pDomNodeList = pDomNodeList;
		bResult = true;
	}

	return bResult;
}

//-----------------------------------------------------------------------------

CXmlNode CXmlNode::SelectNode(LPCTSTR lpszXPathExpr)
{
	CXmlNode Result;

	if (!IsNull())
	{
		CComPtr<IXMLDOMNode> pDomNode;
		HRESULT hr = m_pDomNode->selectSingleNode(CComBSTR(lpszXPathExpr), &pDomNode);
		if (IS_S_OK(hr) && pDomNode != NULL)
			Result.m_pDomNode = pDomNode;
	}

	return Result;
}

///////////////////////////////////////////////////////////////////////////////
/// CXmlNodeList

CXmlNodeList::CXmlNodeList()
{
	// nothing
}

CXmlNodeList::~CXmlNodeList()
{
	// nothing
}

//-----------------------------------------------------------------------------

int CXmlNodeList::GetCount() const
{
	if (IsNull()) return 0;

	long nCount = 0;
	HRESULT hr = m_pDomNodeList->get_length(&nCount);
	return (IS_S_OK(hr) ? nCount : 0);
}

//-----------------------------------------------------------------------------

CXmlNode CXmlNodeList::GetItem(int nIndex) const
{
	IFC_ASSERT(!IsNull());

	CXmlNode Result;
	CComPtr<IXMLDOMNode> pDomNode;
	HRESULT hr = m_pDomNodeList->get_item(nIndex, &pDomNode);
	if (IS_S_OK(hr))
		Result.m_pDomNode = pDomNode;

	return Result;
}

///////////////////////////////////////////////////////////////////////////////
/// CXmlValue

int CXmlValue::AsInteger(int nDefault) const
{
	return StrToInt(AsString(), nDefault);
}

//-----------------------------------------------------------------------------

INT64 CXmlValue::AsInt64(INT64 nDefault) const
{
	return StrToInt64(AsString(), nDefault);
}

//-----------------------------------------------------------------------------

UINT CXmlValue::AsUInteger(UINT nDefault) const
{
	return (UINT)AsInteger(nDefault);
}

//-----------------------------------------------------------------------------

UINT64 CXmlValue::AsUInt64(UINT64 nDefault) const
{
	return (UINT64)AsInt64(nDefault);
}

//-----------------------------------------------------------------------------

double CXmlValue::AsFloat(double fDefault) const
{
	return StrToFloat(AsString(), fDefault);
}

//-----------------------------------------------------------------------------

bool CXmlValue::AsBoolean(bool bDefault) const
{
	return StrToBool(AsString(), bDefault);
}

///////////////////////////////////////////////////////////////////////////////
/// CXmlNodeAttrs

CXmlNodeAttrs::CXmlNodeAttrs(CXmlNode& Node) :
	m_Node(Node)
{
	// nothing
}

CXmlNodeAttrs::~CXmlNodeAttrs()
{
	// nothing
}

//-----------------------------------------------------------------------------

bool CXmlNodeAttrs::Add(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	IFC_ASSERT(!m_Node.IsNull());

	CComPtr<IXMLDOMDocument> pDomDoc;
	CComPtr<IXMLDOMNode> pDomNode;
	CComPtr<IXMLDOMAttribute> pDomAttr;
	CComPtr<IXMLDOMNamedNodeMap> pDomNamedNodeMap;

	bool bResult =
		IS_S_OK(GetDomNode()->get_ownerDocument(&pDomDoc)) &&
		IS_S_OK(GetDomNode()->get_attributes(&pDomNamedNodeMap)) &&
		IS_S_OK(pDomDoc->createAttribute(CComBSTR(lpszName), &pDomAttr)) &&
		IS_S_OK(pDomAttr->put_nodeValue(CComVariant(lpszValue))) &&
		IS_S_OK(pDomNamedNodeMap->setNamedItem(pDomAttr, &pDomNode));

	return bResult;
}

//-----------------------------------------------------------------------------

bool CXmlNodeAttrs::Remove(LPCTSTR lpszName)
{
	IFC_ASSERT(!m_Node.IsNull());

	CComPtr<IXMLDOMNamedNodeMap> pDomNamedNodeMap;

	bool bResult =
		IS_S_OK(GetDomNode()->get_attributes(&pDomNamedNodeMap)) &&
		IS_S_OK(pDomNamedNodeMap->removeNamedItem(CComBSTR(lpszName), NULL));

	return bResult;
}

//-----------------------------------------------------------------------------

void CXmlNodeAttrs::Clear()
{
	IFC_ASSERT(!m_Node.IsNull());

	CComPtr<IXMLDOMNamedNodeMap> pDomNamedNodeMap;
	CComPtr<IXMLDOMNode> pDomNode;

	HRESULT hr = GetDomNode()->get_attributes(&pDomNamedNodeMap);
	if (IS_S_OK(hr))
	{
		while (true)
		{
			long nDomNodeCount = 0;
			pDomNamedNodeMap->get_length(&nDomNodeCount);
			if (nDomNodeCount == 0) break;

			CComBSTR strName;
			pDomNode = NULL;

			if (IS_S_OK(pDomNamedNodeMap->get_item(0, &pDomNode)) &&
				IS_S_OK(pDomNode->get_nodeName(&strName)))
			{
				pDomNamedNodeMap->removeNamedItem(strName, NULL);
			}
		}
	}
}

//-----------------------------------------------------------------------------

int CXmlNodeAttrs::GetCount() const
{
	IFC_ASSERT(!m_Node.IsNull());

	long nCount = 0;
	CComPtr<IXMLDOMNamedNodeMap> pDomNamedNodeMap;

	bool bSuccess =
		IS_S_OK(GetDomNode()->get_attributes(&pDomNamedNodeMap)) &&
		IS_S_OK(pDomNamedNodeMap->get_length(&nCount));

	return (bSuccess ? nCount : 0);
}

//-----------------------------------------------------------------------------

CXmlNodeAttrItem CXmlNodeAttrs::GetItem(int nIndex) const
{
	IFC_ASSERT(!m_Node.IsNull());

	CXmlNodeAttrItem Result;
	CComPtr<IXMLDOMNamedNodeMap> pDomNamedNodeMap;
	CComPtr<IXMLDOMNode> pDomNode;
	CComBSTR strName;
	CComVariant Value;

	bool bSuccess =
		IS_S_OK(GetDomNode()->get_attributes(&pDomNamedNodeMap)) &&
		IS_S_OK(pDomNamedNodeMap->get_item(nIndex, &pDomNode)) &&
		IS_S_OK(pDomNode->get_nodeName(&strName)) &&
		IS_S_OK(pDomNode->get_nodeValue(&Value));

	if (bSuccess)
	{
		Result.strName = strName;
		Result.strValue = Value;
	}

	return Result;
}

//-----------------------------------------------------------------------------

CXmlValue CXmlNodeAttrs::ValueOf(LPCTSTR lpszName) const
{
	IFC_ASSERT(!m_Node.IsNull());

	CString strResult;
	CComPtr<IXMLDOMNamedNodeMap> pDomNamedNodeMap;
	CComPtr<IXMLDOMNode> pDomNode;
	CComVariant Value;

	bool bSuccess =
		IS_S_OK(GetDomNode()->get_attributes(&pDomNamedNodeMap)) &&
		IS_S_OK(pDomNamedNodeMap->getNamedItem(CComBSTR(lpszName), &pDomNode)) &&
		IS_S_OK(pDomNode->get_nodeValue(&Value));

	if (bSuccess)
		strResult = Value;

	return CXmlValue(strResult);
}

//-----------------------------------------------------------------------------

bool CXmlNodeAttrs::Exists(LPCTSTR lpszName) const
{
	IFC_ASSERT(!m_Node.IsNull());

	CComPtr<IXMLDOMNamedNodeMap> pDomNamedNodeMap;
	CComPtr<IXMLDOMNode> pDomNode;

	bool bResult =
		IS_S_OK(GetDomNode()->get_attributes(&pDomNamedNodeMap)) &&
		IS_S_OK(pDomNamedNodeMap->getNamedItem(CComBSTR(lpszName), &pDomNode));

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
/// CXmlDocument

CXmlDocument::CXmlDocument()
{
	m_bComInited = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED));
	m_pDoc.CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_ALL);

	m_pReader = new CXmlDocReader(*this);
}

//-----------------------------------------------------------------------------

CXmlDocument::~CXmlDocument()
{
	SAFE_DELETE(m_pReader);
	m_pDoc = NULL;
	if (m_bComInited)
		CoUninitialize();
}

//-----------------------------------------------------------------------------

bool CXmlDocument::CreateRootNode(LPCTSTR lpszNodeName)
{
	if (IsNull()) return false;

	bool bResult = false;
	CComPtr<IXMLDOMElement> pDomElement;
	CComPtr<IXMLDOMNode> pDomNode;

	// delete the root node if it exists.
	HRESULT hr = m_pDoc->get_documentElement(&pDomElement);
	if (IS_S_OK(hr) && pDomElement != NULL)
	{
		pDomNode = NULL;
		pDomElement->get_parentNode(&pDomNode);
		if (pDomNode != NULL)
			pDomNode->removeChild(pDomElement, NULL);
	}

	// create the root node.
	pDomElement = NULL;
	m_pDoc->get_documentElement(&pDomElement);
	if (pDomElement == NULL)
	{
		HRESULT hr = m_pDoc->createElement(CComBSTR(lpszNodeName), &pDomElement);
		if (IS_S_OK(hr))
		{
			hr = m_pDoc->appendChild(pDomElement, NULL);
			bResult = IS_S_OK(hr);
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CXmlDocument::SaveToStream(CStream& Stream)
{
	bool bResult = false;
	try
	{
		CComPtr<IStream> pStream;

		if (IS_S_OK(::CreateStreamOnHGlobal(NULL, TRUE, &pStream)))
		{
			if (IS_S_OK(m_pDoc->save(CComVariant(pStream))))
			{
				const int BLOCK_SIZE = 1024*16;
				CBuffer Buffer(BLOCK_SIZE);

				LARGE_INTEGER nPos;
				nPos.QuadPart = 0;
				pStream->Seek(nPos, STREAM_SEEK_SET, NULL);

				while (true)
				{
					ULONG nBytesRead = 0;
					HRESULT hr = pStream->Read(Buffer.Data(), Buffer.GetSize(), &nBytesRead);
					if (IS_S_OK(hr))
						Stream.WriteBuffer(Buffer.Data(), nBytesRead);

					if (!IS_S_OK(hr) || (nBytesRead < (UINT)Buffer.GetSize()))
						break;
				}

				bResult = true;
			}
		}
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CXmlDocument::LoadFromStream(CStream& Stream)
{
	bool bResult = false;
	try
	{
		INT64 nSize64 = Stream.GetSize() - Stream.GetPosition();
		IFC_ASSERT(nSize64 <= MAXLONG);
		int nSize = (int)nSize64;

		CStringA s;
		LPSTR p = s.GetBufferSetLength(nSize);
		Stream.ReadBuffer(p, nSize);

		bResult = LoadFromString(s, s.GetLength());
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CXmlDocument::SaveToString(std::string& str)
{
	if (IsNull()) return false;

	bool bResult = false;
	try
	{
		CMemoryStream ms;
		bResult = SaveToStream(ms);
		if (bResult)
		{
			ms.SetPosition(0);
			str.assign(ms.GetMemory(), (int)ms.GetSize());
		}
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CXmlDocument::LoadFromString(const char *pStr, int nBytes)
{
	if (IsNull() || !pStr || nBytes <= 0) return false;

	bool bResult = false;
	try
	{
		Clear();

		if (pStr[nBytes-1] == 0)
			nBytes--;

		CComSafeArray<BYTE> SafeArray;
		SafeArray.Add(nBytes, (BYTE*)pStr, FALSE);

		CComVariant Variant(SafeArray);
		Variant.vt = (VT_ARRAY | VT_UI1);

		VARIANT_BOOL bSuccess = VARIANT_FALSE;
		HRESULT hr = m_pDoc->load(Variant, &bSuccess);
		bResult = (IS_S_OK(hr) && bSuccess && !GetRootNode().IsNull());
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CXmlDocument::SaveToBuffer(CBuffer& Buffer)
{
	if (IsNull()) return false;

	bool bResult = false;
	try
	{
		CMemoryStream ms;
		bResult = SaveToStream(ms);
		if (bResult)
		{
			ms.SetPosition(0);
			Buffer.Assign(ms.GetMemory(), (int)ms.GetSize());
		}
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CXmlDocument::LoadFromBuffer(const CBuffer& Buffer)
{
	return LoadFromString(Buffer.Data(), Buffer.GetSize());
}

//-----------------------------------------------------------------------------

bool CXmlDocument::SaveToFile(LPCTSTR lpszFileName)
{
	if (IsNull()) return false;

	bool bResult = false;
	try
	{
		HRESULT hr = m_pDoc->save(CComVariant(lpszFileName));
		bResult = IS_S_OK(hr);
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CXmlDocument::LoadFromFile(LPCTSTR lpszFileName)
{
	if (IsNull()) return false;

	if (!FileExists(lpszFileName))
		return false;

	bool bResult = false;
	try
	{
		Clear();
		VARIANT_BOOL bSuccess = VARIANT_FALSE;
		HRESULT hr = m_pDoc->load(CComVariant(lpszFileName), &bSuccess);
		bResult = (IS_S_OK(hr) && bSuccess && !GetRootNode().IsNull());
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}

	return bResult;
}

//-----------------------------------------------------------------------------

void CXmlDocument::Clear()
{
	if (IsNull()) return;

	CComPtr<IXMLDOMElement> pDomElement;
	CComPtr<IXMLDOMNode> pDomNode;

	// delete the child nodes including "Processing Instruction".
	while (true)
	{
		pDomNode = NULL;
		m_pDoc->get_firstChild(&pDomNode);
		if (pDomNode != NULL)
			m_pDoc->removeChild(pDomNode, NULL);
		else
			break;
	}

	// delete the root node.
	HRESULT hr = m_pDoc->get_documentElement(&pDomElement);
	if (IS_S_OK(hr) && pDomElement != NULL)
	{
		pDomNode = NULL;
		pDomElement->get_parentNode(&pDomNode);
		if (pDomNode != NULL)
			pDomNode->removeChild(pDomElement, NULL);
	}
}

//-----------------------------------------------------------------------------

bool CXmlDocument::CreateNew(LPCTSTR lpszEncoding, LPCTSTR lpszRootNodeName)
{
	if (IsNull()) return false;

	bool bResult = false;
	try
	{
		Clear();

		CString strPI;
		strPI.Format(TEXT("version=\"1.0\" encoding=\"%s\""), lpszEncoding);

		CComPtr<IXMLDOMProcessingInstruction> pPI;
		bResult =
			IS_S_OK(m_pDoc->createProcessingInstruction(CComBSTR(TEXT("xml")), CComBSTR(strPI), &pPI)) &&
			IS_S_OK(m_pDoc->appendChild(pPI, NULL));

		if (bResult)
		{
			if (lpszRootNodeName && !CString(lpszRootNodeName).IsEmpty())
				bResult = CreateRootNode(lpszRootNodeName);
		}
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}

	if (!bResult)
		Clear();

	return bResult;
}

//-----------------------------------------------------------------------------

CXmlNode CXmlDocument::GetRootNode()
{
	CXmlNode Result;

	if (m_pDoc != NULL)
	{
		CComPtr<IXMLDOMElement> pDomElement;
		m_pDoc->get_documentElement(&pDomElement);
		if (pDomElement != NULL)
			Result.m_pDomNode = pDomElement;
	}

	return Result;
}

///////////////////////////////////////////////////////////////////////////////
/// CXmlDocReader

void CXmlDocReader::SplitNamePath(LPCTSTR lpszNamePath, CStrList& List)
{
	const TCHAR NAME_PATH_SPLITTER = '.';
	SplitString(lpszNamePath, NAME_PATH_SPLITTER, List);
}

//-----------------------------------------------------------------------------

CXmlValue CXmlDocReader::GetValue(LPCTSTR lpszNamePath)
{
	CString strResult;
	CStrList NameList;
	CXmlNode Node, ResultNode;

	Node = m_XmlDoc.GetRootNode();

	SplitNamePath(lpszNamePath, NameList);
	if (NameList.GetCount() == 0)
		return Node.IsNull()? TEXT("") : Node.GetString();

	for (int i = 0; (i < NameList.GetCount() - 1) && !Node.IsNull(); i++)
		Node = Node.GetChildNode(NameList[i]);

	if (!Node.IsNull())
	{
		CString strLastName = NameList[NameList.GetCount() - 1];

		// The last name in the list can be a node name or an attribute name.
		ResultNode = Node.GetChildNode(strLastName);
		if (!ResultNode.IsNull())
			strResult = ResultNode.GetString();
		else if (Node.Attributes().Exists(strLastName))
			strResult = Node.Attributes().ValueOf(strLastName);
	}

	return CXmlValue(strResult);
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
