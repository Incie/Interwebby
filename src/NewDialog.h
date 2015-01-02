#pragma once

#include<wx/dialog.h>

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
	NewDialog(wxWindow *parent, const wxArrayString &groups, const NDLMode mode = NDL_MODE_NEW);

	void SetEntry( const DataEntry& entry );
	void GetEntry( DataEntry &entry );
	void SetSelectedGroup( const wxString &group );

private:
	NewDialog() {}
	void OnButton( wxCommandEvent &evt );
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
	void GetGroup( wxString &string );
	int FindGroupID( const wxString &group );
	bool GroupExists( const wxString &group );
};