#include"MainFrame.h"
#include"DataEntry.h"
#include"tinyxml2.h"
#include<wx/msgdlg.h>
#include<wx/listctrl.h>
#include<wx/notebook.h>
#include"DateTime.h"

#include"File.h"
#include"NewDialog.h"
#include<wx/stattext.h>

using namespace tinyxml2;

wxString GetXMLErrorString(XMLError errorCode)
{
	switch( errorCode )
	{
    case XML_NO_ERROR:
    //case XML_SUCCESS: //Both are 0
		return wxString(wxT("XML_ERROR"));

    case XML_NO_ATTRIBUTE:
		return wxString(wxT("XML_NO_ATTRIBUTE"));
    case XML_WRONG_ATTRIBUTE_TYPE:
		return wxString(wxT("XML_WRONG_ATTRIBUTE_TYPE"));
    case XML_ERROR_FILE_NOT_FOUND:
		return wxString(wxT("XML_ERROR_FILE_NOT_FOUND"));
    case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
		return wxString(wxT("XML_ERROR_FILE_COULD_NOT_BE_OPENED"));
    case XML_ERROR_FILE_READ_ERROR:
		return wxString(wxT("XML_ERROR_FILE_READ_ERROR"));
    case XML_ERROR_ELEMENT_MISMATCH:
		return wxString(wxT("XML_ERROR_ELEMENT_MISMATCH"));
    case XML_ERROR_PARSING_ELEMENT:
		return wxString(wxT("XML_ERROR_PARSING_ELEMENT"));
    case XML_ERROR_PARSING_ATTRIBUTE:
		return wxString(wxT("XML_ERROR_PARSING_ATTRIBUTE"));
    case XML_ERROR_IDENTIFYING_TAG:
		return wxString(wxT("XML_ERROR_IDENTIFYING_TAG"));
    case XML_ERROR_PARSING_TEXT:
		return wxString(wxT("XML_ERROR_PARSING_TEXT"));
    case XML_ERROR_PARSING_CDATA:
		return wxString(wxT("XML_ERROR_PARSING_CDATA"));
    case XML_ERROR_PARSING_COMMENT:
		return wxString(wxT("XML_ERROR_PARSING_COMMENT"));
    case XML_ERROR_PARSING_DECLARATION:
		return wxString(wxT("XML_ERROR_PARSING_DECLARATION"));
    case XML_ERROR_PARSING_UNKNOWN:
		return wxString(wxT("XML_ERROR_PARSING_UNKNOWN"));
    case XML_ERROR_EMPTY_DOCUMENT:
		return wxString(wxT("XML_ERROR_EMPTY_DOCUMENT"));
    case XML_ERROR_MISMATCHED_ELEMENT:
		return wxString(wxT("XML_ERROR_MISMATCHED_ELEMENT"));
    case XML_ERROR_PARSING:
		return wxString(wxT("XML_ERROR_PARSING"));

    case XML_CAN_NOT_CONVERT_TEXT:
		return wxString(wxT("XML_CAN_NOT_CONVERT_TEXT"));
	case XML_NO_TEXT_NODE:
		return wxString(wxT("XML_NO_TEXT_NODE"));
	default:
		break;
	};

	return wxString::Format<int>(wxT("Unknown Error Code: %i"), (int)errorCode );
}



int MainFrame::UpdateEntryProc()
{
	wxString groupName = GetSelectedDesc();
	const DataEntry* currentEntry = listinterface.GetSelectedEntry(groupName);

	if( !currentEntry )
		return -1;

	wxArrayString groupList;
	GenerateGroupList(groupList);

	NewDialog dlg(this, groupList, NDL_MODE_EDIT);
	dlg.SetEntry(*currentEntry);

	if( dlg.ShowModal() == ID_YES )
	{
		DataEntry updatedEntry(*currentEntry);
		dlg.GetEntry(updatedEntry);

		if( updatedEntry.GetGroup().compare(currentEntry->GetGroup()) == 0 )
		{
			if( !currentEntry->CompareName(updatedEntry.GetName()) )
			{
				if( !listinterface.ValidEntryName(updatedEntry.GetName()) )
				{
					wxMessageBox(wxT("Name Already Exists"), wxT("Error editing entry"));
					return 0;
				}
			}

			listinterface.UpdateEntry(*currentEntry, updatedEntry);
		}
		else
		{
			if( !currentEntry->CompareName(updatedEntry.GetName()) )
			{
				if( !listinterface.ValidEntryName(updatedEntry.GetName()) )
				{
					wxMessageBox(wxT("Name Already Exists"), wxT("Error editing entry"));
					return 0;
				}
			}

			int status = listinterface.DeleteSelectionFrom(groupName);
			if( status == DELETE_OK_AND_EMPTY_LIST )
				DeleteSelectedTab();			

			NewData(updatedEntry);
		}
	}

	return 1;
}

