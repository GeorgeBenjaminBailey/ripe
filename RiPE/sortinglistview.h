#ifndef __SORTING_LIST_VIEW_H
#define __SORTING_LIST_VIEW_H

#include <map>

#include <wx/icon.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/event.h>

#include "exe_icon.xpm"
#include "listarrows.xpm"

class wxSortingListView : public wxListView
{
private:
    wxImageList *m_imageList;
    std::map<wxString, int> m_iconMap; // a mapping of filenames to icons

public:
    int  m_sortCol;
    bool m_bSortAsc;

    wxSortingListView() { }
    wxSortingListView( wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxLC_REPORT,
        const wxValidator& validator = wxDefaultValidator,
        const wxString &name = wxListCtrlNameStr)
    {
        Create(parent, winid, pos, size, style, validator, name);
        m_imageList = new wxImageList(16, 16, true);
        m_imageList->Add(wxBitmap(sort_null_xpm) );
        m_imageList->Add(wxBitmap(sort_asc_xpm) );
        m_imageList->Add(wxBitmap(sort_desc_xpm) );
        m_imageList->Add(wxBitmap(exe_icon_xpm) );

        SetImageList(m_imageList, wxIMAGE_LIST_SMALL); 

        Connect(winid, wxEVT_COMMAND_LIST_COL_CLICK,
            wxListEventHandler(wxSortingListView::OnColClick));
        m_sortCol = 0;
        m_bSortAsc = true;
    }

    ~wxSortingListView()
    {
        delete m_imageList;
    }

    long FindItem( const wxString &text, const int col );

    void OnColClick(wxListEvent& event);

    void OnInsertItem(wxListEvent& event);

    long GetIndexFromOrder( const unsigned int pId, const wxString &filename );

    void SetColumnImage(int col, int image);

    void SetItemIcon( const long index, const wxIcon &icon, const wxString &key );

    void SetItemIconDefault( const long index );

    void Sort();
};

#endif
