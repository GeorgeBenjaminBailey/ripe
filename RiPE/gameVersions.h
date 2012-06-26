#ifndef __GAME_VERSIONS_H
#define __GAME_VERSIONS_H

#include "defs.h"

enum GameScriptVersion
{
    OTHER_SENDALL,
    OTHER_SEND,
    OTHER_SENDTO,
    OTHER_WSASEND,
    OTHER_WSASENDTO,
    CUSTOM,
    INVALID_SCRIPT
};

class GameClient
{
  private:
    long      version;
    wxString  scriptName;
    wxString  scriptText;

  public:
    GameClient()
    {
        version = OTHER_SEND;
    }

    GameClient( const GameClient &other )
    {
        version    = other.version;
        scriptName = other.scriptName;
        scriptText = other.scriptText;
    }

    long GetVersion()
    {
        return version;
    }

    void SetVersion( long vers )
    {
        version = vers;
    }

    const wxString &GetScriptName()
    {
        return scriptName;
    }

    void SetScriptName( const wxString &name )
    {
        scriptName = name;
    }

    const wxString &GetScriptText()
    {
        return scriptText;
    }

    void SetScriptText( const wxString &text )
    {
        scriptText = text;
    }
};

#endif //__GAME_VERSIONS_H
