#pragma once

#include<wx/colour.h>
#include<vector>

/** List Objects **
Collection of flags and objects

>struct List
- Abstraction of wxListCtrl

>struct ColumnData
- Information for columns

>ColumnSettings
- Collection of ColumnData

>ListColours
- Collection of Colours used in the GUI lists
*/

class wxListCtrl;

enum DeleteFlags
{
	DELETE_OK,
	DELETE_OK_AND_EMPTY_LIST,
	DELETE_FAIL
};

struct List
{
	int CmpAsNumerical(const wxString &s0, const wxString &s1);

	int GetColumnWidth(int columnIndex);
	void SetColumnWidth(int columnIndex, int columnWidth);

	int GetSelectedIndex();
	wxString GetColumnValue( int itemid, int column );
	int FindItemIdFromColumnValue( const wxString &value, int column );
	bool MoveIndex( int itemid, int direction );
	int UpdateValue( int itemid, int column, const wxString &newValue );
	bool DeleteItem( int itemid );
	bool isEmpty();
	bool SetItemColours( int itemid, const wxColour &textColour, const wxColour &bgColour );
	int GetNextItemId( int previousid );
	int GetSortStatus(int columnId, bool bAsNumerical = false);
	bool Sort(int columnId, bool bReverse, bool bSortAsNumerical = false);
	bool SwapItems(int id0, int id1);

	wxListCtrl *list;
	wxString name;
};


struct ColumnData
{
	wxString name;
	int columnindex;
	int width;
	bool isEnabled;
};

class ColumnSettings
{
public:
	ColumnSettings();
	~ColumnSettings();

	bool SetStatus(const wxString &name, bool enabled);
	void SetSize(const wxString &name, int newsize);

	void SetWidthFromIndex(int columnIndex, int columnWidth);
	const ColumnData* GetColumnData(const wxString &name) const;
	int GetColumnID(const wxString &name) const;

	const ColumnData* GetColumnDataById(int id) const;
	const ColumnData* GetColumnDataByIndex(int n) const;
	int GetColumnCount() const;

	static const int INVALID_COLUMNID = -1;
private:
	bool AddColumn(const wxString &name, int size, bool enabled);
	ColumnData* GetColumn(const wxString &name) const;
	void RefreshColumnIDs();

	std::vector<ColumnData*> columns;
};

struct ListColours
{
	//colours
	wxColour rgbBackground;
	wxColour rgbLaunchedTextBg;
	wxColour rgbLaunchedText;
	wxColour rgbNormalTextBg;
	wxColour rgbNormalText;
	wxColour rgbFilteredText;
	wxColour rgbFilteredBg;
};