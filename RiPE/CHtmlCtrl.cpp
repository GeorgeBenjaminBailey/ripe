
// webBrowserMFCAttempt3View.cpp : implementation of the CwebBrowserMFCAttempt3View class
//

#include "stdafx.h"
#include "CHtmlCtrl.h"

// CwebBrowserMFCAttempt3View

IMPLEMENT_DYNCREATE(CHtmlCtrl, CHtmlView)

BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlView)
    ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CwebBrowserMFCAttempt3View construction/destruction

CHtmlCtrl::CHtmlCtrl()
{
	// TODO: add construction code here
}

CHtmlCtrl::~CHtmlCtrl()
{
}

BOOL CHtmlCtrl::CreateFromStatic(UINT nID, CWnd* pParent)
{
    CStatic wndStatic;
    if (!wndStatic.SubclassDlgItem(nID, pParent))
        return FALSE;
    
    // Get static control rect, convert to parent's client coords.
    CRect rc;
    wndStatic.GetWindowRect(&rc);
    pParent->ScreenToClient(&rc);
    wndStatic.DestroyWindow();

    // create HTML control (CHtmlView)
    return Create(NULL,                  // class name
        NULL,                             // title
        (WS_CHILD | WS_VISIBLE ),         // style
        rc,                               // rectangle
        pParent,                          // parent
        nID,                              // control ID
        NULL);                            // frame/doc context not used
    return TRUE;
}

BOOL CHtmlCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CHtmlView::PreCreateWindow(cs);
}

void CHtmlCtrl::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	Navigate2(_T("http://www.riukuzaki.com/"),NULL,NULL);
}

void CHtmlCtrl::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CHtmlCtrl::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CwebBrowserMFCAttempt3View diagnostics

#ifdef _DEBUG
void CHtmlCtrl::AssertValid() const
{
	CHtmlView::AssertValid();
}

void GHtmlCtrl::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CwebBrowserMFCAttempt3Doc* CHtmlCtrl::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CwebBrowserMFCAttempt3Doc)));
	return (CwebBrowserMFCAttempt3Doc*)m_pDocument;
}
#endif //_DEBUG


// CwebBrowserMFCAttempt3View message handlers
