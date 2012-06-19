/////////////////////////////////////////////////////////////////////////////
// Name:        wx/html/webkit.h
// Purpose:     wxOSXWebKitCtrl - embeddable web kit control,
//                             OS X implementation of web view component
// Author:      Jethro Grassie / Kevin Ollivier / Marianne Gagnon
// Modified by:
// Created:     2004-4-16
// RCS-ID:      $Id: webkit.h 64533 2010-06-09 14:28:08Z FM $
// Copyright:   (c) Jethro Grassie / Kevin Ollivier / Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBKIT_H
#define _WX_WEBKIT_H

#include "wx/setup.h"

#if wxUSE_WEBKIT && (defined(__WXMAC__) || defined(__WXCOCOA__))

// TODO: define this in setup.h ?
#define wxHAVE_WEB_BACKEND_OSX_WEBKIT 1

#include "wx/control.h"
#include "webview.h"

// ----------------------------------------------------------------------------
// Web Kit Control
// ----------------------------------------------------------------------------

extern WXDLLIMPEXP_DATA_CORE(const char) wxOSXWebKitCtrlNameStr[];

class WXDLLIMPEXP_CORE wxOSXWebKitCtrl : public wxWebView
{
public:
    DECLARE_DYNAMIC_CLASS(wxOSXWebKitCtrl)

    wxOSXWebKitCtrl() {};
    wxOSXWebKitCtrl(wxWindow *parent,
                    wxWindowID winID = wxID_ANY,
                    const wxString& strURL = "about:blank",
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxString& name = wxOSXWebKitCtrlNameStr)
    {
        Create(parent, winID, strURL, pos, size, style, name);
    };
    bool Create(wxWindow *parent,
                wxWindowID winID = wxID_ANY,
                const wxString& strURL = "about:blank",
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxString& name = wxOSXWebKitCtrlNameStr);
    virtual ~wxOSXWebKitCtrl();

    void InternalLoadURL(const wxString &url);

    virtual bool CanGoBack();
    virtual bool CanGoForward();
    virtual void GoBack();
    virtual void GoForward();
    virtual void Reload(bool bypassCache=false);
    virtual void Stop();
    virtual wxString GetPageSource();
    virtual void SetPageTitle(const wxString& title) { m_pageTitle = title; }
    virtual wxString GetPageTitle(){ return m_pageTitle; }

    virtual void SetPage(wxString html, wxString baseUrl);

    virtual void Print();
    
    virtual void LoadUrl(wxString);
    virtual wxString GetCurrentURL();
    virtual wxString GetCurrentTitle();
    virtual wxWebViewZoom GetZoom();
    virtual void SetZoom(wxWebViewZoom zoom);
    
    virtual void SetZoomType(const wxWebViewZoomType zoomType);
    virtual wxWebViewZoomType GetZoomType() const;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const;

    virtual bool IsBusy() { return m_busy; }

    // ---- methods not from the parent (common) interface
    wxString GetSelectedText();
    
    wxString RunScript(const wxString& javascript);

    bool  CanGetPageSource();

    void  SetScrollPos(int pos);
    int   GetScrollPos();
    
    void  MakeEditable(bool enable = true);
    bool  IsEditable();
    
    wxString GetSelection();

    bool  CanIncreaseTextSize();
    void  IncreaseTextSize();
    bool  CanDecreaseTextSize();
    void  DecreaseTextSize();

    float GetWebkitZoom();
    void  SetWebkitZoom(float zoom);

    // don't hide base class virtuals
    virtual void SetScrollPos( int orient, int pos, bool refresh = true )
        { return wxControl::SetScrollPos(orient, pos, refresh); }
    virtual int GetScrollPos( int orient ) const
        { return wxControl::GetScrollPos(orient); }

    //we need to resize the webview when the control size changes
    void OnSize(wxSizeEvent &event);
    void OnMove(wxMoveEvent &event);
    void OnMouseEvents(wxMouseEvent &event);

    bool m_busy;
    
protected:
    DECLARE_EVENT_TABLE()
    void MacVisibilityChanged();

private:
    wxWindow *m_parent;
    wxWindowID m_windowID;
    wxString m_pageTitle;

    struct objc_object *m_webView;

    // we may use this later to setup our own mouse events,
    // so leave it in for now.
    void* m_webKitCtrlEventHandler;
    //It should be WebView*, but WebView is an Objective-C class
    //TODO: look into using DECLARE_WXCOCOA_OBJC_CLASS rather than this.
};

// ----------------------------------------------------------------------------
// Web Kit Events
// ----------------------------------------------------------------------------

enum {
    wxWEBKIT_STATE_START = 1,
    wxWEBKIT_STATE_NEGOTIATING = 2,
    wxWEBKIT_STATE_REDIRECTING = 4,
    wxWEBKIT_STATE_TRANSFERRING = 8,
    wxWEBKIT_STATE_STOP = 16,
        wxWEBKIT_STATE_FAILED = 32
};

enum {
    wxWEBKIT_NAV_LINK_CLICKED = 1,
    wxWEBKIT_NAV_BACK_NEXT = 2,
    wxWEBKIT_NAV_FORM_SUBMITTED = 4,
    wxWEBKIT_NAV_RELOAD = 8,
    wxWEBKIT_NAV_FORM_RESUBMITTED = 16,
    wxWEBKIT_NAV_OTHER = 32

};

#else
// TODO: define this in setup.h ?
#define wxHAVE_WEB_BACKEND_OSX_WEBKIT 0

#endif // wxUSE_WEBKIT

#endif // _WX_WEBKIT_H_
