#pragma once


#include<wx/dialog.h>
#include<vector>

class wxButton;
class wxPanel;
class wxStaticText;
class wxRadioButton;
class wxColour;
class wxCheckBox;
class wxStaticBox;

struct ListColours;

class SettingsDialog : public wxDialog 
{
public: 
	SettingsDialog(wxWindow *parent);

	void AddCheckBox(const wxString &name, bool isEnabled);
	void SetColours( const ListColours &listcolours );
	void GetColours( ListColours &colours ) const;

	void SetPath( const wxString &newpath );
	wxString GetPath() const;

	bool GetColumnStatusByName( const wxString &name ) const;

private:
	SettingsDialog() {}

	wxPoint GetNextCheckBoxPosition();
	wxPoint checkBoxPosition;

	void OnButtonSetPath( wxCommandEvent &evt );
	void OnRadioButton(wxCommandEvent &evt);
	void OnColour( wxCommandEvent &evt );
	void OnCheck( wxCommandEvent &evt );
	void OnButtonOk(wxCommandEvent &evt);
	void OnButtonCancel(wxCommandEvent &evt);

	wxPanel *panelMain;

	wxStaticBox *parentColumns;
	wxStaticBox *parentColours;
	wxStaticBox *parentPaths;

	wxCheckBox *checkUniformBG;
	wxRadioButton *radioBG, *radioFG;
	
	wxStaticText *stPath;

	void SetColour( int radioIndex, const wxColour &colour, bool background, bool uniformBG );
	std::vector<wxStaticText*> staticTexts;
	std::vector<wxRadioButton*> radioButtons;

	struct column { wxString name; bool enabled; int checkboxid; };
	std::vector<column> columns;
};