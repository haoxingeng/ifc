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

/// @file ifc_socket.cpp

#include "stdafx.h"
#include "ifc_socket.h"
#include "ifc_errmsgs.h"
#include "ifc_sysutils.h"

#pragma comment(lib, "ws2_32.lib")

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

int IfcSocketGetLastError();
CString IfcSocketGetErrorMsg(int nError);
CString IfcSocketGetLastErrMsg();
void IfcCloseSocket(int nHandle);

//-----------------------------------------------------------------------------

static int s_nNetworkInitCount = 0;

//-----------------------------------------------------------------------------

void NetworkInitialize()
{
	s_nNetworkInitCount++;
	if (s_nNetworkInitCount > 1) return;

	WSAData Wsd;
	if (WSAStartup(MAKEWORD(2, 2), &Wsd) != 0)
	{
		s_nNetworkInitCount--;
		IfcThrowSocketLastError();
	}
}

//-----------------------------------------------------------------------------

void NetworkFinalize()
{
	if (s_nNetworkInitCount > 0)
		s_nNetworkInitCount--;
	if (s_nNetworkInitCount != 0) return;

	WSACleanup();
}

//-----------------------------------------------------------------------------

bool IsNetworkInited()
{
	return (s_nNetworkInitCount > 0);
}

//-----------------------------------------------------------------------------

void EnsureNetworkInited()
{
	if (!IsNetworkInited())
		NetworkInitialize();
}

//-----------------------------------------------------------------------------

int IfcSocketGetLastError()
{
	return WSAGetLastError();
}

//-----------------------------------------------------------------------------

CString IfcSocketGetErrorMsg(int nError)
{
	CString strResult;
	const TCHAR *p = TEXT("");

	switch (nError)
	{
	case SS_NOTINITIALISED:     p = SSEM_NOTINITIALISED;       break;
	case SS_EINTR:              p = SSEM_EINTR;                break;
	case SS_EBADF:              p = SSEM_EBADF;                break;
	case SS_EACCES:             p = SSEM_EACCES;               break;
	case SS_EFAULT:             p = SSEM_EFAULT;               break;
	case SS_EINVAL:             p = SSEM_EINVAL;               break;
	case SS_EMFILE:             p = SSEM_EMFILE;               break;

	case SS_EWOULDBLOCK:        p = SSEM_EWOULDBLOCK;          break;
	case SS_EINPROGRESS:        p = SSEM_EINPROGRESS;          break;
	case SS_EALREADY:           p = SSEM_EALREADY;             break;
	case SS_ENOTSOCK:           p = SSEM_ENOTSOCK;             break;
	case SS_EDESTADDRREQ:       p = SSEM_EDESTADDRREQ;         break;
	case SS_EMSGSIZE:           p = SSEM_EMSGSIZE;             break;
	case SS_EPROTOTYPE:         p = SSEM_EPROTOTYPE;           break;
	case SS_ENOPROTOOPT:        p = SSEM_ENOPROTOOPT;          break;
	case SS_EPROTONOSUPPORT:    p = SSEM_EPROTONOSUPPORT;      break;
	case SS_ESOCKTNOSUPPORT:    p = SSEM_ESOCKTNOSUPPORT;      break;
	case SS_EOPNOTSUPP:         p = SSEM_EOPNOTSUPP;           break;
	case SS_EPFNOSUPPORT:       p = SSEM_EPFNOSUPPORT;         break;
	case SS_EAFNOSUPPORT:       p = SSEM_EAFNOSUPPORT;         break;
	case SS_EADDRINUSE:         p = SSEM_EADDRINUSE;           break;
	case SS_EADDRNOTAVAIL:      p = SSEM_EADDRNOTAVAIL;        break;
	case SS_ENETDOWN:           p = SSEM_ENETDOWN;             break;
	case SS_ENETUNREACH:        p = SSEM_ENETUNREACH;          break;
	case SS_ENETRESET:          p = SSEM_ENETRESET;            break;
	case SS_ECONNABORTED:       p = SSEM_ECONNABORTED;         break;
	case SS_ECONNRESET:         p = SSEM_ECONNRESET;           break;
	case SS_ENOBUFS:            p = SSEM_ENOBUFS;              break;
	case SS_EISCONN:            p = SSEM_EISCONN;              break;
	case SS_ENOTCONN:           p = SSEM_ENOTCONN;             break;
	case SS_ESHUTDOWN:          p = SSEM_ESHUTDOWN;            break;
	case SS_ETOOMANYREFS:       p = SSEM_ETOOMANYREFS;         break;
	case SS_ETIMEDOUT:          p = SSEM_ETIMEDOUT;            break;
	case SS_ECONNREFUSED:       p = SSEM_ECONNREFUSED;         break;
	case SS_ELOOP:              p = SSEM_ELOOP;                break;
	case SS_ENAMETOOLONG:       p = SSEM_ENAMETOOLONG;         break;
	case SS_EHOSTDOWN:          p = SSEM_EHOSTDOWN;            break;
	case SS_EHOSTUNREACH:       p = SSEM_EHOSTUNREACH;         break;
	case SS_ENOTEMPTY:          p = SSEM_ENOTEMPTY;            break;
	}

	strResult.Format(SSEM_ERROR, nError, p);
	return strResult;
}

//-----------------------------------------------------------------------------

CString IfcSocketGetLastErrMsg()
{
	return IfcSocketGetErrorMsg(IfcSocketGetLastError());
}

//-----------------------------------------------------------------------------

void IfcCloseSocket(SOCKET nHandle)
{
	closesocket(nHandle);
}

//-----------------------------------------------------------------------------

