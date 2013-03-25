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

/// @file ifc_classes.cpp

#include "stdafx.h"
#include "ifc_classes.h"
#include "ifc_errmsgs.h"
#include "ifc_sysutils.h"
#include "ifc_sync_objs.h"

#include <math.h>
#include <Imm.h>

#pragma comment(lib, "imm32.lib")

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

// Returns the global object of CScreen.
CScreen& Screen()
{
	return CScreen::Instance();
}

//-----------------------------------------------------------------------------

// Returns the global object of CClipboard.
CClipboard& Clipboard()
{
	return CClipboard::Instance();
}

//-----------------------------------------------------------------------------

// Returns the global object of CLogger.
CLogger& Logger()
{
	return CLogger::Instance();
}

///////////////////////////////////////////////////////////////////////////////
// CBuffer

//-----------------------------------------------------------------------------

CBuffer::CBuffer()
{
	Init();
}

//-----------------------------------------------------------------------------

CBuffer::CBuffer(const CBuffer& src)
{
	Init();
	Assign(src);
}

//-----------------------------------------------------------------------------

CBuffer::CBuffer(int nSize)
{
	Init();
	SetSize(nSize);
}

//-----------------------------------------------------------------------------

CBuffer::CBuffer(const void *pBuffer, int nSize)
{
	Init();
	Assign(pBuffer, nSize);
}

//-----------------------------------------------------------------------------

CBuffer::~CBuffer()
{
	if (m_pBuffer)
		free(m_pBuffer);
}

//-----------------------------------------------------------------------------

void CBuffer::Assign(const CBuffer& src)
{
	SetSize(src.GetSize());
	if (m_nSize > 0)
		memmove(m_pBuffer, src.m_pBuffer, m_nSize);
}

//-----------------------------------------------------------------------------

CBuffer& CBuffer::operator = (const CBuffer& rhs)
{
	if (this != &rhs)
		Assign(rhs);
	return *this;
}

//-----------------------------------------------------------------------------

void CBuffer::SetSize(int nSize, bool bInitZero)
{
	if (nSize <= 0)
	{
		if (m_pBuffer) free(m_pBuffer);
		m_pBuffer = NULL;
		m_nSize = 0;
	}
	else if (nSize != m_nSize)
	{
		void *pNewBuf;

		// The realloc is identical to malloc when m_pBuffer == NULL.
		pNewBuf = realloc(m_pBuffer, nSize + 1);  // The additional byte is used for c_str().

		if (pNewBuf)
		{
			if (bInitZero && (nSize > m_nSize))
				memset(((char*)pNewBuf) + m_nSize, 0, nSize - m_nSize);
			m_pBuffer = pNewBuf;
			m_nSize = nSize;
		}
		else
			IfcThrowMemoryException();
	}
}

//-----------------------------------------------------------------------------

char* CBuffer::c_str() const
{
	if (m_nSize <= 0 || !m_pBuffer)
		return "";
	else
	{
		((char*)m_pBuffer)[m_nSize] = 0;
		return (char*)m_pBuffer;
	}
}

//-----------------------------------------------------------------------------

void CBuffer::Assign(const void *pBuffer, int nSize)
{
	SetSize(nSize);
	if (m_nSize > 0)
		memmove(m_pBuffer, pBuffer, m_nSize);
}

//-----------------------------------------------------------------------------

bool CBuffer::LoadFromStream(CStream& Stream)
{
	try
	{
		INT64 nSize64 = Stream.GetSize() - Stream.GetPosition();
		IFC_ASSERT(nSize64 <= MAXLONG);
		int nSize = (int)nSize64;

		SetSize(nSize);
		Stream.ReadBuffer(Data(), nSize);
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		return false;
	}
}

//-----------------------------------------------------------------------------

bool CBuffer::LoadFromFile(LPCTSTR lpszFileName)
{
	CFileStream fs;
	bool bResult = fs.Open(lpszFileName, FM_OPEN_READ | FM_SHARE_DENY_WRITE);
	if (bResult)
		bResult = LoadFromStream(fs);
	return bResult;
}

//-----------------------------------------------------------------------------

bool CBuffer::SaveToStream(CStream& Stream) const
{
	try
	{
		if (GetSize() > 0)
			Stream.WriteBuffer(Data(), GetSize());
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		return false;
	}
}

//-----------------------------------------------------------------------------

