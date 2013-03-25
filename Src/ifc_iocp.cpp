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

/// @file ifc_iocp.cpp

#include "stdafx.h"
#include "ifc_iocp.h"
#include "ifc_sysutils.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

CIocpObject& GetIocpObject()
{
	return CIocpObject::Instance();
}

//-----------------------------------------------------------------------------

void DeleteIocpObject()
{
	CIocpObject::Delete();
}

//-----------------------------------------------------------------------------

bool IsIocpObjectAvailable()
{
	return CIocpObject::IsAvailable();
}

///////////////////////////////////////////////////////////////////////////////
// CIocpParams

CIocpParams::CIocpParams()
{
	Init();
}

//-----------------------------------------------------------------------------

CIocpParams::CIocpParams(const CIocpParams& src)
{
	Init();
	*this = src;
}

//-----------------------------------------------------------------------------

CIocpParams::CIocpParams(int nCount, ...)
{
	IFC_ASSERT(nCount >= 0 && nCount <= MAX_PARAM_COUNT);

	Init();

	va_list argList;
	va_start(argList, nCount);
	for (int i = 0; i < nCount; i++)
	{
		PVOID pArg = va_arg(argList, PVOID);
		Add(pArg);
	}
	va_end(argList);
}

//-----------------------------------------------------------------------------

void CIocpParams::Init()
{
	m_nCount = 0;
}

//-----------------------------------------------------------------------------

bool CIocpParams::Add(PVOID pValue)
{
	bool bResult = (m_nCount < MAX_PARAM_COUNT);
	if (bResult)
	{
		m_pParams[m_nCount] = pValue;
		m_nCount++;
	}

	return bResult;
}

//-----------------------------------------------------------------------------

void CIocpParams::Clear()
{
	Init();
}

//-----------------------------------------------------------------------------

CIocpParams& CIocpParams::operator = (const CIocpParams& rhs)
{
	if (this == &rhs) return *this;

	memmove(m_pParams, rhs.m_pParams, sizeof(m_pParams));
	m_nCount = rhs.m_nCount;

	return *this;
}

//-----------------------------------------------------------------------------

PVOID& CIocpParams::operator[] (int nIndex)
{
	return
		const_cast<PVOID&>(
			((const CIocpParams&)(*this))[nIndex]
		);
}

//-----------------------------------------------------------------------------

const PVOID& CIocpParams::operator[] (int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_nCount)
		IfcThrowException(FormatString(SEM_LIST_INDEX_ERROR, nIndex));

	return m_pParams[nIndex];
}

///////////////////////////////////////////////////////////////////////////////
// CIocpTaskData

CIocpTaskData::CIocpTaskData() :
	m_hIocpHandle(INVALID_HANDLE_VALUE),
	m_hFileHandle(INVALID_HANDLE_VALUE),
	m_nTaskType((IOCP_TASK_TYPE)0),
	m_nTaskSeqNum(0),
	m_pCaller(0),
	m_pEntireDataBuf(0),
	m_nEntireDataSize(0),
	m_nBytesTrans(0),
	m_nErrorCode(0)
{
	m_WSABuffer.buf = NULL;
	m_WSABuffer.len = 0;
}

///////////////////////////////////////////////////////////////////////////////
// CIocpBufferAllocator

CIocpBufferAllocator::CIocpBufferAllocator(int nBufferSize) :
	m_nBufferSize(nBufferSize),
	m_nUsedCount(0)
{
	// nothing
}

//-----------------------------------------------------------------------------

CIocpBufferAllocator::~CIocpBufferAllocator()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CIocpBufferAllocator::Clear()
{
	CAutoLocker Locker(m_Lock);

	for (int i = 0; i < m_Items.GetCount(); i++)
		delete[] (char*)m_Items[i];
	m_Items.Clear();
}

//-----------------------------------------------------------------------------

PVOID CIocpBufferAllocator::AllocBuffer()
{
	CAutoLocker Locker(m_Lock);
	PVOID pResult;

	if (!m_Items.IsEmpty())
	{
		pResult = m_Items.Last();
		m_Items.Delete(m_Items.GetCount() - 1);
	}
	else
	{
		pResult = new char[m_nBufferSize];
		if (pResult == NULL)
			IfcThrowMemoryException();
	}

	m_nUsedCount++;
	return pResult;
}

//-----------------------------------------------------------------------------

void CIocpBufferAllocator::ReturnBuffer(PVOID pBuffer)
{
	CAutoLocker Locker(m_Lock);

	if (pBuffer != NULL && m_Items.IndexOf(pBuffer) == -1)
	{
		m_Items.Add(pBuffer);
		m_nUsedCount--;
	}
}