CString IpToString(DWORD nIp)
{
#pragma pack(1)
	union CIpUnion
	{
		DWORD nValue;
		struct
		{
			unsigned char ch1;
			unsigned char ch2;
			unsigned char ch3;
			unsigned char ch4;
		} Bytes;
	} IpUnion;
#pragma pack()
	CString strResult;

	IpUnion.nValue = nIp;
	strResult.Format(TEXT("%u.%u.%u.%u"),
		IpUnion.Bytes.ch4, IpUnion.Bytes.ch3, IpUnion.Bytes.ch2, IpUnion.Bytes.ch1);

	return strResult;
}

//-----------------------------------------------------------------------------

DWORD StringToIp(LPCTSTR lpszString)
{
#pragma pack(1)
	union CIpUnion
	{
		DWORD nValue;
		struct
		{
			unsigned char ch1;
			unsigned char ch2;
			unsigned char ch3;
			unsigned char ch4;
		} Bytes;
	} IpUnion;
#pragma pack()

	CStrList StrList;
	std::vector<int> IntList;

	SplitString(lpszString, '.', StrList);
	for (int i = 0; i < StrList.GetCount(); i++)
		IntList.push_back(StrToInt(StrList[i], 0));

	if (IntList.size() == 4)
	{
		IpUnion.Bytes.ch1 = IntList[3];
		IpUnion.Bytes.ch2 = IntList[2];
		IpUnion.Bytes.ch3 = IntList[1];
		IpUnion.Bytes.ch4 = IntList[0];
		return IpUnion.nValue;
	}
	else
		return 0;
}

//-----------------------------------------------------------------------------

void GetSocketAddr(SOCK_ADDR& SockAddr, DWORD nIpHostValue, int nPort)
{
	memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = htonl(nIpHostValue);
	SockAddr.sin_port = htons(nPort);
}

//-----------------------------------------------------------------------------

int GetFreePort(NET_PROTO_TYPE nProto, int nStartPort, int nCheckTimes)
{
	SOCKET s;
	int i, nResult = 0;
	bool bSuccess;
	SOCK_ADDR Addr;

	NetworkInitialize();
	struct CAutoFinalizer {
		~CAutoFinalizer() { NetworkFinalize(); }
	} AutoFinalizer;

	s = socket(PF_INET, (nProto == NPT_UDP? SOCK_DGRAM : SOCK_STREAM), IPPROTO_IP);
	if (s == INVALID_SOCKET) return 0;

	bSuccess = false;
	for (i = 0; i < nCheckTimes; i++)
	{
		nResult = nStartPort + i;
		GetSocketAddr(Addr, ntohl(INADDR_ANY), nResult);
		if (bind(s, (struct sockaddr*)&Addr, sizeof(Addr)) != -1)
		{
			bSuccess = true;
			break;
		}
	}

	IfcCloseSocket(s);
	if (!bSuccess) nResult = 0;
	return nResult;
}

//-----------------------------------------------------------------------------

void GetLocalIpList(CStrList& IpList)
{
	char sHostName[250];
	hostent *pHostEnt;
	in_addr **addr_ptr;

	IpList.Clear();
	gethostname(sHostName, sizeof(sHostName));
	pHostEnt = gethostbyname(sHostName);
	if (pHostEnt)
	{
		addr_ptr = (in_addr**)(pHostEnt->h_addr_list);
		int i = 0;
		while (addr_ptr[i])
		{
			DWORD nIp = ntohl( *(DWORD*)(addr_ptr[i]) );
			IpList.Add(IpToString(nIp));
			i++;
		}
	}
}

//-----------------------------------------------------------------------------

CString GetLocalIp()
{
	CStrList IpList;
	CString strResult;

	GetLocalIpList(IpList);
	if (IpList.GetCount() > 0)
	{
		if (IpList.GetCount() == 1)
			strResult = IpList[0];
		else
		{
			for (int i = 0; i < IpList.GetCount(); i++)
				if (IpList[i] != TEXT("127.0.0.1"))
				{
					strResult = IpList[i];
					break;
				}

				if (strResult.IsEmpty())
				strResult = IpList[0];
		}
	}

	return strResult;
}

//-----------------------------------------------------------------------------

CString LookupHostAddr(LPCTSTR lpszHost)
{
	CString strResult;
	CStringA strHost(lpszHost);

	struct hostent* pHost = gethostbyname(strHost);
	if (pHost != NULL)
		strResult = IpToString(ntohl(((struct in_addr *)pHost->h_addr)->s_addr));

	return strResult;
}

//-----------------------------------------------------------------------------

void IfcThrowSocketLastError()
{
	IfcThrowSocketException(IfcSocketGetLastErrMsg());
}

//-----------------------------------------------------------------------------

CTcpConnectorPool& GetTcpConnectorPoolObject()
{
	return CTcpConnectorPool::Instance();
}

//-----------------------------------------------------------------------------

void DeleteTcpConnectorPoolObject()
{
	CTcpConnectorPool::Delete();
}

///////////////////////////////////////////////////////////////////////////////
// CIfcSocket

CIfcSocket::CIfcSocket() :
	m_bActive(false),
	m_nHandle(INVALID_SOCKET),
	m_nDomain(PF_INET),
	m_nType(SOCK_STREAM),
	m_nProtocol(IPPROTO_IP),
	m_bBlockMode(true)
{
	// nothing
}

//-----------------------------------------------------------------------------

CIfcSocket::~CIfcSocket()
{
	Close();
}

//-----------------------------------------------------------------------------

void CIfcSocket::DoSetBlockMode(SOCKET nHandle, bool bValue)
{
	DWORD nNotBlock = (bValue? 0 : 1);
	if (ioctlsocket(nHandle, FIONBIO, (u_long*)&nNotBlock) < 0)
		IfcThrowSocketLastError();
}

//-----------------------------------------------------------------------------

void CIfcSocket::DoClose()
{
	shutdown(m_nHandle, SS_SD_BOTH);
	IfcCloseSocket(m_nHandle);
	m_nHandle = INVALID_SOCKET;
	m_bActive = false;
}

