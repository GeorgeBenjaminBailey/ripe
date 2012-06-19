#ifndef ENUMMODULESDLG_H
#define ENUMMODULESDLG_H

//STL includes
#include <list>
#include <set>

//(*Headers(enumModulesDlg)
#include <wx/srchctrl.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
//*)

//Windows includes
#include <windows.h>
#include <DbgHelp.h>
#include <Psapi.h>

struct EnumModulesSymbolInfo
{
    ULONG64  addr;
    ULONG    size;
    wxString name;

    // needed for set comparison
    bool operator<( const EnumModulesSymbolInfo &other ) const
    {
        if( addr != other.addr )
            return addr < other.addr;
        else
            return name.CmpNoCase(other.name) < 0 ? true : false;
    }
};

struct EnumModuleInfo
{
    HMODULE                         handle;
    wxString                        moduleName;
    std::set<EnumModulesSymbolInfo> symInfoArr;

    // needed for set comparison
    bool operator<( const EnumModuleInfo &other ) const
    {
        if( handle != other.handle )
            return handle < other.handle;
        else
            return moduleName.CmpNoCase( other.moduleName ) < 0 ? true : false;
    }
};

class enumModulesDlg: public wxDialog
{
	public:

		enumModulesDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~enumModulesDlg();

		//(*Declarations(enumModulesDlg)
		wxTreeCtrl* TreeCtrl1;
		wxSearchCtrl* SearchCtrl1;
		//*)

        static int  GetAllModules( DWORD processID );

        static void GetModuleSymbols( PCSTR imageName, EnumModuleInfo *userContext );

        static BOOL CALLBACK EnumSymProc( 
            PSYMBOL_INFO pSymInfo,
            ULONG SymbolSize,
            PVOID UserContext );

        // list all modules and symbols from the moduleSymbols vector
        void ListAllSymbols();

        void ListSymbolsContaining( wxString substr );

        static std::list<EnumModulesSymbolInfo> GetSymbolsContaining( wxString substr, wxString moduleSubstr = _("") );

	protected:

		//(*Identifiers(enumModulesDlg)
		static const long ID_SEARCHCTRL1;
		static const long ID_TREECTRL1;
		//*)

	private:

		//(*Handlers(enumModulesDlg)
		void OnSearchCtrl1Text(wxCommandEvent& event);
		//*)

        // a vector of all modules found
        static std::set<EnumModuleInfo> moduleSymbols;

		DECLARE_EVENT_TABLE()
};

#endif