bool CBuffer::SaveToFile(LPCTSTR lpszFileName) const
{
	CFileStream fs;
	bool bResult = fs.Open(lpszFileName, FM_CREATE | FM_SHARE_DENY_WRITE);
	if (bResult)
		bResult = SaveToStream(fs);
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// CStream

//-----------------------------------------------------------------------------

void CStream::ReadBuffer(void *pBuffer, int nBytes)
{
	if (nBytes != 0 && Read(pBuffer, nBytes) != nBytes)
		IfcThrowStreamException(SEM_STREAM_READ_ERROR);
}

//-----------------------------------------------------------------------------

void CStream::WriteBuffer(const void *pBuffer, int nBytes)
{
	if (nBytes != 0 && Write(pBuffer, nBytes) != nBytes)
		IfcThrowStreamException(SEM_STREAM_WRITE_ERROR);
}

//-----------------------------------------------------------------------------

INT64 CStream::CopyFrom(CStream& Source, INT64 nBytes)
{
	const int MAX_BUF_SIZE = 0xF000;

	INT64 nResult;
	int nBufSize, n;

	if (nBytes == 0)
	{
		Source.SetPosition(0);
		nBytes = Source.GetSize();
	}

	nResult = nBytes;
	nBufSize = (int)((nBytes > MAX_BUF_SIZE)? MAX_BUF_SIZE : nBytes);

	CBuffer Buffer(nBufSize);
	while (nBytes != 0)
	{
		n = (int)((nBytes > nBufSize)? nBufSize : nBytes);
		Source.ReadBuffer(Buffer.Data(), n);
		WriteBuffer(Buffer.Data(), n);
		nBytes -= n;
	}

	return nResult;
}

//-----------------------------------------------------------------------------

INT64 CStream::GetSize() const
{
	INT64 nPos, nResult;

	nPos = const_cast<CStream&>(*this).Seek(0, SO_CURRENT);
	nResult = const_cast<CStream&>(*this).Seek(0, SO_END);
	const_cast<CStream&>(*this).Seek(nPos, SO_BEGINNING);

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// CCustomMemoryStream

//-----------------------------------------------------------------------------

CCustomMemoryStream::CCustomMemoryStream() :
	m_pMemory(NULL),
	m_nSize(0),
	m_nPosition(0)
{
	// nothing
}

//-----------------------------------------------------------------------------

void CCustomMemoryStream::SetPointer(char *pMemory, int nSize)
{
	m_pMemory = pMemory;
	m_nSize = nSize;
}

//-----------------------------------------------------------------------------

int CCustomMemoryStream::Read(void *pBuffer, int nBytes)
{
	int nResult = 0;

	if (m_nPosition >= 0 && nBytes >= 0)
	{
		nResult = m_nSize - m_nPosition;
		if (nResult > 0)
		{
			if (nResult > nBytes) nResult = nBytes;
			memmove(pBuffer, m_pMemory + (DWORD)m_nPosition, nResult);
			m_nPosition += nResult;
		}
	}

	return nResult;
}

//-----------------------------------------------------------------------------

INT64 CCustomMemoryStream::Seek(INT64 nOffset, SEEK_ORIGIN nSeekOrigin)
{
	IFC_ASSERT(nOffset <= MAXLONG);

	switch (nSeekOrigin)
	{
	case SO_BEGINNING:
		m_nPosition = (int)nOffset;
		break;
	case SO_CURRENT:
		m_nPosition += (int)nOffset;
		break;
	case SO_END:
		m_nPosition = m_nSize + (int)nOffset;
		break;
	}

	m_nPosition = EnsureRange(m_nPosition, 0, m_nSize);
	return m_nPosition;
}

//-----------------------------------------------------------------------------

bool CCustomMemoryStream::SaveToStream(CStream& Stream)
{
	try
	{
		if (m_nSize != 0)
			Stream.WriteBuffer(m_pMemory, m_nSize);
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		return false;
	}
}

//-----------------------------------------------------------------------------

bool CCustomMemoryStream::SaveToFile(LPCTSTR lpszFileName)
{
	CFileStream fs;
	bool bResult = fs.Open(lpszFileName, FM_CREATE | FM_SHARE_DENY_WRITE);
	if (bResult)
		bResult = SaveToStream(fs);
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// CMemoryStream

//-----------------------------------------------------------------------------

CMemoryStream::CMemoryStream(int nMemoryDelta) :
	m_nCapacity(0)
{
	SetMemoryDelta(nMemoryDelta);
}

//-----------------------------------------------------------------------------

CMemoryStream::~CMemoryStream()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CMemoryStream::SetMemoryDelta(int nNewMemoryDelta)
{
	if (nNewMemoryDelta != DEFAULT_MEMORY_DELTA)
	{
		// Ensure the result value is 2^N
		nNewMemoryDelta = Max(nNewMemoryDelta, (int)MIN_MEMORY_DELTA);
		nNewMemoryDelta = (1 << (int)(log((double)nNewMemoryDelta) / log(2.0)));
	}

	m_nMemoryDelta = nNewMemoryDelta;
}

//-----------------------------------------------------------------------------

void CMemoryStream::SetCapacity(int nNewCapacity)
{
	SetPointer(Realloc(nNewCapacity), m_nSize);
	m_nCapacity = nNewCapacity;
}

//-----------------------------------------------------------------------------

char* CMemoryStream::Realloc(int& nNewCapacity)
{
	char* pResult;

	if (nNewCapacity > 0 && nNewCapacity != m_nSize)
		nNewCapacity = (nNewCapacity + (m_nMemoryDelta - 1)) & ~(m_nMemoryDelta - 1);

	pResult = m_pMemory;
	if (nNewCapacity != m_nCapacity)
	{
		if (nNewCapacity == 0)
		{
			free(m_pMemory);
			pResult = NULL;
		}
		else
		{
			if (m_nCapacity == 0)
				pResult = (char*)malloc(nNewCapacity);
			else
				pResult = (char*)realloc(m_pMemory, nNewCapacity);

			if (!pResult)
				IfcThrowMemoryException();
		}
	}

	return pResult;
}

//-----------------------------------------------------------------------------

int CMemoryStream::Write(const void *pBuffer, int nBytes)
{
	int nResult = 0;

	if (m_nPosition >= 0 && nBytes >= 0)
	{
		int nPos = m_nPosition + nBytes;
		if (nPos > 0)
		{
			if (nPos > m_nSize)
			{
				if (nPos > m_nCapacity)
					SetCapacity(nPos);
				m_nSize = nPos;
			}
			memmove(m_pMemory + (DWORD)m_nPosition, pBuffer, nBytes);
			m_nPosition = nPos;
			nResult = nBytes;
		}
	}

	return nResult;
}

//-----------------------------------------------------------------------------

void CMemoryStream::SetSize(INT64 nSize)
{
	IFC_ASSERT(nSize <= MAXLONG);

	int nOldPos = m_nPosition;

	SetCapacity((int)nSize);
	m_nSize = (int)nSize;
	if (nOldPos > nSize) Seek(0, SO_END);
}

//-----------------------------------------------------------------------------

bool CMemoryStream::LoadFromStream(CStream& Stream)
{
	try
	{
		INT64 nBytes = Stream.GetSize();
		IFC_ASSERT(nBytes <= MAXLONG);

		Stream.SetPosition(0);
		SetSize(nBytes);
		if (nBytes > 0)
			Stream.ReadBuffer(m_pMemory, (int)nBytes);
		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		return false;
	}
}

//-----------------------------------------------------------------------------

bool CMemoryStream::LoadFromFile(LPCTSTR lpszFileName)
{
	CFileStream fs;
	bool bResult = fs.Open(lpszFileName, FM_OPEN_READ | FM_SHARE_DENY_WRITE);
	if (bResult)
		bResult = LoadFromStream(fs);
	return bResult;
}

//-----------------------------------------------------------------------------

void CMemoryStream::Clear()
{
	SetCapacity(0);
	m_nSize = 0;
	m_nPosition = 0;
}

///////////////////////////////////////////////////////////////////////////////
// CFileStream

//-----------------------------------------------------------------------------

CFileStream::CFileStream()
{
	Init();
}

//-----------------------------------------------------------------------------

CFileStream::CFileStream(LPCTSTR lpszFileName, DWORD nOpenMode)
{
	Init();

	CIfcFileException e;
	if (!Open(lpszFileName, nOpenMode, &e))
		IfcThrowFileException(lpszFileName, e.m_nCause, e.m_nOsError);
}

//-----------------------------------------------------------------------------

CFileStream::~CFileStream()
{
	Close();
}

//-----------------------------------------------------------------------------

void CFileStream::Init()
{
	m_strFileName.Empty();
	m_hHandle = INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------

HANDLE CFileStream::FileCreate(LPCTSTR lpszFileName, DWORD nOpenMode)
{
	DWORD nShareModes[5] = {
		0,
		0,
		FILE_SHARE_READ,
		FILE_SHARE_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE
	};

	HANDLE hFileHandle = INVALID_HANDLE_VALUE;
	if ((nOpenMode & 0xF0) <=  FM_SHARE_DENY_NONE)
	{
		hFileHandle = ::CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE,
			nShareModes[(nOpenMode & 0xF0) >> 4], NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	}

	return hFileHandle;
}

//-----------------------------------------------------------------------------

HANDLE CFileStream::FileOpen(LPCTSTR lpszFileName, DWORD nOpenMode)
{
	DWORD nAccessModes[3] = {
		GENERIC_READ,
		GENERIC_WRITE,
		GENERIC_READ | GENERIC_WRITE
	};
	DWORD nShareModes[5] = {
		0,
		0,
		FILE_SHARE_READ,
		FILE_SHARE_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE
	};

	HANDLE hFileHandle = INVALID_HANDLE_VALUE;

	if ((nOpenMode & 3) <= FM_OPEN_READ_WRITE && (nOpenMode & 0xF0) <= FM_SHARE_DENY_NONE)
	{
		hFileHandle = ::CreateFile(lpszFileName, nAccessModes[nOpenMode & 3],
			nShareModes[(nOpenMode & 0xF0) >> 4], NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	}

	return hFileHandle;
}

//-----------------------------------------------------------------------------

void CFileStream::FileClose(HANDLE hHandle)
{
	::CloseHandle(hHandle);
}

//-----------------------------------------------------------------------------

int CFileStream::FileRead(HANDLE hHandle, void *pBuffer, int nBytes)
{
	DWORD nResult;
	if (!::ReadFile(hHandle, pBuffer, nBytes, &nResult, NULL))
		nResult = -1;
	return nResult;
}

//-----------------------------------------------------------------------------

int CFileStream::FileWrite(HANDLE hHandle, const void *pBuffer, int nBytes)
{
	DWORD nResult;
	if (!::WriteFile(hHandle, pBuffer, nBytes, &nResult, NULL))
		nResult = -1;
	return nResult;
}

//-----------------------------------------------------------------------------

INT64 CFileStream::FileSeek(HANDLE hHandle, INT64 nOffset, SEEK_ORIGIN nSeekOrigin)
{
	INT64 nResult = nOffset;
	((INT64_REC*)&nResult)->ints.lo = ::SetFilePointer(
		hHandle, ((INT64_REC*)&nResult)->ints.lo,
		(PLONG)&(((INT64_REC*)&nResult)->ints.hi), nSeekOrigin);
	if (((INT64_REC*)&nResult)->ints.lo == -1 && GetLastError() != 0)
		((INT64_REC*)&nResult)->ints.hi = -1;
	return nResult;
}

//-----------------------------------------------------------------------------

bool CFileStream::Open(LPCTSTR lpszFileName, DWORD nOpenMode, CIfcFileException* pException)
{
	Close();

	if ((nOpenMode & FM_CREATE) == FM_CREATE)
	{
		DWORD nShareMode = nOpenMode & 0xFF;
		if (nShareMode == 0xFF)
			nShareMode = FM_SHARE_EXCLUSIVE;
		m_hHandle = FileCreate(lpszFileName, nShareMode);
	}
	else
		m_hHandle = FileOpen(lpszFileName, nOpenMode);

	bool bResult = (m_hHandle != INVALID_HANDLE_VALUE);

	if (!bResult && pException != NULL)
	{
		pException->m_strFileName = lpszFileName;
		pException->m_nOsError = ::GetLastError();
		pException->m_nCause = CIfcFileException::OsErrorToCause(pException->m_nOsError);
	}

	m_strFileName = lpszFileName;
	return bResult;
}

//-----------------------------------------------------------------------------

void CFileStream::Close()
{
	if (m_hHandle != INVALID_HANDLE_VALUE)
	{
		FileClose(m_hHandle);
		m_hHandle = INVALID_HANDLE_VALUE;
	}
	m_strFileName.Empty();
}

//-----------------------------------------------------------------------------

int CFileStream::Read(void *pBuffer, int nBytes)
{
	int nResult;

	nResult = FileRead(m_hHandle, pBuffer, nBytes);
	if (nResult == -1) nResult = 0;

	return nResult;
}

//-----------------------------------------------------------------------------

int CFileStream::Write(const void *pBuffer, int nBytes)
{
	int nResult;

	nResult = FileWrite(m_hHandle, pBuffer, nBytes);
	if (nResult == -1) nResult = 0;

	return nResult;
}

//-----------------------------------------------------------------------------

INT64 CFileStream::Seek(INT64 nOffset, SEEK_ORIGIN nSeekOrigin)
{
	return FileSeek(m_hHandle, nOffset, nSeekOrigin);
}

//-----------------------------------------------------------------------------

void CFileStream::SetSize(INT64 nSize)
{
	bool bSuccess;
	Seek(nSize, SO_BEGINNING);

	bSuccess = SetEndOfFile(m_hHandle) != 0;

	if (!bSuccess)
		IfcThrowFileException(m_strFileName, ::GetLastError());
}

//-----------------------------------------------------------------------------

bool CFileStream::IsOpen() const
{
	return (m_hHandle != INVALID_HANDLE_VALUE);
}

///////////////////////////////////////////////////////////////////////////////
// CResourceStream

//-----------------------------------------------------------------------------

CResourceStream::CResourceStream(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType) :
	m_hResInfo(0),
	m_hGlobal(0)
{
	Initialize(hInstance, (PVOID)lpszResName, lpszResType, false);
}

//-----------------------------------------------------------------------------

CResourceStream::CResourceStream(HINSTANCE hInstance, int nResId, LPCTSTR lpszResType) :
	m_hResInfo(0),
	m_hGlobal(0)
{
	Initialize(hInstance, (PVOID)nResId, lpszResType, true);
}

//-----------------------------------------------------------------------------

CResourceStream::~CResourceStream()
{
	UnlockResource(m_hGlobal);
	FreeResource(m_hGlobal);
}

//-----------------------------------------------------------------------------

void CResourceStream::Initialize(HINSTANCE hInstance, PVOID pResName, LPCTSTR lpszResType, bool bIsFromId)
{
	m_hResInfo = ::FindResource(hInstance, (LPCTSTR)pResName, lpszResType);
	if (m_hResInfo == 0) IfcThrowException(SEM_RESOURCE_ERROR);
	m_hGlobal = ::LoadResource(hInstance, m_hResInfo);
	if (m_hGlobal == 0) IfcThrowException(SEM_RESOURCE_ERROR);
	SetPointer((char*)::LockResource(m_hGlobal), ::SizeofResource(hInstance, m_hResInfo));
}

//-----------------------------------------------------------------------------

int CResourceStream::Write(const void *pBuffer, int nBytes)
{
	IfcThrowStreamException(SEM_CANNOT_WRITE_RES_STREAM);
	return 0;
}

//-----------------------------------------------------------------------------

void CResourceStream::SetSize(INT64 nSize)
{
	IfcThrowStreamException(SEM_CANNOT_WRITE_RES_STREAM);
}

///////////////////////////////////////////////////////////////////////////////
// CSeqNumberAlloc

//-----------------------------------------------------------------------------

CSeqNumberAlloc::CSeqNumberAlloc(DWORD nStartId)
{
	m_nCurrentId = nStartId;
}

//-----------------------------------------------------------------------------

DWORD CSeqNumberAlloc::AllocId() const
{
	return InterlockedIncrement((LONG volatile *)&m_nCurrentId) - 1;
}

///////////////////////////////////////////////////////////////////////////////
// CPointerList

//-----------------------------------------------------------------------------

CPointerList::CPointerList()
{
	Init();
}

//-----------------------------------------------------------------------------

CPointerList::CPointerList(const CPointerList& src)
{
	Init();
	*this = src;
}

//-----------------------------------------------------------------------------

CPointerList::CPointerList(int nCount, ...)
{
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

CPointerList::~CPointerList()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CPointerList::Init()
{
	m_pList = NULL;
	m_nCount = 0;
	m_nCapacity = 0;
}

//-----------------------------------------------------------------------------

void CPointerList::Grow()
{
	int nDelta;

	if (m_nCapacity > 64)
		nDelta = m_nCapacity / 4;
	else if (m_nCapacity > 8)
		nDelta = 16;
	else
		nDelta = 4;

	SetCapacity(m_nCapacity + nDelta);
}

//-----------------------------------------------------------------------------

PVOID CPointerList::Get(int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_nCount)
		ThrowIndexError(nIndex);

	return m_pList[nIndex];
}

//-----------------------------------------------------------------------------

void CPointerList::Put(int nIndex, PVOID pItem)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		ThrowIndexError(nIndex);

	m_pList[nIndex] = pItem;
}

//-----------------------------------------------------------------------------

void CPointerList::SetCount(int nNewCount)
{
	if (nNewCount < 0)
		IfcThrowException(SEM_LIST_COUNT_ERROR);

	if (nNewCount > m_nCapacity)
		SetCapacity(nNewCount);
	if (nNewCount > m_nCount)
		memset(&m_pList[m_nCount], 0, (nNewCount - m_nCount) * sizeof(PVOID));
	else
	{
		for (int i = m_nCount - 1; i >= nNewCount; i--)
			Delete(i);
	}

	m_nCount = nNewCount;
}

//-----------------------------------------------------------------------------

void CPointerList::QuickSort(PVOID *pList, int l, int r, LIST_COMPARE_PROC pfnCompareProc)
{
	int i, j;
	PVOID p, t;

	do
	{
		i = l;
		j = r;
		p = pList[(l + r) >> 1];
		do
		{
			while (pfnCompareProc(pList[i], p) < 0) i++;
			while (pfnCompareProc(pList[j], p) > 0) j--;
			if (i <= j)
			{
				t = pList[i];
				pList[i] = pList[j];
				pList[j] = t;
				i++;
				j--;
			}
		}
		while (i <= j);

		if (l < j)
			QuickSort(pList, l, j, pfnCompareProc);
		l = i;
	}
	while (i < r);
}

//-----------------------------------------------------------------------------

void CPointerList::ThrowIndexError(int nIndex) const
{
	CString strMsg;
	strMsg.Format(SEM_LIST_INDEX_ERROR, nIndex);
	IfcThrowException(strMsg);
}

//-----------------------------------------------------------------------------

int CPointerList::Add(PVOID pItem)
{
	if (m_nCount == m_nCapacity) Grow();
	m_pList[m_nCount] = pItem;
	m_nCount++;

	return m_nCount - 1;
}

//-----------------------------------------------------------------------------

void CPointerList::Insert(int nIndex, PVOID pItem)
{
	if (nIndex < 0 || nIndex > m_nCount)
		ThrowIndexError(nIndex);

	if (m_nCount == m_nCapacity) Grow();
	if (nIndex < m_nCount)
		memmove(&m_pList[nIndex + 1], &m_pList[nIndex], (m_nCount - nIndex) * sizeof(PVOID));
	m_pList[nIndex] = pItem;
	m_nCount++;
}

//-----------------------------------------------------------------------------

void CPointerList::Delete(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		ThrowIndexError(nIndex);

	m_nCount--;
	if (nIndex < m_nCount)
		memmove(&m_pList[nIndex], &m_pList[nIndex + 1], (m_nCount - nIndex) * sizeof(PVOID));
}

//-----------------------------------------------------------------------------

int CPointerList::Remove(PVOID pItem)
{
	int nResult;

	nResult = IndexOf(pItem);
	if (nResult >= 0)
		Delete(nResult);

	return nResult;
}

//-----------------------------------------------------------------------------

PVOID CPointerList::Extract(PVOID pItem)
{
	int i;
	PVOID pResult = NULL;

	i = IndexOf(pItem);
	if (i >= 0)
	{
		pResult = pItem;
		m_pList[i] = NULL;
		Delete(i);
	}

	return pResult;
}

//-----------------------------------------------------------------------------

void CPointerList::Move(int nCurIndex, int nNewIndex)
{
	PVOID pItem;

	if (nCurIndex != nNewIndex)
	{
		if (nNewIndex < 0 || nNewIndex >= m_nCount)
			ThrowIndexError(nNewIndex);

		pItem = Get(nCurIndex);
		m_pList[nCurIndex] = NULL;
		Delete(nCurIndex);
		Insert(nNewIndex, NULL);
		m_pList[nNewIndex] = pItem;
	}
}

//-----------------------------------------------------------------------------

void CPointerList::Resize(int nCount)
{
	SetCount(nCount);
}

//-----------------------------------------------------------------------------

void CPointerList::Clear()
{
	SetCount(0);
	SetCapacity(0);
}

//-----------------------------------------------------------------------------

void CPointerList::Sort(LIST_COMPARE_PROC pfnCompareProc)
{
	if (m_pList != NULL && m_nCount > 0)
		QuickSort(m_pList, 0, m_nCount - 1, pfnCompareProc);
}

//-----------------------------------------------------------------------------

PVOID CPointerList::First() const
{
	return Get(0);
}

//-----------------------------------------------------------------------------

PVOID CPointerList::Last() const
{
	return Get(m_nCount - 1);
}

//-----------------------------------------------------------------------------

int CPointerList::IndexOf(PVOID pItem) const
{
	int nResult = 0;

	while (nResult < m_nCount && m_pList[nResult] != pItem) nResult++;
	if (nResult == m_nCount)
		nResult = -1;

	return nResult;
}

//-----------------------------------------------------------------------------

void CPointerList::SetCapacity(int nNewCapacity)
{
	if (nNewCapacity < m_nCount)
		IfcThrowException(SEM_LIST_CAPACITY_ERROR);

	if (nNewCapacity != m_nCapacity)
	{
		if (nNewCapacity > 0)
		{
			PVOID *p = (PVOID*)realloc(m_pList, nNewCapacity * sizeof(PVOID));
			if (p)
				m_pList = p;
			else
				IfcThrowMemoryException();
		}
		else
		{
			if (m_pList)
			{
				free(m_pList);
				m_pList = NULL;
			}
		}

		m_nCapacity = nNewCapacity;
	}
}

//-----------------------------------------------------------------------------

CPointerList& CPointerList::operator = (const CPointerList& rhs)
{
	if (this == &rhs) return *this;

	m_nCount = 0;
	SetCapacity(rhs.GetCapacity());
	if (rhs.m_nCount > 0)
	{
		m_nCount = rhs.m_nCount;
		memmove(m_pList, rhs.m_pList, m_nCount * sizeof(PVOID));
	}

	return *this;
}

//-----------------------------------------------------------------------------

const PVOID& CPointerList::operator[] (int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_nCount)
		ThrowIndexError(nIndex);

	return m_pList[nIndex];
}

//-----------------------------------------------------------------------------

PVOID& CPointerList::operator[] (int nIndex)
{
	return
		const_cast<PVOID&>(
			((const CPointerList&)(*this))[nIndex]
		);
}

///////////////////////////////////////////////////////////////////////////////
// CStrings

//-----------------------------------------------------------------------------

CStrings::CStrings()
{
	Init();
}

//-----------------------------------------------------------------------------

void CStrings::Assign(const CStrings& src)
{
	CAutoUpdater AutoUpdater(*this);
	Clear();

	m_nDefined = src.m_nDefined;
	m_chDelimiter = src.m_chDelimiter;
	m_strLineBreak = src.m_strLineBreak;
	m_chQuoteChar = src.m_chQuoteChar;
	m_chNameValueSeparator = src.m_chNameValueSeparator;

	AddStrings(src);
}

//-----------------------------------------------------------------------------

void CStrings::Init()
{
	m_nDefined = 0;
	m_chDelimiter = 0;
	m_chQuoteChar = 0;
	m_chNameValueSeparator = 0;
	m_nUpdateCount = 0;
}

//-----------------------------------------------------------------------------

void CStrings::Error(LPCTSTR lpszMsg, int nData) const
{
	CString strMsg;
	strMsg.Format(lpszMsg, nData);

	IfcThrowException(strMsg);
}

//-----------------------------------------------------------------------------

CString CStrings::ExtractName(LPCTSTR lpszStr) const
{
	CString strResult(lpszStr);

	int i = strResult.Find(GetNameValueSeparator());
	if (i >= 0)
		strResult = strResult.Left(i);
	else
		strResult.Empty();

	return strResult;
}

//-----------------------------------------------------------------------------

int CStrings::CompareStrings(LPCTSTR str1, LPCTSTR str2) const
{
	return ::CompareString(LOCALE_USER_DEFAULT, NORM_IGNORECASE, str1, (int)_tcsclen(str1),
		str2, (int)_tcsclen(str2)) - 2;
}

//-----------------------------------------------------------------------------

int CStrings::Add(LPCTSTR lpszStr)
{
	int nResult = GetCount();
	Insert(nResult, lpszStr);
	return nResult;
}

//-----------------------------------------------------------------------------

int CStrings::Add(LPCTSTR lpszStr, PVOID pData)
{
	int nResult = Add(lpszStr);
	SetData(nResult, pData);
	return nResult;
}

//-----------------------------------------------------------------------------

void CStrings::AddStrings(const CStrings& Strings)
{
	CAutoUpdater AutoUpdater(*this);

	for (int i = 0; i < Strings.GetCount(); i++)
		Add(Strings.GetString(i), Strings.GetData(i));
}

//-----------------------------------------------------------------------------

void CStrings::Insert(int nIndex, LPCTSTR lpszStr, PVOID pData)
{
	Insert(nIndex, lpszStr);
	SetData(nIndex, pData);
}

//-----------------------------------------------------------------------------

bool CStrings::Equals(const CStrings& Strings)
{
	bool bResult;
	int nCount = GetCount();

	bResult = (nCount == Strings.GetCount());
	if (bResult)
	{
		for (int i = 0; i < nCount; i++)
			if (GetString(i) != Strings.GetString(i))
			{
				bResult = false;
				break;
			}
	}

	return bResult;
}

//-----------------------------------------------------------------------------

void CStrings::Exchange(int nIndex1, int nIndex2)
{
	CAutoUpdater AutoUpdater(*this);

	PVOID pTempData;
	CString strTempStr;

	strTempStr = GetString(nIndex1);
	pTempData = GetData(nIndex1);
	SetString(nIndex1, GetString(nIndex2));
	SetData(nIndex1, GetData(nIndex2));
	SetString(nIndex2, strTempStr);
	SetData(nIndex2, pTempData);
}

//-----------------------------------------------------------------------------

void CStrings::Move(int nCurIndex, int nNewIndex)
{
	if (nCurIndex != nNewIndex)
	{
		CAutoUpdater AutoUpdater(*this);

		PVOID pTempData;
		CString strTempStr;

		strTempStr = GetString(nCurIndex);
		pTempData = GetData(nCurIndex);
		Delete(nCurIndex);
		Insert(nNewIndex, strTempStr, pTempData);
	}
}

//-----------------------------------------------------------------------------

bool CStrings::Exists(LPCTSTR lpszStr) const
{
	return (IndexOf(lpszStr) >= 0);
}

//-----------------------------------------------------------------------------

int CStrings::IndexOf(LPCTSTR lpszStr) const
{
	int nResult = -1;

	for (int i = 0; i < GetCount(); i++)
		if (CompareStrings(GetString(i), lpszStr) == 0)
		{
			nResult = i;
			break;
		}

	return nResult;
}

//-----------------------------------------------------------------------------

int CStrings::IndexOfName(LPCTSTR lpszName) const
{
	int nResult = -1;

	for (int i = 0; i < GetCount(); i++)
		if (CompareStrings(ExtractName(GetString(i)), lpszName) == 0)
		{
			nResult = i;
			break;
		}

	return nResult;
}

//-----------------------------------------------------------------------------

int CStrings::IndexOfData(PVOID pData) const
{
	int nResult = -1;

	for (int i = 0; i < GetCount(); i++)
		if (GetData(i) == pData)
		{
			nResult = i;
			break;
		}

	return nResult;
}

//-----------------------------------------------------------------------------

bool CStrings::LoadFromStream(CStream& Stream, bool bAsUnicode)
{
	try
	{
		CAutoUpdater AutoUpdater(*this);

		INT64 nSize64 = Stream.GetSize() - Stream.GetPosition();
		IFC_ASSERT(nSize64 <= MAXLONG);
		int nSize = (int)nSize64;

		CBuffer Buffer(nSize + sizeof(TCHAR));
		Stream.ReadBuffer(Buffer, nSize);
		*((TCHAR*)(Buffer.Data() + nSize)) = '\0';

		if (bAsUnicode)
			SetText(CW2T((LPCWSTR)(PVOID)Buffer));
		else
			SetText(CA2T((LPCSTR)(PVOID)Buffer));

		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		return false;
	}
}

//-----------------------------------------------------------------------------

bool CStrings::LoadFromFile(LPCTSTR lpszFileName, bool bAsUnicode)
{
	CFileStream fs;
	bool bResult = fs.Open(lpszFileName, FM_OPEN_READ | FM_SHARE_DENY_WRITE);
	if (bResult)
		bResult = LoadFromStream(fs, bAsUnicode);
	return bResult;
}

//-----------------------------------------------------------------------------

bool CStrings::SaveToStream(CStream& Stream, bool bAsUnicode) const
{
	try
	{
		CString strText = GetText();

		if (bAsUnicode)
		{
			CT2W s(strText);
			int nLen = (int)wcslen(s);
			Stream.WriteBuffer(s, nLen * sizeof(WCHAR));
		}
		else
		{
			CT2A s(strText);
			int nLen = (int)strlen(s);
			Stream.WriteBuffer(s, nLen * sizeof(CHAR));
		}

		return true;
	}
	catch (CException* e)
	{
		e->Delete();
		return false;
	}
}

//-----------------------------------------------------------------------------

bool CStrings::SaveToFile(LPCTSTR lpszFileName, bool bAsUnicode) const
{
	CFileStream fs;
	bool bResult = fs.Open(lpszFileName, FM_CREATE | FM_SHARE_DENY_WRITE);
	if (bResult)
		bResult = SaveToStream(fs, bAsUnicode);
	return bResult;
}

//-----------------------------------------------------------------------------

TCHAR CStrings::GetDelimiter() const
{
	if ((m_nDefined & SD_DELIMITER) == 0)
		const_cast<CStrings&>(*this).SetDelimiter(DEFAULT_DELIMITER);
	return m_chDelimiter;
}

//-----------------------------------------------------------------------------

void CStrings::SetDelimiter(TCHAR chValue)
{
	if (m_chDelimiter != chValue || (m_nDefined & SD_DELIMITER) == 0)
	{
		m_nDefined |= SD_DELIMITER;
		m_chDelimiter = chValue;
	}
}

//-----------------------------------------------------------------------------

CString CStrings::GetLineBreak() const
{
	if ((m_nDefined & SD_LINE_BREAK) == 0)
		const_cast<CStrings&>(*this).SetLineBreak(DEFAULT_LINE_BREAK);
	return m_strLineBreak;
}

//-----------------------------------------------------------------------------

void CStrings::SetLineBreak(LPCTSTR lpszValue)
{
	if (m_strLineBreak != CString(lpszValue) || (m_nDefined & SD_LINE_BREAK) == 0)
	{
		m_nDefined |= SD_LINE_BREAK;
		m_strLineBreak = lpszValue;
	}
}

//-----------------------------------------------------------------------------

TCHAR CStrings::GetQuoteChar() const
{
	if ((m_nDefined & SD_QUOTE_CHAR) == 0)
		const_cast<CStrings&>(*this).SetQuoteChar(DEFAULT_QUOTE_CHAR);
	return m_chQuoteChar;
}

//-----------------------------------------------------------------------------

void CStrings::SetQuoteChar(TCHAR chValue)
{
	if (m_chQuoteChar != chValue || (m_nDefined & SD_QUOTE_CHAR) == 0)
	{
		m_nDefined |= SD_QUOTE_CHAR;
		m_chQuoteChar = chValue;
	}
}

//-----------------------------------------------------------------------------

TCHAR CStrings::GetNameValueSeparator() const
{
	if ((m_nDefined & SD_NAME_VALUE_SEP) == 0)
		const_cast<CStrings&>(*this).SetNameValueSeparator(DEFAULT_NAME_VALUE_SEP);
	return m_chNameValueSeparator;
}

//-----------------------------------------------------------------------------

void CStrings::SetNameValueSeparator(TCHAR chValue)
{
	if (m_chNameValueSeparator != chValue || (m_nDefined & SD_NAME_VALUE_SEP) == 0)
	{
		m_nDefined |= SD_NAME_VALUE_SEP;
		m_chNameValueSeparator = chValue;
	}
}

//-----------------------------------------------------------------------------

CString CStrings::CombineNameValue(LPCTSTR lpszName, LPCTSTR lpszValue) const
{
	CString strName(lpszName);
	TCHAR chNameValueSep = GetNameValueSeparator();

	if (strName.Find(chNameValueSep) >= 0)
		Error(SEM_STRINGS_NAME_ERROR, 0);

	return strName + chNameValueSep + lpszValue;
}

//-----------------------------------------------------------------------------

CString CStrings::GetName(int nIndex) const
{
	return ExtractName(GetString(nIndex));
}

//-----------------------------------------------------------------------------

CString CStrings::GetValue(LPCTSTR lpszName) const
{
	CString strName(lpszName);
	int i = IndexOfName(strName);
	if (i >= 0)
		return GetString(i).Mid(strName.GetLength() + 1);
	else
		return TEXT("");
}

//-----------------------------------------------------------------------------

CString CStrings::GetValue(int nIndex) const
{
	if (nIndex >= 0)
	{
		CString strName = GetName(nIndex);
		if (!strName.IsEmpty())
			return GetString(nIndex).Mid(strName.GetLength() + 1);
		else
		{
			CString strItem = GetString(nIndex);
			if (!strItem.IsEmpty() && strItem[0] == GetNameValueSeparator())
				return strItem.Mid(1);
			else
				return TEXT("");
		}
	}
	else
		return TEXT("");
}

//-----------------------------------------------------------------------------

void CStrings::SetValue(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CString strName(lpszName);
	CString strValue(lpszValue);

	int i = IndexOfName(lpszName);
	if (strValue.IsEmpty())
	{
		if (i >= 0) Delete(i);
	}
	else
	{
		if (i < 0)
			i = Add(TEXT(""));
		SetString(i, strName + GetNameValueSeparator() + strValue);
	}
}

//-----------------------------------------------------------------------------

void CStrings::SetValue(int nIndex, LPCTSTR lpszValue)
{
	CString strValue(lpszValue);

	if (strValue.IsEmpty())
	{
		if (nIndex >= 0) Delete(nIndex);
	}
	else
	{
		if (nIndex < 0)
			nIndex = Add(TEXT(""));
		SetString(nIndex, GetName(nIndex) + GetNameValueSeparator() + strValue);
	}
}

//-----------------------------------------------------------------------------

CString CStrings::GetText() const
{
	CString strResult;
	int nCount = GetCount();
	CString strLineBreak = GetLineBreak();

	for (int i = 0; i < nCount; i++)
	{
		strResult += GetString(i);
		strResult += strLineBreak;
	}

	return strResult;
}

//-----------------------------------------------------------------------------

void CStrings::SetText(LPCTSTR lpszValue)
{
	CAutoUpdater AutoUpdater(*this);

	CString strValue(lpszValue);
	CString strLineBreak = GetLineBreak();
	int nStart = 0, nPos;

	Clear();
	while (true)
	{
		nPos = strValue.Find(strLineBreak, nStart);
		if (nPos >= 0)
		{
			Add(strValue.Mid(nStart, nPos - nStart));
			nStart = nPos + strLineBreak.GetLength();
		}
		else
		{
			CString str = strValue.Mid(nStart);
			if (!str.IsEmpty())
				Add(str);
			break;
		}
	}
}

//-----------------------------------------------------------------------------

CString CStrings::GetCommaText() const
{
	DWORD nBakDefined = m_nDefined;
	TCHAR chBakDelimiter = m_chDelimiter;
	TCHAR chBakQuoteChar = m_chQuoteChar;

	const_cast<CStrings&>(*this).SetDelimiter(DEFAULT_DELIMITER);
	const_cast<CStrings&>(*this).SetQuoteChar(DEFAULT_QUOTE_CHAR);

	CString strResult = GetDelimitedText();  // 不可以抛出异常

	const_cast<CStrings&>(*this).m_nDefined = nBakDefined;
	const_cast<CStrings&>(*this).m_chDelimiter = chBakDelimiter;
	const_cast<CStrings&>(*this).m_chQuoteChar = chBakQuoteChar;

	return strResult;
}

//-----------------------------------------------------------------------------

void CStrings::SetCommaText(LPCTSTR lpszValue)
{
	SetDelimiter(DEFAULT_DELIMITER);
	SetQuoteChar(DEFAULT_QUOTE_CHAR);

	SetDelimitedText(lpszValue);
}

//-----------------------------------------------------------------------------

CString CStrings::GetDelimitedText() const
{
	CString strResult;
	CString strLine;
	int nCount = GetCount();
	TCHAR chQuoteChar = GetQuoteChar();
	TCHAR chDelimiter = GetDelimiter();

	if (nCount == 1 && GetString(0).IsEmpty())
		return CString(GetQuoteChar(), 2);

	CString strDelimiters;
	for (int i = 1; i <= 32; i++)
		strDelimiters += (TCHAR)i;
	strDelimiters += chQuoteChar;
	strDelimiters += chDelimiter;

	// 预分配 strResult 的存储空间
	int nResultSize = 0;
	for (int i = 0; i < nCount; i++)
		nResultSize += GetString(i).GetLength() + 1;
	strResult.Preallocate(nResultSize);

	for (int i = 0; i < nCount; i++)
	{
		strLine = GetString(i);
		if (strLine.FindOneOf(strDelimiters) >= 0)
			strLine = GetQuotedStr(strLine, chQuoteChar);

		if (i > 0) strResult += chDelimiter;
		strResult += strLine;
	}
	strResult.FreeExtra();

	return strResult;
}

//-----------------------------------------------------------------------------

void CStrings::SetDelimitedText(LPCTSTR lpszValue)
{
	CAutoUpdater AutoUpdater(*this);

	TCHAR chQuoteChar = GetQuoteChar();
	TCHAR chDelimiter = GetDelimiter();
	LPCTSTR pCur = lpszValue;
	CString strLine;

	Clear();
	while (*pCur >= 1 && *pCur <= 32)
		pCur = _tcsinc(pCur);

	while (*pCur != '\0')
	{
		if (*pCur == chQuoteChar)
			strLine = ExtractQuotedStr(pCur, chQuoteChar);
		else
		{
			LPCTSTR p = pCur;
			while (*pCur > 32 && *pCur != chDelimiter)
				pCur = _tcsinc(pCur);
			strLine.SetString(p, (int)(pCur - p));
		}

		Add(strLine);

		while (*pCur >= 1 && *pCur <= 32)
			pCur = _tcsinc(pCur);

		if (*pCur == chDelimiter)
		{
			LPCTSTR p = pCur;
			p = _tcsinc(p);
			if (*p == '\0')
				Add(TEXT(""));

			do pCur = _tcsinc(pCur); while (*pCur >= 1 && *pCur <= 32);
		}
	}
}

//-----------------------------------------------------------------------------

void CStrings::SetString(int nIndex, LPCTSTR lpszValue)
{
	PVOID pTempData = GetData(nIndex);
	Delete(nIndex);
	Insert(nIndex, lpszValue, pTempData);
}

//-----------------------------------------------------------------------------

void CStrings::BeginUpdate()
{
	if (m_nUpdateCount == 0)
		SetUpdateState(true);
	m_nUpdateCount++;
}

//-----------------------------------------------------------------------------

void CStrings::EndUpdate()
{
	m_nUpdateCount--;
	if (m_nUpdateCount == 0)
		SetUpdateState(false);
}

//-----------------------------------------------------------------------------

void CStrings::ToMfcObject(CStringArray& StringArray)
{
	StringArray.RemoveAll();
	for (int i = 0; i < GetCount(); i++)
		StringArray.Add(GetString(i));
}

//-----------------------------------------------------------------------------

void CStrings::ToMfcObject(CStringList& StringList)
{
	StringList.RemoveAll();
	for (int i = 0; i < GetCount(); i++)
		StringList.AddTail(GetString(i));
}

//-----------------------------------------------------------------------------

void CStrings::FromMfcObject(CStringArray& StringArray)
{
	CAutoUpdater AutoUpdater(*this);
	Clear();
	for (int i = 0; i < StringArray.GetCount(); i++)
		Add(StringArray[i]);
}

//-----------------------------------------------------------------------------

void CStrings::FromMfcObject(CStringList& StringList)
{
	CAutoUpdater AutoUpdater(*this);
	Clear();
	for (POSITION pos = StringList.GetHeadPosition(); pos != NULL;)
		Add(StringList.GetNext(pos));
}

//-----------------------------------------------------------------------------

CStrings& CStrings::operator = (const CStrings& rhs)
{
	if (this != &rhs)
		Assign(rhs);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// CStrList

//-----------------------------------------------------------------------------

CStrList::CStrList()
{
	Init();
}

//-----------------------------------------------------------------------------

CStrList::CStrList(const CStrList& src)
{
	Init();
	Assign(src);
}

//-----------------------------------------------------------------------------

CStrList::~CStrList()
{
	InternalClear();
}

//-----------------------------------------------------------------------------

void CStrList::Assign(const CStrList& src)
{
	CStrings::operator=(src);
}

//-----------------------------------------------------------------------------

void CStrList::Init()
{
	m_pList = NULL;
	m_nCount = 0;
	m_nCapacity = 0;
	m_nDupMode = DM_IGNORE;
	m_bSorted = false;
	m_bCaseSensitive = false;
}

//-----------------------------------------------------------------------------

void CStrList::InternalClear()
{
	SetCapacity(0);
}

//-----------------------------------------------------------------------------

CString& CStrList::StringObjectNeeded(int nIndex) const
{
	if (m_pList[nIndex].pStr == NULL)
		m_pList[nIndex].pStr = new CString();
	return *(m_pList[nIndex].pStr);
}

//-----------------------------------------------------------------------------

void CStrList::ExchangeItems(int nIndex1, int nIndex2)
{
	CStringItem Temp;

	Temp = m_pList[nIndex1];
	m_pList[nIndex1] = m_pList[nIndex2];
	m_pList[nIndex2] = Temp;
}

//-----------------------------------------------------------------------------

void CStrList::Grow()
{
	int nDelta;

	if (m_nCapacity > 64)
		nDelta = m_nCapacity / 4;
	else if (m_nCapacity > 8)
		nDelta = 16;
	else
		nDelta = 4;

	SetCapacity(m_nCapacity + nDelta);
}

//-----------------------------------------------------------------------------

void CStrList::QuickSort(int l, int r, STRINGLIST_COMPARE_PROC pfnCompareProc)
{
	int i, j, p;

	do
	{
		i = l;
		j = r;
		p = (l + r) >> 1;
		do
		{
			while (pfnCompareProc(*this, i, p) < 0) i++;
			while (pfnCompareProc(*this, j, p) > 0) j--;
			if (i <= j)
			{
				ExchangeItems(i, j);
				if (p == i)
					p = j;
				else if (p == j)
					p = i;
				i++;
				j--;
			}
		}
		while (i <= j);

		if (l < j)
			QuickSort(l, j, pfnCompareProc);
		l = i;
	}
	while (i < r);
}

//-----------------------------------------------------------------------------

void CStrList::SetUpdateState(bool bUpdating)
{
	if (bUpdating)
		OnChanging();
	else
		OnChanged();
}

//-----------------------------------------------------------------------------

int CStrList::CompareStrings(LPCTSTR str1, LPCTSTR str2) const
{
	if (m_bCaseSensitive)
		return _tcscmp(str1, str2);
	else
		return _tcsicmp(str1, str2);
}

//-----------------------------------------------------------------------------

void CStrList::InsertItem(int nIndex, LPCTSTR lpszStr, PVOID pData)
{
	OnChanging();
	if (m_nCount == m_nCapacity) Grow();
	if (nIndex < m_nCount)
	{
		memmove(m_pList + nIndex + 1, m_pList + nIndex, (m_nCount - nIndex) * sizeof(CStringItem));
		m_pList[nIndex].pStr = NULL;
	}

	StringObjectNeeded(nIndex) = lpszStr;
	m_pList[nIndex].pData = pData;

	m_nCount++;
	OnChanged();
}

//-----------------------------------------------------------------------------

int CStrList::Add(LPCTSTR lpszStr)
{
	return Add(lpszStr, NULL);
}

//-----------------------------------------------------------------------------

int CStrList::Add(LPCTSTR lpszStr, PVOID pData)
{
	int nResult;

	if (!m_bSorted)
		nResult = m_nCount;
	else
	{
		if (Find(lpszStr, nResult))
			switch (m_nDupMode)
			{
			case DM_IGNORE:
				return nResult;
			case DM_ERROR:
				Error(SEM_DUPLICATE_STRING, 0);
			}
	}

	InsertItem(nResult, lpszStr, pData);
	return nResult;
}

//-----------------------------------------------------------------------------

void CStrList::Clear()
{
	InternalClear();
}

//-----------------------------------------------------------------------------

void CStrList::Delete(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		Error(SEM_LIST_INDEX_ERROR, nIndex);

	OnChanging();

	delete m_pList[nIndex].pStr;
	m_pList[nIndex].pStr = NULL;

	m_nCount--;
	if (nIndex < m_nCount)
	{
		memmove(m_pList + nIndex, m_pList + nIndex + 1, (m_nCount - nIndex) * sizeof(CStringItem));
		m_pList[m_nCount].pStr = NULL;
	}

	OnChanged();
}

//-----------------------------------------------------------------------------

void CStrList::Exchange(int nIndex1, int nIndex2)
{
	if (nIndex1 < 0 || nIndex1 >= m_nCount)
		Error(SEM_LIST_INDEX_ERROR, nIndex1);
	if (nIndex2 < 0 || nIndex2 >= m_nCount)
		Error(SEM_LIST_INDEX_ERROR, nIndex2);

	OnChanging();
	ExchangeItems(nIndex1, nIndex2);
	OnChanged();
}

//-----------------------------------------------------------------------------

int CStrList::IndexOf(LPCTSTR lpszStr) const
{
	int nResult;

	if (!m_bSorted)
		nResult = CStrings::IndexOf(lpszStr);
	else if (!Find(lpszStr, nResult))
		nResult = -1;

	return nResult;
}

//-----------------------------------------------------------------------------

void CStrList::Insert(int nIndex, LPCTSTR lpszStr)
{
	Insert(nIndex, lpszStr, NULL);
}

//-----------------------------------------------------------------------------

void CStrList::Insert(int nIndex, LPCTSTR lpszStr, PVOID pData)
{
	if (m_bSorted)
		Error(SEM_SORTED_LIST_ERROR, 0);
	if (nIndex < 0 || nIndex > m_nCount)
		Error(SEM_LIST_INDEX_ERROR, nIndex);

	InsertItem(nIndex, lpszStr, pData);
}

//-----------------------------------------------------------------------------

void CStrList::SetCapacity(int nValue)
{
	if (nValue < 0) nValue = 0;

	for (int i = nValue; i < m_nCapacity; i++)
		delete m_pList[i].pStr;

	if (nValue > 0)
	{
		CStringItem *p = (CStringItem*)realloc(m_pList, nValue * sizeof(CStringItem));
		if (p)
			m_pList = p;
		else
			IfcThrowMemoryException();
	}
	else
	{
		if (m_pList)
		{
			free(m_pList);
			m_pList = NULL;
		}
	}

	if (m_pList != NULL)
	{
		for (int i = m_nCapacity; i < nValue; i++)
		{
			m_pList[i].pStr = NULL;   // new CString object when needed
			m_pList[i].pData = NULL;
		}
	}

	m_nCapacity = nValue;
	if (m_nCount > m_nCapacity)
		m_nCount = m_nCapacity;
}

//-----------------------------------------------------------------------------

PVOID CStrList::GetData(int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_nCount)
		Error(SEM_LIST_INDEX_ERROR, nIndex);

	return m_pList[nIndex].pData;
}

//-----------------------------------------------------------------------------

void CStrList::SetData(int nIndex, PVOID pData)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		Error(SEM_LIST_INDEX_ERROR, nIndex);

	OnChanging();
	m_pList[nIndex].pData = pData;
	OnChanged();
}

//-----------------------------------------------------------------------------

const CString& CStrList::GetString(int nIndex) const
{
	if (nIndex < 0 || nIndex >= m_nCount)
		Error(SEM_LIST_INDEX_ERROR, nIndex);

	return StringObjectNeeded(nIndex);
}

//-----------------------------------------------------------------------------

void CStrList::SetString(int nIndex, LPCTSTR lpszValue)
{
	if (m_bSorted)
		Error(SEM_SORTED_LIST_ERROR, 0);
	if (nIndex < 0 || nIndex >= m_nCount)
		Error(SEM_LIST_INDEX_ERROR, nIndex);

	OnChanging();
	StringObjectNeeded(nIndex) = lpszValue;
	OnChanged();
}

//-----------------------------------------------------------------------------

bool CStrList::Find(LPCTSTR lpszStr, int& nIndex) const
{
	if (!m_bSorted)
	{
		nIndex = IndexOf(lpszStr);
		return (nIndex >= 0);
	}

	bool bResult = false;
	int l, h, i, c;

	l = 0;
	h = m_nCount - 1;
	while (l <= h)
	{
		i = (l + h) >> 1;
		c = CompareStrings(StringObjectNeeded(i), lpszStr);
		if (c < 0)
			l = i + 1;
		else
		{
			h = i - 1;
			if (c == 0)
			{
				bResult = true;
				if (m_nDupMode != DM_ACCEPT)
					l = i;
			}
		}
	}

	nIndex = l;
	return bResult;
}

//-----------------------------------------------------------------------------

int StringListCompareProc(const CStrList& StringList, int nIndex1, int nIndex2)
{
	return StringList.CompareStrings(
		StringList.StringObjectNeeded(nIndex1),
		StringList.StringObjectNeeded(nIndex2));
}

//-----------------------------------------------------------------------------

void CStrList::Sort()
{
	Sort(StringListCompareProc);
}

//-----------------------------------------------------------------------------

void CStrList::Sort(STRINGLIST_COMPARE_PROC pfnCompareProc)
{
	if (!m_bSorted && m_nCount > 1)
	{
		OnChanging();
		QuickSort(0, m_nCount - 1, pfnCompareProc);
		OnChanged();
	}
}

//-----------------------------------------------------------------------------

void CStrList::SetSorted(bool bValue)
{
	if (bValue != m_bSorted)
	{
		if (bValue) Sort();
		m_bSorted = bValue;
	}
}

//-----------------------------------------------------------------------------

void CStrList::SetCaseSensitive(bool bValue)
{
	if (bValue != m_bCaseSensitive)
	{
		m_bCaseSensitive = bValue;
		if (m_bSorted) Sort();
	}
}

//-----------------------------------------------------------------------------

CStrList& CStrList::operator = (const CStrList& rhs)
{
	if (this != &rhs)
		Assign(rhs);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// CHashedStrList

//-----------------------------------------------------------------------------

CHashedStrList::CHashedStrList()
{
	Init();
}

//-----------------------------------------------------------------------------

CHashedStrList::CHashedStrList(const CHashedStrList& src)
{
	Init();
	Assign(src);
}

//-----------------------------------------------------------------------------

void CHashedStrList::Assign(const CHashedStrList& src)
{
	CStrList::operator=(src);
}

//-----------------------------------------------------------------------------

void CHashedStrList::Init()
{
	SetHashInvalid();
}

//-----------------------------------------------------------------------------

void CHashedStrList::SetHashInvalid()
{
	m_bItemHashValid = false;
	m_bNameHashValid = false;
}

//-----------------------------------------------------------------------------

void CHashedStrList::UpdateItemHash()
{
	if (m_bItemHashValid) return;

	CString str;
	m_ItemHash.clear();
	for (int i = 0; i < GetCount(); i++)
	{
		str = GetString(i);
		if (!GetCaseSensitive())
			str.MakeUpper();
		m_ItemHash[str] = i;
	}

	m_bItemHashValid = true;
}

//-----------------------------------------------------------------------------

void CHashedStrList::UpdateNameHash()
{
	if (m_bNameHashValid) return;

	CString str;
	m_NameHash.clear();
	for (int i = 0; i < GetCount(); i++)
	{
		str = ExtractName(GetString(i));
		if (!GetCaseSensitive())
			str.MakeUpper();
		m_NameHash[str] = i;
	}

	m_bNameHashValid = true;
}

//-----------------------------------------------------------------------------

void CHashedStrList::OnChanged()
{
	CStrList::OnChanged();
	SetHashInvalid();
}

//-----------------------------------------------------------------------------

int CHashedStrList::IndexOf(LPCTSTR lpszStr) const
{
	const_cast<CHashedStrList&>(*this).UpdateItemHash();

	CString str(lpszStr);
	if (!GetCaseSensitive())
		str.MakeUpper();

	int nResult = -1;
	STRLIST_HASH::const_iterator iter = m_ItemHash.find(str);
	if (iter != m_ItemHash.end())
		nResult = iter->second;

	return nResult;
}

//-----------------------------------------------------------------------------

int CHashedStrList::IndexOfName(LPCTSTR lpszName) const
{
	const_cast<CHashedStrList&>(*this).UpdateNameHash();

	CString str(lpszName);
	if (!GetCaseSensitive())
		str.MakeUpper();

	int nResult = -1;
	STRLIST_HASH::const_iterator iter = m_NameHash.find(str);
	if (iter != m_NameHash.end())
		nResult = iter->second;

	return nResult;
}

//-----------------------------------------------------------------------------

void CHashedStrList::SetCaseSensitive(bool bValue)
{
	if (bValue != GetCaseSensitive())
	{
		CStrList::SetCaseSensitive(bValue);
		SetHashInvalid();
	}
}

//-----------------------------------------------------------------------------

CHashedStrList& CHashedStrList::operator = (const CHashedStrList& rhs)
{
	if (this != &rhs)
		Assign(rhs);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// CNameValueList

//-----------------------------------------------------------------------------

CNameValueList::CNameValueList()
{
	Init();
}

//-----------------------------------------------------------------------------

CNameValueList::CNameValueList(const CNameValueList& src)
{
	Init();
	Assign(src);
}

//-----------------------------------------------------------------------------

CNameValueList::~CNameValueList()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CNameValueList::Init()
{
	m_bCaseSensitive = false;
}

//-----------------------------------------------------------------------------

void CNameValueList::Assign(const CNameValueList& src)
{
	Clear();
	m_bCaseSensitive = src.m_bCaseSensitive;

	CString strName, strValue;
	for (int i = 0; i < src.GetCount(); i++)
	{
		src.GetItems(i, strName, strValue);
		Add(strName, strValue);
	}
}

//-----------------------------------------------------------------------------
// Throws an exception if the name is duplicated.
//-----------------------------------------------------------------------------
void CNameValueList::CheckNameDuplicated()
{
	for (int i = 0; i < GetCount() - 1; i++)
		for (int j = i + 1; j < GetCount(); j++)
		{
			int nCompRes;
			if (GetCaseSensitive())
				nCompRes = ItemPtr(i)->strName.Compare(ItemPtr(j)->strName);
			else
				nCompRes = ItemPtr(i)->strName.CompareNoCase(ItemPtr(j)->strName);

			if (nCompRes == 0)
				IfcThrowException(FormatString(SEM_DUPLICATE_NAME, ItemPtr(i)->strName));
		}
}

//-----------------------------------------------------------------------------

bool CNameValueList::Add(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CString strName(lpszName);

	if (strName.Find(DEFAULT_NAME_VALUE_SEP) >= 0)
		IfcThrowException(SEM_STRINGS_NAME_ERROR);

	int i = IndexOf(strName);
	bool bResult = (i < 0);

	if (bResult)
	{
		CNameValueItem *pItem = new CNameValueItem(strName, lpszValue);
		i = m_Items.Add(pItem);
	}
	else
		ItemPtr(i)->strValue = lpszValue;

	return bResult;
}

//-----------------------------------------------------------------------------

bool CNameValueList::Insert(int nIndex, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (nIndex < 0 || nIndex > GetCount())
		IfcThrowException(FormatString(SEM_LIST_INDEX_ERROR, nIndex));

	bool bResult = Add(lpszName, lpszValue);
	if (bResult)
		m_Items.Move(m_Items.GetCount() - 1, nIndex);

	return bResult;
}

//-----------------------------------------------------------------------------

void CNameValueList::Delete(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetCount())
		IfcThrowException(FormatString(SEM_LIST_INDEX_ERROR, nIndex));

	delete ItemPtr(nIndex);
	m_Items.Delete(nIndex);
}

//-----------------------------------------------------------------------------

bool CNameValueList::Remove(LPCTSTR lpszName)
{
	int i = IndexOf(lpszName);
	bool bResult = (i >= 0);

	if (bResult) Delete(i);

	return bResult;
}

//-----------------------------------------------------------------------------

void CNameValueList::Clear()
{
	for (int i = 0; i < m_Items.GetCount(); i++)
		delete ItemPtr(i);
	m_Items.Clear();
}

//-----------------------------------------------------------------------------

int CNameValueList::IndexOf(LPCTSTR lpszName) const
{
	int nResult = -1;
	CString strName(lpszName);

	for (int i = 0; i < m_Items.GetCount(); i++)
	{
		int nCompRes;
		if (GetCaseSensitive())
			nCompRes = strName.Compare(ItemPtr(i)->strName);
		else
			nCompRes = strName.CompareNoCase(ItemPtr(i)->strName);

		if (nCompRes == 0)
		{
			nResult = i;
			break;
		}
	}

	return nResult;
}

//-----------------------------------------------------------------------------

bool CNameValueList::NameExists(LPCTSTR lpszName) const
{
	return (IndexOf(lpszName) >= 0);
}

//-----------------------------------------------------------------------------

bool CNameValueList::GetValue(LPCTSTR lpszName, CString& strValue) const
{
	int i = IndexOf(lpszName);
	bool bResult = (i >= 0);

	if (bResult)
		strValue = ItemPtr(i)->strValue;

	return bResult;
}

//-----------------------------------------------------------------------------

bool CNameValueList::SetValue(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	int i = IndexOf(lpszName);
	bool bResult = (i >= 0);

	if (bResult)
		ItemPtr(i)->strValue = lpszValue;

	return bResult;
}

//-----------------------------------------------------------------------------

void CNameValueList::GetItems(int nIndex, CString& strName, CString& strValue) const
{
	if (nIndex < 0 || nIndex >= GetCount())
		IfcThrowException(FormatString(SEM_LIST_INDEX_ERROR, nIndex));

	strName = ItemPtr(nIndex)->strName;
	strValue = ItemPtr(nIndex)->strValue;
}

//-----------------------------------------------------------------------------

CString CNameValueList::GetText() const
{
	CStrList StrList;

	for (int i = 0; i < GetCount(); i++)
		StrList.Add(StrList.CombineNameValue(ItemPtr(i)->strName, ItemPtr(i)->strValue));

	return StrList.GetText();
}

//-----------------------------------------------------------------------------

CString CNameValueList::GetCommaText() const
{
	CStrList StrList;

	for (int i = 0; i < GetCount(); i++)
		StrList.Add(StrList.CombineNameValue(ItemPtr(i)->strName, ItemPtr(i)->strValue));

	return StrList.GetCommaText();
}

//-----------------------------------------------------------------------------

void CNameValueList::SetCommaText(LPCTSTR lpszText)
{
	CStrList StrList;

	Clear();
	StrList.SetCommaText(lpszText);
	for (int i = 0; i < StrList.GetCount(); i++)
		Add(StrList.GetName(i), StrList.GetValue(i));
}

//-----------------------------------------------------------------------------

void CNameValueList::SetCaseSensitive(bool bValue)
{
	if (bValue != m_bCaseSensitive)
	{
		m_bCaseSensitive = bValue;
		if (!bValue)
		{
			try
			{
				CheckNameDuplicated();
			}
			catch (...)
			{
				m_bCaseSensitive = !m_bCaseSensitive;
				throw;
			}
		}
	}
}

//-----------------------------------------------------------------------------

CNameValueList& CNameValueList::operator = (const CNameValueList& rhs)
{
	if (this != &rhs)
		Assign(rhs);
	return *this;
}

//-----------------------------------------------------------------------------

CString& CNameValueList::operator[] (LPCTSTR lpszName)
{
	int i = IndexOf(lpszName);
	if (i < 0)
	{
		Add(lpszName, TEXT(""));
		i = GetCount() - 1;
	}

	return ItemPtr(i)->strValue;
}

///////////////////////////////////////////////////////////////////////////////
// CRegistry

//-----------------------------------------------------------------------------

CRegistry::CRegistry() :
	m_hRootKey(HKEY_CURRENT_USER),
	m_hCurrentKey(0),
	m_bLazyWrite(true),
	m_nAccess(KEY_ALL_ACCESS)
{
	// nothing
}

//-----------------------------------------------------------------------------

CRegistry::~CRegistry()
{
	CloseKey();
}

//-----------------------------------------------------------------------------

void CRegistry::ChangeKey(HKEY hValue, LPCTSTR lpszPath)
{
	CloseKey();
	m_hCurrentKey = hValue;
	m_strCurrentPath = lpszPath;
}

//-----------------------------------------------------------------------------

HKEY CRegistry::GetBaseKey(bool bRelative)
{
	if (m_hCurrentKey == 0 || !bRelative)
		return m_hRootKey;
	else
		return m_hCurrentKey;
}

//-----------------------------------------------------------------------------

HKEY CRegistry::GetKey(LPCTSTR lpszKey)
{
	HKEY hResult = 0;
	CString strKey(lpszKey);

	bool bRelative = IsRelative(strKey);
	if (!bRelative)
		strKey.Delete(0, 1);
	::RegOpenKeyEx(GetBaseKey(bRelative), strKey, 0, m_nAccess, &hResult);

	return hResult;
}

//-----------------------------------------------------------------------------

void CRegistry::SetCurrentKey(HKEY hKey)
{
	m_hCurrentKey = hKey;
}

//-----------------------------------------------------------------------------

int CRegistry::GetData(LPCTSTR lpszName, LPBYTE pBuffer, DWORD nBufSize, REG_DATA_TYPE& nDataType)
{
	DWORD nWinDataType = REG_NONE;

	if (::RegQueryValueEx(m_hCurrentKey, lpszName, NULL, &nWinDataType, (LPBYTE)pBuffer, &nBufSize) != ERROR_SUCCESS)
		Error(SEM_REG_GET_DATA_FAILED, lpszName);
	nDataType = WinTypeToDataType(nWinDataType);

	return nBufSize;
}

//-----------------------------------------------------------------------------

void CRegistry::PutData(LPCTSTR lpszName, LPBYTE pBuffer, DWORD nBufSize, REG_DATA_TYPE nDataType)
{
	DWORD nWinDataType = DataTypeToWinType(nDataType);
	if (::RegSetValueEx(m_hCurrentKey, lpszName, 0, nWinDataType, pBuffer, nBufSize) != ERROR_SUCCESS)
		Error(SEM_REG_SET_DATA_FAILED, lpszName);
}

//-----------------------------------------------------------------------------

bool CRegistry::IsRelative(LPCTSTR lpszValue)
{
	return *lpszValue != '\\';
}

//-----------------------------------------------------------------------------

int CRegistry::DataTypeToWinType(REG_DATA_TYPE nType)
{
	switch (nType)
	{
	case RDT_STRING:         return REG_SZ;
	case RDT_EXPAND_STRING:  return REG_EXPAND_SZ;
	case RDT_INTEGER:        return REG_DWORD;
	case RDT_BINARY:         return REG_BINARY;
	default:                 return REG_NONE;
	}
}

//-----------------------------------------------------------------------------

CRegistry::REG_DATA_TYPE CRegistry::WinTypeToDataType(int nType)
{
	switch (nType)
	{
	case REG_SZ:             return RDT_STRING;
	case REG_EXPAND_SZ:      return RDT_EXPAND_STRING;
	case REG_DWORD:          return RDT_INTEGER;
	case REG_BINARY:         return RDT_BINARY;
	default:                 return RDT_UNKNOWN;
	}
}

//-----------------------------------------------------------------------------

void CRegistry::Error(LPCTSTR lpszMsg, LPCTSTR lpszData)
{
	CString strMsg;
	if (lpszData != NULL)
		strMsg.Format(lpszMsg, lpszData);
	else
		strMsg = lpszMsg;

	IfcThrowRegistryException(strMsg);
}

//-----------------------------------------------------------------------------

void CRegistry::CreateKey(LPCTSTR lpszKey)
{
	bool bResult;
	CString strKey(lpszKey);
	HKEY hTempKey = 0;
	DWORD nDisposition;
	bool bRelative = IsRelative(lpszKey);

	if (!bRelative)
		strKey.Delete(0, 1);

	bResult = ::RegCreateKeyEx(GetBaseKey(bRelative), strKey, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hTempKey, &nDisposition) == ERROR_SUCCESS;
	if (bResult)
		::RegCloseKey(hTempKey);
	else
		Error(SEM_REG_CREATE_FAILED, lpszKey);
}

//-----------------------------------------------------------------------------

bool CRegistry::DeleteKey(LPCTSTR lpszKey)
{
	CString strKey(lpszKey);
	bool bRelative = IsRelative(lpszKey);
	HKEY hOldKey, hDelKey;

	if (!bRelative)
		strKey.Delete(0, 1);

	hOldKey = m_hCurrentKey;
	hDelKey = GetKey(lpszKey);
	if (hDelKey != 0)
	{
		CRegKeyInfo KeyInfo;

		struct CAutoFinalizer {
			CRegistry *m_pRegistry;
			HKEY *m_pOldKey, *m_pDelKey;
			CAutoFinalizer(CRegistry *pRegistry, HKEY *pOldKey, HKEY *pDelKey) :
				m_pRegistry(pRegistry), m_pOldKey(pOldKey), m_pDelKey(pDelKey)
			{}
			~CAutoFinalizer()
			{
				m_pRegistry->SetCurrentKey(*m_pOldKey);
				::RegCloseKey(*m_pDelKey);
			}
		} AutoFinalizer(this, &hOldKey, &hDelKey);

		SetCurrentKey(hDelKey);
		if (GetKeyInfo(KeyInfo))
		{
			CBuffer KeyNameBuf((KeyInfo.nMaxSubKeyLen + 1) * sizeof(TCHAR));
			for (int i = KeyInfo.nNumSubKeys - 1; i >= 0; i--)
			{
				DWORD nSize = KeyInfo.nMaxSubKeyLen + 1;
				if (::RegEnumKeyEx(hDelKey, (DWORD)i, (LPTSTR)(PVOID)KeyNameBuf.Data(), &nSize,
					NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
					this->DeleteKey((LPTSTR)(PVOID)KeyNameBuf.Data());
			}
		}
	}

	return ::RegDeleteKey(GetBaseKey(bRelative), strKey) == ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

bool CRegistry::OpenKey(LPCTSTR lpszKey, bool bCanCreate)
{
	bool bResult;
	CString strKey(lpszKey);
	DWORD nDisposition;
	bool bRelative = IsRelative(lpszKey);
	HKEY hTempKey = 0;

	if (!bRelative)
		strKey.Delete(0, 1);

	if (!bCanCreate || strKey.IsEmpty())
		bResult = ::RegOpenKeyEx(GetBaseKey(bRelative), strKey, 0, m_nAccess, &hTempKey) == ERROR_SUCCESS;
	else
		bResult = ::RegCreateKeyEx(GetBaseKey(bRelative), strKey, 0, NULL, REG_OPTION_NON_VOLATILE,
			m_nAccess, NULL, &hTempKey, &nDisposition) == ERROR_SUCCESS;

	if (bResult)
	{
		if (m_hCurrentKey != 0 && bRelative)
			strKey = m_strCurrentPath + TEXT("\\") + strKey;
		ChangeKey(hTempKey, strKey);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CRegistry::OpenKeyReadOnly(LPCTSTR lpszKey)
{
	bool bResult;
	CString strKey(lpszKey);
	bool bRelative = IsRelative(lpszKey);
	HKEY hTempKey = 0;
	DWORD nWOWFlags = 0;
	DWORD nAccess = m_nAccess;

	if (!bRelative)
		strKey.Delete(0, 1);

	// Preserve KEY_WOW64_XXX flags for later use
	nWOWFlags = m_nAccess & KEY_WOW64_RES;

	nAccess = KEY_READ | nWOWFlags;
	bResult = ::RegOpenKeyEx(GetBaseKey(bRelative), strKey, 0, nAccess, &hTempKey) == ERROR_SUCCESS;
	if (!bResult)
	{
		DWORD nAccess = STANDARD_RIGHTS_READ | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | nWOWFlags;
		bResult = ::RegOpenKeyEx(GetBaseKey(bRelative), strKey, 0, nAccess, &hTempKey) == ERROR_SUCCESS;
	}
	if (!bResult)
	{
		nAccess = KEY_QUERY_VALUE | nWOWFlags;
		bResult = ::RegOpenKeyEx(GetBaseKey(bRelative), strKey, 0, nAccess, &hTempKey) == ERROR_SUCCESS;
	}

	if (bResult)
	{
		m_nAccess = nAccess;
		if (m_hCurrentKey != 0 && bRelative)
			strKey = m_strCurrentPath + TEXT("\\") + strKey;
		ChangeKey(hTempKey, strKey);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

void CRegistry::CloseKey()
{
	if (m_hCurrentKey != 0)
	{
		if (!m_bLazyWrite)
			::RegFlushKey(m_hCurrentKey);
		::RegCloseKey(m_hCurrentKey);
		m_hCurrentKey = 0;
		m_strCurrentPath.Empty();
	}
}

//-----------------------------------------------------------------------------

bool CRegistry::DeleteValue(LPCTSTR lpszName)
{
	return ::RegDeleteValue(m_hCurrentKey, lpszName) == ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------

bool CRegistry::HasSubKeys()
{
	CRegKeyInfo KeyInfo;
	return GetKeyInfo(KeyInfo) && (KeyInfo.nNumSubKeys > 0);
}

//-----------------------------------------------------------------------------

bool CRegistry::KeyExists(LPCTSTR lpszKey)
{
	HKEY hTempKey;
	DWORD nOldAccess = m_nAccess;

	struct CAutoFinalizer {
		CRegistry *m_pRegistry;
		const DWORD& m_nOldAccess;
		CAutoFinalizer(CRegistry *pRegistry, const DWORD& nOldAccess) :
			m_pRegistry(pRegistry), m_nOldAccess(nOldAccess) {}
		~CAutoFinalizer()
		{ m_pRegistry->m_nAccess = m_nOldAccess; }
	} AutoFinalizer(this, nOldAccess);

	m_nAccess = STANDARD_RIGHTS_READ | KEY_QUERY_VALUE |
		KEY_ENUMERATE_SUB_KEYS | (nOldAccess & KEY_WOW64_RES);
	hTempKey = GetKey(lpszKey);
	if (hTempKey != 0)
		::RegCloseKey(hTempKey);

	return (hTempKey != 0);
}

//-----------------------------------------------------------------------------

bool CRegistry::ValueExists(LPCTSTR lpszName)
{
	CRegDataInfo DataInfo;
	return GetDataInfo(lpszName, DataInfo);
}

//-----------------------------------------------------------------------------

void CRegistry::GetKeyNames(CStrList& List)
{
	CRegKeyInfo KeyInfo;

	List.Clear();
	if (GetKeyInfo(KeyInfo))
	{
		CBuffer NameBuf((KeyInfo.nMaxSubKeyLen + 1) * sizeof(TCHAR));
		for (DWORD i = 0; i < KeyInfo.nNumSubKeys; i++)
		{
			DWORD nSize = KeyInfo.nMaxSubKeyLen + 1;
			if (::RegEnumKeyEx(m_hCurrentKey, i, (LPTSTR)(PVOID)NameBuf.Data(), &nSize,
				NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
				List.Add((LPTSTR)(PVOID)NameBuf.Data());
		}
	}
}

//-----------------------------------------------------------------------------

void CRegistry::GetValueNames(CStrList& List)
{
	CRegKeyInfo KeyInfo;

	List.Clear();
	if (GetKeyInfo(KeyInfo))
	{
		CBuffer NameBuf((KeyInfo.nMaxValueLen + 1) * sizeof(TCHAR));
		for (DWORD i = 0; i < KeyInfo.nNumValues; i++)
		{
			DWORD nSize = KeyInfo.nMaxValueLen + 1;
			if (::RegEnumValue(m_hCurrentKey, i, (LPTSTR)(PVOID)NameBuf.Data(), &nSize,
				NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
				List.Add((LPTSTR)(PVOID)NameBuf.Data());
		}
	}
}

//-----------------------------------------------------------------------------

bool CRegistry::GetDataInfo(LPCTSTR lpszValueName, CRegDataInfo& DataInfo)
{
	bool bResult;
	DWORD nWinDataType;

	DataInfo.Clear();
	bResult = ::RegQueryValueEx(m_hCurrentKey, lpszValueName, NULL, &nWinDataType,
		NULL, &DataInfo.nDataSize) == ERROR_SUCCESS;
	DataInfo.nDataType = WinTypeToDataType(nWinDataType);

	return bResult;
}

//-----------------------------------------------------------------------------

int CRegistry::GetDataSize(LPCTSTR lpszValueName)
{
	CRegDataInfo DataInfo;

	if (GetDataInfo(lpszValueName, DataInfo))
		return DataInfo.nDataSize;
	else
		return -1;
}

//-----------------------------------------------------------------------------

CRegistry::REG_DATA_TYPE CRegistry::GetDataType(LPCTSTR lpszValueName)
{
	CRegDataInfo DataInfo;

	if (GetDataInfo(lpszValueName, DataInfo))
		return DataInfo.nDataType;
	else
		return RDT_UNKNOWN;
}

//-----------------------------------------------------------------------------

bool CRegistry::GetKeyInfo(CRegKeyInfo& KeyInfo)
{
	bool bResult;

	KeyInfo.Clear();
	bResult = ::RegQueryInfoKey(m_hCurrentKey, NULL, NULL, NULL, &KeyInfo.nNumSubKeys,
		&KeyInfo.nMaxSubKeyLen, NULL, &KeyInfo.nNumValues, &KeyInfo.nMaxValueLen,
		&KeyInfo.nMaxDataLen, NULL, &KeyInfo.tFileTime) == ERROR_SUCCESS;

	return bResult;
}

//-----------------------------------------------------------------------------

int CRegistry::ReadInteger(LPCTSTR lpszName)
{
	INT32 nResult;
	REG_DATA_TYPE nDataType;

	GetData(lpszName, (LPBYTE)&nResult, sizeof(INT32), nDataType);
	if (nDataType != RDT_INTEGER)
		Error(SEM_INVALID_REG_TYPE, lpszName);

	return nResult;
}

//-----------------------------------------------------------------------------

bool CRegistry::ReadBool(LPCTSTR lpszName)
{
	return ReadInteger(lpszName) != 0;
}

//-----------------------------------------------------------------------------

double CRegistry::ReadFloat(LPCTSTR lpszName)
{
	double fResult;
	REG_DATA_TYPE nDataType;

	int nLen = GetData(lpszName, (LPBYTE)&fResult, sizeof(double), nDataType);
	if (nDataType != RDT_BINARY || nLen != sizeof(double))
		Error(SEM_INVALID_REG_TYPE, lpszName);

	return fResult;
}

//-----------------------------------------------------------------------------

CString CRegistry::ReadString(LPCTSTR lpszName)
{
	CString strResult;
	int nLen = GetDataSize(lpszName);
	REG_DATA_TYPE nDataType;

	if (nLen > 0)
	{
		LPTSTR p = strResult.GetBuffer(nLen);
		GetData(lpszName, (LPBYTE)p, nLen, nDataType);
		strResult.ReleaseBuffer();
		if (nDataType != RDT_STRING && nDataType != RDT_EXPAND_STRING)
			Error(SEM_INVALID_REG_TYPE, lpszName);
	}

	return strResult;
}

//-----------------------------------------------------------------------------

int CRegistry::ReadBinaryData(LPCTSTR lpszName, PVOID pBuffer, int nBufSize)
{
	int nResult = 0;
	REG_DATA_TYPE nDataType;
	CRegDataInfo DataInfo;

	if (GetDataInfo(lpszName, DataInfo))
	{
		nResult = DataInfo.nDataSize;
		nDataType = DataInfo.nDataType;
		if ((nDataType == RDT_BINARY || nDataType == RDT_UNKNOWN) && (nResult <= nBufSize))
			GetData(lpszName, (LPBYTE)pBuffer, nResult, nDataType);
		else
			Error(SEM_INVALID_REG_TYPE, lpszName);
	}

	return nResult;
}

//-----------------------------------------------------------------------------

void CRegistry::WriteInteger(LPCTSTR lpszName, int nValue)
{
	INT32 nTempValue = nValue;
	PutData(lpszName, (LPBYTE)&nTempValue, sizeof(INT32), RDT_INTEGER);
}

//-----------------------------------------------------------------------------

void CRegistry::WriteBool(LPCTSTR lpszName, bool bValue)
{
	WriteInteger(lpszName, bValue? 1 : 0);
}

//-----------------------------------------------------------------------------

void CRegistry::WriteFloat(LPCTSTR lpszName, double fValue)
{
	PutData(lpszName, (LPBYTE)&fValue, sizeof(double), RDT_BINARY);
}

//-----------------------------------------------------------------------------

void CRegistry::WriteString(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CString strValue(lpszValue);
	PutData(lpszName, (LPBYTE)(LPCTSTR)strValue,
		(strValue.GetLength() + 1) * sizeof(TCHAR), RDT_STRING);
}

//-----------------------------------------------------------------------------

void CRegistry::WriteExpandString(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CString strValue(lpszValue);
	PutData(lpszName, (LPBYTE)(LPCTSTR)strValue,
		(strValue.GetLength() + 1) * sizeof(TCHAR), RDT_EXPAND_STRING);
}

//-----------------------------------------------------------------------------

void CRegistry::WriteBinaryData(LPCTSTR lpszName, PVOID pBuffer, int nBufSize)
{
	PutData(lpszName, (LPBYTE)pBuffer, nBufSize, RDT_BINARY);
}

//-----------------------------------------------------------------------------

void CRegistry::SetRootKey(HKEY hValue)
{
	if (m_hRootKey != hValue)
	{
		m_hRootKey = hValue;
		CloseKey();
	}
}

///////////////////////////////////////////////////////////////////////////////
// CCustomIniFile

//-----------------------------------------------------------------------------

CCustomIniFile::CCustomIniFile(LPCTSTR lpszFileName) :
	m_strFileName(lpszFileName)
{
	// nothing
}

//-----------------------------------------------------------------------------

int CCustomIniFile::ReadInteger(LPCTSTR lpszSection, LPCTSTR lpszName, int nDefault)
{
	CString str = ReadString(lpszSection, lpszName, TEXT(""));
	return StrToInt(str, nDefault);
}

//-----------------------------------------------------------------------------

bool CCustomIniFile::ReadBool(LPCTSTR lpszSection, LPCTSTR lpszName, bool bDefault)
{
	return ReadInteger(lpszSection, lpszName, bDefault? 1:0) != 0;
}

//-----------------------------------------------------------------------------

double CCustomIniFile::ReadFloat(LPCTSTR lpszSection, LPCTSTR lpszName, double fDefault)
{
	CString str = ReadString(lpszSection, lpszName, TEXT(""));
	return StrToFloat(str, fDefault);
}

//-----------------------------------------------------------------------------

int CCustomIniFile::ReadBinaryData(LPCTSTR lpszSection, LPCTSTR lpszName, CBuffer& Value)
{
	int nResult = 0;

	Value.Clear();
	CString strText = ReadString(lpszSection, lpszName, TEXT(""));
	if (!strText.IsEmpty())
	{
		DecodeBase16(strText, Value);
		nResult = Value.GetSize();
	}

	return nResult;
}

//-----------------------------------------------------------------------------

int CCustomIniFile::ReadBinaryStream(LPCTSTR lpszSection, LPCTSTR lpszName, CStream& Value)
{
	CBuffer Buffer;
	int nResult = ReadBinaryData(lpszSection, lpszName, Buffer);
	if (nResult > 0)
		Value.Write(Buffer.Data(), Buffer.GetSize());
	return nResult;
}

//-----------------------------------------------------------------------------

void CCustomIniFile::WriteInteger(LPCTSTR lpszSection, LPCTSTR lpszName, int nValue)
{
	WriteString(lpszSection, lpszName, IntToStr(nValue));
}

//-----------------------------------------------------------------------------

void CCustomIniFile::WriteBool(LPCTSTR lpszSection, LPCTSTR lpszName, bool bValue)
{
	WriteInteger(lpszSection, lpszName, (bValue? 1 : 0));
}

//-----------------------------------------------------------------------------

void CCustomIniFile::WriteFloat(LPCTSTR lpszSection, LPCTSTR lpszName, double fValue)
{
	WriteString(lpszSection, lpszName, FloatToStr(fValue));
}

//-----------------------------------------------------------------------------

void CCustomIniFile::WriteBinaryData(LPCTSTR lpszSection, LPCTSTR lpszName, PVOID pDataBuf, int nDataSize)
{
	CString strText;
	if (nDataSize > 0 && pDataBuf != NULL)
		strText = EncodeBase16((char*)pDataBuf, nDataSize);
	WriteString(lpszSection, lpszName, strText);
}

//-----------------------------------------------------------------------------

void CCustomIniFile::WriteBinaryStream(LPCTSTR lpszSection, LPCTSTR lpszName, CStream& Value)
{
	INT64 nSize64 = Value.GetSize() - Value.GetPosition();
	IFC_ASSERT(nSize64 <= MAXLONG);
	int nSize = (int)nSize64;

	CBuffer Buffer(nSize);
	if (Buffer.GetSize() > 0)
		Value.Read(Buffer.Data(), Buffer.GetSize());
	WriteBinaryData(lpszSection, lpszName, Buffer.Data(), Buffer.GetSize());
}

//-----------------------------------------------------------------------------

bool CCustomIniFile::SectionExists(LPCTSTR lpszSection)
{
	CStrList List;
	ReadKeyNames(lpszSection, List);
	return (List.GetCount() > 0);
}

//-----------------------------------------------------------------------------

bool CCustomIniFile::KeyExists(LPCTSTR lpszSection, LPCTSTR lpszName)
{
	CStrList List;
	ReadKeyNames(lpszSection, List);
	return (List.IndexOf(lpszName) >= 0);
}

///////////////////////////////////////////////////////////////////////////////
// CMemIniFile

CMemIniFile::CMemIniFile(LPCTSTR lpszFileName) : CCustomIniFile(lpszFileName)
{
	m_Sections.SetCaseSensitive(false);
	LoadData();
	m_bModified = false;
}

//-----------------------------------------------------------------------------

CMemIniFile::~CMemIniFile()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CMemIniFile::LoadData()
{
	Clear();
	if (!m_strFileName.IsEmpty() && FileExists(m_strFileName))
	{
		CStrList StrList;
		if (StrList.LoadFromFile(m_strFileName, false))
			SetStrings(StrList);
	}
}

//-----------------------------------------------------------------------------

CStrings* CMemIniFile::AddSection(LPCTSTR lpszSection)
{
	CHashedStrList *pResult = new CHashedStrList();
	pResult->SetCaseSensitive(false);
	m_Sections.Add(lpszSection, pResult);
	return pResult;
}

//-----------------------------------------------------------------------------

CStrings* CMemIniFile::GetSectionData(LPCTSTR lpszSection)
{
	int i = m_Sections.IndexOf(lpszSection);
	if (i >= 0)
		return (CStrings*)m_Sections.GetData(i);
	else
		return NULL;
}

//-----------------------------------------------------------------------------

CString CMemIniFile::ReadString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault)
{
	CStrings *pStrings = GetSectionData(lpszSection);
	if (pStrings != NULL)
	{
		CString strName(lpszName);
		int i = pStrings->IndexOfName(strName);
		if (i >= 0)
			return pStrings->GetString(i).Mid(strName.GetLength() + 1);
	}

	return lpszDefault;
}

//-----------------------------------------------------------------------------

void CMemIniFile::WriteString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CStrings *pStrings = GetSectionData(lpszSection);
	if (pStrings == NULL)
		pStrings = AddSection(lpszSection);

	CString s = CString(lpszName) + '=' + lpszValue;
	int i = pStrings->IndexOfName(lpszName);
	if (i >= 0)
		pStrings->SetString(i, s);
	else
		pStrings->Add(s);

	m_bModified = true;
}

//-----------------------------------------------------------------------------

void CMemIniFile::ReadKeyNames(LPCTSTR lpszSection, CStrings& KeyNames)
{
	CStrings::CAutoUpdater AutoUpdater(KeyNames);

	KeyNames.Clear();
	CStrings *pStrings = GetSectionData(lpszSection);
	if (pStrings != NULL)
	{
		for (int i = 0; i < pStrings->GetCount(); i++)
			KeyNames.Add(pStrings->GetName(i));
	}
}

//-----------------------------------------------------------------------------

void CMemIniFile::ReadSectionNames(CStrings& SectionNames)
{
	SectionNames = m_Sections;
}

//-----------------------------------------------------------------------------

void CMemIniFile::ReadStrings(LPCTSTR lpszSection, CStrings& Strings)
{
	CStrings::CAutoUpdater AutoUpdater(Strings);

	Strings.Clear();
	CStrings *pStrings = GetSectionData(lpszSection);
	if (pStrings)
		Strings = *pStrings;
}

//-----------------------------------------------------------------------------

void CMemIniFile::EraseSection(LPCTSTR lpszSection)
{
	int i = m_Sections.IndexOf(lpszSection);
	if (i >= 0)
	{
		delete (CStrings*)m_Sections.GetData(i);
		m_Sections.Delete(i);
		m_bModified = true;
	}
}

//-----------------------------------------------------------------------------

void CMemIniFile::DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszName)
{
	CStrings *pStrings = GetSectionData(lpszSection);
	if (pStrings != NULL)
	{
		int i = pStrings->IndexOfName(lpszName);
		if (i >= 0)
		{
			pStrings->Delete(i);
			m_bModified = true;
		}
	}
}

//-----------------------------------------------------------------------------

void CMemIniFile::UpdateFile()
{
	if (m_bModified)
	{
		CStrList StrList;
		GetStrings(StrList);
		StrList.SaveToFile(m_strFileName, false);
		m_bModified = false;
	}
}

//-----------------------------------------------------------------------------

void CMemIniFile::Clear()
{
	for (int i = 0; i < m_Sections.GetCount(); i++)
		delete (CStrings*)m_Sections.GetData(i);
	m_Sections.Clear();
	m_bModified = true;
}

//-----------------------------------------------------------------------------

void CMemIniFile::SetStrings(const CStrings& List)
{
	Clear();

	CStrings *pStrings = NULL;
	for (int i = 0; i < List.GetCount(); i++)
	{
		CString s = List[i];
		s.Trim();
		if (!s.IsEmpty() && s[0] != ';')
		{
			if (s[0] == '[' && s[s.GetLength()-1] == ']')
			{
				s = s.Mid(1, s.GetLength() - 2).Trim();
				pStrings = AddSection(s);
			}
			else
			{
				if (pStrings != NULL)
				{
					int j = s.Find('=');
					if (j > 0)
						pStrings->Add(s.Mid(0, j).Trim() + '=' + s.Mid(j+1).Trim());
				}
			}
		}
	}

	m_bModified = true;
}

//-----------------------------------------------------------------------------

void CMemIniFile::GetStrings(CStrings& List)
{
	CStrings::CAutoUpdater AutoUpdater(List);
	int nSectionCount = m_Sections.GetCount();

	for (int i = 0; i < nSectionCount; i++)
	{
		List.Add('[' + m_Sections[i] + ']');

		CStrings *pStrings = (CStrings*)m_Sections.GetData(i);
		for (int j = 0; j < pStrings->GetCount(); j++)
			List.Add(pStrings->GetString(j));

		if (i < nSectionCount - 1)
			List.Add(TEXT(""));
	}
}

///////////////////////////////////////////////////////////////////////////////
// CIniFile

//-----------------------------------------------------------------------------

CIniFile::~CIniFile()
{
	InternalUpdateFile();
}

//-----------------------------------------------------------------------------

void CIniFile::InternalUpdateFile()
{
	::WritePrivateProfileString(NULL, NULL, NULL, m_strFileName);
}

//-----------------------------------------------------------------------------

CString CIniFile::ReadString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault)
{
	const int MAX_SIZE = 2047;

	CString strResult;
	LPTSTR pBuffer = strResult.GetBuffer(MAX_SIZE);
	int nSize = ::GetPrivateProfileString(lpszSection, lpszName, lpszDefault, pBuffer, MAX_SIZE, m_strFileName);
	strResult.ReleaseBuffer();
	return strResult;
}

//-----------------------------------------------------------------------------

void CIniFile::WriteString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (!::WritePrivateProfileString(lpszSection, lpszName, lpszValue, m_strFileName))
		IfcThrowIniFileException(FormatString(SEM_INI_FILE_WRITE_ERROR, m_strFileName));
}

//-----------------------------------------------------------------------------

void CIniFile::ReadKeyNames(LPCTSTR lpszSection, CStrings& KeyNames)
{
	const int MAX_SIZE = 16384;

	CStrings::CAutoUpdater AutoUpdater(KeyNames);
	CString strBuffer;

	KeyNames.Clear();
	LPTSTR pBuffer = strBuffer.GetBuffer(MAX_SIZE);
	bool bSuccess = ::GetPrivateProfileString(lpszSection, NULL, NULL, pBuffer,
		MAX_SIZE, m_strFileName) != 0;
	strBuffer.ReleaseBuffer(MAX_SIZE);

	if (bSuccess)
	{
		while (*pBuffer != '\0')
		{
			KeyNames.Add(pBuffer);
			pBuffer += (_tcsclen(pBuffer) + 1);
		}
	}
}

//-----------------------------------------------------------------------------

void CIniFile::ReadSectionNames(CStrings& SectionNames)
{
	const int MAX_SIZE = 16384;

	CStrings::CAutoUpdater AutoUpdater(SectionNames);
	CString strBuffer;

	SectionNames.Clear();
	LPTSTR pBuffer = strBuffer.GetBuffer(MAX_SIZE);
	bool bSuccess = ::GetPrivateProfileString(NULL, NULL, NULL, pBuffer,
		MAX_SIZE, m_strFileName) != 0;
	strBuffer.ReleaseBuffer(MAX_SIZE);

	if (bSuccess)
	{
		while (*pBuffer != '\0')
		{
			SectionNames.Add(pBuffer);
			pBuffer += (_tcsclen(pBuffer) + 1);
		}
	}
}

//-----------------------------------------------------------------------------

void CIniFile::ReadStrings(LPCTSTR lpszSection, CStrings& Strings)
{
	CStrings::CAutoUpdater AutoUpdater(Strings);
	CStrList KeyNames;

	ReadKeyNames(lpszSection, KeyNames);
	Strings.Clear();
	for (int i = 0; i < KeyNames.GetCount(); i++)
		Strings.Add(KeyNames[i] + TEXT("=") + ReadString(lpszSection, KeyNames[i], TEXT("")));
}

//-----------------------------------------------------------------------------

void CIniFile::EraseSection(LPCTSTR lpszSection)
{
	if (!::WritePrivateProfileString(lpszSection, NULL, NULL, m_strFileName))
		IfcThrowIniFileException(FormatString(SEM_INI_FILE_WRITE_ERROR, m_strFileName));
}

//-----------------------------------------------------------------------------

void CIniFile::DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszName)
{
	::WritePrivateProfileString(lpszSection, lpszName, NULL, m_strFileName);
}

//-----------------------------------------------------------------------------

void CIniFile::UpdateFile()
{
	InternalUpdateFile();
}

///////////////////////////////////////////////////////////////////////////////
// CMonitor

//-----------------------------------------------------------------------------

CMonitor::CMonitor(HMONITOR hHandle, int nMonitorIndex) :
	m_hHandle(hHandle),
	m_nMonitorIndex(nMonitorIndex)
{
	// nothing
}

//-----------------------------------------------------------------------------

int CMonitor::GetLeft() const
{
	return GetBoundsRect().left;
}

//-----------------------------------------------------------------------------

int CMonitor::GetTop() const
{
	return GetBoundsRect().top;
}

//-----------------------------------------------------------------------------

int CMonitor::GetWidth() const
{
	return GetBoundsRect().Width();
}

//-----------------------------------------------------------------------------

int CMonitor::GetHeight() const
{
	return GetBoundsRect().Height();
}

//-----------------------------------------------------------------------------

CRect CMonitor::GetBoundsRect() const
{
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	::GetMonitorInfo(m_hHandle, &mi);
	return mi.rcMonitor;
}

//-----------------------------------------------------------------------------

CRect CMonitor::GetWorkAreaRect() const
{
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	::GetMonitorInfo(m_hHandle, &mi);
	return mi.rcWork;
}

//-----------------------------------------------------------------------------

bool CMonitor::IsPrimary() const
{
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	::GetMonitorInfo(m_hHandle, &mi);
	return (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
}

///////////////////////////////////////////////////////////////////////////////
// CScreen

std::auto_ptr<CScreen> CScreen::s_pSingleton(NULL);

//-----------------------------------------------------------------------------

CScreen::CScreen()
{
	InitMonitorList();

	HDC dc = GetDC(0);
	m_nPixelsPerInch = ::GetDeviceCaps(dc, LOGPIXELSY);
	ReleaseDC(0, dc);

	m_nDefaultKbLayout = ::GetKeyboardLayout(0);
}

//-----------------------------------------------------------------------------

CScreen::~CScreen()
{
	ClearMonitorList();
}

//-----------------------------------------------------------------------------

CScreen& CScreen::Instance()
{
	if (s_pSingleton.get() == NULL)
		s_pSingleton.reset(new CScreen());
	return *s_pSingleton;
}

//-----------------------------------------------------------------------------
// EnumDisplayMonitors callback
//-----------------------------------------------------------------------------
BOOL __stdcall EnumMonitorsProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	CPointerList *pList = (CPointerList*)dwData;
	CMonitor *pMonitor = new CMonitor(hMonitor, pList->GetCount());
	pList->Add(pMonitor);

	return TRUE;
}

//-----------------------------------------------------------------------------

void CScreen::InitMonitorList()
{
	ClearMonitorList();
	::EnumDisplayMonitors(0, NULL, EnumMonitorsProc, (LPARAM)(&m_MonitorList));
}

//-----------------------------------------------------------------------------

void CScreen::ClearMonitorList()
{
	for (int i = 0; i < m_MonitorList.GetCount(); i++)
		delete (CMonitor*)m_MonitorList[i];
	m_MonitorList.Clear();
}

//-----------------------------------------------------------------------------

int CScreen::GetWidth()
{
	return ::GetSystemMetrics(SM_CXSCREEN);
}

//-----------------------------------------------------------------------------

int CScreen::GetHeight()
{
	return ::GetSystemMetrics(SM_CYSCREEN);
}

//-----------------------------------------------------------------------------

CRect CScreen::GetDesktopRect()
{
	CRect r;
	r.left = GetDesktopLeft();
	r.top = GetDesktopTop();
	r.right = r.left + GetDesktopWidth();
	r.bottom = r.top + GetDesktopHeight();

	return r;
}

//-----------------------------------------------------------------------------

int CScreen::GetDesktopLeft()
{
	return ::GetSystemMetrics(SM_XVIRTUALSCREEN);
}

//-----------------------------------------------------------------------------

int CScreen::GetDesktopTop()
{
	return ::GetSystemMetrics(SM_YVIRTUALSCREEN);
}

//-----------------------------------------------------------------------------

int CScreen::GetDesktopWidth()
{
	return ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
}

//-----------------------------------------------------------------------------

int CScreen::GetDesktopHeight()
{
	return ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

//-----------------------------------------------------------------------------

CRect CScreen::GetWorkAreaRect()
{
	CRect r;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
	return r;
}

//-----------------------------------------------------------------------------

int CScreen::GetWorkAreaLeft()
{
	return GetWorkAreaRect().left;
}

//-----------------------------------------------------------------------------

int CScreen::GetWorkAreaTop()
{
	return GetWorkAreaRect().top;
}

//-----------------------------------------------------------------------------

int CScreen::GetWorkAreaWidth()
{
	return GetWorkAreaRect().Width();
}

//-----------------------------------------------------------------------------

int CScreen::GetWorkAreaHeight()
{
	return GetWorkAreaRect().Height();
}

//-----------------------------------------------------------------------------

int CScreen::GetMonitorCount()
{
	if (m_MonitorList.GetCount() == 0)
		return ::GetSystemMetrics(SM_CMONITORS);
	else
		return m_MonitorList.GetCount();
}

//-----------------------------------------------------------------------------

CMonitor* CScreen::GetMonitor(int nIndex)
{
	return (CMonitor*)m_MonitorList[nIndex];
}

//-----------------------------------------------------------------------------

CMonitor* CScreen::GetPrimaryMonitor()
{
	for (int i = 0; i < m_MonitorList.GetCount(); i++)
		if (GetMonitor(i)->IsPrimary())
			return GetMonitor(i);

	/* If we didn't find the primary monitor, reset the display and try
	   again (it may have changed) */
	InitMonitorList();

	for (int i = 0; i < m_MonitorList.GetCount(); i++)
		if (GetMonitor(i)->IsPrimary())
			return GetMonitor(i);

	return NULL;
}

//-----------------------------------------------------------------------------
// EnumFontFamiliesEx callback
//-----------------------------------------------------------------------------
int __stdcall EnumFontsProc(const LOGFONT *lpLogFont, const TEXTMETRIC *lpTextMetric,
	DWORD nFontType, LPARAM lParam)
{
	CStrList *pStrList = (CStrList*)lParam;
	CString str = lpLogFont->lfFaceName;
	if (pStrList->GetCount() == 0 || str.CompareNoCase((*pStrList)[pStrList->GetCount()-1]) != 0)
		pStrList->Add(str);
	return 1;
}

//-----------------------------------------------------------------------------

const CStrList& CScreen::GetFontNames()
{
	if (m_FontList.GetCount() == 0)
	{
		CClientDC dc(0);
		LOGFONT lf;

		m_FontList.Add(TEXT("Default"));
		memset(&lf, 0, sizeof(lf));
		lf.lfCharSet = DEFAULT_CHARSET;
		::EnumFontFamiliesEx(dc, &lf, EnumFontsProc, (LPARAM)&m_FontList, 0);
		m_FontList.SetSorted(true);
	}

	return m_FontList;
}

//-----------------------------------------------------------------------------

const CStrList& CScreen::GetImeNames()
{
	const TCHAR* const KBLAYOUT_REG_KEY_FMT = TEXT("\\System\\CurrentControlSet\\Control\\Keyboard Layouts\\%.8X");
	const TCHAR* const KBLAYOUT_REG_SUB_KEY = TEXT("Layout Text");
	const int KBLIST_SIZE = 64;

	if (m_ImeList.GetCount() == 0)
	{
		CRegistry Reg;
		HKL hKbList[KBLIST_SIZE];

		m_strDefaultIme.Empty();
		int nTotalKbLayout = ::GetKeyboardLayoutList(KBLIST_SIZE, hKbList);

		for (int i = 0; i < nTotalKbLayout; i++)
		{
			// NOTE: ImmIsIME is not available for all versions of Windows.
			if (::ImmIsIME(hKbList[i]))
			{
				CString strKey;
				strKey.Format(KBLAYOUT_REG_KEY_FMT, hKbList[i]);
				Reg.SetRootKey(HKEY_LOCAL_MACHINE);
				if (Reg.OpenKey(strKey, false) && Reg.ValueExists(KBLAYOUT_REG_SUB_KEY))
				{
					CString strImeName = Reg.ReadString(KBLAYOUT_REG_SUB_KEY);
					m_ImeList.Add(strImeName, (PVOID)hKbList[i]);
					if (hKbList[i] == m_nDefaultKbLayout)
						m_strDefaultIme = strImeName;
				}
			}
		}

		m_ImeList.SetDupMode(CStrList::DM_IGNORE);
		m_ImeList.SetSorted(true);
	}

	return m_ImeList;
}

//-----------------------------------------------------------------------------

CString CScreen::GetDefaultIme()
{
	GetImeNames();
	return m_strDefaultIme;
}

//-----------------------------------------------------------------------------

HKL CScreen::GetDefaultKbLayout()
{
	return m_nDefaultKbLayout;
}

//-----------------------------------------------------------------------------

int CScreen::GetPixelsPerInch()
{
	return m_nPixelsPerInch;
}

///////////////////////////////////////////////////////////////////////////////
// CClipboard

#ifdef UNICODE
#define CF_TCHAR  CF_UNICODETEXT
#else
#define CF_TCHAR  CF_TEXT
#endif

std::auto_ptr<CClipboard> CClipboard::s_pSingleton(NULL);

//-----------------------------------------------------------------------------

CClipboard::CClipboard() :
	m_nOpenRefCount(0),
	m_bEmptied(false),
	m_hWndOwner(0)
{
	// nothing
}

//-----------------------------------------------------------------------------

CClipboard& CClipboard::Instance()
{
	if (s_pSingleton.get() == NULL)
		s_pSingleton.reset(new CClipboard());
	return *s_pSingleton;
}

//-----------------------------------------------------------------------------

HWND CClipboard::GetOwnerWindow()
{
	if (m_hWndOwner != 0)
		return m_hWndOwner;

	return ::AfxGetMainWnd()->GetSafeHwnd();
}

//-----------------------------------------------------------------------------

void CClipboard::BeforeSetData()
{
	if (m_nOpenRefCount != 0 && !m_bEmptied)
	{
		Clear();
		m_bEmptied = true;
	}
}

//-----------------------------------------------------------------------------

void CClipboard::SetBuffer(int nFormat, PVOID pBuffer, int nSize)
{
	HANDLE hHandle;
	PVOID pData;

	CAutoOpener AutoOpener(*this);

	hHandle = ::GlobalAlloc(GHND, nSize);
	try
	{
		class CGlobalMemLocker
		{
		private:
			HANDLE m_hHandle;
			PVOID m_pData;
		public:
			CGlobalMemLocker(HANDLE hHandle) : m_hHandle(hHandle)
				{ m_pData = ::GlobalLock(hHandle); }
			~CGlobalMemLocker()
				{ ::GlobalUnlock(m_hHandle); }
			PVOID GetDataPtr() { return m_pData; }
		};

		CGlobalMemLocker MemLocker(hHandle);
		pData = MemLocker.GetDataPtr();

		memmove(pData, pBuffer, nSize);
		BeforeSetData();
		::SetClipboardData(nFormat, hHandle);
	}
	catch (...)
	{
		::GlobalFree(hHandle);
		throw;
	}
}

//-----------------------------------------------------------------------------

void CClipboard::Open()
{
	if (m_nOpenRefCount == 0)
	{
		if (!::OpenClipboard(GetOwnerWindow()))
			IfcThrowException(SEM_OPEN_CLIPBOARD_ERROR);

		m_bEmptied = false;
	}

	m_nOpenRefCount++;
}

//-----------------------------------------------------------------------------

void CClipboard::Close()
{
	if (m_nOpenRefCount == 0) return;

	m_nOpenRefCount--;
	if (m_nOpenRefCount == 0)
		::CloseClipboard();
}

//-----------------------------------------------------------------------------

void CClipboard::Clear()
{
	CAutoOpener AutoOpener(*this);
	::EmptyClipboard();
}

//-----------------------------------------------------------------------------

void CClipboard::SetAsText(LPCTSTR lpszText)
{
	int nBytes = lstrlen(lpszText);
	SetBuffer(CF_TCHAR, (PVOID)lpszText, (nBytes + 1) * sizeof(TCHAR));
}

//-----------------------------------------------------------------------------

CString CClipboard::GetAsText()
{
	CString strResult;
	CAutoOpener AutoOpener(*this);

	HANDLE hHandle = ::GetClipboardData(CF_TCHAR);
	if (hHandle != 0)
	{
		LPTSTR pData = (LPTSTR)::GlobalLock(hHandle);
		strResult = pData;
		::GlobalUnlock(hHandle);
	}

	return strResult;
}

//-----------------------------------------------------------------------------

void CClipboard::SetAsHandle(int nFormat, HANDLE hHandle)
{
	CAutoOpener AutoOpener(*this);
	BeforeSetData();
	::SetClipboardData(nFormat, hHandle);
}

//-----------------------------------------------------------------------------

HANDLE CClipboard::GetAsHandle(int nFormat)
{
	CAutoOpener AutoOpener(*this);
	return ::GetClipboardData(nFormat);
}

//-----------------------------------------------------------------------------

int CClipboard::GetFormatCount()
{
	return ::CountClipboardFormats();
}

//-----------------------------------------------------------------------------

int CClipboard::GetFormat(int nIndex)
{
	CAutoOpener AutoOpener(*this);

	int nResult = ::EnumClipboardFormats(0);
	while (nIndex > 0)
	{
		nIndex--;
		nResult = ::EnumClipboardFormats(nResult);
	}

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// CUrl

CUrl::CUrl(LPCTSTR lpszUrl)
{
	SetUrl(lpszUrl);
}

CUrl::CUrl(const CUrl& src)
{
	(*this) = src;
}

//-----------------------------------------------------------------------------

void CUrl::Clear()
{
	m_strProtocol.Empty();
	m_strHost.Empty();
	m_strPort.Empty();
	m_strPath.Empty();
	m_strFileName.Empty();
	m_strBookmark.Empty();
	m_strUserName.Empty();
	m_strPassword.Empty();
	m_strParams.Empty();
}

//-----------------------------------------------------------------------------

CUrl& CUrl::operator = (const CUrl& rhs)
{
	if (this == &rhs) return *this;

	m_strProtocol = rhs.m_strProtocol;
	m_strHost = rhs.m_strHost;
	m_strPort = rhs.m_strPort;
	m_strPath = rhs.m_strPath;
	m_strFileName = rhs.m_strFileName;
	m_strBookmark = rhs.m_strBookmark;
	m_strUserName = rhs.m_strUserName;
	m_strPassword = rhs.m_strPassword;
	m_strParams = rhs.m_strParams;

	return *this;
}

//-----------------------------------------------------------------------------

CString CUrl::GetUrl() const
{
	const TCHAR SEP_CHAR = TEXT('/');
	CString strResult;

	if (!m_strProtocol.IsEmpty())
		strResult = m_strProtocol + TEXT("://");

	if (!m_strUserName.IsEmpty())
	{
		strResult += m_strUserName;
		if (!m_strPassword.IsEmpty())
			strResult = strResult + TEXT(":") + m_strPassword;
		strResult += TEXT("@");
	}

	strResult += m_strHost;

	if (!m_strPort.IsEmpty())
	{
		if (SameText(m_strProtocol, TEXT("HTTP")))
		{
			if (m_strPort != TEXT("80"))
				strResult = strResult + TEXT(":") + m_strPort;
		}
		else if (SameText(m_strProtocol, TEXT("HTTPS")))
		{
			if (m_strPort != TEXT("443"))
				strResult = strResult + TEXT(":") + m_strPort;
		}
		else if (SameText(m_strProtocol, TEXT("FTP")))
		{
			if (m_strPort != TEXT("21"))
				strResult = strResult + TEXT(":") + m_strPort;
		}
	}

	// path and filename
	CString str = m_strPath;
	if (!str.IsEmpty() && str[str.GetLength()-1] != SEP_CHAR)
		str += SEP_CHAR;
	str += m_strFileName;

	if (!str.IsEmpty())
	{
		if (!strResult.IsEmpty() && str[0] == SEP_CHAR) str.Delete(0);
		if (!m_strHost.IsEmpty() && strResult[strResult.GetLength()-1] != SEP_CHAR)
			strResult += SEP_CHAR;
		strResult += str;
	}

	if (!m_strParams.IsEmpty())
		strResult = strResult + TEXT("?") + m_strParams;

	if (!m_strBookmark.IsEmpty())
		strResult = strResult + TEXT("#") + m_strBookmark;

	return strResult;
}

//-----------------------------------------------------------------------------

CString CUrl::GetUrl(UINT nParts)
{
	CUrl Url(*this);

	if (!(nParts & URL_PROTOCOL)) Url.SetProtocol(TEXT(""));
	if (!(nParts & URL_HOST)) Url.SetHost(TEXT(""));
	if (!(nParts & URL_PORT)) Url.SetPort(TEXT(""));
	if (!(nParts & URL_PATH)) Url.SetPath(TEXT(""));
	if (!(nParts & URL_FILENAME)) Url.SetFileName(TEXT(""));
	if (!(nParts & URL_BOOKMARK)) Url.SetBookmark(TEXT(""));
	if (!(nParts & URL_USERNAME)) Url.SetUserName(TEXT(""));
	if (!(nParts & URL_PASSWORD)) Url.SetPassword(TEXT(""));
	if (!(nParts & URL_PARAMS)) Url.SetParams(TEXT(""));

	return Url.GetUrl();
}

//-----------------------------------------------------------------------------

void CUrl::SetUrl(LPCTSTR lpszValue)
{
	Clear();

	CString strUrl(lpszValue);
	if (strUrl.IsEmpty()) return;

	// get the bookmark
	int nPos = strUrl.ReverseFind('#');
	if (nPos >= 0)
	{
		m_strBookmark = strUrl.Mid(nPos + 1);
		strUrl.Delete(nPos, strUrl.GetLength());
	}

	// get the parameters
	nPos = strUrl.Find('?');
	if (nPos >= 0)
	{
		m_strParams = strUrl.Mid(nPos + 1);
		strUrl = strUrl.Mid(0, nPos);
	}

	CString strBuffer;
	nPos = strUrl.Find(TEXT("://"));
	if (nPos >= 0)
	{
		m_strProtocol = strUrl.Mid(0, nPos);
		strUrl.Delete(0, nPos + 3);
		// get the user name, password, host and the port number
		strBuffer = FetchStr(strUrl, '/', true);
		// get username and password
		nPos = strBuffer.Find('@');
		if (nPos >= 0)
		{
			m_strPassword = strBuffer.Mid(0, nPos);
			strBuffer.Delete(0, nPos + 1);
			m_strUserName = FetchStr(m_strPassword, ':');
			if (m_strUserName.IsEmpty())
				m_strPassword.Empty();
		}
		// get the host and the port number
		if (strBuffer.Find('[') >= 0 &&
			strBuffer.Find(']') > strBuffer.Find('['))
		{
			// this is for IPv6 Hosts
			m_strHost = FetchStr(strBuffer, ']');
			FetchStr(m_strHost, '[');
			FetchStr(strBuffer, ':');
		}
		else
			m_strHost = FetchStr(strBuffer, ':', true);
		m_strPort = strBuffer;
		// get the path
		nPos = strUrl.ReverseFind('/');
		if (nPos >= 0)
		{
			m_strPath = TEXT("/") + strUrl.Mid(0, nPos + 1);
			strUrl.Delete(0, nPos + 1);
		}
		else
			m_strPath = TEXT("/");
	}
	else
	{
		// get the path
		nPos = strUrl.ReverseFind('/');
		if (nPos >= 0)
		{
			m_strPath = strUrl.Mid(0, nPos + 1);
			strUrl.Delete(0, nPos + 1);
		}
	}

	// get the filename
	m_strFileName = strUrl;
}

///////////////////////////////////////////////////////////////////////////////
// CPacket

CPacket::CPacket()
{
	Init();
}

//-----------------------------------------------------------------------------

CPacket::~CPacket()
{
	Clear();
}

//-----------------------------------------------------------------------------

void CPacket::Init()
{
	m_pStream = NULL;
	m_bAvailable = false;
	m_bIsPacked = false;
}

//-----------------------------------------------------------------------------

void CPacket::ThrowUnpackError()
{
	IfcThrowException(SEM_PACKET_UNPACK_ERROR);
}

//-----------------------------------------------------------------------------

void CPacket::ThrowPackError()
{
	IfcThrowException(SEM_PACKET_PACK_ERROR);
}

//-----------------------------------------------------------------------------

void CPacket::CheckUnsafeSize(int nValue)
{
	const int MAX_UNSAFE_SIZE = 1024*1024*8;  // 8M

	if (nValue < 0 || nValue > MAX_UNSAFE_SIZE)
		IfcThrowException(SEM_UNSAFE_VALUE_IN_PACKET);
}

//-----------------------------------------------------------------------------

void CPacket::ReadBuffer(void *pBuffer, int nBytes)
{
	if (m_pStream->Read(pBuffer, nBytes) != nBytes)
		ThrowUnpackError();
}

//-----------------------------------------------------------------------------

void CPacket::ReadString(std::string& str)
{
	DWORD nSize;

	str.clear();
	if (m_pStream->Read(&nSize, sizeof(DWORD)) == sizeof(DWORD))
	{
		CheckUnsafeSize(nSize);
		if (nSize > 0)
		{
			str.resize(nSize);
			if (m_pStream->Read((void*)str.data(), nSize) != nSize)
				ThrowUnpackError();
		}
	}
	else
	{
		ThrowUnpackError();
	}
}

//-----------------------------------------------------------------------------

void CPacket::ReadString(CString& str)
{
	std::string s;
	ReadString(s);
	str = CA2T(s.c_str());
}

//-----------------------------------------------------------------------------

void CPacket::ReadBlob(std::string& str)
{
	ReadString(str);
}

//-----------------------------------------------------------------------------

void CPacket::ReadBlob(CStringA& str)
{
	std::string s;
	ReadString(s);
	str = s.c_str();
}

//-----------------------------------------------------------------------------

void CPacket::ReadBlob(CStream& Stream)
{
	std::string str;

	ReadBlob(str);
	Stream.SetSize(0);
	if (!str.empty())
		Stream.Write((void*)str.c_str(), (int)str.length());
}

//-----------------------------------------------------------------------------

void CPacket::ReadBlob(CBuffer& Buffer)
{
	std::string str;

	ReadBlob(str);
	Buffer.SetSize(0);
	if (!str.empty())
		Buffer.Assign((void*)str.c_str(), (int)str.length());
}

//-----------------------------------------------------------------------------

void CPacket::WriteBuffer(const void *pBuffer, int nBytes)
{
	IFC_ASSERT(pBuffer && nBytes >= 0);
	m_pStream->Write(pBuffer, nBytes);
}

//-----------------------------------------------------------------------------

void CPacket::WriteString(const std::string& str)
{
	DWORD nSize;

	nSize = (DWORD)str.length();
	m_pStream->Write(&nSize, sizeof(DWORD));
	if (nSize > 0)
		m_pStream->Write((void*)str.c_str(), nSize);
}

//-----------------------------------------------------------------------------

void CPacket::WriteString(const CString& str)
{
	std::string s = CT2A(str);
	WriteString(s);
}

//-----------------------------------------------------------------------------

void CPacket::WriteBlob(void *pBuffer, int nBytes)
{
	DWORD nSize = 0;

	if (pBuffer && nBytes >= 0)
		nSize = (DWORD)nBytes;
	m_pStream->Write(&nSize, sizeof(DWORD));
	if (nSize > 0)
		m_pStream->Write(pBuffer, nSize);
}

//-----------------------------------------------------------------------------

void CPacket::WriteBlob(const CBuffer& Buffer)
{
	WriteBlob(Buffer.Data(), Buffer.GetSize());
}

//-----------------------------------------------------------------------------

void CPacket::FixStrLength(std::string& str, int nLength)
{
	if ((int)str.length() != nLength)
		str.resize(nLength, 0);
}

//-----------------------------------------------------------------------------

void CPacket::TruncString(std::string& str, int nMaxLength)
{
	if ((int)str.length() > nMaxLength)
		str.resize(nMaxLength);
}

//-----------------------------------------------------------------------------

bool CPacket::Pack()
{
	bool bResult;

	try
	{
		delete m_pStream;
		m_pStream = new CMemoryStream(DEFAULT_MEMORY_DELTA);
		DoPack();
		DoAfterPack();
		DoCompress();
		DoEncrypt();
		m_bAvailable = true;
		m_bIsPacked = true;
		bResult = true;
	}
	catch (CException* e)
	{
		e->Delete();
		bResult = false;
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CPacket::Unpack(void *pBuffer, int nBytes)
{
	bool bResult;

	try
	{
		delete m_pStream;
		m_pStream = new CMemoryStream(DEFAULT_MEMORY_DELTA);
		m_pStream->SetSize(nBytes);
		memmove(m_pStream->GetMemory(), pBuffer, nBytes);
		DoDecrypt();
		DoDecompress();
		DoUnpack();
		m_bAvailable = true;
		m_bIsPacked = false;
		bResult = true;
	}
	catch (CException* e)
	{
		e->Delete();
		bResult = false;
		Clear();
	}

	delete m_pStream;
	m_pStream = NULL;

	return bResult;
}

//-----------------------------------------------------------------------------

bool CPacket::Unpack(const CBuffer& Buffer)
{
	return Unpack(Buffer.Data(), Buffer.GetSize());
}

//-----------------------------------------------------------------------------

void CPacket::Clear()
{
	delete m_pStream;
	m_pStream = NULL;
	m_bAvailable = false;
	m_bIsPacked = false;
}

//-----------------------------------------------------------------------------

void CPacket::EnsurePacked()
{
	if (!IsPacked()) Pack();
}

///////////////////////////////////////////////////////////////////////////////
// CBits

const int BITS_PER_BYTE = 8;

//-----------------------------------------------------------------------------

CBits::CBits() :
	m_nBitCount(0)
{
	// nothing
}

CBits::CBits(const CBits& src)
{
	(*this) = src;
}

CBits::~CBits()
{
	// nothing
}

//-----------------------------------------------------------------------------

int CBits::CalcByteCount(int nBitCount)
{
	return ((nBitCount + BITS_PER_BYTE - 1) / BITS_PER_BYTE) * sizeof(BYTE);
}

//-----------------------------------------------------------------------------

bool CBits::GetBit(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= m_nBitCount)) return false;

	int nResult = 0;
	void *p = m_Buffer.Data();

	__asm
	{
		MOV     EAX, p
		MOV     ECX, nIndex
		BT      [EAX], ECX
		SBB     EAX, EAX
		AND     EAX, 1
		MOV     nResult, EAX
	}

	return (nResult != 0);
}

//-----------------------------------------------------------------------------

void CBits::SetBit(int nIndex, bool bValue)
{
	IFC_ASSERT((nIndex >= 0) && (nIndex < m_nBitCount));

	void *p = m_Buffer.Data();

	__asm
	{
		PUSH    ECX
		MOV     EAX, p
		MOV     CL, bValue
		OR      CL, CL
		JZ      _1
		MOV     ECX, nIndex
		BTS     [EAX], ECX
		JMP     _2
	_1:
		MOV     ECX, nIndex
		BTR     [EAX], ECX
	_2:
		POP     ECX
	}
}

//-----------------------------------------------------------------------------

void CBits::SetAllBits()
{
	if (m_Buffer.GetSize() > 0)
	{
		memset(m_Buffer.Data(), 255, m_Buffer.GetSize());
		FillUnusedBits(false);
	}
}

//-----------------------------------------------------------------------------

void CBits::ClearAllBits()
{
	if (m_Buffer.GetSize() > 0)
		memset(m_Buffer.Data(), 0, m_Buffer.GetSize());
}

//-----------------------------------------------------------------------------

void CBits::FillUnusedBits(bool bValue) const
{
	int nBitsInLastByte = m_nBitCount % BITS_PER_BYTE;
	if (nBitsInLastByte != 0)
	{
		BYTE *pByte = (BYTE*)(m_Buffer.Data() + GetByteCount() - 1);
		if (bValue)
		{
			BYTE nMask = (((BYTE)(0xFF)) << nBitsInLastByte);
			*pByte |= nMask;
		}
		else
		{
			BYTE nMask = ~(((BYTE)(0xFF)) << nBitsInLastByte);
			*pByte &= nMask;
		}
	}
}

//-----------------------------------------------------------------------------

void CBits::AndBits(const CBits& src)
{
	int nByteCount = Min(GetByteCount(), src.GetByteCount());
	BYTE *pSrcByte = (BYTE*)src.m_Buffer.Data();
	BYTE *pDestByte = (BYTE*)m_Buffer.Data();

	for (int i = 0; i < nByteCount; i++)
	{
		*pDestByte &= *pSrcByte;
		pSrcByte++;
		pDestByte++;
	}

	FillUnusedBits(0);
}

//-----------------------------------------------------------------------------

void CBits::OrBits(const CBits& src)
{
	int nByteCount = Min(GetByteCount(), src.GetByteCount());
	BYTE *pSrcByte = (BYTE*)src.m_Buffer.Data();
	BYTE *pDestByte = (BYTE*)m_Buffer.Data();

	for (int i = 0; i < nByteCount; i++)
	{
		*pDestByte |= *pSrcByte;
		pSrcByte++;
		pDestByte++;
	}

	FillUnusedBits(0);
}

//-----------------------------------------------------------------------------

void CBits::XorBits(const CBits& src)
{
	int nByteCount = Min(GetByteCount(), src.GetByteCount());
	BYTE *pSrcByte = (BYTE*)src.m_Buffer.Data();
	BYTE *pDestByte = (BYTE*)m_Buffer.Data();

	for (int i = 0; i < nByteCount; i++)
	{
		*pDestByte ^= *pSrcByte;
		pSrcByte++;
		pDestByte++;
	}

	FillUnusedBits(0);
}

//-----------------------------------------------------------------------------

void CBits::NotBits()
{
	int nByteCount = GetByteCount();
	BYTE *pByte = (BYTE*)m_Buffer.Data();

	for (int i = 0; i < nByteCount; i++)
	{
		*pByte = ~*pByte;
		pByte++;
	}

	FillUnusedBits(0);
}

//-----------------------------------------------------------------------------

void CBits::SetBitCount(int nCount)
{
	if (nCount < 0) nCount = 0;
	if (nCount != m_nBitCount)
	{
		int nNewSize = CalcByteCount(nCount);
		int nOldSize = CalcByteCount(m_nBitCount);

		if (nNewSize != nOldSize)
			m_Buffer.SetSize(nNewSize, true);

		m_nBitCount = nCount;
	}
}

//-----------------------------------------------------------------------------

int CBits::GetTotalBitsSet() const
{
	static const BYTE TABLE[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

	FillUnusedBits(false);
	int nResult = 0;
	int nByteCount = GetByteCount();
	BYTE *pByte = (BYTE*)m_Buffer.Data();

	for (int i = 0; i < nByteCount; i++, pByte++)
	{
		nResult += TABLE[*pByte & 0x0F];
		nResult += TABLE[*pByte >> 4];
	}

	return nResult;
}

//-----------------------------------------------------------------------------

bool CBits::IsAllBitsSet() const
{
	FillUnusedBits(true);

	bool bResult = true;
	int nByteCount = GetByteCount();
	BYTE *pByte = (BYTE*)m_Buffer.Data();

	for (int i = 0; i < nByteCount; i++, pByte++)
	{
		if (*pByte != (BYTE)0xFF)
		{
			bResult = false;
			break;
		}
	}

	FillUnusedBits(false);
	return bResult;
}

//-----------------------------------------------------------------------------

bool CBits::IsAllBitsCleared() const
{
	FillUnusedBits(false);

	bool bResult = true;
	int nByteCount = GetByteCount();
	BYTE *pByte = (BYTE*)m_Buffer.Data();

	for (int i = 0; i < nByteCount; i++, pByte++)
	{
		if (*pByte != 0)
		{
			bResult = false;
			break;
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------

int CBits::FindFirstClearedBit(int nStartIndex) const
{
	if (nStartIndex < 0 || nStartIndex >= m_nBitCount)
		return -1;

	const int BITS_PER_DWORD = 32;

	int m = nStartIndex % BITS_PER_DWORD;
	int n = m_nBitCount % BITS_PER_DWORD;

	int nAlignedStartIndex = nStartIndex - m + (m ? BITS_PER_DWORD : 0);
	int nAlignedEndIndex = m_nBitCount - n - 1;

	if (m > 0)
	{
		for (int i = nStartIndex; i < nAlignedStartIndex; i++)
		{
			if (!GetBit(i))
				return i;
		}
	}

	int nStartDWordIndex = nAlignedStartIndex / BITS_PER_DWORD;
	int nEndDWordIndex = nAlignedEndIndex / BITS_PER_DWORD;
	DWORD *p = (DWORD*)m_Buffer.Data() + nStartDWordIndex;

	for (int i = nStartDWordIndex; i <= nEndDWordIndex; i++,p++)
	{
		if (*p != 0xFFFFFFFF)
		{
			DWORD nValue = *p;
			bool bFound = false;
			int nIndex;

			__asm
			{
				MOV   EAX, nValue
				NOT   EAX
				BSF   EDX, EAX
				SETNZ bFound
				MOV   nIndex, EDX
			}

			if (bFound)
				return i * BITS_PER_DWORD + nIndex;
		}
	}

	if (n > 0)
	{
		for (int i = nAlignedEndIndex + 1; i < m_nBitCount; i++)
		{
			if (!GetBit(i))
				return i;
		}
	}

	return -1;
}

//-----------------------------------------------------------------------------

void CBits::GetBuffer(CBuffer& Buffer) const
{
	Buffer = m_Buffer;
}

//-----------------------------------------------------------------------------

void CBits::SetBuffer(const CBuffer& Buffer, int nBitCount)
{
	IFC_ASSERT(CalcByteCount(nBitCount) == Buffer.GetSize());

	m_Buffer = Buffer;
	m_nBitCount = nBitCount;
}

//-----------------------------------------------------------------------------

int CBits::GetByteCount() const
{
	return CalcByteCount(m_nBitCount);
}

//-----------------------------------------------------------------------------

CBits& CBits::operator = (const CBits& rhs)
{
	if (this == &rhs) return *this;

	m_Buffer = rhs.m_Buffer;
	m_nBitCount = rhs.m_nBitCount;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// CLogger

CLogger::CLogger() :
	m_bNewFileDaily(false)
{
	m_pLock = new CCriticalSection();
}

CLogger::~CLogger()
{
	SAFE_DELETE(m_pLock);
}

//-----------------------------------------------------------------------------

CLogger& CLogger::Instance()
{
	static CLogger obj;
	return obj;
}

//-----------------------------------------------------------------------------

CString CLogger::GetLogFileName()
{
	CString strResult = m_strFileName;

	if (strResult.IsEmpty())
		strResult = GetAppPath() + TEXT("log.txt");

	if (m_bNewFileDaily)
	{
		CString strFileExt = ExtractFileExt(strResult);
		strResult = strResult.Mid(0, strResult.GetLength() - strFileExt.GetLength());
		strResult += CTime::GetCurrentTime().Format(TEXT(".%Y%m%d"));
		strResult += strFileExt;
	}

	return strResult;
}

//-----------------------------------------------------------------------------

bool CLogger::OpenFile(CFileStream& FileStream, LPCTSTR lpszFileName)
{
	return
		FileStream.Open(lpszFileName, FM_OPEN_WRITE | FM_SHARE_DENY_NONE) ||
		FileStream.Open(lpszFileName, FM_CREATE | FM_SHARE_DENY_NONE);
}

//-----------------------------------------------------------------------------

void CLogger::WriteToFile(const char *str)
{
	CAutoLocker Locker(m_pLock);

	CString strFileName = GetLogFileName();
	CFileStream fs;
	if (!OpenFile(fs, strFileName))
	{
		CString strPath = ExtractFilePath(strFileName);
		if (!strPath.IsEmpty())
		{
			ForceDirectories(strPath);
			OpenFile(fs, strFileName);
		}
	}

	if (fs.IsOpen())
	{
		fs.Seek(0, SO_END);
		fs.Write(str, (int)strlen(str));
	}
}

//-----------------------------------------------------------------------------

void CLogger::SetFileName(LPCTSTR lpszFileName, bool bNewFileDaily)
{
	m_strFileName = lpszFileName;
	m_bNewFileDaily = bNewFileDaily;
}

//-----------------------------------------------------------------------------

void CLogger::WriteStr(LPCTSTR lpszStr)
{
	CString strTime = CTime::GetCurrentTime().Format(TEXT("%Y-%m-%d %H:%M:%S"));
	UINT nProcessId = GetCurrentProcessId();
	UINT nThreadId = GetCurrentThreadId();

	CString strResult;
	strResult.Format(TEXT("[%s](%05d|%05u) %s\r\n"),
		strTime, nProcessId, nThreadId, lpszStr);

	WriteToFile(CT2A(strResult));

#ifdef _DEBUG
	OutputDebugString(strResult);
#endif
}

//-----------------------------------------------------------------------------

void CLogger::WriteFmt(LPCTSTR lpszFormat, ...)
{
	CString strText;

	va_list argList;
	va_start(argList, lpszFormat);
	strText.FormatV(lpszFormat, argList);
	va_end(argList);

	WriteStr(strText);
}

//-----------------------------------------------------------------------------

void CLogger::WriteException(CException *e)
{
	IFC_ASSERT(e != NULL);

	const int MAX_SIZE = 1024;
	TCHAR szMsg[MAX_SIZE];
	if (e->GetErrorMessage(szMsg, MAX_SIZE))
		WriteFmt(CString(TEXT("ERROR: ")) + szMsg);
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
