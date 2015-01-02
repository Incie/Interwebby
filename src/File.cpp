#include"File.h"
#include<wx/log.h>
#include<wx/file.h>

bool File::HasAccess( const wxString &file )
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

bool File::HasWriteAccess( const wxString &file )
{
	wxLogNull PleaseDoNotDisturb;

	wxFile openfile;
	if( !openfile.Open(file, wxFile::write) )
	{
		openfile.ClearLastError();
		return false;
	}

	openfile.Close();
	return true;
}

bool File::HasReadAccess( const wxString &file )
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