bool MainFrame::NewData(const DataEntry &newEntry)
{
	wxString groupName = newEntry.GetGroup();
	
	if( !TabExists(groupName) )
		AddNewTab(newEntry.GetGroup());

	return listinterface.AddNewEntry(groupName, newEntry);
}

void MainFrame::LoadData()
{
	XMLDocument doc;
	XMLError err = doc.LoadFile(szdatapath);

	if( err != XML_NO_ERROR )
	{
		wxString errorString = wxString(GetXMLErrorString(err)) + wxString(wxT(" - Additional Info: ")) + wxString((char*) doc.GetErrorStr1());
		wxString readAccess = File::HasReadAccess(szdatapath) ? wxT("Yes"):wxT("No");
		wxString format = wxT("Could not load file: %s\nXML Error: %s\nRead Access: %s");
		wxString errorReport = wxString::Format<wxString, wxString, wxString>(format, szdatapath, errorString, readAccess);
		wxMessageBox(errorReport, wxT("Error Loading Data"));
		return;
	}

	int entryCount = 0;

	XMLNode *data = doc.FirstChildElement("data");
	XMLNode *column = data->FirstChildElement("column");

	while( column )
	{
		DataEntry newEntry;
		XMLElement *nam = column->FirstChildElement("name");
		if( nam ) newEntry.SetName(nam->GetText());

		XMLElement *des = column->FirstChildElement("desc");
		if( des ) newEntry.SetGroup(des->GetText());

		XMLElement *url = column->FirstChildElement("url");
		if( url ) newEntry.SetURL(url->GetText());

		XMLElement *dat = column->FirstChildElement("date");
		if( dat ) newEntry.SetDateLaunched(dat->GetText());

		XMLElement *added = column->FirstChildElement("added");
		if( added ) newEntry.SetDateAdded(added->GetText());

		XMLElement *launched = column->FirstChildElement("timeslaunched");
		if( launched ) newEntry.SetTimesLaunched(launched->GetText());

		NewData(newEntry);

		entryCount++;
		column = column->NextSiblingElement();
	} 

	wxStatusBar *sb = GetStatusBar();
	wxStaticText *t = new wxStaticText(sb, wxID_ANY, wxT(""));
	t->SetForegroundColour(wxColour(255,0,0));

	wxString status;
	status.Printf(wxT("Loaded %i entries in %i categories"), entryCount, nb->GetPageCount());
	t->SetLabel(status);

	//Todo: Move calculation to a new method: CalculateTopLeft
	wxSize s = t->GetSize();
	wxSize sbs = sb->GetSize();

	wxPoint tp;
	tp.x = 5;
	tp.y = (sbs.y / 2) - (s.y / 2);
	t->SetPosition(tp);

}

XMLElement* CreateElement(XMLDocument &document, const char* elementName, const wxString& data)
{
	XMLElement *element = document.NewElement(elementName);
	XMLText *nameText = document.NewText((const char*)data.mb_str(wxConvUTF8)); 
	element->LinkEndChild(nameText);

	return element;
}

bool MainFrame::SaveData()
{
	unsigned int count = listinterface.GetEntryCount();

	if( count == 0 )
		return true;

	XMLDocument doc;
	XMLElement *root = doc.NewElement( "data" );
	doc.LinkEndChild(root);

	for( unsigned int i = 0; i < listinterface.GetEntryCount(); ++i )
	{
		const DataEntry* entry = listinterface.GetEntryAt(i);

		if( !entry )
		{
			continue;
		}

		XMLElement *column = doc.NewElement("column");
		root->LinkEndChild(column);
	
		column->LinkEndChild( CreateElement(doc, "name", entry->GetName()) );
		column->LinkEndChild( CreateElement(doc, "desc", entry->GetGroup()) );
		column->LinkEndChild( CreateElement(doc, "url", entry->GetURL()) );
		column->LinkEndChild( CreateElement(doc, "date", entry->GetDateLaunched()) );
		column->LinkEndChild( CreateElement(doc, "added", entry->GetDateAdded()) );
		column->LinkEndChild( CreateElement(doc, "timeslaunched", entry->GetTimesLaunched() ) );
	}

	if( doc.SaveFile(szdatapath) )
	{
		bool writeAccess = File::HasWriteAccess(szdatapath);

		if( !writeAccess )
		{
			wxString format = wxT("You do not have access to the following file: \n%s\n\nDo you wish to exit without saving?");
			wxString errorMsg = wxString::Format<wxString>(format, szdatapath);
			wxString errorHeader = wxT("Error while trying to save data");
			int exitWithoutSaving = wxMessageBox(errorMsg, errorHeader, wxYES_NO);

			if(  exitWithoutSaving == wxYES )
				return true;
			else
			{
				wxString infoText = wxT("You can change where to save your data in the Settings\nPress '>>' and Settings\nThere change the data path if you do not have access to the current location");
				wxMessageBox(infoText);
				return false;
			}
		}
	}

	return true;
}


