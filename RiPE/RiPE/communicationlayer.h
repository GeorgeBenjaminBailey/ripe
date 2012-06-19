#ifndef __COMMUNICATION_LAYER_H
#define __COMMUNICATION_LAYER_H

#include <list>
#include <map>
#include <queue>

#include <boost/thread.hpp>

#include <Windows.h>

#include <wx/string.h>

struct BlockPacket {
    wxString type;    // the type of the packet (Block, Ignore, Ntercept, ...)
    wxString packet1; // the block portion of the packet
    wxString packet2; // the modify portion of the packet

    BlockPacket() {
    }

    BlockPacket(const wxString &type, const wxString &packet1, const wxString &packet2) {
        this->type    = type;
        this->packet1 = packet1;
        this->packet2 = packet2;
    }
};
    
struct LogPacket {
    wxString returnAddress;
    wxString size;
    int      socket;     // the socket number if applicable
    wxString opcode;     // the opcode of the packet
    wxString packet;     // the "formatted" packet displayed in Tree / List view
    wxString packetData; // the "raw" packet returned by double-clicking the packet

    LogPacket() {
        socket = 0;
    }

    LogPacket(const wxString &returnAddress, 
        const wxString &size, const int socket, 
        const wxString &opcode, const wxString &packet, 
        const wxString &packetData) {

        this->returnAddress = returnAddress;
        this->size   = size;
        this->socket = socket;
        this->opcode = opcode;
        this->packet = packet;
        this->packetData = packetData;
    }
};

typedef void (*OpcodeFunction)(const wxString &message);

/**
 * The GUI uses this to interact indirectly with the DLL.
 */
class CommunicationLayer {
private:
    HHOOK m_hook;
    boost::shared_ptr<boost::mutex>    m_sLogMutex;
    boost::shared_ptr<boost::mutex>    m_rLogMutex;
    std::list<BlockPacket>             m_sBlock; // send block list
    std::list<BlockPacket>             m_rBlock; // recv block list
    std::map<wxString, HANDLE>         pipes;    // contains all active pipes
    std::map<wxString, HANDLE>         m_pipe_threads;
    std::map<wxString, wxString>       m_scripts;
    std::map<wxString, OpcodeFunction> m_opcode_map;
    std::queue<LogPacket>              m_sLog;   // log containing unprocessed send packets
    std::queue<LogPacket>              m_rLog;   // log containing unprocessed recv packets
    wxString                           m_hookInterface; // keeps track of current hook interface
    bool                               m_hookSend;
    bool                               m_hookRecv;
    int m_pId;

    void Init();

public:
    CommunicationLayer();

    void SetHookInterface(const wxString &str);
    void HookSend(const bool enable);
    void HookRecv(const bool enable);
    void PacketSend(const char *cstrPacket, SOCKET s);
    void PacketRecv(const char *cstrPacket, SOCKET s);
    void AddPacketToSBlock(const wxString &type, const wxString &packet1, const wxString &packet2);
    void AddPacketToRBlock(const wxString &type, const wxString &packet1, const wxString &packet2);
    void RemovePacketFromSBlock(const wxString &type, const wxString &packet1, const wxString &packet2);
    void RemovePacketFromRBlock(const wxString &type, const wxString &packet1, const wxString &packet2);
    void ProcessHotKey(const DWORD vkCode);
    // This needs to mimic the script manager to a small extent so that it knows the available script names
    bool AddScript( const wxString &scriptName, const wxString &scriptText);
    void Connect(const int pid);
    int  GetPID();
    void RemoveScript(const wxString &scriptName);
    void DeleteScript(const wxString &scriptName);
    void ToggleScript( const wxString &scriptName, const bool enable);
    void SaveScripts();
    bool LoadScript(const wxString &filename, const wxString &name);
    HANDLE GetPipe(const wxString &pipeName);
    void SetPipe(const wxString &pipeName, HANDLE hPipe);
    void InitDll(const wxString &pipeName); // initializes the DLL (sets up packets in multi, S - Block, etc.)
    void SendMessageToPipe( const wxString &message, const wxString &pipeName);
    void SendMessageToPipe(HANDLE hPipe, wxString message);
    bool HasPipe(const wxString &pipeName);
    void ProcessPipeMessage(char *chBuf, DWORD cbRead);
    void ClosePipe(const wxString &pipeName, const bool closeHandle);
    void ClosePipeThread(const wxString &pipeName);

    // Add a packet to the send queue to be processed by the GUI
    void AddLogSend(const LogPacket &packet);

    // Add a packet to the recv queue to be processed by the GUI
    void AddLogRecv(const LogPacket &packet);

    // Returns true if a packet was returned in packet
    bool GetNextSendPacket(LogPacket &packet);

    // Returns true if a packet was returned in packet
    bool GetNextRecvPacket(LogPacket &packet);
};

#endif // __COMMUNICATION_LAYER_H
