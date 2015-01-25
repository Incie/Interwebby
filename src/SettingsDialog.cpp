#include"SettingsDialog.h"
#include"List_objects.h"
#include<wx/stattext.h>
#include<wx/button.h>
#include<wx/checkbox.h>
#include<wx/filedlg.h>
#include<wx/clrpicker.h>
#include<wx/radiobut.h>
#include<wx/msgdlg.h>
#include<wx/statbox.h>
#include <wx/choicdlg.h>
#include"List_objects.h"
#include"File.h"

SettingsDialog::SettingsDialog(wxWindow *parent, const std::vector<ColumnData> &columns) 
	:wxDialog(parent, wxID_ANY, wxT("Settings")), dataAction(FILE_NO_ACTION)
{
	//Layout 
	/*
	 *  |-PathBox----------|
	 *  |-Colour-||-Column-|
	 *  |---------OkCancel-|
	 */

	CenterOnParent();

	wxPanel* panelMain = new wxPanel(this, wxID_ANY, wxPoint(0,0));

	wxStaticBox* pathBox = BuildPathBox(panelMain);
	wxStaticBox* colourBox = BuildColourBox(panelMain);
	wxStaticBox* columnBox = BuildColumnBox(panelMain, columns);

	AdjustBoxSizes(pathBox, colourBox, columnBox);
	AdjustLayoutPositions(panelMain, pathBox, colourBox, columnBox);

	//Ok Cancel Buttons
	wxSize clientSize = GetClientSize();
	wxSize buttonSize(75, 25);
	wxButton *buttonCancel = new wxButton(panelMain, wxID_ANY, wxT("Cancel"), wxPoint(clientSize.x - buttonSize.x - 5,clientSize.y - 30), buttonSize);
	wxButton *buttonOk = new wxButton(panelMain, wxID_ANY, wxT("OK"), wxPoint(buttonCancel->GetPosition().x - buttonSize.x - 5,clientSize.y - 30), buttonSize);

	Connect(buttonOk->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsDialog::OnButtonOk));
	Connect(buttonCancel->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsDialog::OnButtonCancel));
}

wxStaticBox* SettingsDialog::BuildPathBox(wxWindow* parent)
{
	//Paths
	wxStaticBox* pathbox = new wxStaticBox(parent, wxID_ANY, wxT("Paths"), wxPoint(5,5), wxSize(490, 75));
	stPath = new wxStaticText(pathbox, wxID_ANY, wxT("C:/"), wxPoint(10,25));
	wxButton *buttonSetPath = new wxButton(pathbox, wxID_ANY, wxT("Set Data Path"), wxPoint(10,45), wxSize(75,20));

	Connect(buttonSetPath->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsDialog::OnButtonSetPath));
	return pathbox;
}

