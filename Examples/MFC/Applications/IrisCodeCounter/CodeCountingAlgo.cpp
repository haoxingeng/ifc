#include "StdAfx.h"
#include "CodeCountingAlgo.h"
#include <memory>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// class CCodeCounter

CCodeCounter::CCodeCounter()
{
    InitFileExtList();
}

CCodeCounter::~CCodeCounter()
{
}

void CCodeCounter::InitFileExtList()
{
    m_CppFileExtList.Clear();
    m_CppFileExtList.Add(TEXT(".h"));
    m_CppFileExtList.Add(TEXT(".hpp"));
    m_CppFileExtList.Add(TEXT(".c"));
    m_CppFileExtList.Add(TEXT(".cpp"));
    m_CppFileExtList.Add(TEXT(".cxx"));
    m_CppFileExtList.Add(TEXT(".inl"));

    m_PascalFileExtList.Clear();
    m_PascalFileExtList.Add(TEXT(".pas"));
    m_PascalFileExtList.Add(TEXT(".inc"));

    m_JavaFileExtList.Clear();
    m_JavaFileExtList.Add(TEXT(".java"));

    m_CsFileExtList.Clear();
    m_CsFileExtList.Add(TEXT(".cs"));
}

bool CCodeCounter::Perform(LPCTSTR lpszFileName, CCodeCntResultVals& Result)
{
    bool bResult = false;
    CBaseCodeCntAlgo *pAlgo = NULL;
    CString strFileExt = ExtractFileExt(lpszFileName);

    Result.Clear();
    if (!FileExists(lpszFileName)) return false;

    if (m_CppFileExtList.Exists(strFileExt))
        pAlgo = new CCppCodeCntAlgo();
    if (m_PascalFileExtList.Exists(strFileExt))
        pAlgo = new CPascalCodeCntAlgo();
    if (m_JavaFileExtList.Exists(strFileExt))
        pAlgo = new CJavaCodeCntAlgo();
    if (m_CsFileExtList.Exists(strFileExt))
        pAlgo = new CCsCodeCntAlgo();

    if (pAlgo != NULL)
    {
        auto_ptr<CBaseCodeCntAlgo> AutoPtr(pAlgo);
        try
        {
            CBuffer Buffer;
            Buffer.LoadFromFile(lpszFileName);
            CStringA strSourceA;
            strSourceA.SetString(Buffer.Data(), Buffer.GetSize());
            CStringW strSourceW(strSourceA);
            
            pAlgo->Execute(strSourceW, Result);
            bResult = true;
        }
        catch (CException *e)
        {
            e->Delete();
        }
    }

    return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// class CBaseCodeCntAlgo

CBaseCodeCntAlgo::CBaseCodeCntAlgo()
{
}

void CBaseCodeCntAlgo::InitCommentsChars()
{
    for (int i = 0; i < CCT_TYPE_COUNT; i++)
        m_chCmtChars[i][0] = m_chCmtChars[i][1] = 0;
    for (int i = 0; i < CCT_TYPE_COUNT; i++)
        GetCommentsChars((COMMENTS_CHAR_TYPE)i, m_chCmtChars[i]);
}

bool CBaseCodeCntAlgo::IsBlankLine(LPCWSTR pStart, LPCWSTR pEnd)
{
    ASSERT(pStart != NULL && pEnd != NULL);

    for (LPCWSTR p = pStart; p < pEnd; p++)
        if (*p > 32) return false;

    return true;
}

void CBaseCodeCntAlgo::ScanLineBreak(LPCWSTR& pStr)
{
    ASSERT(IsLineBreakChar(pStr[0]));

    LPCWSTR pStart = pStr;
    pStr++;
    if (IsLineBreakChar(pStart[1]) && pStart[0] != pStart[1])
        pStr++;
}

bool CBaseCodeCntAlgo::CheckScanLineBreak(LPCWSTR& pStr)
{
    bool bResult = IsLineBreakStart(pStr);
    if (bResult)
        ScanLineBreak(pStr);
    return bResult;
}

bool CBaseCodeCntAlgo::IsCommentsStart(LPCWSTR pStr)
{
    WCHAR *p;

    p = m_chCmtChars[CCT_LINE_CMT_SINGLE_CHAR];
    if (p[0] != 0 && pStr[0] == p[0])
        return true;

    p = m_chCmtChars[CCT_LINE_CMT_DOUBLE_CHAR];
    if (p[0] != 0 && pStr[0] == p[0] && pStr[1] == p[1])
        return true;

    p = m_chCmtChars[CCT_SEG_CMT_BEGIN_SINGLE_CHAR];
    if (p[0] != 0 && pStr[0] == p[0])
        return true;

    p = m_chCmtChars[CCT_SEG_CMT_BEGIN_DOUBLE_CHAR];
    if (p[0] != 0 && pStr[0] == p[0] && pStr[1] == p[1])
        return true;

    return false;
}

void CBaseCodeCntAlgo::ScanComments(LPCWSTR& pStr, int& nLines)
{
    ASSERT(IsCommentsStart(pStr));

    WCHAR *p, *q;
    nLines = 0;

    p = m_chCmtChars[CCT_LINE_CMT_SINGLE_CHAR];
    if (p[0] != 0 && pStr[0] == p[0])
    {
        while (*pStr != NULL && !IsLineBreakChar(*pStr)) pStr++;
        CheckScanLineBreak(pStr);
        nLines = 1;
        return;
    }

    p = m_chCmtChars[CCT_LINE_CMT_DOUBLE_CHAR];
    if (p[0] != 0 && pStr[0] == p[0] && pStr[1] == p[1])
    {
        while (*pStr != NULL && !IsLineBreakChar(*pStr)) pStr++;
        CheckScanLineBreak(pStr);
        nLines = 1;
        return;
    }

    p = m_chCmtChars[CCT_SEG_CMT_BEGIN_SINGLE_CHAR];
    q = m_chCmtChars[CCT_SEG_CMT_END_SINGLE_CHAR];
    if (p[0] != 0 && q[0] != 0 && pStr[0] == p[0])
    {
        while (*pStr != NULL && *pStr != q[0])
        {
            if (CheckScanLineBreak(pStr))
                nLines++;
            else
                pStr++;
        }

        if (*pStr != NULL) pStr++;
        if (*pStr == NULL) nLines++;

        if (CheckScanLineBreak(pStr))
            nLines++;

        return;
    }

    p = m_chCmtChars[CCT_SEG_CMT_BEGIN_DOUBLE_CHAR];
    q = m_chCmtChars[CCT_SEG_CMT_END_DOUBLE_CHAR];
    if (p[0] != 0 && q[0] != 0 && 
        pStr[0] == p[0] && pStr[1] == p[1])
    {
        pStr += 2;
        while (*pStr != NULL && (pStr[0] != q[0] || pStr[1] != q[1]))
        {
            if (CheckScanLineBreak(pStr))
                nLines++;
            else
                pStr++;
        }

        if (*pStr != NULL) pStr += 2;
        if (*pStr == NULL) nLines++;

        if (CheckScanLineBreak(pStr))
            nLines++;

        return;
    }
}

void CBaseCodeCntAlgo::Execute(LPCWSTR lpszSource, CCodeCntResultVals& Result)
{
    LPCWSTR pStr = lpszSource;
    LPCWSTR pPrevLineBreak = pStr;
    int nDupLines = 0;

    Result.Clear();
    InitCommentsChars();

    while (*pStr != 0)
    {
        if (IsLineBreakStart(pStr))
        {
            bool bIsBlankLine = IsBlankLine(pPrevLineBreak, pStr);
            pPrevLineBreak = pStr;

            ScanLineBreak(pStr);

            if (bIsBlankLine)
                Result.nBlankLines++;
            else
                Result.nCodeLines++;
        }
        else if (IsLiteralConstStart(pStr))
        {
            int nLines = 0;
            ScanLiteralConst(pStr, nLines);
            pPrevLineBreak = pStr;
            Result.nCodeLines += nLines;
        }
        else if (IsCommentsStart(pStr))
        {
            int nLines = 0;
            ScanComments(pStr, nLines);
            pPrevLineBreak = pStr;
            Result.nCommentsLines += nLines;
        }
        else
        {
            while (*pStr != NULL)
            {
                if (IsCommentsStart(pStr))
                {
                    // 考虑 “code//comments”的情况
                    Result.nCodeLines++;
                    nDupLines++;
                    break;
                }
                else if (IsLiteralConstStart(pStr) || IsLineBreakStart(pStr))
                    break;
                else
                    pStr++;
            }
        }

        if (*pStr == 0 && pStr > pPrevLineBreak)
        {
            if (IsBlankLine(pPrevLineBreak, pStr))
                Result.nBlankLines++;
            else
                Result.nCodeLines++;
        }
    }

    Result.nTotalLines = 
        Result.nCodeLines + Result.nCommentsLines + Result.nBlankLines - nDupLines;
}

///////////////////////////////////////////////////////////////////////////////
// class CCppCodeCntAlgo

bool CCppCodeCntAlgo::GetCommentsChars(COMMENTS_CHAR_TYPE nType, WCHAR chChars[2])
{
    switch (nType)
    {
    case CCT_LINE_CMT_SINGLE_CHAR:
        return false;
    case CCT_LINE_CMT_DOUBLE_CHAR:    
        chChars[0] = '/';
        chChars[1] = '/';
        return true;
    case CCT_SEG_CMT_BEGIN_SINGLE_CHAR:
        return false;
    case CCT_SEG_CMT_END_SINGLE_CHAR:
        return false;
    case CCT_SEG_CMT_BEGIN_DOUBLE_CHAR:
        chChars[0] = '/';
        chChars[1] = '*';
        return true;
    case CCT_SEG_CMT_END_DOUBLE_CHAR:
        chChars[0] = '*';
        chChars[1] = '/';
        return true;
    default:
        return false;
    }
}

bool CCppCodeCntAlgo::IsLiteralConstStart(LPCWSTR pStr)
{
    return (pStr[0] == '"');
}

void CCppCodeCntAlgo::ScanLiteralConst(LPCWSTR& pStr, int& nLines)
{
    nLines = 0;

    pStr++;
    while (*pStr != NULL)
    {
        if (*pStr == '"')
        {
            // 若 '"' 前的 '\' 数量是双数，则说明此引号是字符串定界符
            int n = 0;
            while (*(pStr - n - 1) == '\\') n++;
            if ((n % 2) == 0)
            {
                pStr++;
                break;
            }
            else
                pStr++;
        }
        else
        {
            if (CheckScanLineBreak(pStr))
                nLines++;
            else
                pStr++;
        }
    }

    if (*pStr == NULL) nLines++;

    if (CheckScanLineBreak(pStr))
        nLines++;
}

///////////////////////////////////////////////////////////////////////////////
// class CPascalCodeCntAlgo

bool CPascalCodeCntAlgo::GetCommentsChars(COMMENTS_CHAR_TYPE nType, WCHAR chChars[2])
{
    switch (nType)
    {
    case CCT_LINE_CMT_SINGLE_CHAR:
        return false;
    case CCT_LINE_CMT_DOUBLE_CHAR:    
        chChars[0] = '/';
        chChars[1] = '/';
        return true;
    case CCT_SEG_CMT_BEGIN_SINGLE_CHAR:
        chChars[0] = '{';
        return true;
    case CCT_SEG_CMT_END_SINGLE_CHAR:
        chChars[0] = '}';
        return true;
    case CCT_SEG_CMT_BEGIN_DOUBLE_CHAR:
        chChars[0] = '(';
        chChars[1] = '*';
        return true;
    case CCT_SEG_CMT_END_DOUBLE_CHAR:
        chChars[0] = '*';
        chChars[1] = ')';
        return true;
    default:
        return false;
    }
}

bool CPascalCodeCntAlgo::IsLiteralConstStart(LPCWSTR pStr)
{
    return (pStr[0] == '\'');
}

void CPascalCodeCntAlgo::ScanLiteralConst(LPCWSTR& pStr, int& nLines)
{
    ASSERT(IsLiteralConstStart(pStr));

    nLines = 0;

    pStr++;
    while (*pStr != NULL)
    {
        if (*pStr == '\'')
        {
            pStr++;
            if (*pStr == '\'') 
                pStr++;
            else
                break;
        }
        else
        {
            if (CheckScanLineBreak(pStr))
                nLines++;
            else
                pStr++;
        }
    }

    if (*pStr == NULL) nLines++;

    if (CheckScanLineBreak(pStr))
        nLines++;
}

///////////////////////////////////////////////////////////////////////////////
