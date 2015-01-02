#include"ListInterface.h"
#include<wx/listctrl.h>

bool List::MoveIndex( int itemid, int direction )
{
	int swapitemid = itemid + (direction < 0 ? 1 : -1); //ListCtrl indices are logically in reverse (so up is down, and down is up)

	if( swapitemid < 0 || swapitemid >= list->GetItemCount() )
		return false;

	for( int i = 0; i < list->GetColumnCount(); ++i )
	{
		wxString v0 = GetColumnValue(itemid, i);
		wxString v1 = GetColumnValue(swapitemid, i);

		UpdateValue(itemid, i, v1);
		UpdateValue(swapitemid, i, v0);
	}

	list->SetItemState(swapitemid, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

	return true;
}

void List::SetColumnWidth(int columnIndex, int columnWidth)
{
	if( !list )
		return;

	if( columnIndex < 0 || columnIndex >= list->GetColumnCount() )
		return;

	list->SetColumnWidth(columnIndex, columnWidth);
}

int List::GetColumnWidth(int columnIndex)
{
	if( !list )
		return -1;

	if( columnIndex < 0 || columnIndex >= list->GetColumnCount() )
		return -2;

	//This assumes columnindices only can be from 0 to count, and without holes
	return list->GetColumnWidth(columnIndex);
}

int List::GetNextItemId( int previousid )
{
	if( !list )
		return -1;

	return list->GetNextItem(previousid, wxLIST_NEXT_ALL,wxLIST_STATE_DONTCARE); 
}

bool List::SetItemColours( int itemid, const wxColour &textColour, const wxColour &bgColour )
{
	if( !list )
		return false;

	list->SetItemBackgroundColour(itemid, bgColour);
	list->SetItemTextColour(itemid, textColour);

	return true;
}

int List::GetSelectedIndex() 
{ 
	if( !list ) 
		return -1; 
	return list->GetNextItem(-1, wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED); 
}

bool List::isEmpty() 
{ 
	if( !list ) 
		return true; 
	
	if( list->GetItemCount() == 0 ) 
		return true; 
	return false; 
}

int List::UpdateValue( int itemid, int column, const wxString &newValue )
{
	if( !list )
		return false;

	return list->SetItem(itemid, column, newValue);
}

int List::FindItemIdFromColumnValue( const wxString &value, int column )
{
	if( !list )
		return -1;

	int itemid = -1;
	do 
	{
		itemid = list->GetNextItem(itemid, wxLIST_NEXT_ALL,wxLIST_STATE_DONTCARE);

		if( itemid != -1 )
		{
			wxListItem listitem;
			listitem.m_itemId = itemid;
			listitem.m_col = column;
			listitem.m_mask = wxLIST_MASK_TEXT;
			list->GetItem(listitem);

			const wxString &itemValue = listitem.m_text;
			if( itemValue.Cmp(value) == 0 )
				return itemid;
		}
	} while( itemid != -1 );

	return -1;
}

wxString List::GetColumnValue( int itemid, int column ) 
{ 
	if( !list )
		return wxEmptyString;

	wxListItem listitem;
	listitem.m_itemId = itemid;
	listitem.m_col = column;
	listitem.m_mask = wxLIST_MASK_TEXT;
		
	if( !list->GetItem(listitem) )
		return wxEmptyString;

	return listitem.m_text;
}

bool List::DeleteItem( int itemid )
{
	if( !list )
		return false;

	return list->DeleteItem(itemid);
}
