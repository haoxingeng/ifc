// HyperLink.h : header file
//
// HyperLink static control. Will open the default browser with the given Url
// when the user clicks on the link.
//
// Copyright Chris Maunder, 1997
// Feel free to use and distribute. May not be sold for profit. 

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CHyperLink window

class CHyperLink : public CStatic
{
public:
    CHyperLink();
    virtual ~CHyperLink();

public:
    void SetUrl(LPCTSTR lpszUrl);
    CString GetUrl() const;

    void SetColors(COLORREF crLinkColor, COLORREF crVisitedColor, COLORREF crHoverColor = -1);
    COLORREF GetLinkColor() const;
    COLORREF GetVisitedColor() const;
    COLORREF GetHoverColor() const;

    void SetVisited(BOOL bVisited = TRUE);
    BOOL GetVisited() const;

    void SetLinkCursor(HCURSOR hCursor);
    HCURSOR GetLinkCursor() const;

    void SetUnderline(BOOL bUnderline = TRUE);
    BOOL GetUnderline() const;

    void SetAutoSize(BOOL bAutoSize = TRUE);
    BOOL GetAutoSize() const;

    HINSTANCE GotoUrl(LPCTSTR lpszUrl, int nShowCmd);

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void PreSubclassWindow();
protected:
    void ReportError(int nError);
    void PositionWindow();
    void SetDefaultCursor();

protected:
    COLORREF m_crLinkColor, m_crVisitedColor;       // Hyperlink colours
    COLORREF m_crHoverColor;                        // Hover colour
    BOOL     m_bOverControl;                        // cursor over control?
    BOOL     m_bVisited;                            // Has it been visited?
    BOOL     m_bUnderline;                          // underline hyperlink?
    BOOL     m_bAdjustToFit;                        // Adjust window size to fit text?
    CString  m_strUrl;                              // hyperlink Url
    CFont    m_Font;                                // Underline font if necessary
    HCURSOR  m_hLinkCursor;                         // Cursor for hyperlink
    CToolTipCtrl m_ToolTip;                         // The tooltip

protected:
    //{{AFX_MSG(CHyperLink)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnClicked();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