void GetXMLColour(const XMLElement *element, wxColour &colour)
{
	if( element )
	{
		int red = element->UnsignedAttribute("Red");
		int green = element->UnsignedAttribute("Green");
		int blue = element->UnsignedAttribute("Blue");
		colour.Set(red,green,blue);
	}
}

//Settings
void MainFrame::LoadDefaultSettings()
{
	szdatapath = wxT("data.xml");
	bCustomPath = false;
	for( int  i = 0; i < 4; ++i )
		columnWidths[i] = 80;

	ListColours colours;
	colours.rgbLaunchedText.Set(0,0,0);
	colours.rgbLaunchedTextBg.Set(255,255,255);
	colours.rgbFilteredText.Set(50,50,50);
	colours.rgbFilteredBg.Set(255,255,255);
	colours.rgbBackground.Set(255,255,255);
	colours.rgbNormalText.Set(0,0,0);
	colours.rgbNormalTextBg.Set(255,255,255);
	listinterface.SetColours(colours);
}

#include<wx/log.h>
void MainFrame::LoadSettings()
{
	XMLDocument doc;
	XMLError err = doc.LoadFile("settings.xml");

	if( err != XML_SUCCESS )
	{
		wxString xmlError = GetXMLErrorString(err);
		wxString readAccess = File::HasReadAccess(wxT("settings.xml")) ? wxT("Yes") : wxT("No");
		wxString format = wxT("Could not load settings.xml\nXML ErrorCode: %s\nRead Access: %s");
		wxString errorMsg = wxString::Format<wxString, wxString>(format, xmlError, readAccess);
		wxMessageBox(errorMsg, wxT("Error Loading settings.xml"));
		return;
	}

	XMLElement *settings = doc.FirstChildElement("settings");
	if( !settings )
		return;

	XMLElement *columns = settings->FirstChildElement("columns");
	if( columns )
	{
		XMLElement *columnchild = columns->FirstChildElement();
		while( columnchild )
		{
			wxString columnName( columnchild->Attribute("name") );
			int width = columnchild->IntAttribute("width");
			bool enabled = columnchild->BoolAttribute("enabled");
		
			listinterface.SetColumnWidth(columnName, width);
			listinterface.SetColumnStatus(columnName, enabled);

			columnchild = columnchild->NextSiblingElement();
		}
	}

	XMLElement *datapath = settings->FirstChildElement("data-path");
	if( datapath )
	{
		const char *path = datapath->GetText();
		
		if( path )
		{
			szdatapath = path;
			bCustomPath = true;
		}
	}


	XMLElement *window = settings->FirstChildElement("window");
	if( window )
	{
		unsigned int winX = window->UnsignedAttribute("WinX");
		unsigned int winY = window->UnsignedAttribute("WinY");
		unsigned int winW = window->UnsignedAttribute("WinW");
		unsigned int winH = window->UnsignedAttribute("WinH");
		bool maximized = window->BoolAttribute("Maximized");

		//todo: sanitize values?

		if( maximized )
			Maximize();
		else
		{
			SetPosition(wxPoint(winX, winY));
			SetSize(winW, winH);
		}
	}

	XMLElement *colours = settings->FirstChildElement("colours");
	if( colours )
	{
		XMLElement *launch_text = colours->FirstChildElement("launch-text");
		XMLElement *launch_bg = colours->FirstChildElement("launch-bg");
		XMLElement *bg = colours->FirstChildElement("bg");
		XMLElement *normal_text = colours->FirstChildElement("normal-text");
		XMLElement *normal_bg = colours->FirstChildElement("normal-bg");
		XMLElement *filter_text = colours->FirstChildElement("filter-text");
		XMLElement *filter_bg = colours->FirstChildElement("filter-bg");

		ListColours colours;
		listinterface.GetColours(colours);

		GetXMLColour(launch_text, colours.rgbLaunchedText);
		GetXMLColour(launch_bg, colours.rgbLaunchedTextBg);
		GetXMLColour(bg, colours.rgbBackground);
		GetXMLColour(normal_text, colours.rgbNormalText);
		GetXMLColour(normal_bg, colours.rgbNormalTextBg);
		GetXMLColour(filter_text, colours.rgbFilteredText);
		GetXMLColour(filter_bg, colours.rgbFilteredBg);

		listinterface.SetColours(colours);
	}
}

