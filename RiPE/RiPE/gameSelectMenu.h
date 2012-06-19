#ifndef GAMESELECTMENU_H
#define GAMESELECTMENU_H

#include <algorithm>
#include <vector>

#ifndef WX_PRECOMP
	//(*HeadersPCH(gameSelectMenu)
	#include <wx/listctrl.h>
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/panel.h>
	#include <wx/button.h>
	#include <wx/dialog.h>
	//*)
#endif
//(*Headers(gameSelectMenu)
//*)
#include <wx/msgdlg.h>

#include "../AddPluginDlg.h"
#include "../gameVersions.h"
#include "../PluginSearchDlg.h"

struct GameScript
{
    wxString name; // the name of the script
    wxString script; // the raw text of the script
    GameScriptVersion version;

    GameScript()
    {
        version = CUSTOM;
    }

    GameScript( const wxString &scriptName, const wxString &scriptText )
    {
        name = scriptName;
        script = scriptText;
        version = CUSTOM;
    }

    GameScript( const wxString &scriptName, const GameScriptVersion scriptVersion )
    {
        name = scriptName;
        version = scriptVersion;
    }

    bool operator() (GameScript i, GameScript j)
    {
        return i.name.CompareTo(j.name, wxString::ignoreCase) < 0;
    }
};

struct GamePlugin
{
    wxString name; // the name of the game / program
    std::vector<GameScript> scripts;

    GamePlugin()
    {}

    GamePlugin( const wxString &pluginName )
    {
        name = pluginName;
    }

    GamePlugin( const wxString &pluginName, const GameScript &gameScript )
    {
        name = pluginName;
        scripts.push_back(gameScript);
    }

    bool operator() (GamePlugin i, GamePlugin j)
    {
        return i.name.CompareTo(j.name, wxString::ignoreCase) < 0;
    }
};

class gameSelectMenu: public wxDialog
{
	public:

		gameSelectMenu(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~gameSelectMenu();

		//(*Declarations(gameSelectMenu)
		wxListCtrl* ListCtrl1;
		wxStaticText* StaticText2;
		wxButton* Button1;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* Button2;
		wxListCtrl* ListCtrl2;
		wxButton* Button3;
		//*)
        AddPluginDlg *apDlg;
        PluginSearchDlg *psdDlg;

        // returns a struct containing the game title and version of the game
        // that RiPE is injected into.  If the game can't be automatically
        // detected, a dialog menu will prompt the user to manually enter
        // the game that they're playing.  Returns a struct containing NOT_FOUND
        // for GameTitle and 0 for version.
        GameClient GetGame()
        {
            ShowModal();
            return curGame;
        }

        GameScriptVersion GetScriptVersion(const int pluginIndex, const int scriptIndex);
        const wxString & GetScriptText(const int pluginIndex, const int scriptIndex) const;
        void LoadPlugins();
        void DisplayPlugins();

	protected:

		//(*Identifiers(gameSelectMenu)
		static const long ID_STATICTEXT1;
		static const long ID_BUTTON3;
		static const long ID_BUTTON2;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_LISTCTRL1;
		static const long ID_LISTCTRL2;
		static const long ID_BUTTON1;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(gameSelectMenu)
		void OnButton1Click(wxCommandEvent& event);
		void OnListBox1Select(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnListCtrl1ItemSelect(wxListEvent& event);
		void OnListCtrl2BeginDrag(wxListEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		//*)
		void UpdateGameVersion();
        void LoadCustomPlugins();
        void LoadDefaultPlugins();
        void DisplayScripts();
        void Init();
        GameClient curGame;

        std::vector<GamePlugin> gamePlugins;

		DECLARE_EVENT_TABLE()
};

#endif
