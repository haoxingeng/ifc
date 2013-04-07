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

/// @file ifc_http.h
/// Defines the http classes.

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_sysutils.h"
#include "ifc_socket.h"
#include "ifc_iocp.h"
#include "ifc_exceptions.h"

/// The namespace of IFC.
namespace ifc
{

/// @addtogroup Network
/// @{

///////////////////////////////////////////////////////////////////////////////
// Classes

class CUrl;
class CHttpHeaderStrList;
class CHttpEntityHeaderInfo;
class CHttpRequestHeaderInfo;
class CHttpResponseHeaderInfo;
class CHttpRequest;
class CHttpResponse;
class CCustomHttpClient;
class CHttpClient;
class CIocpHttpClient;

///////////////////////////////////////////////////////////////////////////////
// Type Definitions

/// HTTP version
enum HTTP_PROTO_VER
{
	HPV_1_0,
	HPV_1_1,
};

/// HTTP methods
enum HTTP_METHOD_TYPE
{
	HMT_GET,
	HMT_POST,
};

/// The return value of ProcessResponseHeader()
enum HTTP_NEXT_OP
{
	HNO_REDIRECT,
	HNO_RECV_CONTENT,
	HNO_EXIT,
};

///////////////////////////////////////////////////////////////////////////////
// Callback Definitions

typedef void (*IOCPHTTP_ONDOWNLOADFILE_PROC)(void *pParam, CIocpHttpClient *pHttpClient,
	LPCTSTR lpszUrl, LPCTSTR lpszLocalFileName, int nErrorCode);
typedef void (*IOCPHTTP_ONREQUESTFILE_PROC)(void *pParam, CIocpHttpClient *pHttpClient,
	LPCTSTR lpszUrl, int nErrorCode);
typedef void (*IOCPHTTP_ONRECEIVEFILE_PROC)(void *pParam, CIocpHttpClient *pHttpClient,
	void *pBuffer, int nSize, int nErrorCode);

///////////////////////////////////////////////////////////////////////////////
// Constant Definitions

/// Default user-agent string
const TCHAR* const IFC_DEFAULT_USER_AGENT = TEXT("Mozilla/4.0 (compatible; IFC)");
/// The default http port
const int DEFAULT_HTTP_PORT = 80;

/// Default Timeout defines:
const int HTTP_TCP_CONNECT_TIMEOUT         = 1000*30;     // TCP connect timeout (ms).
const int HTTP_SEND_REQ_HEADER_TIMEOUT     = 1000*30;     // Send request header timeout.
const int HTTP_SEND_REQ_CONT_BLOCK_TIMEOUT = 1000*60*2;   // Send request content block timeout.
const int HTTP_RECV_RES_HEADER_TIMEOUT     = 1000*30;     // Receive response header timeout.
const int HTTP_RECV_RES_CONT_BLOCK_TIMEOUT = 1000*60*2;   // Receive response content block timeout.
const int HTTP_SOCKET_OP_TIMEOUT           = 1000*60*10;  // Socket operation (recv/send) timeout.

/// Error Codes:
const int EC_HTTP_SUCCESS                  =  0;
const int EC_HTTP_UNKNOWN_ERROR            = -1;
const int EC_HTTP_SOCKET_ERROR             = -2;
const int EC_HTTP_URL_ERROR                = -3;
const int EC_HTTP_CONTENT_LENGTH_ERROR     = -4;
const int EC_HTTP_SEND_TIMEOUT             = -5;
const int EC_HTTP_RECV_TIMEOUT             = -6;
const int EC_HTTP_CANNOT_CREATE_FILE       = -7;
const int EC_HTTP_RESPONSE_TEXT_ERROR      = -8;
const int EC_HTTP_CANNOT_RECV_CONTENT      = -9;
const int EC_HTTP_IOCP_ERROR               = -10;

///////////////////////////////////////////////////////////////////////////////
/// CHttpClientOptions

struct CHttpClientOptions
{
public:
	int nTcpConnectTimeOut;                // TCP connect timeout (ms).
	int nSendReqHeaderTimeOut;             // Send request header timeout.
	int nSendReqContBlockTimeOut;          // Send request content block timeout.
	int nRecvResHeaderTimeOut;             // Receive response header timeout.
	int nRecvResContBlockTimeOut;          // Receive response content block timeout.
	int nSocketOpTimeOut;                  // Socket operation (recv/send) timeout.
public:
	CHttpClientOptions()
	{
		nTcpConnectTimeOut = HTTP_TCP_CONNECT_TIMEOUT;
		nSendReqHeaderTimeOut = HTTP_SEND_REQ_HEADER_TIMEOUT;
		nSendReqContBlockTimeOut = HTTP_SEND_REQ_CONT_BLOCK_TIMEOUT;
		nRecvResHeaderTimeOut = HTTP_RECV_RES_HEADER_TIMEOUT;
		nRecvResContBlockTimeOut = HTTP_RECV_RES_CONT_BLOCK_TIMEOUT;
		nSocketOpTimeOut = HTTP_SOCKET_OP_TIMEOUT;
	}
};

///////////////////////////////////////////////////////////////////////////////
/// CHttpHeaderStrList

class CHttpHeaderStrList
{
private:
	CStrList m_Items;
	CString m_strNameValueSep;
private:
	CString MakeLine(LPCTSTR lpszName, LPCTSTR lpszValue) const;
public:
	CHttpHeaderStrList();
	virtual ~CHttpHeaderStrList() {}

