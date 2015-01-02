#include"List_objects.h"

typedef std::vector<ColumnData*>::iterator columniterator;
typedef std::vector<ColumnData*>::const_iterator constcolumniterator;

ColumnSettings::ColumnSettings()
{
	AddColumn(wxT("Name"), 100, true);
	AddColumn(wxT("URL"), 50, true);
	AddColumn(wxT("Last Launched"), 125, true);
	AddColumn(wxT("Date Added"), 125, true);
	AddColumn(wxT("Launched"), 100, true);
	RefreshColumnIDs();
}

ColumnSettings::~ColumnSettings()
{
	for( columniterator it = columns.begin(); it != columns.end(); ++it )
	{
		ColumnData *cdata = *it;

		if( cdata )
			delete cdata;
	}

	columns.clear();
}

const ColumnData* ColumnSettings::GetColumnDataByIndex(int n) const
{
	if( n < 0 || n >= (int)columns.size() )
		return 0;

	return columns[n];
}

int ColumnSettings::GetColumnCount() const
{
	return columns.size(); 
}

void ColumnSettings::SetWidthFromIndex(int columnIndex, int columnWidth)
{
	for( columniterator it = columns.begin(); it != columns.end(); ++it )
	{
		ColumnData *data = *it;

		if( !data )
			continue;

		if( data->columnindex == columnIndex )
		{
			data->width = columnWidth;
			break;
		}
	}
}

bool ColumnSettings::AddColumn( const wxString &name, int size, bool enabled )
{
	ColumnData *exists = GetColumn(name);

	if( exists )
		return false;

	ColumnData *newcolumn = new ColumnData();
	newcolumn->name = name;
	newcolumn->isEnabled = enabled;
	newcolumn->width = size;
	newcolumn->columnindex = INVALID_COLUMNID;
	columns.push_back(newcolumn);

	return true;
}

const ColumnData* ColumnSettings::GetColumnData(const wxString &name) const
{
	return GetColumn(name);
}

ColumnData* ColumnSettings::GetColumn(const wxString &name) const
{
	for( constcolumniterator it = columns.begin(); it != columns.end(); ++it )
	{
		ColumnData *data = *it;

		if( !data )
			continue;

		if( data->name.Cmp(name) == 0 )
			return data;
	}

	return 0;
}

void ColumnSettings::RefreshColumnIDs()
{
	int index = 0;
	for( columniterator it = columns.begin(); it != columns.end(); ++it )
	{
		ColumnData* data = *it;

		if( !data )
			continue;

		data->columnindex = INVALID_COLUMNID;

		if( data->isEnabled )
		{
			data->columnindex = index;
			index++;
		}
	}
}

bool ColumnSettings::SetStatus(const wxString &name, bool enabled)
{
	ColumnData *data = GetColumn(name);

	if( !data )
		return false;

	data->isEnabled = enabled;
	RefreshColumnIDs();

	return true;
}

void ColumnSettings::SetSize(const wxString &name, int newsize)
{
	ColumnData *data = GetColumn(name);
	
	if( !data )
		return;

	data->width = newsize;
}

int ColumnSettings::GetColumnID(const wxString &name) const
{
	ColumnData *data = GetColumn(name);

	if( !data )
		return INVALID_COLUMNID;

	return data->columnindex;
}