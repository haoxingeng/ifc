// IrisCodeCounterDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "ifc.h"
#include "AppConfigMgr.h"
#include "CodeCountingThread.h"
#include "CodeCountingResult.h"
#include "CodeCountingAlgo.h"


// CIrisCodeCounterDlg 对话框
class CIrisCodeCounterDlg : public CDialog
{
public:
    enum { IDD = IDD_IRISCODECOUNTER_DIALOG };

private:
    HICON m_hIcon;
    CComboBox m_FileTypeComboBox;
    CComboBox m_SrcPathComboBox;
    CListCtrl m_ListCtrl;
    CCodeCountingThread *m_pCodeCntThread;
private:
    void InitFileTypeComboBox();
    void InitListCtrl();
    void LoadRecentSrcPaths();
    void SaveRecentSrcPaths();
    void AddSpecItemForSrcPathComboBox();
    void AddRecentSrcPathToComboBox();
    void UpdateUIState(bool bCounting);
    void UpdateListResultCaption(int nItemCount);
    void ClearCodeCountingResult();
    void ShowCodeCountingResult();
    void ShowCodeCountingSumVals();
    void GetCodeCountingParams(CCodeCountingParams& Params);
    void GetFileExtListFromUI(CStrList& List);
    void OutputCodeCountingResult(LPCTSTR lpszFileName);
    bool CheckValidBeforeStart();
protected:
    // 生成的消息映射函数
    afx_msg LRESULT OnCodeCntFinishedFileMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCodeCntFinishedAllMsg(WPARAM wParam, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonBrowse();
    afx_msg void OnNMRClickListResult(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnKeydownListResult(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHdnItemclickListResult(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnListCtrlRemove();
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnBnClickedButtonStop();
    afx_msg void OnBnClickedButtonOutput();
    afx_msg void OnBnClickedButtonAbout();
    afx_msg void OnBnClickedButtonExit();
    afx_msg void OnCbnSelendokComboSrcdir();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    DECLARE_MESSAGE_MAP()
protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);
public:
    CIrisCodeCounterDlg(CWnd* pParent = NULL);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
