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

/// @file ifc_classes.h
/// Defines the common classes.

/// @addtogroup Classes
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_exceptions.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Classes

class CBuffer;
class CStream;
class CCustomMemoryStream;
class CMemoryStream;
class CFileStream;
class CResourceStream;
class CSeqNumberAlloc;
class CPointerList;
class CStrings;
class CStrList;
class CHashedStrList;
class CNameValueList;
class CRegistry;
class CCustomIniFile;
class CMemIniFile;
class CIniFile;
class CMonitor;
class CScreen;
class CClipboard;
class CUrl;
class CPacket;
class CBits;
class CAutoInvokable;
class CAutoInvoker;
class CLogger;
template<typename ObjectType> class CCustomObjectList;
template<typename ObjectType> class CObjectList;
template<typename SenderType, typename ParamType> class IEventHandler;
template<typename SenderType, typename ParamType> class CEventInvoker;
template<typename CallBackType> class CCallBackDef;
template<typename CallBackType> class CCallBackList;

class CCriticalSection;

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

/// Returns the global object of CScreen.
/// @remarks
///   - The global CScreen object is created when the function is accessed for the first time.
///   - The global CScreen object is deleted automatically when application terminates.
CScreen& Screen();

/// Returns the global object of CClipboard.
/// @remarks
///   - The global CClipboard object is created when the function is accessed for the first time.
///   - The global CClipboard object is deleted automatically when application terminates.
CClipboard& Clipboard();

/// Returns the global object of CLogger.
/// @remarks
///   - The global CLogger object is created when the function is accessed for the first time.
///   - The global CLogger object is deleted automatically when application terminates.
CLogger& Logger();

///////////////////////////////////////////////////////////////////////////////
/// CBuffer - Memory buffer wrapper class.

class CBuffer
{
protected:
	void *m_pBuffer;    ///< The buffer pointer.
	int m_nSize;        ///< The buffer size.
private:
	inline void Init() { m_pBuffer = NULL; m_nSize = 0; }
	void Assign(const CBuffer& src);
public:
	/// Default Constructor.
	CBuffer();
	/// Copy Constructor.
	CBuffer(const CBuffer& src);
	/// Constructor using specified initial buffer size.
	explicit CBuffer(int nSize);
	/// Constructor using specified buffer pointer and buffer size.
	CBuffer(const void *pBuffer, int nSize);
	/// Destructor.
	virtual ~CBuffer();

	/// Assignment operator.
	CBuffer& operator = (const CBuffer& rhs);

	/// Gets the byte value as const char by index(0-based).
	const char& operator[] (int nIndex) const { return ((char*)m_pBuffer)[nIndex]; }

	/// Gets the byte value as char by index(0-based).
	char& operator[] (int nIndex) { return const_cast<char&>(((const CBuffer&)(*this))[nIndex]); }

	/// Gets the buffer pointer.
	operator char*() const { return (char*)m_pBuffer; }

	/// Return the buffer pointer.
	char* Data() const { return (char*)m_pBuffer; }

	/// Returns the buffer pointer as a regular C string, the return string is null-terminated.
	///
	/// @remarks
	///   A NULL char will be appended to the end of buffer when this function was called.
	char* c_str() const;

	/// Assign a specified memory to buffer.
	///
	/// @param[in] pBuffer
	///   The source memory buffer.
	/// @param[in] nSize
	///   The source memory size in bytes.
	virtual void Assign(const void *pBuffer, int nSize);

	/// Sets the buffer size in bytes.
	///
	/// @param[in] nSize
	///   The buffer size in bytes.
	/// @param[in] bInitZero
	///   Fill buffer with zeros or not.
	/// @remarks
	///   The new buffer will reserve old data.
	virtual void SetSize(int nSize, bool bInitZero = false);

	/// Get the size of buffer in bytes.
	virtual int GetSize() const { return m_nSize; }

	/// Empty the buffer.
	void Clear() { SetSize(0); }

	/// Loads the entire contents of a stream into the buffer.
	///
	/// LoadFromStream reallocates the memory buffer so that the contents of the source stream will
	/// exactly fit. It calls the SetSize() accordingly, and then reads the entire contents of the
	/// source stream into the memory buffer. Thus, LoadFromStream will discard any pre-existing data
	/// stored in the buffer.
	///
	/// @param[in] Stream
	///   The stream to be loaded from.
	bool LoadFromStream(CStream& Stream);

	/// Loads the entire contents of a file into the buffer.
	///
	/// LoadFromFile reallocates the memory buffer so that the contents of the file will exactly fit.
	/// It calls the SetSize() accordingly, and then reads the entire contents of the file into the
	/// memory buffer. Thus, LoadFromFile will discard any pre-existing data stored in the buffer.
	///
	/// @param[in] lpszFileName
	///   The name of the file to be loaded from.
	bool LoadFromFile(LPCTSTR lpszFileName);

	/// Writes the entire contents of the memory buffer to the stream object specified by Stream parameter.
	///
	/// Use SaveToStream to copy data that is stored in memory into another storage medium.
	/// SaveToStream writes the entire contents of memory buffer into the indicated stream object,
	/// starting at the current position in the stream.
	///
	/// @param[in] Stream
	///   The stream to be saved to.
	bool SaveToStream(CStream& Stream) const;

	/// Writes the entire contents of the memory buffer to the file with a given file name.
	///
	/// Use SaveToFile to write the contents of memory buffer to a file.
	/// SaveToFile allows an application to write out the contents of the memory stream without
	/// having to explicitly create and free a file stream object. If the named file cannot be created
	/// or opened, SaveToFile throws a CIfcFileException exception.
	///
	/// @param[in] lpszFileName
	///   The name of the file to be saved to.
	bool SaveToFile(LPCTSTR lpszFileName) const;
};

///////////////////////////////////////////////////////////////////////////////
/// CStream - The base class for stream.
///
/// CStream is the base class type for stream objects that can read from or write to
/// various kinds of storage media, such as disk files, dynamic memory, and so on.
///
/// Use specialized stream objects to read from, write to, or copy information stored
/// in a particular medium. Each descendant of CStream implements methods for transferring
/// information to and from a particular storage medium, such as a disk file, dynamic memory,
/// and so on. In addition to methods for reading, writing, and copying bytes to and from
/// the stream, stream objects permit applications to seek to an arbitrary position in the
/// stream.

class CStream
{
public:
	/// Destructor
	virtual ~CStream() {}

	/// Introduces an abstract or pure virtual method responsible for reading from the stream.
	///
	/// @param[in] pBuffer
	///   Storge the data read from stream.
	/// @param[in] nBytes
	///   Maximum number of bytes to be read.
	/// @return
	///   The number of bytes actually read.
	/// @remarks
	///   Each descendant stream object defines a Read method that reads data from its particular
	///   storage medium (such as memory or a disk file).
	virtual int Read(void *pBuffer, int nBytes) = 0;

	/// Introduces a pure virtual method for writing to the stream.
	///
	/// @param[in] pBuffer
	///   Pointer to the data to be written.
	/// @param[in] nBytes
	///   Maximum number of bytes to be written.
	/// @return
	///   The number of bytes actually written.
	/// @remarks
	///   Each descendant stream object defines a Write method that writes data to its particular
	///   storage medium (such as memory or a disk file).
	virtual int Write(const void *pBuffer, int nBytes) = 0;

	/// Moves to a specified position in the streamed resource.
	///
	/// @param[in] nOffset
	///   Number of bytes from @a nSeekOrigin.
	/// @param[in] nSeekOrigin
	///   The seek origin, indicates how to interpret the @a nOffset parameter.
	/// @return
	///   The new value of GetPosition().
	virtual INT64 Seek(INT64 nOffset, SEEK_ORIGIN nSeekOrigin) = 0;

	/// Reads bytes from the stream into @a pBuffer.
	///
	/// @param[in] pBuffer
	///   Storge the data read from stream.
	/// @param[in] nBytes
	///   Number of bytes to be read.
	/// @remarks
	///   Use ReadBuffer to read @a nBytes bytes from the stream into a buffer in cases where the
	///   number of bytes is known and fixed, for example when reading in structures.
	///   ReadBuffer calls Read() to do the actual reading. If @a nBytes bytes cannot be read from
	///   the stream, a CIfcStreamException exception is thrown.
	void ReadBuffer(void *pBuffer, int nBytes);

	/// Writes @a nBytes bytes from @a pBuffer onto the stream and advances the current position of the
	/// stream by @a nBytes bytes.
	///
	/// @param[in] pBuffer
	///   Pointer to the data to be written.
	/// @param[in] nBytes
	///   Number of bytes to be written.
	/// @remarks
	///   WriteBuffer calls Write() to handle the actual writing. If the stream fails to write
	///   all the requested bytes, a CIfcStreamException exception is thrown.
	void WriteBuffer(const void *pBuffer, int nBytes);

	/// Copies a specified number of bytes from one stream to another.
	///
	/// @param[in] Source
	///   The source stream.
	/// @param[in] nBytes
	///   Number of bytes to be copied.
	/// @return
	///   The number of bytes copied.
	/// @remarks
	///   If @a nBytes is 0, CopyFrom sets Source position to 0 before reading and then copies the
	///   entire contents of Source into the stream. If @a nBytes is greater than or less than 0,
	///   CopyFrom reads from the current position in Source.
	INT64 CopyFrom(CStream& Source, INT64 nBytes);

	/// Get the current position of the stream.
	/// The position is the number of bytes from the beginning of the streamed data.
	INT64 GetPosition() const { return const_cast<CStream&>(*this).Seek(0, SO_CURRENT); }

	/// Set the current position of the stream.
	void SetPosition(INT64 nPos) { Seek(nPos, SO_BEGINNING); }

	/// Get the size in bytes of the stream.
	virtual INT64 GetSize() const;

	/// Set the size in bytes of the stream.
	///
	/// @remarks
	///   Setting the size of CStream does nothing. Descendants of CStream can override this
	///   method to allow applications to change the size of the resource accessed using the stream.
	virtual void SetSize(INT64 nSize) {}
};

///////////////////////////////////////////////////////////////////////////////
/// CCustomMemoryStream - The base class for memory stream.
///
/// Use CCustomMemoryStream as a base class when defining a stream object that can transfer data
/// that is stored in memory. Memory streams are useful for providing file-like access to data that
/// is stored in a less accessible medium. Data can be moved to an internal memory buffer when the
/// memory stream is created. After manipulating the data in a memory stream, the data can be written
/// out to its actual storage medium when the memory stream is destroyed.
/// Do not instantiate an instance of CCustomMemoryStream. It is an abstract class that implements
/// behavior common to all memory streams. To work with an instance of a memory stream, use one of
/// the descendants of CCustomMemoryStream, such as CMemoryStream or CResourceStream.

class CCustomMemoryStream : public CStream
{
protected:
	char *m_pMemory;        ///< The memory buffer pointer.
	int m_nSize;            ///< The current stream size.
	int m_nPosition;        ///< The current stream position.
protected:
	/// Set the m_pMemory and m_nSize.
	void SetPointer(char *pMemory, int nSize);
public:
	/// Default constructor.
	CCustomMemoryStream();

	/// Reads up to @a nBytes bytes from the memory stream into @a pBuffer and advances the current position
	/// of the stream by the number of bytes read.
	virtual int Read(void *pBuffer, int nBytes);

	/// Moves the current position of the stream by @a nOffset bytes, relative to the origin specified
	/// by @a nSeekOrigin.
	virtual INT64 Seek(INT64 nOffset, SEEK_ORIGIN nSeekOrigin);

	/// Writes the entire contents of the memory stream to the stream object specified by @a Stream.
	bool SaveToStream(CStream& Stream);

	/// Writes the entire contents of the memory stream to the file with a given file name.
	bool SaveToFile(LPCTSTR lpszFileName);

	/// Provides direct access to the memory pool allocated for the memory stream.
	char* GetMemory() { return m_pMemory; }
};

///////////////////////////////////////////////////////////////////////////////
/// CMemoryStream - Memory stream class.
///
/// Use CMemoryStream to store data in a dynamic memory buffer that is enhanced with file-like access
/// capabilities. CMemoryStream provides the general I/O capabilities of a stream object while
/// introducing methods and properties to manage a dynamic memory buffer.

class CMemoryStream : public CCustomMemoryStream
{
public:
	enum { DEFAULT_MEMORY_DELTA = 1024 };   ///< The default value of memory growth (must be 2^N)
	enum { MIN_MEMORY_DELTA = 256 };        ///< The minimum value of memory growth.

private:
	int m_nCapacity;
	int m_nMemoryDelta;
private:
	void SetMemoryDelta(int nNewMemoryDelta);
	void SetCapacity(int nNewCapacity);
	char* Realloc(int& nNewCapacity);
public:
	/// Constructor using specified memory growth value.
	explicit CMemoryStream(int nMemoryDelta = DEFAULT_MEMORY_DELTA);
	/// Destructor
	virtual ~CMemoryStream();

	/// Writes @a nBytes bytes from @a pBuffer to the current position in the memory buffer and
	/// updates the current position by @a nBytes bytes.
	///
	/// Use Write to insert @a nBytes bytes into the memory buffer of the memory stream, starting at
	/// the current position. Write will increase the size of the memory buffer, if necessary,
	/// to accommodate the data being written in. If the current position is not the end of the
	/// memory buffer, Write will overwrite the data following the current position.
	/// Write always writes the @a nBytes bytes in the @a pBuffer, unless there is a memory failure.
	/// Thus, for CMemoryStream, Write() is equivalent to the WriteBuffer() method.
	///
	/// @param[in] pBuffer
	///   Pointer to the data to be written.
	/// @param[in] nBytes
	///   Maximum number of bytes to be written.
	/// @return
	///   The number of bytes actually written.
	virtual int Write(const void *pBuffer, int nBytes);

	/// Sets the size of memory stream.
	///
	/// Use SetSize to set the size of a memory stream before filling it with data. SetSize
	/// allocates the memory buffer to hold @a nSize bytes, preserving as much of the existing
	/// data as possible.
	///
	/// @param[in] nSize
	///   The new size of memory stream size.
	virtual void SetSize(INT64 nSize);

	/// Loads the entire contents of a stream into the memory buffer.
	///
	/// @param[in] Stream
	///   The stream to be loaded from.
	/// @remarks
	///   LoadFromStream always sets the position of the source stream to 0, before streaming in
	///   the number of bytes indicated by the source stream's size.
	bool LoadFromStream(CStream& Stream);

	/// Loads the entire contents of a file into the memory buffer.
	///
	/// LoadFromFile reallocates the memory buffer so that the contents of the file will exactly fit.
	/// It calls the SetSize() accordingly, and then reads the entire contents of the file into the
	/// memory stream. Thus, LoadFromFile will discard any pre-existing data stored in the buffer.
	///
	/// @param[in] lpszFileName
	///   The name of the file to be loaded from.
	bool LoadFromFile(LPCTSTR lpszFileName);

	/// Sets the memory stream size to 0, discarding all data associated with the memory stream.
	void Clear();
};

///////////////////////////////////////////////////////////////////////////////
/// CFileStream - File stream class.
///
/// Use CFileStream to access the information in disk files. CFileStream will open a named file and
/// provide methods to read from or write to it.

class CFileStream : public CStream
{
private:
	CString m_strFileName;
	HANDLE m_hHandle;
private:
	void Init();
	HANDLE FileCreate(LPCTSTR lpszFileName, DWORD nOpenMode);
	HANDLE FileOpen(LPCTSTR lpszFileName, DWORD nOpenMode);
	void FileClose(HANDLE hHandle);
	int FileRead(HANDLE hHandle, void *pBuffer, int nBytes);
	int FileWrite(HANDLE hHandle, const void *pBuffer, int nBytes);
	INT64 FileSeek(HANDLE hHandle, INT64 nOffset, SEEK_ORIGIN nSeekOrigin);
public:
	/// Default constructor.
	/// The default constructor does nothing but initializes class members.
	CFileStream();

	/// Constructs object and opens a file.
	///
	/// @param[in] lpszFileName
	///   The name of the file to be opened.
	/// @param[in] nOpenMode
	///   Access and sharing mode. Specifies the action to take when opening the file.
	///   You can combine options listed below by using the bitwise-OR (|) operator.
	///   See enum type @ref FILE_OPEN_MODE.
	/// @remarks
	///   If the file cannot be created or opened, the constructor throws a CIfcFileException exception.
	CFileStream(LPCTSTR lpszFileName, DWORD nOpenMode);

	/// Destructor.
	/// The destructor closes the file handle before destroying the object.
	virtual ~CFileStream();

	/// Safely opens or creates a file with an error-testing option.
	///
	/// @param[in] lpszFileName
	///   The name of the file to be opened.
	/// @param[in] nOpenMode
	///   Access and sharing mode. Specifies the action to take when opening the file.
	///   You can combine options listed below by using the bitwise-OR (|) operator.
	///   See enum type @ref FILE_OPEN_MODE.
	/// @param[in] pException
	///   A pointer to an existing file-exception object that will receive the status of a failed operation.
	///   If @a pException is NULL, it will be ignored.
	/// @return
	///   True indicates success, false indicates failure.
	///   The @a pException parameter is meaningful only if false is returned.
	bool Open(LPCTSTR lpszFileName, DWORD nOpenMode, CIfcFileException* pException = NULL);

	/// Closes the file associated with this object and makes the file unavailable for reading or writing.
	void Close();

	/// Reads up to @a nBytes bytes of data from the file stream into @a pBuffer.
	///
	/// @param[in] pBuffer
	///   Storge the data read from stream.
	/// @param[in] nBytes
	///   Maximum number of bytes to be read.
	/// @return
	///   The number of bytes actually read.
	virtual int Read(void *pBuffer, int nBytes);

	/// Writes @a nBytes bytes from the @a pBuffer to the current position in the file.
	///
	/// @param[in] pBuffer
	///   Pointer to the data to be written.
	/// @param[in] nBytes
	///   Maximum number of bytes to be written.
	/// @return
	///   The number of bytes actually written.
	virtual int Write(const void *pBuffer, int nBytes);


	/// Moves to a specified position in the streamed file.
	///
	/// @param[in] nOffset
	///   Number of bytes from @a nSeekOrigin.
	/// @param[in] nSeekOrigin
	///   The seek origin, indicates how to interpret the @a nOffset parameter.
	/// @return
	///   The new value of GetPosition().
	virtual INT64 Seek(INT64 nOffset, SEEK_ORIGIN nSeekOrigin);

	/// Set the size in bytes of the file.
	virtual void SetSize(INT64 nSize);

	/// Get the file name of the current opened file.
	const CString& GetFileName() const { return m_strFileName; }

	/// Get the handle of the current opened file.
	HANDLE GetHandle() const { return m_hHandle; }

	/// Indicates whether the file stream is currently open.
	bool IsOpen() const;
};

///////////////////////////////////////////////////////////////////////////////
/// CResourceStream - Resource stream class.
///
/// Use CResourceStream to read the resources of an application. An instance of CResourceStream holds
/// the value of a single resource in a memory buffer where it is accessible to the application.

class CResourceStream : public CCustomMemoryStream
{
private:
	HRSRC m_hResInfo;
	HANDLE m_hGlobal;
private:
	void Initialize(HINSTANCE hInstance, PVOID pResName, LPCTSTR lpszResType, bool bIsFromId);
public:
	/// Construct the object associated with a particular resource name and type.
	/// The constructor finds the resource data and loads it into the memory buffer.
	///
	/// @param[in] hInstance
	///   The instance handle associated with the executable or shared library that contains the resource.
	/// @param[in] lpszResName
	///   The string associated with the resource in the .rc file that was compiled with the application.
	///   If the resource is associated with an integer ID rather than a string, use the string
	///   representation of the integer after a pound sign. Thus, for example, a resource with an integer
	///   identifier of 128 be identified by a @a lpszResName of #128.
	/// @param[in] lpszResType
	///   A string identifying the type of the resource.
	///   Applications can define their own resource types and identify them by name in the .rc file.
	///   In addition, there are a number of predefined resource types (which reflect Windows resource types).
	///   To identify a resource that is one of the predefined resource types, set @a lpszResType to the
	///   appropriate value from the following table: @n@n
	///   <table border="0" cellpadding="1" cellspacing="1" bgcolor="#EEEEEE">
	///     <tr bgcolor="#CCCCCC"><td width=200><b>lpszResType</b></td><td><b>Type of resource</b></td></tr>
	///     <tr> <td>@b RT_ACCELERATOR</td>   <td>Accelerator table</td> </tr>
	///     <tr> <td>@b RT_BITMAP</td>        <td>Bitmap resource</td> </tr>
	///     <tr> <td>@b RT_DIALOG</td>        <td>Dialog box</td> </tr>
	///     <tr> <td>@b RT_FONT</td>          <td>Font resource</td> </tr>
	///     <tr> <td>@b RT_FONTDIR</td>       <td>Font directory resource</td> </tr>
	///     <tr> <td>@b RT_MENU</td>          <td>Menu resource</td> </tr>
	///     <tr> <td>@b RT_RCDATA</td>        <td>Application-defined resource (raw data)</td> </tr>
	///     <tr> <td>@b RT_STRING</td>        <td>String-table entry</td> </tr>
	///     <tr> <td>@b RT_MESSAGETABLE</td>  <td>Message-table entry</td> </tr>
	///     <tr> <td>@b RT_CURSOR</td>        <td>Hardware-dependent cursor resource</td> </tr>
	///     <tr> <td>@b RT_GROUP_CURSOR</td>  <td>Hardware-independent cursor resource</td> </tr>
	///     <tr> <td>@b RT_ICON</td>          <td>Hardware-dependent icon resource</td> </tr>
	///     <tr> <td>@b RT_GROUP_ICON</td>    <td>Hardware-independent icon resource</td> </tr>
	///     <tr> <td>@b RT_VERSION</td>       <td>Version resource</td> </tr>
	///   </table>
	/// @remarks
	///   Specifying resources by ID requires less memory than specifying resources by name.
	CResourceStream(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType);

	/// Construct the object associated with a particular resource ID and type.
	/// The constructor finds the resource data and loads it into the memory buffer.
	///
	/// @param[in] hInstance
	///   The instance handle associated with the executable or shared library that contains the resource.
	/// @param[in] nResId
	///   The integer ID associated with the resource in the .rc file that was compiled with the application.
	/// @param[in] lpszResType
	///   A string identifying the type of the resource.
	CResourceStream(HINSTANCE hInstance, int nResId, LPCTSTR lpszResType);

	/// Destructor
	virtual ~CResourceStream();

	/// Overrides the inherited method to throw an exception when an attempt is made to write the resource
	/// back to the application.
	virtual int Write(const void *pBuffer, int nBytes);

	/// Overrides the inherited method to throw an exception when an attempt is changing the resource size.
	virtual void SetSize(INT64 nSize);
};

///////////////////////////////////////////////////////////////////////////////
/// CSeqNumberAlloc - Sequence number allocator class.
///
/// @remarks
///   @li CSeqNumberAlloc is thread-safe.
///   @li The sequence number is zero based.
///   @li You can spcify a start value(>=0), default is 0.

class CSeqNumberAlloc
{
private:
	volatile DWORD m_nCurrentId;
public:
	/// Constructor using specified start value.
	explicit CSeqNumberAlloc(DWORD nStartId = 0);

	/// Allocates a new sequence number and returns it.
	DWORD AllocId() const;
};

///////////////////////////////////////////////////////////////////////////////
/// CPointerList - Pointer list class.

class CPointerList
{
public:
	/// The comparison function prototype that used by Sort().
	typedef int (*LIST_COMPARE_PROC)(PVOID pItem1, PVOID pItem2);
private:
	PVOID *m_pList;
	int m_nCount;
	int m_nCapacity;
protected:
	inline void Init();
	PVOID Get(int nIndex) const;
	void Put(int nIndex, PVOID pItem);
	void SetCount(int nNewCount);
	void QuickSort(PVOID *pList, int l, int r, LIST_COMPARE_PROC pfnCompareProc);
	void ThrowIndexError(int nIndex) const;
protected:
	virtual void Grow();
public:
	/// Default constructor.
	CPointerList();

	/// Copy constructor.
	CPointerList(const CPointerList& src);

	/// Constructor.
	CPointerList(int nCount, ...);

	/// Destructor.
	virtual ~CPointerList();

	/// Inserts a new item at the end of the list.
	///
	/// @param[in] pItem
	///   The item to be added to this list.
	/// @return
	///   The index (0-based) of the new item.
	int Add(PVOID pItem);

	/// Inserts a new item at the middle of the list.
	///
	/// @param[in] nIndex
	///   Specifies the index of the new item in this list. It is 0-based.
	///   If the @a nIndex is equal to the GetCount() before inserting, the call to Insert()
	///   is same as Add().
	/// @param[in] pItem
	///   The item to be inserted to this list.
	void Insert(int nIndex, PVOID pItem);

	/// Removes the item at the position given by the @a nIndex parameter.
	///
	/// @param[in] nIndex
	///   The index (0-based) of the item to be removed.
	/// @remarks
	///   Calling Delete() moves up all items in the list that follow the deleted item.
	void Delete(int nIndex);

	/// Removes a specified item from the list.
	///
	/// @param[in] pItem
	///   The item to be removed.
	/// @return
	///   - The index (0-based) of the item before it was removed, or
	///   - (-1), if the item is not found int this list.
	/// @remarks
	///   Calling Remove() moves up all items in the list that follow the deleted item.
	int Remove(PVOID pItem);

	/// Removes a specified item from the list.
	///
	/// @param[in] pItem
	///   The item to be removed.
	/// @return
	///   - The removed item, or
	///   - NULL, if the item is not found in this list.
	/// @remarks
	///   Calling Extract() moves up all items in the list that follow the deleted item.
	PVOID Extract(PVOID pItem);

	/// Changes the position of an item in the list.
	///
	/// @param[in] nCurIndex
	///   The index of the item to be moved.
	/// @param[in] nNewIndex
	///   The new index of item after moving.
	void Move(int nCurIndex, int nNewIndex);

	/// Resize the list.
	///
	/// @param[in] nCount
	///   The new list size (number of items). Must be greater than or equal to 0.
	/// @remark
	///   Increasing the list size will add the necessary number of NULL pointers to the end of the list.
	///   Decreasing the list size will remove the necessary number of items from the end of the list.
	void Resize(int nCount);

	/// Deletes all items from the list.
	/// Calling Clear() also set the capacity to 0.
	void Clear();

	/// Performs a QuickSort on the list based on a specified comparison function.
	///
	/// @param[in] pfnCompareProc
	///   The comparison function that indicates how the items are to be ordered. See @ref LIST_COMPARE_PROC.
	void Sort(LIST_COMPARE_PROC pfnCompareProc);

	/// Returns the first item in the list.
	/// Throws exception if the list is empty.
	PVOID First() const;

	/// Returns the last item in the list.
	/// Throws exception if the list is empty.
	PVOID Last() const;

	/// Returns the index (0-based) of the first item in the list with a specified value.
	///
	/// @param[in] pItem
	///   The item to be found.
	/// @return
	///   - The index of the first item in the list, or
	///   - (-1), if not found.
	int IndexOf(PVOID pItem) const;

	/// Returns the number of items in the list.
	int GetCount() const { return m_nCount; }

	/// Indicates whether the list is empty or not.
	bool IsEmpty() const { return (GetCount() <= 0); }

	/// Returns the number of items the list can hold without reallocating memory.
	///
	/// @remarks
	///   Do not confuse the GetCapacity() with the GetCount(), which is the number of items
	///   in the list that are in use. The capacity is always greater than or equal to the count.
	///   When the capacity is greater than the count, the unused memory can be reclaimed by
	///   setting the capacity to the count.
	int GetCapacity() const { return m_nCapacity; }

	/// Changes the capacity of the list.
	///
	/// @param[in] nNewCapacity
	///   The new capacity of the list.
	/// @remarks
	///   When an item is added to a list that is already filled to capacity, the capacity of
	///   the list is automatically increased. Setting the list capacity before adding items can
	///   reduce the number of memory reallocations and thereby improve performance.
	void SetCapacity(int nNewCapacity);

	/// Assignment operator.
	CPointerList& operator = (const CPointerList& rhs);

	/// Gets the item at the specified index (0-based).
	const PVOID& operator[] (int nIndex) const;

	/// Sets or gets the item at the specified index (0-based).
	PVOID& operator[] (int nIndex);
};

///////////////////////////////////////////////////////////////////////////////
/// CStrings - The abstract base class for string list.
///
/// CStrings is an abstract class, so it should not be directly instantiated.
/// Derived classes from CStrings can store and manipulate a list of strings.

class CStrings
{
private:
	enum STRINGS_DEFINED
	{
		SD_DELIMITER         = 0x0001,
		SD_QUOTE_CHAR        = 0x0002,
		SD_NAME_VALUE_SEP    = 0x0004,
		SD_LINE_BREAK        = 0x0008
	};

public:
	/// Calls BeginUpdate() and EndUpdate() automatically in a scope.
	class CAutoUpdater
	{
	private:
		CStrings& m_Strings;
	public:
		/// Constructor. The constructor calls BeginUpdate() automatically.
		CAutoUpdater(CStrings& Strings) : m_Strings(Strings)
			{ m_Strings.BeginUpdate(); }
		/// Destructor. The destructor calls EndUpdate() automatically.
		~CAutoUpdater()
			{ m_Strings.EndUpdate(); }
	};

protected:
	DWORD m_nDefined;
	TCHAR m_chDelimiter;
	CString m_strLineBreak;
	TCHAR m_chQuoteChar;
	TCHAR m_chNameValueSeparator;
	int m_nUpdateCount;
private:
	void Assign(const CStrings& src);
protected:
	void Init();
	void Error(LPCTSTR lpszMsg, int nData) const;
	int GetUpdateCount() const { return m_nUpdateCount; }
	CString ExtractName(LPCTSTR lpszStr) const;
protected:
	/// Performs internal adjustments before or after a series of updates.
	///
	/// SetUpdateState is called at the beginning or end of a series of updates. When the BeginUpdate method
	/// is first called and the CStrings object is not already in the middle of an update, CStrings calls
	/// SetUpdateState internally, with @a bUpdating set to true. When the EndUpdate method is called and it
	/// cancels out the last unmatched call to BeginUpdate, CStrings calls SetUpdateState internally, with
	/// @a bUpdating set to false. @n
	/// As implemented in CStrings, SetUpdateState does nothing. Descendant classes can override this method
	/// to optimize the response to updates.
	virtual void SetUpdateState(bool bUpdating) {}

	/// Compares two strings.
	///
	/// CStrings uses CompareStrings internally to compare the values of strings that appear in the list.
	/// @return
	///   - Less than 0, if @a str1 < @a str2.
	///   - 0, if @a str1 = @a str2.
	///   - Greater than 0, if @a str1 > @a str2.
	/// @remarks
	///   As implemented in CStrings, CompareStrings uses the Windows API CompareString function, which
	///   compares strings case insensitively. Some descendant classes override this method to change
	///   the way strings are compared (for example, to introduce case sensitivity).
	virtual int CompareStrings(LPCTSTR str1, LPCTSTR str2) const;
public:
	/// Default constructor.
	CStrings();

	/// Destructor.
	virtual ~CStrings() {}

	/// Adds a string at the end of the list.
	///
	/// @param[in] lpszStr
	///   The string to be added.
	/// @return
	///   The index (0-based) of the new string.
	virtual int Add(LPCTSTR lpszStr);

	/// Adds a string at the end of the list, and associates a data pointer with the string.
	///
	/// @param[in] lpszStr
	///   The string to be added.
	/// @param[in] pData
	///   The data pointer to be associated with the @a lpszStr.
	/// @return
	///   The index (0-based) of the new string.
	virtual int Add(LPCTSTR lpszStr, PVOID pData);

	/// Adds a group of strings to the list.
	///
	/// @param[in] Strings
	///   The string list to be added.
	virtual void AddStrings(const CStrings& Strings);

	/// Introduces an abstract method to insert a string at a specified position.
	///
	/// @param[in] nIndex
	///   Specifies the index of the new string in this list. It is 0-based.
	///   If the @a nIndex is equal to the GetCount() before inserting, the call to Insert()
	///   is same as Add().
	/// @param[in] lpszStr
	///   The string to be inserted to this list.
	virtual void Insert(int nIndex, LPCTSTR lpszStr) = 0;

	/// Inserts a string into the list at the specified position, and associates it with a data pointer.
	///
	/// @param[in] nIndex
	///   Specifies the index of the new string in this list. It is 0-based.
	/// @param[in] lpszStr
	///   The string to be inserted to this list.
	/// @param[in] pData
	///   The data pointer to be associated with @a lpszStr.
	virtual void Insert(int nIndex, LPCTSTR lpszStr, PVOID pData);

	/// Introduces an abstract method to empty the list.
	virtual void Clear() = 0;

	/// Introduces an abstract method to delete a string at the specified position from the list.
	///
	/// @param[in] nIndex
	///   The index (0-based) of the string to be deleted.
	virtual void Delete(int nIndex) = 0;

	/// Compares the list of strings to the list from another CStrings object and returns true if the
	/// two lists match.
	///
	/// @param[in] Strings
	///   Another string list to be compare.
	/// @return
	///   Returns true if the lists for both objects have the same number of strings and the strings
	///   in each list match; false otherwise.
	/// @remarks
	///   - This function compares only the strings, not any associated data pointers.
	///   - The comparison is always case-sensitive.
	virtual bool Equals(const CStrings& Strings);

	/// Swaps the position of two strings in the list.
	///
	/// @param[in] nIndex1
	///   The index (0-based) of the first string.
	/// @param[in] nIndex2
	///   The index (0-based) of the second string.
	/// @remarks
	///   If either string has a associated data pointer, Exchange changes the position of the data
	///   pointer as well.
	virtual void Exchange(int nIndex1, int nIndex2);

	/// Moves the string at position @a nCurIndex so that it occupies the position @a nNewIndex.
	///
	/// @param[in] nCurIndex
	///   The index of the string to be moved.
	/// @param[in] nNewIndex
	///   The new index of string after moving. It can be equal to GetCount().
	/// @remarks
	///   If the string has an associated data pointer, the data pointer remains assocated with the
	///   string in its new position.
	virtual void Move(int nCurIndex, int nNewIndex);

	/// Determines whether the specified string exists in the list.
	///
	/// @param[in] lpszStr
	///   The string to be found.
	/// @return
	///   Returns true if exists; false otherwise.
	virtual bool Exists(LPCTSTR lpszStr) const;

	/// Returns the index (0-based) of a string in the list.
	///
	/// @param[in] lpszStr
	///   The string to be found.
	/// @return
	///   - The index of the first occurrence in the list, or
	///   - (-1), if not found.
	/// @remarks
	///   Call IndexOf to obtain the position of the first occurrence of the string @a lpszStr, or of
	///   a string that differs from @a lpszStr only by case. IndexOf returns the 0-based index of the
	///   string. If the string appears in the list more than once, IndexOf returns the position of
	///   the first occurrence.
	virtual int IndexOf(LPCTSTR lpszStr) const;

	/// Returns the position of the first name-value pair with the specified name.
	///
	/// @param[in] lpszName
	///   The name to be found.
	/// @return
	///   - The index of the first occurrence in the list, or
	///   - (-1), if not found.
	/// @remarks
	///   Call IndexOfName to locate the first occurrence of a name-value pair where the name part is
	///   equal to the @a lpszName parameter or differs only in case. IndexOfName returns the 0-based
	///   index of the string. If there are more than one name-value pair with a name portion matching
	///   the @a lpszName parameter, IndexOfName returns the position of the first such string.
	virtual int IndexOfName(LPCTSTR lpszName) const;

	/// Returns the index of first string in the list associated with a given data pointer.
	///
	/// @param[in] pData
	///   The data pointer to be found.
	/// @return
	///   - The index of the first occurrence in the list, or
	///   - (-1), if not found.
	virtual int IndexOfData(PVOID pData) const;

	/// Fills the list with lines of text read from a stream.
	///
	/// @param[in] Stream
	///   The stream to be read from.
	/// @param[in] bAsUnicode
	///   Determines whether the streamed data is read as unicode or not.
	/// @remarks
	///   The text read from stream is parsed into strings separated by carriage return or linefeed characters.
	virtual bool LoadFromStream(CStream& Stream, bool bAsUnicode);

	/// Fills the list with lines of text in a specified file.
	///
	/// @param[in] lpszFileName
	///   The name of the file to be read from.
	/// @param[in] bAsUnicode
	///   Determines whether the data in file is read as unicode or not.
	/// @remarks
	///   The text read from file is parsed into strings separated by carriage return or linefeed characters.
	virtual bool LoadFromFile(LPCTSTR lpszFileName, bool bAsUnicode);

	/// Writes the value of the GetText() to a stream object.
	///
	/// @param[in] Stream
	///   The stream to be written.
	/// @param[in] bAsUnicode
	///   Determines whether the text is written as unicode or not.
	/// @remarks
	///   This function writes the strings delimited by carriage return, line feed pairs.
	virtual bool SaveToStream(CStream& Stream, bool bAsUnicode) const;

	/// Writes the value of the GetText() to a file.
	///
	/// @param[in] lpszFileName
	///   The name of the file to be written.
	/// @param[in] bAsUnicode
	///   Determines whether the text is written as unicode or not.
	/// @remarks
	///   Each string in the list is written to a separate line in the file.
	virtual bool SaveToFile(LPCTSTR lpszFileName, bool bAsUnicode) const;

	/// Returns the number of strings the list can hold.
	///
	/// @remarks
	///   Call GetCapacity() to determine the currently allocated size of the string list. For the CStrings
	///   class, Calling GetCapacity returns the value of GetCount(), and setting capacity by calling
	///   SetCapacity() does nothing. Descentdants of CStrings can override this method to allow a string list
	///   to allocate memory for entries that have not been added to the list.
	virtual int GetCapacity() const { return GetCount(); }

	/// Changes the capacity of the list.
	///
	/// @param[in] nValue
	///   The new capacity of the list.
	/// @remarks
	///   For CStrings class, calling SetCapacity does nothing.
	virtual void SetCapacity(int nValue) {}

	/// Introduces an abstract method to represent the number of  strings in the list.
	/// Descendants of CStrings can implement the GetCount method to indicate the number of string in the list.
	virtual int GetCount() const = 0;

	/// Indicates whether the list is empty or not.
	bool IsEmpty() const { return (GetCount() <= 0); }

	/// Returns the delimiter character used by the GetDelimitedText() method.
	/// The default delimiter character is @ref DEFAULT_DELIMITER.
	TCHAR GetDelimiter() const;

	/// Specifies the delimiter character used by the GetDelimitedText() method.
	void SetDelimiter(TCHAR chValue);

	/// Returns the line break string used by the GetText() method.
	/// The default line break string is @ref DEFAULT_LINE_BREAK.
	CString GetLineBreak() const;

	/// Specifies the line break string used by the GetText() method.
	void SetLineBreak(LPCTSTR lpszValue);

	/// Returns the quote character used by the GetDelimitedText() method.
	/// The default quote character is @ref DEFAULT_QUOTE_CHAR.
	TCHAR GetQuoteChar() const;

	/// Specifies the quote character used by the GetDelimitedText() method.
	void SetQuoteChar(TCHAR chValue);

	/// Returns the name-value separator.
	/// The default name-value separator is @ref DEFAULT_NAME_VALUE_SEP.
	TCHAR GetNameValueSeparator() const;

	/// Specifies the name-value separator.
	void SetNameValueSeparator(TCHAR chValue);

	/// Returns a string by combining @a lpszName and @a lpszValue with the name-value separator.
	/// @remarks
	///   - The name-value separator can be get by calling GetNameValueSeparator().
	///   - If the string @a lpszName contains the name-value separator, an exception is thrown.
	CString CombineNameValue(LPCTSTR lpszName, LPCTSTR lpszValue) const;

	/// Returns the name part of the string at @a nIndex if the string is a name-value pair.
	/// @remarks
	///   If the string at the position specified by @a nIndex is not a name-value pair, this function
	///   returns an empty string.
	CString GetName(int nIndex) const;

	/// Returns the value part of a string associated with a given name if the string is a name-value pair.
	/// @remarks
	///   If the string associated with the @a lpszName is not a name-value pair, this function
	///   returns an empty string.
	CString GetValue(LPCTSTR lpszName) const;

	/// Returns the value part of the string at @a nIndex if the string is a name-value pair.
	/// @remarks
	///   If the string at the position specified by @a nIndex is not a name-value pair, this function
	///   returns an empty string.
	CString GetValue(int nIndex) const;

	/// Sets the value part of a string associated with a given name.
	///
	/// @param[in] lpszName
	///   The name part of a string.
	/// @param[in] lpszValue
	///   The value part of a string.
	/// @remarks
	///   - If the string that has the name part @a lpszName is not found in the list, an new string
	///     will be added at the end of the list automatically. The new string is a name-value pair
	///     combined with @a lpszName and @a lpszValue.
	///   - If the specified value part @a lpszValue is empty, the string associated with the @a lpszName
	///     in the list will be removed automatically.
	void SetValue(LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Sets the value part of a string at @a nIndex.
	///
	/// @param[in] nIndex
	///   The 0-based index.
	/// @param[in] lpszValue
	///   The value part of a string.
	/// @remarks
	///   - If @a nIndex is less than 0, a new string will be added at the end of the list automatically.
	///     The new string is a name-value pair combined with an empty name and @a lpszValue.
	///   - If the specified value part @a lpszValue is empty, the string at @a nIndex will be removed
	///     automatically.
	void SetValue(int nIndex, LPCTSTR lpszValue);

	/// Returns the data pointer associated with the string at the specified position.
	/// @remarks
	///   For CStrings, calling GetData returns NULL. Descendant classes can return the data pointer
	///   associated with the string at @a nIndex by overriding the GetData method.
	virtual PVOID GetData(int nIndex) const { return NULL; }

	/// Sets the data pointer associated with the string at the specified position.
	/// @remarks
	///   For CStrings, calling SetData does nothing. Descendant classes can associate the data pointer
	///   with the string at @a nIndex by overriding the SetData method.
	virtual void SetData(int nIndex, PVOID pData) {}

	/// Gets all the strings in the CStrings object in a single string delimited by line break.
	virtual CString GetText() const;

	/// Sets all the strings in the CStrings object in a single string delimited by line break.
	virtual void SetText(LPCTSTR lpszValue);

	/// Gets all the strings in the CStrings object in a single comma-delimited string.
	///
	/// Any string in the list that include spaces, commas or quotes will be contained in double quotes,
	/// and any double quotes in a string will be repeated. @n
	/// For example, if the list contains the following strings:
	/// <pre>
	///   ab,c
	///   d ef
	///   12"3
	///   xyz
	/// </pre>
	/// GetCommaText will return:
	/// <pre>
	///   "ab,c","d ef","12""3",xyz
	/// </pre>
	/// @note
	///   GetCommaText() is the same as the GetDelimitedText() with a delimiter of '<tt>,</tt>'
	///   and a quote character of '<tt>"</tt>'.
	CString GetCommaText() const;

	/// Sets all the strings in the CStrings object in a single comma-delimited string.
	///
	/// When calling SetCommaText, the value is parsed as a comma-delimited string. Strings are separated by
	/// commas or spaces, and optionally enclosed in double quotes. Double quote marks that are part of the
	/// string are repeated to distinguish them from the quotes that surround the string. Spaces and commas
	/// that are not contained within double quote marks are delimiters. Two commas next to each other will
	/// indicate an empty string, but spaces that appear next to another delimiter are ignored. @n
	/// For example, call SetCommaText with the string:
	/// <pre>
	///   "ab,c","d ef","12""3",xyz
	/// </pre>
	/// The list will then contain:
	/// <pre>
	///   ab,c
	///   d ef
	///   12"3
	///   xyz
	/// </pre>
	/// @note
	///   SetCommaText() is the same as the SetDelimitedText() with a delimiter of '<tt>,</tt>'
	///   and a quote character of '<tt>"</tt>'.
	/// @remarks
	///   Including a trailing comma in the source string causes a blank item to be included in the
	///   string list.
	void SetCommaText(LPCTSTR lpszValue);

	/// Gets all the strings in the CStrings object in a single delimited string.
	///
	/// When calling GetDelimitedText, the resulting value delimits individual strings in two ways:
	/// each string is surrounded (before and after) by the quote character specified by the GetQuoteChar()
	/// method. In addition, individual strings are separated by the character specified by the
	/// GetDelimiter() method.
	///
	/// @note
	///   GetCommaText() is the same as the GetDelimitedText() with a delimiter of '<tt>,</tt>'
	///   and a quote character of '<tt>"</tt>'.
	CString GetDelimitedText() const;

	/// Sets all the strings in the CStrings object in a single delimited string.
	///
	/// When calling SetDelimitedText, individual strings must be separated using the quote character
	/// specified by GetQuoteChar() at both ends, using the character specified by GetDelimiter() as a
	/// separator, or using both these methods.
	///
	/// @note
	///   SetCommaText() is the same as the SetDelimitedText() with a delimiter of '<tt>,</tt>'
	///   and a quote character of '<tt>"</tt>'.
	void SetDelimitedText(LPCTSTR lpszValue);

	/// Introduces an abstract method to return the string in the list at a specified position.
	///
	/// @param[in] nIndex
	///   The 0-based position of the string to be retrieved.
	/// @remarks
	///   Descendants of CStrings must implement an accessor function for GetString to return the string
	///   at the position indicated by @a nIndex.
	virtual const CString& GetString(int nIndex) const = 0;

	/// Sets the string in the list at a specified position.
	///
	/// @param[in] nIndex
	///   The 0-based position of the string to be set.
	/// @param[in] lpszValue
	///   The string to be set.
	/// @remarks
	///   To store a string as a name-value pair, call SetString with a value that includes the character
	///   specified by GetNameValueSeparator(). The name and value will then be accessible separately using
	///   the GetName() and GetValue() method.
	virtual void SetString(int nIndex, LPCTSTR lpszValue);

	/// Enables the CStrings object to track when the list of strings is changing.
	/// @remarks
	///   Use CStrings::CAutoUpdater object to call BeginUpdate and EndUpdate automatically in a scope.
	void BeginUpdate();

	/// Enables the CStrings object to keep track of when the list of strings has finished changing.
	/// @remarks
	///   Use CStrings::CAutoUpdater object to call BeginUpdate and EndUpdate automatically in a scope.
	void EndUpdate();

	/// Converts the contents from CStrings to MFC CStringArray.
	void ToMfcObject(CStringArray& StringArray);
	/// Converts the contents from CStrings to MFC CStringList.
	void ToMfcObject(CStringList& StringList);
	/// Converts the contents from MFC CStringArray to CStrings.
	void FromMfcObject(CStringArray& StringArray);
	/// Converts the contents from MFC CStringList to CStrings.
	void FromMfcObject(CStringList& StringList);

	/// Assignment operator.
	CStrings& operator = (const CStrings& rhs);

	/// Get the string at @a nIndex(0-based).
	const CString& operator[] (int nIndex) const { return GetString(nIndex); }
};

///////////////////////////////////////////////////////////////////////////////
/// CStrList - String list class.

class CStrList : public CStrings
{
public:
	friend int StringListCompareProc(const CStrList& StringList, int nIndex1, int nIndex2);

public:
	/// The comparison function prototype that used by Sort().
	typedef int (*STRINGLIST_COMPARE_PROC)(const CStrList& StringList, int nIndex1, int nIndex2);

	/// Indicates the response when an application attempts to add a duplicate entry to a list.
	enum DUPLICATE_MODE
	{
		DM_IGNORE,      ///< Ignore attempts to add duplicate entries (do not add the duplicate).
		DM_ACCEPT,      ///< Allow the list to contain duplicate entries (add the duplicate).
		DM_ERROR        ///< Throw an exception when the application tries to add a duplicate.
	};

private:
	struct CStringItem
	{
		CString *pStr;
		PVOID pData;
	};

private:
	CStringItem *m_pList;
	int m_nCount;
	int m_nCapacity;
	DUPLICATE_MODE m_nDupMode;
	bool m_bSorted;
	bool m_bCaseSensitive;
private:
	void Init();
	void Assign(const CStrList& src);
	void InternalClear();
	CString& StringObjectNeeded(int nIndex) const;
	void ExchangeItems(int nIndex1, int nIndex2);
	void Grow();
	void QuickSort(int l, int r, STRINGLIST_COMPARE_PROC pfnCompareProc);
protected: // override
	virtual void SetUpdateState(bool bUpdating);
	virtual int CompareStrings(LPCTSTR str1, LPCTSTR str2) const;
protected: // virtual
	/// Occurs immediately before the list of strings changes.
	virtual void OnChanging() {}
	/// Occurs immediately after the list of strings changes.
	virtual void OnChanged() {}
	/// Internal method used to insert a string to the list.
	virtual void InsertItem(int nIndex, LPCTSTR lpszStr, PVOID pData);
public:
	CStrList();
	CStrList(const CStrList& src);
	virtual ~CStrList();

	/// Adds a new string to the list.
	///
	/// If the list is sorted (see GetSorted()), @a lpszStr is added to the appropriate position in the sort
	/// order. If the list is not sorted, @a lpszStr is added to the end of the list.
	///
	/// @param[in] lpszStr
	///   The string to be added.
	/// @return
	///   The index (0-based) of the new string.
	/// @remarks
	///   For sorted list, Add() will throw an exception if the string @a lpszStr already appears in the list
	///   and the duplicate mode is set to @ref DM_ERROR by SetDupMode(). If the duplicate mode is set to
	///   @ref DM_IGNORE, trying to add a duplicate string causes Add() to return the index of the existing
	///   entry.
	virtual int Add(LPCTSTR lpszStr);

	/// Adds a string to the list, and associates a data pointer with the string.
	///
	/// @param[in] lpszStr
	///   The string to be added.
	/// @param[in] pData
	///   The data pointer to be associated with the @a lpszStr.
	/// @return
	///   The index (0-based) of the new string.
	virtual int Add(LPCTSTR lpszStr, PVOID pData);

	/// Deletes all the strings from the list.
	virtual void Clear();

	/// Deletes the string specified by the @a nIndex parameter.
	virtual void Delete(int nIndex);

	/// Swaps the position of two strings in the list.
	///
	/// @param[in] nIndex1
	///   The index (0-based) of the first string.
	/// @param[in] nIndex2
	///   The index (0-based) of the second string.
	/// @remarks
	///   Do not call Exchange on a sorted list except to swap two identical strings. Exchange does not check
	///   whether the list is sorted, and can destroy the sort order of a sorted list.
	virtual void Exchange(int nIndex1, int nIndex2);

	/// Returns the index (0-based) of a string in the list.
	///
	/// @param[in] lpszStr
	///   The string to be found.
	/// @return
	///   - The index of the first occurrence in the list, or
	///   - (-1), if not found.
	/// @remarks
	///   Call IndexOf to obtain the position of the first occurrence of a string that matches @a lpszStr.
	///   A string matches @a lpszStr if it is identical to @a lpszStr or, if GetCaseSensitive() is false,
	///   if it differs only in case.
	virtual int IndexOf(LPCTSTR lpszStr) const;

	/// Inserts a string to the list at the position specified by @a nIndex.
	///
	/// @param[in] nIndex
	///   Specifies the index of the new string in this list. It is 0-based.
	///   If the @a nIndex is equal to the GetCount() before inserting, the call to Insert()
	///   is same as Add().
	/// @param[in] lpszStr
	///   The string to be inserted to this list.
	/// @remarks
	///   If the list is sorted, Calling Insert method will throw an exception. Use Add() with sorted list.
	virtual void Insert(int nIndex, LPCTSTR lpszStr);

	/// Inserts a string into the list at the specified position, and associates it with a data pointer.
	///
	/// @param[in] nIndex
	///   Specifies the index of the new string in this list. It is 0-based.
	/// @param[in] lpszStr
	///   The string to be inserted to this list.
	/// @param[in] pData
	///   The data pointer to be associated with @a lpszStr.
	/// @remarks
	///   If the list is sorted, Calling Insert method will throw an exception. Use Add() with sorted list.
	virtual void Insert(int nIndex, LPCTSTR lpszStr, PVOID pData);

	/// Returns the number of strings the string list has allocated memory to hold.
	///
	/// @remarks
	///   Use GetCapacity to find out how much memory is available for holding strings, or to reallocate
	///   the memory to allow more or fewer strings. @n
	///   The capacity is the number of entries in the string list that have been allocated, as opposed to
	///   the count (see GetCount()), which is the number of strings in the list. Thus, the capacity is
	///   always greater than or equal to the count.
	virtual int GetCapacity() const { return m_nCapacity; }

	/// Changes the capacity of the list.
	///
	/// @param[in] nValue
	///   The new capacity of the list.
	/// @remarks
	///   Adding new strings will cause the capacity to increase if necessary, but setting the capacity
	///   will not change the count. Do not set capacity to a value less than count, or the list will be
	///   truncated and count will indicate that the list contains more strings than is the case. Also,
	///   if capacity is set to a value less than count, the memory used to store the strings that
	///   are dropped from the list (as opposed to the entries in the list that point to those strings)
	///   is not freed.
	virtual void SetCapacity(int nValue);

	/// Indicates the number of strings in the list.
	virtual int GetCount() const { return m_nCount; }

	/// Returns the data pointer associated with the string at the specified position.
	virtual PVOID GetData(int nIndex) const;

	/// Sets the data pointer associated with the string at the specified position.
	virtual void SetData(int nIndex, PVOID pData);

	/// Returns the string in the list at a specified position.
	virtual const CString& GetString(int nIndex) const;

	/// Sets the string in the list at a specified position.
	///
	/// @param[in] nIndex
	///   The 0-based position of the string to be set.
	/// @param[in] lpszValue
	///   The string to be set.
	/// @remarks
	///   To store a string as a name-value pair, call SetString with a value that includes the character
	///   specified by GetNameValueSeparator(). The name and value will then be accessible separately
	///   using the GetName() and GetValue() method.
	virtual void SetString(int nIndex, LPCTSTR lpszValue);

	/// Locates the index for a string in the list.
	///
	/// @param[in] lpszStr
	///   The string to be found.
	/// @param[out] nIndex
	///   If the string @a lpszStr is found in the list, the @a nIndex stores the index (0-based) of
	///   the string.
	/// @return
	///   Returns true if the string is found in the list; false otherwise.
	/// @note
	///   You can use Find on a sorted or unsorted list.
	virtual bool Find(LPCTSTR lpszStr, int& nIndex) const;

	/// Sorts the strings in the list in ascending order.
	/// @remarks
	///   - This method does not affect the sorted flag. To change the sorted flag, use SetSorted().
	///   - A sorted list is sorted automatically. For a sorted list, the call to GetSorted() returns true.
	///     It is not necessary to call Sort method to sort the list which sorted flag set to true.
	virtual void Sort();

	/// Sorts the strings in the list with specified comparison function.
	virtual void Sort(STRINGLIST_COMPARE_PROC pfnCompareProc);

	/// Returns the duplicate mode of the list.
	DUPLICATE_MODE GetDupMode() const { return m_nDupMode; }

	/// Sets the duplicate mode of the list.
	/// Set duplicate mode to specify what should happen when an attempt is made to add a duplicate string
	/// to a sorted list. See @ref DUPLICATE_MODE.
	void SetDupMode(DUPLICATE_MODE nValue) { m_nDupMode = nValue; }

	/// Returns whether the strings in the list should be automatically sorted.
	bool GetSorted() const { return m_bSorted; }

	/// Specifies whether the strings in the list should be automatically sorted.
	/// Set sorted flag to true to cause the strings in the list to be automatically sorted in ascending
	/// order. Set sorted flag to false to allow strings to remain where they are inserted. When sorted
	/// flag is false, the strings in the list can be put in ascending order at any time by calling the
	/// Sort() method.
	virtual void SetSorted(bool bValue);

	/// Returns whether strings in the list should be compared in case-sensitive manner or not.
	bool GetCaseSensitive() const { return m_bCaseSensitive; }

	/// Specifies whether strings in the list should be compared in case-sensitive manner or not.
	virtual void SetCaseSensitive(bool bValue);

	/// Assignment operator
	CStrList& operator = (const CStrList& rhs);
};

///////////////////////////////////////////////////////////////////////////////
/// CHashedStrList - Hashed string list class.
///
/// CHashedStrList maintains a list of strings using an internal hash table.
/// By using CHashedStrList instead of CStrList, you can improve performance when the list
/// contains a large number of strings.

class CHashedStrList : public CStrList
{
private:
	typedef hash_map<CString, int> STRLIST_HASH;
private:
	STRLIST_HASH m_ItemHash;
	STRLIST_HASH m_NameHash;
	bool m_bItemHashValid;
	bool m_bNameHashValid;
private:
	void Init();
	void Assign(const CHashedStrList& src);
	void SetHashInvalid();
	void UpdateItemHash();
	void UpdateNameHash();
protected:
	virtual void OnChanged();
public:
	/// Constructor
	CHashedStrList();
	/// Copy constructor.
	CHashedStrList(const CHashedStrList& src);
	/// Destructor.
	virtual ~CHashedStrList() {}

	/// Returns the position of a string in the list.
	virtual int IndexOf(LPCTSTR lpszStr) const;

	/// Returns the position of the first name-value pair with the specified name.
	virtual int IndexOfName(LPCTSTR lpszName) const;

	/// Specifies whether strings in the list should be compared in case-sensitive manner or not.
	virtual void SetCaseSensitive(bool bValue);

	/// Assignment operator
	CHashedStrList& operator = (const CHashedStrList& rhs);
};

///////////////////////////////////////////////////////////////////////////////
/// CNameValueList - Name-Value pair list class.
///
/// @remarks
///   - This class maintains a list of items, an item consists of name part and value part.
///   - The names must be unique in the list, and the name cannot contain the equal mark ("=").
///     The value part can be any string.
///   - This class has no hash table or tree mechanism.

class CNameValueList
{
public:
	/// The item structure.
	struct CNameValueItem
	{
	public:
		CString strName;        ///< The name part of the item.
		CString strValue;       ///< The value part of the item.
	public:
		CNameValueItem(const CNameValueItem& src) :
			strName(src.strName), strValue(src.strValue) {}
		CNameValueItem(const CString& strNameA, const CString& strValueA) :
			strName(strNameA), strValue(strValueA) {}
	};

protected:
	CPointerList m_Items;       // (CNameValueItem* [])
	bool m_bCaseSensitive;
private:
	void Init();
	void Assign(const CNameValueList& src);
protected:
	inline CNameValueItem* ItemPtr(int nIndex) const
		{ return (CNameValueItem*)m_Items[nIndex]; }
	void CheckNameDuplicated();
public:
	/// Default Constructor.
	CNameValueList();
	/// Copy constructor.
	CNameValueList(const CNameValueList& src);
	/// Destructor.
	virtual ~CNameValueList();

	/// Adds an item to the list.
	///
	/// @param[in] lpszName
	///   The name part of the item to be added.
	/// @param[in] lpszValue
	///   The value part of the item to be added.
	/// @return
	///   Returns true if the name @a lpszName is not found in the list before adding; false otherwise.
	/// @remarks
	///   If the name @a lpszName already exists in the list, calling Add overwrites the value part specified
	///   by @a lpszValue.
	bool Add(LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Inserts an item to the list at the specified position.
	///
	/// @param[in] nIndex
	///   Specifies the 0-based index of the new item in the list.
	/// @param[in] lpszName
	///   The name part of the item to be inserted.
	/// @param[in] lpszValue
	///   The value part of the item to be inserted.
	/// @return
	///   Returns true if the name @a lpszName is not found in the list before inserting; false otherwise.
	/// @remarks
	///   If the name @a lpszName already exists in the list, calling Insert overwrites the value part
	///   specified by @a lpszValue.
	bool Insert(int nIndex, LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Deletes an item from the list at the specified position.
	///
	/// @param[in] nIndex
	///   The index (0-based) of the item to be deleted.
	void Delete(int nIndex);

	/// Removes a specified item from the list.
	///
	/// @param[in] lpszName
	///   The name part of the item to be deleted.
	/// @return
	///   Returns true if successful; false otherwise.
	bool Remove(LPCTSTR lpszName);

	/// Deletes all items from the list.
	void Clear();

	/// Returns the index (0-based) of the item in the list with a specified name.
	///
	/// @param[in] lpszName
	///   The name part of the item to be found.
	/// @return
	///   - The index of the item in the list, or
	///   - (-1), if not found.
	int IndexOf(LPCTSTR lpszName) const;

	/// Indicates whether the item with the specified name exists in the list.
	bool NameExists(LPCTSTR lpszName) const;

	/// Looks up the value associated with the given name.
	///
	/// @param[in] lpszName
	///   The name part of the item to be searched.
	/// @param[out] strValue
	///   If the item is found, the parameter stores the value part of the item.
	/// @return
	///   Returns true if found; false otherwise.
	bool GetValue(LPCTSTR lpszName, CString& strValue) const;

	/// Modifies the value associated with the given name.
	///
	/// @param[in] lpszName
	///   The name part of the item to be modified.
	/// @param[in] lpszValue
	///   The value part of the item to be modified.
	/// @return
	///   Returns true if found; false otherwise.
	bool SetValue(LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Returns an item including name and value at the specified position.
	///
	/// @param[in] nIndex
	///   The 0-based index of the item to be retrieved.
	/// @param[out] strName
	///   Stores the name part of the retrieved item.
	/// @param[out] strValue
	///   Stores the value part of the retrieved item.
	void GetItems(int nIndex, CString& strName, CString& strValue) const;

	/// Returns the number of items in the list.
	int GetCount() const { return m_Items.GetCount(); }

	/// Indicates whether the list is empty or not.
	bool IsEmpty() const { return (GetCount() <= 0); }

	/// Gets all the items in the CNameValueList object in a single string delimited by line break.
	///
	/// For example:
	/// <pre>
	///   [<abc,123>, <def,456>]        ->      abc=123
	///                                         def=456
	///
	///   [<abc,12=3>, <def,45 6>]      ->      abc=12=3
	/// </pre>
	CString GetText() const;

	/// Gets all the items in the CNameValueList object in a single comma-delimited string.
	///
	/// For example:
	/// <pre>
	///   [<abc,123>, <def,456>]        ->      abc=123,def=456
	///   [<abc,123>, <,456>]           ->      abc=123,=456
	///   [<abc,123>, <",456>]          ->      abc=123,"""=456"
	///   [<abc,123>, <',456>]          ->      abc=123,'=456
	///   [<abc,123>, <def,">]          ->      abc=123,"def="""
	///   [<abc,123>, < def,456>]       ->      abc=123," def=456"
	///   [<abc,123>, <def,=>]          ->      abc=123,def==
	///   [<abc,123>, <=,456>]          ->      Throw an exception. (The name contains an equal mark)
	/// </pre>
	CString GetCommaText() const;

	/// Sets all the items in the CNameValueList object in a single comma-delimited string.
	void SetCommaText(LPCTSTR lpszText);

	/// Returns whether strings in the list should be compared in case-sensitive manner or not.
	bool GetCaseSensitive() const { return m_bCaseSensitive; }

	/// Specifies whether strings in the list should be compared in case-sensitive manner or not.
	void SetCaseSensitive(bool bValue);

	/// Assignment operator
	CNameValueList& operator = (const CNameValueList& rhs);

	/// Returns the value part of an item associated with a given name.
	CString& operator[] (LPCTSTR lpszName);
};

///////////////////////////////////////////////////////////////////////////////
/// CRegistry - Registry class.
///
/// Use CRegistry to encapsulate access to the Windows system registry in an application.
/// The registry is a database that an application can use to store and retrieve configuration
/// information. Configuration information is stored in a hierarchical tree. Each node in the tree
/// is called a key. Every key can contain subkeys and data values that represent part of the
/// configuration information for an application.
///
/// All keys that an application creates, opens, reads, or writes are subkeys of predefined
/// root keys. By default, a CRegistry object is created with a root key of HKEY_CURRENT_USER.
///
/// Only one key is accessible at a time in a CRegistry object. To determine the key that is
/// currently accessible, read the value of the GetCurrentKey method. CRegistry methods enable an
/// application to open, close, save, move, copy, and delete keys.
///
/// One or more data values containing actual configuration information can be stored in a key.
/// CRegistry methods enable an application to query a key to see if it contains data, to read data
/// in a key, and to write data to a key.

class CRegistry
{
public:
	/// Registry data type.
	enum REG_DATA_TYPE
	{
		RDT_UNKNOWN         = REG_NONE,         ///< Unknown data type.
		RDT_STRING          = REG_SZ,           ///< String type.
		RDT_EXPAND_STRING   = REG_EXPAND_SZ,    ///< Expand string type.
		RDT_INTEGER         = REG_DWORD,        ///< Integer type.
		RDT_BINARY          = REG_BINARY        ///< Binary type.
	};

	/// Registry key information.
	struct CRegKeyInfo
	{
		DWORD nNumSubKeys;
		DWORD nMaxSubKeyLen;
		DWORD nNumValues;
		DWORD nMaxValueLen;
		DWORD nMaxDataLen;
		FILETIME tFileTime;

		CRegKeyInfo() { Clear(); }
		void Clear() { memset(this, 0, sizeof(*this)); }
	};

	/// Registry data information.
	struct CRegDataInfo
	{
		REG_DATA_TYPE nDataType;
		DWORD nDataSize;

		CRegDataInfo() { Clear(); }
		void Clear() { memset(this, 0, sizeof(*this)); }
	};

private:
	HKEY m_hRootKey;
	HKEY m_hCurrentKey;
	CString m_strCurrentPath;
	bool m_bLazyWrite;
	DWORD m_nAccess;
protected:
	void ChangeKey(HKEY hValue, LPCTSTR lpszPath);
	HKEY GetBaseKey(bool bRelative);
	HKEY GetKey(LPCTSTR lpszKey);
	void SetCurrentKey(HKEY hKey);
	int GetData(LPCTSTR lpszName, LPBYTE pBuffer, DWORD nBufSize, REG_DATA_TYPE& nDataType);
	void PutData(LPCTSTR lpszName, LPBYTE pBuffer, DWORD nBufSize, REG_DATA_TYPE nDataType);
	bool IsRelative(LPCTSTR lpszValue);
	int DataTypeToWinType(REG_DATA_TYPE nType);
	REG_DATA_TYPE WinTypeToDataType(int nType);
	void Error(LPCTSTR lpszMsg, LPCTSTR lpszData = NULL);
public:
	/// Default constructor.
	CRegistry();

	/// Destructor.
	virtual ~CRegistry();

	/// Creates a new key in the registry.
	///
	/// @param[in] lpszKey
	///   The name of the key to create. The key can be an absolute or relative name. An absolute key
	///   begins with a backslash (\) and is a subkey of the root key. A relative key is a subkey of
	///   the current key.
	/// @remarks
	///   On error, an CIfcRegException is thrown.
	void CreateKey(LPCTSTR lpszKey);

	/// Removes a specified key and its associated data from the registry.
	///
	/// @return
	///   Returns true if successful; false otherwise.
	bool DeleteKey(LPCTSTR lpszKey);

	/// Opens a specified key.
	/// Call OpenKey to make a specified key the current key.
	///
	/// @param[in] lpszKey
	///   The name of the key to open. If @a lpszKey is NULL, calling OpenKey opens the root key specified
	///   by GetRootKey().
	/// @param[in] bCanCreate
	///   Specifies whether to create the specified key if it does not exists. If @a bCanCreate is true,
	///   the key is created if necessary.
	/// @return
	///   Returns true if the key is successfully opened or created; false otherwise.
	bool OpenKey(LPCTSTR lpszKey, bool bCanCreate);

	/// Opens a specified key in a read-only manner.
	///
	/// @param[in] lpszKey
	///   The name of the key to open. If @a lpszKey is NULL, calling OpenKeyReadOnly opens the root key
	///   specified by GetRootKey().
	/// @return
	///   Returns true if the key is successfully opened or created; false otherwise.
	/// @remarks
	///   After a successful call to OpenKeyReadOnly the m_nAccess variable of the registry object is
	///   automatically changed to KEY_READ, regardless of what that property was set to prior to the
	///   method call.
	bool OpenKeyReadOnly(LPCTSTR lpszKey);

	/// Writes the current key to the registry and close the key.
	void CloseKey();

	/// Erases a data value that is associated with the current key.
	/// Call DeleteValue to remove a specific data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The string containing the name of the value to delete.
	/// @return
	///   Returns true if the operation is successful; false otherwise.
	bool DeleteValue(LPCTSTR lpszName);

	/// Determines if the current key has subkeys.
	bool HasSubKeys();

	/// Determines if a specified key exists.
	bool KeyExists(LPCTSTR lpszKey);

	/// Determines if a specified data value exists for the current key.
	bool ValueExists(LPCTSTR lpszName);

	/// Returns a string list containing the names of all subkeys belonging to the current key.
	void GetKeyNames(CStrList& List);

	/// Returns a string list containing the names of all data values associated with the current key.
	void GetValueNames(CStrList& List);

	/// Returns information about a specified data value associated with the current key.
	///
	/// @param[in] lpszValueName
	///   The string containing the name of the data value to query.
	/// @param[out] DataInfo
	///   Stores the information about the data value.
	/// @return
	///   On success GetDataInfo returns true. On failure it returns false, and the @a DataInfo
	///   is set to zeros.
	bool GetDataInfo(LPCTSTR lpszValueName, CRegDataInfo& DataInfo);

	/// Returns the size, in bytes, of a specified data value associated with the current key.
	///
	/// @param[in] lpszValueName
	///   The string containing the name of the data value to query.
	/// @return
	///   Returns the data size in bytes. If the data value is a string, GetDataSize returns the size of
	///   the data value and one extra byte for the terminating null character. @n
	///   On failure, GetDataSize returns -1.
	int GetDataSize(LPCTSTR lpszValueName);

	/// Returns the enumerated data type of a specified data value associated with the current key.
	///
	/// @param[in] lpszValueName
	///   The string containing the name of the data value to query.
	/// @return
	///   Returns the type of value associated with the current key.
	REG_DATA_TYPE GetDataType(LPCTSTR lpszValueName);

	/// Returns information about the current key.
	///
	/// @param[in] KeyInfo
	///   Stores the key information returned by this function.
	/// @return
	///   On success GetKeyInfo returns true. On failure it returns false, and the @a KeyInfo is set to zeros.
	bool GetKeyInfo(CRegKeyInfo& KeyInfo);

	/// Retrieves an integer value from a specified data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The name of the data value to read.
	/// @return
	///   The retrieved value.
	/// @remarks
	///   If the registry entry does not contain an integer value, an exception is thrown.
	int ReadInteger(LPCTSTR lpszName);

	/// Retrieves a bool value from a specified data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The name of the data value to read.
	/// @return
	///   The retrieved value.
	/// @remarks
	///   If the registry entry does not contain a bool or integer value, an exception is thrown.
	bool ReadBool(LPCTSTR lpszName);

	/// Retrieves a double value from a specified data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The name of the data value to read.
	/// @return
	///   The retrieved value.
	/// @remarks
	///   If the registry entry does not contain a floating-point value, an exception is thrown.
	double ReadFloat(LPCTSTR lpszName);

	/// Retrieves a string value from a specified data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The name of the data value to read.
	/// @return
	///   The retrieved value.
	/// @remarks
	///   If there is no registry entry with the specified name, ReadString returns an empty string.
	///   If the registry entry contains something other than a string, an exception is thrown.
	CString ReadString(LPCTSTR lpszName);

	/// Retrieves a binary value from a specified data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The name of the data value to read.
	/// @param[in] pBuffer
	///   Stores the registry data returned by this function.
	/// @param[in] nBufSize
	///   Specifies the size in bytes of the @a pBuffer.
	/// @return
	///   The retrieved value.
	/// @remarks
	///   If successful, ReadBinaryData writes the requested data into @a pBuffer and returns the number of
	///   bytes written. If the registry entry contains a known type (such as a string), ReadBinaryData
	///   throws an exception.
	int ReadBinaryData(LPCTSTR lpszName, PVOID pBuffer, int nBufSize);

	/// Stores an integer value in a specifically named data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The string containing the name of the data value in which to store data. If @a lpszName already
	///   exists, its current value is overwritten by WriteInteger. If @a lpszName does not exist,
	///   it is created.
	/// @param[in] nValue
	///   The integer value to store in the registry.
	/// @remarks
	///   If WriteInteger fails, an exception is thrown, and the value is not written to the registry.
	void WriteInteger(LPCTSTR lpszName, int nValue);

	/// Stores a bool value in a specifically named data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The string containing the name of the data value in which to store data. If @a lpszName already
	///   exists, its current value is overwritten by WriteBool. If @a lpszName does not exist,
	///   it is created.
	/// @param[in] bValue
	///   The bool value to store in the registry.
	/// @remarks
	///   If WriteBool fails, an exception is thrown, and the value is not written to the registry.
	void WriteBool(LPCTSTR lpszName, bool bValue);

	/// Stores a double value in a specifically named data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The string containing the name of the data value in which to store data. If @a lpszName already
	///   exists, its current value is overwritten by WriteFloat. If @a lpszName does not exist,
	///   it is created.
	/// @param[in] fValue
	///   The double value to store in the registry.
	/// @remarks
	///   If WriteFloat fails, an exception is thrown, and the value is not written to the registry.
	void WriteFloat(LPCTSTR lpszName, double fValue);

	/// Stores a string value in a specifically named data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The string containing the name of the data value in which to store data. If @a lpszName already
	///   exists, its current value is overwritten by WriteString. If @a lpszName does not exist,
	///   it is created.
	/// @param[in] lpszValue
	///   The string value to store in the registry.
	/// @remarks
	///   If WriteString fails, an exception is thrown, and the value is not written to the registry.
	void WriteString(LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Stores a string containing unexpanded references to environment variables in a specifically
	/// named data value associated with the current key.
	///
	/// Call WriteExpandString to store a string that contains unexpanded references to environment variables
	/// such as "%PATH%". The string is stored in a data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The string containing the name of the data value in which to store data. If @a lpszName already
	///   exists, its current value is overwritten by WriteExpandString. If @a lpszName does not exist,
	///   it is created.
	/// @param[in] lpszValue
	///   The string value to store in the registry.
	/// @remarks
	///   If WriteExpandString fails, an exception is thrown, and the value is not written to the registry.
	void WriteExpandString(LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Stores a specifically named data value associated with the current key.
	///
	/// @param[in] lpszName
	///   The string containing the name of the data value in which to store data. If @a lpszName already
	///   exists, its current value is overwritten by WriteBinary. If @a lpszName does not exist,
	///   it is created.
	/// @param[in] pBuffer
	///   The data buffer containing the data to store in the registry.
	/// @param[in] nBufSize
	///   Indicates the size of @a lpszBuffer.
	/// @remarks
	///   If WriteBinaryData fails, an exception is thrown, and the value is not written to the registry.
	void WriteBinaryData(LPCTSTR lpszName, PVOID pBuffer, int nBufSize);

	/// Returns the registry key that is currently open.
	/// All operations performed by CRegistry affect only the current key. To access another key, call the
	/// OpenKey method.
	HKEY GetCurrentKey() const { return m_hCurrentKey; }

	/// Returns the registry path associated with the current key.
	CString GetCurrentPath() const { return m_strCurrentPath; }

	/// Returns how keys are written to the registry when calling the CloseKey method.
	bool GetLazyWrite() const { return m_bLazyWrite; }

	/// Specifies how keys are written to the registry when calling the CloseKey method.
	/// The m_bLazyWrite is initialized to true when a registry object is first created. When m_bLazyWrite
	/// is true, keys are written to the registry when they are closed, but the CloseKey method may return
	/// before the write operation takes place.
	/// When m_bLazyWrite is false, keys are written to the registry before CloseKey returns.
	/// Set m_bLazyWrite to false only when absolutely necessary.
	void SetLazyWrite(bool bValue) { m_bLazyWrite = bValue; }

	/// Returns the root key for the CRegistry object in an application.
	HKEY GetRootKey() const { return m_hRootKey; }

	/// Specifies the root key for the CRegistry object in an application.
	/// Use RootKey to determine the hierarchy of subkeys an application can access, or to specify
	/// the root key for the application. By default, root key is set to HKEY_CURRENT_USER when the
	/// registry object is created.
	void SetRootKey(HKEY hValue);
};

///////////////////////////////////////////////////////////////////////////////
/// CCustomIniFile - The base class for manipulating INI file.
///
/// CCustomIniFile is the base class that encapsulates access to .ini files.
/// Do not create instances of CCustomIniFile. Instead, use or create descendant classes,
/// such as CIniFile.

class CCustomIniFile
{
protected:
	CString m_strFileName;      ///< The INI file name.
public:
	/// Constructor using a specified ini file name
	explicit CCustomIniFile(LPCTSTR lpszFileName);
	/// Destructor
	virtual ~CCustomIniFile() {}

	/// Retrieves an integer value from an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] nDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	virtual int ReadInteger(LPCTSTR lpszSection, LPCTSTR lpszName, int nDefault = 0);

	/// Retrieves a bool value from an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] bDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	virtual bool ReadBool(LPCTSTR lpszSection, LPCTSTR lpszName, bool bDefault = false);

	/// Retrieves a double-precision value from an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] fDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	virtual double ReadFloat(LPCTSTR lpszSection, LPCTSTR lpszName, double fDefault = 0);

	/// Retrieves a string value from an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] lpszDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	virtual CString ReadString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault = TEXT("")) = 0;

	/// Retrieves a binary value from an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[out] Value
	///   Stores the retrieved value.
	/// @return
	///   The number of bytes written to the buffer @a Value.
	int ReadBinaryData(LPCTSTR lpszSection, LPCTSTR lpszName, CBuffer& Value);

	/// Retrieves a binary value from an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[out] Value
	///   Stores the retrieved value.
	/// @return
	///   The number of bytes written to the stream @a Value.
	int ReadBinaryStream(LPCTSTR lpszSection, LPCTSTR lpszName, CStream& Value);

	/// Writes an integer value to an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] nValue
	///   The value to write.
	virtual void WriteInteger(LPCTSTR lpszSection, LPCTSTR lpszName, int nValue);

	/// Writes a bool value to an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] bValue
	///   The value to write.
	virtual void WriteBool(LPCTSTR lpszSection, LPCTSTR lpszName, bool bValue);

	/// Writes a double-precision value to an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] fValue
	///   The value to write.
	virtual void WriteFloat(LPCTSTR lpszSection, LPCTSTR lpszName, double fValue);

	/// Writes a string value to an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] lpszValue
	///   The value to write.
	virtual void WriteString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue) = 0;

	/// Writes a binary value to an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] pDataBuf
	///   Pointer to the data to be written.
	/// @param[in] nDataSize
	///   The size in bytes of the buffer @a pDataBuf.
	void WriteBinaryData(LPCTSTR lpszSection, LPCTSTR lpszName, PVOID pDataBuf, int nDataSize);

	/// Writes a binary value to an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] Value
	///   The stream to be written.
	void WriteBinaryStream(LPCTSTR lpszSection, LPCTSTR lpszName, CStream& Value);

	/// Reads all the key names from a specified section of an ini file into a string list.
	///
	/// @param[in] lpszSection
	///   The section from which to retrieve a list of key names.
	/// @param[out] KeyNames
	///   Stores the retrieved names.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSection()</b>.
	virtual void ReadKeyNames(LPCTSTR lpszSection, CStrings& KeyNames) = 0;

	/// Reads the names of all sections in an ini file into a string list.
	///
	/// @param[out] SectionNames
	///   Stores the retrieved names.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSections()</b>.
	virtual void ReadSectionNames(CStrings& SectionNames) = 0;

	/// Reads all lines from a specified section of an ini file into a string list.
	/// The results are added to the string list as strings of the form KeyName=ValueName.
	///
	/// @param[in] lpszSection
	///   The section in the file from which to read strings.
	/// @param[out] Strings
	///   Stores the retrieved strings.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSectionValues()</b>.
	virtual void ReadStrings(LPCTSTR lpszSection, CStrings& Strings) = 0;

	/// Erases an entire section of an ini file.
	virtual void EraseSection(LPCTSTR lpszSection) = 0;

	/// Erases the data value associated with an ini file entry.
	virtual void DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszName) = 0;

	/// Indicates whether a section exists in the ini file.
	virtual bool SectionExists(LPCTSTR lpszSection);

	/// Indicates whether a key exists in the ini file.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ValueExists()</b>.
	virtual bool KeyExists(LPCTSTR lpszSection, LPCTSTR lpszName);

	/// Flushes buffered ini file data to disk.
	virtual void UpdateFile() = 0;

	/// Returns the ini file name.
	const CString& GetFileName() const { return m_strFileName; }
};

///////////////////////////////////////////////////////////////////////////////
/// CMemIniFile - Memory INI file class.
///
/// CMemIniFile loads an entire INI file into memory and allows all operations
/// to be performed on the memory image.  The image can then be written out to
/// the disk file.

class CMemIniFile : public CCustomIniFile
{
private:
	CStrList m_Sections;
	bool m_bModified;
private:
	void LoadData();
	CStrings* AddSection(LPCTSTR lpszSection);
	CStrings* GetSectionData(LPCTSTR lpszSection);
public:
	/// Constructor using a specified ini file name.
	explicit CMemIniFile(LPCTSTR lpszFileName);
	/// Destructor.
	virtual ~CMemIniFile();

	/// Retrieves a string value from an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] lpszDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	virtual CString ReadString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault = TEXT(""));

	/// Writes a string value to an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] lpszValue
	///   The value to write.
	virtual void WriteString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Reads all the key names from a specified section of an ini file into a string list.
	///
	/// @param[in] lpszSection
	///   The section from which to retrieve a list of key names.
	/// @param[out] KeyNames
	///   Stores the retrieved names.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSection()</b>.
	virtual void ReadKeyNames(LPCTSTR lpszSection, CStrings& KeyNames);

	/// Reads the names of all sections in an ini file into a string list.
	///
	/// @param[out] SectionNames
	///   Stores the retrieved names.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSections()</b>.
	virtual void ReadSectionNames(CStrings& SectionNames);

	/// Reads all lines from a specified section of an ini file into a string list.
	/// The results are added to the string list as strings of the form KeyName=ValueName.
	///
	/// @param[in] lpszSection
	///   The section in the file from which to read strings.
	/// @param[out] Strings
	///   Stores the retrieved strings.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSectionValues()</b>.
	virtual void ReadStrings(LPCTSTR lpszSection, CStrings& Strings);

	/// Erases an entire section of an ini file.
	virtual void EraseSection(LPCTSTR lpszSection);

	/// Erases the data value associated with an ini file entry.
	virtual void DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszName);

	/// Flushes buffered ini file data to disk.
	virtual void UpdateFile();

	/// Erases all data from the INI file in memory.
	void Clear();

	/// Populates the memory buffer with contents of a string list.
	void SetStrings(const CStrings& List);

	/// Populates a string list with INI file data.
	void GetStrings(CStrings& List);
};

///////////////////////////////////////////////////////////////////////////////
/// CIniFile - INI file class.
///
/// CIniFile enables handling the storage and retrieval of application-specific information and
/// settings in a standard INI file. An INI file stores information in logical groupings, called
/// "sections." Within each section, actual data values are stored in named keys.
/// Keys take the form: name=value.

class CIniFile : public CCustomIniFile
{
private:
	void InternalUpdateFile();
public:
	/// Constructor using a specified ini file name.
	explicit CIniFile(LPCTSTR lpszFileName) : CCustomIniFile(lpszFileName) {}
	/// Destructor.
	virtual ~CIniFile();

	/// Retrieves a string value from an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contains the desired key.
	/// @param[in] lpszName
	///   The name of the key from which to retrieve the value.
	/// @param[in] lpszDefault
	///   The default value to return if the value does not exist.
	/// @return
	///   The retrieved value.
	virtual CString ReadString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszDefault = TEXT(""));

	/// Writes a string value to an ini file.
	///
	/// @param[in] lpszSection
	///   The section in the file that contain the key to which to write.
	/// @param[in] lpszName
	///   The name of the key for which to set a value.
	/// @param[in] lpszValue
	///   The value to write.
	virtual void WriteString(LPCTSTR lpszSection, LPCTSTR lpszName, LPCTSTR lpszValue);

	/// Reads all the key names from a specified section of an ini file into a string list.
	///
	/// @param[in] lpszSection
	///   The section from which to retrieve a list of key names.
	/// @param[out] KeyNames
	///   Stores the retrieved names.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSection()</b>.
	virtual void ReadKeyNames(LPCTSTR lpszSection, CStrings& KeyNames);

	/// Reads the names of all sections in an ini file into a string list.
	///
	/// @param[out] SectionNames
	///   Stores the retrieved names.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSections()</b>.
	virtual void ReadSectionNames(CStrings& SectionNames);

	/// Reads all lines from a specified section of an ini file into a string list.
	/// The results are added to the string list as strings of the form KeyName=ValueName.
	///
	/// @param[in] lpszSection
	///   The section in the file from which to read strings.
	/// @param[out] Strings
	///   Stores the retrieved strings.
	/// @note
	///   This method is mapped to <b>Delphi::TCustomIniFile::ReadSectionValues()</b>.
	virtual void ReadStrings(LPCTSTR lpszSection, CStrings& Strings);

	/// Erases an entire section of an ini file.
	virtual void EraseSection(LPCTSTR lpszSection);

	/// Erases the data value associated with an ini file entry.
	virtual void DeleteKey(LPCTSTR lpszSection, LPCTSTR lpszName);

	/// Flushes buffered ini file data to disk.
	virtual void UpdateFile();
};

///////////////////////////////////////////////////////////////////////////////
/// CMonitor - Monitor class.
///
/// CMonitor represents a monitor that displays the user interface of an application. It introduces
/// methods to describe the dimensions of a single monitor. In addition, the IsPrimary method
/// distinguishes the primary property from other monitors on the system.
///
/// When writing multi-monitor applications, use CMonitor to help lay out forms and dialogs so that
/// they are not split over several monitors.
///
/// Applications do not create instances of CMonitor. Instead, monitor components for all available
/// monitors are listed by the GetMonitors method of the global Screen(). The global Screen() also
/// provides global information about the monitors on which the application runs, such as screen
/// resolution, available fonts, and so on.

class CMonitor
{
private:
	HMONITOR m_hHandle;
	int m_nMonitorIndex;    // 0-based
public:
	CMonitor(HMONITOR hHandle, int nMonitorIndex);
public:
	/// Indicates the Windows handle for the monitor.
	HMONITOR GetHandle() const { return m_hHandle; }

	/// Specifies the index (0-based) of the monitor in the global screen object's GetMonitors() list.
	int GetMonitorIndex() const { return m_nMonitorIndex; }

	/// Indicates the logical position of the left edge of the monitor.
	/// In multi-monitor applications, the available screen space can span multiple monitors from left
	/// to right. Call GetLeft to determine where, in the entire available screen space, this monitor
	/// appears. The value of GetLeft is in pixels relative to the left edge of the primary monitor.
	int GetLeft() const;

	/// Indicates the logical position of the top edge of the monitor.
	/// In multi-monitor applications, the available screen space can span multiple monitors from top to
	/// bottom. Call GetTop to determine where, in the entire available screen space, this monitor appears.
	/// The value of GetTop is in pixels relative to the top edge of the primary monitor.
	int GetTop() const;

	/// Indicates the horizontal size of the monitor in pixels.
	int GetWidth() const;

	/// Indicates the vertical size of the monitor in pixels.
	int GetHeight() const;

	/// Indicates the dimensions of the monitor in pixels.
	/// Call GetBoundsRect to learn the dimensions of the monitor. GetBoundsRect gives the dimensions of
	/// the monitor in pixels, where (0,0) represents the top-left corner of the primary monitor.
	CRect GetBoundsRect() const;

	/// Indicates the dimensions of the usable area on the monitor.
	/// Call GetWorkAreaRect to learn the dimensions of the monitor, minus any space taken by task bars
	/// or tool bars. GetWorkAreaRect gives the dimensions of the work area in pixels, where (0,0)
	/// represents the top-left corner of the primary monitor.
	CRect GetWorkAreaRect() const;

	/// Indicates whether this is the primary monitor.
	bool IsPrimary() const;
};

///////////////////////////////////////////////////////////////////////////////
/// CScreen - Screen class.
///
/// CScreen represents the state of the screen in which an application runs.
///
/// Do not instantiate separate CScreen objects by calling the constructor. Instead,
/// use the Screen() method to get the global CScreen object.

class CScreen
{
private:
	static std::auto_ptr<CScreen> s_pSingleton;
private:
	CPointerList m_MonitorList;
	CStrList m_FontList;
	CStrList m_ImeList;
	CString m_strDefaultIme;
	int m_nPixelsPerInch;
	HKL m_nDefaultKbLayout;
private:
	void InitMonitorList();
	void ClearMonitorList();
protected:
	CScreen();
public:
	~CScreen();
	static CScreen& Instance();

	/// Indicates the horizontal size of the screen in pixels.
	int GetWidth();

	/// Indicates the vertical size of the screen in pixels.
	int GetHeight();

	/// Returns the boundaries of the virtual desktop relative to the upper-left corner of the primary monitor.
	/// Use GetDesktopRect to determine the coordinates of the entire virtual desktop, which includes all
	/// monitors in the system. GetDesktopRect is expressed in coordinates where (0,0) is the upper-left
	/// corner of the primary monitor. The primary monitor is the monitor with its Primary property set
	/// to true.
	CRect GetDesktopRect();

	/// Returns the x-coordinate of the desktop's left edge relative to the upper-left corner of the
	/// primary monitor. On a single-monitor system, GetDesktopLeft is 0.
	int GetDesktopLeft();

	/// Returns the y-coordinate of the entire desktop's top edge relative to the upper-left corner of the
	/// primary monitor. On a single-monitor system, GetDesktopTop is 0.
	int GetDesktopTop();

	/// Returns the width of the entire virtual desktop.
	/// Use GetDesktopWidth to determine the width of the desktop. The desktop is defined as the entire
	/// virtual desktop, which includes all monitors in the system. On a single-monitor system,
	/// GetDesktopWidth corresponds to GetWidth().
	int GetDesktopWidth();

	/// Returns the height of the entire virtual desktop.
	/// Use GetDesktopHeight to determine the height of the entire desktop. The desktop is defined as the
	/// entire virtual desktop, which includes all monitors in the system. On a single-monitor system,
	/// GetDesktopHeight corresponds to GetHeight().
	int GetDesktopHeight();

	/// Returns the boundaries of the usable work area on the primary monitor.
	/// Use GetWorkAreaRect to determine the coordinates of the usable space on the primary monitor.
	/// The usable work space is the surface of the primary monitor with the space occupied by the task bar
	/// and any tool bars subtracted off.
	CRect GetWorkAreaRect();

	/// Returns the left edge of the work area on the primary monitor.
	int GetWorkAreaLeft();

	/// Returns the top edge of the work area on the primary monitor.
	int GetWorkAreaTop();

	/// Returns the width of the work area on the primary monitor.
	int GetWorkAreaWidth();

	/// Returns the height of the work area on the primary monitor.
	int GetWorkAreaHeight();

	/// Returns the number of monitors used to comprise the desktop.
	int GetMonitorCount();

	/// Returns the monitor object by 0-based index.
	CMonitor* GetMonitor(int nIndex);

	/// Returns the parimary monitor object.
	CMonitor* GetPrimaryMonitor();

	/// Lists the face names for all fonts supported by the screen.
	const CStrList& GetFontNames();

	/// Lists all input method editors (IMEs) installed on the system.
	const CStrList& GetImeNames();

	/// Indicates which input method editor (IME) is active when the CScreen object is created.
	CString GetDefaultIme();

	/// Indicates the Windows handle to the keyboard layout that was active when the app started running.
	HKL GetDefaultKbLayout();

	/// Indicates the number of screen pixels that make up a logical inch in the vertical direction.
	int GetPixelsPerInch();
};

///////////////////////////////////////////////////////////////////////////////
/// CClipboard - Clipboard class.
///
/// CClipboard is a wrapper for the system clipboard, which contains information that has
/// been cut or copied from an application.
///
/// Do not instantiate separate CClipboard objects by calling the constructor. Instead,
/// use the Clipboard() method to get the global CClipboard object.

class CClipboard
{
public:
	/// Calls Open() and Close() automatically in a scope.
	class CAutoOpener
	{
	private:
		CClipboard& m_Owner;
	public:
		/// Constructor. The constructor calls Open() automatically.
		CAutoOpener(CClipboard& Owner) : m_Owner(Owner) { m_Owner.Open(); }
		/// Destructor. The destructor calls Close() automatically.
		~CAutoOpener() { m_Owner.Close(); }
	};

private:
	static std::auto_ptr<CClipboard> s_pSingleton;
private:
	int m_nOpenRefCount;
	bool m_bEmptied;
	HWND m_hWndOwner;
private:
	HWND GetOwnerWindow();
	void BeforeSetData();
	void SetBuffer(int nFormat, PVOID pBuffer, int nSize);
protected:
	CClipboard();
public:
	~CClipboard() {}
	static CClipboard& Instance();

	/// Specifies the handle of owner window.
	/// In MFC application, the default owner window is AfxGetMainWnd(), and in WTL application,
	/// the default owner window is not specified.
	void SetOwnerWindow(HWND hWnd) { m_hWndOwner = hWnd; }

	/// Opens the clipboard, preventing other applications from changing its contents until the
	/// clipboard is closed.
	/// @remarks
	///   Use CClipboard::CAutoOpener object to call Open and Close automatically in a scope.
	void Open();

	/// Closes the clipboard if it is open.
	/// @remarks
	///   Use CClipboard::CAutoOpener object to call Open and Close automatically in a scope.
	void Close();

	/// Deletes the contents of the clipboard.
	void Clear();

	/// Places contents in the clipboard as a string.
	void SetAsText(LPCTSTR lpszText);

	/// Retrieves contents from the clipboard as a string.
	CString GetAsText();

	/// Writes data represented by the specified handle @a hHandle to the clipboard using the specified
	/// format @a nFormat.
	void SetAsHandle(int nFormat, HANDLE hHandle);

	/// Retrieves data from the clipboard using Windows handles in the specified format @a nFormat.
	HANDLE GetAsHandle(int nFormat);

	/// Returns the number of different format types used to encode the information currently stored
	/// in the clipboard.
	int GetFormatCount();

	/// Returns the clipboard format by the specified index (0-based).
	int GetFormat(int nIndex);
};

///////////////////////////////////////////////////////////////////////////////
/// CUrl - The URL parser class.

class CUrl
{
public:
	/// The URL parts.
	enum URL_PART
	{
		URL_PROTOCOL  = 0x0001,
		URL_HOST      = 0x0002,
		URL_PORT      = 0x0004,
		URL_PATH      = 0x0008,
		URL_FILENAME  = 0x0010,
		URL_BOOKMARK  = 0x0020,
		URL_USERNAME  = 0x0040,
		URL_PASSWORD  = 0x0080,
		URL_PARAMS    = 0x0100,
		URL_ALL       = 0xFFFF,
	};

private:
	CString m_strProtocol;
	CString m_strHost;
	CString m_strPort;
	CString m_strPath;
	CString m_strFileName;
	CString m_strBookmark;
	CString m_strUserName;
	CString m_strPassword;
	CString m_strParams;
public:
	/// Constructor.
	CUrl(LPCTSTR lpszUrl = TEXT(""));
	/// Copy constructor.
	CUrl(const CUrl& src);
	/// Destructor.
	virtual ~CUrl() {}

	/// Clears all data.
	void Clear();
	/// Assignment operator.
	CUrl& operator = (const CUrl& rhs);

	/// Combines all the url parts and returns it.
	CString GetUrl() const;
	/// Combines the specified url parts and returns it.
	CString GetUrl(UINT nParts);
	/// Parses the specified url string and splits it to several parts.
	void SetUrl(LPCTSTR lpszValue);

	/// Returns the part of protocol.
	const CString& GetProtocol() const { return m_strProtocol; }
	/// Returns the part of host.
	const CString& GetHost() const { return m_strHost; }
	/// Returns the part of port.
	const CString& GetPort() const { return m_strPort; }
	/// Returns the part of path.
	const CString& GetPath() const { return m_strPath; }
	/// Returns the part of filename.
	const CString& GetFileName() const { return m_strFileName; }
	/// Returns the part of bookmark.
	const CString& GetBookmark() const { return m_strBookmark; }
	/// Returns the part of username.
	const CString& GetUserName() const { return m_strUserName; }
	/// Returns the part of password.
	const CString& GetPassword() const { return m_strPassword; }
	/// Returns the part of parameters.
	const CString& GetParams() const { return m_strParams; }

	/// Sets the part of protocol.
	void SetProtocol(LPCTSTR lpszValue) { m_strProtocol = lpszValue; }
	/// Sets the part of host.
	void SetHost(LPCTSTR lpszValue) { m_strHost = lpszValue; }
	/// Sets the part of port.
	void SetPort(LPCTSTR lpszValue) { m_strPort = lpszValue; }
	/// Sets the part of path.
	void SetPath(LPCTSTR lpszValue) { m_strPath = lpszValue; }
	/// Sets the part of filename.
	void SetFileName(LPCTSTR lpszValue) { m_strFileName = lpszValue; }
	/// Sets the part of bookmark.
	void SetBookmark(LPCTSTR lpszValue) { m_strBookmark = lpszValue; }
	/// Sets the part of username.
	void SetUserName(LPCTSTR lpszValue) { m_strUserName = lpszValue; }
	/// Sets the part of password.
	void SetPassword(LPCTSTR lpszValue) { m_strPassword = lpszValue; }
	/// Sets the part of parameters.
	void SetParams(LPCTSTR lpszValue) { m_strParams = lpszValue; }
};

///////////////////////////////////////////////////////////////////////////////
/// CPacket - The base class for building packets.

class CPacket
{
public:
	// The default delta bytes for memory growth. (must be 2^n)
	enum { DEFAULT_MEMORY_DELTA = 1024 };

private:
	void Init();
protected:
	CMemoryStream *m_pStream;
	bool m_bAvailable;
	bool m_bIsPacked;
protected:
	void ThrowUnpackError();
	void ThrowPackError();
	void CheckUnsafeSize(int nValue);

	void ReadBuffer(void *pBuffer, int nBytes);
	void ReadINT8(INT8& nValue) { ReadBuffer(&nValue, sizeof(INT8)); }
	void ReadINT16(INT16& nValue) { ReadBuffer(&nValue, sizeof(INT16)); }
	void ReadINT32(INT32& nValue) { ReadBuffer(&nValue, sizeof(INT32)); }
	void ReadINT64(INT64& nValue) { ReadBuffer(&nValue, sizeof(INT64)); }
	void ReadString(std::string& str);
	void ReadString(CString& str);
	void ReadBlob(std::string& str);
	void ReadBlob(CStringA& str);
	void ReadBlob(CStream& Stream);
	void ReadBlob(CBuffer& Buffer);
	INT8 ReadINT8() { INT8 v; ReadINT8(v); return v; }
	INT16 ReadINT16() { INT16 v; ReadINT16(v); return v; }
	INT32 ReadINT32() { INT32 v; ReadINT32(v); return v; }
	INT64 ReadINT64() { INT64 v; ReadINT64(v); return v; }
	bool ReadBool() { INT8 v; ReadINT8(v); return (v? true : false); }
	std::string ReadString() { std::string v; ReadString(v); return v; }

	void WriteBuffer(const void *pBuffer, int nBytes);
	void WriteINT8(const INT8& nValue) { WriteBuffer(&nValue, sizeof(INT8)); }
	void WriteINT16(const INT16& nValue) { WriteBuffer(&nValue, sizeof(INT16)); }
	void WriteINT32(const INT32& nValue) { WriteBuffer(&nValue, sizeof(INT32)); }
	void WriteINT64(const INT64& nValue) { WriteBuffer(&nValue, sizeof(INT64)); }
	void WriteBool(bool bValue) { WriteINT8(bValue ? 1 : 0); }
	void WriteString(const std::string& str);
	void WriteString(const CString& str);
	void WriteBlob(void *pBuffer, int nBytes);
	void WriteBlob(const CBuffer& Buffer);

	void FixStrLength(std::string& str, int nLength);
	void TruncString(std::string& str, int nMaxLength);
protected:
	virtual void DoPack() {}
	virtual void DoUnpack() {}
	virtual void DoAfterPack() {}
	virtual void DoEncrypt() {}
	virtual void DoDecrypt() {}
	virtual void DoCompress() {}
	virtual void DoDecompress() {}
public:
	CPacket();
	virtual ~CPacket();

	/// Packs the packet.
	bool Pack();
	/// Unpacks the packet.
	bool Unpack(void *pBuffer, int nBytes);
	/// Unpacks the packet.
	bool Unpack(const CBuffer& Buffer);
	/// Clears the data.
	void Clear();
	/// Ensure the packet is packed.
	void EnsurePacked();

	/// Returns the buffer of packed packet.
	char* GetBuffer() const { return (m_pStream? (char*)m_pStream->GetMemory() : NULL); }
	/// Returns the packet size in bytes of packed packet.
	int GetSize() const { return (m_pStream? (int)m_pStream->GetSize() : 0); }
	/// Indicates whether the packet buffer is accessable or not.
	bool Available() const { return m_bAvailable; }
	/// Indicates whether the packet is packed or not.
	bool IsPacked() const { return m_bIsPacked; }
};

///////////////////////////////////////////////////////////////////////////////
/// CBits - The bits management class.

class CBits
{
protected:
	CBuffer m_Buffer;
	int m_nBitCount;
public:
	/// Default Constructor.
	CBits();
	/// Copy Constructor.
	CBits(const CBits& src);
	/// Destructor.
	virtual ~CBits();

	/// Returns the number of bytes occupied by the buffer that hold the specified number of bits.
	static int CalcByteCount(int nBitCount);

	/// Returns the value of the bit at the position (0-based).
	bool GetBit(int nIndex) const;
	/// Updates the bit value at the position (0-based) to the specified value.
	void SetBit(int nIndex, bool bValue);

	/// Updates all of the bits to 1.
	void SetAllBits();
	/// Updates all of the bits to 0.
	void ClearAllBits();
	/// Fills the unused bits to the specified value.
	void FillUnusedBits(bool bValue) const;

	/// Performs "AND" operation with @a src.
	void AndBits(const CBits& src);
	/// Performs "OR" operation with @a src.
	void OrBits(const CBits& src);
	/// Performs "XOR" operation with @a src.
	void XorBits(const CBits& src);
	/// Performs "NOT" operation.
	void NotBits();

	/// Returns the number of bits that the object can hold.
	int GetBitCount() const { return m_nBitCount; }
	/// Sets the number of bits that the object can hold.
	void SetBitCount(int nCount);

	/// Returns the number of set bits.
	int GetTotalBitsSet() const;
	/// Indicates whether all of the bits is 1.
	bool IsAllBitsSet() const;
	/// Indicates whether all of the bits is 0.
	bool IsAllBitsCleared() const;
	/// Finds the first cleared bit and returns the position (0-based).
	/// Returns -1 if not found.
	int FindFirstClearedBit(int nStartIndex) const;

	/// Gets the bits buffer.
	void GetBuffer(CBuffer& Buffer) const;
	/// Sets the bits buffer and the number of bits.
	void SetBuffer(const CBuffer& Buffer, int nBitCount);

	/// Returns the bits buffer pointer. NULL if empty.
	char* GetData() const { return (m_nBitCount? m_Buffer.Data() : NULL); }
	/// Returns the number of bytes occupied by the bit buffer.
	int GetByteCount() const;
	/// Gets the bits buffer.
	const CBuffer& GetBuffer() const { return m_Buffer; }

	/// Sets the number of bits to 0.
	void Clear() { SetBitCount(0); }

	/// Assignment operator.
	CBits& operator = (const CBits& rhs);
	/// Returns the value of the bit at the position (0-based).
	bool operator[] (int nIndex) const { return GetBit(nIndex); }
};

///////////////////////////////////////////////////////////////////////////////
/// CAutoInvokable - The base class for auto-invokable object.

class CAutoInvokable
{
public:
	friend class CAutoInvoker;

protected:
	virtual void InvokeInitialize() {}
	virtual void InvokeFinalize() {}
public:
	virtual ~CAutoInvokable() {}
};

///////////////////////////////////////////////////////////////////////////////
/// CAutoInvoker - The invoker class for CAutoInvokable.

class CAutoInvoker
{
private:
	CAutoInvokable *m_pObject;
public:
	explicit CAutoInvoker(CAutoInvokable& Object)
		{ m_pObject = &Object; m_pObject->InvokeInitialize(); }

	explicit CAutoInvoker(CAutoInvokable *pObject)
		{ m_pObject = pObject; if (m_pObject) m_pObject->InvokeInitialize(); }

	virtual ~CAutoInvoker()
		{ if (m_pObject) m_pObject->InvokeFinalize(); }
};

///////////////////////////////////////////////////////////////////////////////
/// CLogger - The logger.

class CLogger
{
private:
	CString m_strFileName;
	bool m_bNewFileDaily;
	CCriticalSection *m_pLock;
private:
	CString GetLogFileName();
	bool OpenFile(CFileStream& FileStream, LPCTSTR lpszFileName);
	void WriteToFile(const char *str);
private:
	CLogger();
public:
	~CLogger();
	static CLogger& Instance();

	/// Sets the log filename including path.
	void SetFileName(LPCTSTR lpszFileName, bool bNewFileDaily = false);

	/// Outputs the log.
	void WriteStr(LPCTSTR lpszStr);
	/// Outputs the log.
	void WriteFmt(LPCTSTR lpszFormat, ...);
	/// Converts the exception to a string and outputs it.
	void WriteException(CException *e);
};

///////////////////////////////////////////////////////////////////////////////
/// CCustomObjectList - The base class of object list.

template<typename ObjectType>
class CCustomObjectList
{
public:
	class CLock : public CAutoInvokable
	{
	private:
		CCriticalSection *m_pLock;
	protected:
		virtual void InvokeInitialize() { if (m_pLock) m_pLock->Lock(); }
		virtual void InvokeFinalize() { if (m_pLock) m_pLock->Unlock(); }
	public:
		CLock(bool bActive) : m_pLock(NULL) { if (bActive) m_pLock = new CCriticalSection(); }
		virtual ~CLock() { delete m_pLock; }
	};

	typedef ObjectType* ObjectPtr;

protected:
	CPointerList m_Items;     // Object list.
	bool m_bOwnsObjects;      // Indicates whether to destroy the object automatically when remove it from the list.
	mutable CLock m_Lock;

protected:
	virtual void NotifyDelete(int nIndex)
	{
		if (m_bOwnsObjects)
		{
			ObjectPtr pItem = (ObjectPtr)m_Items[nIndex];
			m_Items[nIndex] = NULL;
			delete pItem;
		}
	}
protected:
	/// Inserts a new item at the end of the list.
	int Add(ObjectPtr pItem, bool bAllowDuplicate = true)
	{
		IFC_ASSERT(pItem);
		CAutoLocker Locker(m_Lock);

		if (bAllowDuplicate || m_Items.IndexOf(pItem) == -1)
			return m_Items.Add(pItem);
		else
			return -1;
	}

	/// Removes a specified item from the list.
	int Remove(ObjectPtr pItem)
	{
		CAutoLocker Locker(m_Lock);

		int nResult = m_Items.IndexOf(pItem);
		if (nResult >= 0)
		{
			NotifyDelete(nResult);
			m_Items.Delete(nResult);
		}
		return nResult;
	}

	/// Removes a specified item from the list without destroying it.
	ObjectPtr Extract(ObjectPtr pItem)
	{
		CAutoLocker Locker(m_Lock);

		ObjectPtr pResult = NULL;
		int i = m_Items.Remove(pItem);
		if (i >= 0)
			pResult = pItem;
		return pResult;
	}

	/// Removes a specified item from the list without destroying it.
	ObjectPtr Extract(int nIndex)
	{
		CAutoLocker Locker(m_Lock);

		ObjectPtr pResult = NULL;
		if (nIndex >= 0 && nIndex < m_Items.GetCount())
		{
			pResult = (ObjectPtr)m_Items[nIndex];
			m_Items.Delete(nIndex);
		}
		return pResult;
	}

	/// Removes the item at the position given by the @a nIndex parameter.
	void Delete (int nIndex)
	{
		CAutoLocker Locker(m_Lock);

		if (nIndex >= 0 && nIndex < m_Items.GetCount())
		{
			NotifyDelete(nIndex);
			m_Items.Delete(nIndex);
		}
	}

	/// Inserts a new item at the middle of the list.
	void Insert(int nIndex, ObjectPtr pItem)
	{
		IFC_ASSERT(pItem);
		CAutoLocker Locker(m_Lock);
		m_Items.Insert(nIndex, pItem);
	}

	/// Returns the index (0-based) of the first item in the list with a specified value.
	int IndexOf(ObjectPtr pItem) const
	{
		CAutoLocker Locker(m_Lock);
		return m_Items.IndexOf(pItem);
	}

	/// Indicates whether the specified item exists in the list or not.
	bool Exists(ObjectPtr pItem) const
	{
		CAutoLocker Locker(m_Lock);
		return m_Items.IndexOf(pItem) >= 0;
	}

	/// Returns the first item in the list.
	/// Throws exception if the list is empty.
	ObjectPtr First() const
	{
		CAutoLocker Locker(m_Lock);
		return (ObjectPtr)m_Items.First();
	}

	/// Returns the last item in the list.
	/// Throws exception if the list is empty.
	ObjectPtr Last() const
	{
		CAutoLocker Locker(m_Lock);
		return (ObjectPtr)m_Items.Last();
	}

	/// Deletes all items from the list.
	void Clear()
	{
		CAutoLocker Locker(m_Lock);

		for (int i = m_Items.GetCount() - 1; i >= 0; i--)
			NotifyDelete(i);
		m_Items.Clear();
	}

	/// Destroy all object items in the list without removing item.
	void FreeObjects()
	{
		CAutoLocker Locker(m_Lock);

		for (int i = m_Items.GetCount() - 1; i >= 0; i--)
		{
			ObjectPtr pItem = (ObjectPtr)m_Items[i];
			m_Items[i] = NULL;
			delete pItem;
		}
	}

	/// Returns the number of items in the list.
	inline int GetCount() const { return m_Items.GetCount(); }

	/// Sets or gets the item at the specified index (0-based).
	inline ObjectPtr& GetItem(int nIndex) const { return (ObjectPtr&)m_Items[nIndex]; }

	/// Sets or gets the item at the specified index (0-based).
	inline ObjectPtr& operator[] (int nIndex) const { return GetItem(nIndex); }

	/// Assignment operator.
	inline CCustomObjectList& operator = (const CCustomObjectList& rhs) { m_Items = rhs.m_Items; return *this; }

	/// Indicates whether the list is empty or not.
	inline bool IsEmpty() const { return (GetCount() <= 0); }

	/// Sets owns objects or not.
	inline void SetOwnsObjects(bool bValue) { m_bOwnsObjects = bValue; }

	/// Returns the lock object.
	inline CLock& GetLock() const { return m_Lock; }
public:
	/// Default constructor.
	CCustomObjectList() :
		m_Lock(false), m_bOwnsObjects(true) {}

	  /// Constructor.
	CCustomObjectList(bool bThreadSafe, bool bOwnsObjects) :
		m_Lock(bThreadSafe), m_bOwnsObjects(bOwnsObjects) {}

	/// Destructor.
	virtual ~CCustomObjectList() { Clear(); }
};

///////////////////////////////////////////////////////////////////////////////
/// CObjectList - Object list class.

template<typename ObjectType>
class CObjectList : public CCustomObjectList<ObjectType>
{
public:
	CObjectList() :
		CCustomObjectList<ObjectType>(false, true) {}
	CObjectList(bool bThreadSafe, bool bOwnsObjects) :
		CCustomObjectList<ObjectType>(bThreadSafe, bOwnsObjects) {}
	virtual ~CObjectList() {}

	using CCustomObjectList<ObjectType>::Add;
	using CCustomObjectList<ObjectType>::Remove;
	using CCustomObjectList<ObjectType>::Extract;
	using CCustomObjectList<ObjectType>::Delete;
	using CCustomObjectList<ObjectType>::Insert;
	using CCustomObjectList<ObjectType>::IndexOf;
	using CCustomObjectList<ObjectType>::Exists;
	using CCustomObjectList<ObjectType>::First;
	using CCustomObjectList<ObjectType>::Last;
	using CCustomObjectList<ObjectType>::Clear;
	using CCustomObjectList<ObjectType>::FreeObjects;
	using CCustomObjectList<ObjectType>::GetCount;
	using CCustomObjectList<ObjectType>::GetItem;
	using CCustomObjectList<ObjectType>::operator=;
	using CCustomObjectList<ObjectType>::operator[];
	using CCustomObjectList<ObjectType>::IsEmpty;
	using CCustomObjectList<ObjectType>::SetOwnsObjects;
	using CCustomObjectList<ObjectType>::GetLock;
};

///////////////////////////////////////////////////////////////////////////////
// IEventHandler - C++ event handler interface.

class CNullSender {};
class CNullParam {};

template<typename SenderType, typename ParamType>
class IEventHandler
{
public:
	virtual ~IEventHandler() {}
	virtual void HandleEvent(const SenderType& Sender, const ParamType& Param) = 0;
};

// CEventInvoker - C++ event invoker class.

template<typename SenderType, typename ParamType>
class CEventInvoker
{
public:
	typedef IEventHandler<SenderType, ParamType> EventHanderType;
private:
	CPointerList m_HandlerList;       // (EventHanderType*)[]
public:
	virtual ~CEventInvoker() {}

	/// Registers an event handler to this invoker.
	virtual void RegisterHandler(EventHanderType *pHandler)
	{
		if (pHandler && m_HandlerList.IndexOf(pHandler) == -1)
			m_HandlerList.Add(pHandler);
	}

	/// Unregisters an event handler from this invoker.
	virtual void UnregisterHandler(EventHanderType *pHandler)
	{
		m_HandlerList.Remove(pHandler);
	}

	/// Invokes the event.
	virtual void Invoke(const SenderType& Sender, const ParamType& Param)
	{
		for (int i = 0; i < m_HandlerList.GetCount(); ++i)
			((EventHanderType*)m_HandlerList[i])->HandleEvent(Sender, Param);
	}
};

///////////////////////////////////////////////////////////////////////////////
// CCallBackDef - Callback info definition.

template<typename CallBackType>
class CCallBackDef
{
public:
	CallBackType pProc;
	void *pParam;
public:
	CCallBackDef() { Clear(); }
	CCallBackDef(CallBackType pProc_, void *pParam_) : pProc(pProc_), pParam(pParam_) {}
	void Clear() { pProc = NULL; pParam = NULL; }
};

///////////////////////////////////////////////////////////////////////////////
// CCallBackList - Callback list.

template<typename CallBackType>
class CCallBackList
{
public:
	typedef CCallBackDef<CallBackType> CALLBACK_ITEM;
	typedef CObjectList<CALLBACK_ITEM> CALLBACK_LIST;
private:
	CALLBACK_LIST m_Items;
private:
	bool Exists(CallBackType pProc)
	{
		for (int i = 0; i < m_Items.GetCount(); i++)
			if (m_Items[i]->pProc == pProc) return true;
		return false;
	}
public:
	CCallBackList() : m_Items(true, true) {}

	void Register(CallBackType pProc, void *pParam = NULL)
	{
		if (pProc && !Exists(pProc))
			m_Items.Add(new CALLBACK_ITEM(pProc, pParam));
	}

	int GetCount() const { return m_Items.GetCount(); }
	const CALLBACK_ITEM& GetItem(int nIndex) const { return *m_Items[nIndex]; }
};

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)

/// @}
