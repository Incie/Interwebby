#include"ListInterface.h"
#include<wx/listctrl.h>


int List::GetSortStatus(int columnId, bool bAsNumerical)
{
	int sortStatus = 0;
	bool bSortedReverse = true;
	bool bSorted = true;

	//Setup Initial Conditions
	int itemId = GetNextItemId(-1);
	if( itemId == -1 )
		return 0;

	wxString prevString = GetColumnValue(itemId, columnId);
	itemId = GetNextItemId(itemId);
	
	while( itemId != -1 )
	{
		wxString value = GetColumnValue(itemId, columnId);

		int cmpValue = prevString.CmpNoCase(value);

		if( bAsNumerical )
		{
			long nVal = -1;
			value.ToLong(&nVal);

			long nPrev = -1;
			prevString.ToLong(&nPrev);

			cmpValue = 0;
			if( nPrev > nVal )
				cmpValue = 1;
			else if( nPrev < nVal )
				cmpValue = -1;
		}


		if( bSorted && cmpValue > 0 ) bSorted = false;
		if( bSortedReverse && cmpValue < 0 ) bSortedReverse = false;

		prevString = value;
		itemId = GetNextItemId(itemId);
	}

	if( bSorted ) sortStatus = 1;
	if( bSortedReverse ) sortStatus = -1;

	if( bSorted && bSortedReverse )
		return 0;

	return sortStatus;
}

/**
  Sort the listctrl using Selection-Sort
 **/
bool List::Sort(int columnId, bool bReverse, bool bSortAsNumerical)
{
	int reverseMultiplier = bReverse? -1 : 1;
	for( int itemid = 0; itemid < list->GetItemCount(); itemid++ )
	{
		const wxString itemText = GetColumnValue(itemid, columnId);
		wxString bestCandidate;

		int swapId = -1;
		for( int sortitem = itemid+1; sortitem < list->GetItemCount(); ++sortitem )
		{
			const wxString cmpText = GetColumnValue(sortitem, columnId);
			int cmpValue = cmpText.CmpNoCase(itemText);

			if( bSortAsNumerical )
				cmpValue = CmpAsNumerical(cmpText, itemText);

			if( cmpValue * reverseMultiplier < 0 )
			{
				int cmpBestCand = cmpText.CmpNoCase(bestCandidate);

				if( bSortAsNumerical )
					cmpBestCand = CmpAsNumerical(cmpText, bestCandidate);				

				if( swapId == -1 || 
					cmpBestCand * reverseMultiplier < 0 )
				{
					swapId = sortitem;
					bestCandidate = cmpText;
				}
			}
		}

		if( swapId != -1 )
			SwapItems(itemid, swapId);
	}

	return true;
}

int List::CmpAsNumerical(const wxString &s0, const wxString &s1)
{
	long n0 = 0;
	long n1 = 0;

	if( !s0.ToLong(&n0) || !s1.ToLong(&n1) )
		return 1;

	if( n0 < n1 )
		return -1;

	if( n0 > n1 )
		return 1;

	return 0;
}

bool List::SwapItems(int id0, int id1)
{
	if( id0 < 0 || id0 >= list->GetItemCount() )
		return false;

	if( id1 < 0 || id1 >= list->GetItemCount() )
		return false;

	//Swap values by column
	for( int i = 0; i < list->GetColumnCount(); ++i )
	{
		wxString v0 = GetColumnValue(id0, i);
		wxString v1 = GetColumnValue(id1, i);

		//Set ID0 with Value1
		UpdateValue(id0, i, v1);
		
		//Set ID1 with Value0
		UpdateValue(id1, i, v0);
	}

	//Swap Colours
	wxColour c0[2] = { list->GetItemBackgroundColour(id0), list->GetItemTextColour(id0) };
	wxColour c1[2] = { list->GetItemBackgroundColour(id1), list->GetItemTextColour(id1) };

	SetItemColours(id0, c1[1], c1[0]);
	SetItemColours(id1, c0[1], c0[0]);

	return true;
}

bool List::MoveIndex( int itemid, int direction )
{
	int swapitemid = itemid + (direction < 0 ? 1 : -1); //ListCtrl indices are logically in reverse (so up is down, and down is up)

	SwapItems(itemid, swapitemid);

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
			const wxString itemValue = GetColumnValue(itemid, column);
			if( itemValue.Cmp(value) == 0 )
				return itemid;
		}
	} while( itemid != -1 );

	return -1;
}

wxString List::GetColumnValue( int itemid, int column ) 
{ 
	if( !list || itemid == -1 )
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
