/////////////////////////////////////////////////////////////////////////////
// Name:        wxgtkwebkitctrl.cpp
// Purpose:     GTK WebKit backend for web view component
// Author:      Marianne Gagnon, Robert Roebling
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon, 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wxgtkwebkitctrl.h"

#if wxHAVE_WEB_BACKEND_GTK_WEBKIT

#ifndef WX_PRECOMP
    #include "wx/button.h"
#endif

#include "wx/stockitem.h"

// FIXME: use the wxGTK_CONV from wxWidgets sources instead of a local copy
#define wxGTK_CONV(s) wxConvUTF8.cWX2MB(s)

//#include "wx/gtk/private.h"

#include "webkit/webkit.h"

#include "wx/gtk/control.h"

// ----------------------------------------------------------------------------
// GTK callbacks
// ----------------------------------------------------------------------------

extern "C"
{

static void
wxgtk_webkitctrl_load_status_callback(GtkWidget* widget, GParamSpec* arg1, wxGtkWebKitCtrl *webKitCtrl)
{
	if (!webKitCtrl->m_ready) return;
	
	wxString url = webKitCtrl->GetCurrentURL();
	
	WebKitLoadStatus status;
	g_object_get(G_OBJECT(widget), "load-status", &status, NULL);
	
	wxString target; // TODO: get target (if possible)
	
	if (status == WEBKIT_LOAD_FINISHED)
	{
		webKitCtrl->m_busy = false;
		wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_LOADED, webKitCtrl->GetId(),
										url, target, false);
		
		if (webKitCtrl && webKitCtrl->GetEventHandler())
			webKitCtrl->GetEventHandler()->ProcessEvent(thisEvent);
	}
	else if (status ==  WEBKIT_LOAD_COMMITTED)
	{
		webKitCtrl->m_busy = true;
		wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_NAVIGATED, webKitCtrl->GetId(),
										url, target, false);
		
		if (webKitCtrl && webKitCtrl->GetEventHandler())
			webKitCtrl->GetEventHandler()->ProcessEvent(thisEvent);
	}
}

static WebKitNavigationResponse
wxgtk_webkitctrl_navigation_requ_callback(WebKitWebView        *web_view,
                                          WebKitWebFrame       *frame,
										  WebKitNetworkRequest *request,
										  wxGtkWebKitCtrl      *webKitCtrl)
{
	webKitCtrl->m_busy = true;
	
	const gchar* uri = webkit_network_request_get_uri(request);
	
    wxString target = webkit_web_frame_get_name (frame);
    wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_NAVIGATING, webKitCtrl->GetId(),
                                    wxString( uri, wxConvUTF8 ), target, true);
    
    if (webKitCtrl && webKitCtrl->GetEventHandler())
        webKitCtrl->GetEventHandler()->ProcessEvent(thisEvent);

    if (thisEvent.IsVetoed())
	{
		webKitCtrl->m_busy = false;
        return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
	}
    else
	{
        return  WEBKIT_NAVIGATION_RESPONSE_ACCEPT;
	}
}
														
//-----------------------------------------------------------------------------
// "style_set" from m_widget
//-----------------------------------------------------------------------------

static void
wxgtk_webkitctrl_style_set_callback(GtkWidget* widget, GtkStyle*, wxGtkWebKitCtrl* win)
{
    /* the default button has a border around it */
    wxWindow* parent = win->GetParent();
    if (parent && parent->m_wxwindow && GTK_WIDGET_CAN_DEFAULT(widget))
    {
        GtkBorder* border = NULL;
        gtk_widget_style_get(widget, "default_border", &border, NULL);
        if (border)
        {
            win->MoveWindow(
                win->m_x - border->left,
                win->m_y - border->top,
                win->m_width + border->left + border->right,
                win->m_height + border->top + border->bottom);
            gtk_border_free(border);
        }
    }
}