wxStaticBox* SettingsDialog::BuildColourBox(wxWindow* parent)
{
	//ColourBox
	wxStaticBox* colourBox = new wxStaticBox(parent, wxID_ANY, wxT("Colours"), wxPoint(5,85), wxSize(300, 150));
	wxStaticText *stColourPicker = new wxStaticText(colourBox, wxID_ANY, wxT("Set Colour:"), wxPoint(10, 25));
	colourPicker = new wxColourPickerCtrl(colourBox, wxID_ANY, wxColour(0,0,0), wxPoint(10 + stColourPicker->GetSize().x, 19), wxDefaultSize);
	Connect(colourPicker->GetId(), wxEVT_COMMAND_COLOURPICKER_CHANGED, wxCommandEventHandler(SettingsDialog::OnColour));

	int startHeight = 75;
	staticTexts.push_back(new wxStaticText(colourBox, wxID_ANY, wxT(" Normal Text "), wxPoint(25,startHeight)));
	int textHeight = staticTexts[0]->GetSize().y;

	staticTexts.push_back(new wxStaticText(colourBox, wxID_ANY, wxT(" Launched Text   "), wxPoint(25,startHeight + textHeight)));
	staticTexts.push_back(new wxStaticText(colourBox, wxID_ANY, wxT(" Filtered Text   "), wxPoint(25,startHeight + textHeight * 2)));
	staticTexts.push_back(new wxStaticText(colourBox, wxID_ANY, wxT(" Background      "), wxPoint(25,startHeight + textHeight * 3)));

	radioButtons.push_back(new wxRadioButton(colourBox, wxID_ANY, wxEmptyString, wxPoint(10,startHeight), wxDefaultSize, wxRB_GROUP));
	radioButtons.push_back(new wxRadioButton(colourBox, wxID_ANY, wxEmptyString, wxPoint(10,startHeight + textHeight)));
	radioButtons.push_back(new wxRadioButton(colourBox, wxID_ANY, wxEmptyString, wxPoint(10,startHeight + textHeight*2)));
	radioButtons.push_back(new wxRadioButton(colourBox, wxID_ANY, wxEmptyString, wxPoint(10,startHeight + textHeight*3)));

	//checkbox for uniform background 
	checkUniformBG = new wxCheckBox(colourBox, wxID_ANY, wxT("Uniform Background"), wxPoint(10, 50));

	//radiobuttons for selecting foreground or background
	radioFG = new wxRadioButton(colourBox, wxID_ANY, wxT("Set Foreground"), wxPoint(150,startHeight), wxDefaultSize, wxRB_GROUP);
	radioBG = new wxRadioButton(colourBox, wxID_ANY, wxT("Set Background"), wxPoint(150,startHeight + textHeight));

	for( unsigned int i = 0; i < radioButtons.size(); ++i )
		Connect(radioButtons[i]->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(SettingsDialog::OnRadioButton));

	//Connect FG and BG to the updatecolourpicker
	Connect(radioFG->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(SettingsDialog::OnRadioButton));
	Connect(radioBG->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(SettingsDialog::OnRadioButton));

	return colourBox;
}

wxCheckBox* SettingsDialog::AddCheckBox( wxWindow* parent, const ColumnData& column, const wxPoint &position )
{
	wxCheckBox *checkBox = new wxCheckBox(parent, wxID_ANY, column.name, position );
	checkBox->SetValue(column.isEnabled);
	Connect(checkBox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SettingsDialog::OnCheck));

	return checkBox;
}

wxStaticBox* SettingsDialog::BuildColumnBox(wxWindow* parent, const std::vector<ColumnData> &columns)
{
	//ColumnBox
	wxStaticBox* columnBox = new wxStaticBox(parent, wxID_ANY, wxT("Columns"), wxPoint(255,85));
	
	wxPoint position(10, 25);
	wxSize size(175,50);
	for( unsigned int i = 0; i < columns.size(); ++i )
	{
		const ColumnData& column = columns[i];
		wxCheckBox* checkBox = AddCheckBox(columnBox, column, position);

		position.y += checkBox->GetSize().y + 5;

		SettingsDialog::column c(column.name, column.isEnabled, checkBox->GetId());
		this->columns.push_back(c);
	}

	size.y = position.y + 10; //padding
	columnBox->SetSize(size);


	return columnBox;
}

void SettingsDialog::AdjustBoxSizes( wxStaticBox* pathBox, wxStaticBox* colourBox, wxStaticBox* columnBox )
{
	wxSize pathBoxSize = pathBox->GetSize();
	wxSize colourBoxSize = colourBox->GetSize();
	wxSize columnBoxSize = columnBox->GetSize();

	const int xPadding = 5;
	pathBoxSize.x = colourBoxSize.x + xPadding + columnBoxSize.x;

	pathBox->SetSize(pathBoxSize);
	colourBox->SetSize(colourBoxSize);
	columnBox->SetSize(columnBoxSize);
}

void SettingsDialog::AdjustLayoutPositions( wxPanel *mainPanel, wxStaticBox* pathBox, wxStaticBox* colourBox, wxStaticBox* columnBox)
{
	wxPoint pathBoxPosition(5,5);
	wxSize pathBoxSize = pathBox->GetSize();
	wxPoint colourBoxPosition = colourBox->GetPosition();
	wxSize colourBoxSize = colourBox->GetSize();
	wxPoint columnBoxPosition = columnBox->GetPosition();

	wxSize padding(5,5);
	colourBoxPosition.x = pathBoxPosition.x;
	colourBoxPosition.y = pathBoxPosition.y + pathBoxSize.y + padding.y;

	columnBoxPosition.x = colourBoxPosition.x + colourBoxSize.x + padding.x;
	columnBoxPosition.y = colourBoxPosition.y;

	pathBox->SetPosition(pathBoxPosition);
	colourBox->SetPosition(colourBoxPosition);
	columnBox->SetPosition(columnBoxPosition);

	wxSize clientSize;
	clientSize.x = pathBoxSize.x + 5 + 5; //5px padding per side
	clientSize.y = colourBoxPosition.y + colourBoxSize.y + 5 + 30; //5px padding from the box to button, 30x size = buttonsize + padding
	SetClientSize(clientSize);
	mainPanel->SetSize(clientSize);
}

