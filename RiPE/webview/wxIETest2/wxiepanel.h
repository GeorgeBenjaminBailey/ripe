/////////////////////////////////////////////////////////////////////////////
// Name:        wxiepanel.h
// Purpose:     wxMSW wxIEPanel class declaration
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXIEPANEL_H
#define WXIEPANEL_H

#ifdef __WXMSW__

// TODO: define this in setup.h ?
#define wxHAVE_WEB_BACKEND_IE 1

#include "wx/control.h"
#include "../webview.h"
#include "wx/msw/ole/automtn.h"
#include "wx/msw/ole/activex.h"

#include <wx/msgdlg.h>

#if wxUSE_ACTIVEX != 1
#error "wxUSE_ACTIVEX must be activated for this to work"
#endif

#if wxUSE_OLE_AUTOMATION != 1
#error "wxUSE_OLE_AUTOMATION must be activated for this to work"
#endif

// FIXME: get those DLL export macros right...

class wxIEPanel : public wxWebView
{
    wxActiveXContainer* m_container;
    wxAutomationObject m_ie;
    IWebBrowser2* m_web_browser;
    DWORD m_dwCookie;
    bool m_can_navigate_back;
    bool m_can_navigate_forward;
    
	/** The "Busy" property of IWebBrowser2 does not always return busy when we'd want it to;
	 *  this variable may be set to true in cases where the Busy property is false but should
	 * be true.
	 */
	bool m_is_busy;
	
public:

    wxIEPanel() {}
	
    wxIEPanel(wxWindow* parent,
           wxWindowID id,
           const wxString& url = "about:blank",
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = "wxIEPanel")
   {
	   Create(parent, id, url, pos, size, style, name);
   }
    
	bool Create(wxWindow* parent,
           wxWindowID id,
           const wxString& url = "about:blank",
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = "wxIEPanel");
	
    virtual void LoadUrl(wxString url);
    
    virtual bool CanGoForward() { return m_can_navigate_forward; }
    virtual bool CanGoBack() { return m_can_navigate_back; }
    virtual void GoBack();
    virtual void GoForward();
    virtual void Stop();
    virtual void Reload(bool bypassCache=false);
    
    virtual wxString GetPageSource();
    
    // Here's how to do it if ever needed...
    /*
    void RunScript(wxString script)
    {
        IHTMLDocument2.parentWindow.execScript()
        
        // OR
        
        IHTMLDocument* pHTMLDoc = ...;

        DISPID idMethod = 0;
        OLECHAR FAR* sMethod = L"DoSomething";
        IDispatch* pScript = 0;
        pHTMLDoc->get_Script(&pScript);
        HRESULT hr = pScript->GetIDsOfNames(IID_NULL, &sMethod, 1, LOCALE_SYSTEM_DEFAULT,
                                            &idSave);
        if (SUCCEEDED(hr)) {
          // invoke assuming no method parameters
          DISPPARAMS dpNoArgs = {NULL, NULL, 0, 0};
          hr = pScript->Invoke(idSave, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
                               &dpNoArgs, NULL, NULL, NULL);
        }
        pScript->Release();
        pHTMLDoc->Release(); 
    }
    */
    
    virtual bool IsBusy();
    virtual wxString GetCurrentURL();
    virtual wxString GetCurrentTitle();

	virtual void SetZoomType(const wxWebViewZoomType);
	virtual wxWebViewZoomType GetZoomType() const;
	virtual bool CanSetZoomType(wxWebViewZoomType) const;

    virtual void Print();

    virtual void SetPage(wxString html, wxString baseUrl);

    virtual wxWebViewZoom GetZoom();
    virtual void SetZoom(wxWebViewZoom zoom);
	
    // In case I need to implement this, a little tip
    //int GetScrollPos()
    //{
    //    http://www.ureader.com/msg/1517840.aspx
    //    return document.body.scrollTop
    //    (IWebBrowser2::get_Document()->get_body()->get_scrollTop (or put_scrollTop)
    //}

    int GetScrollPos();


	// ---- IE-specific methods

    // FIXME: I seem to be able to access remote webpages even in offline mode...
    bool IsOfflineMode();
    void SetOfflineMode(bool offline);
    
    /**
     * Get text zoom
     * @return text zoom from 0 to 4
     */
    int GetIETextZoom();
    
    /**
     *  @param level 0 to 4
     */
    void SetIETextZoom(int level);

	void SetIEOpticalZoom(float zoom);
	float GetIEOpticalZoom();

    void onActiveXEvent(wxActiveXEvent& evt);
    void onEraseBg(wxEraseEvent& evt) {}
	
    DECLARE_EVENT_TABLE();
};

#else
// TODO: define this in setup.h ?
#define wxHAVE_WEB_BACKEND_IE 0
#endif

#endif // WXIEPANEL_H
