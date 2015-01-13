#pragma once


#include<wx/frame.h>
#include<vector>
#include"ListInterface.h"

class wxListCtrl;
class wxButton;
class wxPanel;
class wxNotebook;
class wxListEvent;
class wxTextCtrl;
class wxStaticText;
class wxMenuBar;
class DataEntry;

struct ButtonData;

class MainFrame : public wxFrame
{
public:
	MainFrame();

private:
	wxButton* AddButton(wxWindow* parent, const wxPoint &position, const ButtonData &data);
	void OnChar(wxKeyEvent &evt);
	void OnButtonDelete(wxCommandEvent &evt);
	void OnButtonLaunch(wxCommandEvent &evt);
	void OnButtonLaunchAll(wxCommandEvent &evt);
	void OnButtonNew(wxCommandEvent &evt);
	void OnButtonModify(wxCommandEvent &evt);
	void OnButtonExpand(wxCommandEvent &evt);
	void OnButtonSettings(wxCommandEvent &evt);

	void OnButtonTabRight(wxCommandEvent &evt);
	void OnButtonTabLeft(wxCommandEvent &evt);
	void OnButtonEntryUp(wxCommandEvent &evt);
	void OnButtonEntryDown(wxCommandEvent &evt);

	void OnTextFilter(wxCommandEvent &evt);
	void OnButtonX(wxCommandEvent &evt);
	void OnClose(wxCloseEvent &evt);
	void OnResize(wxSizeEvent & evt);
	void OnColumnResize(wxListEvent &evt);

	bool NewData(const DataEntry &newEntry);
	
	int UpdateEntryProc();

	void LoadData();
	void LoadDefaultSettings();
	void LoadSettings();
	bool SaveData();
	void SaveSettings();

	wxString GetSelectedDesc();

	void InitNotebook(const wxPoint &position);
	void MoveTab(int direction);
	bool GenerateGroupList( wxArrayString &groupList );
	wxListCtrl* AddNewTab( const wxString &name, int insertIndex = -1 );
	bool TabExists( const wxString &name );
	void DeleteSelectedTab();
	void SelectPage(int page);
	void SelectPage(const wxString &groupName);

	wxNotebook *nb;
	wxPanel *mainPanel;
	wxButton *newBtn, *modBtn, *delBtn, *expandBtn, *settingsBtn;
	wxButton *tabLeftBtn, *tabRightBtn, *entryUpBtn, *entryDownBtn;

	wxButton *xbtn;
	wxTextCtrl *filter;
	wxStaticText *filtertxt;
	wxMenuBar *menubar;

	bool bExpandButtons;

	wxSize winSize;
	wxPoint winPos;

	bool bCustomPath;
	wxString szdatapath;

	int columnWidths[4];

	ListInterface listinterface;
};