//-----------------------------------------------------------------------------

void CIfcSocket::SetActive(bool bValue)
{
	if (bValue != m_bActive)
	{
		if (bValue) Open();
		else Close();
	}
}

//-----------------------------------------------------------------------------

void CIfcSocket::SetDomain(int nValue)
{
	if (nValue != m_nDomain)
	{
		if (GetActive()) Close();
		m_nDomain = nValue;
	}
}

//-----------------------------------------------------------------------------

void CIfcSocket::SetType(int nValue)
{
	if (nValue != m_nType)
	{
		if (GetActive()) Close();
		m_nType = nValue;
	}
}

//-----------------------------------------------------------------------------

void CIfcSocket::SetProtocol(int nValue)
{
	if (nValue != m_nProtocol)
	{
		if (GetActive()) Close();
		m_nProtocol = nValue;
	}
}

//-----------------------------------------------------------------------------

void CIfcSocket::SetBlockMode(bool bValue)
{
	if (m_bActive)
		DoSetBlockMode(m_nHandle, bValue);
	m_bBlockMode = bValue;
}

//-----------------------------------------------------------------------------

void CIfcSocket::SetHandle(SOCKET nValue)
{
	if (nValue != m_nHandle)
	{
		if (GetActive()) Close();
		m_nHandle = nValue;
		if (m_nHandle != INVALID_SOCKET)
			m_bActive = true;
	}
}

//-----------------------------------------------------------------------------

void CIfcSocket::Bind(int nPort, bool bForce)
{
	SOCK_ADDR Addr;
	int nValue = 1;

	GetSocketAddr(Addr, ntohl(INADDR_ANY), nPort);

	// Force bind the socket
	if (bForce)
		setsockopt(m_nHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&nValue, sizeof(int));

	if (bind(m_nHandle, (struct sockaddr*)&Addr, sizeof(Addr)) < 0)
		IfcThrowSocketLastError();
}

//-----------------------------------------------------------------------------

void CIfcSocket::Open()
{
	if (!m_bActive)
	{
		try
		{
			SOCKET nHandle;
			nHandle = socket(m_nDomain, m_nType, m_nProtocol);
			if (nHandle == INVALID_SOCKET)
				IfcThrowSocketLastError();
			m_bActive = (nHandle != INVALID_SOCKET);
			if (m_bActive)
			{
				m_nHandle = nHandle;
				SetBlockMode(m_bBlockMode);
			}
		}
		catch (...)
		{
			DoClose();
			throw;
		}
	}
}

//-----------------------------------------------------------------------------

void CIfcSocket::Close()
{
	if (m_bActive)
		DoClose();
}

///////////////////////////////////////////////////////////////////////////////
// CUdpSocket

int CUdpSocket::RecvBuffer(void *pBuffer, int nSize)
{
	CPeerAddress PeerAddr;
	return RecvBuffer(pBuffer, nSize, PeerAddr);
}

//-----------------------------------------------------------------------------

int CUdpSocket::RecvBuffer(void *pBuffer, int nSize, CPeerAddress& PeerAddr)
{
	SOCK_ADDR Addr;
	int nBytes;
	socklen_t nSockLen = sizeof(Addr);

	memset(&Addr, 0, sizeof(Addr));
	nBytes = recvfrom(m_nHandle, (char*)pBuffer, nSize, 0, (struct sockaddr*)&Addr, &nSockLen);

	if (nBytes > 0)
	{
		PeerAddr.nIp = ntohl(Addr.sin_addr.s_addr);
		PeerAddr.nPort = ntohs(Addr.sin_port);
	}

	return nBytes;
}

//-----------------------------------------------------------------------------

int CUdpSocket::SendBuffer(void *pBuffer, int nSize, const CPeerAddress& PeerAddr, int nSendTimes)
{
	int nResult = 0;
	SOCK_ADDR Addr;
	socklen_t nSockLen = sizeof(Addr);

	GetSocketAddr(Addr, PeerAddr.nIp, PeerAddr.nPort);

	for (int i = 0; i < nSendTimes; i++)
		nResult = sendto(m_nHandle, (char*)pBuffer, nSize, 0, (struct sockaddr*)&Addr, nSockLen);

	return nResult;
}

//-----------------------------------------------------------------------------

