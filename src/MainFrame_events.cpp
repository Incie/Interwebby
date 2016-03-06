#include"MainFrame.h"
#include"NewDialog.h"
#include"DataEntry.h"
#include"DateTime.h"

#include<wx/msgdlg.h>
#include<wx/panel.h>
#include<wx/button.h>
#include<wx/listctrl.h>
#include<wx/panel.h>

#include"SettingsDialog.h"

void MainFrame::OnButtonStats(wxCommandEvent& evt)
{
	wxMessageBox(listinterface.GetTop5(),"Top 5 Launched");
}

void MainFrame::OnColumnClick(wxListEvent& evt)
{
	listinterface.SortList(GetSelectedDesc(), evt.GetColumn());
}

void MainFrame::OnButtonTabRight(wxCommandEvent&)
{
	MoveTab(1);
}

void MainFrame:: OnButtonTabLeft(wxCommandEvent&)
{
	MoveTab(-1);
}

void MainFrame::OnButtonEntryUp(wxCommandEvent&)
{
	listinterface.MoveSelectedEntry(GetSelectedDesc(), 1);
}

void MainFrame::OnButtonEntryDown(wxCommandEvent&)
{
	listinterface.MoveSelectedEntry(GetSelectedDesc(), -1);
}


void MainFrame::OnButtonSettings(wxCommandEvent&)
{
	//Set Columnsettings
	std::vector<ColumnData> columns = listinterface.GetColumns();
	SettingsDialog settingsDlg(this, columns);

	//Get current list colours and send them to the settingsdialog
	ListColours listcolours;
	listinterface.GetColours(listcolours);
	settingsDlg.SetColours(listcolours);

	settingsDlg.SetPath( szdatapath );


	//Wait for dialog
	int returnValue = settingsDlg.ShowModal();
		
	//Did user press OK?
	if( returnValue == wxID_OK )
	{
		//Disable redrawing untill changes are done
		Freeze();

		//Update Column Statuses
		const ColumnSettings* columnsettings = listinterface.GetColumnSettings();
		for( int i = 0; i < columnsettings->GetColumnCount(); ++i )
		{
			const ColumnData *c = columnsettings->GetColumnDataByIndex(i);
			bool status = settingsDlg.GetColumnStatusByName(c->name);
			listinterface.SetColumnStatus(c->name, status);
		}

		//Path
		wxString path = settingsDlg.GetPath();
		if( path.Cmp(szdatapath) != 0 )
		{
			SettingsDialog::DataAction action = settingsDlg.GetDataAction();

			switch( action )
			{
			case SettingsDialog::FILE_MERGE:
				szdatapath = path;
				LoadData();
				break;
			case SettingsDialog::FILE_OVERWRITE:
				szdatapath = path;
				break;
			case SettingsDialog::FILE_DISCARD:
				listinterface.DeleteAll(); //Delete the Data-side
				DeleteAllTabs(); //Delete the UI side
				szdatapath = path;
				LoadData();
				break;

			case SettingsDialog::FILE_NO_ACTION:
			default: break;
			}
		}

		//Get new colours and send them to the listinterface
		ListColours newcolours;
		settingsDlg.GetColours(newcolours);
		listinterface.SetColours(newcolours);

		//Reset and rebuild the listcontrols
		listinterface.ClearAllLists();
		listinterface.RebuildListColumns();
		
		//re-add all entries into lists
		for( unsigned int i = 0; i < listinterface.GetEntryCount(); ++i )
		{
			const DataEntry *entry = listinterface.GetEntryAt(i);

			if( entry )
				listinterface.AddEntryToList(*entry);
		}

		//Enable Drawing again
		Thaw();
	}
}

void MainFrame::OnTextFilter(wxCommandEvent&)
{
	wxString filterValue = filter->GetValue();
	listinterface.SetFilter(filterValue);

	if( filterValue.length() > 0 )
		xbtn->Show();
	else xbtn->Hide();
}

void MainFrame::OnButtonX(wxCommandEvent&)
{
	filter->SetValue(wxEmptyString);
}

void MainFrame::OnColumnResize(wxListEvent &evt)
{
	int columnIndex = evt.GetColumn();
	int columnWidth = evt.GetItem().GetWidth();
	wxString group = GetSelectedDesc();
	
	listinterface.ColumnsResized(group, columnIndex, columnWidth);
	Refresh(true);
}

