#pragma once

#include<vector>
#include<wx/string.h>
#include"List_objects.h"

/** ListInterface **
Handles the actual GUI lists, columns and the entry database.

--> It was meant as a simple abstraction, but got a bit out of hand
*/

class DataEntry;
class wxListCtrl;
class wxSize;
class wxColour;

class ListInterface
{
public:
	ListInterface();
	~ListInterface();

	//lists
	int RegisterList( const wxString &name, wxListCtrl *list, int insertIndex = -1 );
	int UnregisterList( const wxString &name );
	void ResizeAllLists( const wxSize &size );

	void ClearAllLists();
	void RebuildListColumns();
	void ColumnsResized(const wxString &group, int columnIndex, int columnWidth);
	void SetColumnWidth( const wxString &name, int newWidth );
	void SetColumnStatus( const wxString &name, bool enable );
	void SetFilter( const wxString &filter );
	void FilterList( const wxString &filter, List &list );
	void AddEntryToList( const DataEntry &entry );

	//data
	bool MoveSelectedEntry(const wxString &group, int direction);

	int ValidEntryName( const wxString& entryName ) const;
	int DeleteSelectionFrom( const wxString &name );
	const DataEntry* GetSelectedEntry( const wxString &groupName );
	bool UpdateEntry( const DataEntry &oldEntry, const DataEntry &updatedEntry);
	bool AddNewEntry(const wxString &groupName, const DataEntry &newEntry );

	void SetColours( const ListColours &newsettings );
	void GetColours( ListColours &settings ) const;
	void AssignEntryColour( List &list, int itemID, const DataEntry &entry, bool filtered = false );

	std::vector<ColumnData> GetColumns() const;
	const ColumnSettings* GetColumnSettings() const { return &columnsettings; }
	unsigned int GetEntryCount() { return data.size(); }
	const DataEntry* GetEntryAt(unsigned int i) { if( i >= 0 && i < data.size() ) return data[i]; return 0; }

	void SortEntriesByGroup();

	wxString GetLastErrorMessage() const { return lastErrorMessage; } 
	void SetErrorMessage(const wxString &errorMessage) { lastErrorMessage = errorMessage; }
private:
	int AddToList( List &list, const DataEntry &entry );
	bool DeleteEntry( const wxString &name );
	List* FindListByName( const wxString &name );
	DataEntry* FindEntryByName( const wxString &name );

	std::vector<DataEntry*> data;
	std::vector<List> lists;

	ColumnSettings columnsettings;
	ListColours listcolours;

	wxString lastErrorMessage;
};