static gboolean
wxgtk_webkitctrl_error (WebKitWebView  *web_view,
						WebKitWebFrame *web_frame,
						gchar          *uri,
						gpointer        web_error,
						wxGtkWebKitCtrl* webKitWindow)
{
	webKitWindow->m_busy = false;
	wxWebNavigationError type = wxWEB_NAV_ERR_OTHER;
	
	GError* error = (GError*)web_error;	
	wxString description(error->message, wxConvUTF8);
		
	if (strcmp(g_quark_to_string(error->domain), "soup_http_error_quark") == 0)
	{
		switch (error->code)
		{
			case SOUP_STATUS_CANCELLED:
				type = wxWEB_NAV_ERR_USER_CANCELLED;
				break;
				
			case SOUP_STATUS_CANT_RESOLVE:
			case SOUP_STATUS_NOT_FOUND:
				type = wxWEB_NAV_ERR_NOT_FOUND;
				break;
			
			case SOUP_STATUS_CANT_RESOLVE_PROXY:
			case SOUP_STATUS_CANT_CONNECT:
			case SOUP_STATUS_CANT_CONNECT_PROXY:
			case SOUP_STATUS_SSL_FAILED:
			case SOUP_STATUS_IO_ERROR:
				type = wxWEB_NAV_ERR_CONNECTION;
				break;

			case SOUP_STATUS_MALFORMED:
			//case SOUP_STATUS_TOO_MANY_REDIRECTS:
				type = wxWEB_NAV_ERR_REQUEST;
				break;

			//case SOUP_STATUS_NO_CONTENT:
			//case SOUP_STATUS_RESET_CONTENT:
			
			case SOUP_STATUS_BAD_REQUEST:
				type = wxWEB_NAV_ERR_REQUEST;
				break;
			
			case SOUP_STATUS_UNAUTHORIZED:
			case SOUP_STATUS_FORBIDDEN:
				type = wxWEB_NAV_ERR_AUTH;
				break;	
			
			case SOUP_STATUS_METHOD_NOT_ALLOWED:
			case SOUP_STATUS_NOT_ACCEPTABLE:
				type = wxWEB_NAV_ERR_SECURITY;
				break;
				
			case SOUP_STATUS_PROXY_AUTHENTICATION_REQUIRED:
				type = wxWEB_NAV_ERR_AUTH;
				break;
			
			case SOUP_STATUS_REQUEST_TIMEOUT:
				type = wxWEB_NAV_ERR_CONNECTION;
				break;
				
			//case SOUP_STATUS_PAYMENT_REQUIRED:
			case SOUP_STATUS_REQUEST_ENTITY_TOO_LARGE:
			case SOUP_STATUS_REQUEST_URI_TOO_LONG:
			case SOUP_STATUS_UNSUPPORTED_MEDIA_TYPE:
				type = wxWEB_NAV_ERR_REQUEST;
				break;

			case SOUP_STATUS_BAD_GATEWAY:
			case SOUP_STATUS_SERVICE_UNAVAILABLE:
			case SOUP_STATUS_GATEWAY_TIMEOUT:
				type = wxWEB_NAV_ERR_CONNECTION;
				break;
				
			case SOUP_STATUS_HTTP_VERSION_NOT_SUPPORTED:
				type = wxWEB_NAV_ERR_REQUEST;
				break;
			//case SOUP_STATUS_INSUFFICIENT_STORAGE:
			//case SOUP_STATUS_NOT_EXTENDED:
		}
	}
	else if (strcmp(g_quark_to_string(error->domain), "webkit-network-error-quark") == 0)
	{		
		switch (error->code)
		{
			//WEBKIT_NETWORK_ERROR_FAILED:
			//WEBKIT_NETWORK_ERROR_TRANSPORT:
			
			case WEBKIT_NETWORK_ERROR_UNKNOWN_PROTOCOL:
				type = wxWEB_NAV_ERR_REQUEST;
				break;
				
			case WEBKIT_NETWORK_ERROR_CANCELLED:
				type = wxWEB_NAV_ERR_USER_CANCELLED;
				break;
			
			case WEBKIT_NETWORK_ERROR_FILE_DOES_NOT_EXIST:
				type = wxWEB_NAV_ERR_NOT_FOUND;
				break;
		}
	}
	else if (strcmp(g_quark_to_string(error->domain), "webkit-policy-error-quark") == 0)
	{
		switch (error->code)
		{
			//case WEBKIT_POLICY_ERROR_FAILED:
			//case WEBKIT_POLICY_ERROR_CANNOT_SHOW_MIME_TYPE:
			//case WEBKIT_POLICY_ERROR_CANNOT_SHOW_URL:
			//case WEBKIT_POLICY_ERROR_FRAME_LOAD_INTERRUPTED_BY_POLICY_CHANGE:
			case WEBKIT_POLICY_ERROR_CANNOT_USE_RESTRICTED_PORT:
				type = wxWEB_NAV_ERR_SECURITY;
				break;
		}
	}
	/*
	webkit_plugin_error_quark
	else
	{
		printf("Error domain %s\n", g_quark_to_string(error->domain));
	}
	*/
	
	wxWebNavigationEvent thisEvent(wxEVT_COMMAND_WEB_VIEW_ERROR, webKitWindow->GetId(),
								    wxString( uri ), wxEmptyString, false);
	thisEvent.SetString(description);
	thisEvent.SetInt(type);
			
	if (webKitWindow && webKitWindow->GetEventHandler())
	{
		webKitWindow->GetEventHandler()->ProcessEvent(thisEvent);
	}
	
	return FALSE;
}


} // extern "C"

