/***************************************************************
 * Name:      RiMEApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Ryan "Riukuzaki" Cornwall ()
 * Created:   2010-08-04
 * Copyright: Ryan "Riukuzaki" Cornwall ()
 * License:
 **************************************************************/

#include "stdafx.h"
//#include "wx_pch.h"
#include "RiMEApp.h"

//(*AppHeaders
#include "../RiME.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(RiMEApp);

bool RiMEApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	RiME *RiMEPtr = new RiME(0);
    	RiMEPtr->Show();
    	SetTopWindow( RiMEPtr );
    }
    //*)
    return wxsOK;
}
