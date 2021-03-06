#pragma once

#include<wx/dialog.h>

/** NewDialog **
A window for adding or modifying DataEntries.
It can set values for name, group and url. 
The values for date added, date launched and times launched are handled when the individual actions they are executed.
*/

class wxButton;
class wxTextCtrl;
class wxStaticText;
class wxPanel;
class wxString;
class DataEntry;
class wxComboBox;

enum NDL_retVals
{
	ID_NO,
	ID_YES,
	ID_YES_AND_MORE
};

enum NDLMode
{
	NDL_MODE_NEW,
	NDL_MODE_EDIT	
};

class NewDialog : public wxDialog 
{
public: 
	NewDialog(wxWindow *parent, const NDLMode mode = NDL_MODE_NEW);

	void SetEntry( const DataEntry& entry );
	void GetEntry( DataEntry &entry ) const;

	void SetSelectedGroupAs( const wxString &group );
	void SetGroupData( const wxArrayString &groups, const wxString &selection );

private:
	NewDialog() {}

	void OnButtonX(wxCommandEvent&);
	void OnButtonOK(wxCommandEvent&);
	void OnButtonOKPlus(wxCommandEvent&);
	void OnButtonCancel(wxCommandEvent&);
	void OnKeyEscape(wxCommandEvent&);

	void OnShow( wxShowEvent &evt );
	void OnEnter(wxCommandEvent&evt);
	void OnClose(wxCloseEvent&evt);
	void OnKeyUp( wxKeyEvent &evt );
	void OnEscape( wxKeyEvent &evt );
	void OnCombo( wxCommandEvent &evt );

	wxString newGroupString;

	wxPanel *mainpanel;
	wxButton *ok, *cancel, *more, *xbtn;
	wxStaticText *szname, *szurl, *szdesc;
	wxTextCtrl *name, *url, *desc;
	
	wxStaticText *szgroups;
	wxComboBox *groups;

	enum GroupMode 
	{
		GROUP_COMBOBOX,
		GROUP_TEXT
	};

	void SetGroupMode( NewDialog::GroupMode mode );
	wxString GetGroup() const;
	int FindGroupID( const wxString &group ) const;
	bool GroupExists( const wxString &group ) const;
};