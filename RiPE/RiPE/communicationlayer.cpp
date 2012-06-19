#include "stdafx.h"
#include "communicationlayer.h"

#include <wx/ffile.h>

#include "../defs.h"
#include "RiPEApp.h"
#include "../RiPE.h"

namespace {
    wxString activePipeName = "";

    // A struct used in callback functions for pipes
    struct PipeCallbackStruct
    {
        wxString   pipeName;
    };

    wxString MakePipeName(const wxString &pipeName) {
        wxString result = wxT("\\\\.\\pipe\\") + pipeName + wxString::Format("%X",RiPEApp::GetRiPE()->communicationLayer.GetPID());
        return result;
    }

    void LogErrorMessage( const wxString &message )
    {
        wxFile *file = new wxFile(wxGetCwd() + wxT("\\") + ERROR_MESSAGE_LOG,
            wxFile::write_append);

        file->Write( message );
        file->Write( wxT("\n") );

        delete file;
    }

    DWORD WINAPI PipeListener( LPVOID lpParam )
    {
        const wxString pipeName = ((PipeCallbackStruct *)lpParam)->pipeName;
        const DWORD BUFSIZE = 4096;
        HANDLE hPipe   = RiPEApp::GetRiPE()->communicationLayer.GetPipe(pipeName);
        char  chBuf[BUFSIZE];
        BOOL   success = FALSE;
        DWORD  cbRead;
        activePipeName = pipeName; // only the last connection should be active

        // Since we no longer need lpParam, we need to delete it.
        delete lpParam;

        while( pipeName == activePipeName )
        {
            if( RiPEApp::GetRiPE() == NULL )
                break;
            // If we no longer have a pipe associated with this window, then
            // we must exit the thread.
            if( !RiPEApp::GetRiPE()->communicationLayer.HasPipe(pipeName) )
                break;

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
                LogErrorMessage( pipeName + wxT(" has broken.  Terminating pipe.") );
                break;
            }

            if( cbRead > 0 )
                RiPEApp::GetRiPE()->communicationLayer.ProcessPipeMessage( chBuf, cbRead );

            // If there's more data available, don't sleep
            if( GetLastError() != ERROR_MORE_DATA )
                Sleep(100);
        }

        // Our connection has been terminated, so close the pipe.
        if( RiPEApp::GetRiPE() != NULL )
        {
            RiPEApp::GetRiPE()->communicationLayer.ClosePipe(pipeName, success == TRUE);
        }

        return 0;
    }

    DWORD WINAPI CreateAndConnectPipe( LPVOID lpParam )
    {
        HANDLE hPipe      = NULL;
        wxString pipeName = ((PipeCallbackStruct *)lpParam)->pipeName;
        DWORD dwMode      = PIPE_READMODE_MESSAGE|PIPE_NOWAIT;
        BOOL  success     = FALSE;
        int   numberOfTries = 0;
        const int MAX_NUMBER_OF_TRIES = 20;

        // Give the DLL some time to create the file
        Sleep(2000);

        for( ;; )
        {
            hPipe = CreateFileA(
                pipeName,
                GENERIC_READ|GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL );

            if( hPipe != INVALID_HANDLE_VALUE )
                break; // We succeeded

            if( GetLastError() == ERROR_FILE_NOT_FOUND &&
                numberOfTries++ < MAX_NUMBER_OF_TRIES )
            {
                Sleep(1000);
                continue;
            }

            // The pipe wasn't busy, so that's pretty bad :(
            else if( GetLastError() != ERROR_PIPE_BUSY )
            {
                LogErrorMessage( wxT("Failed to connect to pipe: ") + pipeName +
                    wxString::Format(".  Error Code: %i", GetLastError()) );
                return -1; // Stop trying
            }

            // All pipe instances are busy, so wait up to 10 seconds
            if( !WaitNamedPipeA( pipeName, 10000 ) )
            {
                LogErrorMessage( wxT("Waiting for pipe: ") + pipeName + 
                    wxString::Format(" timed out.  Error Code: %i", GetLastError()) );
                return -1;
            }
        }

        // After connecting, let's set up the pipe
        success = SetNamedPipeHandleState(
            hPipe,
            &dwMode,
            NULL,
            NULL );
        if( !success )
        {
            LogErrorMessage( wxT("Failed to set named pipe handle state for pipe: ") +
                pipeName );
            return -1;
        }

        RiPEApp::GetRiPE()->communicationLayer.SetPipe( pipeName, hPipe );
        
        // Initialize the DLL (send it all packets in S - Block, R - Block, etc.)
        RiPEApp::GetRiPE()->communicationLayer.InitDll(pipeName);

        // Now we make this thread a listener on the pipe
        PipeListener( lpParam );

        return 0;
    }

    void ProcessOpcodeLogSend(const wxString &message) {
        wxString remainingMessage = message;
        
        LogPacket packet;
        packet.returnAddress = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.size = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.opcode = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.packet = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.packetData = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.socket = wxAtoi(remainingMessage);

        RiPEApp::GetRiPE()->communicationLayer.AddLogSend(packet);
    }

    void ProcessOpcodeLogRecv(const wxString &message) {
        wxString remainingMessage = message;
        
        LogPacket packet;
        packet.returnAddress = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.size = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.opcode = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.packet = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.packetData = remainingMessage.BeforeFirst('\n');
        remainingMessage = remainingMessage.AfterFirst('\n');
        packet.socket = wxAtoi(remainingMessage);

        RiPEApp::GetRiPE()->communicationLayer.AddLogRecv(packet);
    }
} // end of unnamed namespace