///////////////////////////////////////////////////////////////////////////////
// CIocpPendingCounter

void CIocpPendingCounter::Inc(PVOID pCaller, IOCP_TASK_TYPE nTaskType)
{
	CAutoLocker Locker(m_Lock);

	ITEMS::iterator iter = m_Items.find(pCaller);
	if (iter == m_Items.end())
	{
		COUNT_DATA Data = {0, 0};
		iter = m_Items.insert(std::make_pair(pCaller, Data)).first;
	}

	if (nTaskType == ITT_SEND)
		iter->second.nSendCount++;
	else if (nTaskType == ITT_RECV)
		iter->second.nRecvCount++;
}

//-----------------------------------------------------------------------------

void CIocpPendingCounter::Dec(PVOID pCaller, IOCP_TASK_TYPE nTaskType)
{
	CAutoLocker Locker(m_Lock);

	ITEMS::iterator iter = m_Items.find(pCaller);
	if (iter != m_Items.end())
	{
		if (nTaskType == ITT_SEND)
			iter->second.nSendCount--;
		else if (nTaskType == ITT_RECV)
			iter->second.nRecvCount--;

		if (iter->second.nSendCount <= 0 && iter->second.nRecvCount <= 0)
			m_Items.erase(iter);
	}
}

//-----------------------------------------------------------------------------

int CIocpPendingCounter::Get(PVOID pCaller)
{
	CAutoLocker Locker(m_Lock);

	ITEMS::iterator iter = m_Items.find(pCaller);
	if (iter == m_Items.end())
		return 0;
	else
		return Max(0, iter->second.nSendCount + iter->second.nRecvCount);
}

//-----------------------------------------------------------------------------

