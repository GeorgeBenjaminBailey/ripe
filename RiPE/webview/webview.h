/////////////////////////////////////////////////////////////////////////////
// Name:        webview.h
// Purpose:     Common interface and events for web view component
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_WEB_VIEW_H__
#define __WX_WEB_VIEW_H__

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/sstream.h>

/**
 * Zoom level in web view component
 */
enum wxWebViewZoom
{
    wxWEB_VIEW_ZOOM_TINY,
    wxWEB_VIEW_ZOOM_SMALL,
    wxWEB_VIEW_ZOOM_MEDIUM,
    wxWEB_VIEW_ZOOM_LARGE,
    wxWEB_VIEW_ZOOM_LARGEST
};

/**
 * The type of zooming that the web view control can perform
 */
enum wxWebViewZoomType
{
    /** The entire layout scales when zooming, including images */
    wxWEB_VIEW_ZOOM_TYPE_LAYOUT,
    /** Only the text changes in size when zooming, images and other layout elements retain their initial size */
    wxWEB_VIEW_ZOOM_TYPE_TEXT
};

/** Types of errors that can cause navigation to fail */
enum wxWebNavigationError
{
    /** Connection error (timeout, etc.) */
    wxWEB_NAV_ERR_CONNECTION = 1,
    /** Invalid certificate */
    wxWEB_NAV_ERR_CERTIFICATE = 2,
    /** Authentication required */
    wxWEB_NAV_ERR_AUTH = 3,
    /** Other security error */
    wxWEB_NAV_ERR_SECURITY = 4,
    /** Requested resource not found */
    wxWEB_NAV_ERR_NOT_FOUND = 5,
    /** Invalid request/parameters (e.g. bad URL, bad protocol, unsupported resource type) */
    wxWEB_NAV_ERR_REQUEST = 6,
    /** The user cancelled (e.g. in a dialog) */
    wxWEB_NAV_ERR_USER_CANCELLED = 7,
    /** Another (exotic) type of error that didn't fit in other categories*/
    wxWEB_NAV_ERR_OTHER = 8
};

/**
 * List of available backends for wxWebView
 */
enum wxWebViewBackend
{
    /** Value that may be passed to wxWebView to let it pick an appropriate engine
     * for the current platform*/
    wxWEB_VIEW_BACKEND_DEFAULT,
    
    /** The OSX-native WebKit web engine */
    wxWEB_VIEW_BACKEND_OSX_WEBKIT,
    
    /** The GTK port of the WebKit engine */
    wxWEB_VIEW_BACKEND_GTK_WEBKIT,
    
    /** Use Microsoft Internet Explorer as web engine */
    wxWEB_VIEW_BACKEND_IE
};

/** 
  * @class wxBetterHTMLControl
  * 
  * This control may be used to render HTML documents.
  * Capabilities of the HTML renderer will depend upon the backend.
  * TODO: describe each backend and its capabilities here
  * 
  * Note that errors are generally reported asynchronously though the wxEVT_COMMAND_WEB_VIEW_ERROR
  * event described below.
  * 
  * @beginEventEmissionTable{wxWebNavigationEvent}
  * @event{EVT_BUTTON(id, func)}
  * 
  * @event{EVT_WEB_VIEW_NAVIGATING(id, func)}
  * Process a wxEVT_COMMAND_WEB_VIEW_NAVIGATING event, generated before trying to get a resource.
  * This event may be vetoed to prevent navigating to this resource. Note that if the displayed
  * HTML document has several frames, one such event will be generated per frame.
  * 
  * @event{EVT_WEB_VIEW_NAVIGATED(id, func)}
  * Process a wxEVT_COMMAND_WEB_VIEW_NAVIGATED event generated after it was confirmed that a resource
  * would be requested. This event may not be vetoed. Note that if the displayed
  * HTML document has several frames, one such event will be generated per frame.
  * 
  * @event{EVT_WEB_VIEW_LOADED(id, func)}
  * Process a wxEVT_COMMAND_WEB_VIEW_LOADED event generated when the document is fully
  * loaded and displayed.
  * 
  * @event{EVT_WEB_VIEW_ERRROR(id, func)}
  * Process a wxEVT_COMMAND_WEB_VIEW_ERROR event generated when a navigation error occurs.
  * The integer associated with this event will be a wxWebNavigationError item.
  * The string associated with this event may contain a backend-specific more precise error message/code.
  * 
  * @endEventTable
  */
class wxWebView : public wxControl
{
public:
    
    /**
     *  Creation function for two-step creation. 
     */
	virtual bool Create(wxWindow* parent,
           wxWindowID id,
           const wxString& url,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name) = 0;
    
