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

/// @file ifc_socket.h
/// Defines the socket classes.

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_exceptions.h"
#include "ifc_classes.h"
#include "ifc_thread.h"
#include "ifc_sync_objs.h"

#include <winsock2.h>

/// The namespace of IFC.
namespace ifc
{

/// @addtogroup Network
/// @{

///////////////////////////////////////////////////////////////////////////////
// Classes

class CIfcSocket;
class CUdpSocket;
class CUdpClient;
class CUdpServer;
class CTcpSocket;
class CDtpConnection;
class CTcpConnection;
class CTcpClient;
class CTcpServer;
class CListenerThread;
class CUdpListenerThread;
class CUdpListenerThreadPool;
class CTcpListenerThread;
class CTcpConnectorPool;

///////////////////////////////////////////////////////////////////////////////
// Const Defines

const int SS_SD_RECV            = 0;
const int SS_SD_SEND            = 1;
const int SS_SD_BOTH            = 2;

const int SS_NOTINITIALISED     = WSANOTINITIALISED;
const int SS_EINTR              = WSAEINTR;
const int SS_EBADF              = WSAEBADF;
const int SS_EACCES             = WSAEACCES;
const int SS_EFAULT             = WSAEFAULT;
const int SS_EINVAL             = WSAEINVAL;
const int SS_EMFILE             = WSAEMFILE;
const int SS_EWOULDBLOCK        = WSAEWOULDBLOCK;
const int SS_EINPROGRESS        = WSAEINPROGRESS;
const int SS_EALREADY           = WSAEALREADY;
const int SS_ENOTSOCK           = WSAENOTSOCK;
const int SS_EDESTADDRREQ       = WSAEDESTADDRREQ;
const int SS_EMSGSIZE           = WSAEMSGSIZE;
const int SS_EPROTOTYPE         = WSAEPROTOTYPE;
const int SS_ENOPROTOOPT        = WSAENOPROTOOPT;
const int SS_EPROTONOSUPPORT    = WSAEPROTONOSUPPORT;
const int SS_ESOCKTNOSUPPORT    = WSAESOCKTNOSUPPORT;
const int SS_EOPNOTSUPP         = WSAEOPNOTSUPP;
const int SS_EPFNOSUPPORT       = WSAEPFNOSUPPORT;
const int SS_EAFNOSUPPORT       = WSAEAFNOSUPPORT;
const int SS_EADDRINUSE         = WSAEADDRINUSE;
const int SS_EADDRNOTAVAIL      = WSAEADDRNOTAVAIL;
const int SS_ENETDOWN           = WSAENETDOWN;
const int SS_ENETUNREACH        = WSAENETUNREACH;
const int SS_ENETRESET          = WSAENETRESET;
const int SS_ECONNABORTED       = WSAECONNABORTED;
const int SS_ECONNRESET         = WSAECONNRESET;
const int SS_ENOBUFS            = WSAENOBUFS;
const int SS_EISCONN            = WSAEISCONN;
const int SS_ENOTCONN           = WSAENOTCONN;
const int SS_ESHUTDOWN          = WSAESHUTDOWN;
const int SS_ETOOMANYREFS       = WSAETOOMANYREFS;
const int SS_ETIMEDOUT          = WSAETIMEDOUT;
const int SS_ECONNREFUSED       = WSAECONNREFUSED;
const int SS_ELOOP              = WSAELOOP;
const int SS_ENAMETOOLONG       = WSAENAMETOOLONG;
const int SS_EHOSTDOWN          = WSAEHOSTDOWN;
const int SS_EHOSTUNREACH       = WSAEHOSTUNREACH;
const int SS_ENOTEMPTY          = WSAENOTEMPTY;

///////////////////////////////////////////////////////////////////////////////
// IFC Socket Error Message

const TCHAR* const SSEM_ERROR             = TEXT("Socket Error #%d: %s");
const TCHAR* const SSEM_SOCKETERROR       = TEXT("Socket error");
const TCHAR* const SSEM_TCPSENDTIMEOUT    = TEXT("TCP send timeout");
const TCHAR* const SSEM_TCPRECVTIMEOUT    = TEXT("TCP recv timeout");

const TCHAR* const SSEM_NOTINITIALISED    = TEXT("WSAStartup not performed.");
const TCHAR* const SSEM_EINTR             = TEXT("Interrupted system call.");
const TCHAR* const SSEM_EBADF             = TEXT("Bad file number.");
const TCHAR* const SSEM_EACCES            = TEXT("Access denied.");
const TCHAR* const SSEM_EFAULT            = TEXT("Buffer fault.");
const TCHAR* const SSEM_EINVAL            = TEXT("Invalid argument.");
const TCHAR* const SSEM_EMFILE            = TEXT("Too many open files.");
const TCHAR* const SSEM_EWOULDBLOCK       = TEXT("Operation would block.");
const TCHAR* const SSEM_EINPROGRESS       = TEXT("Operation now in progress.");
const TCHAR* const SSEM_EALREADY          = TEXT("Operation already in progress.");
const TCHAR* const SSEM_ENOTSOCK          = TEXT("Socket operation on non-socket.");
const TCHAR* const SSEM_EDESTADDRREQ      = TEXT("Destination address required.");
const TCHAR* const SSEM_EMSGSIZE          = TEXT("Message too long.");
const TCHAR* const SSEM_EPROTOTYPE        = TEXT("Protocol wrong type for socket.");
const TCHAR* const SSEM_ENOPROTOOPT       = TEXT("Bad protocol option.");
const TCHAR* const SSEM_EPROTONOSUPPORT   = TEXT("Protocol not supported.");
const TCHAR* const SSEM_ESOCKTNOSUPPORT   = TEXT("Socket type not supported.");
const TCHAR* const SSEM_EOPNOTSUPP        = TEXT("Operation not supported on socket.");
const TCHAR* const SSEM_EPFNOSUPPORT      = TEXT("Protocol family not supported.");
const TCHAR* const SSEM_EAFNOSUPPORT      = TEXT("Address family not supported by protocol family.");
const TCHAR* const SSEM_EADDRINUSE        = TEXT("Address already in use.");
const TCHAR* const SSEM_EADDRNOTAVAIL     = TEXT("Cannot assign requested address.");
const TCHAR* const SSEM_ENETDOWN          = TEXT("Network is down.");
const TCHAR* const SSEM_ENETUNREACH       = TEXT("Network is unreachable.");
const TCHAR* const SSEM_ENETRESET         = TEXT("Net dropped connection or reset.");
const TCHAR* const SSEM_ECONNABORTED      = TEXT("Software caused connection abort.");
const TCHAR* const SSEM_ECONNRESET        = TEXT("Connection reset by peer.");
const TCHAR* const SSEM_ENOBUFS           = TEXT("No buffer space available.");
const TCHAR* const SSEM_EISCONN           = TEXT("Socket is already connected.");
const TCHAR* const SSEM_ENOTCONN          = TEXT("Socket is not connected.");
const TCHAR* const SSEM_ESHUTDOWN         = TEXT("Cannot send or receive after socket is closed.");
const TCHAR* const SSEM_ETOOMANYREFS      = TEXT("Too many references, cannot splice.");
const TCHAR* const SSEM_ETIMEDOUT         = TEXT("Connection timed out.");
const TCHAR* const SSEM_ECONNREFUSED      = TEXT("Connection refused.");
const TCHAR* const SSEM_ELOOP             = TEXT("Too many levels of symbolic links.");
const TCHAR* const SSEM_ENAMETOOLONG      = TEXT("File name too long.");
const TCHAR* const SSEM_EHOSTDOWN         = TEXT("Host is down.");
const TCHAR* const SSEM_EHOSTUNREACH      = TEXT("No route to host.");
const TCHAR* const SSEM_ENOTEMPTY         = TEXT("Directory not empty");

///////////////////////////////////////////////////////////////////////////////
// Type Defines

typedef int socklen_t;
typedef struct sockaddr_in SOCK_ADDR;

/// Network protocol type (UDP|TCP)
enum NET_PROTO_TYPE
{
	NPT_UDP  = 0x01,  // UDP
	NPT_TCP  = 0x02,  // TCP
};

/// DTP protocol type (TCP|UDT)
enum DTP_PROTO_TYPE
{
	DPT_TCP  = 0x01,  // TCP
	DPT_UDT  = 0x02,  // UDT
};

/// The state of async connection
enum ASYNC_CONNECT_STATE
{
	ACS_NONE,         // Not connect yet.
	ACS_CONNECTING,   // Connecting, and no error occurs.
	ACS_CONNECTED,    // Connected successfully.
	ACS_FAILED        // Connect failed.
};

#pragma pack(1)

/// Peer address info
struct CPeerAddress
{
	DWORD nIp;        // IP (Host byte order)
	int nPort;        // Port Number

