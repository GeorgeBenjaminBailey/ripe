#include "defs.h"

#include <cstdio>
#include <fstream>

// Disable strcpy and sprintf warning
#pragma warning( push )
#pragma warning( disable : 4995 4996 )
#include <Windows.h>

#include <boost/thread.hpp>

#include "ripe_dll.h"

#include "hookhop.h"
#include "lasterror.h"
#include "memreadwrite.h"
#include "RiME/memscan.hpp"
#include "ripecontainer.h"
#include "RiPE/atohx.h"

#include "RiPE/customfacade.h"
#include "RiPE/sendallfacade.h"
#include "RiPE/sendfacade.h"
#include "RiPE/sendtofacade.h"
#include "RiPE/wsasendfacade.h"
#include "RiPE/wsasendtofacade.h"


namespace
{
    bool continueRunning = true;

    // Convenience function for adding opcode
    #define ADD_OPCODE(func) m_opcode_map[_str(func)] = (OpcodeFunction)func

    void LogErrorMessage( const std::string &message )
    {
        std::fstream file;
        file.open(ERROR_MESSAGE_LOG, std::ios::app | std::ios::out);

        file << message << std::endl;

        file.close();
    }

    void ProcessHotKey(const std::string &message) {
        DWORD vkCode = atoi(message.c_str());
        RiPE::Container::GetHackDLL()->ProcessHotKey( vkCode );
    }

    // The following functions perform deserialization
    void SetHookInterface(const std::string &message) {
        RiPE::Container::GetHackDLL()->SetHookInterface(message);
    }
    void HookSend(const std::string &message) {
        RiPE::Container::GetHackDLL()->HookSend(message != "0");
    }
    void HookRecv(const std::string &message) {
        RiPE::Container::GetHackDLL()->HookRecv(message != "0");
    }
    void PacketSend(const std::string &message) {
        SOCKET s = atoi(message.substr(0, message.find_first_of('\n')).c_str());
        RiPE::Container::GetHackDLL()->PacketSend(message.substr(message.find_first_of('\n')+1), s);
    }
    void PacketRecv(const std::string &message) {
        SOCKET s = atoi(message.substr(0, message.find_first_of('\n')).c_str());
        RiPE::Container::GetHackDLL()->PacketRecv(message.substr(message.find_first_of('\n')+1), s);
    }
    void AddPacketToSBlock(const std::string &message) {
        std::string buffer = message;
        RiPE::BlockPacket packet;
        packet.type = buffer.substr(0, buffer.find_first_of('\n'));
        buffer = buffer.substr(buffer.find_first_of('\n')+1);
        packet.packet1 = buffer.substr(0, buffer.find_first_of('\n'));
        packet.packet2 = buffer.substr(buffer.find_first_of('\n')+1);

        RiPE::Container::GetHackDLL()->AddPacketToSBlock(packet);
    }
    void AddPacketToRBlock(const std::string &message) {
        std::string buffer = message;
        RiPE::BlockPacket packet;
        packet.type = buffer.substr(0, buffer.find_first_of('\n'));
        buffer = buffer.substr(buffer.find_first_of('\n')+1);
        packet.packet1 = buffer.substr(0, buffer.find_first_of('\n'));
        packet.packet2 = buffer.substr(buffer.find_first_of('\n')+1);

        RiPE::Container::GetHackDLL()->AddPacketToRBlock(packet);
    }
    void RemovePacketFromSBlock(const std::string &message) {
        std::string buffer = message;
        RiPE::BlockPacket packet;
        packet.type = buffer.substr(0, buffer.find_first_of('\n'));
        buffer = buffer.substr(buffer.find_first_of('\n')+1);
        packet.packet1 = buffer.substr(0, buffer.find_first_of('\n'));
        packet.packet2 = buffer.substr(buffer.find_first_of('\n')+1);

        RiPE::Container::GetHackDLL()->RemovePacketFromSBlock(packet);
    }
    void RemovePacketFromRBlock(const std::string &message) {
        std::string buffer = message;
        RiPE::BlockPacket packet;
        packet.type = buffer.substr(0, buffer.find_first_of('\n'));
        buffer = buffer.substr(buffer.find_first_of('\n')+1);
        packet.packet1 = buffer.substr(0, buffer.find_first_of('\n'));
        packet.packet2 = buffer.substr(buffer.find_first_of('\n')+1);

        RiPE::Container::GetHackDLL()->RemovePacketFromRBlock(packet);
    }
    void AddScript(const std::string &message) {
        RiPE::Container::GetHackDLL()->AddScript(message.substr(0, message.find_first_of('\n')), 
                message.substr(message.find_first_of('\n')+1));
    }
    void RemoveScript(const std::string &message) {
        RiPE::Container::GetHackDLL()->RemoveScript(message);
    }
    void DeleteScript(const std::string &message) {
        RiPE::Container::GetHackDLL()->DeleteScript(message);
    }
    void ToggleScript(const std::string &message) {
        RiPE::Container::GetHackDLL()->ToggleScript(message.substr(0, message.find_first_of('\n')) != "0", 
                message.substr(message.find_first_of('\n')+1));
    }
    void SaveScripts(const std::string &message) {
        RiPE::Container::GetHackDLL()->SaveScripts();
    }
    void SetSocket(const std::string &message) {
        SOCKET s = atoi(message.c_str());
        RiPE::Container::GetHackDLL()->SetSocket(s);
    }

