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

/// @file ifc_iocp.h
/// Defines the pipe classes.

/// @addtogroup Network
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_thread.h"
#include "ifc_iocp.h"
#include "ifc_exceptions.h"

#include <vector>

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Classes

class CNamedPipeServer;
class CNamedPipeConnection;
class CNamedPipeConnList;
class CNamedPipeListenerThread;
class CNamedPipeClient;

///////////////////////////////////////////////////////////////////////////////
// Constant Defines

const int PIPE_IO_BUFFER_SIZE       = 1024*8;
const int PIPE_DEF_MAX_CONN_COUNT   = 512;

///////////////////////////////////////////////////////////////////////////////
// Type Defines

typedef void (*PIPE_CONNLIST_ITERATE_PROC)(CNamedPipeConnection *pItem, PVOID pParam);

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeServer

class CNamedPipeServer
{
public:
	friend class CNamedPipeListenerThread;
	friend class CNamedPipeConnection;

private:
	CString m_strPipeName;                 // The pipe name, such as '\\.\Pipe\MyPipeName'.
	CNamedPipeListenerThread *m_pListenerThread;   // The listener thread
	CNamedPipeConnList *m_pConnList;       // The accepted connection list for this server.
	int m_nMaxConnCount;                   // The maximum connection limit. 0 for unlimited.
	bool m_bUseIocp;                       // Determines whether to use iocp or not.
private:
	void InternalClose();
	void StartListenerThread();
	void StopListenerThread();
	bool IsConnListFull();
	bool NotifyConnect(HANDLE hPipeHandle);

	static void IocpCallBackProc(const CIocpTaskData& TaskData, PVOID pParam);
protected:
	virtual void OnConnect(CNamedPipeConnection *pConnection) {}
	virtual CNamedPipeConnection* CreateConnection(HANDLE hPipeHandle);
public:
	explicit CNamedPipeServer(bool bUseIocp = true);
	virtual ~CNamedPipeServer();

	virtual void Open();
	virtual void Close();

	bool GetActive() const;
	void SetActive(bool bValue);
	CString GetPipeName() const { return m_strPipeName; }
	void SetPipeName(LPCTSTR lpszPipeName) { m_strPipeName = lpszPipeName; }
	int GetMaxConnCount() const { return m_nMaxConnCount; }
	void SetMaxConnCount(int nValue);
	int GetCurConnCount() const;
	bool IsUseIocp() const { return m_bUseIocp; }
	CNamedPipeConnList& GetConnList() { return *m_pConnList; }
};

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeConnection

class CNamedPipeConnection
{
public:
	friend class CNamedPipeServer;

private:
	CNamedPipeServer *m_pServer;
	OVERLAPPED m_ov;
	HANDLE m_hStopEvent;
protected:
	HANDLE m_hPipeHandle;
	bool m_bConnected;
protected:
	void InitVariables();
	void InitConnection();
	void InternalDisconnect();
	CNamedPipeServer* GetServer() { return m_pServer; }
protected:
	virtual void OnConnect() {}
	virtual void OnDisconnect() {}
	virtual void OnIocpCallBack(CIocpTaskData *pTaskData) {}
	virtual int DoSyncTransferBlock(bool bSend, void *pBuffer, int nSize, int nTimeOutMSecs);
	virtual int DoSyncTransferBuffer(bool bSend, void *pBuffer, int nSize, int nTimeOutMSecs);
public:
	CNamedPipeConnection();
	CNamedPipeConnection(CNamedPipeServer& Server, HANDLE hPipeHandle);
	virtual ~CNamedPipeConnection();

	bool IsClient() const { return m_pServer == NULL; }
	bool IsConnected() const { return m_bConnected; }

	virtual void Disconnect();

	int SendBuffer(void *pBuffer, int nSize, PVOID pParam = NULL, int nTimeOutMSecs = -1);
	int RecvBuffer(void *pBuffer, int nSize, PVOID pParam = NULL, int nTimeOutMSecs = -1);
};

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeConnList

class CNamedPipeConnList
{
public:
	friend class CNamedPipeConnection;

private:
	CNamedPipeServer& m_Server;
	CPointerList m_Items;                // (CNamedPipeConnection*)[]
	mutable CCriticalSection m_Lock;
private:
	void RegisterItem(CNamedPipeConnection *pItem);
	void UnregisterItem(CNamedPipeConnection *pItem);
public:
	CNamedPipeConnList(CNamedPipeServer& Server);
	virtual ~CNamedPipeConnList();

	bool Remove(CNamedPipeConnection *pItem);
	void Clear();
	int GetCount() const;
	void GetItems(std::vector<CNamedPipeConnection*>& Items) const;
	CCriticalSection& GetLock() const { return m_Lock; }
	void Iterate(PIPE_CONNLIST_ITERATE_PROC pProc, PVOID pParam = NULL);
};

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeListenerThread

class CNamedPipeListenerThread : public CThread
{
private:
	CNamedPipeServer& m_Server;
	OVERLAPPED m_ov;
	HANDLE m_hStopEvent;
protected:
	virtual void BeforeTerminate();
	virtual void Execute();
public:
	CNamedPipeListenerThread(CNamedPipeServer& Server);
	virtual ~CNamedPipeListenerThread();
};

///////////////////////////////////////////////////////////////////////////////
// CNamedPipeClient

class CNamedPipeClient : public CNamedPipeConnection
{
public:
	CNamedPipeClient();
	virtual ~CNamedPipeClient() {}

	bool Connect(LPCTSTR lpszPipeName, int nTimeOutMSecs = -1);
};

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)

/// @}
