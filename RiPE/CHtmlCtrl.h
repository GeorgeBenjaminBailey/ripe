
// webBrowserMFCAttempt3View.h : interface of the CwebBrowserMFCAttempt3View class
//

#pragma once

#include <wx/panel.h>
#include <wx/msgdlg.h>
#include <wx/dcclient.h>
#include <wx/evtloop.h>

#include "afxhtml.h"

class CHtmlCtrl : public CHtmlView
{
// Attributes
public:

// Operations
public:
    BOOL CreateFromStatic(UINT nID, CWnd* pParent);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
    CHtmlCtrl();
    DECLARE_DYNCREATE(CHtmlCtrl)
	virtual ~CHtmlCtrl();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};


// a dummy wCWnd pointing to a wxWindow's HWND
class CDummyWindow : public CWnd,
                     public wxPanel
{
public:
    CDummyWindow(wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxPanelNameStr,
        HWND hWnd = 0) : wxPanel(parent, winid, pos, size, style, name)
    {
        Attach(hWnd);
    }
    
    CDummyWindow(HWND hWnd)
    {
        Attach(hWnd);
    }

    ~CDummyWindow()
    {
        Detach();
    }

    afx_msg void OnPaint()
    {
        wxMessageBox("Hey You", "Yeah, you!");
        CString s = wxT("Hello, Windows!");
        CPaintDC dc( this );
        CRect rect;

        CWnd::GetClientRect( rect );
        dc.SetTextAlign( TA_BASELINE | TA_CENTER );
        dc.SetTextColor( ::GetSysColor( COLOR_WINDOWTEXT ) );
        dc.SetBkMode(TRANSPARENT);
        dc.TextOut( ( rect.right / 2 ), ( rect.bottom / 2 ),
            s, s.GetLength() );
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        dc.SetFont(* wxSWISS_FONT);
        dc.SetPen(* wxGREEN_PEN);
        dc.DrawLine(0, 0, 200, 200);
        dc.DrawLine(200, 0, 0, 200);

        dc.SetBrush(* wxCYAN_BRUSH);
        dc.SetPen(* wxRED_PEN);
        dc.DrawRectangle(100, 100, 100, 50);
        dc.DrawRoundedRectangle(150, 150, 100, 50, 20);

        dc.DrawEllipse(250, 250, 100, 50);
        dc.DrawLine(50, 230, 200, 230);
        dc.DrawText(wxT("This is a test string"), 50, 230);

        wxMessageBox("Hello", "Thar");
    }

    inline void SetSizer(wxSizer *sizer, bool something=true)
    {
        OnPaint();
    }

    BOOL PreTranslateMessage(MSG *msg)
    {
        wxEventLoop * const
            evtLoop = static_cast<wxEventLoop *>(wxEventLoop::GetActive());
        if ( evtLoop && evtLoop->PreProcessMessage(msg) )
            return TRUE;

        return CDummyWindow::PreTranslateMessage(msg);
    }
};