void XMLColour(XMLDocument &document, XMLElement *parent, const char* name, wxColour &colour)
{
	XMLElement *child = document.NewElement(name);
	if( child )
	{
		parent->LinkEndChild(child);

		child->SetAttribute("Red", colour.Red());
		child->SetAttribute("Green", colour.Green());
		child->SetAttribute("Blue", colour.Blue());
	}
}


void MainFrame::SaveSettings()
{
	XMLDocument doc;
	XMLElement *settings = doc.NewElement( "settings" );
	doc.LinkEndChild(settings);

	XMLElement *columns = doc.NewElement( "columns" );
	settings->LinkEndChild( columns );

	const ColumnSettings* columnsettings = listinterface.GetColumnSettings();
	if( columnsettings )
	{
		const ColumnData *coldatas[5];
		coldatas[0] = columnsettings->GetColumnData(wxT("Name"));
		coldatas[1] = columnsettings->GetColumnData(wxT("URL"));
		coldatas[2] = columnsettings->GetColumnData(wxT("Last Launched"));
		coldatas[3] = columnsettings->GetColumnData(wxT("Date Added"));
		coldatas[4] = columnsettings->GetColumnData(wxT("Launched"));

		for( int i = 0; i < 5; ++i )
		{
			const ColumnData *cd = coldatas[i];
			if( !cd )
				continue;
			XMLElement *element = doc.NewElement("column");
			element->SetAttribute("name", cd->name.mb_str(wxConvUTF8));
			element->SetAttribute("width", cd->width);
			element->SetAttribute("enabled", cd->isEnabled);
			columns->LinkEndChild(element);
		}
	}

	XMLElement *datapath = doc.NewElement("data-path");
	settings->LinkEndChild(datapath);

	const char *pdatapath = szdatapath.mb_str(wxConvUTF8);
	XMLText *text = doc.NewText(pdatapath);
	datapath->LinkEndChild(text);

	wxPoint windowPosition = GetPosition();
	wxSize windowSize = GetSize();
	bool maxed = IsMaximized();
	XMLElement *windowInfo = doc.NewElement("window");
	settings->LinkEndChild(windowInfo);

	windowInfo->SetAttribute("WinX", windowPosition.x);
	windowInfo->SetAttribute("WinY", windowPosition.y);
	windowInfo->SetAttribute("WinW", windowSize.x);
	windowInfo->SetAttribute("WinH", windowSize.y);
	windowInfo->SetAttribute("Maximized", maxed);


	ListColours listcolours;
	listinterface.GetColours(listcolours); 

	XMLElement *colours = doc.NewElement("colours");
	settings->LinkEndChild(colours);

	XMLColour(doc, colours, "launch-text", listcolours.rgbLaunchedText);
	XMLColour(doc, colours, "launch-bg", listcolours.rgbLaunchedTextBg);
	XMLColour(doc, colours, "bg", listcolours.rgbBackground);
	XMLColour(doc, colours, "normal-text", listcolours.rgbNormalText);
	XMLColour(doc, colours, "normal-bg", listcolours.rgbNormalTextBg);
	XMLColour(doc, colours, "filter-text", listcolours.rgbFilteredText);
	XMLColour(doc, colours, "filter-bg", listcolours.rgbFilteredBg);

	XMLError err = doc.SaveFile("settings.xml");
	if( err != XML_NO_ERROR )
	{
		bool writeAccess = File::HasWriteAccess(wxT("settings.xml"));

		wxString errormsg = wxString::Format<int, wxString>(wxT("Could not save settings.xml\nTinyXML2 errorcode: %i\nWrite Access to settings.xml: %s"), err, writeAccess?wxT("Yes"):wxT("No"));
		wxString header(wxT("Error saving settings"));
		wxMessageBox(errormsg, header);
		return;
	}
}