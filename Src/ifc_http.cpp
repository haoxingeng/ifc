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

/// @file ifc_http.cpp

#include "stdafx.h"
#include "ifc_http.h"
#include "ifc_errmsgs.h"
#include "ifc_sysutils.h"

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

CString GetHttpProtoVerStr(HTTP_PROTO_VER nVersion)
{
	switch (nVersion)
	{
	case HPV_1_0:  return TEXT("1.0");
	case HPV_1_1:  return TEXT("1.1");
	default:       return TEXT("");
	}
}

//-----------------------------------------------------------------------------

CString GetHttpMethodStr(HTTP_METHOD_TYPE nHttpMethod)
{
	switch (nHttpMethod)
	{
	case HMT_GET:   return TEXT("GET");
	case HMT_POST:  return TEXT("POST");
	default:        return TEXT("");
	}
}

///////////////////////////////////////////////////////////////////////////////
// CHttpHeaderStrList

CHttpHeaderStrList::CHttpHeaderStrList()
{
	m_strNameValueSep = TEXT(":");
}

//-----------------------------------------------------------------------------

CString CHttpHeaderStrList::MakeLine(LPCTSTR lpszName, LPCTSTR lpszValue) const
{
	return lpszName + m_strNameValueSep + TEXT(" ") + lpszValue;
}

//-----------------------------------------------------------------------------

int CHttpHeaderStrList::Add(LPCTSTR lpszStr)
{
	return m_Items.Add(lpszStr);
}

//-----------------------------------------------------------------------------

void CHttpHeaderStrList::Delete(int nIndex)
{
	m_Items.Delete(nIndex);
}

//-----------------------------------------------------------------------------

void CHttpHeaderStrList::Clear()
{
	m_Items.Clear();
}

//-----------------------------------------------------------------------------

void CHttpHeaderStrList::AddStrings(const CHttpHeaderStrList& Strings)
{
	for (int i = 0; i < Strings.GetCount(); i++)
		Add(Strings.GetString(i));
}

//-----------------------------------------------------------------------------

void CHttpHeaderStrList::MoveToTop(const CStrList& NameList)
{
	for (int i = NameList.GetCount() - 1; i >= 0; i--)
	{
		int nIndex = IndexOfName(NameList[i]);
		if (nIndex >= 0)
			m_Items.Move(nIndex, 0);
	}
}

//-----------------------------------------------------------------------------

int CHttpHeaderStrList::IndexOfName(LPCTSTR lpszName) const
{
	int nResult = -1;

	for (int i = 0; i < m_Items.GetCount(); i++)
		if (SameText(GetName(i), lpszName))
		{
			nResult = i;
			break;
		}

	return nResult;
}

//-----------------------------------------------------------------------------

CString CHttpHeaderStrList::GetText() const
{
	CString strResult;
	for (int i = 0; i < m_Items.GetCount(); i++)
	{
		CString strName = GetName(i);
		CString strValue = GetValue(i);
		if (!strName.IsEmpty() && !strValue.IsEmpty())
		{
			strResult += MakeLine(strName, strValue);
			strResult += TEXT("\r\n");
		}
	}
	return strResult;
}

//-----------------------------------------------------------------------------

CString CHttpHeaderStrList::GetName(int nIndex) const
{
	CString strResult;
	CString strLine = m_Items[nIndex];
	int nPos = strLine.Find(m_strNameValueSep);
	if (nPos > 0)
		strResult = strLine.Mid(0, nPos).Trim();
	return strResult;
}

//-----------------------------------------------------------------------------

CString CHttpHeaderStrList::GetValue(int nIndex) const
{
	CString strResult;
	CString strLine = m_Items[nIndex];
	int nPos = strLine.Find(m_strNameValueSep);
	if (nPos > 0)
		strResult = strLine.Mid(nPos + 1).Trim();
	return strResult;
}

//-----------------------------------------------------------------------------

CString CHttpHeaderStrList::GetValue(LPCTSTR lpszName) const
{
	CString strResult;
	int nIndex = IndexOfName(lpszName);
	if (nIndex >= 0)
		strResult = GetValue(nIndex);
	return strResult;
}

//-----------------------------------------------------------------------------

void CHttpHeaderStrList::SetValue(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CString strName(lpszName);
	CString strValue(lpszValue);

	strName.Trim();
	strValue.Trim();

	int nIndex = IndexOfName(strName);
	if (strValue.IsEmpty())
	{
		if (nIndex >= 0) Delete(nIndex);
	}
	else
	{
		if (nIndex < 0)
			nIndex = Add(TEXT(""));
		m_Items.SetString(nIndex, MakeLine(strName, strValue));
	}
}

///////////////////////////////////////////////////////////////////////////////
// CHttpEntityHeaderInfo

void CHttpEntityHeaderInfo::Init()
{
	m_RawHeaders.Clear();
	m_CustomHeaders.Clear();
	m_strCacheControl = TEXT("no-cache");
	m_strConnection = TEXT("close");
	m_strContentDisposition.Empty();
	m_strContentEncoding.Empty();
	m_strContentLanguage.Empty();
	m_nContentLength = -1;
	m_nContentRangeStart = 0;
	m_nContentRangeEnd = 0;
	m_nContentRangeInstanceLength = 0;
	m_strContentType.Empty();
	m_strContentVersion.Empty();
	m_strDate.Empty();
	m_strExpires.Empty();
	m_strETag.Empty();
	m_strLastModified.Empty();
	m_strPragma = TEXT("no-cache");
	m_strTransferEncoding.Empty();
}

//-----------------------------------------------------------------------------

void CHttpEntityHeaderInfo::Clear()
{
	Init();
}

//-----------------------------------------------------------------------------