void MainFrame::OnButtonExpand(wxCommandEvent&)
{
	bExpandButtons = !bExpandButtons;

	if( bExpandButtons )
	{
		expandBtn->SetLabel(wxT("<<"));

		wxPoint newPosition = statsBtn->GetPosition() + wxPoint(3,0);
		newPosition.x += entryDownBtn->GetSize().x;
		expandBtn->SetPosition(newPosition);

		modBtn->Show();
		delBtn->Show();
		newBtn->Show();
		settingsBtn->Show();
		tabLeftBtn->Show();
		tabRightBtn->Show();
		entryUpBtn->Show();
		entryDownBtn->Show();
		statsBtn->Show();

		return;
	}

	modBtn->Show(false);
	delBtn->Show(false);
	newBtn->Show(false);
	settingsBtn->Show(false);
	tabLeftBtn->Show(false);
	tabRightBtn->Show(false);
	entryUpBtn->Show(false);
	entryDownBtn->Show(false);
	statsBtn->Show(false);

	expandBtn->SetLabel(wxT(">>"));
	expandBtn->SetPosition(newBtn->GetPosition());
}

void MainFrame::OnClose(wxCloseEvent &evt)
{
	SaveSettings();
	if( !SaveData() )
	{
		if( evt.CanVeto() )
		{
			evt.Veto(true);
			return;
		}
	}
	
	evt.Skip();
}

void MainFrame::OnButtonDelete(wxCommandEvent&) 
{
	wxString groupName = GetSelectedDesc();
	
	int retval = listinterface.DeleteSelectionFrom(groupName);

	if( retval == DELETE_OK_AND_EMPTY_LIST )
		DeleteSelectedTab();
}

void MainFrame::OnButtonModify(wxCommandEvent&)
{
	// UpdateEntryProc returns 0 if an error happened, but you should restart
	//returns 1 if successful
	//returns -1 if a unrecoverable error happens
	
	int retValue = 0;

	do
	{
		retValue = UpdateEntryProc();
	} 
	while( retValue == 0 );
}

void MainFrame::OnButtonNew(wxCommandEvent&)
{
	//Create Dialog
	NewDialog dlg(this);

	int retValue = ID_YES;
	do
	{
		wxArrayString groupList;
		GenerateGroupList(groupList);
		
		//Combine SetSelected and SetGroups?
		dlg.SetGroupData(groupList, GetSelectedDesc());

		//Show Dialog
		retValue = dlg.ShowModal();
		
		if( retValue == ID_NO ) 
			break;

		//Grab Entry 
		DataEntry newEntry;
		dlg.GetEntry(newEntry);

		//Validate Unique name
		if( listinterface.ValidEntryName(newEntry.GetName()) )
		{
			//Add Metadata
			newEntry.SetDateLaunched(wxT("<never>"));
			newEntry.SetDateAdded(DateTime::GetDateString());

			//Try inserting new data in its new home
			if( NewData(newEntry) )
				SelectPage(newEntry.GetGroup());
			else
			{
				wxMessageBox(listinterface.GetLastErrorMessage(), wxT("Could not add new entry"));
				retValue = ID_YES_AND_MORE; //to restart the loop
			}
		}
	} 
	while( retValue == ID_YES_AND_MORE ); //User Pressed Ok+
}

#include<Windows.h>
void MainFrame::OnButtonLaunch(wxCommandEvent&)
{
	wxString groupName = GetSelectedDesc();
	const DataEntry *currentEntry = listinterface.GetSelectedEntry(groupName);

	if( !currentEntry )
		return;

	DataEntry updatedEntry(*currentEntry);
	updatedEntry.IncreaseTimesLaunched();
	updatedEntry.SetDateLaunched( DateTime::GetDateTimeString() );
	listinterface.UpdateEntry(*currentEntry, updatedEntry);

	//TODO: Tokenize the string to allow running of more stuff (e.g 'firefox about:addons' )
	const wxString url = currentEntry->GetURL();
	const wchar_t* lpstr = url.wc_str(wxConvUTF8);
	ShellExecute(0, 0, lpstr,0, 0, SW_SHOW);
}

//Disabled untill core (single launch) functionality is completed
void MainFrame::OnButtonLaunchAll(wxCommandEvent&)
{
	//wxListCtrl *lc = GetCurrentList();

	////TODO: Ask user if count > 5 (?)

	//for( int i = 0; i < lc->GetItemCount(); ++i )
	//{
	//	int itemIndex = i;

	//	lc->SetItemTextColour(itemIndex, rgbLaunch);
	//	lc->SetItemBackgroundColour(itemIndex, rgbLaunchBg);
	//	lc->SetItem(itemIndex, 3, GetDateTimeString() );

	//	wxListItem row_info;
	//	row_info.m_itemId = itemIndex;
	//	row_info.m_col = 2;
	//	row_info.m_mask = wxLIST_MASK_TEXT;
	//	lc->GetItem( row_info );
	//
	//	wxString str = row_info.GetText();
	//	const wchar_t* lpstr = str.ToStdWstring().c_str();
	//	ShellExecute(0, 0, lpstr,0, 0, SW_SHOW);

	//	wxMilliSleep(500);
	//}
}