	CPeerAddress() : nIp(0), nPort(0) {}
	CPeerAddress(DWORD _nIp, int _nPort)
		{ nIp = _nIp;  nPort = _nPort; }
	bool operator == (const CPeerAddress& rhs) const
		{ return (nIp == rhs.nIp && nPort == rhs.nPort); }
	bool operator != (const CPeerAddress& rhs) const
		{ return !((*this) == rhs); }
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////////
// Procedure Type Defines

typedef void (*UDPSVR_ON_RECV_DATA_PROC)(void *pParam, void *pPacketBuffer,
	int nPacketSize, const CPeerAddress& PeerAddr);
typedef void (*TCPSVR_ON_CREATE_CONN_PROC)(void *pParam, SOCKET nSocketHandle,
	const CPeerAddress& PeerAddr, CTcpConnection*& pConnection);
typedef void (*TCPSVR_ON_ACCEPT_CONN_PROC)(void *pParam, CTcpConnection *pConnection);
typedef void (*TCPCP_ON_RESULT_PROC)(void *pParam, CTcpClient *pTcpClient, bool bSuccess);

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

/// Network initialization.
void NetworkInitialize();
/// Network finalization.
void NetworkFinalize();
/// Indicates whether the network initialized or not.
bool IsNetworkInited();
/// Ensure the network is initialized.
void EnsureNetworkInited();

/// Converts the integer (Host byte order) value to a IP string.
CString IpToString(DWORD nIp);
///  Converts the IP string to an integer value (Host byte order).
DWORD StringToIp(LPCTSTR lpszString);
/// Fills the @a SOCK_ADDR struct with the specified parameters.
void GetSocketAddr(SOCK_ADDR& SockAddr, DWORD nIpHostValue, int nPort);
/// Finds a free port and returns it, returns 0 if failed.
int GetFreePort(NET_PROTO_TYPE nProto, int nStartPort, int nCheckTimes);
/// Returns the IP address list of the local machine.
void GetLocalIpList(CStrList& IpList);
/// Returns the local machine IP address.
CString GetLocalIp();
/// Resolves the host name and return the IP address, returns empty string if failed.
CString LookupHostAddr(LPCTSTR lpszHost);
/// Throws the socket exception of the last error.
void IfcThrowSocketLastError();

// Returns the global CTcpConnectorPool object.
CTcpConnectorPool& GetTcpConnectorPoolObject();
// Delete the global CTcpConnectorPool object.
void DeleteTcpConnectorPoolObject();

///////////////////////////////////////////////////////////////////////////////
/// CIfcSocket - The base socket class.

class CIfcSocket
{
public:
	friend class CTcpServer;

protected:
	bool m_bActive;     // Indicates whether the socket is ready or not.
	SOCKET m_nHandle;   // The socket handle.
	int m_nDomain;      // (PF_UNIX, PF_INET, PF_INET6, PF_IPX, ...)
	int m_nType;        // The socket type. (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET)
	int m_nProtocol;    // The socket protocol, can be zero. (IPPROTO_IP, IPPROTO_UDP, IPPROTO_TCP, ...)
	bool m_bBlockMode;  // Indicates whether the socket is set to block mode, the default value is true.

private:
	void DoSetBlockMode(SOCKET nHandle, bool bValue);
	void DoClose();

protected:
	void SetActive(bool bValue);
	void SetDomain(int nValue);
	void SetType(int nValue);
	void SetProtocol(int nValue);

