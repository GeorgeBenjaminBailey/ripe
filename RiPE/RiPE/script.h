#ifndef __RIPE_SCRIPT_H
#define __RIPE_SCRIPT_H

#include <list>
#include <map>
#include <stack>
#include <vector>

#include <boost/thread.hpp>

#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include <wx/string.h>

#include "../RiME/memscan.hpp"
#include "../breakpoint.h" // Needed to set DRs
#include "../hexconverter.hpp"
#include "../memreadwrite.h"
#include "../millitime.h"
#include "../usefulFunctions.h"
#include "hookInterface.h"
#include "operator.h"
#include "script_variable.h"

// This namespace contains structures used for parsing
namespace Script
{
    // performs the adding of a token to the list.  Splits up combined tokens as needed
    void AddTokens( std::list<Token> &tokens, const wxString &line );

    struct PacketProcessStruct
    {
        std::string pkt;
        HookInterface *hIntf;
        sendPacketType pktTyp;
        SOCKET s;

        PacketProcessStruct()
        {}

        PacketProcessStruct(const std::string &packet, HookInterface *hkIntf, sendPacketType pktType, SOCKET sock)
        {
            pkt    = packet;
            hIntf  = hkIntf;
            pktTyp = pktType;
            s      = sock;
        }
    };

    struct Container
    {
        enum Type
        {
            TYPE_BLOCK,
            TYPE_LINE
        };
        Type type;

        // Using pseudo-polymorphism so I don't have to deal with memory management
        std::list<Container> containers;
        std::list<Token> tokens;
    };

    struct Line : Container
    {
        Line()
        {
            type = TYPE_LINE;
        }

        Line( const wxString &line )
        {
            type = TYPE_LINE;
            AddTokens(tokens, line);
        }
    };

    struct Block : Container
    {
        Block()
        {
            type = TYPE_BLOCK;
        }
    };

    struct Function 
    {
        Block block;
        std::list<wxString> params; // the function's parameters

        Function()
        {
        }

        Function(Block b)
        {
            block = b;
        }
    };

    struct Event
    {
        wxString type; // s, r, time, k, etc.
        Block    block;
        //Line expression; // the expression that must be true for this event
        wxString expression; // TODO:  Make this dynamic
        int      timer;

        Event()
        {
            timer = 0;
        }

        Event(const wxString &typ, const Block &b, const wxString &exp)
        {
            type       = typ;
            block      = b;
            expression = exp;
            timer      = 0;
        }
    };

    // Used to keep track of the function calls, the function names,
    // and the callStack before the function was called
    struct FunctionCall
    {
        wxString               name;
        std::vector<TokenEval> callStack;
        unsigned long          location; // location of the function.  Needed for near calls

        FunctionCall()
        {}

        FunctionCall( const wxString &nam, const std::vector<TokenEval> &callStk, unsigned long loc)
        {
            name      = nam;
            callStack = callStk;
            location  = loc;
        }
    };

