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

/// @file ifc_pipe.cpp

#include "stdafx.h"
#include "ifc_pipe.h"
#include "ifc_sysutils.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeServer

CNamedPipeServer::CNamedPipeServer(bool bUseIocp) :
	m_pListenerThread(NULL),
	m_nMaxConnCount(PIPE_DEF_MAX_CONN_COUNT),
	m_bUseIocp(bUseIocp)
{
	m_pConnList = new CNamedPipeConnList(*this);
}

//-----------------------------------------------------------------------------

CNamedPipeServer::~CNamedPipeServer()
{
	InternalClose();

	delete m_pConnList;
	m_pConnList = NULL;
}

//-----------------------------------------------------------------------------

void CNamedPipeServer::InternalClose()
{
	if (GetActive())
	{
		StopListenerThread();
	}
}

//-----------------------------------------------------------------------------

void CNamedPipeServer::StartListenerThread()
{
	if (!m_pListenerThread)
	{
		m_pListenerThread = new CNamedPipeListenerThread(*this);
		m_pListenerThread->Run();
	}
}

//-----------------------------------------------------------------------------

void CNamedPipeServer::StopListenerThread()
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

bool CNamedPipeServer::IsConnListFull()
{
	return (GetMaxConnCount() != 0 && GetCurConnCount() >= GetMaxConnCount());
}

//-----------------------------------------------------------------------------

bool CNamedPipeServer::NotifyConnect(HANDLE hPipeHandle)
{
	bool bResult = !IsConnListFull();
	if (bResult)
	{
		bResult = false;
		try
		{
			CNamedPipeConnection *pConnection = CreateConnection(hPipeHandle);
			if (pConnection)
			{
				std::auto_ptr<CNamedPipeConnection> AutoPtr(pConnection);
				pConnection->OnConnect();
				AutoPtr.release();

				OnConnect(pConnection);
				bResult = true;
			}
		}
		catch (CException* e)
		{
			e->Delete();
		}
		catch (...)
		{}
	}

	return bResult;
}

//-----------------------------------------------------------------------------

void CNamedPipeServer::IocpCallBackProc(const CIocpTaskData& TaskData, PVOID pParam)
{
	CNamedPipeConnection *pConnection = (CNamedPipeConnection*)TaskData.GetCaller();
	if (pConnection)
		CATCH_ALL_EXCEPTION( pConnection->OnIocpCallBack((CIocpTaskData*)(&TaskData)); );
}

//-----------------------------------------------------------------------------

CNamedPipeConnection* CNamedPipeServer::CreateConnection(HANDLE hPipeHandle)
{
	return new CNamedPipeConnection(*this, hPipeHandle);
}

//-----------------------------------------------------------------------------

void CNamedPipeServer::Open()
{
	if (!GetActive())
		StartListenerThread();
}

//-----------------------------------------------------------------------------

void CNamedPipeServer::Close()
{
	InternalClose();
}

//-----------------------------------------------------------------------------

bool CNamedPipeServer::GetActive() const
{
	return (m_pListenerThread != NULL);
}

//-----------------------------------------------------------------------------

void CNamedPipeServer::SetActive(bool bValue)
{
	if (GetActive() != bValue)
	{
		if (bValue) Open();
		else Close();
	}
}

//-----------------------------------------------------------------------------

void CNamedPipeServer::SetMaxConnCount(int nValue)
{
	nValue = Max(nValue, 0);
	m_nMaxConnCount = nValue;
}

//-----------------------------------------------------------------------------

int CNamedPipeServer::GetCurConnCount() const
{
	return m_pConnList->GetCount();
}

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeConnection

CNamedPipeConnection::CNamedPipeConnection() :
	m_pServer(NULL),
	m_hPipeHandle(INVALID_HANDLE_VALUE)
{
	InitVariables();
}

//-----------------------------------------------------------------------------

CNamedPipeConnection::CNamedPipeConnection(CNamedPipeServer& Server, HANDLE hPipeHandle) :
	m_pServer(&Server),
	m_hPipeHandle(hPipeHandle)
{
	InitVariables();

	if (m_pServer->IsUseIocp())
		GetIocpObject().AssociateHandle(hPipeHandle);
	InitConnection();

	m_pServer->GetConnList().RegisterItem(this);
}

