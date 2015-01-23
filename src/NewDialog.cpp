#include"NewDialog.h"
#include"DataEntry.h"

#include<wx/panel.h>
#include<wx/button.h>
#include<wx/textctrl.h>
#include<wx/stattext.h>
#include<wx/combobox.h>

#include<wx/msgdlg.h>

wxPoint GetNextPosition( const wxWindow* win, const wxSize &padding )
{
	wxPoint nextposition = win->GetPosition();
	nextposition.y += win->GetSize().y;
	nextposition += padding;
	
	return nextposition;
}

NewDialog::NewDialog(wxWindow *parent, const wxArrayString &groupList, const NDLMode mode) : wxDialog(parent, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, (wxSYSTEM_MENU |  wxCAPTION | wxCLIP_CHILDREN | wxCLOSE_BOX) )
{
	Center();

	if( mode == NDL_MODE_EDIT ) 
		SetTitle(wxT("Modify.."));

	wxSize padSmall(0,3);
	wxSize pad(0,9);

	mainpanel = new wxPanel(this, wxID_ANY, wxPoint(0,0), GetClientSize());

	szname = new wxStaticText(mainpanel, wxID_ANY, wxT("Name"), wxPoint(5,5) );
	name = new wxTextCtrl(mainpanel, wxID_ANY, wxEmptyString, GetNextPosition(szname, padSmall) , wxSize(390, 20), wxTE_PROCESS_ENTER);

	szdesc = new wxStaticText(mainpanel, wxID_ANY, wxT("Enter a new group"), GetNextPosition(name, pad));
	desc = new wxTextCtrl(mainpanel, wxID_ANY, wxEmptyString, GetNextPosition(szdesc, padSmall), wxSize(390, 20), wxTE_PROCESS_ENTER);

	xbtn = new wxButton(desc, wxID_ANY, wxT("X"), wxPoint(368,1), wxSize(20,16), wxBORDER_NONE);
	xbtn->SetBackgroundColour(desc->GetBackgroundColour());
	xbtn->SetForegroundColour(wxColour(255,25,10));


	szgroups = new wxStaticText(mainpanel, wxID_ANY, wxT("Select a Group"), GetNextPosition(name, pad) );
	groups = new wxComboBox(mainpanel, wxID_ANY, wxEmptyString, GetNextPosition(szdesc, padSmall), wxSize(390, 24), groupList, wxTE_PROCESS_ENTER|wxCB_READONLY);

	szurl = new wxStaticText(mainpanel, wxID_ANY, wxT("URL"), GetNextPosition(groups, pad) );
	url = new wxTextCtrl(mainpanel, wxID_ANY, wxEmptyString, GetNextPosition(szurl, padSmall), wxSize(390, 20), wxTE_PROCESS_ENTER);

	url->SetValue( wxT("http://") );

	NewDialog::GroupMode gMode = NewDialog::GROUP_TEXT;
	if( groupList.size() != 0 )
	{
		groups->Select(0);
		gMode = NewDialog::GROUP_COMBOBOX;
	}

	SetGroupMode(gMode);

	newGroupString = wxT("New group...");
	groups->Append(newGroupString);

	//Buttons
	wxPoint buttonPosition = GetNextPosition(url, wxSize(0,12));
	ok = new wxButton(mainpanel, wxID_ANY, wxT("Ok"), wxPoint(400 - 140, buttonPosition.y), wxSize(70,20) );
	cancel = new wxButton(mainpanel, wxID_ANY, wxT("Cancel"), wxPoint(400 - 70, buttonPosition.y), wxSize(70,20) );

	more = 0;
	if( mode == NDL_MODE_NEW )
	{
		more = new wxButton(mainpanel, wxID_ANY, wxT("Ok+"), wxPoint(400 - 210, buttonPosition.y), wxSize(70,20) );
		Connect(more->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDialog::OnButtonOKPlus) );
	}

	//ClientSize
	wxSize clientSize;
	clientSize.x = 400;
	clientSize.y = cancel->GetSize().y + cancel->GetPosition().y + 5;
	SetClientSize(clientSize);

	//Events
	Connect(groups->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(NewDialog::OnCombo) );

	Connect(xbtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDialog::OnButtonX) );
	Connect(ok->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDialog::OnButtonOK) );
	Connect(cancel->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDialog::OnButtonCancel) );
	Connect(wxEVT_SHOW, wxShowEventHandler(NewDialog::OnShow));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(NewDialog::OnClose));

	Connect(url->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(NewDialog::OnEnter) );
	Connect(desc->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(NewDialog::OnEnter) );
	Connect(name->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(NewDialog::OnEnter) );
	Connect(groups->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(NewDialog::OnEnter) );

	Connect(wxEVT_KEY_UP, wxKeyEventHandler(NewDialog::OnEscape) );
	url->Connect(wxEVT_KEY_UP, wxKeyEventHandler(NewDialog::OnKeyUp) );
	desc->Connect(wxEVT_KEY_UP, wxKeyEventHandler(NewDialog::OnKeyUp) );
	name->Connect(wxEVT_KEY_UP, wxKeyEventHandler(NewDialog::OnKeyUp) );
	groups->Connect(wxEVT_KEY_UP, wxKeyEventHandler(NewDialog::OnKeyUp) );
}