	void Bind(int nPort, bool bForce);
public:
	CIfcSocket();
	virtual ~CIfcSocket();

	/// Opens the socket.
	virtual void Open();
	/// Closes the socket.
	virtual void Close();

	/// Indicates whether the socket is opened or not.
	bool GetActive() { return m_bActive; }
	/// Returns the socket handle.
	SOCKET GetHandle() { return m_nHandle; }
	/// Returns true if the socket is block mode, false otherwise.
	bool GetBlockMode() { return m_bBlockMode; }
	/// Sets the socket block mode.
	void SetBlockMode(bool bValue);
	/// Attachs the object to the specified socket handle.
	void SetHandle(SOCKET nValue);
};

///////////////////////////////////////////////////////////////////////////////
/// CUdpSocket - The UDP socket class.

class CUdpSocket : public CIfcSocket
{
public:
	CUdpSocket()
	{
		m_nType = SOCK_DGRAM;
		m_nProtocol = IPPROTO_UDP;
		m_bBlockMode = true;
	}

	/// Receives data from the socket.
	int RecvBuffer(void *pBuffer, int nSize);
	/// Receives data from the socket.
	int RecvBuffer(void *pBuffer, int nSize, CPeerAddress& PeerAddr);
	/// Sends data through the socket.
	int SendBuffer(void *pBuffer, int nSize, const CPeerAddress& PeerAddr, int nSendTimes = 1);

