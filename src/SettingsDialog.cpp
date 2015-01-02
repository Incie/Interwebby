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
#include"File.h"

SettingsDialog::SettingsDialog(wxWindow *parent) 
	:wxDialog(parent, wxID_ANY, wxT("Settings"))
{
	CenterOnParent();

	wxSize clientSize(500, 280);
	SetClientSize(clientSize);

	panelMain = new wxPanel(this, wxID_ANY, wxPoint(0,0), clientSize);

	parentPaths   = new wxStaticBox(panelMain, wxID_ANY, wxT("Paths"), wxPoint(5,5), wxSize(490, 75));
	parentColours = new wxStaticBox(panelMain, wxID_ANY, wxT("Colours"), wxPoint(5,85), wxSize(240, 150));
	parentColumns = new wxStaticBox(panelMain, wxID_ANY, wxT("Columns"), wxPoint(255,85), wxSize(240,50));
	
	wxStaticText *stColourPicker = new wxStaticText(parentColours, wxID_ANY, wxT("Set Colour:"), wxPoint(10, 25));
	wxColourPickerCtrl *colour = new wxColourPickerCtrl(parentColours, wxID_ANY, wxColour(0,0,0), wxPoint(10 + stColourPicker->GetSize().x, 19), wxDefaultSize);
	Connect(colour->GetId(), wxEVT_COMMAND_COLOURPICKER_CHANGED, wxCommandEventHandler(SettingsDialog::OnColour));

	int startHeight = 75;
	staticTexts.push_back(new wxStaticText(parentColours, wxID_ANY, wxT(" Normal Text "), wxPoint(25,startHeight)));
	int textHeight = staticTexts[0]->GetSize().y;

	staticTexts.push_back(new wxStaticText(parentColours, wxID_ANY, wxT(" Launched Text   "), wxPoint(25,startHeight + textHeight)));
	staticTexts.push_back(new wxStaticText(parentColours, wxID_ANY, wxT(" Filtered Text "), wxPoint(25,startHeight + textHeight * 2)));
	staticTexts.push_back(new wxStaticText(parentColours, wxID_ANY, wxT(" Background    "), wxPoint(25,startHeight + textHeight * 3)));

	radioButtons.push_back(new wxRadioButton(parentColours, wxID_ANY, wxT(""), wxPoint(10,startHeight), wxDefaultSize, wxRB_GROUP));
	radioButtons.push_back(new wxRadioButton(parentColours, wxID_ANY, wxT(""), wxPoint(10,startHeight + textHeight)));
	radioButtons.push_back(new wxRadioButton(parentColours, wxID_ANY, wxT(""), wxPoint(10,startHeight + textHeight*2)));
	radioButtons.push_back(new wxRadioButton(parentColours, wxID_ANY, wxT(""), wxPoint(10,startHeight + textHeight*3)));

	//checkbox for uniform background 
	checkUniformBG = new wxCheckBox(parentColours, wxID_ANY, wxT("Uniform Background"), wxPoint(10, 50));

	//radiobuttons for selecting foreground or background
	radioFG = new wxRadioButton(parentColours, wxID_ANY, wxT("Set Fackground"), wxPoint(150,startHeight), wxDefaultSize, wxRB_GROUP);
	radioBG = new wxRadioButton(parentColours, wxID_ANY, wxT("Set Boreground"), wxPoint(150,startHeight + textHeight));

	for( unsigned int i = 0; i < radioButtons.size(); ++i )
		Connect(radioButtons[i]->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(SettingsDialog::OnRadioButton));

	//Paths
	stPath = new wxStaticText(parentPaths, wxID_ANY, wxT("C:/"), wxPoint(10,25));
	wxButton *buttonSetPath = new wxButton(parentPaths, wxID_ANY, wxT("Set Data Path"), wxPoint(10,45), wxSize(75,20));

	Connect(buttonSetPath->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsDialog::OnButtonSetPath));

	//Dialog Specific
	wxSize buttonSize(75, 25);
	wxButton *buttonOk = new wxButton(panelMain, wxID_ANY, wxT("OK"), wxPoint(80,clientSize.y - 30), buttonSize);
	wxButton *buttonCancel = new wxButton(panelMain, wxID_ANY, wxT("Cancel"), wxPoint(160,clientSize.y - 30), buttonSize);

	Connect(buttonOk->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsDialog::OnButtonOk));
	Connect(buttonCancel->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsDialog::OnButtonCancel));

	checkBoxPosition = wxPoint(10,25);
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

		wxString filetype = wxT(".xml");
		if( !newpath.Contains(filetype) )
			newpath.Append( filetype );

		if( newpath.StartsWith(cwd) )
		{
			newpath = newpath.Remove(0, cwd.length());

			if( newpath.StartsWith(wxT("/")) || newpath.StartsWith(wxT("\\")) )
				newpath = newpath.Remove(0,1);
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
}

void SettingsDialog::OnRadioButton( wxCommandEvent& )
{
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

	//TODO? Set foreground as background is bool background = false ?
	if( index == 3 ) //special case for the actual background 
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
	wxColourPickerCtrl *ctrl = dynamic_cast<wxColourPickerCtrl*>(FindWindowById(evt.GetId(), panelMain));

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
	panelMain->Refresh();
}

wxPoint SettingsDialog::GetNextCheckBoxPosition()
{
	wxPoint returnPoint = checkBoxPosition;
	checkBoxPosition.y += 20;
	return returnPoint;
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

void SettingsDialog::AddCheckBox( const wxString &name, bool isEnabled )
{
	wxCheckBox *checkBox = new wxCheckBox(parentColumns, wxID_ANY, name, GetNextCheckBoxPosition() );
	checkBox->SetValue(isEnabled);
	Connect(checkBox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SettingsDialog::OnCheck));
	
	wxSize parentSize = parentColumns->GetSize();
	parentColumns->SetSize(parentSize.x, checkBox->GetPosition().y + 30 );

	SettingsDialog::column c;
	c.checkboxid = checkBox->GetId();
	c.enabled = isEnabled;
	c.name = name;
	columns.push_back(c);
}

void SettingsDialog::OnButtonOk( wxCommandEvent& )
{
	EndModal(wxID_OK);
}

void SettingsDialog::OnButtonCancel( wxCommandEvent& )
{
	EndModal(wxID_CANCEL);
}