    /**
     * Factory function to create a new wxWebView for two-step creation (you need to call wxWebView::Create
     * on the returned object)
     * @param backend which web engine to use as backend for wxWebView
     * @return the created wxWebView, or NULL if the requested backend is not available
     */
    static wxWebView* GetNew(wxWebViewBackend backend = wxWEB_VIEW_BACKEND_DEFAULT);
    
    /**
     * Factory function to create a new wxWebView
     * @param parent parent window to create this view in
     * @param id ID of this control
     * @param url URL to load by default in the web view
     * @param pos position to create this control at (you may use wxDefaultPosition if you use sizers)
     * @param size size to create this control with (you may use wxDefaultSize if you use sizers)
     * @param backend which web engine to use as backend for wxWebView
     * @return the created wxWebView, or NULL if the requested backend is not available
     */
    static wxWebView* GetNew(wxWindow* parent,
           wxWindowID id,
           const wxString& url = "about:blank",
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           wxWebViewBackend backend = wxWEB_VIEW_BACKEND_DEFAULT,
           long style = 0,
           const wxString& name = "wxWebView");
    
    
    /** Get whether it is possible to navigate back in the history of visited pages */
    virtual bool CanGoBack() = 0;
    
    /** Get whether it is possible to navigate forward in the history of visited pages */
    virtual bool CanGoForward() = 0;
    
    /** Navigate back in the history of visited pages. Only valid if CanGoBack() returned true. */
    virtual void GoBack() = 0;
    
    /** Navigate forwardin the history of visited pages. Only valid if CanGoForward() returned true. */
    virtual void GoForward() = 0;
    
    /**
     * Load a HTMl document (web page) from a URL
     * @param url the URL where the HTML document to display can be found
     * @return whether opening the URL was a success.
     */
    virtual void LoadUrl(wxString url) = 0;

    /**
     * Stop the current page loading process, if any.
     * May trigger an error event of type wxWEB_NAV_ERR_USER_CANCELLED.
     * TODO: make wxWEB_NAV_ERR_USER_CANCELLED errors uniform across ports.
     */
    virtual void Stop() = 0;
    
    /**
     * Reload the currently displayed URL.
     * @param bypassCache if true, the reloading process should ignore the contents of the cache
     *        and re-fetch all data anew
     */
    virtual void Reload(bool bypassCache=false) = 0;
    
    
    /**
     * Get the URL of the currently displayed document
     */
    virtual wxString GetCurrentURL() = 0;
    
    /**
     * Get the title of the current web page, or its URL/path if title is not available
     */
    virtual wxString GetCurrentTitle() = 0;
    
    // TODO: handle choosing a frame when calling GetPageSource()?
    /**
     * Get the HTML source code of the currently displayed document
     * @return the HTML source code, or an empty string if no page is currently shown
     */
    virtual wxString GetPageSource() = 0;
    
     /**
      * Get the scroll position of the frame.
      * @return the scroll position
      */
    virtual int GetScrollPos() = 0;

   /**
     * Get the zoom factor of the page
     * @return How much the HTML document is zoomed (scaleed)
     */
    virtual wxWebViewZoom GetZoom() = 0;
    
    /**
     * Set the zoom factor of the page
     * @param zoom How much to zoom (scale) the HTML document
     */
    virtual void SetZoom(wxWebViewZoom zoom) = 0;
    
    /**
     * Set how to interpret the zoom factor
     * @param zoomType how the zoom factor should be interpreted by the HTML engine
     * @note invoke canSetZoomType() first, some HTML renderers may not support all zoom types
     */
    virtual void SetZoomType(const wxWebViewZoomType zoomType) = 0;
    
    /**
     * Get how the zoom factor is currently interpreted
     * @return how the zoom factor is currently interpreted by the HTML engine
     */
    virtual wxWebViewZoomType GetZoomType() const = 0;
    
    /**
     * Retrieve whether the current HTML engine supports a type of zoom
     * @param type the type of zoom to test
     * @return whether this type of zoom is supported by this HTML engine (and thus can be set
     * through setZoomType())
     */
    virtual bool CanSetZoomType(wxWebViewZoomType type) const = 0;
    
    // TODO: allow 'SetPage' to find files (e.g. images) from a virtual file system if possible
    /**
     * Set the displayed page source to the contents of the given string
     * @param html the string that contains the HTML data to display
     * @param baseUrl URL assigned to the HTML data, to be used to resolve relative paths, for instance
     */
    virtual void SetPage(wxString html, wxString baseUrl) = 0;
    
    /**
     * Set the displayed page source to the contents of the given stream
     * @param html the stream to read HTML data from
     * @param baseUrl URL assigned to the HTML data, to be used to resolve relative paths, for instance
     */
    virtual void SetPage(wxInputStream& html, wxString baseUrl)
    {
        wxStringOutputStream stream;
        stream.Write(html);
        SetPage(stream.GetString(), baseUrl);
    }
    
    // TODO:
    //     wxString GetSelection();                         // maybe?
    //     void SetSelection(...);                          // maybe?
    
