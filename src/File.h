#pragma once


class wxString;

class File
{
public:
	static bool HasAccess( const wxString &file );
	static bool HasWriteAccess( const wxString &file );
	static bool HasReadAccess( const wxString &file );
};