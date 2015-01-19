#pragma once


/** File **
A simple interface for checking if the application has file-access.
It abstracts wxFile.
**/

class wxString;

class File
{
public:
	static bool HasAccess( const wxString &file );
	static bool HasWriteAccess( const wxString &file );
	static bool HasReadAccess( const wxString &file );
};