#include"File.h"
#include<wx/log.h>
#include<wx/file.h>

bool File::HasAccess( const wxString& file )
{
	wxFile openfile;
	if( !openfile.Open(file, wxFile::read_write) )
	{
		openfile.ClearLastError();
		return false;
	}

	openfile.Close();
	return true;
}

bool File::HasWriteAccess( const wxString& file )
{
	wxLogNull PleaseDoNotDisturb;

	bool fileExists = wxFile::Exists(file);

	wxFile openfile;
	if( !openfile.Open(file, wxFile::write_append) )
	{
		openfile.ClearLastError();
		return false;
	}

	openfile.Close();

	if( !fileExists )
		wxRemoveFile(file);
	return true;
}

bool File::HasReadAccess( const wxString& file )
{
	wxLogNull PleaseDoNotDisturb;

	wxFile openfile;
	if( !openfile.Open(file, wxFile::read) )
	{
		openfile.ClearLastError();
		return false;
	}

	openfile.Close();
	return true;
}

bool File::Exists( const wxString& file )
{
	return wxFile::Exists(file);
}

bool File::ContainsCWD( const wxString& path )
{
	wxString cwd = wxGetCwd();
	if( path.StartsWith(cwd) )
		return true;
	return false;
}

void File::TrimCWD( wxString& path )
{
	wxString cwd = wxGetCwd();
	if( path.StartsWith(cwd) )
	{
		path = path.Remove(0, cwd.length());

		if( path.StartsWith(wxT("/")) || path.StartsWith(wxT("\\")) )
			path = path.Remove(0,1);
	}
}