//-----------------------------------------------------------------------------
// wxGtkWebKitCtrl
//-----------------------------------------------------------------------------

//IMPLEMENT_DYNAMIC_CLASS(wxGtkWebKitCtrl, wxControl)

bool wxGtkWebKitCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString &url,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
	m_ready = false;
	m_busy = false;
	
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxGtkWebKitCtrl creation failed") );
        return false;
    }
	
	GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	web_view = webkit_web_view_new ();
	g_object_ref(web_view); // TODO: check memory management
	
	m_widget = scrolled_window;
	g_object_ref(m_widget); // TODO: check memory management

	/* Place the WebKitWebView in the GtkScrolledWindow */
	gtk_container_add (GTK_CONTAINER (scrolled_window), web_view);
	gtk_widget_show(m_widget);
	gtk_widget_show(web_view);

	g_signal_connect_after(web_view, "notify::load-status",  G_CALLBACK(wxgtk_webkitctrl_load_status_callback), this);
	g_signal_connect_after(web_view, "navigation-requested", G_CALLBACK(wxgtk_webkitctrl_navigation_requ_callback), this);
	g_signal_connect_after(web_view, "load-error",           G_CALLBACK(wxgtk_webkitctrl_error), this);
	
	// this signal can be added if we care about new frames open requests
	//g_signal_connect_after(web_view, "new-window-policy-decision-requested",  G_CALLBACK(...), this);	
	
    m_parent->DoAddChild( this );
	
    PostCreation(size);
	
	/* Open a webpage */
	webkit_web_view_load_uri (WEBKIT_WEB_VIEW (web_view), url);
	
	m_ready = true;
	
    return true;
}

bool wxGtkWebKitCtrl::Enable( bool enable )
{
    if (!wxControl::Enable(enable))
        return false;

    gtk_widget_set_sensitive(GTK_BIN(m_widget)->child, enable);

    //if (enable)
    //    GTKFixSensitivity();

    return true;
}

GdkWindow *wxGtkWebKitCtrl::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
	GdkWindow* window = gtk_widget_get_parent_window(m_widget);
	//printf("GTKGetWindow called, returning %lu\n", (unsigned long)(void*)window);
	return window;
	
	// FIXME?
    //return WEBKIT_WEB_VIEW(m_widget)->event_window;
	//return NULL;
}

void wxGtkWebKitCtrl::DoApplyWidgetStyle(GtkRcStyle *style)
{
    gtk_widget_modify_style(m_widget, style);
    GtkWidget *child = GTK_BIN(m_widget)->child;
    gtk_widget_modify_style(child, style);

    // for buttons with images, the path to the label is (at least in 2.12)
    // GtkButton -> GtkAlignment -> GtkHBox -> GtkLabel
    if ( GTK_IS_ALIGNMENT(child) )
    {
        GtkWidget *box = GTK_BIN(child)->child;
        if ( GTK_IS_BOX(box) )
        {
            for (GList* item = GTK_BOX(box)->children; item; item = item->next)
            {
                GtkBoxChild* boxChild = static_cast<GtkBoxChild*>(item->data);
                gtk_widget_modify_style(boxChild->widget, style);
            }
        }
    }
}