    class Worker
    {
    public:
        Worker()
        {
            working = true;
            m_mutex  = boost::shared_ptr<boost::mutex>(new boost::mutex());
            m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Worker::Work, this)));
        }

        ~Worker()
        {
            m_mutex->lock();
            working = false;
            m_mutex->unlock();

            m_thread->join();
        }

        void Work();

        void AddPacket( const PacketProcessStruct &pkt );

        void SendPackets();

    private:
        boost::shared_ptr<boost::mutex> m_mutex;
        boost::shared_ptr<boost::thread> m_thread;
        std::list<PacketProcessStruct> packets;
        bool working;
    };

    class Script
    {
    private:
        Worker m_worker;
        Block block;
        Breakpoint *drPtr;
        std::map<wxString, Variable> globals;
        std::map<wxString, Function> functions;
        std::map<wxString, Function> defaultFunctions; // pre-made functions
        std::list<Event> events; // event blocks
        bool enabled;
        bool timeEventExecuting; // sort of like a mutex :)
        int  lastTime;
        wxString text; // the raw script text, before any formatting.  Used in saving.
        wxString scriptName; // the name of the script
        wxString hookRecvFunction;
        wxString hookSendFunction;
        wxString packetRecvFunction;
        wxString packetSendFunction;

        // Supported calling conventions
        enum
        {
            CC_CDECL,
            CC_MSFASTCALL,
            CC_BORLANDFASTCALL,
            CC_SYSCALL,
            CC_GCCTHISCALL,
            CC_MSTHISCALL,
            CC_STDCALL
        };

    public:
        Script();

        Script(const wxString &scriptNam, Block b, const wxString &txt, Breakpoint *drP);

        ~Script()
        {   
        }

        Script(const Script &other);

        void Init();

        // Extract functions from the block variable.  Should only be called once.
        void ExtractFunctions();

        // TODO:  Correct / finish this.  It's basically a copy of ExtractFunctions atm
        void ExtractEvents();

        void ExecuteNearCall( unsigned long addr, 
            std::map<wxString, TokenEval> &parentLocals, std::vector<TokenEval> &callStack );

        void ExecuteSpoofReturnCall( unsigned long addr, 
            std::map<wxString, TokenEval> &parentLocals, std::vector<TokenEval> &callStack );

        void ExecuteFunction( const wxString &name, std::map<wxString, TokenEval> &parentLocals );

        TokenEval ExecuteExpression( Line *line, std::map<wxString, Variable *> &parentLocals, 
            std::map<wxString, Variable> &myLocals, int depth=0 );

        bool ExecuteLineSEH( Line *line, std::map<wxString, Variable *> &parentLocals,
            std::map<wxString, Variable> &myLocals );

        bool ExecuteLine(Block &b, std::map<wxString, Variable *> &parentLocals, 
            std::map<wxString, Variable> &myLocals, int depth=0 );

        void ExecuteBlock(Block &b, std::map<wxString, Variable *> &parentLocals, int depth=0);

        void FunctionCallHelper( const wxString &functionName, std::vector<TokenEval> &callStack, int depth );

        void ProcessDisableEvent();
        void ProcessEnableEvent();

        void ProcessKeyboardEvent( unsigned int vkCode );

        void ProcessHookRecvEvent( const bool enable );

        void ProcessHookSendEvent( const bool enable );

        // Used for DR, Int3, and other hooks.  This is as low-level as RiPEST goes.
        // No processing is done to the registers before or after this function is called.
        // Any changes made here will reflect in the appropriate register.
        void ProcessLogEvent( const wxString &functionName, std::map<wxString, Variable *> &registers );

        void ProcessPacketRecvEvent( const char *cstrPacket, SOCKET s );

        void ProcessPacketSendEvent( const char *cstrPacket, SOCKET s );

        void ProcessRecvEvent( std::vector<unsigned char> &packet );

        void ProcessSendEvent( std::vector<unsigned char> &packet );

        // Protip:  Don't screw around with the time variable while you're
        //          in an event... that would be stupid.
        void ProcessTimeEvent( int t );

        const wxString & GetText() const;
    };

    // Houses the scripts used for RiPEST, such as 7-miss, etc.
    class ScriptManager
    {
    private:
        std::map<wxString, Script> scripts;
        std::map<wxString, Script> activeScripts;
        Breakpoint *drPtr;

        Block ParseScript( const wxString &src );

        Block ParseScriptHelper( wxString &src, size_t depth );

        // returns the depth of the line
        size_t GetLineDepth( const wxString &line );

    public:
        ScriptManager();

        ~ScriptManager()
        {
            if( drPtr != NULL )
                delete drPtr;
        }

        bool AddScript( const wxString &name, const wxString &text );

        bool AddScript(const wxString &name, const wxString &text, Block block);

        void RemoveScript(const wxString &name);

        void ToggleScript(const wxString &name, const bool enable);

        void SetHookInterface( HookInterface *hiPtr );

        void ProcessDisableEvent( const wxString &script );

        void ProcessEnableEvent( const wxString &script );

        void ProcessHookRecvEvent( const wxString &script, const bool enable );

        void ProcessHookSendEvent( const wxString &script, const bool enable );

        void ProcessKeyboardEvent( unsigned int vkCode );

        // Used for DR, Int3, and other hooks.  This is as low-level as RiPEST goes.
        // No processing is done to the registers before or after this function is called.
        // Any changes made here will reflect in the appropriate register.
        void ProcessLogEvent( const wxString &scriptName, const wxString &functionName, 
            std::map<wxString, Variable *> &registers );

        void ProcessPacketRecvEvent( const wxString &script, const char *cstrPacket, const SOCKET s );

        void ProcessPacketSendEvent( const wxString &script, const char *cstrPacket, const SOCKET s );

        void ProcessRecvEvent( std::vector<unsigned char> &packet );

        void ProcessSendEvent( std::vector<unsigned char> &packet );

        void ProcessTimeEvent();

        // Saves the scripts in a /script folder
        void SaveScripts();

        // Deletes a script in the /script folder
        void DeleteScript( const wxString &name );

        // Loads script from filename
        bool LoadScript( const wxString &filename, const wxString &name );
    };
}

#endif //__RIPE_SCRIPT_H