	virtual void Open();
};

///////////////////////////////////////////////////////////////////////////////
/// CUdpClient - The UDP client class.

class CUdpClient : public CUdpSocket
{
public:
	CUdpClient() { Open(); }
};

///////////////////////////////////////////////////////////////////////////////
/// CUdpServer - The UDP server class.

class CUdpServer : public CUdpSocket
{
public:
	friend class CUdpListenerThread;
private:
	int m_nLocalPort;
	bool m_bForceBind;
	CUdpListenerThreadPool *m_pListenerThreadPool;
	CCallBackDef<UDPSVR_ON_RECV_DATA_PROC> m_OnRecvData;
private:
	void DataReceived(void *pPacketBuffer, int nPacketSize, const CPeerAddress& PeerAddr);
protected:
	virtual void StartListenerThreads();
	virtual void StopListenerThreads();
public:
	CUdpServer();
	virtual ~CUdpServer();

	/// Opens the UDP server.
	virtual void Open();
	/// Closes the UDP server.
	virtual void Close();

	/// Returns the bound port.
	int GetLocalPort() { return m_nLocalPort; }
	/// Sets the port to bind.
	void SetLocalPort(int nValue, bool bForceBind = false);

	/// Returns the listener thread count.
	int GetListenerThreadCount();
	/// Sets the listener thread count.
	void SetListenerThreadCount(int nValue);

	/// Sets OnRecvData callback.
	void SetOnRecvDataCallBack(UDPSVR_ON_RECV_DATA_PROC pProc, void *pParam = NULL);
};

///////////////////////////////////////////////////////////////////////////////
/// CTcpSocket - The TCP socket class.

class CTcpSocket : public CIfcSocket
{
public:
	CTcpSocket()
	{
		m_nType = SOCK_STREAM;
		m_nProtocol = IPPROTO_TCP;
		m_bBlockMode = false;
	}
};

///////////////////////////////////////////////////////////////////////////////
/// CDtpConnection - The DTP connection class (Data Transfer Protocol, including TCP and UDT).

class CDtpConnection
{
private:
	static long s_nObjectCount;
protected:
	DTP_PROTO_TYPE m_nConnType;
	CPeerAddress m_PeerAddr;
	PVOID m_pCustomData;
protected:
	/// Sends data using sync mode, returns the total number of bytes sent actually, -1 if error.
	virtual int DoSyncSendBuffer(void *pBuffer, int nSize, int nTimeOutMSecs = -1) = 0;
	/// Receives data using sync mode, returns the total number of bytes received actually, -1 if error.
	virtual int DoSyncRecvBuffer(void *pBuffer, int nSize, int nTimeOutMSecs = -1) = 0;
	/// Sends data using async mode, returns the total number of bytes sent actually, -1 if error.
	virtual int DoAsyncSendBuffer(void *pBuffer, int nSize) = 0;
	/// Receives data using async mode, returns the total number of bytes received actually, -1 if error.
	virtual int DoAsyncRecvBuffer(void *pBuffer, int nSize) = 0;
	/// Disconnects the connection.
	virtual void DoDisconnect() = 0;
public:
	CDtpConnection(DTP_PROTO_TYPE nConnType);
	virtual ~CDtpConnection();

