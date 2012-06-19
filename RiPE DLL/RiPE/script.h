#ifndef __RIPE_SCRIPT_H
#define __RIPE_SCRIPT_H

#include <list>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include <boost/thread.hpp>

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
    void AddTokens( std::list<Token> &tokens, const std::string &line );

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

        Line( const std::string &line )
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
        std::list<std::string> params; // the function's parameters

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
        std::string type; // s, r, time, k, etc.
        Block    block;
        //Line expression; // the expression that must be true for this event
        std::string expression; // TODO:  Make this dynamic
        int      timer;

        Event()
        {
            timer = 0;
        }

        Event(const std::string &typ, const Block &b, const std::string &exp)
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
        std::string               name;
        std::vector<TokenEval> callStack;
        unsigned long          location; // location of the function.  Needed for near calls

        FunctionCall()
        {}

        FunctionCall( const std::string &nam, const std::vector<TokenEval> &callStk, unsigned long loc)
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
        std::map<std::string, Variable> globals;
        std::map<std::string, Function> functions;
        std::map<std::string, Function> defaultFunctions; // pre-made functions
        std::list<Event> events; // event blocks
        bool enabled;
        bool timeEventExecuting; // sort of like a mutex :)
        int  lastTime;
        std::string text; // the raw script text, before any formatting.  Used in saving.
        std::string scriptName; // the name of the script
        std::string hookRecvFunction;
        std::string hookSendFunction;
        std::string packetRecvFunction;
        std::string packetSendFunction;

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

        Script(const std::string &scriptNam, Block b, const std::string &txt);

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
            std::map<std::string, TokenEval> &parentLocals, std::vector<TokenEval> &callStack );

        void ExecuteSpoofReturnCall( unsigned long addr, 
            std::map<std::string, TokenEval> &parentLocals, std::vector<TokenEval> &callStack );

        void ExecuteFunction( const std::string &name, std::map<std::string, TokenEval> &parentLocals );

        TokenEval ExecuteExpression( Line *line, std::map<std::string, Variable *> &parentLocals, 
            std::map<std::string, Variable> &myLocals, int depth=0 );

        bool ExecuteLineSEH( Line *line, std::map<std::string, Variable *> &parentLocals,
            std::map<std::string, Variable> &myLocals );

        bool ExecuteLine(Block &b, std::map<std::string, Variable *> &parentLocals, 
            std::map<std::string, Variable> &myLocals, int depth=0 );

        void ExecuteBlock(Block &b, std::map<std::string, Variable *> &parentLocals, int depth=0);

        void FunctionCallHelper( const std::string &functionName, std::vector<TokenEval> &callStack, int depth );

        void ProcessDisableEvent();
        void ProcessEnableEvent();

        void ProcessKeyboardEvent( unsigned int vkCode );

        void ProcessHookRecvEvent( const bool enable );

        void ProcessHookSendEvent( const bool enable );

        // Used for DR, Int3, and other hooks.  This is as low-level as RiPEST goes.
        // No processing is done to the registers before or after this function is called.
        // Any changes made here will reflect in the appropriate register.
        void ProcessLogEvent( const std::string &functionName, std::map<std::string, Variable *> &registers );

        void ProcessPacketRecvEvent( const char *cstrPacket, SOCKET s );

        void ProcessPacketSendEvent( const char *cstrPacket, SOCKET s );

        void ProcessRecvEvent( std::vector<unsigned char> &packet );

        void ProcessSendEvent( std::vector<unsigned char> &packet );

        // Protip:  Don't screw around with the time variable while you're
        //          in an event... that would be stupid.
        void ProcessTimeEvent( int t );

        const std::string & GetText() const;
    };

    // Houses the scripts used for RiPEST, such as 7-miss, etc.
    class ScriptManager
    {
    private:
        std::map<std::string, Script> scripts;
        std::map<std::string, Script> activeScripts;

        Block ParseScript( const std::string &src );

        Block ParseScriptHelper( std::string &src, size_t depth );

        // returns the depth of the line
        size_t GetLineDepth( const std::string &line );

    public:
        ScriptManager();

        ~ScriptManager()
        {
        }

        bool AddScript( const std::string &name, const std::string &text );

        bool AddScript(const std::string &name, const std::string &text, Block block);

        void RemoveScript(const std::string &name);

        void ToggleScript(const std::string &name, const bool enable);

        void SetHookInterface( HookInterface *hiPtr );

        void ProcessDisableEvent( const std::string &script );

        void ProcessEnableEvent( const std::string &script );

        void ProcessHookRecvEvent( const std::string &script, const bool enable );

        void ProcessHookSendEvent( const std::string &script, const bool enable );

        void ProcessKeyboardEvent( unsigned int vkCode );

        // Used for DR, Int3, and other hooks.  This is as low-level as RiPEST goes.
        // No processing is done to the registers before or after this function is called.
        // Any changes made here will reflect in the appropriate register.
        void ProcessLogEvent( const std::string &scriptName, const std::string &functionName, 
            std::map<std::string, Variable *> &registers );

        void ProcessPacketRecvEvent( const std::string &script, const char *cstrPacket, const SOCKET s );

        void ProcessPacketSendEvent( const std::string &script, const char *cstrPacket, const SOCKET s );

        void ProcessRecvEvent( std::vector<unsigned char> &packet );

        void ProcessSendEvent( std::vector<unsigned char> &packet );

        void ProcessTimeEvent();

        // Saves the scripts in a /script folder
        void SaveScripts();

        // Deletes a script in the /script folder
        void DeleteScript( const std::string &name );

        // Loads script from filename
        bool LoadScript( const std::string &filename, const std::string &name );
    };
}

#endif //__RIPE_SCRIPT_H