CommunicationLayer::CommunicationLayer() {
    m_sLogMutex = boost::shared_ptr<boost::mutex>(new boost::mutex());
    m_rLogMutex = boost::shared_ptr<boost::mutex>(new boost::mutex());
    m_hookSend = false;
    m_hookRecv = false;
    Init();
}

void CommunicationLayer::Init() {
    // Init the opcode handler
    m_opcode_map["LogSend"] = (OpcodeFunction)ProcessOpcodeLogSend;
    m_opcode_map["LogRecv"] = (OpcodeFunction)ProcessOpcodeLogRecv;
}

HANDLE CommunicationLayer::GetPipe(const wxString &pipeName) {
    if (pipes.count(pipeName) > 0) {
        return pipes[pipeName];
    } else {
        return NULL;
    }
}

void CommunicationLayer::SetPipe(const wxString &pipeName, HANDLE hPipe) {
    pipes[pipeName] = hPipe;
}

void CommunicationLayer::InitDll(const wxString &pipeName) {

    if (!m_hookInterface.empty()) {
        // Tell RiPE DLL what hook interface it's using
        SendMessageToPipe(wxString::Format("SetHookInterface %s", m_hookInterface), MakePipeName(RIPE_DLL));

        // Tell RiPE DLL what it's hooked to
        SendMessageToPipe(wxString::Format("HookSend %i", m_hookSend), MakePipeName(RIPE_DLL));
        SendMessageToPipe(wxString::Format("HookRecv %i", m_hookRecv), MakePipeName(RIPE_DLL));
    }

    // Tell RiPE DLL about S - Block packets
    std::list<BlockPacket>::iterator iter = m_sBlock.begin();
    for (; iter != m_sBlock.end(); ++iter) {
        SendMessageToPipe(wxString::Format("AddPacketToSBlock %s\n%s\n%s", 
                iter->type, iter->packet1, iter->packet2), MakePipeName(RIPE_DLL));
    }

    // Tell RiPE DLL about R - Block packets
    iter = m_rBlock.begin();
    for (; iter != m_rBlock.end(); ++iter) {
        SendMessageToPipe(wxString::Format("AddPacketToRBlock %s\n%s\n%s", 
                iter->type, iter->packet1, iter->packet2), MakePipeName(RIPE_DLL));
    }
    
    // Tell RiPE DLL about RiPEST scripts
    std::map<wxString, wxString>::iterator scriptIter = m_scripts.begin();
    for (; scriptIter != m_scripts.end(); ++scriptIter) {
        SendMessageToPipe(wxString::Format("AddScript %s\n%s", 
                scriptIter->first, scriptIter->second), MakePipeName(RIPE_DLL));
    }
}

bool CommunicationLayer::HasPipe(const wxString &pipeName) {
    return pipes.count(pipeName) > 0;
}

void CommunicationLayer::ProcessPipeMessage(char *chBuf, DWORD cbRead) {
    wxString message = chBuf;
    wxString msgId = message.BeforeFirst(' ');
    message = message.AfterFirst(' ');

    if (m_opcode_map.count(msgId) != 0) {
        m_opcode_map[msgId](message);
    } else {
        // log error.  No handler for this opcode
    }
}