	int Add(LPCTSTR lpszStr);
	void Delete(int nIndex);
	void Clear();
	void AddStrings(const CHttpHeaderStrList& Strings);
	void MoveToTop(const CStrList& NameList);
	int IndexOfName(LPCTSTR lpszName) const;

	int GetCount() const { return m_Items.GetCount(); }
	const CString& GetString(int nIndex) const { return m_Items[nIndex]; }
	CString GetText() const;
	CString GetName(int nIndex) const;
	CString GetValue(int nIndex) const;
	CString GetValue(LPCTSTR lpszName) const;
	void SetValue(LPCTSTR lpszName, LPCTSTR lpszValue);

	const CString& operator[] (int nIndex) const { return GetString(nIndex); }
};

///////////////////////////////////////////////////////////////////////////////
/// CHttpEntityHeaderInfo

class CHttpEntityHeaderInfo
{
protected:
	CHttpHeaderStrList m_RawHeaders;
	CHttpHeaderStrList m_CustomHeaders;
	CString m_strCacheControl;
	CString m_strConnection;
	CString m_strContentDisposition;
	CString m_strContentEncoding;
	CString m_strContentLanguage;
	INT64 m_nContentLength;
	INT64 m_nContentRangeStart;
	INT64 m_nContentRangeEnd;
	INT64 m_nContentRangeInstanceLength;
	CString m_strContentType;
	CString m_strContentVersion;
	CString m_strDate;
	CString m_strExpires;
	CString m_strETag;
	CString m_strLastModified;
	CString m_strPragma;
	CString m_strTransferEncoding;
protected:
	void Init();
public:
	CHttpEntityHeaderInfo() { Init(); }
	virtual ~CHttpEntityHeaderInfo() {}

	virtual void Clear();
	virtual void ParseHeaders();
	virtual void BuildHeaders();

	bool HasContentLength() { return m_nContentLength >= 0; }
	bool HasContentRange() { return m_nContentRangeEnd > 0; }
	bool HasContentRangeInstance() { return m_nContentRangeInstanceLength > 0; }

	CHttpHeaderStrList& GetRawHeaders() { return m_RawHeaders; }
	CHttpHeaderStrList& GetCustomHeaders() { return m_CustomHeaders; }

