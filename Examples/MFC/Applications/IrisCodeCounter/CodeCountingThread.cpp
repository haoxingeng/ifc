#include "StdAfx.h"
#include "CodeCountingThread.h"
#include "CodeCountingResult.h"

///////////////////////////////////////////////////////////////////////////////

CCodeCountingThread::CCodeCountingThread(const CCodeCountingParams& Params) :
    m_Params(Params)
{
    SetFreeOnTerminate(true);
}

CCodeCountingThread::~CCodeCountingThread()
{
}

void CCodeCountingThread::ScanDir(LPCTSTR lpszPath)
{
    CString strPath(PathWithSlash(lpszPath));
    CFileFind Finder;

    BOOL bWorking = Finder.FindFile(strPath + TEXT("*.*"));
    while (bWorking && !GetTerminated())
    {
        bWorking = Finder.FindNextFile();

        if (Finder.IsDots())
            continue;

        if (Finder.IsDirectory())
        {
            if (m_Params.bRecurseSubDir)
                ScanDir(Finder.GetFilePath());
        }
        else
        {
            CString strFileName = strPath + Finder.GetFileName();
            CString strFileExt = ExtractFileExt(strFileName);
            if (m_Params.FileExtList.Exists(strFileExt))
                AnalyzeFile(strFileName);
        }
    }

    Finder.Close();
}

void CCodeCountingThread::AnalyzeFile(LPCTSTR lpszFileName)
{
    CCodeCntResultItem Item;
    Item.strFileName = lpszFileName;
    m_CodeCounter.Perform(lpszFileName, Item.Values);

    CodeCountingResult.Add(Item);
    NotifyFinishedFile();
}

void CCodeCountingThread::NotifyFinishedFile()
{
    PostMessage(m_Params.hMsgRecver, WM_CODECNT_FINISHED_FILE, 0, 0);
}

void CCodeCountingThread::NotifyFinishedAll()
{
    PostMessage(m_Params.hMsgRecver, WM_CODECNT_FINISHED_ALL, 0, 0);
}

void CCodeCountingThread::Execute()
{
    CodeCountingResult.Clear();
    CodeCountingResult.SetSourcePath(m_Params.strPath);
    ScanDir(m_Params.strPath);
    CodeCountingResult.SortInitially();
}

void CCodeCountingThread::AfterExecute()
{
    NotifyFinishedAll();
}