void CommunicationLayer::SendMessageToPipe( const wxString &message, const wxString &pipeName) {
    HANDLE hPipe = GetPipe(pipeName);
    SendMessageToPipe(hPipe, message);
}

void CommunicationLayer::SendMessageToPipe(HANDLE hPipe, wxString message) {
    DWORD dwNumberOfBytesWritten;
    const DWORD MAX_WRITE_ATTEMPTS = 5;

    for (int i=0; i < MAX_WRITE_ATTEMPTS; ++i) {
        if (message.size() == 0) {
            break;
        }

        WriteFile( hPipe, message, message.size()+1, &dwNumberOfBytesWritten, 0);
        message = message.Mid(dwNumberOfBytesWritten);
    }
}

void CommunicationLayer::ClosePipe(const wxString &pipeName, const bool closeHandle) {
    if (pipes.count(pipeName) > 0) {
        if (closeHandle && pipes[pipeName] != INVALID_HANDLE_VALUE &&
            pipes[pipeName] != NULL) {
            CloseHandle(pipes[pipeName]);
            ClosePipeThread(pipeName);
        }
        pipes.erase(pipeName);
    }
}

void CommunicationLayer::ClosePipeThread(const wxString &pipeName) {
    if (m_pipe_threads.count(pipeName) > 0) {
        HANDLE hThread = m_pipe_threads[pipeName];
        if (hThread != INVALID_HANDLE_VALUE && hThread != NULL) {
            WaitForSingleObject(hThread, 1000);
            CloseHandle(hThread);
        }
        m_pipe_threads.erase(pipeName);
    }
}

// Connect to a process.  This means create a pipe thread for this process
void CommunicationLayer::Connect(const int pid) {
    m_pId = pid;
    wxString pipeName = MakePipeName(RIPE_DLL);
    PipeCallbackStruct *lpParam = new PipeCallbackStruct();
    lpParam->pipeName = pipeName;
    HANDLE hThread = CreateThread(NULL, 0, CreateAndConnectPipe, lpParam, 0, NULL);

    if (hThread == NULL) {
        delete lpParam;
    } else {
        m_pipe_threads[pipeName] = hThread;
    }
}

int CommunicationLayer::GetPID() {
    return m_pId;
}

