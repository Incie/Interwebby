#include"MainFrame.h"

#include<wx/stattext.h>
#include<wx/panel.h>
#include<wx/listctrl.h>
#include<wx/notebook.h>
#include"DataEntry.h"

void MainFrame::InitNotebook(const wxPoint &position)
{
	nb = new wxNotebook(mainPanel, wxID_ANY, position, wxSize(800, 270), wxNB_TOP );
}

void MainFrame::MoveTab(int direction)
{
	int currentPage = nb->GetSelection();
	int swapPage = currentPage + (direction < 0 ? -1 : 1);

	if( swapPage < 0 || swapPage >= (int)nb->GetPageCount() )
		return;

	wxString groupName = nb->GetPageText(currentPage);

	DeleteSelectedTab();
	AddNewTab(groupName, swapPage);

	for( unsigned int i = 0; i < listinterface.GetEntryCount(); ++i )
	{
		const DataEntry *entry = listinterface.GetEntryAt(i);

		if( !entry )
			continue;

		if( entry->CompareGroup( groupName ) )
			listinterface.AddEntryToList(*entry);
	}

	listinterface.SortEntriesByGroup();
}

bool MainFrame::GenerateGroupList( wxArrayString &groupList )
{
	int pageCount = nb->GetPageCount();
	groupList.Alloc(pageCount);

	for( int i = 0; i < pageCount; ++i )
		groupList.Add(nb->GetPageText(i));

	return true;
}

wxString MainFrame::GetSelectedDesc()
{
	int selection = nb->GetSelection();

	if( selection == wxNOT_FOUND )
		return wxT("<nogroup>");

	return nb->GetPageText( selection );
}

void MainFrame::SelectPage(const wxString &groupName)
{
	for( unsigned int i = 0; i < nb->GetPageCount(); ++i )
	{
		const wxString &pageName = nb->GetPageText(i);

		if( pageName.Cmp(groupName) == 0 )
		{
			nb->SetSelection(i);
			return;
		}
	}
}

void MainFrame::SelectPage(int page)
{
	if( page >= 0 && page < (int)nb->GetPageCount() )
		nb->SetSelection(page);
}

wxListCtrl* MainFrame::AddNewTab( const wxString &name, int insertIndex )
{
	wxPanel *newPanel = new wxPanel(nb, wxID_ANY);
	wxListCtrl *pageList = new wxListCtrl(newPanel, wxID_ANY, wxPoint(0,0), nb->GetSize(), wxLC_REPORT|wxLC_SINGLE_SEL);

	Connect(pageList->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxCommandEventHandler(MainFrame::OnButtonLaunch) );
	Connect(pageList->GetId(), wxEVT_COMMAND_LIST_COL_END_DRAG, wxListEventHandler(MainFrame::OnColumnResize) );

	if( insertIndex == -1 )
	{
		nb->AddPage(newPanel, name);
		SelectPage( nb->GetPageCount() - 1 );
	}
	else
		nb->InsertPage(insertIndex, newPanel, name, true);

	listinterface.RegisterList(name, pageList, insertIndex);

	return pageList;
}

bool MainFrame::TabExists( const wxString &name )
{
	for( unsigned int i = 0; i < nb->GetPageCount(); ++i )
	{
		const wxString& pageText = nb->GetPageText(i);
		if( pageText.Cmp(name) == 0 )
			return true;
	}

	return false;
}

void MainFrame::DeleteSelectedTab()
{
	int selectedPage = nb->GetSelection();

	if( selectedPage < 0 || selectedPage > (int)nb->GetPageCount() )
		return;

	wxString pageText = nb->GetPageText(selectedPage);
	int listid = listinterface.UnregisterList(pageText);

	if( listid != -1 )
	{
		Disconnect(listid, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxCommandEventHandler(MainFrame::OnButtonLaunch) );
		Disconnect(listid, wxEVT_COMMAND_LIST_COL_END_DRAG, wxListEventHandler(MainFrame::OnColumnResize) );
	}

	nb->DeletePage( selectedPage );
}

void MainFrame::OnResize( wxSizeEvent& )
{
	wxSize clientSize = GetClientSize();
	mainPanel->SetSize( clientSize );

	//the filterbox and text is right-aligned
	wxSize filtersize = filter->GetSize();
	wxPoint filterposition( clientSize.x - filtersize.x - 5, 5);
	filter->SetPosition( filterposition );

	wxSize filtertxtsize = filtertxt->GetSize();
	wxPoint filtertxtposition( filterposition.x - filtertxtsize.x - 5, 7 );
	filtertxt->SetPosition(filtertxtposition);


	//Todo: verify if minus statusbar.y is actually needed
	nb->SetSize(clientSize.x, clientSize.y - GetStatusBar()->GetSize().y);

	int selection = nb->GetSelection();
	if( selection != wxNOT_FOUND )
	{
		wxPanel *selectedPanel = (wxPanel*) nb->GetPage(selection);
		wxSize panelSize = selectedPanel->GetSize();
		listinterface.ResizeAllLists(panelSize);
	}
}