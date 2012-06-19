/////////////////////////////////////////////////////////////////////////////
// Name:        wxgtkwebkitctrl.h
// Purpose:     GTK webkit backend for web view component
// Author:      Robert Roebling, Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 200 Marianne Gagnon, 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_WEBKITCTRL_H_
#define _WX_GTK_WEBKITCTRL_H_

#ifdef __WXGTK__
// TODO: define this in setup.h ?
#define wxHAVE_WEB_BACKEND_GTK_WEBKIT 1

#include "webview.h"

//-----------------------------------------------------------------------------
// wxGtkWebKitCtrl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGtkWebKitCtrl : public wxWebView
{
public:
    wxGtkWebKitCtrl() { Init(); }
	
    wxGtkWebKitCtrl(wxWindow *parent,
		   wxWindowID id = wxID_ANY,
           const wxString& url = "about:blank",
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxString& name = wxT("wxGtkWebKitCtrl"))
    {
        Init();

        Create(parent, id, url, pos, size, style, name);
    }

    virtual bool Create(wxWindow *parent,
		   wxWindowID id = wxID_ANY,
           const wxString& url = "about:blank",
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxString& name = wxT("wxGtkWebKitCtrl"));
		   
    virtual bool Enable( bool enable = true );

    // implementation
    // --------------

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // helper to allow access to protected member from GTK callback
    void MoveWindow(int x, int y, int width, int height) { DoMoveWindow(x, y, width, height); }

    // called from GTK callbacks: they update the button state and call
    // GTKUpdateBitmap()
	/*
    void GTKMouseEnters();
    void GTKMouseLeaves();
    void GTKPressed();
    void GTKReleased();
	*/

	void zoomIn();
	void zoomOut();
	void SetWebkitZoom(float level);
	float GetWebkitZoom();
	
	virtual void Stop();
	virtual void LoadUrl(wxString);
	virtual void GoBack();
	virtual void GoForward();
	virtual void Reload(bool bypasscache = false);
	virtual bool CanGoBack();
	virtual bool CanGoForward();
	virtual wxString GetCurrentURL();
	virtual wxString GetCurrentTitle();
	virtual wxString GetPageSource();
	virtual void SetPage(wxString html, wxString baseUrl);
	virtual void Print();
	virtual bool IsBusy();
	
	void SetZoomType(wxWebViewZoomType);
	wxWebViewZoomType GetZoomType() const;
	bool CanSetZoomType(wxWebViewZoomType) const;
	virtual wxWebViewZoom GetZoom();
	virtual void SetZoom(wxWebViewZoom);
	


	/** FIXME: hack to work around signals being received too early */
	bool m_ready;


	/** TODO: check if this can be made private
	 * The native control has a getter to check for busy state, but except in very recent
	 * versions of webkit this getter doesn't say everything we need (namely it seems to
	 * stay indefinitely busy when loading is cancelled by user) */
	bool m_busy;
	
protected:

    virtual void DoApplyWidgetStyle(GtkRcStyle *style);

    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

//    virtual wxBitmap DoGetBitmap(State which) const;
//    virtual void DoSetBitmap(const wxBitmap& bitmap, State which);
//    virtual void DoSetBitmapPosition(wxDirection dir);

private:
	
    // focus event handler: calls GTKUpdateBitmap()
    void GTKOnFocus(wxFocusEvent& event);

    GtkWidget *web_view;

	// FIXME: try to get DECLARE_DYNAMIC_CLASS macros & stuff right
    //DECLARE_DYNAMIC_CLASS(wxGtkWebKitCtrl)
};

typedef wxGtkWebKitCtrl wxBetterHTMLControl;

#else

// TODO: define this in setup.h ?
#define wxHAVE_WEB_BACKEND_GTK_WEBKIT 0

#endif

#endif // _WX_GTK_BUTTON_H_
