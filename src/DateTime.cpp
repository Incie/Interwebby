#include"DateTime.h"
#include<wx/datetime.h>

DateTime::DateTime()
{
}

DateTime::~DateTime()
{
}

wxString DateTime::GetDateTimeString()
{
	wxDateTime now = wxDateTime::Now();
	return now.Format(wxT("%d.%m.%Y - %H:%S"));
}

wxString DateTime::GetDateString()
{
	wxDateTime now = wxDateTime::Now();
	return now.Format(wxT("%d.%m.%Y"));
}

bool DateTime::ContainsToday(const wxString &date)
{
	wxString today = GetDateString();
	if( date.Contains(today) )
		return true;
	return false;
}