	const CString& GetCacheControl() const { return m_strCacheControl; }
	const CString& GetConnection() const { return m_strConnection; }
	const CString& GetContentDisposition() const { return m_strContentDisposition; }
	const CString& GetContentEncoding() const { return m_strContentEncoding; }
	const CString& GetContentLanguage() const { return m_strContentLanguage; }
	const INT64& GetContentLength() const { return m_nContentLength; }
	const INT64& GetContentRangeStart() const { return m_nContentRangeStart; }
	const INT64& GetContentRangeEnd() const { return m_nContentRangeEnd; }
	const INT64& GetContentRangeInstanceLength() const { return m_nContentRangeInstanceLength; }
	const CString& GetContentType() const { return m_strContentType; }
	const CString& GetContentVersion() const { return m_strContentVersion; }
	const CString& GetDate() const { return m_strDate; }
	const CString& GetExpires() const { return m_strExpires; }
	const CString& GetETag() const { return m_strETag; }
	const CString& GetLastModified() const { return m_strLastModified; }
	const CString& GetPragma() const { return m_strPragma; }
	const CString& GetTransferEncoding() const { return m_strTransferEncoding; }

	void SetCustomHeaders(const CHttpHeaderStrList& val) { m_CustomHeaders = val; }
	void SetCacheControl(LPCTSTR lpszValue) { m_strCacheControl = lpszValue; }
	void SetConnection(LPCTSTR lpszValue) { m_strConnection = lpszValue; }
	void SetConnection(bool bKeepAlive) { m_strConnection = (bKeepAlive? TEXT("keep-alive") : TEXT("close")); }
	void SetContentDisposition(LPCTSTR lpszValue) { m_strContentDisposition = lpszValue; }
	void SetContentEncoding(LPCTSTR lpszValue) { m_strContentEncoding = lpszValue; }
	void SetContentLanguage(LPCTSTR lpszValue) { m_strContentLanguage = lpszValue; }
	void SetContentLength(INT64 nValue) { m_nContentLength = nValue; }
	void SetContentRangeStart(INT64 nValue) { m_nContentRangeStart = nValue; }
	void SetContentRangeEnd(INT64 nValue) { m_nContentRangeEnd = nValue; }
	void SetContentRangeInstanceLength(INT64 nValue) { m_nContentRangeInstanceLength = nValue; }
	void SetContentType(LPCTSTR lpszValue) { m_strContentType = lpszValue; }
	void SetContentVersion(LPCTSTR lpszValue) { m_strContentVersion = lpszValue; }
	void SetDate(LPCTSTR lpszValue) { m_strDate = lpszValue; }
	void SetExpires(LPCTSTR lpszValue) { m_strExpires = lpszValue; }
	void SetETag(LPCTSTR lpszValue) { m_strETag = lpszValue; }
	void SetLastModified(LPCTSTR lpszValue) { m_strLastModified = lpszValue; }
	void SetPragma(LPCTSTR lpszValue) { m_strPragma = lpszValue; }
	void SetTransferEncoding(LPCTSTR lpszValue) { m_strTransferEncoding = lpszValue; }
};

///////////////////////////////////////////////////////////////////////////////
/// CHttpRequestHeaderInfo

class CHttpRequestHeaderInfo : public CHttpEntityHeaderInfo
{
protected:
	CString m_strAccept;
	CString m_strAcceptCharSet;
	CString m_strAcceptEncoding;
	CString m_strAcceptLanguage;
	CString m_strFrom;
	CString m_strReferer;
	CString m_strUserAgent;
	CString m_strHost;
	CString m_strRange;
protected:
	void Init();
public:
	CHttpRequestHeaderInfo() { Init(); }

	virtual void Clear();
	virtual void ParseHeaders();
	virtual void BuildHeaders();

	const CString& GetAccept() const { return  m_strAccept; }
	const CString& GetAcceptCharSet() const { return  m_strAcceptCharSet; }
	const CString& GetAcceptEncoding() const { return  m_strAcceptEncoding; }
	const CString& GetAcceptLanguage() const { return  m_strAcceptLanguage; }
	const CString& GetFrom() const { return  m_strFrom; }
	const CString& GetReferer() const { return  m_strReferer; }
	const CString& GetUserAgent() const { return  m_strUserAgent; }
	const CString& GetHost() const { return  m_strHost; }
	const CString& GetRange() const { return  m_strRange; }