//-----------------------------------------------------------------------------

CNamedPipeConnection::~CNamedPipeConnection()
{
	if (m_pServer)
		m_pServer->GetConnList().UnregisterItem(this);

	CATCH_ALL_EXCEPTION( InternalDisconnect(); );

	CloseHandle(m_ov.hEvent);
	CloseHandle(m_hStopEvent);
}

//-----------------------------------------------------------------------------

void CNamedPipeConnection::InitVariables()
{
	m_bConnected = false;

	memset(&m_ov, 0, sizeof(m_ov));
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

//-----------------------------------------------------------------------------

void CNamedPipeConnection::InitConnection()
{
	m_bConnected = true;
	ResetEvent(m_hStopEvent);
}

//-----------------------------------------------------------------------------

void CNamedPipeConnection::InternalDisconnect()
{
	bool bSavedConnected = m_bConnected;

	if (m_bConnected)
		SetEvent(m_hStopEvent);
	m_bConnected = false;

	if (!IsClient())
		DisconnectNamedPipe(m_hPipeHandle);
	CloseHandle(m_hPipeHandle);
	m_hPipeHandle = INVALID_HANDLE_VALUE;

	if (bSavedConnected)
		OnDisconnect();
}

//-----------------------------------------------------------------------------

void CNamedPipeConnection::Disconnect()
{
	InternalDisconnect();
}

//-----------------------------------------------------------------------------

int CNamedPipeConnection::DoSyncTransferBlock(bool bSend, void *pBuffer, int nSize, int nTimeOutMSecs)
{
	if (!IsConnected()) return 0;
	ResetEvent(m_ov.hEvent);

	int nResult = 0;
	int nRemainSize = nSize;
	DWORD nStartTime = GetTickCount();

	while (nRemainSize > 0)
	{
		BOOL bSuccess = bSend ?
			WriteFile(m_hPipeHandle, ((char*)pBuffer) + (nSize - nRemainSize), nRemainSize, NULL, &m_ov) :
			ReadFile(m_hPipeHandle, ((char*)pBuffer) + (nSize - nRemainSize), nRemainSize, NULL, &m_ov);

		// An error occurred
		if (!bSuccess)
		{
			int nErrorCode = GetLastError();
			if (nErrorCode != ERROR_IO_PENDING)
				break;
		}

		DWORD nTransBytes = 0;
		HANDLE h[2] = { m_ov.hEvent, m_hStopEvent };
		DWORD nRet = WaitForMultipleObjects(2, h, FALSE, nTimeOutMSecs >= 0 ? nTimeOutMSecs : INFINITE);
		if (nRet == WAIT_OBJECT_0)
		{
			GetOverlappedResult(m_hPipeHandle, &m_ov, &nTransBytes, TRUE);
			ResetEvent(m_ov.hEvent);
		}
		else
		{
			break;
		}

		nRemainSize -= nTransBytes;
		nResult += nTransBytes;

		// If need to check timeout
		if (nTimeOutMSecs >= 0 && nRemainSize > 0)
		{
			nTimeOutMSecs -= GetTickDiff(nStartTime, GetTickCount());
			if (nTimeOutMSecs <= 0)
				break;
		}
	}

	nResult = EnsureRange(nResult, 0, nSize);
	return nResult;
}

//-----------------------------------------------------------------------------

int CNamedPipeConnection::DoSyncTransferBuffer(bool bSend, void *pBuffer, int nSize, int nTimeOutMSecs)
{
	int nRemainSize = nSize;
	int nResult = 0;

	while (nRemainSize > 0)
	{
		int nBlockSize = Min(PIPE_IO_BUFFER_SIZE, nRemainSize);
		DWORD nStartTime = GetTickCount();

		int nTransBytes = DoSyncTransferBlock(bSend, (char*)pBuffer + (nSize - nRemainSize), nBlockSize, nTimeOutMSecs);

		nRemainSize -= nTransBytes;
		nResult += nTransBytes;

		if (nTimeOutMSecs >= 0)
		{
			nTimeOutMSecs -= GetTickDiff(nStartTime, GetTickCount());
			if (nTimeOutMSecs <= 0) break;
		}

		if (nTransBytes != nBlockSize) break;
	}

	return nResult;
}

//-----------------------------------------------------------------------------

int CNamedPipeConnection::SendBuffer(void *pBuffer, int nSize, PVOID pParam, int nTimeOutMSecs)
{
	if (m_pServer && m_pServer->IsUseIocp())
	{
		GetIocpObject().Send(m_hPipeHandle, pBuffer, nSize, 0,
			IOCP_CALLBACK_DEF(CNamedPipeServer::IocpCallBackProc, NULL),
			this, CIocpParams(1, pParam));
		return 0;
	}
	else
	{
		return DoSyncTransferBuffer(true, pBuffer, nSize, nTimeOutMSecs);
	}
}

//-----------------------------------------------------------------------------

int CNamedPipeConnection::RecvBuffer(void *pBuffer, int nSize, PVOID pParam, int nTimeOutMSecs)
{
	if (m_pServer && m_pServer->IsUseIocp())
	{
		GetIocpObject().Recv(m_hPipeHandle, pBuffer, nSize, 0,
			IOCP_CALLBACK_DEF(CNamedPipeServer::IocpCallBackProc, NULL),
			this, CIocpParams(1, pParam));
		return 0;
	}
	else
	{
		return DoSyncTransferBuffer(false, pBuffer, nSize, nTimeOutMSecs);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeConnList

CNamedPipeConnList::CNamedPipeConnList(CNamedPipeServer& Server) :
	m_Server(Server)
{
	// nothing
}

//-----------------------------------------------------------------------------

CNamedPipeConnList::~CNamedPipeConnList()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CNamedPipeConnList::RegisterItem(CNamedPipeConnection *pItem)
{
	CAutoLocker Locker(m_Lock);
	if (pItem && m_Items.IndexOf(pItem) == -1)
		m_Items.Add(pItem);
}

//-----------------------------------------------------------------------------

void CNamedPipeConnList::UnregisterItem(CNamedPipeConnection *pItem)
{
	CAutoLocker Locker(m_Lock);
	m_Items.Remove(pItem);
}

//-----------------------------------------------------------------------------

bool CNamedPipeConnList::Remove(CNamedPipeConnection *pItem)
{
	CAutoLocker Locker(m_Lock);

	bool bResult = (m_Items.IndexOf(pItem) >= 0);
	if (bResult)
		delete pItem;
	return bResult;
}

//-----------------------------------------------------------------------------

void CNamedPipeConnList::Clear()
{
	CAutoLocker Locker(m_Lock);

	while (!m_Items.IsEmpty())
		delete (CNamedPipeConnection*)m_Items[0];
}

//-----------------------------------------------------------------------------

int CNamedPipeConnList::GetCount() const
{
	CAutoLocker Locker(m_Lock);
	return m_Items.GetCount();
}

//-----------------------------------------------------------------------------

void CNamedPipeConnList::GetItems(std::vector<CNamedPipeConnection*>& Items) const
{
	CAutoLocker Locker(m_Lock);

	Items.clear();
	for (int i = 0; i < m_Items.GetCount(); i++)
		Items.push_back((CNamedPipeConnection*)m_Items[i]);
}

//-----------------------------------------------------------------------------

void CNamedPipeConnList::Iterate(PIPE_CONNLIST_ITERATE_PROC pProc, PVOID pParam)
{
	CAutoLocker Locker(m_Lock);

	for (int i = m_Items.GetCount() - 1; i >= 0; i--)
	{
		if (i < m_Items.GetCount())
			pProc((CNamedPipeConnection*)m_Items[i], pParam);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeListenerThread

CNamedPipeListenerThread::CNamedPipeListenerThread(CNamedPipeServer& Server) :
	m_Server(Server)
{
	SetFreeOnTerminate(false);

	memset(&m_ov, 0, sizeof(m_ov));
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

//-----------------------------------------------------------------------------

CNamedPipeListenerThread::~CNamedPipeListenerThread()
{
	CloseHandle(m_ov.hEvent);
	CloseHandle(m_hStopEvent);
}

//-----------------------------------------------------------------------------

void CNamedPipeListenerThread::BeforeTerminate()
{
	SetEvent(m_hStopEvent);
}

//-----------------------------------------------------------------------------

void CNamedPipeListenerThread::Execute()
{
	const int PIPE_TIMEOUT = 1000*5;

	while (!GetTerminated() && m_Server.GetActive())
	try
	{
		HANDLE hPipeHandle;
		bool bConnected = false;

		hPipeHandle = CreateNamedPipe(
			m_Server.GetPipeName(),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			PIPE_IO_BUFFER_SIZE,
			PIPE_IO_BUFFER_SIZE,
			PIPE_TIMEOUT,            // client time-out
			NULL);

		if (hPipeHandle == INVALID_HANDLE_VALUE)
		{
			SleepThread(1);
			continue;
		}

		// Start an overlapped connection for this pipe instance.
		ResetEvent(m_ov.hEvent);
		BOOL bRet = ConnectNamedPipe(hPipeHandle, &m_ov);

		// Overlapped ConnectNamedPipe should return zero.
		if (bRet)
		{
			// printf("ConnectNamedPipe failed with %d.\n", GetLastError());
			CloseHandle(hPipeHandle);
			SleepThread(1);
			continue;
		}

		switch (GetLastError())
		{
		// The overlapped connection in progress.
		case ERROR_IO_PENDING:
			break;

		// Client is already connected, so signal an event.
		case ERROR_PIPE_CONNECTED:
			bConnected = true;
			break;

		// If an error occurs during the connect operation...
		default:
			// printf("ConnectNamedPipe failed with %d.\n", GetLastError());
			CloseHandle(hPipeHandle);
			SleepThread(1);
			continue;
		}

		if (!bConnected)
		{
			HANDLE h[2] = { m_ov.hEvent, m_hStopEvent };
			DWORD nRet = WaitForMultipleObjects(2, h, FALSE, INFINITE);
			if (nRet == WAIT_OBJECT_0)
			{
				DWORD nRecvBytes;
				if (GetOverlappedResult(hPipeHandle, &m_ov, &nRecvBytes, FALSE))
					bConnected = true;
			}
			else
			{
				CloseHandle(hPipeHandle);
				SleepThread(1);
				continue;
			}
		}

		// Connection established.
		if (bConnected)
		{
			if (!m_Server.NotifyConnect(hPipeHandle))
			{
				DisconnectNamedPipe(hPipeHandle);
				CloseHandle(hPipeHandle);
			}
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
// CNamedPipeClient

CNamedPipeClient::CNamedPipeClient()
{
	// nothing
}

//-----------------------------------------------------------------------------

bool CNamedPipeClient::Connect(LPCTSTR lpszPipeName, int nTimeOutMSecs)
{
	bool bResult;
	Disconnect();

	while (true)
	{
		m_hPipeHandle = CreateFile(
			lpszPipeName,                  // pipe name
			GENERIC_READ | GENERIC_WRITE,  // read and write access
			0,                             // no sharing
			NULL,                          // default security attributes
			OPEN_EXISTING,                 // opens existing pipe
			FILE_FLAG_OVERLAPPED,          // flags and attributes
			NULL);                         // no template file

		// Break if the pipe handle is valid.
		if (m_hPipeHandle != INVALID_HANDLE_VALUE)
		{
			bResult = true;
			break;
		}

		// Exit if an error other than ERROR_PIPE_BUSY occurs.
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			// ERROR: Could not open pipe.
			bResult = false;
			break;
		}

		// All pipe instances are busy, so wait for...
		DWORD dwTimeOut = (nTimeOutMSecs >= 0 ? nTimeOutMSecs : NMPWAIT_WAIT_FOREVER);
		if (!WaitNamedPipe(lpszPipeName, dwTimeOut))
		{
			// ERROR: Could not open pipe.
			bResult = false;
			break;
		}
	}

	if (bResult)
	{
		InitConnection();
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