void wxGtkWebKitCtrl::zoomIn()
{
	webkit_web_view_zoom_in (WEBKIT_WEB_VIEW(web_view));
}

void wxGtkWebKitCtrl::zoomOut()
{
	webkit_web_view_zoom_out (WEBKIT_WEB_VIEW(web_view));
}

void wxGtkWebKitCtrl::SetWebkitZoom(float level)
{
	webkit_web_view_set_zoom_level (WEBKIT_WEB_VIEW(web_view), level);
}

float wxGtkWebKitCtrl::GetWebkitZoom()
{
	return webkit_web_view_get_zoom_level (WEBKIT_WEB_VIEW(web_view));
}

void wxGtkWebKitCtrl::Stop()
{
	 webkit_web_view_stop_loading (WEBKIT_WEB_VIEW(web_view));
}

void wxGtkWebKitCtrl::Reload(bool bypassCache)
{
	if (bypassCache)
	{
		webkit_web_view_reload_bypass_cache (WEBKIT_WEB_VIEW(web_view));
	}
	else
	{
		webkit_web_view_reload (WEBKIT_WEB_VIEW(web_view));
	}
}

void wxGtkWebKitCtrl::LoadUrl(wxString loc)
{
	webkit_web_view_open(WEBKIT_WEB_VIEW(web_view), wxGTK_CONV(loc));
}


void wxGtkWebKitCtrl::GoBack()
{
	webkit_web_view_go_back (WEBKIT_WEB_VIEW(web_view));
}

void wxGtkWebKitCtrl::GoForward()
{
	webkit_web_view_go_forward (WEBKIT_WEB_VIEW(web_view));
}


bool wxGtkWebKitCtrl::CanGoBack()
{
	return webkit_web_view_can_go_back (WEBKIT_WEB_VIEW(web_view));
}


bool wxGtkWebKitCtrl::CanGoForward()
{
	return webkit_web_view_can_go_forward (WEBKIT_WEB_VIEW(web_view));
}


wxString wxGtkWebKitCtrl::GetCurrentURL()
{	
	// GTK+ uses UTF-8 for all strings normally. The GTK WebKit docs do not explicitely
	// mention which encoding is used, but hopefully the port respects the UTF-8 convention
	return wxString(webkit_web_view_get_uri (WEBKIT_WEB_VIEW(web_view)), wxConvUTF8);
}


wxString wxGtkWebKitCtrl::GetCurrentTitle()
{
	// GTK+ uses UTF-8 for all strings normally. The GTK WebKit docs do not explicitely
	// mention which encoding is used, but hopefully the port respects the UTF-8 convention
	return wxString(webkit_web_view_get_title (WEBKIT_WEB_VIEW(web_view)), wxConvUTF8);
}


wxString wxGtkWebKitCtrl::GetPageSource()
{
	WebKitWebFrame* frame = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW(web_view));
	WebKitWebDataSource* src = webkit_web_frame_get_data_source (frame);
	
	const gchar* encoding = webkit_web_data_source_get_encoding(src);
	
	wxCSConv converter(encoding);
	if (!converter.IsOk())
	{
		wxLogWarning("Page source encoding <%s> is unknown to wxCSConv, result may be incorrect\n", encoding);
	}
	
	// FIXME: this fails with multi-byte encodings (e.g. UTF-16). I suspect the underlying GTK Webkit port
	//        has a bug where it stops copying the string when meeting the first 0 (the string is empty)
	return wxString(webkit_web_data_source_get_data (src)->str, converter);
}


wxWebViewZoom wxGtkWebKitCtrl::GetZoom()
{
	float zoom = GetWebkitZoom();
    
    // arbitrary way to map float zoom to our common zoom enum 
    if (zoom <= 0.65)
    {
        return wxWEB_VIEW_ZOOM_TINY;
    }
    else if (zoom > 0.65 && zoom <= 0.90)
    {
        return wxWEB_VIEW_ZOOM_SMALL;
    }
    else if (zoom > 0.90 && zoom <= 1.15)
    {
        return wxWEB_VIEW_ZOOM_MEDIUM;
    }
    else if (zoom > 1.15 && zoom <= 1.45)
    {
        return wxWEB_VIEW_ZOOM_LARGE;
    }
    else if (zoom > 1.45)
    {
        return wxWEB_VIEW_ZOOM_LARGEST;
    }
    
    // to shut up compilers, this can never be reached logically
    wxASSERT(false);
    return wxWEB_VIEW_ZOOM_MEDIUM;
}