	/// Indicates whether the connection is connected or not.
	virtual bool IsConnected() = 0;
	/// Disconnects the connection.
	void Disconnect();

	/// Sends data through the connection.
	/// Throws exception if failed.
	int SendBuffer(void *pBuffer, int nSize, bool bSyncMode = false, int nTimeOutMSecs = -1);

	/// Receives data from the connection.
	/// Throws exception if failed.
	int RecvBuffer(void *pBuffer, int nSize, bool bSyncMode = false, int nTimeOutMSecs = -1);

	DTP_PROTO_TYPE GetConnType() const { return m_nConnType; }
	const CPeerAddress& GetPeerAddr() const { return m_PeerAddr; }
	PVOID& CustomData() { return m_pCustomData; }

	/// Gets the number of CDtpConnection object currently.
	static int GetObjectCount() { return (int)s_nObjectCount; }
};

///////////////////////////////////////////////////////////////////////////////
/// CTcpConnection - The TCP connection class.

class CTcpConnection : public CDtpConnection
{
protected:
	CTcpSocket m_Socket;
protected:
	virtual int DoSyncSendBuffer(void *pBuffer, int nSize, int nTimeOutMSecs = -1);
	virtual int DoSyncRecvBuffer(void *pBuffer, int nSize, int nTimeOutMSecs = -1);
	virtual int DoAsyncSendBuffer(void *pBuffer, int nSize);
	virtual int DoAsyncRecvBuffer(void *pBuffer, int nSize);
	virtual void DoDisconnect();
public:
	CTcpConnection();
	CTcpConnection(SOCKET nSocketHandle, const CPeerAddress& PeerAddr);
	virtual ~CTcpConnection();

	virtual bool IsConnected();
	CTcpSocket& GetSocket() { return m_Socket; }
};

///////////////////////////////////////////////////////////////////////////////
/// CTcpClient - The TCP client class.

class CTcpClient : public CTcpConnection
{
public:
	/// Connects to the specified address using sync mode.
	/// Throws exception if failed.
	void Connect(const CString& strIp, int nPort);

	/// Connects to the specified address using async mode, and returns the connect state immediately.
	/// This function will never throw any exceptions.
	int AsyncConnect(const CString& strIp, int nPort, int nTimeOutMSecs = -1);

	/// Checks the async connect state.
	/// This function will never throw any exceptions.
	int CheckAsyncConnectState(int nTimeOutMSecs = -1);
};

///////////////////////////////////////////////////////////////////////////////
/// CTcpServer - The TCP server class.

class CTcpServer
{
public:
	friend class CTcpListenerThread;
public:
	enum { LISTEN_QUEUE_SIZE = 30 };   // The length of TCP listen queue.
private:
	CTcpSocket m_Socket;
	int m_nLocalPort;
	bool m_bForceBind;
	CTcpListenerThread *m_pListenerThread;
	CCallBackDef<TCPSVR_ON_CREATE_CONN_PROC> m_OnCreateConn;
	CCallBackDef<TCPSVR_ON_ACCEPT_CONN_PROC> m_OnAcceptConn;
private:
	CTcpConnection* CreateConnection(SOCKET nSocketHandle, const CPeerAddress& PeerAddr);
	void AcceptConnection(CTcpConnection *pConnection);
protected:
	virtual void StartListenerThread();
	virtual void StopListenerThread();
public:
	CTcpServer();
	virtual ~CTcpServer();

	/// Opens the TCP server.
	virtual void Open();
	/// Closes the TCP server.
	virtual void Close();

	bool GetActive() { return m_Socket.GetActive(); }
	void SetActive(bool bValue);

	int GetLocalPort() { return m_nLocalPort; }
	void SetLocalPort(int nValue, bool bForceBind = false);

	CTcpSocket& GetSocket() { return m_Socket; }

