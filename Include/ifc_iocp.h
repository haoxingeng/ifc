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
/// Defines the IOCP classes.

/// @addtogroup Network
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_thread.h"
#include "ifc_socket.h"
#include "ifc_exceptions.h"

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////
// Classes

class CIocpParams;
class CIocpTaskData;
class CIocpBufferAllocator;
class CIocpObject;
class CIocpTcpConnection;
class CIocpTcpServer;

///////////////////////////////////////////////////////////////////////////////
// Type Definitions

enum IOCP_TASK_TYPE
{
	ITT_SEND = 1,
	ITT_RECV = 2,
};

typedef void (*IOCP_CALLBACK_PROC)(const CIocpTaskData& TaskData, PVOID pParam);
typedef CCallBackDef<IOCP_CALLBACK_PROC> IOCP_CALLBACK_DEF;

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

// Returns the global iocp object.
CIocpObject& GetIocpObject();
// Delete the global iocp object.
void DeleteIocpObject();
// Indicates whether the global iocp object is created or not.
bool IsIocpObjectAvailable();

///////////////////////////////////////////////////////////////////////////////
// CIocpParams

class CIocpParams
{
public:
	enum { MAX_PARAM_COUNT = 8 };
private:
	int m_nCount;
	PVOID m_pParams[MAX_PARAM_COUNT];
private:
	inline void Init();
public:
	CIocpParams();
	CIocpParams(const CIocpParams& src);
	CIocpParams(int nCount, ...);

	bool Add(PVOID pValue);
	void Clear();
	int GetCount() const { return m_nCount; }

	CIocpParams& operator = (const CIocpParams& rhs);
	PVOID& operator[] (int nIndex);
	const PVOID& operator[] (int nIndex) const;
};

///////////////////////////////////////////////////////////////////////////////
// CIocpTaskData

class CIocpTaskData
{
public:
	friend class CIocpObject;
private:
	HANDLE m_hIocpHandle;
	HANDLE m_hFileHandle;
	IOCP_TASK_TYPE m_nTaskType;
	UINT m_nTaskSeqNum;
	CCallBackDef<IOCP_CALLBACK_PROC> m_CallBack;
	PVOID m_pCaller;
	CIocpParams m_Params;
	PVOID m_pEntireDataBuf;
	int m_nEntireDataSize;
	WSABUF m_WSABuffer;
	int m_nBytesTrans;
	int m_nErrorCode;

public:
	CIocpTaskData();

	HANDLE GetIocpHandle() const { return m_hIocpHandle; }
	HANDLE GetFileHandle() const { return m_hFileHandle; }
	IOCP_TASK_TYPE GetTaskType() const { return m_nTaskType; }
	UINT GetTaskSeqNum() const { return m_nTaskSeqNum; }
	const IOCP_CALLBACK_DEF& GetCallBack() const { return m_CallBack; }
	PVOID GetCaller() const { return m_pCaller; }
	const CIocpParams& GetParams() const { return m_Params; }
	char* GetEntireDataBuf() const { return (char*)m_pEntireDataBuf; }
	int GetEntireDataSize() const { return m_nEntireDataSize; }
	char* GetDataBuf() const { return (char*)m_WSABuffer.buf; }
	int GetDataSize() const { return m_WSABuffer.len; }
	int GetBytesTrans() const { return m_nBytesTrans; }
	int GetErrorCode() const { return m_nErrorCode; }
};

#pragma pack(1)
struct CIocpOverlappedData
{
	OVERLAPPED Overlapped;
	CIocpTaskData TaskData;
};
#pragma pack()

///////////////////////////////////////////////////////////////////////////////
// CIocpBufferAllocator

class CIocpBufferAllocator
{
private:
	int m_nBufferSize;
	CPointerList m_Items;
	int m_nUsedCount;
	CCriticalSection m_Lock;
private:
	void Clear();
public:
	CIocpBufferAllocator(int nBufferSize);
	~CIocpBufferAllocator();

	PVOID AllocBuffer();
	void ReturnBuffer(PVOID pBuffer);

	int GetUsedCount() const { return m_nUsedCount; }
};

///////////////////////////////////////////////////////////////////////////////
// CIocpPendingCounter

class CIocpPendingCounter
{
private:
	struct COUNT_DATA
	{
		int nSendCount;
		int nRecvCount;
	};

	typedef std::map<PVOID, COUNT_DATA> ITEMS;   // <pCaller, COUNT_DATA>

	ITEMS m_Items;
	CCriticalSection m_Lock;
public:
	CIocpPendingCounter() {}
	virtual ~CIocpPendingCounter() {}

	void Inc(PVOID pCaller, IOCP_TASK_TYPE nTaskType);
	void Dec(PVOID pCaller, IOCP_TASK_TYPE nTaskType);
	int Get(PVOID pCaller);
	int Get(IOCP_TASK_TYPE nTaskType);
};

///////////////////////////////////////////////////////////////////////////////
// CIocpObject

class CIocpObject
{
public:
	friend class CIocpWorkerThread;
	friend class CAutoFinalizer;

private:
	class CIocpWorkerThread : public CThread
	{
	protected:
		CIocpObject& m_IocpObject;
		virtual void Execute();
	public:
		CIocpWorkerThread(CIocpObject& IocpObject);
	};

private:
	HANDLE m_hIocpHandle;
	CPointerList m_WorkerThreads;
	CIocpBufferAllocator m_BufferAlloc;
	CSeqNumberAlloc m_TaskSeqAlloc;
	CIocpPendingCounter m_PendingCounter;
	long m_nErrorCount;

private:
	static std::auto_ptr<CIocpObject> s_pSingleton;

private:
	void Initialize();
	void Finalize();
	void ThrowGeneralError();
	CIocpOverlappedData* CreateOverlappedData(IOCP_TASK_TYPE nTaskType,
		HANDLE hFileHandle, PVOID pBuffer, int nSize, int nOffset,
		const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller,
		const CIocpParams& Params);
	void DestroyOverlappedData(CIocpOverlappedData *pOvDataPtr);
	void PostError(int nErrorCode, CIocpOverlappedData *pOvDataPtr);
	void InvokeCallBack(const CIocpTaskData& TaskData);
	void Work();

private:
	CIocpObject();
public:
	virtual ~CIocpObject();
	static CIocpObject& Instance();
	static void Delete();
	static bool IsAvailable();

	bool AssociateHandle(HANDLE hFileHandle);
	bool AssociateHandle(SOCKET hSocketHandle);

	void Send(HANDLE hFileHandle, PVOID pBuffer, int nSize, int nOffset,
		const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params);
	void Recv(HANDLE hFileHandle, PVOID pBuffer, int nSize, int nOffset,
		const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params);

	void Send(SOCKET hSocketHandle, PVOID pBuffer, int nSize, int nOffset,
		const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params);
	void Recv(SOCKET hSocketHandle, PVOID pBuffer, int nSize, int nOffset,
		const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params);

	void SendTo(SOCKET hSocketHandle, const CPeerAddress& PeerAddr,
		PVOID pBuffer, int nSize, int nOffset,
		const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params);

	void WaitForComplete(PVOID pCaller);
	bool IsComplete(PVOID pCaller);
	bool IsInWorkerThread();
	int GetPendingCount(PVOID pCaller);
	int GetPendingCount(IOCP_TASK_TYPE nTaskType);
	int GetErrorCount() { return m_nErrorCount; }
	int GetUsedBufferCount() { return m_BufferAlloc.GetUsedCount(); }
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc

/// @}
