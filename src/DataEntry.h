#pragma once

#include<wx/string.h>

class DataEntry
{
public:
	DataEntry():numTimesLaunched(0) {}
	DataEntry(const wxString &name, const wxString &group, const wxString &url, const wxString &dateLaunched, const wxString &dateAdded, const int numTimesLaunched):
	  name(name), group(group), url(url), dateLaunched(dateLaunched), dateAdded(dateAdded), numTimesLaunched(numTimesLaunched) 
	 {}
	DataEntry(const DataEntry& e):
	  name(e.name), group(e.group), url(e.url), dateLaunched(e.dateLaunched), dateAdded(e.dateAdded), numTimesLaunched(e.numTimesLaunched) 
	 {}

	 ~DataEntry() {}

	void SetName(const wxString &newName) { name = newName; }
	void SetGroup(const wxString &newGroup) { group = newGroup; }
	void SetURL(const wxString &newURL) { url = newURL; }
	void SetDateLaunched(const wxString &newDateLaunched) { dateLaunched = newDateLaunched; }
	void SetDateAdded(const wxString &newDateAdded) { dateAdded = newDateAdded; }
	void SetTimesLaunched( const int newTimesLaunched ) { numTimesLaunched = newTimesLaunched; }
	void SetTimesLaunched( const wxString &strTimesLaunched  ) { long val = -1; if( strTimesLaunched.ToLong(&val) ) { numTimesLaunched = val; } }
	void IncreaseTimesLaunched() { numTimesLaunched++; }

	bool CompareName( const wxString &n ) const { return (name.Cmp(n) == 0 ); }
	bool CompareGroup( const wxString &grp ) const { return (group.Cmp(grp) == 0); }

	wxString GetName() const { return name; }
	wxString GetGroup() const { return group; }
	wxString GetURL() const { return url; }
	wxString GetDateLaunched() const { return dateLaunched; }
	wxString GetDateAdded() const { return dateAdded; }
	wxString GetTimesLaunched() const { wxString str; str.Printf<int>(wxT("%i"), numTimesLaunched); return str; }
	int GetNumTimesLaunched() const { return numTimesLaunched; }

private:
	//operator =(DataEntry&e); ?
	wxString name;
	wxString group;
	wxString url;
	wxString dateLaunched;
	wxString dateAdded;
	int numTimesLaunched;
};