void SettingsDialog::SetPath( const wxString &newpath )
{
	stPath->SetLabel(newpath);
}

wxString SettingsDialog::GetPath() const
{
	return stPath->GetLabel();
}

void SettingsDialog::OnButtonSetPath( wxCommandEvent& )
{
	wxString cwd = wxGetCwd();
	wxFileDialog openPath(this, wxT("Select the folder your data will be saved"), wxEmptyString, wxEmptyString, wxT("XML files (*.xml)|*.xml"), wxFD_OPEN);
	openPath.SetPath(cwd);
		
	if( openPath.ShowModal() == wxID_OK )
	{
		wxString newpath = openPath.GetPath();

		//Process Path
		wxString filetype = wxT(".xml");
		if( !newpath.Contains(filetype) )
			newpath.Append( filetype );
	
		if( File::ContainsCWD(newpath) )
			File::TrimCWD(newpath);

		/** -> New File Path
		  * File Exists?
		  * - Overwrite file?
		  * - Discard current file for the new file?
		  * - Merge current and new file?
		  *
		  * Files Does not Exist?
		  * - Save [Overwrite]
		  */
		if( File::Exists(newpath) )
		{
			wxString message(wxT("The chosen file already exist\nWhat action do you want to take?\n\n* Discard current content and load new content into Interwebby\n* Overwrite the file with current content\n* Merge both files\n\nNothing will happen untill you close the settings dialog"));
			wxString caption(wxT("Existing file found"));
			wxArrayString as;
			as.Add(wxT("Merge"));
			as.Add(wxT("Overwrite"));
			as.Add(wxT("Discard"));
			wxSingleChoiceDialog scd(this, message, caption, as);
		
			//Do nothing if user press cancel
			if( scd.ShowModal() == wxID_CANCEL )
				return;

			int selection = scd.GetSelection();
			switch( selection )
			{
			case 0: dataAction = FILE_MERGE; break;
			case 1: dataAction = FILE_OVERWRITE;break;
			case 2: dataAction = FILE_DISCARD;break;
			default: dataAction = FILE_NO_ACTION;break;
			}
		}
		else
		{
			//If the file does not exist then action will be to 'overwrite'
			dataAction = FILE_OVERWRITE;
		}

		//Test for access
		if( !File::HasWriteAccess(newpath) )
		{
			wxMessageBox(wxString::Format<wxString>(wxT("You do not have write access to the chosen location:\n%s"), newpath), wxT("Access Error"));
			return;
		}

		stPath->SetLabel(newpath);
	}
}

void SettingsDialog::GetColours( ListColours &colours ) const
{
	colours.rgbBackground = staticTexts[3]->GetBackgroundColour();

	colours.rgbNormalText = staticTexts[0]->GetForegroundColour();
	colours.rgbNormalTextBg = staticTexts[0]->GetBackgroundColour();

	colours.rgbLaunchedText = staticTexts[1]->GetForegroundColour();
	colours.rgbLaunchedTextBg = staticTexts[1]->GetBackgroundColour();

	colours.rgbFilteredText = staticTexts[2]->GetForegroundColour();
	colours.rgbFilteredBg = staticTexts[2]->GetBackgroundColour();
}

void SettingsDialog::SetColours( const ListColours &listcolours )
{
	staticTexts[0]->SetForegroundColour( listcolours.rgbNormalText );
	staticTexts[0]->SetBackgroundColour( listcolours.rgbNormalTextBg );

	staticTexts[1]->SetForegroundColour( listcolours.rgbLaunchedText );
	staticTexts[1]->SetBackgroundColour( listcolours.rgbLaunchedTextBg );

	staticTexts[2]->SetForegroundColour( listcolours.rgbFilteredText );
	staticTexts[2]->SetBackgroundColour( listcolours.rgbFilteredBg );

	wxColour background = listcolours.rgbBackground;
	staticTexts[3]->SetBackgroundColour( background );
	staticTexts[3]->SetForegroundColour( wxColour( 255 - background.Red(), 255 - background.Green(), 255 - background.Blue() ) );

	UpdateColourPicker();
}

