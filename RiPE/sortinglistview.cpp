#include "stdafx.h"
#include "sortinglistview.h"

namespace
{
    // TODO:  Add a bool for the columns so that they can be set to text or number
    int wxCALLBACK
        MyCompareFunction( long item1, long item2, wxIntPtr sortData )
    {
        wxSortingListView *ctrl = (wxSortingListView *)sortData;
        wxString a, b;
        a = ctrl->GetItemText(item1, ctrl->m_sortCol);
        b = ctrl->GetItemText(item2, ctrl->m_sortCol);

        // if a number
        if( ctrl->m_sortCol == 1 ) {
            long la, lb;
            a.ToLong(&la, 10);
            b.ToLong(&lb, 10);
            if( ctrl->m_bSortAsc ) {
                return la-lb;
            }
            else {// descending
                return lb-la;
            }
        }
        else { // not a number
            if( ctrl->m_bSortAsc )
                return a.CmpNoCase(b);
            return b.CmpNoCase(a);
        }
    }
}

// Searches for an item based on the text and column.
// If not found, returns -1.  Else returns index.
long wxSortingListView::FindItem( const wxString &text, const int col )
{
    long index = GetNextItem(-1);
    for( ; index != -1; index = GetNextItem(index) ) {
        if( GetItemText(index, col) == text ) // If found
            break;
    }

    return index;
}

void wxSortingListView::OnColClick(wxListEvent& event)
{
    // Uncomment the following SetColumnImage if you have/want a bitmap
    // that gives feedback on sorting order
    SetColumnImage(m_sortCol, -1);
    if( m_sortCol != event.m_col )
        m_bSortAsc = true;
    else
        m_bSortAsc = ! m_bSortAsc;
    m_sortCol = event.m_col;
    SetColumnImage(m_sortCol, m_bSortAsc ? 1 : 2);
    long item = -1;
    for ( ;; )
    {
        item = GetNextItem(item);
        SetItemData(item, item);
        if ( item == -1 )
            break;
    }
    Sort();
}

long wxSortingListView::GetIndexFromOrder( const unsigned int pId, const wxString &filename )
{
    long index = GetNextItem(-1);

    if( m_bSortAsc ) { // if ascending
        if( m_sortCol == 0 ) { // pId
            for( ; index != -1; index = GetNextItem(index) ) {
                if( wxAtoi(GetItemText(index, 0))-pId >= 0 )
                    break;
            }
        } else { //filename
            for( ; index != -1; index = GetNextItem(index) ) {
                if( GetItemText(index, 1).CmpNoCase(filename) >= 0 )
                    break;
            }
        }
    } else { // else descending
        if( m_sortCol == 0 ) { // pId
            for( ; index != -1; index = GetNextItem(index) ) {
                if( pId-wxAtoi(GetItemText(index, 0)) > 0 )
                    break;
            }
        } else { //filename
            for( ; index != -1; index = GetNextItem(index) ) {
                if( filename.CmpNoCase(GetItemText(index, 1)) > 0 )
                    break;
            }
        }
    }
    return index == -1 ? 0 : index;
}

void wxSortingListView::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}

void wxSortingListView::SetItemIcon( const long index, const wxIcon &icon, const wxString &key )
{
    std::map<wxString, int>::iterator iter = m_iconMap.find(key);
    // If not found
    if( iter == m_iconMap.end() ) {
        int iconIndex = m_imageList->Add(icon);
        m_iconMap[key] = iconIndex;
        SetItemImage( index, iconIndex );
    } else {
        SetItemImage( index, iter->second );
    }
}

void wxSortingListView::SetItemIconDefault( const long index )
{
    SetItemImage( index, 3 );
}

void wxSortingListView::Sort()
{
    long item = -1;
    for ( ;; )
    {
        item = GetNextItem(item);
        SetItemData(item, item);
        if ( item == -1 )
            break;
    }
    SortItems(MyCompareFunction, (long)this);
}
