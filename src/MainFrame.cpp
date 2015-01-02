#include"MainFrame.h"

#include<wx/stattext.h>
#include<wx/textctrl.h>
#include<wx/panel.h>
#include<wx/button.h>
#include<wx/listctrl.h>
#include<wx/panel.h>

#include<wx/menu.h>

#ifdef _DEBUG
#define TITLESTRING wxT("Interwebby (Beta 7 Debugbuild) ")
#else
#define TITLESTRING wxT("Interwebby (Beta 7)")
#endif

wxButton* MainFrame::AddButton(wxWindow* parent, const wxString& title, const wxPoint& position, const wxSize& size, wxObjectEventFunction func, bool isShown)
{
	wxButton *btn = new wxButton(parent, wxID_ANY, title, position, size);

	if( btn )
	{
		Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, func);

		if( !isShown )
			btn->Show(false);
	}

	return btn;
}

MainFrame::MainFrame() : wxFrame(0, wxID_ANY, TITLESTRING)
{
	wxImage::AddHandler( new wxPNGHandler() );

	SetIcon(wxIcon( wxT("icon.png"), wxBITMAP_TYPE_PNG));

	SetClientSize(911,400);
	Centre();
	
	mainPanel = new wxPanel(this, wxID_ANY, wxPoint(0,0), wxSize(800,600));
	
	wxSize buttonSize(70,20);
	wxSize buttonSizeSmall(30,20); 

	AddButton(mainPanel, wxT("Launch"), wxPoint(0,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonLaunch) );
	AddButton(mainPanel, wxT("Launch All"), wxPoint(75,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonLaunchAll) );
	newBtn = AddButton(mainPanel, wxT("New.."), wxPoint(150,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonNew), false );
	modBtn = AddButton(mainPanel, wxT("Modify.."), wxPoint(225,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonModify), false );
	delBtn = AddButton(mainPanel, wxT("Delete"), wxPoint(300,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonDelete), false );
	settingsBtn = AddButton(mainPanel, wxT("Settings"), wxPoint(375,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonSettings), false);
	tabLeftBtn = AddButton(mainPanel, wxT("<< Tab"), wxPoint(450,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonTabLeft), false);
	tabRightBtn = AddButton(mainPanel, wxT("Tab >>"), wxPoint(525,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonTabRight), false);
	entryUpBtn = AddButton(mainPanel, wxT("Entry Up"), wxPoint(600,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonEntryUp), false);
	entryDownBtn = AddButton(mainPanel, wxT("Entry Down"), wxPoint(675,5), buttonSize, wxCommandEventHandler(MainFrame::OnButtonEntryDown), false);
	
	expandBtn = AddButton(mainPanel, wxT(">>"), wxPoint(150,5), buttonSizeSmall, wxCommandEventHandler(MainFrame::OnButtonExpand) );

	filtertxt = new wxStaticText(mainPanel, wxID_ANY, wxT("Filter:"), wxPoint(675,7));
	filter = new wxTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxPoint(720,5), wxSize(75,20));
	xbtn = new wxButton(filter, wxID_ANY, wxT("X"), wxPoint(52,1), wxSize(20,16), wxBORDER_NONE);
	xbtn->Show(false);
	xbtn->SetBackgroundColour( filter->GetBackgroundColour() );
	xbtn->SetForegroundColour( wxColour(255,25,10) );
	Connect(xbtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnButtonX));
	Connect(filter->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::OnTextFilter));

	bExpandButtons = false;

	Connect(wxEVT_CHAR, wxKeyEventHandler(MainFrame::OnChar));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));
	Connect(wxEVT_SIZE, wxSizeEventHandler(MainFrame::OnResize));

	CreateStatusBar(1);
	InitNotebook();
	LoadDefaultSettings();
	LoadSettings();
	LoadData();
	SelectPage(0);

	Show();
}

void MainFrame::OnChar(wxKeyEvent&)
{
	/* I wanted to try having the menu get shown when user pressed ALT, but I was not successful */
	//if( evt.GetKeyCode() == WXK_ALT )
	//	menubar->Show( !menubar->IsShown() );
}