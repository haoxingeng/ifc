#include "StdAfx.h"
#include "CodeCountingResult.h"

///////////////////////////////////////////////////////////////////////////////

CCodeCountingResult::CCodeCountingResult()
{
}

CCodeCountingResult::~CCodeCountingResult()
{
    Clear();
}

int CCodeCountingResult::Find(int nItemId) const
{
    ITEMID_MAP::const_iterator iter = m_IdMap.find(nItemId);
    if (iter != m_IdMap.end())
        return iter->second;
    else
        return -1;
}

void CCodeCountingResult::RebuildIdMap()
{
    m_IdMap.clear();
    for (int i = 0; i < GetCount(); i++)
        m_IdMap[ItemPtr(i)->nItemId] = i;
}

void CCodeCountingResult::Add(const CCodeCntResultItem& Item)
{
    CSingleLock SingleLock(&m_Lock, true);

    CCodeCntResultItem *pItem = new CCodeCntResultItem(Item);
    pItem->nItemId = m_SeqAlloc.AllocId();
    int nIndex = m_Items.Add(pItem);
    m_IdMap[pItem->nItemId] = nIndex;
}

void CCodeCountingResult::Remove(int nItemId)
{
    CSingleLock SingleLock(&m_Lock, true);

    int nIndex = Find(nItemId);
    if (nIndex >= 0)
    {
        delete ItemPtr(nIndex);
        m_Items.Delete(nIndex);
        RebuildIdMap();
    }
}

void CCodeCountingResult::Clear()
{
    CSingleLock SingleLock(&m_Lock, true);

    for (int i = 0; i < m_Items.GetCount(); i++)
        delete ItemPtr(i);
    m_Items.Clear();
    m_IdMap.clear();
}

int CCodeCountingResult::GetCount() const
{ 
    CSingleLock SingleLock(&m_Lock, true);
    return m_Items.GetCount(); 
}

void CCodeCountingResult::GetItem(int nIndex, CCodeCntResultItem& Item) const
{
    CSingleLock SingleLock(&m_Lock, true);

    ASSERT(nIndex >= 0 && nIndex < GetCount());
    Item = *ItemPtr(nIndex);
}

bool CCodeCountingResult::GetItemById(int nItemId, CCodeCntResultItem& Item) const
{
    CSingleLock SingleLock(&m_Lock, true);

    int nIndex = Find(nItemId);
    bool bResult = (nIndex >= 0);
    if (bResult)
        GetItem(nIndex, Item);
    return bResult;
}

CCodeCntResultVals CCodeCountingResult::GetSumVals() const
{
    CSingleLock SingleLock(&m_Lock, true);

    CCodeCntResultVals Result;
    Result.Clear();
    for (int i = 0; i < GetCount(); i++)
    {
        Result.nTotalLines += ItemPtr(i)->Values.nTotalLines;
        Result.nCommentsLines += ItemPtr(i)->Values.nCommentsLines;
        Result.nCodeLines += ItemPtr(i)->Values.nCodeLines;
        Result.nBlankLines += ItemPtr(i)->Values.nBlankLines;
    }

    return Result;
}

int SortInitiallyCompareProc(LPVOID pItem1, LPVOID pItem2)
{
    CCodeCntResultItem *p1 = (CCodeCntResultItem*)pItem1;
    CCodeCntResultItem *p2 = (CCodeCntResultItem*)pItem2;

    CString strPath1 = ExtractFilePath(p1->strFileName);
    CString strPath2 = ExtractFilePath(p2->strFileName);

    int nResult = strPath1.Compare(strPath2);
    if (nResult == 0)
    {
        CString strFileName1 = ExtractFileName(p1->strFileName);
        CString strFileName2 = ExtractFileName(p2->strFileName);
        nResult = strFileName1.Compare(strFileName2);
    }

    return nResult;
}

void CCodeCountingResult::SortInitially()
{
    m_Items.Sort(SortInitiallyCompareProc);
    RebuildIdMap();
}

///////////////////////////////////////////////////////////////////////////////

CCodeCountingResult CodeCountingResult;

///////////////////////////////////////////////////////////////////////////////