	void SetAccept(LPCTSTR lpszValue) { m_strAccept = lpszValue; }
	void SetAcceptCharSet(LPCTSTR lpszValue) { m_strAcceptCharSet = lpszValue; }
	void SetAcceptEncoding(LPCTSTR lpszValue) { m_strAcceptEncoding = lpszValue; }
	void SetAcceptLanguage(LPCTSTR lpszValue) { m_strAcceptLanguage = lpszValue; }
	void SetFrom(LPCTSTR lpszValue) { m_strFrom = lpszValue; }
	void SetReferer(LPCTSTR lpszValue) { m_strReferer = lpszValue; }
	void SetUserAgent(LPCTSTR lpszValue) { m_strUserAgent = lpszValue; }
	void SetHost(LPCTSTR lpszValue) { m_strHost = lpszValue; }
	void SetRange(LPCTSTR lpszValue) { m_strRange = lpszValue; }
	void SetRange(INT64 nRangeStart, INT64 nRangeEnd = -1);
};

///////////////////////////////////////////////////////////////////////////////
/// CHttpResponseHeaderInfo

class CHttpResponseHeaderInfo : public CHttpEntityHeaderInfo
{
protected:
	CString m_strAcceptRanges;
	CString m_strLocation;
	CString m_strServer;
protected:
	void Init();
public:
	CHttpResponseHeaderInfo() { Init(); }

	virtual void Clear();
	virtual void ParseHeaders();
	virtual void BuildHeaders();

	const CString& GetAcceptRanges() const { return  m_strAcceptRanges; }
	const CString& GetLocation() const { return  m_strLocation; }
	const CString& GetServer() const { return  m_strServer; }

	void SetAcceptRanges(LPCTSTR lpszValue) { m_strAcceptRanges = lpszValue; }
	void SetLocation(LPCTSTR lpszValue) { m_strLocation = lpszValue; }
	void SetServer(LPCTSTR lpszValue) { m_strServer = lpszValue; }
};

///////////////////////////////////////////////////////////////////////////////
/// CHttpRequest

class CHttpRequest : public CHttpRequestHeaderInfo
{
protected:
	CCustomHttpClient& m_HttpClient;
	HTTP_PROTO_VER m_nProtocolVersion;
	CString m_strUrl;
	CString m_strMethod;
	CStream *m_pContentStream;
protected:
	void Init();
public:
	CHttpRequest(CCustomHttpClient& HttpClient);

	virtual void Clear();

	HTTP_PROTO_VER GetProtocolVersion() const { return m_nProtocolVersion; }
	const CString& GetUrl() const { return m_strUrl; }
	const CString& GetMethod() const { return m_strMethod; }
	CStream* GetContentStream() const { return m_pContentStream; }

	void SetProtocolVersion(HTTP_PROTO_VER nValue) { m_nProtocolVersion = nValue; }
	void SetUrl(LPCTSTR lpszValue) { m_strUrl = lpszValue; }
	void SetMethod(LPCTSTR lpszValue) { m_strMethod = lpszValue; }
	void SetContentStream(CStream *pValue) { m_pContentStream = pValue; }
};

///////////////////////////////////////////////////////////////////////////////
/// CHttpResponse

class CHttpResponse : public CHttpResponseHeaderInfo
{
protected:
	CCustomHttpClient& m_HttpClient;
	CString m_strResponseText;
	CStream *m_pContentStream;
protected:
	void Init();
public:
	CHttpResponse(CCustomHttpClient& HttpClient);

	virtual void Clear();

	bool GetKeepAlive();
	const CString& GetResponseText() const { return m_strResponseText; }
	int GetResponseCode() const;
	HTTP_PROTO_VER GetResponseVersion() const;
	CStream* GetContentStream() const { return m_pContentStream; }

