#ifndef __RIPE_DLL_H
#define __RIPE_DLL_H

#include "defs.h"

#include <list>
#include <map>
#include <queue>

#include "RiPE/hookinterface.h"
#include "RiPE/script.h"

#include "packetitem.h"

namespace RiPE
{
    struct BlockPacket {
        std::string type;    // the type of the packet (Block, Ignore, Ntercept, ...)
        std::string packet1; // the block portion of the packet
        std::string packet2; // the modify portion of the packet

        BlockPacket() {
        }

        BlockPacket(const std::string &type, const std::string &packet1, const std::string &packet2) {
            this->type    = type;
            this->packet1 = packet1;
            this->packet2 = packet2;
        }
    };

    struct LogPacket {
        std::string returnAddress;
        std::string size;
        int      socket;     // the socket number if applicable
        std::string opcode;     // the opcode of the packet
        std::string packet;     // the "formatted" packet displayed in Tree / List view
        std::string packetData; // the "raw" packet returned by double-clicking the packet

        LogPacket() {
            socket = 0;
        }

        LogPacket(const std::string &returnAddress, 
            const std::string &size, const int socket, 
            const std::string &opcode, const std::string &packet, 
            const std::string &packetData) {

            this->returnAddress = returnAddress;
            this->size   = size;
            this->socket = socket;
            this->opcode = opcode;
            this->packet = packet;
            this->packetData = packetData;
        }
    };
    
    typedef void (*OpcodeFunction)(const std::string &message);

    // The main object used to listen for events and enable hacks.
    // This class uses a singleton design pattern.
    class HackDLL
    {
    public:
        static HackDLL *GetInstance();

        // Begins processing events
        void Run();

        // Stop running
        void Close();

        // Processes a message from the pipe
        void ProcessPipeMessage( char *chBuf, DWORD cbRead );

        // Sends a message through the pipe
        void SendMessageToPipe( const std::string &message );

        // Attempts to clear the m_message_buffer of messages.
        // Call this when a new pipe is connected
        void SendBufferedMessages();

        // Populates the opcode functions used in ProcessPipeMessage
        void PopulateOpcodes();

        // Getters / Setters
        HWND GetProcessHWND();
        void SetProcessHWND( HWND hwnd );

        Script::ScriptManager GetScriptManager();
        HookInterface * GetHookInterface();

        void SetHookInterface(const std::string &object);
        void HookSend(const bool enable);
        void HookRecv(const bool enable);
        void PacketSend(const std::string &packet, SOCKET s);
        void PacketRecv(const std::string &packet, SOCKET s);
        void AddPacketToSBlock(const BlockPacket &packet);
        void AddPacketToRBlock(const BlockPacket &packet);
        void RemovePacketFromSBlock(const BlockPacket &packet);
        void RemovePacketFromRBlock(const BlockPacket &packet);
        void AddScript(const std::string &scriptName, const std::string &scriptData);
        void RemoveScript(const std::string &scriptName);
        void DeleteScript(const std::string &scriptName);
        void ToggleScript(const bool enable, const std::string &scriptName);
        void SaveScripts();
        SOCKET GetSocket();
        void SetSocket(const SOCKET s);

        void ProcessSendEvent(std::vector<unsigned char> &packet);
        void ProcessRecvEvent(std::vector<unsigned char> &packet);
        void ProcessHookRecvEvent( const std::string &scriptName, const bool enable);
        void ProcessHookSendEvent( const std::string &scriptName, const bool enable);
        void ProcessPacketRecvEvent(const std::string &scriptName, const char *cstrPacket, SOCKET s);
        void ProcessPacketSendEvent(const std::string &scriptName, const char *cstrPacket, SOCKET s);

        void LogSend(const LogPacket &packet);
        void LogRecv(const LogPacket &packet);

        // Returns status (blocked, ignored) and also performs modify
        BlockStatus GetBlockStatus(const PacketFlag type, std::string &packetString, 
                GeneralPacketStruct *dataPacket, const DWORD packetSize);

        void ProcessHotKey(const DWORD vkCode);
        void ProcessTimeEvent();

    private:
        HackDLL(); // Private so that it can't be called
        ~HackDLL();
        HackDLL(HackDLL const &){}; // Copy ctor is private
        HackDLL& operator=(HackDLL const &){}; // Assignment operator is private
        static HackDLL *m_instance;
        std::list<HANDLE> m_hPipe_list; // The pipe used to send / receive hack messages
        HWND m_process_hwnd; // the HWND of MapleStory.  Set when CreateWindowExA is called
        std::list<BlockPacket> m_sBlock; // send block list
        std::list<BlockPacket> m_rBlock; // recv block list
        std::map<std::string, OpcodeFunction> m_opcode_map;

        // a buffer that stores messages that have never been sent
        std::queue<std::string> m_message_buffer;
        SOCKET m_currentSocket;
        Script::ScriptManager scriptManager;
        HookInterface *hookIntf;
    };
}

#endif // __RIPE_DLL_H