void CHttpEntityHeaderInfo::ParseHeaders()
{
	m_strCacheControl = m_RawHeaders.GetValue(TEXT("Cache-control"));
	m_strConnection = m_RawHeaders.GetValue(TEXT("Connection"));
	m_strContentVersion = m_RawHeaders.GetValue(TEXT("Content-Version"));
	m_strContentDisposition = m_RawHeaders.GetValue(TEXT("Content-Disposition"));
	m_strContentEncoding = m_RawHeaders.GetValue(TEXT("Content-Encoding"));
	m_strContentLanguage = m_RawHeaders.GetValue(TEXT("Content-Language"));
	m_strContentType = m_RawHeaders.GetValue(TEXT("Content-Type"));
	m_nContentLength = StrToInt64(m_RawHeaders.GetValue(TEXT("Content-Length")), -1);

	m_nContentRangeStart = 0;
	m_nContentRangeEnd = 0;
	m_nContentRangeInstanceLength = 0;

	/* Content-Range Examples: */
	// content-range: bytes 1-65536/102400
	// content-range: bytes */102400
	// content-range: bytes 1-65536/*

	CString s = m_RawHeaders.GetValue(TEXT("Content-Range"));
	if (!s.IsEmpty())
	{
		FetchStr(s);
		CString strRange = FetchStr(s, '/');
		CString strLength = FetchStr(s);

		m_nContentRangeStart = StrToInt64(FetchStr(strRange, '-'), 0);
		m_nContentRangeEnd = StrToInt64(strRange, 0);
		m_nContentRangeInstanceLength = StrToInt64(strLength, 0);
	}

	m_strDate = m_RawHeaders.GetValue(TEXT("Date"));
	m_strLastModified = m_RawHeaders.GetValue(TEXT("Last-Modified"));
	m_strExpires = m_RawHeaders.GetValue(TEXT("Expires"));
	m_strETag = m_RawHeaders.GetValue(TEXT("ETag"));
	m_strPragma = m_RawHeaders.GetValue(TEXT("Pragma"));
	m_strTransferEncoding = m_RawHeaders.GetValue(TEXT("Transfer-Encoding"));
}

//-----------------------------------------------------------------------------

