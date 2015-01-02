#include<wx/app.h>
#include"MainFrame.h"

//#define ENABLE_VLD
#ifdef ENABLE_VLD
#ifdef _DEBUG
//This has a big chance of breaking the Open file dialog in Settings
#include<vld\vld.h>
#pragma comment(lib, "vld.lib")
#endif
#endif



class EntryPoint : public wxApp 
{
public:
	EntryPoint();
	bool OnInit();
};

IMPLEMENT_APP( EntryPoint );


EntryPoint::EntryPoint()
{
}

bool EntryPoint::OnInit()
{
	new MainFrame();
	return true;
}