int CIocpPendingCounter::Get(IOCP_TASK_TYPE nTaskType)
{
	CAutoLocker Locker(m_Lock);

	int nResult = 0;
	if (nTaskType == ITT_SEND)
	{
		for (ITEMS::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
			nResult += iter->second.nSendCount;
	}
	else if (nTaskType == ITT_RECV)
	{
		for (ITEMS::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
			nResult += iter->second.nRecvCount;
	}

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// CIocpObject::CIocpWorkerThread

CIocpObject::CIocpWorkerThread::CIocpWorkerThread(CIocpObject& IocpObject) :
	m_IocpObject(IocpObject)
{
	SetFreeOnTerminate(false);
}

//-----------------------------------------------------------------------------

void CIocpObject::CIocpWorkerThread::Execute()
{
	while (!GetTerminated())
	try
	{
		m_IocpObject.Work();
	}
	catch (CException* e)
	{
		e->Delete();
	}
	catch (...)
	{}
}

///////////////////////////////////////////////////////////////////////////////
// CIocpObject

std::auto_ptr<CIocpObject> CIocpObject::s_pSingleton(NULL);

//-----------------------------------------------------------------------------

CIocpObject::CIocpObject() :
	m_hIocpHandle(0),
	m_BufferAlloc(sizeof(CIocpOverlappedData)),
	m_TaskSeqAlloc(0),
	m_nErrorCount(0)
{
	Initialize();
}

//-----------------------------------------------------------------------------

CIocpObject::~CIocpObject()
{
	Finalize();
}

//-----------------------------------------------------------------------------

CIocpObject& CIocpObject::Instance()
{
	if (s_pSingleton.get() == NULL)
		s_pSingleton.reset(new CIocpObject());
	return *s_pSingleton;
}

//-----------------------------------------------------------------------------

void CIocpObject::Delete()
{
	s_pSingleton.reset(NULL);
}

//-----------------------------------------------------------------------------

bool CIocpObject::IsAvailable()
{
	return (s_pSingleton.get() != NULL);
}

//-----------------------------------------------------------------------------

void CIocpObject::Initialize()
{
	SYSTEM_INFO SysInfo;
	int nThreadCount;

	GetSystemInfo(&SysInfo);
	nThreadCount = SysInfo.dwNumberOfProcessors * 2 + 4;

	m_hIocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, nThreadCount);
	if (m_hIocpHandle == 0)
		ThrowGeneralError();

	for (int i = 0; i < nThreadCount; i++)
	{
		CIocpWorkerThread *pThread = new CIocpWorkerThread(*this);
		m_WorkerThreads.Add(pThread);
		pThread->Run();
	}
}

//-----------------------------------------------------------------------------

void CIocpObject::Finalize()
{
	// Notify the threads to exit
	for (int i = 0; i < m_WorkerThreads.GetCount(); i++)
		((CIocpWorkerThread*)m_WorkerThreads[i])->Terminate();
	for (int i = 0; i < m_WorkerThreads.GetCount(); i++)
		::PostQueuedCompletionStatus(m_hIocpHandle, 0, 0, NULL);

	// Wait for the threads to exit
	for (int i = 0; i < m_WorkerThreads.GetCount(); i++)
	{
		CIocpWorkerThread *pThread = (CIocpWorkerThread*)m_WorkerThreads[i];
		pThread->WaitFor();
		delete pThread;
	}
	m_WorkerThreads.Clear();

	CloseHandle(m_hIocpHandle);
	m_hIocpHandle = 0;
}

//-----------------------------------------------------------------------------

void CIocpObject::ThrowGeneralError()
{
	IfcThrowException(FormatString(SEM_IOCP_ERROR, GetLastError()));
}

//-----------------------------------------------------------------------------

CIocpOverlappedData* CIocpObject::CreateOverlappedData(IOCP_TASK_TYPE nTaskType,
	HANDLE hFileHandle, PVOID pBuffer, int nSize, int nOffset,
	const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params)
{
	IFC_ASSERT(pBuffer != NULL);
	IFC_ASSERT(nSize >= 0);
	IFC_ASSERT(nOffset >= 0);
	IFC_ASSERT(nOffset < nSize);

	CIocpOverlappedData *pResult = (CIocpOverlappedData*)m_BufferAlloc.AllocBuffer();
	memset(pResult, 0, sizeof(*pResult));

	pResult->TaskData.m_hIocpHandle = m_hIocpHandle;
	pResult->TaskData.m_hFileHandle = hFileHandle;
	pResult->TaskData.m_nTaskType = nTaskType;
	pResult->TaskData.m_nTaskSeqNum = m_TaskSeqAlloc.AllocId();
	pResult->TaskData.m_CallBack = CallBackDef;
	pResult->TaskData.m_pCaller = pCaller;
	pResult->TaskData.m_Params = Params;
	pResult->TaskData.m_pEntireDataBuf = pBuffer;
	pResult->TaskData.m_nEntireDataSize = nSize;
	pResult->TaskData.m_WSABuffer.buf = (char*)pBuffer + nOffset;
	pResult->TaskData.m_WSABuffer.len = nSize - nOffset;

	return pResult;
}

//-----------------------------------------------------------------------------

void CIocpObject::DestroyOverlappedData(CIocpOverlappedData *pOvDataPtr)
{
	m_BufferAlloc.ReturnBuffer(pOvDataPtr);
}

//-----------------------------------------------------------------------------

void CIocpObject::PostError(int nErrorCode, CIocpOverlappedData *pOvDataPtr)
{
	pOvDataPtr->TaskData.m_nErrorCode = nErrorCode;
	::PostQueuedCompletionStatus(m_hIocpHandle, 0, 0, LPOVERLAPPED(pOvDataPtr));
}

//-----------------------------------------------------------------------------

void CIocpObject::InvokeCallBack(const CIocpTaskData& TaskData)
{
	const IOCP_CALLBACK_DEF& CallBackDef = TaskData.GetCallBack();
	if (CallBackDef.pProc != NULL)
		CallBackDef.pProc(TaskData, CallBackDef.pParam);
}

//-----------------------------------------------------------------------------

void CIocpObject::Work()
{
	CIocpOverlappedData *pOverlappedPtr = NULL;
	DWORD nBytesTransferred = 0, nTemp = 0;
	int nErrorCode = 0;

	struct CAutoFinalizer
	{
	private:
		CIocpObject& m_IocpObject;
		CIocpOverlappedData*& m_pOvPtr;
	public:
		CAutoFinalizer(CIocpObject& IocpObject, CIocpOverlappedData*& pOvPtr) :
			m_IocpObject(IocpObject), m_pOvPtr(pOvPtr) {}
		~CAutoFinalizer()
		{
			if (m_pOvPtr)
			{
				m_IocpObject.m_PendingCounter.Dec(
					m_pOvPtr->TaskData.GetCaller(),
					m_pOvPtr->TaskData.GetTaskType());
				m_IocpObject.DestroyOverlappedData(m_pOvPtr);
			}
		}
	} AutoFinalizer(*this, pOverlappedPtr);

	/*
	FROM MSDN:

	If the function dequeues a completion packet for a successful I/O operation from the completion port,
	the return value is nonzero. The function stores information in the variables pointed to by the
	lpNumberOfBytes, lpCompletionKey, and lpOverlapped parameters.

	If *lpOverlapped is NULL and the function does not dequeue a completion packet from the completion port,
	the return value is zero. The function does not store information in the variables pointed to by the
	lpNumberOfBytes and lpCompletionKey parameters. To get extended error information, call GetLastError.
	If the function did not dequeue a completion packet because the wait timed out, GetLastError returns
	WAIT_TIMEOUT.

	If *lpOverlapped is not NULL and the function dequeues a completion packet for a failed I/O operation
	from the completion port, the return value is zero. The function stores information in the variables
	pointed to by lpNumberOfBytes, lpCompletionKey, and lpOverlapped. To get extended error information,
	call GetLastError.

	If a socket handle associated with a completion port is closed, GetQueuedCompletionStatus returns
	ERROR_SUCCESS (0), with *lpOverlapped non-NULL and lpNumberOfBytes equal zero.
	*/

	if (::GetQueuedCompletionStatus(m_hIocpHandle, &nBytesTransferred, &nTemp,
		(LPOVERLAPPED*)&pOverlappedPtr, INFINITE))
	{
		if (pOverlappedPtr != NULL && nBytesTransferred == 0)
		{
			nErrorCode = pOverlappedPtr->TaskData.GetErrorCode();
			if (nErrorCode == 0)
				nErrorCode = GetLastError();
			if (nErrorCode == 0)
				nErrorCode = SOCKET_ERROR;
		}
	}
	else
	{
		if (pOverlappedPtr != NULL)
			nErrorCode = GetLastError();
		else
		{
			if (GetLastError() != WAIT_TIMEOUT)
				ThrowGeneralError();
		}
	}

	if (pOverlappedPtr != NULL)
	{
		CIocpTaskData *pTaskPtr = &pOverlappedPtr->TaskData;
		pTaskPtr->m_nBytesTrans = nBytesTransferred;
		if (pTaskPtr->m_nErrorCode == 0)
			pTaskPtr->m_nErrorCode = nErrorCode;

		if (pTaskPtr->m_nErrorCode != 0)
			InterlockedIncrement(&m_nErrorCount);

		InvokeCallBack(*pTaskPtr);
	}
}

//-----------------------------------------------------------------------------

bool CIocpObject::AssociateHandle(HANDLE hFileHandle)
{
	HANDLE h = ::CreateIoCompletionPort(hFileHandle, m_hIocpHandle, 0, 0);
	return (h != 0);
}

//-----------------------------------------------------------------------------

bool CIocpObject::AssociateHandle(SOCKET hSocketHandle)
{
	return AssociateHandle((HANDLE)hSocketHandle);
}

//-----------------------------------------------------------------------------

void CIocpObject::Send(HANDLE hFileHandle, PVOID pBuffer, int nSize, int nOffset,
	const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params)
{
	CIocpOverlappedData *pOvDataPtr;
	CIocpTaskData *pTaskPtr;
	DWORD nNumberOfBytesSent;

	m_PendingCounter.Inc(pCaller, ITT_SEND);

	pOvDataPtr = CreateOverlappedData(ITT_SEND, hFileHandle, pBuffer, nSize,
		nOffset, CallBackDef, pCaller, Params);
	pTaskPtr = &(pOvDataPtr->TaskData);

	if (!::WriteFile(hFileHandle, pBuffer, nSize, &nNumberOfBytesSent, (LPWSAOVERLAPPED)pOvDataPtr))
	{
		if (GetLastError() != ERROR_IO_PENDING)
			PostError(GetLastError(), pOvDataPtr);
	}
}

//-----------------------------------------------------------------------------

void CIocpObject::Recv(HANDLE hFileHandle, PVOID pBuffer, int nSize, int nOffset,
	const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params)
{
	CIocpOverlappedData *pOvDataPtr;
	CIocpTaskData *pTaskPtr;
	DWORD nNumberOfBytesSent;

	m_PendingCounter.Inc(pCaller, ITT_RECV);

	pOvDataPtr = CreateOverlappedData(ITT_RECV, hFileHandle, pBuffer, nSize,
		nOffset, CallBackDef, pCaller, Params);
	pTaskPtr = &(pOvDataPtr->TaskData);

	if (!::ReadFile(hFileHandle, pBuffer, nSize, &nNumberOfBytesSent, (LPWSAOVERLAPPED)pOvDataPtr))
	{
		if (GetLastError() != ERROR_IO_PENDING)
			PostError(GetLastError(), pOvDataPtr);
	}
}

//-----------------------------------------------------------------------------

void CIocpObject::Send(SOCKET hSocketHandle, PVOID pBuffer, int nSize, int nOffset,
	const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params)
{
	CIocpOverlappedData *pOvDataPtr;
	CIocpTaskData *pTaskPtr;
	DWORD nNumberOfBytesSent;

	m_PendingCounter.Inc(pCaller, ITT_SEND);

	pOvDataPtr = CreateOverlappedData(ITT_SEND, (HANDLE)hSocketHandle, pBuffer, nSize,
		nOffset, CallBackDef, pCaller, Params);
	pTaskPtr = &(pOvDataPtr->TaskData);

	if (::WSASend(hSocketHandle, &pTaskPtr->m_WSABuffer, 1, &nNumberOfBytesSent, 0,
		(LPWSAOVERLAPPED)pOvDataPtr, NULL) == SOCKET_ERROR)
	{
		if (GetLastError() != ERROR_IO_PENDING)
			PostError(GetLastError(), pOvDataPtr);
	}
}

//-----------------------------------------------------------------------------

void CIocpObject::Recv(SOCKET hSocketHandle, PVOID pBuffer, int nSize, int nOffset,
	const IOCP_CALLBACK_DEF& CallBackDef, PVOID pCaller, const CIocpParams& Params)
{
	CIocpOverlappedData *pOvDataPtr;
	CIocpTaskData *pTaskPtr;
	DWORD nNumberOfBytesRecvd, nFlags = 0;

	m_PendingCounter.Inc(pCaller, ITT_RECV);

	pOvDataPtr = CreateOverlappedData(ITT_RECV, (HANDLE)hSocketHandle, pBuffer, nSize,
		nOffset, CallBackDef, pCaller, Params);
	pTaskPtr = &(pOvDataPtr->TaskData);

	if (::WSARecv(hSocketHandle, &pTaskPtr->m_WSABuffer, 1, &nNumberOfBytesRecvd, &nFlags,
		(LPWSAOVERLAPPED)pOvDataPtr, NULL) == SOCKET_ERROR)
	{
		if (GetLastError() != ERROR_IO_PENDING)
			PostError(GetLastError(), pOvDataPtr);
	}
}

//-----------------------------------------------------------------------------

void CIocpObject::SendTo(SOCKET hSocketHandle, const CPeerAddress& PeerAddr,
	PVOID pBuffer, int nSize, int nOffset, const IOCP_CALLBACK_DEF& CallBackDef,
	PVOID pCaller, const CIocpParams& Params)
{
	CIocpOverlappedData *pOvDataPtr;
	CIocpTaskData *pTaskPtr;
	DWORD nNumberOfBytesSent;

	SOCK_ADDR SockAddr;
	GetSocketAddr(SockAddr, PeerAddr.nIp, PeerAddr.nPort);

	m_PendingCounter.Inc(pCaller, ITT_SEND);

	pOvDataPtr = CreateOverlappedData(ITT_SEND, (HANDLE)hSocketHandle, pBuffer, nSize,
		nOffset, CallBackDef, pCaller, Params);
	pTaskPtr = &(pOvDataPtr->TaskData);

	if (::WSASendTo(hSocketHandle, &pTaskPtr->m_WSABuffer, 1, &nNumberOfBytesSent, 0,
		(sockaddr*)&SockAddr, sizeof(SockAddr),
		(LPWSAOVERLAPPED)pOvDataPtr, NULL) == SOCKET_ERROR)
	{
		if (GetLastError() != ERROR_IO_PENDING)
			PostError(GetLastError(), pOvDataPtr);
	}
}

//-----------------------------------------------------------------------------

void CIocpObject::WaitForComplete(PVOID pCaller)
{
	while (m_PendingCounter.Get(pCaller) > 0)
		Sleep(1);
}

//-----------------------------------------------------------------------------

bool CIocpObject::IsComplete(PVOID pCaller)
{
	return (m_PendingCounter.Get(pCaller) <= 0);
}

//-----------------------------------------------------------------------------

bool CIocpObject::IsInWorkerThread()
{
	bool bResult = false;
	DWORD nCurThreadId = GetCurrentThreadId();

	for (int i = 0; i < m_WorkerThreads.GetCount(); i++)
		if (((CThread*)m_WorkerThreads[i])->GetThreadId() == nCurThreadId)
		{
			bResult = true;
			break;
		}

	return bResult;
}

//-----------------------------------------------------------------------------

int CIocpObject::GetPendingCount(PVOID pCaller)
{
	return m_PendingCounter.Get(pCaller);
}

//-----------------------------------------------------------------------------

int CIocpObject::GetPendingCount(IOCP_TASK_TYPE nTaskType)
{
	return m_PendingCounter.Get(nTaskType);
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