void SettingsDialog::UpdateColourPicker()
{
	//Get Current Colours
	wxColour background(0,0,0), foreground(255,255,255);
	for( unsigned int i = 0; i < radioButtons.size(); ++i )
	{
		wxRadioButton *radioButton = radioButtons[i];
		wxStaticText *staticText = staticTexts[i];

		if( radioButton->GetValue() == true )
		{
			background = staticText->GetBackgroundColour();
			foreground = staticText->GetForegroundColour();
			break;
		}
	}

	//BG or FG colour?
	bool isForeground = true;
	if( radioBG->GetValue() )
		isForeground = false;

	//Set Colour
	if( isForeground )
		colourPicker->SetColour(foreground);
	else colourPicker->SetColour(background);

	colourPicker->Refresh();
}

void SettingsDialog::OnRadioButton( wxCommandEvent& )
{
	UpdateColourPicker();
}

void SettingsDialog::SetColour( int index, const wxColour &colour, bool background, bool uniformBG )
{
	if( index < 0 || index > (int)staticTexts.size() )
		return;

	wxStaticText *text = staticTexts[index];

	wxColour fgColour = text->GetForegroundColour();
	wxColour bgColour = text->GetBackgroundColour();

	if( background ) bgColour = colour;
	else fgColour = colour;

	if( index == 3 ) //special case for the actual list-background 
		fgColour = wxColour( 255 - bgColour.Red(), 255 - bgColour.Green(), 255 - bgColour.Blue() );

	text->SetForegroundColour(fgColour);
	text->SetBackgroundColour(bgColour);

	if( background && uniformBG )
	{
		for( unsigned int i = 0; i < staticTexts.size(); ++i )
			staticTexts[i]->SetBackgroundColour(bgColour);

		wxStaticText *bg = staticTexts[3];
		wxColour bgbg = bg->GetBackgroundColour();
		bg->SetForegroundColour( wxColour( 255 - bgbg.Red(), 255 - bgbg.Green(), 255 - bgbg.Blue() ) );
	}
}

void SettingsDialog::OnColour( wxCommandEvent &evt )
{
	wxColourPickerCtrl *ctrl = dynamic_cast<wxColourPickerCtrl*>(FindWindowById(evt.GetId()));

	if( !ctrl )
		return;

	wxColour colour = ctrl->GetColour();
	int index = -1;

	for( unsigned int i = 0; i < radioButtons.size(); ++i )
	{
		if( radioButtons[i]->GetValue() )
		{
			index = i;
			break;
		}
	}
	
	bool background = false;
	if( radioBG->GetValue() )
		background = true;

	bool uniformBackgrounds = checkUniformBG->IsChecked();

	SetColour(index, colour, background, uniformBackgrounds);
	Refresh();
}

bool SettingsDialog::GetColumnStatusByName( const wxString &name ) const
{
	for( std::vector<column>::const_iterator it = columns.begin(); it != columns.end(); ++it )
	{
		const column &c = *it;

		if( c.name.Cmp(name) == 0 )
			return c.enabled;
	}

	return false;
}

void SettingsDialog::OnCheck( wxCommandEvent &evt )
{
	int id = evt.GetId();
	wxCheckBox *checkbox = dynamic_cast<wxCheckBox*>(FindWindowById(id, this));

	if( !checkbox )
		return;

	for( std::vector<column>::iterator it = columns.begin(); it != columns.end(); ++it )
	{
		column &c = *it;

		if( c.checkboxid == id )
		{
			c.enabled = checkbox->IsChecked();
			break;
		}
	}
}

void SettingsDialog::OnButtonOk( wxCommandEvent& )
{
	EndModal(wxID_OK);
}

void SettingsDialog::OnButtonCancel( wxCommandEvent& )
{
	EndModal(wxID_CANCEL);
}