void CHttpEntityHeaderInfo::BuildHeaders()
{
	m_RawHeaders.Clear();

	if (!m_strConnection.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Connection"), m_strConnection);
	if (!m_strContentVersion.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Content-Version"), m_strContentVersion);
	if (!m_strContentDisposition.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Content-Disposition"), m_strContentDisposition);
	if (!m_strContentEncoding.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Content-Encoding"), m_strContentEncoding);
	if (!m_strContentLanguage.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Content-Language"), m_strContentLanguage);
	if (!m_strContentType.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Content-Type"), m_strContentType);
	if (m_nContentLength >= 0)
		m_RawHeaders.SetValue(TEXT("Content-Length"), IntToStr(m_nContentLength));
	if (!m_strCacheControl.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Cache-control"), m_strCacheControl);
	if (!m_strDate.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Date"), m_strDate);
	if (!m_strETag.IsEmpty())
		m_RawHeaders.SetValue(TEXT("ETag"), m_strETag);
	if (!m_strExpires.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Expires"), m_strExpires);
	if (!m_strPragma.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Pragma"), m_strPragma);
	if (!m_strTransferEncoding.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Transfer-Encoding"), m_strTransferEncoding);

	if (m_CustomHeaders.GetCount() > 0)
		m_RawHeaders.AddStrings(m_CustomHeaders);
}

///////////////////////////////////////////////////////////////////////////////
// CHttpRequestHeaderInfo

void CHttpRequestHeaderInfo::Init()
{
	m_strAccept = TEXT("text/html, */*");
	m_strAcceptCharSet.Empty();
	m_strAcceptEncoding.Empty();
	m_strAcceptLanguage.Empty();
	m_strFrom.Empty();
	m_strReferer.Empty();
	m_strUserAgent = IFC_DEFAULT_USER_AGENT;
	m_strHost.Empty();
	m_strRange.Empty();
}

//-----------------------------------------------------------------------------

void CHttpRequestHeaderInfo::Clear()
{
	CHttpEntityHeaderInfo::Clear();
	Init();
}

//-----------------------------------------------------------------------------

void CHttpRequestHeaderInfo::ParseHeaders()
{
	CHttpEntityHeaderInfo::ParseHeaders();

	m_strAccept = m_RawHeaders.GetValue(TEXT("Accept"));
	m_strAcceptCharSet = m_RawHeaders.GetValue(TEXT("Accept-Charset"));
	m_strAcceptEncoding = m_RawHeaders.GetValue(TEXT("Accept-Encoding"));
	m_strAcceptLanguage = m_RawHeaders.GetValue(TEXT("Accept-Language"));
	m_strHost = m_RawHeaders.GetValue(TEXT("Host"));
	m_strFrom = m_RawHeaders.GetValue(TEXT("From"));
	m_strReferer = m_RawHeaders.GetValue(TEXT("Referer"));
	m_strUserAgent = m_RawHeaders.GetValue(TEXT("User-Agent"));

	// strip off the 'bytes=' portion of the header
	CString s = m_RawHeaders.GetValue(TEXT("Range"));
	FetchStr(s, '=');
	m_strRange = s;
}

//-----------------------------------------------------------------------------

void CHttpRequestHeaderInfo::BuildHeaders()
{
	CHttpEntityHeaderInfo::BuildHeaders();

	if (!m_strHost.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Host"), m_strHost);
	if (!m_strAccept.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Accept"), m_strAccept);
	if (!m_strAcceptCharSet.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Accept-Charset"), m_strAcceptCharSet);
	if (!m_strAcceptEncoding.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Accept-Encoding"), m_strAcceptEncoding);
	if (!m_strAcceptLanguage.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Accept-Language"), m_strAcceptLanguage);
	if (!m_strFrom.IsEmpty())
		m_RawHeaders.SetValue(TEXT("From"), m_strFrom);
	if (!m_strReferer.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Referer"), m_strReferer);
	if (!m_strUserAgent.IsEmpty())
		m_RawHeaders.SetValue(TEXT("User-Agent"), m_strUserAgent);
	if (!m_strRange.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Range"), TEXT("bytes=") + m_strRange);
	if (!m_strLastModified.IsEmpty())
		m_RawHeaders.SetValue(TEXT("If-Modified-Since"), m_strLastModified);

	// Sort the list
	CStrList NameList;
	NameList.Add(TEXT("Host"));
	NameList.Add(TEXT("Accept"));
	NameList.Add(TEXT("Accept-Charset"));
	NameList.Add(TEXT("Accept-Encoding"));
	NameList.Add(TEXT("Accept-Language"));
	NameList.Add(TEXT("From"));
	NameList.Add(TEXT("Referer"));
	NameList.Add(TEXT("User-Agent"));
	NameList.Add(TEXT("Range"));
	NameList.Add(TEXT("Connection"));
	m_RawHeaders.MoveToTop(NameList);
}

//-----------------------------------------------------------------------------

void CHttpRequestHeaderInfo::SetRange(INT64 nRangeStart, INT64 nRangeEnd)
{
	CString strRange = FormatString(TEXT("%I64d-"), nRangeStart);
	if (nRangeEnd >= 0)
		strRange += FormatString(TEXT("%I64d"), nRangeEnd);
	SetRange(strRange);
}

///////////////////////////////////////////////////////////////////////////////
// CHttpResponseHeaderInfo

void CHttpResponseHeaderInfo::Init()
{
	m_strAcceptRanges.Empty();
	m_strLocation.Empty();
	m_strServer.Empty();

	m_strContentType = TEXT("text/html");
}

//-----------------------------------------------------------------------------

void CHttpResponseHeaderInfo::Clear()
{
	CHttpEntityHeaderInfo::Clear();
	Init();
}

//-----------------------------------------------------------------------------

void CHttpResponseHeaderInfo::ParseHeaders()
{
	CHttpEntityHeaderInfo::ParseHeaders();

	m_strAcceptRanges = m_RawHeaders.GetValue(TEXT("Accept-Ranges"));
	m_strLocation = m_RawHeaders.GetValue(TEXT("Location"));
	m_strServer = m_RawHeaders.GetValue(TEXT("Server"));
}

//-----------------------------------------------------------------------------

void CHttpResponseHeaderInfo::BuildHeaders()
{
	CHttpEntityHeaderInfo::BuildHeaders();

	if (HasContentRange() || HasContentRangeInstance())
	{
		CString strCR = (HasContentRange() ?
			FormatString(TEXT("%I64d-%I64d"), m_nContentRangeStart, m_nContentRangeEnd) : TEXT("*"));
		CString strCI = (HasContentRangeInstance() ?
			IntToStr(m_nContentRangeInstanceLength) : TEXT("*"));

		m_RawHeaders.SetValue(TEXT("Content-Range"), TEXT("bytes ") + strCR + TEXT("/") + strCI);
	}

	if (!m_strAcceptRanges.IsEmpty())
		m_RawHeaders.SetValue(TEXT("Accept-Ranges"), m_strAcceptRanges);
}

///////////////////////////////////////////////////////////////////////////////
// CHttpRequest

CHttpRequest::CHttpRequest(CCustomHttpClient& HttpClient) :
	m_HttpClient(HttpClient)
{
	Init();
}

//-----------------------------------------------------------------------------

void CHttpRequest::Init()
{
	m_nProtocolVersion = HPV_1_1;
	m_strUrl.Empty();
	m_strMethod.Empty();
	m_pContentStream = NULL;
}

//-----------------------------------------------------------------------------

void CHttpRequest::Clear()
{
	CHttpRequestHeaderInfo::Clear();
	Init();
}

///////////////////////////////////////////////////////////////////////////////
// CHttpResponse

CHttpResponse::CHttpResponse(CCustomHttpClient& HttpClient) :
	m_HttpClient(HttpClient)
{
	Init();
}

//-----------------------------------------------------------------------------

void CHttpResponse::Init()
{
	m_strResponseText.Empty();
	m_pContentStream = NULL;
}

//-----------------------------------------------------------------------------

void CHttpResponse::Clear()
{
	CHttpResponseHeaderInfo::Clear();
	Init();
}

//-----------------------------------------------------------------------------

bool CHttpResponse::GetKeepAlive()
{
	bool bResult = m_HttpClient.m_TcpClient.IsConnected();

	if (bResult)
	{
		CString str = GetConnection();
		bResult = !str.IsEmpty() && !SameText(str, TEXT("close"));
	}

	return bResult;
}

//-----------------------------------------------------------------------------

int CHttpResponse::GetResponseCode() const
{
	CString s = m_strResponseText;
	FetchStr(s);
	s = s.Trim();
	s = FetchStr(s);
	s = FetchStr(s, '.');

	return StrToInt(s, -1);
}

//-----------------------------------------------------------------------------

HTTP_PROTO_VER CHttpResponse::GetResponseVersion() const
{
	// eg: HTTP/1.1 200 OK
	CString s = m_strResponseText.Mid(5, 3);

	if (SameText(s, GetHttpProtoVerStr(HPV_1_0)))
		return HPV_1_0;
	else if (SameText(s, GetHttpProtoVerStr(HPV_1_1)))
		return HPV_1_1;
	else
		return HPV_1_0;
}

///////////////////////////////////////////////////////////////////////////////
// CCustomHttpClient

CCustomHttpClient::CCustomHttpClient() :
	m_Request(*this),
	m_Response(*this),
	m_bHandleRedirects(true),
	m_nRedirectCount(0),
	m_bLastKeepAlive(false)
{
	EnsureNetworkInited();
}

CCustomHttpClient::~CCustomHttpClient()
{
	// nothing
}

//-----------------------------------------------------------------------------

CPeerAddress CCustomHttpClient::GetPeerAddrFromUrl(CUrl& Url)
{
	CPeerAddress PeerAddr(0, 0);

	if (!Url.GetHost().IsEmpty())
	{
		CString strIp = LookupHostAddr(Url.GetHost());
		if (!strIp.IsEmpty())
		{
			PeerAddr.nIp = StringToIp(strIp);
			PeerAddr.nPort = StrToInt(Url.GetPort(), DEFAULT_HTTP_PORT);
		}
	}

	return PeerAddr;
}

//-----------------------------------------------------------------------------

void CCustomHttpClient::MakeRequestBuffer(CBuffer& Buffer)
{
	m_Request.BuildHeaders();

	CString strText;
	strText = m_Request.GetMethod() + TEXT(" ") + m_Request.GetUrl() +
		TEXT(" HTTP/") + GetHttpProtoVerStr(m_Request.GetProtocolVersion()) +
		TEXT("\r\n");

	for (int i = 0; i < m_Request.GetRawHeaders().GetCount(); i++)
	{
		CString s = m_Request.GetRawHeaders()[i];
		if (!s.IsEmpty())
			strText = strText + s + TEXT("\r\n");
	}

	strText += TEXT("\r\n");

	CStringA strBuffer(strText);
	Buffer.Assign(strBuffer.GetBuffer(), strBuffer.GetLength());
}

//-----------------------------------------------------------------------------

int CCustomHttpClient::BeforeRequest(HTTP_METHOD_TYPE nHttpMethod, LPCTSTR lpszUrl,
	CStream *pRequestContent, CStream *pResponseContent,
	INT64 nReqStreamPos, INT64 nResStreamPos)
{
	IFC_ASSERT(nHttpMethod == HMT_GET || nHttpMethod == HMT_POST);

	if (nHttpMethod == HMT_POST)
	{
		m_TcpClient.Disconnect();

		// Currently when issuing a POST, IFC HTTP will automatically set the protocol
		// to version 1.0 independently of the value it had initially. This is because
		// there are some servers that don't respect the RFC to the full extent. In
		// particular, they don't respect sending/not sending the Expect: 100-Continue
		// header. Until we find an optimum solution that does NOT break the RFC, we
		// will restrict POSTS to version 1.0.
		m_Request.SetProtocolVersion(HPV_1_0);

		// Usual posting request have default ContentType is "application/x-www-form-urlencoded"
		if (m_Request.GetContentType().IsEmpty() ||
			SameText(m_Request.GetContentType(), TEXT("text/html")))
		{
			m_Request.SetContentType(TEXT("application/x-www-form-urlencoded"));
		}
	}

	CUrl Url(lpszUrl);
	if (m_nRedirectCount == 0)
	{
		if (Url.GetHost().IsEmpty())
			return EC_HTTP_URL_ERROR;
		m_Url = Url;
	}
	else
	{
		if (Url.GetHost().IsEmpty())
		{
			CUrl OldUrl = m_Url;
			m_Url = Url;
			m_Url.SetProtocol(OldUrl.GetProtocol());
			m_Url.SetHost(OldUrl.GetHost());
			m_Url.SetPort(OldUrl.GetPort());
		}
		else
			m_Url = Url;
	}

	if (pRequestContent)
		pRequestContent->SetPosition(nReqStreamPos);
	if (pResponseContent)
	{
		pResponseContent->SetSize(nResStreamPos);
		pResponseContent->SetPosition(nResStreamPos);
	}

	m_Request.SetMethod(GetHttpMethodStr(nHttpMethod));
	m_Request.SetUrl(m_Url.GetUrl(CUrl::URL_PATH | CUrl::URL_FILENAME | CUrl::URL_PARAMS));

	//if (m_Request.GetReferer().IsEmpty())
	//	m_Request.SetReferer(m_Url.GetUrl(CUrl::URL_ALL & ~(CUrl::URL_FILENAME | CUrl::URL_BOOKMARK | CUrl::URL_PARAMS)));

	int nPort = StrToInt(Url.GetPort(), DEFAULT_HTTP_PORT);
	m_Request.SetHost(nPort == DEFAULT_HTTP_PORT ?
		m_Url.GetHost() : m_Url.GetHost() + TEXT(":") + IntToStr(nPort));

	m_Request.SetContentLength(pRequestContent?
		pRequestContent->GetSize() - pRequestContent->GetPosition() : -1);
	m_Request.SetContentStream(pRequestContent);

	m_Response.Clear();
	m_Response.SetContentStream(pResponseContent);

	return EC_HTTP_SUCCESS;
}

//-----------------------------------------------------------------------------

void CCustomHttpClient::CheckResponseHeader(char *pBuffer, int nSize, bool& bFinished, bool& bError)
{
	bFinished = bError = false;

	if (nSize >= 4)
	{
		char *p = pBuffer + nSize - 4;
		if (p[0] == '\r' && p[1] == '\n' && p[2] == '\r' && p[3] == '\n')
			bFinished = true;

		if (_strnicmp(pBuffer, "HTTP", 4) != 0)
			bError = true;
	}
}

//-----------------------------------------------------------------------------

bool CCustomHttpClient::ParseResponseHeader(void *pBuffer, int nSize)
{
	bool bResult = true;
	CMemoryStream Stream;
	Stream.Write(pBuffer, nSize);

	CStrList Lines;
	Lines.SetLineBreak(TEXT("\r\n"));
	Stream.SetPosition(0);
	Lines.LoadFromStream(Stream, false);

	if (Lines.GetCount() > 0)
	{
		CString str = Lines[0];
		if (str.Mid(0, 7).CompareNoCase(TEXT("HTTP/1.")) == 0)
			m_Response.SetResponseText(str);
		else
			bResult = false;
	}
	else
		bResult = false;

	if (bResult)
	{
		m_Response.GetRawHeaders().Clear();
		for (int i = 1; i < Lines.GetCount(); i++)
		{
			if (!Lines[i].IsEmpty())
				m_Response.GetRawHeaders().Add(Lines[i]);
		}
		m_Response.ParseHeaders();
	}

	return bResult;
}

//-----------------------------------------------------------------------------

HTTP_NEXT_OP CCustomHttpClient::ProcessResponseHeader()
{
	HTTP_NEXT_OP nResult = HNO_EXIT;

	int nResponseCode = m_Response.GetResponseCode();
	int nResponseDigit = nResponseCode / 100;

	m_bLastKeepAlive = m_Response.GetKeepAlive();

	// Handle Redirects
	if ((nResponseDigit == 3 && nResponseCode != 304) ||
		(!m_Response.GetLocation().IsEmpty() && nResponseCode != 201))
	{
		m_nRedirectCount++;
		if (m_bHandleRedirects)
			nResult = HNO_REDIRECT;
	}
	else if (nResponseDigit == 2)
	{
		nResult = HNO_RECV_CONTENT;
	}

	return nResult;
}

//-----------------------------------------------------------------------------

void CCustomHttpClient::TcpDisconnect(bool bForce)
{
	if (!m_bLastKeepAlive || bForce)
		m_TcpClient.Disconnect();
}

///////////////////////////////////////////////////////////////////////////////
// CHttpClient

CHttpClient::CHttpClient()
{
	// nothing
}

CHttpClient::~CHttpClient()
{
	// nothing
}

//-----------------------------------------------------------------------------

int CHttpClient::ReadLine(CString& strLine, int nTimeOut)
{
	int nResult = EC_HTTP_SUCCESS;
	UINT nStartTicks = GetTickCount();
	CMemoryStream Stream;
	char ch;

	while (true)
	{
		int nRemainTimeOut = Max(0, nTimeOut - (int)GetTickDiff(nStartTicks, GetTickCount()));

		int r = m_TcpClient.RecvBuffer(&ch, sizeof(ch), true, nRemainTimeOut);
		if (r < 0)
		{
			nResult = EC_HTTP_SOCKET_ERROR;
			break;
		}
		else if (r > 0)
		{
			Stream.Write(&ch, sizeof(ch));

			char *pBuffer = Stream.GetMemory();
			int nSize = (int)Stream.GetSize();
			bool bFinished = false;

			if (nSize >= 2)
			{
				char *p = pBuffer + nSize - 2;
				if (p[0] == '\r' && p[1] == '\n')
					bFinished = true;
			}

			if (bFinished)
				break;
		}

		if (GetTickDiff(nStartTicks, GetTickCount()) > (UINT)nTimeOut)
		{
			nResult = EC_HTTP_RECV_TIMEOUT;
			break;
		}
	}

	if (nResult == EC_HTTP_SUCCESS)
	{
		std::string s;
		if (Stream.GetSize() > 0)
			s.assign(Stream.GetMemory(), (int)Stream.GetSize());
		strLine = CA2T(s.c_str());
	}

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::ReadChunkSize(UINT& nChunkSize, int nTimeOut)
{
	CString strLine;
	int nResult = ReadLine(strLine, nTimeOut);
	if (nResult == EC_HTTP_SUCCESS)
	{
		int i = strLine.Find(';');
		if (i >= 0)
			strLine = strLine.Left(i);
		nChunkSize = (UINT)_tcstol(strLine, NULL, 16);
	}

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::ReadStream(CStream& Stream, int nBytes, int nTimeOut)
{
	const int BLOCK_SIZE = 1024*64;

	int nResult = EC_HTTP_SUCCESS;
	UINT nStartTicks = GetTickCount();
	INT64 nRemainSize = nBytes;
	CBuffer Buffer(BLOCK_SIZE);

	while (nRemainSize > 0)
	{
		int nBlockSize = (int)Min(nRemainSize, (INT64)BLOCK_SIZE);
		int nRemainTimeOut = Max(0, nTimeOut - (int)GetTickDiff(nStartTicks, GetTickCount()));
		int nRecvSize = m_TcpClient.RecvBuffer(Buffer.Data(), nBlockSize, true, nRemainTimeOut);

		if (nRecvSize < 0)
		{
			nResult = EC_HTTP_SOCKET_ERROR;
			break;
		}

		nRemainSize -= nRecvSize;
		Stream.WriteBuffer(Buffer.Data(), nRecvSize);

		if (GetTickDiff(nStartTicks, GetTickCount()) > (UINT)nTimeOut)
		{
			nResult = EC_HTTP_RECV_TIMEOUT;
			break;
		}
	}

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::ExecuteHttpAction(HTTP_METHOD_TYPE nHttpMethod, LPCTSTR lpszUrl,
	CStream *pRequestContent, CStream *pResponseContent)
{
	class CAutoFinalizer
	{
	private:
		CHttpClient& m_Owner;
	public:
		CAutoFinalizer(CHttpClient& Owner) : m_Owner(Owner) {}
		~CAutoFinalizer() { m_Owner.TcpDisconnect(); }
	} AutoFinalizer(*this);

	bool bNeedRecvContent = (pResponseContent != NULL);
	bool bCanRecvContent = false;

	int nResult = ExecuteHttpRequest(nHttpMethod, lpszUrl, pRequestContent,
		pResponseContent, bNeedRecvContent, bCanRecvContent);

	if (nResult == EC_HTTP_SUCCESS && bNeedRecvContent)
		nResult = RecvResponseContent();

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::ExecuteHttpRequest(HTTP_METHOD_TYPE nHttpMethod, LPCTSTR lpszUrl,
	CStream *pRequestContent, CStream *pResponseContent, bool bNeedRecvContent,
	bool& bCanRecvContent)
{
	int nResult = EC_HTTP_SUCCESS;

	CString strUrl(lpszUrl);
	INT64 nReqStreamPos = (pRequestContent? pRequestContent->GetPosition() : 0);
	INT64 nResStreamPos = (pResponseContent? pResponseContent->GetPosition() : 0);

	bCanRecvContent = false;
	m_nRedirectCount = 0;

	while (true)
	{
		nResult = BeforeRequest(nHttpMethod, strUrl, pRequestContent, pResponseContent,
			nReqStreamPos, nResStreamPos);

		if (nResult == EC_HTTP_SUCCESS) nResult = TcpConnect();
		if (nResult == EC_HTTP_SUCCESS) nResult = SendRequestHeader();
		if (nResult == EC_HTTP_SUCCESS) nResult = SendRequestContent();
		if (nResult == EC_HTTP_SUCCESS) nResult = RecvResponseHeader();

		if (nResult == EC_HTTP_SUCCESS)
		{
			HTTP_NEXT_OP nNextOp = ProcessResponseHeader();

			if (nNextOp == HNO_REDIRECT)
			{
				strUrl = m_Response.GetLocation();
			}
			else if (nNextOp == HNO_RECV_CONTENT)
			{
				bCanRecvContent = true;
				break;
			}
			else
				break;
		}
		else
			break;
	}

	if (nResult == EC_HTTP_SUCCESS && bNeedRecvContent && !bCanRecvContent)
		nResult = EC_HTTP_CANNOT_RECV_CONTENT;

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::TcpConnect()
{
	if (!m_bLastKeepAlive)
		m_TcpClient.Disconnect();

	if (m_TcpClient.IsConnected())
	{
		CPeerAddress PeerAddr = GetPeerAddrFromUrl(m_Url);
		if (m_TcpClient.GetPeerAddr() != PeerAddr)
			m_TcpClient.Disconnect();
	}

	if (!m_TcpClient.IsConnected())
	{
		CPeerAddress PeerAddr = GetPeerAddrFromUrl(m_Url);
		int nState = m_TcpClient.AsyncConnect(IpToString(PeerAddr.nIp),
			PeerAddr.nPort, m_Options.nTcpConnectTimeOut);

		if (nState != ACS_CONNECTED)
			return EC_HTTP_SOCKET_ERROR;
	}

	if (m_TcpClient.IsConnected())
	{
		SOCKET hHandle = m_TcpClient.GetSocket().GetHandle();
		int nTimeOut = m_Options.nSocketOpTimeOut;
		::setsockopt(hHandle, SOL_SOCKET, SO_SNDTIMEO, (char*)&nTimeOut, sizeof(nTimeOut));
		::setsockopt(hHandle, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeOut, sizeof(nTimeOut));
	}

	return EC_HTTP_SUCCESS;
}

//-----------------------------------------------------------------------------

int CHttpClient::SendRequestHeader()
{
	int nResult = EC_HTTP_SUCCESS;
	CBuffer Buffer;
	MakeRequestBuffer(Buffer);

	int r = m_TcpClient.SendBuffer(Buffer.Data(), Buffer.GetSize(), true, m_Options.nSendReqHeaderTimeOut);
	if (r != Buffer.GetSize())
		nResult = EC_HTTP_SOCKET_ERROR;

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::SendRequestContent()
{
	int nResult = EC_HTTP_SUCCESS;

	CStream *pStream = m_Request.GetContentStream();
	if (!pStream) return nResult;

	const int BLOCK_SIZE = 1024*64;

	CBuffer Buffer(BLOCK_SIZE);

	while (true)
	{
		int nReadSize = pStream->Read(Buffer.Data(), BLOCK_SIZE);
		if (nReadSize > 0)
		{
			int r = m_TcpClient.SendBuffer(Buffer.Data(), nReadSize, true, m_Options.nSendReqContBlockTimeOut);
			if (r < 0)
			{
				nResult = EC_HTTP_SOCKET_ERROR;
				break;
			}
			else if (r != nReadSize)
			{
				nResult = EC_HTTP_SEND_TIMEOUT;
				break;
			}
		}

		if (nReadSize < BLOCK_SIZE) break;
	}

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::RecvResponseHeader()
{
	const int RECV_TIMEOUT = m_Options.nRecvResHeaderTimeOut;

	int nResult = EC_HTTP_SUCCESS;
	UINT nStartTicks = GetTickCount();

	do
	{
		CMemoryStream Stream;
		char ch;

		while (true)
		{
			int nRemainTimeOut = Max(0, RECV_TIMEOUT - (int)GetTickDiff(nStartTicks, GetTickCount()));

			int r = m_TcpClient.RecvBuffer(&ch, sizeof(ch), true, nRemainTimeOut);
			if (r < 0)
			{
				nResult = EC_HTTP_SOCKET_ERROR;
				break;
			}
			else if (r > 0)
			{
				Stream.Write(&ch, sizeof(ch));

				bool bFinished, bError;
				CheckResponseHeader(Stream.GetMemory(), (int)Stream.GetSize(), bFinished, bError);
				if (bError)
				{
					nResult = EC_HTTP_RESPONSE_TEXT_ERROR;
					break;
				}
				if (bFinished)
					break;
			}

			if (GetTickDiff(nStartTicks, GetTickCount()) > (UINT)RECV_TIMEOUT)
			{
				nResult = EC_HTTP_RECV_TIMEOUT;
				break;
			}
		}

		if (nResult == EC_HTTP_SUCCESS)
		{
			if (!ParseResponseHeader(Stream.GetMemory(), (int)Stream.GetSize()))
				nResult = EC_HTTP_RESPONSE_TEXT_ERROR;
		}
	}
	while (m_Response.GetResponseCode() == 100 && nResult == EC_HTTP_SUCCESS);

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::RecvResponseContent()
{
	int nResult = EC_HTTP_SUCCESS;
	if (!m_Response.GetContentStream()) return nResult;

	CString strTransferEncoding = m_Response.GetTransferEncoding();
	if (strTransferEncoding.MakeLower().Find(TEXT("chunked")) >= 0)
	{
		while (true)
		{
			int nTimeOut = m_Options.nRecvResContBlockTimeOut;
			UINT nChunkSize = 0;
			nResult = ReadChunkSize(nChunkSize, nTimeOut);
			if (nResult != EC_HTTP_SUCCESS)
				break;

			if (nChunkSize != 0)
			{
				nResult = ReadStream(*m_Response.GetContentStream(), nChunkSize, nTimeOut);
				if (nResult != EC_HTTP_SUCCESS)
					break;

				CString strCrLf;
				nResult = ReadLine(strCrLf, nTimeOut);
				if (nResult != EC_HTTP_SUCCESS)
					break;
			}
			else
				break;
		}
	}
	else
	{
		INT64 nContentLength = m_Response.GetContentLength();
		if (nContentLength <= 0)
			nResult = EC_HTTP_CONTENT_LENGTH_ERROR;

		if (nResult == EC_HTTP_SUCCESS)
		{
			const int BLOCK_SIZE = 1024*64;

			INT64 nRemainSize = nContentLength;
			CBuffer Buffer(BLOCK_SIZE);

			while (nRemainSize > 0)
			{
				int nBlockSize = (int)Min(nRemainSize, (INT64)BLOCK_SIZE);
				int nRecvSize = m_TcpClient.RecvBuffer(Buffer.Data(), nBlockSize,
					true, m_Options.nRecvResContBlockTimeOut);

				if (nRecvSize < 0)
				{
					nResult = EC_HTTP_SOCKET_ERROR;
					break;
				}

				nRemainSize -= nRecvSize;
				m_Response.GetContentStream()->WriteBuffer(Buffer.Data(), nRecvSize);
			}
		}
	}

	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::Get(LPCTSTR lpszUrl, CStream *pResponseContent)
{
	return ExecuteHttpAction(HMT_GET, lpszUrl, NULL, pResponseContent);
}

//-----------------------------------------------------------------------------

int CHttpClient::Post(LPCTSTR lpszUrl, CStream *pRequestContent, CStream *pResponseContent)
{
	IFC_ASSERT(pRequestContent != NULL);
	return ExecuteHttpAction(HMT_POST, lpszUrl, pRequestContent, pResponseContent);
}

//-----------------------------------------------------------------------------

int CHttpClient::DownloadFile(LPCTSTR lpszUrl, LPCTSTR lpszLocalFileName)
{
	ForceDirectories(ExtractFilePath(lpszLocalFileName));
	CFileStream fs;
	if (!fs.Open(lpszLocalFileName, FM_CREATE | FM_SHARE_DENY_WRITE))
		return EC_HTTP_CANNOT_CREATE_FILE;

	int nResult = Get(lpszUrl, &fs);
	if (nResult != EC_HTTP_SUCCESS)
		DeleteFile(lpszLocalFileName);
	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::RequestFile(LPCTSTR lpszUrl)
{
	bool bCanRecvContent;
	int nResult = ExecuteHttpRequest(HMT_GET, lpszUrl, NULL, NULL, true, bCanRecvContent);
	if (nResult != EC_HTTP_SUCCESS || !bCanRecvContent)
		TcpDisconnect(true);
	return nResult;
}

//-----------------------------------------------------------------------------

int CHttpClient::ReceiveFile(void *pBuffer, int nSize, int nTimeOutMSecs)
{
	return m_TcpClient.RecvBuffer(pBuffer, nSize, true, nTimeOutMSecs);
}

///////////////////////////////////////////////////////////////////////////////
// CIocpHttpClient

CIocpHttpClient::CIocpHttpClient() :
	m_nCurOpType(OT_NONE),
	m_nCurOpStep(OS_NONE),
	m_nErrorCode(EC_HTTP_SUCCESS),
	m_nHttpMethod(HMT_GET),
	m_pRequestContent(NULL),
	m_pResponseContent(NULL),
	m_nReqStreamPos(0),
	m_nResStreamPos(0),
	m_nRemainContentSize(0),
	m_bWorking(false),
	m_bDestroying(false)
{
	// nothing
}

//-----------------------------------------------------------------------------

CIocpHttpClient::~CIocpHttpClient()
{
	IFC_ASSERT(IsIocpObjectAvailable());
	IFC_ASSERT(!GetIocpObject().IsInWorkerThread());

	m_bDestroying = true;
	GetTcpConnectorPoolObject().RemoveTask(&m_TcpClient);
	m_TcpClient.Disconnect();
	GetIocpObject().WaitForComplete(this);
	Finalize();
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::InvokeCallBack()
{
	class CAutoFinalizer
	{
	private:
		CIocpHttpClient& m_Owner;
	public:
		CAutoFinalizer(CIocpHttpClient& Owner) : m_Owner(Owner) {}
		~CAutoFinalizer() { m_Owner.m_bWorking = false; }
	} AutoFinalizer(*this);

	Finalize();

	OPER_TYPE nOldOpType = m_nCurOpType;
	m_nCurOpType = OT_NONE;

	switch (nOldOpType)
	{
	case OT_DOWNLOAD_FILE:
		{
			TcpDisconnect();

			if (m_nErrorCode != EC_HTTP_SUCCESS)
				DeleteFile(m_DownloadFileTask.strLocalFileName);

			if (m_OnDownloadFile.pProc)
			{
				m_OnDownloadFile.pProc(
					m_OnDownloadFile.pParam,
					this,
					m_DownloadFileTask.strUrl,
					m_DownloadFileTask.strLocalFileName,
					m_nErrorCode
					);
			}
		}
		break;

	case OT_REQUEST_FILE:
		if (m_OnRequestFile.pProc)
		{
			m_OnRequestFile.pProc(
				m_OnRequestFile.pParam,
				this,
				m_RequestFileTask.strUrl,
				m_nErrorCode
				);
		}
		break;

	case OT_RECEIVE_FILE:
		if (m_OnReceiveFile.pProc)
		{
			m_OnReceiveFile.pProc(
				m_OnReceiveFile.pParam,
				this,
				m_ReceiveFileTask.pBuffer,
				m_ReceiveFileTask.nSize,
				m_nErrorCode
				);
		}
		break;
	}
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::Finalize()
{
	delete m_DownloadFileTask.pFileStream;
	m_DownloadFileTask.pFileStream = NULL;

	if (m_nCurOpType == OT_DOWNLOAD_FILE)
	{
		if (m_bDestroying || m_nErrorCode != EC_HTTP_SUCCESS)
			DeleteFile(m_DownloadFileTask.strLocalFileName);
	}
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::PrepareHttpAction(HTTP_METHOD_TYPE nHttpMethod, LPCTSTR lpszUrl,
	CStream *pRequestContent, CStream *pResponseContent)
{
	m_nHttpMethod = nHttpMethod;
	m_strUrl = lpszUrl;

	m_pRequestContent = pRequestContent;
	m_pResponseContent = pResponseContent;
	m_nReqStreamPos = (pRequestContent? pRequestContent->GetPosition() : 0);
	m_nResStreamPos = (pResponseContent? pResponseContent->GetPosition() : 0);

	m_nRedirectCount = 0;
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::ExecuteHttpAction()
{
	m_nCurOpStep = OS_NONE;

	int nResult = BeforeRequest(m_nHttpMethod, m_strUrl, m_pRequestContent, m_pResponseContent,
		m_nReqStreamPos, m_nResStreamPos);

	if (nResult == EC_HTTP_SUCCESS)
	{
		m_nCurOpStep = OS_TCP_CONNECT;
		TcpConnect();
	}
	else
	{
		m_nErrorCode = nResult;
		InvokeCallBack();
	}
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::TcpConnect()
{
	if (!m_bLastKeepAlive)
		m_TcpClient.Disconnect();

	if (m_TcpClient.IsConnected())
	{
		CPeerAddress PeerAddr = GetPeerAddrFromUrl(m_Url);
		if (m_TcpClient.GetPeerAddr() != PeerAddr)
			m_TcpClient.Disconnect();
	}

	if (!m_TcpClient.IsConnected())
	{
		GetTcpConnectorPoolObject().AddTask(&m_TcpClient, m_Url.GetHost(),
			StrToInt(m_Url.GetPort(), DEFAULT_HTTP_PORT), TcpConnectResultProc,
			this, m_Options.nTcpConnectTimeOut);
	}
	else
		TcpConnectResultProc(this, &m_TcpClient, true);
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::TcpConnectResultProc(void *pParam, CTcpClient *pTcpClient, bool bSuccess)
{
	CIocpHttpClient *pThis = (CIocpHttpClient*)pParam;
	if (bSuccess)
	{
		pThis->m_nCurOpStep = OS_SEND_REQUEST_HEADER;

		SOCKET nSocketHandle = pTcpClient->GetSocket().GetHandle();
		GetIocpObject().AssociateHandle(nSocketHandle);

		pThis->MakeRequestBuffer(pThis->m_TransBuffer);
		GetIocpObject().Send((SOCKET)nSocketHandle, pThis->m_TransBuffer.Data(),
			pThis->m_TransBuffer.GetSize(), 0, IOCP_CALLBACK_DEF(IocpCallBackProc, NULL),
			pThis, CIocpParams());
	}
	else
	{
		pThis->m_nErrorCode = EC_HTTP_SOCKET_ERROR;
		pThis->InvokeCallBack();
	}
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::IocpCallBackProc(const CIocpTaskData& TaskData, void *pParam)
{
	CIocpHttpClient *pThis = (CIocpHttpClient*)TaskData.GetCaller();
	if (pThis->m_bDestroying) return;

	if (TaskData.GetErrorCode() != 0)
	{
		pThis->TcpDisconnect(true);
		pThis->m_nErrorCode = EC_HTTP_IOCP_ERROR;
		pThis->InvokeCallBack();
		return;
	}

	if (TaskData.GetBytesTrans() < TaskData.GetDataSize())
	{
		if (TaskData.GetTaskType() == ITT_SEND)
		{
			GetIocpObject().Send((SOCKET)TaskData.GetFileHandle(),
				TaskData.GetEntireDataBuf(),
				TaskData.GetEntireDataSize(),
				(int)(TaskData.GetDataBuf() - TaskData.GetEntireDataBuf() + TaskData.GetBytesTrans()),
				TaskData.GetCallBack(), TaskData.GetCaller(), TaskData.GetParams());
		}
		else if (TaskData.GetTaskType() == ITT_RECV)
		{
			GetIocpObject().Recv((SOCKET)TaskData.GetFileHandle(),
				TaskData.GetEntireDataBuf(),
				TaskData.GetEntireDataSize(),
				(int)(TaskData.GetDataBuf() - TaskData.GetEntireDataBuf() + TaskData.GetBytesTrans()),
				TaskData.GetCallBack(), TaskData.GetCaller(), TaskData.GetParams());
		}

		return;
	}

	if (pThis->m_nCurOpType == OT_DOWNLOAD_FILE || pThis->m_nCurOpType == OT_REQUEST_FILE)
	{
		switch (pThis->m_nCurOpStep)
		{
		case OS_SEND_REQUEST_HEADER:
			{
				pThis->m_nCurOpStep = OS_RECV_RESPONSE_HEADER;
				pThis->m_TransBuffer.Clear();
				pThis->IocpCallBackProc(TaskData, pParam);
				break;
			}

		case OS_RECV_RESPONSE_HEADER:
			{
				bool bHeaderRecved, bError;
				pThis->CheckResponseHeader(pThis->m_TransBuffer.Data(),
					 pThis->m_TransBuffer.GetSize(), bHeaderRecved, bError);
				if (bError)
				{
					pThis->m_nErrorCode = EC_HTTP_RESPONSE_TEXT_ERROR;
					pThis->InvokeCallBack();
					break;
				}

				if (bHeaderRecved)
				{
					bool bOk = pThis->ParseResponseHeader(pThis->m_TransBuffer.Data(),
						pThis->m_TransBuffer.GetSize());
					if (!bOk)
					{
						pThis->m_nErrorCode = EC_HTTP_RESPONSE_TEXT_ERROR;
						pThis->InvokeCallBack();
						break;
					}

					HTTP_NEXT_OP nNextOp = pThis->ProcessResponseHeader();
					if (nNextOp == HNO_REDIRECT)
					{
						pThis->m_strUrl = pThis->m_Response.GetLocation();
						pThis->ExecuteHttpAction();
					}
					else if (nNextOp == HNO_RECV_CONTENT)
					{
						pThis->m_nCurOpStep = OS_RECV_RESPONSE_CONTENT;
						pThis->m_TransBuffer.Clear();

						if (pThis->m_Response.GetContentStream() != NULL)
							pThis->IocpCallBackProc(TaskData, pParam);
						else
							pThis->InvokeCallBack();
					}
					else
					{
						pThis->m_nErrorCode = EC_HTTP_CANNOT_RECV_CONTENT;
						pThis->InvokeCallBack();
					}
				}
				else
				{
					pThis->m_TransBuffer.SetSize(pThis->m_TransBuffer.GetSize() + 1);
					GetIocpObject().Recv((SOCKET)TaskData.GetFileHandle(),
						pThis->m_TransBuffer.Data(), pThis->m_TransBuffer.GetSize(),
						pThis->m_TransBuffer.GetSize() - 1,
						TaskData.GetCallBack(), TaskData.GetCaller(), TaskData.GetParams());
				}

				break;
			}

		case OS_RECV_RESPONSE_CONTENT:
			{
				INT64 nContentLength = pThis->m_Response.GetContentLength();
				if (nContentLength <= 0)
				{
					pThis->m_nErrorCode = EC_HTTP_CONTENT_LENGTH_ERROR;
					pThis->InvokeCallBack();
					break;
				}

				if (pThis->m_TransBuffer.GetSize() == 0)
				{
					pThis->m_nRemainContentSize = nContentLength;
				}
				else
				{
					pThis->m_nRemainContentSize -= pThis->m_TransBuffer.GetSize();
					pThis->m_Response.GetContentStream()->WriteBuffer(
						pThis->m_TransBuffer.Data(), pThis->m_TransBuffer.GetSize());

					if (pThis->m_nRemainContentSize <= 0)
					{
						pThis->InvokeCallBack();
						break;
					}
				}

				if (pThis->m_nRemainContentSize > 0)
				{
					const int BLOCK_SIZE = 1024*64;
					int nBlockSize = (int)Min(pThis->m_nRemainContentSize, (INT64)BLOCK_SIZE);

					pThis->m_TransBuffer.SetSize(nBlockSize);

					GetIocpObject().Recv((SOCKET)TaskData.GetFileHandle(),
						pThis->m_TransBuffer.Data(), pThis->m_TransBuffer.GetSize(), 0,
						TaskData.GetCallBack(), TaskData.GetCaller(), TaskData.GetParams());
				}

				break;
			}
		}
	}
	else if (pThis->m_nCurOpType == OT_RECEIVE_FILE)
	{
		pThis->InvokeCallBack();
	}
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::DownloadFile(LPCTSTR lpszUrl, LPCTSTR lpszLocalFileName)
{
	IFC_ASSERT(m_nCurOpType == OT_NONE);

	m_bWorking = true;
	m_nCurOpType = OT_DOWNLOAD_FILE;
	m_nErrorCode = EC_HTTP_SUCCESS;

	m_DownloadFileTask.strUrl = lpszUrl;
	m_DownloadFileTask.strLocalFileName = lpszLocalFileName;
	m_DownloadFileTask.pFileStream = NULL;

	ForceDirectories(ExtractFilePath(lpszLocalFileName));

	m_DownloadFileTask.pFileStream = new CFileStream();
	if (m_DownloadFileTask.pFileStream->Open(lpszLocalFileName, FM_CREATE | FM_SHARE_DENY_WRITE))
	{
		PrepareHttpAction(HMT_GET, lpszUrl, NULL, m_DownloadFileTask.pFileStream);
		ExecuteHttpAction();
	}
	else
	{
		m_nErrorCode = EC_HTTP_CANNOT_CREATE_FILE;
		InvokeCallBack();
	}
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::RequestFile(LPCTSTR lpszUrl)
{
	IFC_ASSERT(m_nCurOpType == OT_NONE);

	m_bWorking = true;
	m_nCurOpType = OT_REQUEST_FILE;
	m_nErrorCode = EC_HTTP_SUCCESS;

	m_RequestFileTask.strUrl = lpszUrl;

	PrepareHttpAction(HMT_GET, lpszUrl, NULL, NULL);
	ExecuteHttpAction();
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::ReceiveFile(void *pBuffer, int nSize)
{
	IFC_ASSERT(m_nCurOpType == OT_NONE);

	m_bWorking = true;
	m_nCurOpType = OT_RECEIVE_FILE;
	m_nErrorCode = EC_HTTP_SUCCESS;

	m_ReceiveFileTask.pBuffer = pBuffer;
	m_ReceiveFileTask.nSize = nSize;

	GetIocpObject().Recv((SOCKET)m_TcpClient.GetSocket().GetHandle(),
		pBuffer, nSize, 0, IOCP_CALLBACK_DEF(IocpCallBackProc, NULL), this, CIocpParams());
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::SetDownloadFileCallBack(IOCPHTTP_ONDOWNLOADFILE_PROC pProc, void *pParam)
{
	m_OnDownloadFile.pProc = pProc;
	m_OnDownloadFile.pParam = pParam;
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::SetRequestFileCallBack(IOCPHTTP_ONREQUESTFILE_PROC pProc, void *pParam)
{
	m_OnRequestFile.pProc = pProc;
	m_OnRequestFile.pParam = pParam;
}

//-----------------------------------------------------------------------------

void CIocpHttpClient::SetReceiveFileCallBack(IOCPHTTP_ONRECEIVEFILE_PROC pProc, void *pParam)
{
	m_OnReceiveFile.pProc = pProc;
	m_OnReceiveFile.pParam = pParam;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc
