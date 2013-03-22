#pragma once

#include "ifc.h"
#include <hash_map>

using namespace stdext;

///////////////////////////////////////////////////////////////////////////////

struct CCodeCntResultVals
{
public:
    int nTotalLines;            // 总行数
    int nCommentsLines;         // 注释行数
    int nCodeLines;             // 代码行数
    int nBlankLines;            // 空白行数
public:
    CCodeCntResultVals() { Clear(); }
    void Clear()
    {
        nTotalLines = 0;
        nCommentsLines = 0;
        nCodeLines = 0;
        nBlankLines = 0;
    }
};

struct CCodeCntResultItem
{
public:
    DWORD nItemId;              // 唯一ID
    CString strFileName;        // 文件名(含路径)
    CCodeCntResultVals Values;   // 结果数据
public:
    CCodeCntResultItem() { Clear(); }
    void Clear()
    {
        nItemId = 0;
        strFileName.Empty();
        Values.Clear();
    }
};

///////////////////////////////////////////////////////////////////////////////

class CCodeCountingResult
{
private:
    typedef hash_map<int, int> ITEMID_MAP;
private:
    CPointerList m_Items;       // (CCodeCntResultItem*)[]
    ITEMID_MAP m_IdMap;         // <nItemId, nIndex>
    CString m_strSrcPath;       // 源码路径
    CSeqAllocator m_SeqAlloc;
    mutable CCriticalSection m_Lock;
private:
    inline CCodeCntResultItem* ItemPtr(int nIndex) const
        { return (CCodeCntResultItem*)m_Items[nIndex]; }
    int Find(int nItemId) const;
    void RebuildIdMap();
public:
    CCodeCountingResult();
    ~CCodeCountingResult();

    void Add(const CCodeCntResultItem& Item);
    void Remove(int nItemId);
    void Clear();

    int GetCount() const;
    void GetItem(int nIndex, CCodeCntResultItem& Item) const;
    bool GetItemById(int nItemId, CCodeCntResultItem& Item) const;
    CCodeCntResultVals GetSumVals() const;
    void SortInitially();
    void SetSourcePath(LPCTSTR lpszValue) { m_strSrcPath = lpszValue; }
    CString GetSourcePath() const { return m_strSrcPath; }
};

///////////////////////////////////////////////////////////////////////////////

extern CCodeCountingResult CodeCountingResult;

///////////////////////////////////////////////////////////////////////////////
