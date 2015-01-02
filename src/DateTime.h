#pragma once

#include<wx/string.h>


class DateTime
{
public:
	DateTime();
	~DateTime();

	static wxString GetDateTimeString();
	static wxString GetDateString();

	static bool ContainsToday(const wxString &date);
};