    void ScriptTimer() {
        while (continueRunning) {
            RiPE::Container::GetHackDLL()->ProcessTimeEvent();
            Sleep(10);
        }
    }
}

namespace RiPE
{
const DWORD BUFSIZE = 4096;

HackDLL * HackDLL::m_instance = NULL;

// Reads from client
DWORD WINAPI InstanceThread( LPVOID lpvParam )
{ 
    HANDLE hPipe  = (HANDLE)lpvParam;
    char   chBuf[BUFSIZE] = {0};
    BOOL   success = FALSE;
    DWORD  cbRead;

    RiPE::HackDLL::GetInstance()->SendBufferedMessages();

    // Loop until done reading
    for( ;; )
    { 
        // Read client requests from the pipe. This simplistic code only allows messages
        // up to BUFSIZE characters in length.
        success = ReadFile(
            hPipe,
            chBuf,
            BUFSIZE*sizeof(char),
            &cbRead,
            NULL );

        // If badness has happened
        if( !success && GetLastError() != ERROR_MORE_DATA
            && GetLastError() != ERROR_PIPE_BUSY
            && GetLastError() != ERROR_NO_DATA )
        {
            //LogErrorMessage( std::string("Pipe has broken.  Terminating pipe.") );
            RiPE::Container::GetHackDLL()->Close();
            break;
        }

        if( cbRead > 0 )
            HackDLL::GetInstance()->ProcessPipeMessage( chBuf, cbRead );

        // If there's more data available, don't sleep
        if( GetLastError() != ERROR_MORE_DATA )
            Sleep(100);
    }

    // Flush the pipe to allow the client to read the pipe's contents 
    // before disconnecting. Then disconnect the pipe, and close the 
    // handle to this pipe instance. 

    FlushFileBuffers(hPipe); 
    DisconnectNamedPipe(hPipe); 
    CloseHandle(hPipe); 

    return 1;
}

HackDLL::HackDLL()
{
    RiPE::Container::SetHackDLL(this);
    hookIntf = NULL;
    m_currentSocket = 0;
    PopulateOpcodes();
}
HackDLL::~HackDLL() {
    if (hookIntf != NULL) {
        delete hookIntf;
    }
}


// Returns the instance of the HackDLL object
HackDLL * HackDLL::GetInstance()
{
    if( !m_instance ) // Only allow one instance of class to be generated
        m_instance = new HackDLL;
    
    return m_instance;
}

// Begins processing events
void HackDLL::Run()
{
    BOOL   fConnected = FALSE;
    DWORD  dwThreadId = 0;
    HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
    char   buffer[50] = {0};
    continueRunning = true;

    // Start up the script timer
    static boost::thread scriptTimer( &ScriptTimer );

    sprintf( buffer, "\\\\.\\pipe\\ripe_dll%X", GetCurrentProcessId() );

    hPipe = CreateNamedPipeA(
        buffer,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE |
        PIPE_READMODE_MESSAGE |
        PIPE_NOWAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFSIZE,
        BUFSIZE,
        0,
        NULL );

    if( hPipe == INVALID_HANDLE_VALUE )
        return; // error

    int timeout = 0;
    int MAX_TIMEOUT = 10;
    while( continueRunning )
    {
        fConnected = ConnectNamedPipe( hPipe, NULL ) ?
            TRUE : (GetLastError() == ERROR_PIPE_CONNECTED );

        if( fConnected ) {
            timeout = 0;
            m_hPipe_list.push_back( hPipe );

            // Only allow one thread
            InstanceThread( (LPVOID)hPipe );

            if( hThread == NULL )
            {
                m_hPipe_list.pop_back();
                return;
            }
            else
                break;
        } else {
            if (timeout++ > MAX_TIMEOUT) {
                continueRunning = false;
                break;
            }
        }
        Sleep(1000);
    }
}

void HackDLL::Close() {
    if (hookIntf != NULL) {
        hookIntf->HookSend(false);
        hookIntf->HookRecv(false);
    }
    continueRunning = false;
}

// Processes a message from the pipe
void HackDLL::ProcessPipeMessage( char *chBuf, DWORD cbRead )
{
    std::string message = chBuf;
    std::string msgId   = message.substr(0, message.find_first_of(' '));
    message = message.substr(message.find_first_of(' ')+1);

    if( m_opcode_map.count(msgId) != 0 )
        m_opcode_map[msgId]( message );
    else
    {
        MessageBoxA(0, (std::string("No handler for \"") + msgId +
            std::string("\" with message body: ( ") +
            message + std::string(" ) in ProcessPipeMessage")).c_str(), "", 0 );
        LogErrorMessage( std::string("No handler for \"") + msgId +
            std::string("\" with message body: ( ") +
            message + std::string(" ) in ProcessPipeMessage") );
    }
}

// Sends a message through the pipe
void HackDLL::SendMessageToPipe( const std::string &message )
{
    DWORD dwNumberOfBytesWritten;
    const DWORD MAX_WRITE_ATTEMPTS = 5;
    std::string buf;

    // Nowhere to send the message, so let's store it
    if( m_hPipe_list.empty() )
    {
        m_message_buffer.push( message );
    }

    // For every pipe, send the message
    std::list<HANDLE>::const_iterator iter = m_hPipe_list.begin();
    for( ; iter != m_hPipe_list.end(); ++iter )
    {
        buf = message;
        for( int i=0; i < MAX_WRITE_ATTEMPTS; ++i )
        {
            if( buf.size() == 0 )
                break; // We're done

            WriteFile(
                *iter,
                buf.c_str(),
                buf.size()+1, // +1 for NULL
                &dwNumberOfBytesWritten,
                0);

            buf = buf.substr(dwNumberOfBytesWritten);
        }
    }
}

// Attempts to clear the m_message_buffer of messages.
// Call this when a new pipe is connected
void HackDLL::SendBufferedMessages()
{
    while( !m_message_buffer.empty() )
    {
        SendMessageToPipe( m_message_buffer.front() );
        m_message_buffer.pop();
    }
}

void HackDLL::PopulateOpcodes()
{
    m_opcode_map[_str(SetHookInterface)] = ::SetHookInterface;
    m_opcode_map[_str(HookSend)] = ::HookSend;
    m_opcode_map[_str(HookRecv)] = ::HookRecv;
    m_opcode_map[_str(PacketSend)] = ::PacketSend;
    m_opcode_map[_str(PacketRecv)] = ::PacketRecv;
    m_opcode_map[_str(AddPacketToSBlock)] = ::AddPacketToSBlock;
    m_opcode_map[_str(AddPacketToRBlock)] = ::AddPacketToRBlock;
    m_opcode_map[_str(RemovePacketFromSBlock)] = ::RemovePacketFromSBlock;
    m_opcode_map[_str(RemovePacketFromRBlock)] = ::RemovePacketFromRBlock;
    m_opcode_map[_str(AddScript)] = ::AddScript;
    m_opcode_map[_str(RemoveScript)] = ::RemoveScript;
    m_opcode_map[_str(DeleteScript)] = ::DeleteScript;
    m_opcode_map[_str(ToggleScript)] = ::ToggleScript;
    m_opcode_map[_str(SaveScripts)]  = ::SaveScripts;
    m_opcode_map[_str(ProcessHotKey)] = ::ProcessHotKey;
    m_opcode_map[_str(SetSocket)] = ::SetSocket;
}

HWND HackDLL::GetProcessHWND()
{
    return m_process_hwnd;
}

void HackDLL::SetProcessHWND( HWND hwnd )
{
    m_process_hwnd = hwnd;
}

Script::ScriptManager HackDLL::GetScriptManager() {
    return scriptManager;
}

HookInterface * HackDLL::GetHookInterface() {
    return hookIntf;
}

void HackDLL::SetHookInterface(const std::string &object) {
    if (hookIntf != NULL) {
        delete hookIntf;
        hookIntf = NULL;
    }

    if (object == _str(CustomHookFacade)) {
        hookIntf = new CustomHookFacade();
    } else if (object == _str(SendHookFacade)) {
        hookIntf = new SendHookFacade();
    } else if (object == _str(SendToHookFacade)) {
        hookIntf = new SendToHookFacade();
    } else if (object == _str(WSASendHookFacade)) {
        hookIntf = new WSASendHookFacade();
    } else if (object == _str(WSASendToHookFacade)) {
        hookIntf = new WSASendToHookFacade();
    } else if (object == _str(SendAllHookFacade)) {
        hookIntf = new SendAllHookFacade();
    }
    scriptManager.SetHookInterface(hookIntf);
}
void HackDLL::HookSend(const bool enable) {
    if (hookIntf != NULL) {
        hookIntf->HookSend(enable);
    }
}
void HackDLL::HookRecv(const bool enable) {
    if (hookIntf != NULL) {
        hookIntf->HookRecv(enable);
    }
}
void HackDLL::PacketSend(const std::string &packet, SOCKET s) {
    if (hookIntf != NULL) {
        hookIntf->PacketSend(packet.c_str(), s);
    }
}
void HackDLL::PacketRecv(const std::string &packet, SOCKET s) {
    if (hookIntf != NULL) {
        hookIntf->PacketRecv(packet.c_str(), s);
    }
}
void HackDLL::AddPacketToSBlock(const BlockPacket &packet) {
    m_sBlock.push_back(packet);
}
void HackDLL::AddPacketToRBlock(const BlockPacket &packet) {
    m_rBlock.push_back(packet);
}
void HackDLL::RemovePacketFromSBlock(const BlockPacket &packet) {
    std::list<BlockPacket>::iterator iter = m_sBlock.begin();
    for (; iter != m_sBlock.end(); ++iter) {
        if (iter->type == packet.type && iter->packet1 == packet.packet1 
                && iter->packet2 == packet.packet2) {
            iter = m_sBlock.erase(iter);
        }
    }
}
void HackDLL::RemovePacketFromRBlock(const BlockPacket &packet) {
    std::list<BlockPacket>::iterator iter = m_rBlock.begin();
    for (; iter != m_rBlock.end(); ++iter) {
        if (iter->type == packet.type && iter->packet1 == packet.packet1 
                && iter->packet2 == packet.packet2) {
            iter = m_rBlock.erase(iter);
        }
    }
}
void HackDLL::AddScript(const std::string &scriptName, const std::string &scriptData) {
    scriptManager.AddScript(scriptName, scriptData);
}
void HackDLL::RemoveScript(const std::string &scriptName) {
    scriptManager.RemoveScript(scriptName);
}
void HackDLL::DeleteScript(const std::string &scriptName) {
    scriptManager.DeleteScript(scriptName);
}
void HackDLL::ToggleScript(const bool enable, const std::string &scriptName) {
    scriptManager.ToggleScript(scriptName, enable);
}
void HackDLL::SaveScripts() {
    scriptManager.SaveScripts();
}

SOCKET HackDLL::GetSocket() {
    return m_currentSocket;
}

void HackDLL::SetSocket(const SOCKET s) {
    m_currentSocket = s;
}

BlockStatus HackDLL::GetBlockStatus(const PacketFlag type, std::string &packetString, 
        GeneralPacketStruct *dataPacket, const DWORD packetSize) {
    
    BlockStatus result = STATUS_NOT_BLOCKED;
    std::list<BlockPacket> *blockList = NULL;

    switch (type) {
    case PACKET_STREAM_SEND:
        blockList = &m_sBlock;
        break;
    case PACKET_STREAM_RECV:
        blockList = &m_rBlock;
        break;
    default:
        break;
    }

    // Cycle through all of the packets in the list looking for a match
    if (blockList != NULL) {
        std::list<BlockPacket>::iterator iter = blockList->begin();
        for (; iter != blockList->end(); ++iter) {
            /*
             * For all characters in the block/ignore string, check to see if it matches
             * the current packet
             */
            for (unsigned int i=0, index=0; i < iter->packet1.size() 
                    && index < packetString.size(); ++i, ++index) {
                
                if (iter->packet1[i] == '?') {
                    if (i < iter->packet1.size()-1) {
                        if (index < packetString.size()-1) {
                            continue;
                        } else {
                            break;
                        }
                    }
                } else if( (iter->packet1[i] != packetString[index])
                        && (iter->packet1[i] != ' ')) {
                    break; // not the same, so move on to the next
                }

                // We have a block, ignore, or modify collision
                if (i == iter->packet1.size()-1) {
                    if (iter->type == PACKETTYPE_BLOCK) {
                        result = STATUS_BLOCKED;
                    } else if (iter->type == PACKETTYPE_IGNORE) {
                        result = STATUS_IGNORED;
                    } else if (iter->type == PACKETTYPE_MODIFY) {
                        result = STATUS_MODIFIED;
                    } else if (iter->type == PACKETTYPE_INTERCEPT) {
                        result = STATUS_INTERCEPTED;
                    }
                }

                // ignore spaces
                if (iter->packet1[i] == ' ') {
                    --index;
                    continue;
                }
            }

            if (result == STATUS_BLOCKED || result == STATUS_IGNORED) {
                return result;
            } else if (result == STATUS_MODIFIED) {
                result = STATUS_NOT_BLOCKED; // reset the status
                DWORD sizeDifference = packetSize - (packetString.size() / 2);
                std::string temp = "";
                bool hasDynamicEnding = false; // if a '.' is present

                if (packetSize >= 2) {
                    *(dataPacket->headerShortPtr) = 0;
                }
                for (unsigned int i=2; i < packetSize; ++i) {
                    ((char *)(dataPacket->data))[i-2] = 0;
                }
                for (unsigned int i=0; i < iter->packet2.size(); ++i) {
                    if (iter->packet2[i] == '.') {
                        hasDynamicEnding = true;
                        break;
                    }
                }
                if (!hasDynamicEnding) {
                    *(dataPacket->lengthPtr) = iter->packet2.size()/2 + sizeDifference - packetSize + *(dataPacket->lengthPtr);
                }
                for (unsigned int i=0; i < iter->packet2.size() && i < packetString.size(); ++i) {
                    if (iter->packet2[i] == '?') {
                        temp += packetString[i];
                    } else if (iter->packet2[i] == '*') {
                        char chBuf[2];
                        sprintf(chBuf, "%01X", rand()%16);
                        temp += chBuf;
                    } else if (iter->packet2[i] == '.') {
                        for (unsigned int j=i; j < packetString.size(); ++j) {
                            temp += packetString[j];
                        }
                        break;
                    } else {
                        temp += iter->packet2[i];
                    }
                }
                // Correct the packet for tree control
                for (unsigned int i=0; i < temp.size() && i < packetString.size(); ++i) {
                    packetString[i] = temp[i];
                }

                // Write the new packet
                AsciiToHex::atohx(dataPacket->headerCharPtr, temp.substr(0,4).c_str()); // header
                AsciiToHex::atohx(dataPacket->data, temp.substr(4).c_str()); // data
            }
        } // end of looping through all packets in list
    } // end of if (blockList != NULL)

    return result;
}

void HackDLL::ProcessSendEvent(std::vector<unsigned char> &packet) {
    scriptManager.ProcessSendEvent(packet);
}

void HackDLL::ProcessRecvEvent(std::vector<unsigned char> &packet) {
    scriptManager.ProcessRecvEvent(packet);
}

void HackDLL::ProcessHookRecvEvent(const std::string &scriptName, const bool enable) {
    scriptManager.ProcessHookRecvEvent(scriptName, enable);
}

void HackDLL::ProcessHookSendEvent(const std::string &scriptName, const bool enable) {
    scriptManager.ProcessHookSendEvent(scriptName, enable);
}

void HackDLL::ProcessPacketRecvEvent(const std::string &scriptName, const char *cstrPacket, SOCKET s) {
    scriptManager.ProcessPacketRecvEvent(scriptName, cstrPacket, s);
}

void HackDLL::ProcessPacketSendEvent(const std::string &scriptName, const char *cstrPacket, SOCKET s) {
    scriptManager.ProcessPacketSendEvent(scriptName, cstrPacket, s);
}

void HackDLL::LogSend(const LogPacket &packet) {
    char buf[7];
    std::string message = "LogSend ";
    message += packet.returnAddress + "\n";
    message += packet.size + "\n";
    message += packet.opcode + "\n";
    message += packet.packet + "\n";
    message += packet.packetData + "\n";
    sprintf(buf, "%i", packet.socket);
    message += buf;
    SendMessageToPipe( message );
}

void HackDLL::LogRecv(const LogPacket &packet) {
    char buf[7];
    std::string message = "LogRecv ";
    message += packet.returnAddress + "\n";
    message += packet.size + "\n";
    message += packet.opcode + "\n";
    message += packet.packet + "\n";
    message += packet.packetData + "\n";
    sprintf(buf, "%i", packet.socket);
    message += buf;
    SendMessageToPipe( message );
}

void HackDLL::ProcessHotKey( const DWORD vkCode ) {
    // Send a keyboard press
    scriptManager.ProcessKeyboardEvent(vkCode);
}

void HackDLL::ProcessTimeEvent() {
    scriptManager.ProcessTimeEvent();
}

} // end of namespace RiPE

#pragma warning( pop )
