#pragma once

#include "ifc.h"
#include "CodeCountingResult.h"

///////////////////////////////////////////////////////////////////////////////
// classes:

class CCodeCounter;
class CBaseCodeCntAlgo;
class CCppCodeCntAlgo;
class CPascalCodeCntAlgo;
class CJavaCodeCntAlgo;
class CCsCodeCntAlgo;

///////////////////////////////////////////////////////////////////////////////
// class CCodeCounter

class CCodeCounter
{
private:
    CStrList m_CppFileExtList;
    CStrList m_PascalFileExtList;
    CStrList m_JavaFileExtList;
    CStrList m_CsFileExtList;
private:
    void InitFileExtList();
public:
    CCodeCounter();
    ~CCodeCounter();

    void GetCppFileExtList(CStrList& List) { List = m_CppFileExtList; }
    void GetPascalFileExtList(CStrList& List) { List = m_PascalFileExtList; }
    void GetJavaFileExtList(CStrList& List) { List = m_JavaFileExtList; }
    void GetCsFileExtList(CStrList& List) { List = m_CsFileExtList; }

    bool Perform(LPCTSTR lpszFileName, CCodeCntResultVals& Result);
};

///////////////////////////////////////////////////////////////////////////////
// class CBaseCodeCntAlgo - 源码统计算法 基类

class CBaseCodeCntAlgo
{
public:
    // 注释字符类型
    enum COMMENTS_CHAR_TYPE 
    {
        CCT_LINE_CMT_SINGLE_CHAR      = 0,   // 行注释单字符。如VB的“'”
        CCT_LINE_CMT_DOUBLE_CHAR      = 1,   // 行注释双字符。如“//”
        CCT_SEG_CMT_BEGIN_SINGLE_CHAR = 2,   // 段注释起始单字符。如Delphi的“{”
        CCT_SEG_CMT_END_SINGLE_CHAR   = 3,   // 段注释终止单字符。如Delphi的“}”
        CCT_SEG_CMT_BEGIN_DOUBLE_CHAR = 4,   // 段注释起始双字符。如“/*”
        CCT_SEG_CMT_END_DOUBLE_CHAR   = 5,   // 段注释终止双字符。如“*/”
        CCT_TYPE_COUNT                = 6
    };

private:
    WCHAR m_chCmtChars[CCT_TYPE_COUNT][2];
private:
    void InitCommentsChars();
protected:
    bool IsLineBreakChar(WCHAR ch) { return (ch == 10 || ch == 13);  }
    bool IsBlankLine(LPCWSTR pStart, LPCWSTR pEnd);
    bool IsLineBreakStart(LPCWSTR pStr) { return IsLineBreakChar(pStr[0]); }
    void ScanLineBreak(LPCWSTR& pStr);
    bool CheckScanLineBreak(LPCWSTR& pStr);
    bool IsCommentsStart(LPCWSTR pStr);
    void ScanComments(LPCWSTR& pStr, int& nLines);
protected:
    virtual bool GetCommentsChars(COMMENTS_CHAR_TYPE nType, WCHAR chChars[2]) { return false; }
    virtual bool IsLiteralConstStart(LPCWSTR pStr) { return false; }
    virtual void ScanLiteralConst(LPCWSTR& pStr, int& nLines) {}
public:
    CBaseCodeCntAlgo();
    virtual ~CBaseCodeCntAlgo() {}
    void Execute(LPCWSTR lpszSource, CCodeCntResultVals& Result);
};

///////////////////////////////////////////////////////////////////////////////
// class CCppCodeCntAlgo - C/C++ 源码统计算法

class CCppCodeCntAlgo : public CBaseCodeCntAlgo
{
protected:
    virtual bool GetCommentsChars(COMMENTS_CHAR_TYPE nType, WCHAR chChars[2]);
    virtual bool IsLiteralConstStart(LPCWSTR pStr);
    virtual void ScanLiteralConst(LPCWSTR& pStr, int& nLines);
};

///////////////////////////////////////////////////////////////////////////////
// class CPascalCodeCntAlgo - Pascal 源码统计算法

class CPascalCodeCntAlgo : public CBaseCodeCntAlgo
{
protected:
    virtual bool GetCommentsChars(COMMENTS_CHAR_TYPE nType, WCHAR chChars[2]);
    virtual bool IsLiteralConstStart(LPCWSTR pStr);
    virtual void ScanLiteralConst(LPCWSTR& pStr, int& nLines);
};

///////////////////////////////////////////////////////////////////////////////
// class CJavaCodeCntAlgo - Java 源码统计算法

class CJavaCodeCntAlgo : public CCppCodeCntAlgo {};

///////////////////////////////////////////////////////////////////////////////
// class CCsCodeCntAlgo - C# 源码统计算法

class CCsCodeCntAlgo : public CCppCodeCntAlgo {};

///////////////////////////////////////////////////////////////////////////////