    //     void MakeEditable(bool enable = true);           // maybe?
    //     bool IsEditable();                               // maybe?
    
    //     void EnableJavascript(bool enabled);             // maybe?
    //     wxString RunScript(const wxString& javascript);  // maybe?
    
    //     void SetScrollPos(int pos);                      // maybe?
    //     int GetScrollPos();                              // maybe?
    
    //     wxString GetStatusText();                        // maybe?
    //     void SetStatusText(wxString text);               // maybe?
    //     * status text changed event?
    //     * title changed event?
    
    //    virtual bool IsOfflineMode() = 0;                 // maybe?
    //     virtual void SetOfflineMode(bool offline) = 0;   // maybe?
    
	// TODO: offer API to control the opening of new frames (through <a target="..."> as well as
	//       through javascript), OR provide a behavior consistent across ports.
	// - OSX : I receive an event for new frames opened with HTML target, and currently
	//		   block them all.
	// - IE  : The DISPID_NEWWINDOW2 event looks like it should work, but I receive way too
	//		   many of them. A new IE instance opens.
	// - GTK : All frame open requests are blocked. A slot exists that I could connect to
	//		   to be notified if ever needed
    
    /**
     * Opens a print dialog so that the user may print the currently displayed page.
     */
    virtual void Print() = 0;
    
    /**
     * Returns whether the web control is currently busy (e.g. loading a page)
     */
    virtual bool IsBusy() = 0;
};

//class WXDLLIMPEXP_FWD_HTML wxWebNavigationEvent;

// FIXME: get those WXDLLIMPEXP_HTML & DECLARE_DYNAMIC_CLASS right...
//wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_HTML, wxEVT_COMMAND_WEB_VIEW_NAVIGATE, wxWebNavigationEvent );


// FIXME: get those WXDLLIMPEXP_HTML & DECLARE_DYNAMIC_CLASS right...
class wxWebNavigationEvent : public wxCommandEvent
{
public:
    wxWebNavigationEvent() {}
    wxWebNavigationEvent(wxEventType type, int id, const wxString href, const wxString target, bool canVeto)
        : wxCommandEvent(type, id)
    {
        m_href = href;
        m_target = target;
        m_vetoed = false;
        m_can_veto = canVeto;
    }

    /**
     *  Get the URL being visited
     */
    const wxString &GetHref() const { return m_href; }
    
    /**
     * Get the target (frame or window) in which the URL that caused this event is viewed, or
     * an empty string if not available.
     */
    const wxString &GetTarget() const { return m_target; }
    
    // default copy ctor, assignment operator and dtor are ok
    virtual wxEvent *Clone() const { return new wxWebNavigationEvent(*this); }
    
    /** Get whether this event may be vetoed (stopped/prevented). Only meaningful for events
     *  fired before navigation takes place.
     */
    bool CanVeto() const { return m_can_veto; }
    
    /** Whether this event was vetoed (stopped/prevented). Only meaningful for events
     *  fired before navigation takes place.
     */
    bool IsVetoed() const { return m_vetoed; }
    
    /** Veto (prevent/stop) this event. Only meaningful for events
     *  fired before navigation takes place. Only valid if CanVeto() returned true.
     */
    void Veto() { wxASSERT(m_can_veto); m_vetoed = true; }
    
private:
    wxString m_href;
    wxString m_target;
    bool m_can_veto;
    bool m_vetoed;

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxWebNavigationEvent)
};

wxDECLARE_EVENT( wxEVT_COMMAND_WEB_VIEW_NAVIGATING, wxWebNavigationEvent );
wxDECLARE_EVENT( wxEVT_COMMAND_WEB_VIEW_NAVIGATED, wxWebNavigationEvent );
wxDECLARE_EVENT( wxEVT_COMMAND_WEB_VIEW_LOADED, wxWebNavigationEvent );
wxDECLARE_EVENT( wxEVT_COMMAND_WEB_VIEW_ERROR, wxWebNavigationEvent );

typedef void (wxEvtHandler::*wxWebNavigationEventFunction)(wxWebNavigationEvent&);

#define wxWebNavigationEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxWebNavigationEventFunction, func)

#define EVT_WEB_VIEW_NAVIGATING(id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_WEB_VIEW_NAVIGATING, id, wxHtmlNavigatingEventHandler(fn))

#define EVT_WEB_VIEW_NAVIGATED(id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_WEB_VIEW_NAVIGATED, id, wxHtmlNavigatingEventHandler(fn))
 
#define EVT_WEB_VIEW_LOADED(id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_WEB_VIEW_LOADED, id, wxHtmlNavigatingEventHandler(fn))

#define EVT_WEB_VIEW_ERRROR(id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_WEB_VIEW_ERROR, id, wxHtmlNavigatingEventHandler(fn))
    
    
#endif