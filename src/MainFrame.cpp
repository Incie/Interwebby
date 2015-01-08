#include"MainFrame.h"

#include<wx/stattext.h>
#include<wx/textctrl.h>
#include<wx/panel.h>
#include<wx/button.h>
#include<wx/listctrl.h>
#include<wx/panel.h>

#include<wx/menu.h>

#ifdef _DEBUG
#define TITLESTRING wxT("Interwebby 1.0.1 (Debugbuild) ")
#else
#define TITLESTRING wxT("Interwebby 1.0.1")
#endif


#include<wx/dcclient.h>

struct ButtonData
{
	ButtonData( const wxString &title, wxObjectEventFunction eventFunc, wxButton **buttonPointer, bool isShown = true )
		:title(title), eventFunc(eventFunc), buttonPointer(buttonPointer), isShown(isShown)
	{}

	wxString title;
	wxObjectEventFunction eventFunc;
	wxButton **buttonPointer;
	bool isShown;
};

wxButton* MainFrame::AddButton(wxWindow* parent, const wxPoint &position, const ButtonData &data)
{
	wxButton *btn = new wxButton(parent, wxID_ANY, data.title, position);

	//testing getting string-width
	wxClientDC dc(btn);
	dc.SetFont( btn->GetFont() );
	wxSize stringsize = dc.GetTextExtent( data.title );

	btn->SetSize(wxSize(stringsize.x + 12, 20));

	if( btn )
	{
		Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, data.eventFunc);

		if( !data.isShown )
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

	ButtonData buttons[] = {
		ButtonData(wxT("Launch"), wxCommandEventHandler(MainFrame::OnButtonLaunch), 0),
		ButtonData(wxT("Launch All"), wxCommandEventHandler(MainFrame::OnButtonLaunchAll), 0),
		ButtonData(wxT("New.."), wxCommandEventHandler(MainFrame::OnButtonNew), &newBtn, false),
		ButtonData(wxT("Modify.."), wxCommandEventHandler(MainFrame::OnButtonModify), &modBtn, false),
		ButtonData(wxT("Delete"), wxCommandEventHandler(MainFrame::OnButtonDelete), &delBtn, false),
		ButtonData(wxT("Settings"), wxCommandEventHandler(MainFrame::OnButtonSettings), &settingsBtn, false),
		ButtonData(wxT("<< Tab"), wxCommandEventHandler(MainFrame::OnButtonTabLeft), &tabLeftBtn, false),
		ButtonData(wxT("Tab >>"), wxCommandEventHandler(MainFrame::OnButtonTabRight), &tabRightBtn, false),
		ButtonData(wxT("Entry ^"), wxCommandEventHandler(MainFrame::OnButtonEntryUp), &entryUpBtn, false),
		ButtonData(wxT("Entry v"), wxCommandEventHandler(MainFrame::OnButtonEntryDown), &entryDownBtn, false)
	};

	wxPoint pos(0,5);
	int adjustment = 3;
	int numButtons = sizeof(buttons) / sizeof(ButtonData);
	for( int i = 0; i < numButtons; ++i )
	{
		ButtonData &buttonData = buttons[i];
		wxButton *button = AddButton(mainPanel, pos, buttonData);

		if( buttonData.buttonPointer )
			(*buttonData.buttonPointer) = button;
		
		pos.x += button->GetSize().x + adjustment;
	}

	ButtonData buttonExpand(wxT(">>"), wxCommandEventHandler(MainFrame::OnButtonExpand), 0, true);
	expandBtn = AddButton(mainPanel, newBtn->GetPosition(), buttonExpand );

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