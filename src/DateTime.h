#pragma once

#include<wx/string.h>


/** DateTime **
Interface for generating Date and DateTime strings, and checking if a string contains todays date.
Formats date as DD.MM.YYYY
Formats time as HH.MM
*/


class DateTime
{
public:
	DateTime();
	~DateTime();

	static wxString GetDateTimeString();
	static wxString GetDateString();

	static bool ContainsToday(const wxString &date);
};