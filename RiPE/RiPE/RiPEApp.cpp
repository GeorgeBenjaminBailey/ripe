/***************************************************************
 * Name:      RiPEApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Ryan "Riukuzaki" Cornwall ()
 * Created:   2010-08-04
 * Copyright: Ryan "Riukuzaki" Cornwall ()
 * License:
 **************************************************************/

//#include "wx_pch.h"
#include "stdafx.h"
#include "RiPEApp.h"

//(*AppHeaders
#include "../RiPE.h"
#include <wx/image.h>
//*)

// Initialize the static variable
RiPE *RiPEApp::RiPEPtr = NULL;
wxString RiPEApp::filePath = "";

IMPLEMENT_APP(RiPEApp);

bool RiPEApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	RiPEPtr = new RiPE(0);
    	RiPEPtr->Show();
    	SetTopWindow(RiPEPtr);
    }
    //*)
    return wxsOK;
}
