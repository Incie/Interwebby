#include"ListInterface.h"
#include"DataEntry.h"
#include"DateTime.h"
#include<wx/listctrl.h>

ListInterface::ListInterface()
{
}

ListInterface::~ListInterface()
{
	DeleteAll();
}

void ListInterface::DeleteAll()
{
	for( unsigned int i = 0; i < data.size(); ++i )
	{
		DataEntry *e = data[i];
		delete e;
	}

	data.clear();
	lists.clear();
}

void ListInterface::SortList( const wxString& group, int columnId )
{
	const ColumnData *columndata = columnsettings.GetColumnDataById(columnId);
	if( !columndata )
		return;

	List *list = FindListByName(group);
	if( !list )
		return;

	//Save the Selected Name
	int nameColumnId = columnsettings.GetColumnID(wxT("Name"));
	int selectedIndex = list->GetSelectedIndex();
	wxString selectedName;

	if( selectedIndex != -1 )
		selectedName = list->GetColumnValue(selectedIndex, nameColumnId);

	bool bDone = false;
	int status = list->GetSortStatus(columnId);
	int iterationCount = 0;

	while( !bDone )
	{
		//Get Current Sort Status
		if( status < 0 )
		{
			list->list->DeleteAllItems();
			for( unsigned int i = 0; i < GetEntryCount(); ++i )
			{
				const DataEntry *entry = GetEntryAt(i);
				if( entry && entry->CompareGroup(group) )
					AddEntryToList(*entry);
			}
		}
		else
		{
			bool bReverse = status==0?false:true;
			list->Sort(columnId, bReverse);	
		}

		//Check if something happened
		int finishedstatus = list->GetSortStatus(columnId);
		if( status != finishedstatus )
			bDone = true;
		else
		{
			if( finishedstatus == 0 ) status = 1;
			else if( finishedstatus == 1 ) status = -1;
			else status = 0;
		}

		//Guard against infinite looping
		if( iterationCount > 2 )
			break;
	}

	int toBeSelectedId = list->FindItemIdFromColumnValue(selectedName, nameColumnId);
	if( toBeSelectedId != -1 )
		list->list->SetItemState(toBeSelectedId, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

std::vector<ColumnData> ListInterface::GetColumns() const
{
	std::vector<ColumnData> columns;

	const ColumnSettings* columnSettings = GetColumnSettings();

	for( int i = 0; i < columnSettings->GetColumnCount(); i++ )
	{
		const ColumnData* cd = columnSettings->GetColumnDataByIndex(i);
		columns.push_back(*cd);
	}

	return columns;	
}

bool ListInterface::MoveSelectedEntry(const wxString &group, int direction)
{
	const DataEntry *selected = GetSelectedEntry(group);

	if( !selected )
		return false;

	List *list = FindListByName(group);
	int itemid = list->GetSelectedIndex();
	
	//Move List Entry
	if( !list->MoveIndex(itemid, direction) )
		return false;

	//Move Entry in the entrylist
	int selectedEntryIndex = -1; //Find swap index in list
	for( unsigned int i = 0; i < GetEntryCount(); ++i )
	{
		const DataEntry* entry = data[i];

		if( entry )
		{
			if( entry->CompareName(selected->GetName()) )
			{
				selectedEntryIndex = i;
				break;
			}
		}
	}

	//swap selection and its victim
	if( selectedEntryIndex != -1 )
	{
		int swapIndex = selectedEntryIndex + (direction < 0 ? -1 : 1); 

		if( swapIndex < 0 || swapIndex >= (int)data.size() )
		{
			return false;
		}

		DataEntry *selectedEntry = data[selectedEntryIndex];
		DataEntry *swapEntry = data[swapIndex];
		
		data[selectedEntryIndex] = swapEntry;
		data[swapIndex] = selectedEntry;
	}

	return true;
}

void ListInterface::RebuildListColumns()
{
	for( unsigned int i = 0; i < lists.size(); ++i )
	{
		List &list = lists[i];

		for( int j = 0; j < columnsettings.GetColumnCount(); ++j )
		{
			const ColumnData *data = columnsettings.GetColumnDataByIndex(j);

			if( !data )
				continue;

			const ColumnData &columndata = *data;

			if( columndata.isEnabled )
			{
				list.list->InsertColumn(columndata.columnindex, columndata.name);
				list.list->SetColumnWidth(columndata.columnindex, columndata.width);
			}
		}
	}
}

void ListInterface::ClearAllLists()
{
	for( unsigned int i = 0; i < lists.size(); ++i )
	{
		List &list = lists[i];
		list.list->ClearAll();
		list.list->SetBackgroundColour(listcolours.rgbBackground);
	}
}

void ListInterface::SortEntriesByGroup()
{
	if( data.size() <= 0 )
		return;

	std::vector<DataEntry*> tempentries;

	for( unsigned int groupID = 0; groupID < lists.size(); ++groupID )
	{
		List &list = lists[groupID];
		const wxString& group = list.name;

		for( unsigned int entryID = 0; entryID < data.size(); ++entryID )
		{
			const DataEntry* entry = data[entryID];
			
			if( entry->CompareGroup(group) )
				tempentries.push_back( data[entryID] );
		}
	}

	for( unsigned int i = 0; i < data.size(); ++i )
		data[i] = tempentries[i];
}

void ListInterface::ColumnsResized(const wxString &group, int columnIndex, int columnWidth)
{
	for( unsigned int i = 0; i < lists.size(); ++i )
	{
		List &l = lists[i];
		l.SetColumnWidth(columnIndex, columnWidth);
	}

	columnsettings.SetWidthFromIndex(columnIndex, columnWidth);
}

void ListInterface::SetColumnWidth( const wxString &name, int newWidth )
{
	columnsettings.SetSize(name, newWidth);
}

void ListInterface::SetColumnStatus( const wxString &name, bool enable )
{
	columnsettings.SetStatus(name, enable);
}

void ListInterface::SetFilter( const wxString &filter )
{
	for( unsigned int i = 0; i < lists.size(); ++i )
	{
		List &list = lists[i];
		FilterList(filter, list);
	}
}

void ListInterface::FilterList( const wxString &filter, List &list )
{
	bool bEmptyFilter = (filter.Cmp(wxEmptyString) == 0 );

	int itemid = list.GetNextItemId(-1);
	
	while( itemid != -1 )
	{
		wxListCtrl *lctrl = list.list;
		wxArrayInt columnorder = lctrl->GetColumnsOrder();

		bool bFilter = false;
		wxString name = list.GetColumnValue(itemid, 0);
		DataEntry *entry = FindEntryByName(name);

		if( entry )
		{
			for( unsigned int i = 0; i < columnorder.GetCount(); ++i )
			{
				int c = columnorder[i];

				wxListItem item;
				item.m_col = c;
				item.m_mask = wxLIST_MASK_TEXT;
				item.m_itemId = itemid;
				lctrl->GetItem(item);

				wxString &text = item.m_text;
				if( text.Contains(filter) && !bEmptyFilter )
				 bFilter = true;


				AssignEntryColour(list, itemid, *entry, bFilter);
			}
		}

		itemid = list.GetNextItemId(itemid);
	}
}

void ListInterface::AssignEntryColour( List &list, int itemID, const DataEntry &entry, bool filtered )
{
	const wxString entryDate = entry.GetDateLaunched();

	wxColour text = listcolours.rgbNormalText;
	wxColour bg = listcolours.rgbNormalTextBg;

	if( DateTime::ContainsToday(entryDate) )
	{
		text = listcolours.rgbLaunchedText;
		bg = listcolours.rgbLaunchedTextBg;
	}

	if( filtered )
	{
		text = listcolours.rgbFilteredText;
		bg = listcolours.rgbFilteredBg;
	}

	list.SetItemColours(itemID, text, bg);
}


void ListInterface::SetColours( const ListColours &newcolours )
{
	listcolours.rgbBackground = newcolours.rgbBackground;
	listcolours.rgbLaunchedTextBg = newcolours.rgbLaunchedTextBg;
	listcolours.rgbLaunchedText = newcolours.rgbLaunchedText;
	listcolours.rgbNormalTextBg = newcolours.rgbNormalTextBg;
	listcolours.rgbNormalText = newcolours.rgbNormalText;
	listcolours.rgbFilteredBg = newcolours.rgbFilteredBg;
	listcolours.rgbFilteredText = newcolours.rgbFilteredText;
}


void ListInterface::GetColours( ListColours &colours ) const
{
	colours.rgbBackground = listcolours.rgbBackground;
	colours.rgbLaunchedTextBg = listcolours.rgbLaunchedTextBg;
	colours.rgbLaunchedText = listcolours.rgbLaunchedText;
	colours.rgbNormalTextBg = listcolours.rgbNormalTextBg;
	colours.rgbNormalText = listcolours.rgbNormalText;
	colours.rgbFilteredBg = listcolours.rgbFilteredBg;
	colours.rgbFilteredText = listcolours.rgbFilteredText;
}

int ListInterface::ValidEntryName( const wxString& entryName ) const
{
	for( std::vector<DataEntry*>::const_iterator it = data.begin(); it != data.end(); ++it )
	{
		const DataEntry *entry = *it;

		if( !entry )
			continue;

		const wxString &name = entry->GetName();
		if( name.Cmp(entryName) == 0 )
			return 0;
	}

	return 1;
}

int ListInterface::AddToList( List &list, const DataEntry &entry )
{
	wxListCtrl *listctrl = list.list;

	int itemid = listctrl->InsertItem(0, wxT(""));
	int columnID = ColumnSettings::INVALID_COLUMNID;

	columnID = columnsettings.GetColumnID(wxT("Name"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list.UpdateValue(itemid, columnID, entry.GetName());

	columnID = columnsettings.GetColumnID(wxT("URL"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list.UpdateValue(itemid, columnID, entry.GetURL());

	columnID = columnsettings.GetColumnID(wxT("Last Launched"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list.UpdateValue(itemid, columnID, entry.GetDateLaunched());

	columnID = columnsettings.GetColumnID(wxT("Date Added"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list.UpdateValue(itemid, columnID, entry.GetDateAdded());

	columnID = columnsettings.GetColumnID(wxT("Launched"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list.UpdateValue(itemid, columnID, entry.GetTimesLaunched());

	return itemid;
}

void ListInterface::AddEntryToList( const DataEntry &entry )
{
	List *list = FindListByName(entry.GetGroup());

	if( !list )
	{
		SetErrorMessage(wxT("List Not Found"));
		return;
	}

	int itemid = AddToList(*list, entry);
	AssignEntryColour(*list, itemid, entry);
}

bool ListInterface::AddNewEntry(const wxString &groupName, const DataEntry &newEntry )
{
	List* list = FindListByName(groupName);
	if( !list )
	{
		SetErrorMessage(wxT("List Not Found"));
		return false;
	}

	if( !ValidEntryName( newEntry.GetName() ) )
	{
		SetErrorMessage(wxT("Name Already Exists"));
		return false;
	}

	DataEntry *e = new DataEntry(newEntry);
	data.push_back(e);

	AddEntryToList(*e);
	SortEntriesByGroup();

	return true;
}

bool ListInterface::DeleteEntry( const wxString &name )
{
	for( std::vector<DataEntry*>::iterator it = data.begin(); it != data.end(); ++it )
	{
		DataEntry *entry = *it;
		const wxString &entryName = entry->GetName();
		if( entryName.Cmp(name) == 0 )
		{
			delete entry;
			data.erase(it);
			return true;
		}
	}

	return false;
}

int ListInterface::DeleteSelectionFrom( const wxString &name )
{
	List *list = FindListByName(name);

	if( !list )
		return DELETE_FAIL;

	//Find name in list
	int itemid = list->GetSelectedIndex();
	if( itemid == -1 )
		return DELETE_FAIL;

	wxString entryName = list->GetColumnValue(itemid, 0);
	
	list->DeleteItem(itemid);
	DeleteEntry(entryName);

	if( list->isEmpty() )
		return DELETE_OK_AND_EMPTY_LIST;

	return DELETE_OK;
}

DataEntry* ListInterface::FindEntryByName( const wxString &name )
{
	for( std::vector<DataEntry*>::iterator it = data.begin(); it != data.end(); ++it )
	{
		DataEntry *entry = *it;
		if( !entry )
			continue;

		const wxString &entryName = entry->GetName();
		if( entryName.Cmp(name) == 0 )
			return entry;
	}

	return 0;
}

const DataEntry* ListInterface::GetSelectedEntry( const wxString &groupName )
{
	//find list from group
	List *list = FindListByName(groupName);

	if( !list )
		return 0;

	//find dataentry from name
	int itemid = list->GetSelectedIndex();
	if( itemid < 0 )
		return 0;

	wxString entryName = list->GetColumnValue(itemid, 0);
	DataEntry *entry = FindEntryByName(entryName);

	//if( !entry ) log( could not find entry : %name%
	
	return entry;
}

bool ListInterface::UpdateEntry( const DataEntry &oldEntry, const DataEntry &updatedEntry)
{
	List *list = FindListByName(oldEntry.GetGroup());

	if( !list )
		return false;

	int itemid = list->FindItemIdFromColumnValue( oldEntry.GetName(), 0 );

	int columnID = -1;

	columnID = columnsettings.GetColumnID(wxT("Name"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list->UpdateValue(itemid, columnID, updatedEntry.GetName());

	columnID = columnsettings.GetColumnID(wxT("URL"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list->UpdateValue(itemid, columnID, updatedEntry.GetURL());

	columnID = columnsettings.GetColumnID(wxT("Last Launched"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list->UpdateValue(itemid, columnID, updatedEntry.GetDateLaunched());

	columnID = columnsettings.GetColumnID(wxT("Date Added"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list->UpdateValue(itemid, columnID, updatedEntry.GetDateAdded());

	columnID = columnsettings.GetColumnID(wxT("Launched"));
	if( columnID != ColumnSettings::INVALID_COLUMNID )
		list->UpdateValue(itemid, columnID, updatedEntry.GetTimesLaunched());

	AssignEntryColour(*list, itemid, updatedEntry);

	DataEntry* entry = FindEntryByName(oldEntry.GetName());
	entry->SetName( updatedEntry.GetName() );
	entry->SetGroup( updatedEntry.GetGroup() ); //this could be redundant
	entry->SetURL( updatedEntry.GetURL() );
	entry->SetDateLaunched( updatedEntry.GetDateLaunched() );
	entry->SetTimesLaunched( updatedEntry.GetNumTimesLaunched() );
	entry->SetDateAdded( updatedEntry.GetDateAdded() );

	SortEntriesByGroup();

	return true;
}


#include<wx/msgdlg.h>
int ListInterface::RegisterList( const wxString &name, wxListCtrl *list, int insertIndex )
{
	if( FindListByName(name) )
		return 0;

	//////
	const ColumnData *cdName = columnsettings.GetColumnData( wxT("Name") );
	if( cdName && cdName->isEnabled )
		list->InsertColumn(cdName->columnindex, cdName->name, wxLIST_FORMAT_LEFT, cdName->width);		

	const ColumnData *cdURL = columnsettings.GetColumnData( wxT("URL") );
	if( cdURL && cdURL->isEnabled )
		list->InsertColumn(cdURL->columnindex, cdURL->name, wxLIST_FORMAT_LEFT, cdURL->width);

	const ColumnData *cdDateLaunched = columnsettings.GetColumnData( wxT("Last Launched") );
	if( cdDateLaunched && cdDateLaunched->isEnabled )
		list->InsertColumn(cdDateLaunched->columnindex, cdDateLaunched->name, wxLIST_FORMAT_LEFT, cdDateLaunched->width);

	const ColumnData *cdDateAdded = columnsettings.GetColumnData( wxT("Date Added") );
	if( cdDateAdded && cdDateAdded->isEnabled )
		list->InsertColumn(cdDateAdded->columnindex, cdDateAdded->name, wxLIST_FORMAT_LEFT, cdDateAdded->width);

	const ColumnData *cdTimesLaunched = columnsettings.GetColumnData( wxT("Launched") );
	if( cdTimesLaunched && cdTimesLaunched->isEnabled )
		list->InsertColumn(cdTimesLaunched->columnindex, cdTimesLaunched->name, wxLIST_FORMAT_LEFT, cdTimesLaunched->width);


	//////
	list->SetBackgroundColour( listcolours.rgbBackground );
	list->Refresh();

	List newlist;
	newlist.list = list;
	newlist.name = name;

	if( insertIndex >= 0 && insertIndex < (int)lists.size() )
	{
		std::vector<List>::iterator it = lists.begin();
		for( int i = 0; i < insertIndex; ++i )
			it++;

		lists.insert(it, newlist);
	}
	else //Add to End
		lists.push_back(newlist);

	return 1;
}

int ListInterface::UnregisterList( const wxString &name )
{
	for( std::vector<List>::iterator it = lists.begin(); it != lists.end(); ++it )
	{
		List &list = *it;

		if( list.name.Cmp(name) == 0 ) 
		{
			int listid = list.list->GetId();
			list.list->ClearAll();
			lists.erase(it);
			return listid;
		}
	}

	return -1;
}

void ListInterface::ResizeAllLists( const wxSize &size )
{
	for( std::vector<List>::iterator it = lists.begin(); it != lists.end(); ++it )
	{
		List &list = *it;
		list.list->SetSize(size);
	}
}

List* ListInterface::FindListByName( const wxString &name )
{
	for( std::vector<List>::iterator it = lists.begin(); it != lists.end(); ++it )
	{
		List &list = *it;

		assert(list.list && "list.list is invalid");

		if( list.name.Cmp(name) == 0 )
			return &list;	
	}

	return 0;
}