void wxGtkWebKitCtrl::SetZoom(wxWebViewZoom zoom)
{
	// arbitrary way to map our common zoom enum to float zoom
    switch (zoom)
    {
        case wxWEB_VIEW_ZOOM_TINY:
            SetWebkitZoom(0.6f);
            break;
            
        case wxWEB_VIEW_ZOOM_SMALL:
            SetWebkitZoom(0.8f);
            break;
        
        case wxWEB_VIEW_ZOOM_MEDIUM:
            SetWebkitZoom(1.0f);
            break;
        
        case wxWEB_VIEW_ZOOM_LARGE:
            SetWebkitZoom(1.3);
            break;
            
        case wxWEB_VIEW_ZOOM_LARGEST:
            SetWebkitZoom(1.6);
            break;
            
        default:
            wxASSERT(false);
    }
}

void wxGtkWebKitCtrl::SetZoomType(wxWebViewZoomType type)
{
	webkit_web_view_set_full_content_zoom (WEBKIT_WEB_VIEW(web_view),
										   (type == wxWEB_VIEW_ZOOM_TYPE_LAYOUT ? TRUE : FALSE));
}

wxWebViewZoomType wxGtkWebKitCtrl::GetZoomType() const
{
	gboolean fczoom = webkit_web_view_get_full_content_zoom (WEBKIT_WEB_VIEW(web_view));
	if (fczoom) return wxWEB_VIEW_ZOOM_TYPE_LAYOUT;
	else		return wxWEB_VIEW_ZOOM_TYPE_TEXT;
}

bool wxGtkWebKitCtrl::CanSetZoomType(wxWebViewZoomType) const
{
	// this port supports all zoom types
	return true;
}

void wxGtkWebKitCtrl::SetPage(wxString html, wxString baseUri)
{
	webkit_web_view_load_string (WEBKIT_WEB_VIEW(web_view),
								 html.mb_str(wxConvUTF8),
								 "text/html",
								 "UTF-8",
								 baseUri.mb_str(wxConvUTF8));
}

void wxGtkWebKitCtrl::Print()
{
	WebKitWebFrame* frame = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW(web_view));
	//webkit_web_frame_print (frame);
	
	GError* error = 0;
    GtkPrintOperation* operation = gtk_print_operation_new();
	gtk_print_operation_set_show_progress(operation, TRUE);
	gtk_print_operation_set_allow_async(operation, FALSE);
	webkit_web_frame_print_full(frame,
								operation,
								GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
								&error);
	
	if (error != NULL)
	{
		wxLogWarning ("An error occurred while trying to print: %s\n", error->message);
		g_error_free (error);
	}

}


bool wxGtkWebKitCtrl::IsBusy()
{
	return m_busy;
	
	// This code looks nice but returns true after a page was cancelled
	/*
	WebKitLoadStatus status = webkit_web_view_get_load_status (WEBKIT_WEB_VIEW(web_view));
	
	
#if WEBKIT_CHECK_VERSION(1,1,16)
	// WEBKIT_LOAD_FAILED is new in webkit 1.1.16
	if (status == WEBKIT_LOAD_FAILED)
	{
		return false;
	}
#endif
	if (status == WEBKIT_LOAD_FINISHED)
	{
		return false;
	}
	
	return true;
	 */
}

// static
wxVisualAttributes
wxGtkWebKitCtrl::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
     return GetDefaultAttributesFromGTKWidget(webkit_web_view_new);
}

// ----------------------------------------------------------------------------
// bitmaps support
// ----------------------------------------------------------------------------
/*
void wxGtkWebKitCtrl::GTKMouseEnters()
{
}

void wxGtkWebKitCtrl::GTKMouseLeaves()
{

}

void wxGtkWebKitCtrl::GTKPressed()
{

}

void wxGtkWebKitCtrl::GTKReleased()
{
}

void wxGtkWebKitCtrl::GTKOnFocus(wxFocusEvent& event)
{
    event.Skip();

}
*/


#endif // wxUSE_BUTTON