void NewDialog::SetGroupMode( NewDialog::GroupMode mode )
{
	//Initialize as mode = NewDialog::GROUP_COMBO
	bool textmode = false;
	bool combomode = true;

	if( mode == GROUP_TEXT )
	{
		textmode = true;
		combomode = false;
	}

	desc->Show(textmode);
	szdesc->Show(textmode);
	xbtn->Show(textmode);

	groups->Show(combomode);
	szgroups->Show(combomode);
}

wxString NewDialog::GetGroup() const
{
	//return Comboboxvalue?
	if( groups->IsShown() ) 
		return groups->GetString( groups->GetSelection() );
	
	//return TextBox value
	return desc->GetValue();
}

int NewDialog::FindGroupID( const wxString &group ) const
{
	return groups->FindString(group);
}

void NewDialog::SetSelectedGroupAs( const wxString &group )
{
	int index = FindGroupID(group);

	if( index == wxNOT_FOUND )
	{
		if( groups->GetCount() > 0 )
			groups->Select(0);

		return;
	}

	groups->Select(index);
}

bool NewDialog::GroupExists( const wxString &group ) const
{
	return (FindGroupID(group) != wxNOT_FOUND);
}

void NewDialog::OnCombo( wxCommandEvent& )
{
	int selection = groups->GetSelection();
	int numItems = groups->GetCount();

	if( selection == (numItems-1) )
	{
		SetGroupMode(GROUP_TEXT);
		desc->SetFocus();
		desc->SelectAll();
	}
}

void NewDialog::OnKeyUp( wxKeyEvent &evt )
{
	if( evt.GetKeyCode() == WXK_ESCAPE )
	{
		evt.Skip();
		evt.ResumePropagation(wxEVENT_PROPAGATE_MAX);
		return;
	}
}

void NewDialog::OnEscape( wxKeyEvent &evt )
{
	if( evt.GetKeyCode() == WXK_ESCAPE )
		EndModal(ID_NO);

	evt.StopPropagation();
}

void NewDialog::OnClose(wxCloseEvent&)
{
	EndModal(ID_NO);
}

void NewDialog::OnEnter( wxCommandEvent& )
{
	int exitCode = ID_YES;
	if( wxGetKeyState(WXK_SHIFT) )
		exitCode = ID_YES_AND_MORE;

	EndModal(exitCode);
}

void NewDialog::OnShow( wxShowEvent& evt )
{
	if( evt.IsShown() )
		name->SetFocus();
}

void NewDialog::OnButtonX(wxCommandEvent&)
{
	//Todo: what happens if combobox is empty?
	SetGroupMode(GROUP_COMBOBOX);
}

void NewDialog::OnButtonOK(wxCommandEvent&)
{
	EndModal(ID_YES);
}

void NewDialog::OnButtonCancel(wxCommandEvent&)
{
	EndModal(ID_NO);
}

void NewDialog::OnButtonOKPlus(wxCommandEvent&)
{
	wxString groupname = GetGroup();
	//This assumes the new entry will be added, and therefore new group will exist
	//but this can fail if name already exists in the entrylist
	if( !GroupExists(groupname) )
		groups->Insert( groupname, groups->GetCount() - 1 );

	EndModal(ID_YES_AND_MORE);
}

void NewDialog::SetEntry(const DataEntry& entry)
{
	const wxString &group = entry.GetGroup();

	name->SetValue(entry.GetName());
	url->SetValue(entry.GetURL());
	desc->SetValue(entry.GetGroup());

	if( groups->GetCount() == 0 )
		SetGroupMode(GROUP_TEXT);
	else
	{
		SetGroupMode(GROUP_COMBOBOX);
		SetSelectedGroupAs(group);
	}
}

void NewDialog::GetEntry(DataEntry& entry) const
{
	entry.SetName( name->GetValue() );
	entry.SetURL( url->GetValue() );

	wxString group = GetGroup();
	entry.SetGroup(group);
}