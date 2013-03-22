#pragma once

#include "ifc.h"
#include "CodeCountingAlgo.h"

///////////////////////////////////////////////////////////////////////////////

// 刚统计完一个文件
const int WM_CODECNT_FINISHED_FILE   = WM_USER + 101;
// 全部统计完毕
const int WM_CODECNT_FINISHED_ALL    = WM_USER + 102;

///////////////////////////////////////////////////////////////////////////////

struct CCodeCountingParams
{
    CStrList FileExtList;   // 文件扩展名列表(如: ".c", ".cpp" ...)
    CString strPath;        // 路径
    bool bRecurseSubDir;    // 是否递归包含子目录
    HWND hMsgRecver;        // 消息接收窗口
};

///////////////////////////////////////////////////////////////////////////////

class CCodeCountingThread : public CThread
{
private:
    CCodeCountingParams m_Params;
    CCodeCounter m_CodeCounter;
private:
    void ScanDir(LPCTSTR lpszPath);
    void AnalyzeFile(LPCTSTR lpszFileName);
    void NotifyFinishedFile();
    void NotifyFinishedAll();
protected:
    virtual void Execute();
    virtual void AfterExecute();
public:
    CCodeCountingThread(const CCodeCountingParams& Params);
    virtual ~CCodeCountingThread();
};