void CommunicationLayer::SetHookInterface(const wxString &str) {
    m_hookInterface = str;
    SendMessageToPipe(wxString::Format("SetHookInterface %s", str), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::HookSend(const bool enable) {
    m_hookSend = enable;
    SendMessageToPipe(wxString::Format("HookSend %i", enable), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::HookRecv(const bool enable) {
    m_hookRecv = enable;
    SendMessageToPipe(wxString::Format("HookRecv %i", enable), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::PacketSend(const char *cstrPacket, SOCKET s) {
    SendMessageToPipe(wxString::Format("PacketSend %i\n%s", s, cstrPacket), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::PacketRecv(const char *cstrPacket, SOCKET s) {
    SendMessageToPipe(wxString::Format("PacketRecv %i\n%s", s, cstrPacket), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::AddPacketToSBlock(const wxString &type, const wxString &packet1, const wxString &packet2) {
    m_sBlock.push_back(BlockPacket(type, packet1, packet2));
    SendMessageToPipe(wxString::Format("AddPacketToSBlock %s\n%s\n%s", type, packet1, packet2), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::AddPacketToRBlock(const wxString &type, const wxString &packet1, const wxString &packet2) {
    m_rBlock.push_back(BlockPacket(type, packet1, packet2));
    SendMessageToPipe(wxString::Format("AddPacketToRBlock %s\n%s\n%s", type, packet1, packet2), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::RemovePacketFromSBlock(const wxString &type, const wxString &packet1, const wxString &packet2) {
    std::list<BlockPacket>::iterator iter = m_sBlock.begin();
    for (; iter != m_sBlock.end(); ++iter) {
        if (iter->type == type && iter->packet1 == packet1 && iter->packet2 == packet2) {
            iter = m_sBlock.erase(iter);
        }
    }
    SendMessageToPipe(wxString::Format("RemovePacketFromSBlock %s\n%s\n%s", type, packet1, packet2), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::RemovePacketFromRBlock(const wxString &type, const wxString &packet1, const wxString &packet2) {
    std::list<BlockPacket>::iterator iter = m_rBlock.begin();
    for (; iter != m_rBlock.end(); ++iter) {
        if (iter->type == type && iter->packet1 == packet1 && iter->packet2 == packet2) {
            iter = m_rBlock.erase(iter);
        }
    }
    SendMessageToPipe(wxString::Format("RemovePacketFromRBlock %s\n%s\n%s", type, packet1, packet2), MakePipeName(RIPE_DLL));
}

// WATCH OUT:  Special format!
// Returns true on success
bool CommunicationLayer::AddScript( const wxString &scriptName, const wxString &scriptText) {
    if (m_scripts.count(scriptName) == 0) {
        m_scripts[scriptName] = scriptText;
        SendMessageToPipe(wxString::Format("AddScript %s\n%s", scriptName, scriptText), MakePipeName(RIPE_DLL));
        return true;
    } else {
        return false;
    }
}

void CommunicationLayer::RemoveScript(const wxString &scriptName) {
    if (m_scripts.count(scriptName) > 0) {
        SendMessageToPipe(wxString::Format("RemoveScript %s", scriptName), MakePipeName(RIPE_DLL));
        m_scripts.erase(scriptName);
    }
}

void CommunicationLayer::DeleteScript(const wxString &scriptName) {

    if (m_scripts.count(scriptName) > 0) {
        SendMessageToPipe(wxString::Format("RemoveScript %s", scriptName), MakePipeName(RIPE_DLL));
        wxString filename = wxT("scripts/") + scriptName;
        if (wxFileExists(filename)) {
            wxRemoveFile(filename);
        }
        m_scripts.erase(scriptName);
    }
}

void CommunicationLayer::ToggleScript( const wxString &scriptName, const bool enable) {
    if (m_scripts.count(scriptName) > 0) {
        SendMessageToPipe(wxString::Format("ToggleScript %i\n%s", enable, scriptName), MakePipeName(RIPE_DLL));
    }
}

void CommunicationLayer::SaveScripts() {
    // Create folder if it doesn't exist
    if( !wxDirExists( wxT("scripts") ) )
    {
        wxMkdir( wxT("scripts") );
    }

    // For all scripts
    std::map<wxString, wxString>::const_iterator iter = m_scripts.begin();
    for( ; iter != m_scripts.end(); ++iter )
    {
        wxFFile file;
        file.Open( wxT("scripts/") + iter->first, "w");
        file.Write(iter->second);
        file.Close();
    }
    //SendMessageToPipe(wxString::Format("SaveScripts"), MakePipeName(RIPE_DLL));
}

void CommunicationLayer::ProcessHotKey(const DWORD vkCode) {
    SendMessageToPipe(wxString::Format("ProcessHotKey %i", vkCode), MakePipeName(RIPE_DLL));
}

bool CommunicationLayer::LoadScript(const wxString &filename, const wxString &name) {
    wxFFile file(filename);
    wxString script;

    file.ReadAll(&script);
    file.Close();
    return AddScript(name, script);
}

// Add a packet to the send queue to be processed by the GUI
void CommunicationLayer::AddLogSend(const LogPacket &packet) {
    m_sLogMutex->lock();
    m_sLog.push(packet);
    m_sLogMutex->unlock();
}

// Add a packet to the recv queue to be processed by the GUI
void CommunicationLayer::AddLogRecv(const LogPacket &packet) {
    m_rLogMutex->lock();
    m_rLog.push(packet);
    m_rLogMutex->unlock();
}

// Returns true if a packet was returned in packet
bool CommunicationLayer::GetNextSendPacket(LogPacket &packet) {
    if (!m_sLog.empty()) {
        packet = m_sLog.front();
        m_sLogMutex->lock();
        m_sLog.pop();
        m_sLogMutex->unlock();
        return true;
    } else {
        return false;
    }
}

// Returns true if a packet was returned in packet
bool CommunicationLayer::GetNextRecvPacket(LogPacket &packet) {
    if (!m_rLog.empty()) {
        packet = m_rLog.front();
        m_rLogMutex->lock();
        m_rLog.pop();
        m_rLogMutex->unlock();
        return true;
    } else {
        return false;
    }
}