	/// Sets OnCreateConn callback.
	void SetOnCreateConnCallBack(TCPSVR_ON_CREATE_CONN_PROC pProc, void *pParam = NULL);
	/// Sets OnAcceptConn callback.
	void SetOnAcceptConnCallBack(TCPSVR_ON_ACCEPT_CONN_PROC pProc, void *pParam = NULL);
};

///////////////////////////////////////////////////////////////////////////////
/// CListenerThread - The base class of the listener thread.

class CListenerThread : public CThread
{
protected:
	virtual void Execute() {}
public:
	CListenerThread()
	{
		SetPriority(THREAD_PRIORITY_HIGHEST);
	}
	virtual ~CListenerThread() {}
};

///////////////////////////////////////////////////////////////////////////////
/// CUdpListenerThread - The UDP server listener thread class.

class CUdpListenerThread : public CListenerThread
{
private:
	CUdpListenerThreadPool *m_pThreadPool;
	CUdpServer *m_pUdpServer;
	int m_nIndex;                           // 0-based
protected:
	virtual void Execute();
public:
	explicit CUdpListenerThread(CUdpListenerThreadPool *pThreadPool, int nIndex);
	virtual ~CUdpListenerThread();
};

///////////////////////////////////////////////////////////////////////////////
/// CUdpListenerThreadPool - The UDP server listener thread pool class.

class CUdpListenerThreadPool
{
private:
	CUdpServer *m_pUdpServer;
	CPointerList m_ThreadList;
	int m_nMaxThreadCount;
	CCriticalSection m_Lock;
public:
	explicit CUdpListenerThreadPool(CUdpServer *pUdpServer);
	virtual ~CUdpListenerThreadPool();

	void RegisterThread(CUdpListenerThread *pThread);
	void UnregisterThread(CUdpListenerThread *pThread);

	void StartThreads();
	void StopThreads();

	int GetMaxThreadCount() { return m_nMaxThreadCount; }
	void SetMaxThreadCount(int nValue) { m_nMaxThreadCount = nValue; }

	CUdpServer& GetUdpServer() { return *m_pUdpServer; }
};

///////////////////////////////////////////////////////////////////////////////
/// CTcpListenerThread - The TCP listener thread class.

class CTcpListenerThread : public CListenerThread
{
private:
	CTcpServer *m_pTcpServer;
protected:
	virtual void Execute();
public:
	explicit CTcpListenerThread(CTcpServer *pTcpServer);
};

///////////////////////////////////////////////////////////////////////////////
/// CTcpConnectorPool - The TCP connector pool.

class CTcpConnectorPool
{
public:
	struct TASK_ITEM
	{
		CTcpClient *pTcpClient;            // CTcpClient object
		CString strConnectToHost;          // The host to connect
		int nConnectToPort;                // The port number
		int nTimeOutMSecs;                 // The connect timeout (ms, -1 is infinite)
		UINT nStartTicks;                  // Timestamp of task added
		int nConnectState;                 // The connect state (enum ASYNC_CONNECT_STATE)
		CCallBackDef<TCPCP_ON_RESULT_PROC> OnResult;  // The callback
	};

public:
	class CWorkerThread : public CThread
	{
	private:
		CTcpConnectorPool& m_Owner;
	protected:
		virtual void Execute() { m_Owner.Process(this); }
	public:
		CWorkerThread(CTcpConnectorPool& Owner) : m_Owner(Owner) {}
	};

	friend class CWorkerThread;

private:
	typedef CObjectList<TASK_ITEM> TASK_LIST;

	TASK_LIST m_TaskList;
	TASK_LIST m_AddList;
	CCriticalSection m_Lock;
	CWorkerThread *m_pWorkerThread;
private:
	void Process(CWorkerThread *pThread);
	void ProcessAddList();
	void ProcessConnect();
	void ProcessResult();

	TASK_ITEM* FindTask(CTcpClient *pTcpClient);
private:
	CTcpConnectorPool();
	static std::auto_ptr<CTcpConnectorPool> s_pSingleton;
public:
	virtual ~CTcpConnectorPool();
	static CTcpConnectorPool& Instance();
	static void Delete();

	void Start();
	void Stop();

	bool AddTask(CTcpClient *pTcpClient, LPCTSTR lpszHost, int nPort,
		TCPCP_ON_RESULT_PROC pOnResultProc, void *pProcParam = NULL,
		int nTimeOutMSecs = -1);
	bool RemoveTask(CTcpClient *pTcpClient);
	void RemoveTasks(TCPCP_ON_RESULT_PROC pOnResultProc);
	void Clear();

	int GetTaskCount() const { return m_TaskList.GetCount(); }
};

///////////////////////////////////////////////////////////////////////////////

/// @}

} // namespace ifc
