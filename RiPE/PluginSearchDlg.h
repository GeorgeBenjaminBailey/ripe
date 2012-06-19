#ifndef PLUGINSEARCHDLG_H
#define PLUGINSEARCHDLG_H

#include <map>

//(*Headers(PluginSearchDlg)
#include <wx/srchctrl.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/radiobut.h>
#include <wx/panel.h>
#include <wx/button.h>
#include "asmeditor.h"
#include <wx/dialog.h>
//*)

#include "UploadScriptDlg.h"

struct ScriptNode
{
    bool exists;
    wxString script;
    wxString author;
    wxString downloads;

    ScriptNode()
    {
        exists = false;
        script = "";
        author = "";
        downloads = "";
    }
};

class PluginSearchDlg: public wxDialog
{
	public:

		PluginSearchDlg(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~PluginSearchDlg();

		//(*Declarations(PluginSearchDlg)
		wxSearchCtrl* SearchCtrlScripts;
		wxStaticText* StaticTextDownloads;
		wxRadioButton* RadioButtonScripts;
		wxRadioButton* RadioButtonPlugins;
		wxStaticText* StaticTextAuthor;
		wxButton* Button1;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* Button2;
		AsmEditor* Script;
		wxButton* Button3;
		wxStaticLine* StaticLine1;
		wxTreeCtrl* TreeCtrl1;
		//*)
        UploadScriptDlg *usDlg;

	protected:

		//(*Identifiers(PluginSearchDlg)
		static const long ID_BUTTON3;
		static const long ID_SEARCHCTRL1;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_TREECTRL1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICLINE1;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		static const long ID_CUSTOM1;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(PluginSearchDlg)
		void OnRadioButtonScriptsSelect(wxCommandEvent& event);
		void OnRadioButtonPluginsSelect(wxCommandEvent& event);
		void OnSearchCtrlScriptsText(wxCommandEvent& event);
		void OnTreeCtrl1SelectionChanged(wxTreeEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		//*)

        void LoadAllScripts();
        void LoadAllPlugins();

        void AddScript();
        void AddPlugin();

        void LoadFromMap( std::map<wxString, std::map<wxString, 
            ScriptNode> > &scriptMap );

        void QueryScript( const wxString &processName,
            const wxString &scriptName, ScriptNode &node );

        void DisplayNode( ScriptNode &node );

        // scripts[processName][scriptName]
        std::map<wxString, std::map<wxString, ScriptNode> > scripts;
        // plugins[processName][version]
        std::map<wxString, std::map<wxString, ScriptNode> > plugins;

        wxString curScriptName;
        wxString curProcessName;

		DECLARE_EVENT_TABLE()
};

#endif
