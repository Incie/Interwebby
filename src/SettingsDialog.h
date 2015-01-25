#pragma once

#include<wx/dialog.h>
#include<vector>

/** SettingsDialog **
Window for editing certain settings.
> Where the data-file is saved/loaded
> List Colours
> Which columns are visible
*/

class wxButton;
class wxPanel;
class wxStaticText;
class wxRadioButton;
class wxColour;
class wxCheckBox;
class wxStaticBox;
class wxColourPickerCtrl;
struct ColumnData;

struct ListColours;

class SettingsDialog : public wxDialog 
{
public: 
	SettingsDialog(wxWindow *parent, const std::vector<ColumnData> &columns);

	bool GetColumnStatusByName( const wxString &name ) const;

	void SetColours( const ListColours &listcolours );
	void GetColours( ListColours &colours ) const;

	void SetPath( const wxString &newpath );
	wxString GetPath() const;

	enum DataAction {
		FILE_NO_ACTION,
		FILE_OVERWRITE,
		FILE_DISCARD,
		FILE_MERGE
	};

	DataAction GetDataAction() { return dataAction; }
private:
	SettingsDialog() {}

	wxCheckBox* AddCheckBox( wxWindow* parent, const ColumnData& column, const wxPoint &position );

	void AdjustBoxSizes( wxStaticBox* pathBox, wxStaticBox* colourBox, wxStaticBox* columnBox );
	void AdjustLayoutPositions( wxPanel *mainPanel, wxStaticBox* pathBox, wxStaticBox* colourBox, wxStaticBox* columnBox);

	void OnButtonSetPath( wxCommandEvent &evt );
	void OnRadioButton(wxCommandEvent &evt);
	void OnColour( wxCommandEvent &evt );
	void OnCheck( wxCommandEvent &evt );
	void OnButtonOk(wxCommandEvent &evt);
	void OnButtonCancel(wxCommandEvent &evt);

	wxStaticBox* BuildPathBox(wxWindow* parent);
	wxStaticBox* BuildColourBox(wxWindow* parent);
	wxStaticBox* BuildColumnBox(wxWindow* parent, const std::vector<ColumnData> &columns);

	void UpdateColourPicker();
	wxColourPickerCtrl *colourPicker;

	wxCheckBox *checkUniformBG;
	wxRadioButton *radioBG, *radioFG;
	
	wxStaticText *stPath;
	DataAction dataAction;

	void SetColour( int radioIndex, const wxColour &colour, bool background, bool uniformBG );
	std::vector<wxStaticText*> staticTexts;
	std::vector<wxRadioButton*> radioButtons;

	struct column { column(const wxString& n, bool e, int id):name(n), enabled(e), checkboxid(id) {} wxString name; bool enabled; int checkboxid; };
	std::vector<column> columns;
};