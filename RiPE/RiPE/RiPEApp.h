/***************************************************************
 * Name:      RiPEApp.h
 * Purpose:   Defines Application Class
 * Author:    Ryan "Riukuzaki" Cornwall ()
 * Created:   2010-10-23
 * Copyright: Ryan "Riukuzaki" Cornwall ()
 * License:
 **************************************************************/

#ifndef RIPEAPP_H
#define RIPEAPP_H

#include <wx/app.h>

class RiPE;

class RiPEApp : public wxApp
{
    private:
        static RiPE *RiPEPtr;
        static wxString filePath;

    public:
        virtual bool OnInit();

        static RiPE * GetRiPE()
        {
            return RiPEPtr;
        }

        // Used by RiPE to prematurely set the RiPEPtr.  This is needed,
        // because RiPE * isn't assigned until AFTER the ctor fires off
        // for RiPE.
        static void SetRiPE( RiPE *ripe )
        {
            RiPEPtr = ripe;
        }

        // Returns the fully qualified location of the DLL
        static const wxString GetFilePath()
        {
            return filePath;
        }

        static void SetFilePath( const wxString &path )
        {
            filePath = path;
        }
};

#endif // RIPEAPP_H