void CUdpSocket::Open()
{
	CIfcSocket::Open();

	if (m_bActive)
	{
		// Under Windows, after receives ICMP packet "ICMP port unreachable",
		// subsequent call to recvfrom returns -1, the error code is WSAECONNRESET(10054).
		// The codes below fixed the bug.

		#define IOC_VENDOR        0x18000000
		#define _WSAIOW(x,y)      (IOC_IN|(x)|(y))
		#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)

		DWORD dwBytesReturned = 0;
		BOOL bNewBehavior = FALSE;
		::WSAIoctl(GetHandle(), SIO_UDP_CONNRESET, &bNewBehavior, sizeof(bNewBehavior),
			NULL, 0, &dwBytesReturned, NULL, NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CUdpServer

CUdpServer::CUdpServer() :
	m_nLocalPort(0),
	m_bForceBind(false),
	m_pListenerThreadPool(NULL)
{
	m_pListenerThreadPool = new CUdpListenerThreadPool(this);
	SetListenerThreadCount(1);
}

//-----------------------------------------------------------------------------

CUdpServer::~CUdpServer()
{
	delete m_pListenerThreadPool;
}

//-----------------------------------------------------------------------------

void CUdpServer::DataReceived(void *pPacketBuffer, int nPacketSize, const CPeerAddress& PeerAddr)
{
	if (m_OnRecvData.pProc)
		m_OnRecvData.pProc(m_OnRecvData.pParam, pPacketBuffer, nPacketSize, PeerAddr);
}

//-----------------------------------------------------------------------------

void CUdpServer::StartListenerThreads()
{
	m_pListenerThreadPool->StartThreads();
}

//-----------------------------------------------------------------------------

void CUdpServer::StopListenerThreads()
{
	m_pListenerThreadPool->StopThreads();
}

//-----------------------------------------------------------------------------

void CUdpServer::SetLocalPort(int nValue, bool bForceBind)
{
	if (nValue != m_nLocalPort)
	{
		if (GetActive()) Close();
		m_nLocalPort = nValue;
	}

	m_bForceBind = bForceBind;
}

//-----------------------------------------------------------------------------

void CUdpServer::Open()
{
	try
	{
		if (!m_bActive)
		{
			CUdpSocket::Open();
			if (m_bActive)
			{
				Bind(m_nLocalPort, m_bForceBind);
				StartListenerThreads();
			}
		}
	}
	catch (...)
	{
		Close();
		throw;
	}
}

//-----------------------------------------------------------------------------

void CUdpServer::Close()
{
	if (GetActive())
	{
		StopListenerThreads();
		CUdpSocket::Close();
	}
}

//-----------------------------------------------------------------------------

int CUdpServer::GetListenerThreadCount()
{
	return m_pListenerThreadPool->GetMaxThreadCount();
}

//-----------------------------------------------------------------------------

void CUdpServer::SetListenerThreadCount(int nValue)
{
	if (nValue < 1) nValue = 1;
	m_pListenerThreadPool->SetMaxThreadCount(nValue);
}

//-----------------------------------------------------------------------------

void CUdpServer::SetOnRecvDataCallBack(UDPSVR_ON_RECV_DATA_PROC pProc, void *pParam)
{
	m_OnRecvData.pProc = pProc;
	m_OnRecvData.pParam = pParam;
}

///////////////////////////////////////////////////////////////////////////////
// CDtpConnection

long CDtpConnection::s_nObjectCount = 0;

//-----------------------------------------------------------------------------

CDtpConnection::CDtpConnection(DTP_PROTO_TYPE nConnType) :
	m_nConnType(nConnType),
	m_pCustomData(NULL)
{
	InterlockedIncrement(&s_nObjectCount);
}

CDtpConnection::~CDtpConnection()
{
	InterlockedDecrement(&s_nObjectCount);
}

//-----------------------------------------------------------------------------

void CDtpConnection::Disconnect()
{
	if (IsConnected())
		DoDisconnect();
}

//-----------------------------------------------------------------------------

int CDtpConnection::SendBuffer(void *pBuffer, int nSize, bool bSyncMode, int nTimeOutMSecs)
{
	int nResult = nSize;

	if (bSyncMode)
		nResult = DoSyncSendBuffer(pBuffer, nSize, nTimeOutMSecs);
	else
		nResult = DoAsyncSendBuffer(pBuffer, nSize);

	return nResult;
}

//-----------------------------------------------------------------------------

int CDtpConnection::RecvBuffer(void *pBuffer, int nSize, bool bSyncMode, int nTimeOutMSecs)
{
	int nResult = nSize;

	if (bSyncMode)
		nResult = DoSyncRecvBuffer(pBuffer, nSize, nTimeOutMSecs);
	else
		nResult = DoAsyncRecvBuffer(pBuffer, nSize);

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// CTcpConnection

CTcpConnection::CTcpConnection() :
	CDtpConnection(DPT_TCP)
{
	m_Socket.SetBlockMode(false);
}

//-----------------------------------------------------------------------------

CTcpConnection::CTcpConnection(SOCKET nSocketHandle, const CPeerAddress& PeerAddr) :
	CDtpConnection(DPT_TCP)
{
	m_Socket.SetHandle(nSocketHandle);
	m_Socket.SetBlockMode(false);
	m_PeerAddr = PeerAddr;
}

//-----------------------------------------------------------------------------

CTcpConnection::~CTcpConnection()
{
	// nothing
}

//-----------------------------------------------------------------------------

int CTcpConnection::DoSyncSendBuffer(void *pBuffer, int nSize, int nTimeOutMSecs)
{
	const int SELECT_WAIT_MSEC = 250;

	int nResult = -1;
	bool bError = false;
	fd_set fds;
	struct timeval tv;
	SOCKET nSocketHandle = m_Socket.GetHandle();
	int n, r, nRemainSize, nIndex;
	UINT nStartTime, nElapsedMSecs;

	if (nSize <= 0 || !m_Socket.GetActive())
		return nResult;

	nRemainSize = nSize;
	nIndex = 0;
	nStartTime = GetTickCount();

	while (m_Socket.GetActive() && nRemainSize > 0)
	try
	{
		tv.tv_sec = 0;
		tv.tv_usec = (nTimeOutMSecs? SELECT_WAIT_MSEC * 1000 : 0);

		FD_ZERO(&fds);
		FD_SET((DWORD)nSocketHandle, &fds);

		r = select(0, NULL, &fds, NULL, &tv);
		if (r < 0)
		{
			if (IfcSocketGetLastError() != SS_EINTR)
			{
				bError = true;    // error
				break;
			}
		}

		if (r > 0 && m_Socket.GetActive() && FD_ISSET(nSocketHandle, &fds))
		{
			n = send(nSocketHandle, &((char*)pBuffer)[nIndex], nRemainSize, 0);
			if (n <= 0)
			{
				int nError = IfcSocketGetLastError();
				if ((n == 0) || (nError != SS_EWOULDBLOCK && nError != SS_EINTR))
				{
					bError = true;    // error
					Disconnect();
					break;
				}
				else
					n = 0;
			}

			nIndex += n;
			nRemainSize -= n;
		}

		// If need to check timeout
		if (nTimeOutMSecs >= 0 && nRemainSize > 0)
		{
			nElapsedMSecs = GetTickDiff(nStartTime, GetTickCount());
			if (nElapsedMSecs >= (UINT)nTimeOutMSecs)
				break;
		}
	}
	catch (CException* e)
	{
		e->Delete();
		bError = true;
		break;
	}

	if (nIndex > 0)
		nResult = nIndex;
	else if (bError)
		nResult = -1;
	else
		nResult = 0;

	return nResult;
}

//-----------------------------------------------------------------------------

int CTcpConnection::DoSyncRecvBuffer(void *pBuffer, int nSize, int nTimeOutMSecs)
{
	const int SELECT_WAIT_MSEC = 250;

	int nResult = -1;
	bool bError = false;
	fd_set fds;
	struct timeval tv;
	SOCKET nSocketHandle = m_Socket.GetHandle();
	int n, r, nRemainSize, nIndex;
	UINT nStartTime, nElapsedMSecs;

	if (nSize <= 0 || !m_Socket.GetActive())
		return nResult;

	nRemainSize = nSize;
	nIndex = 0;
	nStartTime = GetTickCount();

	while (m_Socket.GetActive() && nRemainSize > 0)
	try
	{
		tv.tv_sec = 0;
		tv.tv_usec = (nTimeOutMSecs? SELECT_WAIT_MSEC * 1000 : 0);

		FD_ZERO(&fds);
		FD_SET((DWORD)nSocketHandle, &fds);

		r = select(0, &fds, NULL, NULL, &tv);
		if (r < 0)
		{
			if (IfcSocketGetLastError() != SS_EINTR)
			{
				bError = true;    // error
				break;
			}
		}

		if (r > 0 && m_Socket.GetActive() && FD_ISSET(nSocketHandle, &fds))
		{
			n = recv(nSocketHandle, &((char*)pBuffer)[nIndex], nRemainSize, 0);
			if (n <= 0)
			{
				int nError = IfcSocketGetLastError();
				if ((n == 0) || (nError != SS_EWOULDBLOCK && nError != SS_EINTR))
				{
					bError = true;    // error
					Disconnect();
					break;
				}
				else
					n = 0;
			}

			nIndex += n;
			nRemainSize -= n;
		}

		// If need to check timeout
		if (nTimeOutMSecs >= 0 && nRemainSize > 0)
		{
			nElapsedMSecs = GetTickDiff(nStartTime, GetTickCount());
			if (nElapsedMSecs >= (UINT)nTimeOutMSecs)
				break;
		}
	}
	catch (CException* e)
	{
		e->Delete();
		bError = true;
		break;
	}

	if (nIndex > 0)
		nResult = nIndex;
	else if (bError)
		nResult = -1;
	else
		nResult = 0;

	return nResult;
}

//-----------------------------------------------------------------------------

int CTcpConnection::DoAsyncSendBuffer(void *pBuffer, int nSize)
{
	int nResult = -1;
	try
	{
		nResult = send(m_Socket.GetHandle(), (char*)pBuffer, nSize, 0);
		if (nResult <= 0)
		{
			int nError = IfcSocketGetLastError();
			if ((nResult == 0) || (nError != SS_EWOULDBLOCK && nError != SS_EINTR))
			{
				Disconnect();    // error
				nResult = -1;
			}
			else
				nResult = 0;
		}
	}
	catch (...)
	{}

	return nResult;
}

//-----------------------------------------------------------------------------

int CTcpConnection::DoAsyncRecvBuffer(void *pBuffer, int nSize)
{
	int nResult = -1;
	try
	{
		nResult = recv(m_Socket.GetHandle(), (char*)pBuffer, nSize, 0);
		if (nResult <= 0)
		{
			int nError = IfcSocketGetLastError();
			if ((nResult == 0) || (nError != SS_EWOULDBLOCK && nError != SS_EINTR))
			{
				Disconnect();    // error
				nResult = -1;
			}
			else
				nResult = 0;
		}
	}
	catch (...)
	{}

	return nResult;
}

//-----------------------------------------------------------------------------

void CTcpConnection::DoDisconnect()
{
	m_Socket.Close();
}

//-----------------------------------------------------------------------------

bool CTcpConnection::IsConnected()
{
	return m_Socket.GetActive();
}

///////////////////////////////////////////////////////////////////////////////
// CTcpClient

void CTcpClient::Connect(const CString& strIp, int nPort)
{
	if (IsConnected()) Disconnect();

	try
	{
		m_Socket.Open();
		if (m_Socket.GetActive())
		{
			SOCK_ADDR Addr;

			GetSocketAddr(Addr, StringToIp(strIp), nPort);

			bool bOldBlockMode = m_Socket.GetBlockMode();
			m_Socket.SetBlockMode(true);

			if (connect(m_Socket.GetHandle(), (struct sockaddr*)&Addr, sizeof(Addr)) < 0)
				IfcThrowSocketLastError();

			m_Socket.SetBlockMode(bOldBlockMode);
			m_PeerAddr = CPeerAddress(ntohl(Addr.sin_addr.s_addr), nPort);
		}
	}
	catch (...)
	{
		m_Socket.Close();
		throw;
	}
}

//-----------------------------------------------------------------------------

int CTcpClient::AsyncConnect(const CString& strIp, int nPort, int nTimeOutMSecs)
{
	int nResult = ACS_CONNECTING;

	if (IsConnected()) Disconnect();
	try
	{
		m_Socket.Open();
		if (m_Socket.GetActive())
		{
			SOCK_ADDR Addr;
			int r;

			GetSocketAddr(Addr, StringToIp(strIp), nPort);
			m_Socket.SetBlockMode(false);
			r = connect(m_Socket.GetHandle(), (struct sockaddr*)&Addr, sizeof(Addr));
			if (r == 0)
				nResult = ACS_CONNECTED;
			else if (IfcSocketGetLastError() != SS_EWOULDBLOCK)
				nResult = ACS_FAILED;

			m_PeerAddr = CPeerAddress(ntohl(Addr.sin_addr.s_addr), nPort);
		}
	}
	catch (CException* e)
	{
		e->Delete();
		m_Socket.Close();
		nResult = ACS_FAILED;
	}

	if (nResult == ACS_CONNECTING)
		nResult = CheckAsyncConnectState(nTimeOutMSecs);

	return nResult;
}

//-----------------------------------------------------------------------------

int CTcpClient::CheckAsyncConnectState(int nTimeOutMSecs)
{
	if (!m_Socket.GetActive()) return ACS_FAILED;

	const int WAIT_STEP = 100;   // ms
	int nResult = ACS_CONNECTING;
	SOCKET nHandle = GetSocket().GetHandle();
	fd_set rset, wset;
	struct timeval tv;
	int ms = 0;

	nTimeOutMSecs = Max(nTimeOutMSecs, -1);

	while (true)
	{
		tv.tv_sec = 0;
		tv.tv_usec = (nTimeOutMSecs? WAIT_STEP * 1000 : 0);
		FD_ZERO(&rset);
		FD_SET((DWORD)nHandle, &rset);
		wset = rset;

		int r = select(0, &rset, &wset, NULL, &tv);
		if (r > 0 && (FD_ISSET(nHandle, &rset) || FD_ISSET(nHandle, &wset)))
		{
			socklen_t nErrLen = sizeof(int);
			int nError = 0;
			// If error occurs
			if (getsockopt(nHandle, SOL_SOCKET, SO_ERROR, (char*)&nError, &nErrLen) < 0 || nError)
				nResult = ACS_FAILED;
			else
				nResult = ACS_CONNECTED;
		}
		else if (r < 0)
		{
			if (IfcSocketGetLastError() != SS_EINTR)
				nResult = ACS_FAILED;
		}

		if (nResult != ACS_CONNECTING)
			break;

		// Check timeout
		if (nTimeOutMSecs != -1)
		{
			ms += WAIT_STEP;
			if (ms >= nTimeOutMSecs)
				break;
		}
	}

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// CTcpServer

CTcpServer::CTcpServer() :
	m_nLocalPort(0),
	m_bForceBind(false),
	m_pListenerThread(NULL)
{
	// nothing
}

//-----------------------------------------------------------------------------

CTcpServer::~CTcpServer()
{
	Close();
}

//-----------------------------------------------------------------------------

CTcpConnection* CTcpServer::CreateConnection(SOCKET nSocketHandle, const CPeerAddress& PeerAddr)
{
	CTcpConnection *pResult = NULL;

	if (m_OnCreateConn.pProc)
		m_OnCreateConn.pProc(m_OnCreateConn.pParam, nSocketHandle, PeerAddr, pResult);

	if (pResult == NULL)
		pResult = new CTcpConnection(nSocketHandle, PeerAddr);

	return pResult;
}

//-----------------------------------------------------------------------------

void CTcpServer::AcceptConnection(CTcpConnection *pConnection)
{
	if (m_OnAcceptConn.pProc)
		m_OnAcceptConn.pProc(m_OnAcceptConn.pParam, pConnection);
	else
		delete pConnection;
}

//-----------------------------------------------------------------------------

void CTcpServer::StartListenerThread()
{
	if (!m_pListenerThread)
	{
		m_pListenerThread = new CTcpListenerThread(this);
		m_pListenerThread->Run();
	}
}

//-----------------------------------------------------------------------------

void CTcpServer::StopListenerThread()
{
	if (m_pListenerThread)
	{
		m_pListenerThread->Terminate();
		m_pListenerThread->WaitFor();
		delete m_pListenerThread;
		m_pListenerThread = NULL;
	}
}

//-----------------------------------------------------------------------------

void CTcpServer::Open()
{
	try
	{
		if (!GetActive())
		{
			m_Socket.Open();
			m_Socket.Bind(m_nLocalPort, m_bForceBind);
			if (listen(m_Socket.GetHandle(), LISTEN_QUEUE_SIZE) < 0)
				IfcThrowSocketLastError();
			StartListenerThread();
		}
	}
	catch (...)
	{
		Close();
		throw;
	}
}

//-----------------------------------------------------------------------------

void CTcpServer::Close()
{
	if (GetActive())
	{
		StopListenerThread();
		m_Socket.Close();
	}
}

//-----------------------------------------------------------------------------

void CTcpServer::SetActive(bool bValue)
{
	if (GetActive() != bValue)
	{
		if (bValue) Open();
		else Close();
	}
}

//-----------------------------------------------------------------------------

void CTcpServer::SetLocalPort(int nValue, bool bForceBind)
{
	if (nValue != m_nLocalPort)
	{
		if (GetActive()) Close();
		m_nLocalPort = nValue;
	}

	m_bForceBind = bForceBind;
}

//-----------------------------------------------------------------------------

void CTcpServer::SetOnCreateConnCallBack(TCPSVR_ON_CREATE_CONN_PROC pProc, void *pParam)
{
	m_OnCreateConn.pProc = pProc;
	m_OnCreateConn.pParam = pParam;
}

//-----------------------------------------------------------------------------

void CTcpServer::SetOnAcceptConnCallBack(TCPSVR_ON_ACCEPT_CONN_PROC pProc, void *pParam)
{
	m_OnAcceptConn.pProc = pProc;
	m_OnAcceptConn.pParam = pParam;
}

///////////////////////////////////////////////////////////////////////////////
// CUdpListenerThread

CUdpListenerThread::CUdpListenerThread(CUdpListenerThreadPool *pThreadPool, int nIndex) :
	m_pThreadPool(pThreadPool),
	m_nIndex(nIndex)
{
	SetFreeOnTerminate(true);
	m_pUdpServer = &(pThreadPool->GetUdpServer());
	m_pThreadPool->RegisterThread(this);
}

//-----------------------------------------------------------------------------

CUdpListenerThread::~CUdpListenerThread()
{
	m_pThreadPool->UnregisterThread(this);
}

//-----------------------------------------------------------------------------

void CUdpListenerThread::Execute()
{
	const int MAX_UDP_BUFFER_SIZE = 8192;   // The max size of UDP packet (bytes)
	const int SELECT_WAIT_MSEC    = 100;    // The waiting interval (ms)

	fd_set fds;
	struct timeval tv;
	SOCKET nSocketHandle = m_pUdpServer->GetHandle();
	CBuffer PacketBuffer(MAX_UDP_BUFFER_SIZE);
	CPeerAddress PeerAddr;
	int r, n;

	while (!GetTerminated() && m_pUdpServer->GetActive())
	try
	{
		tv.tv_sec = 0;
		tv.tv_usec = SELECT_WAIT_MSEC * 1000;

		FD_ZERO(&fds);
		FD_SET((DWORD)nSocketHandle, &fds);

		r = select(0, &fds, NULL, NULL, &tv);

		if (r > 0 && m_pUdpServer->GetActive() && FD_ISSET(nSocketHandle, &fds))
		{
			n = m_pUdpServer->RecvBuffer(PacketBuffer.Data(), MAX_UDP_BUFFER_SIZE, PeerAddr);
			if (n > 0)
			{
				m_pUdpServer->DataReceived(PacketBuffer.Data(), n, PeerAddr);
			}
		}
		else if (r < 0)
		{
			int nError = IfcSocketGetLastError();
			if (nError != SS_EINTR && nError != SS_EINPROGRESS)
				break;  // error
		}
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}
}

///////////////////////////////////////////////////////////////////////////////
// CUdpListenerThreadPool

CUdpListenerThreadPool::CUdpListenerThreadPool(CUdpServer *pUdpServer) :
	m_pUdpServer(pUdpServer),
	m_nMaxThreadCount(0)
{
	// nothing
}

//-----------------------------------------------------------------------------

CUdpListenerThreadPool::~CUdpListenerThreadPool()
{
	// nothing
}

//-----------------------------------------------------------------------------

void CUdpListenerThreadPool::RegisterThread(CUdpListenerThread *pThread)
{
	CAutoLocker Locker(m_Lock);

	if (m_ThreadList.IndexOf(pThread) == -1)
		m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------

void CUdpListenerThreadPool::UnregisterThread(CUdpListenerThread *pThread)
{
	CAutoLocker Locker(m_Lock);

	m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------

void CUdpListenerThreadPool::StartThreads()
{
	for (int i = 0; i < m_nMaxThreadCount; i++)
	{
		CUdpListenerThread *pThread;
		pThread = new CUdpListenerThread(this, i);
		pThread->Run();
	}
}

//-----------------------------------------------------------------------------

void CUdpListenerThreadPool::StopThreads()
{
	const int MAX_THREAD_WAIT_FOR_SECS = 3; // The max waiting seconds of clearing thread pool.
	const double SLEEP_INTERVAL = 0.1;      // The waiting interval (seconds).
	double nWaitSecs = 0;

	// Notify the threads to terminate.
	{
		CAutoLocker Locker(m_Lock);

		for (int i = 0; i < m_ThreadList.GetCount(); i++)
		{
			CUdpListenerThread *pThread;
			pThread = (CUdpListenerThread*)m_ThreadList[i];
			pThread->Terminate();
		}
	}

	// Wait for the threads to terminate.
	while (nWaitSecs < MAX_THREAD_WAIT_FOR_SECS)
	{
		if (m_ThreadList.GetCount() == 0) break;
		Sleep(DWORD(SLEEP_INTERVAL * 1000));
		nWaitSecs += SLEEP_INTERVAL;
	}

	// Wait timeout, kill all threads.
	if (m_ThreadList.GetCount() > 0)
	{
		CAutoLocker Locker(m_Lock);

		for (int i = 0; i < m_ThreadList.GetCount(); i++)
		{
			CUdpListenerThread *pThread;
			pThread = (CUdpListenerThread*)m_ThreadList[i];
			pThread->Kill();
		}

		m_ThreadList.Clear();
	}
}

///////////////////////////////////////////////////////////////////////////////
// CTcpListenerThread

CTcpListenerThread::CTcpListenerThread(CTcpServer *pTcpServer) :
	m_pTcpServer(pTcpServer)
{
	SetFreeOnTerminate(false);
}

//-----------------------------------------------------------------------------

void CTcpListenerThread::Execute()
{
	const int SELECT_WAIT_MSEC = 100;    // The waiting interval (ms).

	fd_set fds;
	struct timeval tv;
	SOCK_ADDR Addr;
	socklen_t nSockLen = sizeof(Addr);
	SOCKET nSocketHandle = m_pTcpServer->GetSocket().GetHandle();
	CPeerAddress PeerAddr;
	int r;

	while (!GetTerminated() && m_pTcpServer->GetActive())
	try
	{
		tv.tv_sec = 0;
		tv.tv_usec = SELECT_WAIT_MSEC * 1000;

		FD_ZERO(&fds);
		FD_SET((DWORD)nSocketHandle, &fds);

		r = select(0, &fds, NULL, NULL, &tv);

		if (r > 0 && m_pTcpServer->GetActive() && FD_ISSET(nSocketHandle, &fds))
		{
			SOCKET nAcceptHandle = accept(nSocketHandle, (struct sockaddr*)&Addr, &nSockLen);
			if (nAcceptHandle != INVALID_SOCKET)
			{
				PeerAddr = CPeerAddress(ntohl(Addr.sin_addr.s_addr), ntohs(Addr.sin_port));
				CTcpConnection *pConnection = m_pTcpServer->CreateConnection(nAcceptHandle, PeerAddr);
				m_pTcpServer->AcceptConnection(pConnection);
			}
		}
		else if (r < 0)
		{
			int nError = IfcSocketGetLastError();
			if (nError != SS_EINTR && nError != SS_EINPROGRESS)
				break;  // error
		}
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}
}

///////////////////////////////////////////////////////////////////////////////
// CTcpConnectorPool

std::auto_ptr<CTcpConnectorPool> CTcpConnectorPool::s_pSingleton(NULL);

//-----------------------------------------------------------------------------

CTcpConnectorPool::CTcpConnectorPool() :
	m_TaskList(false, true),
	m_AddList(true, true),
	m_pWorkerThread(NULL)
{
	// nothing
}

//-----------------------------------------------------------------------------

CTcpConnectorPool::~CTcpConnectorPool()
{
	Stop();
	Clear();
}

//-----------------------------------------------------------------------------

CTcpConnectorPool& CTcpConnectorPool::Instance()
{
	if (s_pSingleton.get() == NULL)
		s_pSingleton.reset(new CTcpConnectorPool());
	return *s_pSingleton;
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::Delete()
{
	s_pSingleton.reset(NULL);
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::Process(CWorkerThread *pThread)
{
	while (!pThread->GetTerminated())
	{
		try
		{
			ProcessAddList();
			ProcessConnect();
			ProcessResult();
		}
		catch (CException* e)
		{
			e->Delete();
		}
		catch (...)
		{}

		Sleep(10);
	}
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::ProcessAddList()
{
	TASK_ITEM *pTask;
	while ((pTask = m_AddList.Extract(0)))
	{
		CAutoLocker Locker(m_Lock);

		pTask->nStartTicks = GetTickCount();
		m_TaskList.Add(pTask);
	}
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::ProcessConnect()
{
	CAutoLocker Locker(m_Lock);

	for (int i = 0; i < m_TaskList.GetCount(); i++)
	{
		TASK_ITEM *pTask = m_TaskList[i];
		if (pTask->nConnectState == ACS_NONE)
		{
			CString strIp = LookupHostAddr(pTask->strConnectToHost);
			if (strIp.IsEmpty())
				pTask->nConnectState = ACS_FAILED;
			else
				pTask->nConnectState = pTask->pTcpClient->AsyncConnect(strIp, pTask->nConnectToPort, 0);
		}
		else if (pTask->nConnectState == ACS_CONNECTING)
		{
			pTask->nConnectState = pTask->pTcpClient->CheckAsyncConnectState(0);
		}
	}
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::ProcessResult()
{
	CAutoLocker Locker(m_Lock);

	for (int i = m_TaskList.GetCount() - 1; i >= 0; i--)
	{
		TASK_ITEM *pTask = m_TaskList[i];
		if (pTask->nConnectState == ACS_CONNECTED ||
			pTask->nConnectState == ACS_FAILED ||
			(pTask->nTimeOutMSecs >= 0 && GetTickDiff(pTask->nStartTicks, GetTickCount()) >= (UINT)pTask->nTimeOutMSecs))
		{
			pTask->OnResult.pProc(pTask->OnResult.pParam, pTask->pTcpClient,
				pTask->nConnectState == ACS_CONNECTED);
			m_TaskList.Delete(i);
		}
	}
}

//-----------------------------------------------------------------------------

CTcpConnectorPool::TASK_ITEM* CTcpConnectorPool::FindTask(CTcpClient *pTcpClient)
{
	TASK_ITEM *pResult = NULL;

	for (int i = 0; i < m_TaskList.GetCount(); i++)
	{
		if (m_TaskList[i]->pTcpClient == pTcpClient)
		{
			pResult = m_TaskList[i];
			break;
		}
	}

	return pResult;
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::Start()
{
	if (!m_pWorkerThread)
	{
		m_pWorkerThread = new CWorkerThread(*this);
		m_pWorkerThread->Run();
	}
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::Stop()
{
	if (m_pWorkerThread)
	{
		m_pWorkerThread->Terminate();
		m_pWorkerThread->WaitFor();
		delete m_pWorkerThread;
		m_pWorkerThread = NULL;
	}
}

//-----------------------------------------------------------------------------

bool CTcpConnectorPool::AddTask(CTcpClient *pTcpClient, LPCTSTR lpszHost, int nPort,
	TCPCP_ON_RESULT_PROC pOnResultProc, void *pProcParam, int nTimeOutMSecs)
{
	bool bResult = (pTcpClient && lpszHost && pOnResultProc);
	if (bResult)
	{
		TASK_ITEM *pTask = new TASK_ITEM();
		pTask->pTcpClient = pTcpClient;
		pTask->strConnectToHost = lpszHost;
		pTask->nConnectToPort = nPort;
		pTask->nTimeOutMSecs = nTimeOutMSecs;
		pTask->nStartTicks = 0;
		pTask->nConnectState = ACS_NONE;
		pTask->OnResult.pProc = pOnResultProc;
		pTask->OnResult.pParam = pProcParam;

		m_AddList.Add(pTask);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CTcpConnectorPool::RemoveTask(CTcpClient *pTcpClient)
{
	CAutoLocker Locker(m_Lock);

	TASK_ITEM *pTask = FindTask(pTcpClient);
	bool bResult = (pTask != NULL);
	if (bResult)
		m_TaskList.Remove(pTask);

	return bResult;
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::RemoveTasks(TCPCP_ON_RESULT_PROC pOnResultProc)
{
	CAutoLocker Locker(m_Lock);

	for (int i = m_TaskList.GetCount() - 1; i >= 0; i--)
	{
		if (m_TaskList[i]->OnResult.pProc == pOnResultProc)
			m_TaskList.Delete(i);
	}
}

//-----------------------------------------------------------------------------

void CTcpConnectorPool::Clear()
{
	CAutoLocker Locker(m_Lock);
	m_TaskList.Clear();
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