	void SetResponseText(LPCTSTR lpszValue) { m_strResponseText = lpszValue; }
	void SetContentStream(CStream *pValue) { m_pContentStream = pValue; }
};

///////////////////////////////////////////////////////////////////////////////
/// CCustomHttpClient - HTTP client base class.

class CCustomHttpClient
{
public:
	friend class CHttpRequest;
	friend class CHttpResponse;
protected:
	CTcpClient m_TcpClient;
	CHttpRequest m_Request;
	CHttpResponse m_Response;
	CHttpClientOptions m_Options;
	CUrl m_Url;
	bool m_bHandleRedirects;
	int m_nRedirectCount;
	bool m_bLastKeepAlive;
protected:
	CPeerAddress GetPeerAddrFromUrl(CUrl& Url);
	void MakeRequestBuffer(CBuffer& Buffer);
	int BeforeRequest(HTTP_METHOD_TYPE nHttpMethod, LPCTSTR lpszUrl, CStream *pRequestContent,
		CStream *pResponseContent, INT64 nReqStreamPos, INT64 nResStreamPos);
	void CheckResponseHeader(char *pBuffer, int nSize, bool& bFinished, bool& bError);
	bool ParseResponseHeader(void *pBuffer, int nSize);
	HTTP_NEXT_OP ProcessResponseHeader();
	void TcpDisconnect(bool bForce = false);
public:
	CCustomHttpClient();
	virtual ~CCustomHttpClient();

	/// Force to disconnect the connection.
	void Disconnect() { TcpDisconnect(true); }
	/// Indicates whether the connection is currently connected or not.
	bool IsConnected() { return m_TcpClient.IsConnected(); }

	/// The http request info.
	CHttpRequest& HttpRequest() { return m_Request; }
	/// The http response info.
	CHttpResponse& HttpResponse() { return m_Response; }
	/// The http client options
	CHttpClientOptions& Options() { return m_Options; }
	/// Returns the response text.
	CString GetResponseText() { return m_Response.GetResponseText(); }
	/// Returns the response code.
	int GetResponseCode() { return m_Response.GetResponseCode(); }
	/// Indicates if the http client can handle redirections.
	bool GetHandleRedirects() { return m_bHandleRedirects; }
	/// Indicates the number of redirects encountered in the last request for the http client.
	int GetRedirectCount() { return m_nRedirectCount; }
	/// Returns the tcp client object.
	CTcpClient& GetTcpClient() { return m_TcpClient; }

	/// Determines if the http client can handle redirections.
	void SetHandleRedirects(bool bValue) { m_bHandleRedirects = bValue; }
};

///////////////////////////////////////////////////////////////////////////////
/// CHttpClient - HTTP client class.

class CHttpClient : public CCustomHttpClient
{
public:
	friend class CAutoFinalizer;
private:
	int ReadLine(CString& strLine, int nTimeOut);
	int ReadChunkSize(UINT& nChunkSize, int nTimeOut);
	int ReadStream(CStream& Stream, int nBytes, int nTimeOut);
protected:
	int ExecuteHttpAction(HTTP_METHOD_TYPE nHttpMethod, LPCTSTR lpszUrl,
		CStream *pRequestContent, CStream *pResponseContent);
	int ExecuteHttpRequest(HTTP_METHOD_TYPE nHttpMethod, LPCTSTR lpszUrl,
		CStream *pRequestContent, CStream *pResponseContent,
		bool bNeedRecvContent, bool& bCanRecvContent);

	int TcpConnect();
	int SendRequestHeader();
	int SendRequestContent();
	int RecvResponseHeader();
	int RecvResponseContent();
public:
	CHttpClient();
	virtual ~CHttpClient();

	/// Sends a "GET" request to http server, and receives the response content. Returns the error code (EC_HTTP_XXX).
	int Get(LPCTSTR lpszUrl, CStream *pResponseContent);
	/// Sends a "POST" request to http server with the specified request content, and receives the response content. Returns the error code (EC_HTTP_XXX).
	int Post(LPCTSTR lpszUrl, CStream *pRequestContent, CStream *pResponseContent);

	/// Downloads the entire file from the specified url. Returns the error code (EC_HTTP_XXX).
	int DownloadFile(LPCTSTR lpszUrl, LPCTSTR lpszLocalFileName);
	/// Sends the "GET" request to http server, and receives the response text and headers. Returns the error code (EC_HTTP_XXX).
	int RequestFile(LPCTSTR lpszUrl);
	/// Try to receive the response content from http server, returns the total number of bytes received actually, -1 if error.
	int ReceiveFile(void *pBuffer, int nSize, int nTimeOutMSecs = -1);
};

///////////////////////////////////////////////////////////////////////////////
/// CIocpHttpClient - IOCP HTTP client class

class CIocpHttpClient : public CCustomHttpClient
{
public:
	friend class CAutoFinalizer;

private:
	enum OPER_TYPE
	{
		OT_NONE           = 0,
		OT_DOWNLOAD_FILE  = 1,
		OT_REQUEST_FILE   = 2,
		OT_RECEIVE_FILE   = 3,
	};

	enum OPER_STEP
	{
		OS_NONE,
		OS_TCP_CONNECT,
		OS_SEND_REQUEST_HEADER,
		OS_RECV_RESPONSE_HEADER,
		OS_RECV_RESPONSE_CONTENT,
	};

	struct CDownloadFileTask
	{
	public:
		CString strUrl;
		CString strLocalFileName;
		CFileStream *pFileStream;
	public:
		CDownloadFileTask() : pFileStream(NULL) {}
	};

	struct CRequestFileTask
	{
	public:
		CString strUrl;
	};

	struct CReceiveFileTask
	{
	public:
		void *pBuffer;
		int nSize;
	public:
		CReceiveFileTask() : pBuffer(NULL), nSize(0) {}
	};

private:
	OPER_TYPE m_nCurOpType;
	OPER_STEP m_nCurOpStep;
	CDownloadFileTask m_DownloadFileTask;
	CRequestFileTask m_RequestFileTask;
	CReceiveFileTask m_ReceiveFileTask;
	int m_nErrorCode;
	CBuffer m_TransBuffer;
	bool m_bWorking;
	bool m_bDestroying;

	HTTP_METHOD_TYPE m_nHttpMethod;
	CString m_strUrl;
	CStream *m_pRequestContent;
	CStream *m_pResponseContent;
	INT64 m_nReqStreamPos;
	INT64 m_nResStreamPos;
	INT64 m_nRemainContentSize;

	CCallBackDef<IOCPHTTP_ONDOWNLOADFILE_PROC> m_OnDownloadFile;
	CCallBackDef<IOCPHTTP_ONREQUESTFILE_PROC> m_OnRequestFile;
	CCallBackDef<IOCPHTTP_ONRECEIVEFILE_PROC> m_OnReceiveFile;
private:
	void InvokeCallBack();
	void Finalize();

	void PrepareHttpAction(HTTP_METHOD_TYPE nHttpMethod, LPCTSTR lpszUrl,
		CStream *pRequestContent, CStream *pResponseContent);
	void ExecuteHttpAction();
	void TcpConnect();

	static void TcpConnectResultProc(void *pParam, CTcpClient *pTcpClient, bool bSuccess);
	static void IocpCallBackProc(const CIocpTaskData& TaskData, void *pParam);
public:
	CIocpHttpClient();
	virtual ~CIocpHttpClient();

	/// Downloads the entire file from the specified url.
	void DownloadFile(LPCTSTR lpszUrl, LPCTSTR lpszLocalFileName);
	/// Sends the "GET" request to http server, and receives the response text and headers.
	void RequestFile(LPCTSTR lpszUrl);
	/// Try to receive the response content from http server.
	void ReceiveFile(void *pBuffer, int nSize);

	/// Sets the callback of DownloadFile().
	void SetDownloadFileCallBack(IOCPHTTP_ONDOWNLOADFILE_PROC pProc, void *pParam = NULL);
	/// Sets the callback of RequestFile().
	void SetRequestFileCallBack(IOCPHTTP_ONREQUESTFILE_PROC pProc, void *pParam = NULL);
	/// Sets the callback of ReceiveFile().
	void SetReceiveFileCallBack(IOCPHTTP_ONRECEIVEFILE_PROC pProc, void *pParam = NULL);

	/// Indicates whether the http client object is working.
	bool IsWorking() { return m_bWorking; }
};

///////////////////////////////////////////////////////////////////////////////

/// @}

} // namespace ifc
