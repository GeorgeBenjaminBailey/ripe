#include "defs.h"

#include <list>
#include <map>
#include <set>

#include <TlHelp32.h>
#include <Winternl.h>

#include "breakpoint.h"
#include "memreadwrite.h"

//#include <wx/msgdlg.h>

bool freezeOnException  = false;

namespace
{
    BreakpointCallbackFunction callback;

    // time in milliseconds between disabling a DR and stopping the exception
    // handler.  If this value is too low, you will crash in some cases.
    const int MAX_DISABLE_TIME = 1000;

    struct MemoryBuffer
    {
        char buffer[0x100]; // a 256-byte buffer for DR spoofing
    };

    class BreakpointAddress
    {
    private:
        bool  inUse;
        bool terminator;
        PVOID address;
        DWORD eip; // eip here refers to the address to set eip to
        int   disableTime; // time when last disabled

    public:
        ///////////////////////////////////////////////////////////////////////
        // CONSTRUCTOR / DESTRUCTOR                                          //
        ///////////////////////////////////////////////////////////////////////
        BreakpointAddress() {
            inUse       = false;
            terminator  = false;
            address     = NULL;
            eip         = 0;
            disableTime = 0;
        }

        BreakpointAddress( const bool isInUse, 
            const PVOID addr=NULL, 
            const DWORD eipVal=0, 
            const int disableT=0,
            const bool hookIsTerminator=false) {

            inUse       = isInUse;
            terminator  = hookIsTerminator;
            address     = addr;
            eip         = eipVal;
            disableTime = disableT;
        }

        ~BreakpointAddress() {}

        ///////////////////////////////////////////////////////////////////////
        // GETTERS / SETTERS                                                 //
        ///////////////////////////////////////////////////////////////////////
        DWORD GetEip() const {
            return eip;
        }

        void SetEip( const DWORD eipVal ) {
            eip = eipVal;
        }

        PVOID GetAddress() const {
            return address;
        }

        void SetAddress( const PVOID addr ) {
            address = addr;
        }

        PVOID GetTerminatorAddress()
        {
            return (PVOID)eip;
        }

        bool GetTerminator()
        {
            return terminator;
        }

        void SetTerminator( const bool hookIsTerminator )
        {
            terminator = hookIsTerminator;
        }

        ///////////////////////////////////////////////////////////////////////
        // PUBLIC FUNCTIONS                                                  //
        ///////////////////////////////////////////////////////////////////////
        // get the difference in time since last disabled
        int GetMilliSpanSinceDisable() {
            return GetMilliSpan(disableTime);
        }

        // Returns true if breakpoint was recently cleared and isn't past the
        // max disable time.
        bool IsRecentlyCleared() {
            return GetMilliSpanSinceDisable() <= MAX_DISABLE_TIME && !inUse;
        }

        // Get whether it's safe to ignore this DR.  This is used in the
        // vectored exception handler.
        bool IsInUse() {
            return (inUse || GetMilliSpanSinceDisable() <= MAX_DISABLE_TIME);
        }

        // "Clears" the breakpoint and prepares it for "removal."  This does
        // not clear the address, but it sets the breakpoint to no longer in
        // use and also updates the disable time.
        void ClearBreakpoint() {
            inUse       = false;
            disableTime = GetMilliCount();
        }

        // "Sets" the breakpoint and enables it.
        void SetBreakpoint( const PVOID addr, const DWORD eipVal=0 ) {
            inUse   = true;
            address = addr;
            eip     = eipVal;
        }

    };

    struct SoftwareBreakpoint
    {
        unsigned char originalByte;
        ULONG64       hookAddr;
        DWORD         type;
        bool          terminator;

        enum
        {
            TYPE_LOG, // If log, then registers are logged
            TYPE_JUMP // If jump, then breakpoint redirects eip to the hookAddr
        };

        SoftwareBreakpoint()
        {
            originalByte = 0;
            hookAddr     = 0;
            type         = TYPE_LOG;
            terminator   = false;
        }
    };

    BreakpointAddress addr[4];

    bool logAll = false;
    
    std::map<ULONG64, MemoryBuffer> spoofBuffers;
    std::map<ULONG64, MemoryBuffer> memoryHooks;

    // contains mappings of addresses to bytes
    // used by software breakpoints
    // iterating through this should provide all current software breakpoints
    std::map<ULONG64, SoftwareBreakpoint> originalMemory;
    
    // address of the last software breakpoint to be triggered
    // this is used in the VEH to determine which breakpoint to replace
    ULONG64 lastSoftwareBreakpoint = 0;
}

Breakpoint::Breakpoint()
{
    for( int i=0; i < 4; ++i )
        addr[i].ClearBreakpoint();
    logAll = false;
}

Breakpoint::~Breakpoint()
{
    ClearHardwareBreakpoint( 0, true ); //Clear all hardware breakpoints
    ClearSoftwareBreakpoint( -1 );
    std::list<PVOID>::iterator iter = exceptionHandlerList.begin();
    for( ; iter != exceptionHandlerList.end(); ++iter )
        RemoveVectoredExceptionHandler( *iter );
}

// TODO:  Add support to the terminator for DRs
LONG WINAPI Breakpoint::ExceptionFilter( EXCEPTION_POINTERS *exceptionInfo )
{
    // check software breakpoints
    if( (exceptionInfo != NULL) && (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) )
    {
        static ULONG64 lastTerminatorStart = 0;
        // if it was found
        if( ::lastSoftwareBreakpoint != 0 )
        {
            WriteAddress( (LPVOID)::lastSoftwareBreakpoint, "\xCC", 1 ); // replace it with the breakpoint
            
            // If the last software breakpoint uses a terminator
            if( originalMemory.count(::lastSoftwareBreakpoint) > 0 &&
                originalMemory[::lastSoftwareBreakpoint].terminator )
            {
                lastTerminatorStart = ::lastSoftwareBreakpoint;
                callback(exceptionInfo); // log this value

                // If time to stop our logging
                if( exceptionInfo->ContextRecord->Eip == originalMemory[lastTerminatorStart].hookAddr )
                {
                    lastTerminatorStart = 0;
                    exceptionInfo->ContextRecord->EFlags &= ~0x100; // remove single step (trap flag)
                }
                else
                    exceptionInfo->ContextRecord->EFlags |= 0x100;
            }
            else // Not a terminator, so continue normal execution
                exceptionInfo->ContextRecord->EFlags &= ~0x100; // remove single step (trap flag)

            ::lastSoftwareBreakpoint = 0; // reset it
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        else if( lastTerminatorStart != 0 ) // sentinel for terminators
        {
            callback(exceptionInfo);

            // If time to stop our logging
            if( exceptionInfo->ContextRecord->Eip == originalMemory[lastTerminatorStart].hookAddr )
            {
                lastTerminatorStart = 0;
                exceptionInfo->ContextRecord->EFlags &= ~0x100; // remove single step (trap flag)
            }
            else
                exceptionInfo->ContextRecord->EFlags |= 0x100;
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

    // check software breakpoints
    if( (exceptionInfo != NULL) && (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) )
    {
        ULONG64 exceptionAddr = (ULONG64)exceptionInfo->ExceptionRecord->ExceptionAddress;

        // check software breakpoints
        std::map<ULONG64, SoftwareBreakpoint>::iterator iter = originalMemory.begin();
        for( ; iter != originalMemory.end(); ++iter )
        {
            if( exceptionAddr == iter->first )
            {
                if( iter->second.type == SoftwareBreakpoint::TYPE_JUMP ) // if type is a jump
                {
                    exceptionInfo->ContextRecord->Eip = (DWORD)iter->second.hookAddr;
                    return EXCEPTION_CONTINUE_EXECUTION;
                }
                else if( iter->second.type == SoftwareBreakpoint::TYPE_LOG ) // if type is a log
                {
                    //((RiME *)RiMEPtr)->LogRegisters( exceptionInfo );
                    callback( exceptionInfo );
                    if( freezeOnException )
                    {
                        SuspendOtherThreads( threadHandle );
                        if( GetThreadIdXP( threadHandle ) != GetCurrentThreadId() )
                            SuspendCurrentThread();
                    }
                    // replace the memory and single step back over it
                    WriteAddress( (LPVOID)exceptionAddr, &originalMemory[exceptionAddr].originalByte, 1 );
                    exceptionInfo->ContextRecord->EFlags |= 0x100; // single step (trap flag)
                    ::lastSoftwareBreakpoint = iter->first; // set last software breakpoint
                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }
        }
    }

    // check hardware breakpoints
    else if( (exceptionInfo != NULL) && (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) )
    {
        ULONG64 exceptionAddr = (ULONG64)exceptionInfo->ExceptionRecord->ExceptionAddress;

        // check hardware breakpoints
        // if logging all hardware breakpoint addresses
        if( logAll )
        {
            bool ourBreakpoint = true;
            if(      !addr[0].IsInUse() && exceptionInfo->ContextRecord->Dr0 == exceptionAddr )
                ourBreakpoint = false;
            else if( !addr[1].IsInUse() && exceptionInfo->ContextRecord->Dr1 == exceptionAddr )
                ourBreakpoint = false;
            else if( !addr[2].IsInUse() && exceptionInfo->ContextRecord->Dr2 == exceptionAddr )
                ourBreakpoint = false;
            else if( !addr[3].IsInUse() && exceptionInfo->ContextRecord->Dr3 == exceptionAddr )
                ourBreakpoint = false;

            if( ourBreakpoint )
            {
                //TODO:  Alternatively, you can single-step the instruction
                //((RiME *)RiMEPtr)->LogRegisters( exceptionInfo );
                callback( exceptionInfo );
                if( freezeOnException )
                {
                    SuspendOtherThreads( threadHandle );
                    if( GetThreadIdXP( threadHandle ) != GetCurrentThreadId() )
                        SuspendCurrentThread();
                }
                //TODO: Currently this only supports Windows Vista and Windows 7.
                exceptionInfo->ContextRecord->EFlags |= 0x10000; //Set the Resume Flag (only works under Windows Vista and Windows 7)
                // Run through the DRs again and look for "expired" DRs.
                // If found, correct the DR (reset it to 0)
                for( int i=0; i < 4; ++i )
                {
                    if( addr[i].IsInUse() && addr[i].IsRecentlyCleared() )
                    {
                        switch(i)
                        {
                        case 0:
                            exceptionInfo->ContextRecord->Dr0  = 0;
                            exceptionInfo->ContextRecord->Dr7 &= 0xFFF0FFFC;
                            break;
                        case 1:
                            exceptionInfo->ContextRecord->Dr1  = 0;
                            exceptionInfo->ContextRecord->Dr7 &= 0xFF0FFFF3;
                            break;
                        case 2:
                            exceptionInfo->ContextRecord->Dr2  = 0;
                            exceptionInfo->ContextRecord->Dr7 &= 0xF0FFFFCF;
                            break;
                        case 3:
                            exceptionInfo->ContextRecord->Dr3  = 0;
                            exceptionInfo->ContextRecord->Dr7 &= 0x0FFFFF3F;
                            break;
                        }
                    }
                }
                return EXCEPTION_CONTINUE_EXECUTION;
            }
            else
                return EXCEPTION_CONTINUE_SEARCH;
        }
        else
        {
            for( int i=0; i < 4; ++i )
            {
                if( (addr[i].IsInUse()) 
                    && ((PVOID)exceptionAddr == addr[i].GetAddress()) )
                {
                    if( addr[i].GetEip() != 0 )
                    {
                        exceptionInfo->ContextRecord->Eip = addr[i].GetEip();
                        return EXCEPTION_CONTINUE_EXECUTION;
                    }
                    else // eip is 0, so log instead of redirect
                    {
                        // Set a breakpoint on Dr1 for this thread
                        //SetContextHardwareBreakpoint( 0x004D1300, NULL, BREAK_ON_BYTE, BREAK_ON_EXECUTION, exceptionInfo->ContextRecord, 1 );

                        //((RiME *)RiMEPtr)->LogRegisters( exceptionInfo );
                        callback( exceptionInfo );
                        if( freezeOnException )
                        {
                            SuspendOtherThreads( threadHandle );
                            if( GetThreadIdXP( threadHandle ) != GetCurrentThreadId() )
                                SuspendCurrentThread();
                        }
                        //TODO: Currently this only supports Windows Vista and Windows 7.
                        exceptionInfo->ContextRecord->EFlags |= 0x10000; //Set the Resume Flag (only works under Windows Vista and Windows 7)
                        // Run through the DRs again and look for "expired" DRs.
                        // If found, correct the DR (reset it to 0)
                        for( int i=0; i < 4; ++i )
                        {
                            if( addr[i].IsInUse() && addr[i].IsRecentlyCleared() )
                            {
                                switch(i)
                                {
                                case 0:
                                    exceptionInfo->ContextRecord->Dr0  = 0;
                                    exceptionInfo->ContextRecord->Dr7 &= 0xFFF0FFFC;
                                    break;
                                case 1:
                                    exceptionInfo->ContextRecord->Dr1  = 0;
                                    exceptionInfo->ContextRecord->Dr7 &= 0xFF0FFFF3;
                                    break;
                                case 2:
                                    exceptionInfo->ContextRecord->Dr2  = 0;
                                    exceptionInfo->ContextRecord->Dr7 &= 0xF0FFFFCF;
                                    break;
                                case 3:
                                    exceptionInfo->ContextRecord->Dr3  = 0;
                                    exceptionInfo->ContextRecord->Dr7 &= 0x0FFFFF3F;
                                    break;
                                }
                            }
                        }
                        return EXCEPTION_CONTINUE_EXECUTION;
                    }
                }
            }
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

bool Breakpoint::SetContextHardwareBreakpoint( const DWORD breakAddr,
                                               const DWORD hookAddr, 
                                               const DebugControlFlag sz, 
                                               const DebugControlFlag trigType, 
                                               PCONTEXT contextRecord,
                                               const unsigned char drToUse )
{
    bool    retVal = true;
    DWORD   tempRegVal;

    switch( drToUse )
    {
    case 0:
        addr[0].SetBreakpoint((PVOID)breakAddr, hookAddr);
        contextRecord->Dr0 = breakAddr;
        tempRegVal         = contextRecord->Dr7 & 0xFFF0FFFC;
        contextRecord->Dr7 = tempRegVal | (BREAK_ON_LOCAL << 0) | (trigType << 16) | (sz << 18);
        break;

    case 1:
        addr[1].SetBreakpoint((PVOID)breakAddr, hookAddr);
        contextRecord->Dr1 = breakAddr;
        tempRegVal         = contextRecord->Dr7 & 0xFF0FFFF3;
        contextRecord->Dr7 = tempRegVal | (BREAK_ON_LOCAL << 2) | (trigType << 20) | (sz << 22);
        break;

    case 2:
        addr[2].SetBreakpoint((PVOID)breakAddr, hookAddr);
        contextRecord->Dr2 = breakAddr;
        tempRegVal         = contextRecord->Dr7 & 0xF0FFFFCF;
        contextRecord->Dr7 = tempRegVal | (BREAK_ON_LOCAL << 4) | (trigType << 24) | (sz << 26);
        break;

    case 3:
        addr[3].SetBreakpoint((PVOID)breakAddr, hookAddr);
        contextRecord->Dr3 = breakAddr;
        tempRegVal         = contextRecord->Dr7 & 0x0FFFFF3F;
        contextRecord->Dr7 = tempRegVal | (BREAK_ON_LOCAL << 6) | (trigType << 28) | (sz << 30);
        break;
    }

    return retVal;
};

bool Breakpoint::SetThreadHardwareBreakpoint( const DWORD breakAddr,
                                              const DWORD hookAddr, 
                                              const DebugControlFlag sz, 
                                              const DebugControlFlag trigType, 
                                              const HANDLE hThread, 
                                              const unsigned char drToUse,
                                              const bool hookIsTerminator )
{
    bool    retVal = true;
    CONTEXT context;
    DWORD   tempRegVal;
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if( GetThreadContext( hThread, &context ) )
    {
        switch( drToUse )
        {
          case 0:
            addr[0].SetBreakpoint((PVOID)breakAddr, hookAddr);
            addr[0].SetTerminator(hookIsTerminator);
            context.Dr0      = breakAddr;
            tempRegVal       = context.Dr7 & 0xFFF0FFFC;
            context.Dr7      = tempRegVal | (BREAK_ON_LOCAL << 0) | (trigType << 16) | (sz << 18);
            break;

          case 1:
            addr[1].SetBreakpoint((PVOID)breakAddr, hookAddr);
            addr[1].SetTerminator(hookIsTerminator);
            context.Dr1      = breakAddr;
            tempRegVal       = context.Dr7 & 0xFF0FFFF3;
            context.Dr7      = tempRegVal | (BREAK_ON_LOCAL << 2) | (trigType << 20) | (sz << 22);
            break;

          case 2:
            addr[2].SetBreakpoint((PVOID)breakAddr, hookAddr);
            addr[2].SetTerminator(hookIsTerminator);
            context.Dr2      = breakAddr;
            tempRegVal       = context.Dr7 & 0xF0FFFFCF;
            context.Dr7      = tempRegVal | (BREAK_ON_LOCAL << 4) | (trigType << 24) | (sz << 26);
            break;

          case 3:
            addr[3].SetBreakpoint((PVOID)breakAddr, hookAddr);
            addr[3].SetTerminator(hookIsTerminator);
            context.Dr3      = breakAddr;
            tempRegVal       = context.Dr7 & 0x0FFFFF3F;
            context.Dr7      = tempRegVal | (BREAK_ON_LOCAL << 6) | (trigType << 28) | (sz << 30);
            break;
        }

        if( retVal == true ) //If we haven't failed yet
        {
            //if( GetCurrentThreadId() != GetThreadId( hThread ) )
            //if( GetThreadId( threadHandle ) != GetThreadId( hThread ) )
            if( GetThreadIdXP( threadHandle ) != GetThreadIdXP( hThread ) )
            {
                if( SuspendThread( hThread ) == (DWORD)-1 )
                {
                    retVal = false;
                }
                else if( !SetThreadContext( hThread, &context ) )
                {
                    retVal = false;
                }
            }
            if( ResumeThread( hThread ) == (DWORD)-1 )
            {
                retVal = false;;
            }
        }
    }
    else
    {
        return false;
    }
    return retVal;
}

// If hookIsTerminator is set to true, then the breakpoint performs single-steps until
// hookAddr is reached.
DWORD Breakpoint::SetHardwareBreakpoint(const DWORD breakAddr,
                                        const DWORD hookAddr, 
                                        const DebugControlFlag sz, 
                                        const DebugControlFlag trigType, 
                                        int drToUse,
                                        const bool hookIsTerminator)
{
    if (exceptionHandlerList.empty())
        exceptionHandlerList.push_back( AddVectoredExceptionHandler( 1, ExceptionFilter ) );

    DWORD  processId;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    HANDLE        hThread;
    DWORD         threadAmount = 0;

    if( hSnap == INVALID_HANDLE_VALUE )
    {
        return 0;
    }

    if( drToUse == -1 ) //Gets the DR to use (and preserves it for all threads)
    {
        //if( (drInUse[0] == false) )
        //    drToUse = 0;
        if( (addr[1].IsInUse() == false) )
            drToUse = 1;
        else if( (addr[2].IsInUse() == false) )
            drToUse = 2;
        else if( (addr[3].IsInUse() == false) ) //&& (context.Dr3 == 0)
            drToUse = 3;
    }

    processId = GetCurrentProcessId();
    te32.dwSize = sizeof( THREADENTRY32 );
    if( (hSnap != INVALID_HANDLE_VALUE) && Thread32First( hSnap, &te32 ) && (drToUse != -1) )
    {
        do //For all threads
        {
            if( te32.th32OwnerProcessID == processId ) //If thread is part of process
            {
                hThread = OpenThread( THREAD_QUERY_INFORMATION|THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_INFORMATION|THREAD_SET_CONTEXT, TRUE, te32.th32ThreadID );
                if( hThread != NULL )
                {
                    if( SetThreadHardwareBreakpoint( breakAddr, hookAddr, sz, trigType, hThread, drToUse, hookIsTerminator ) )
                        ++threadAmount;
                    CloseHandle( hThread ); //This can fail, but who cares?
                }
            }
        }while( Thread32Next( hSnap, &te32 ) );
    }

    return threadAmount;
}

// If hookIsTerminator is set to true, then the breakpoint performs single-steps until
// hookAddr is reached.
DWORD Breakpoint::SetSoftwareBreakpoint(const ULONG64 breakAddr, const ULONG64 hookAddr, const bool hookIsTerminator )
{
    if (exceptionHandlerList.empty())
        exceptionHandlerList.push_back( AddVectoredExceptionHandler( 1, ExceptionFilter ) );

    DWORD result = 0;

    unsigned char oldByte = static_cast<unsigned char>(ReadAddress((LPCVOID)breakAddr));

    if( oldByte != 0xCC ) { // don't set breakpoints on breakpoints.  That's stupid!
        originalMemory[breakAddr].originalByte = oldByte;
        originalMemory[breakAddr].hookAddr = hookAddr;
        originalMemory[breakAddr].terminator = hookIsTerminator;
        result = static_cast<DWORD>(WriteAddress( (LPVOID)breakAddr, "\xCC", 1 ));
    }

    return result;
}

// addr doubles up as a specific register
DWORD Breakpoint::ClearThreadHardwareBreakpoint(const DWORD address,
                                                const HANDLE hThread, 
                                                bool clearAll)
{
    CONTEXT context;
    DWORD   affectedRegisters = 0;
    DWORD   retVal = 0;
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if( GetThreadContext( hThread, &context ) ) {
        for( int i=0; i < 4; ++i ) {
            if( (addr[i].IsInUse() && ( (PVOID)address == addr[i].GetAddress())) || 
                clearAll || 
                address == i ) {
                addr[i].ClearBreakpoint();
                affectedRegisters |= (1 << i);
            }
        }
        if( affectedRegisters & 0x01 ) //Dr0
        {
            context.Dr0 = 0;
            context.Dr7 = context.Dr7 & 0xFFF0FFFC;
        }
        if( affectedRegisters & 0x02 ) //Dr1
        {
            context.Dr1 = 0;
            context.Dr7 = context.Dr7 & 0xFF0FFFF3;
        }
        if( affectedRegisters & 0x04 ) //Dr2
        {
            context.Dr2 = 0;
            context.Dr7 = context.Dr7 & 0xF0FFFFCF;
        }
        if( affectedRegisters & 0x08 ) //Dr3
        {
            context.Dr3 = 0;
            context.Dr7 = context.Dr7 & 0x0FFFFF3F;
        }

        //if( GetCurrentThreadId() != GetThreadId( hThread ) )
        //if( GetThreadId( threadHandle) != GetThreadId( hThread ) )
        if( GetThreadIdXP( threadHandle ) != GetThreadIdXP( hThread ) )
        {
            if( SuspendThread( hThread ) != -1 ) {
                if( SetThreadContext( hThread, &context ) ) {
                    retVal = 1;
                }
            }
        }
        ResumeThread( hThread );
    }
    return retVal;
}

DWORD Breakpoint::ClearHardwareBreakpoint(const DWORD addr, bool clearAll)
{
    DWORD  processId;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    HANDLE        hThread;
    DWORD         threadAmount = 0;

    processId = GetCurrentProcessId();
    te32.dwSize = sizeof( THREADENTRY32 );
    if( (hSnap != INVALID_HANDLE_VALUE) && Thread32First( hSnap, &te32 ) ) {
        do { //For all threads
            if( te32.th32OwnerProcessID == processId ) { //If thread is part of process
                hThread = OpenThread( THREAD_QUERY_INFORMATION|THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_INFORMATION|THREAD_SET_CONTEXT, TRUE, te32.th32ThreadID );
                if( hThread != NULL ) {
                    if( (ClearThreadHardwareBreakpoint( addr, hThread, clearAll ) > 0) ) {
                        ++threadAmount;
                    }
                    CloseHandle( hThread );
                }
            }
        } while( Thread32Next( hSnap, &te32 ) );
    }

    return threadAmount;
}

DWORD Breakpoint::ClearSoftwareBreakpoint( const ULONG64 addr ) {
    DWORD result = 0;
    std::map<ULONG64, SoftwareBreakpoint>::iterator iter = originalMemory.find(addr);
    if( iter != originalMemory.end() ) {
        // try 5 times to write to the address, then give up
        for( int i=0; i < 5; ++i ) {
            if( WriteAddress((LPVOID)addr, &(iter->second.originalByte), 1) ) {
                result = 1;
                break; // success!
            }
        }
        originalMemory.erase(iter);
    }
    
    return result;
}

unsigned long GetThreadIdXP(HANDLE hThread)
{
    unsigned long nThId = 0;
    THREAD_BASIC_INFORMATION tbi;
    HMODULE hLib = 0L;
    NtQueryInformationThread_PROC hProc = 0L;
    long used = 0, ret = 0;

    hLib = LoadLibrary(L"ntdll.dll");
    if (hLib != 0L)
    {
        hProc = (NtQueryInformationThread_PROC) GetProcAddress(hLib,
            "NtQueryInformationThread");

        if (hProc != 0L)
        {
            ret = hProc(hThread, 0, &tbi, sizeof(THREAD_BASIC_INFORMATION),
                &used);
            if (ret == 0)
                nThId = tbi.UniqueThreadId;
        }

        FreeLibrary(hLib);
    }

    return nThId;
}

void  Breakpoint::SetLogAll(bool val)
{
    logAll = val;
    return;
}

bool  Breakpoint::GetLogAll()
{
    return logAll;
}

void Breakpoint::SetCallbackFunction( BreakpointCallbackFunction f )
{
    callback = f;
}

bool ResumeOtherThreads()
{
    bool          retVal = false;
    DWORD         processId;
    HANDLE        hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    HANDLE        hThread;

    processId = GetCurrentProcessId();
    te32.dwSize = sizeof( THREADENTRY32 );
    if( (hSnap != INVALID_HANDLE_VALUE) && Thread32First( hSnap, &te32 ) )
    {
        retVal = true;
        do //For all threads
        {
            if( te32.th32OwnerProcessID == processId ) //If thread is part of process
            {
                hThread = OpenThread( THREAD_QUERY_INFORMATION|THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_INFORMATION|THREAD_SET_CONTEXT, TRUE, te32.th32ThreadID );
                if( hThread != NULL )
                {
                    //if( GetCurrentThreadId() != GetThreadId( hThread ) )
                    //if( GetThreadId( threadHandle ) != GetThreadId( hThread ) )
                    if( GetThreadIdXP( threadHandle ) != GetThreadIdXP( hThread ) )
                        ResumeThread( hThread );
                    CloseHandle( hThread ); //This can fail, but who cares?
                }
            }
        }while( Thread32Next( hSnap, &te32 ) );
    }
    return retVal;
}

bool SuspendOtherThreads(HANDLE hThread)
{
    bool          retVal = false;
    DWORD         processId;
    HANDLE        hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    HANDLE        hThread2;

    processId = GetCurrentProcessId();
    te32.dwSize = sizeof( THREADENTRY32 );
    if( (hSnap != INVALID_HANDLE_VALUE) && Thread32First( hSnap, &te32 ) )
    {
        retVal = true;
        do //For all threads
        {
            if( te32.th32OwnerProcessID == processId ) //If thread is part of process
            {
                hThread2 = OpenThread( THREAD_QUERY_INFORMATION|THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_INFORMATION|THREAD_SET_CONTEXT, TRUE, te32.th32ThreadID );
                if( hThread2 != hThread )
                {
                    //if( GetCurrentThreadId() != GetThreadId( hThread2 ) )
                    //if( GetThreadId( threadHandle ) != GetThreadId( hThread2 ) )
                    if( GetThreadIdXP( threadHandle ) != GetThreadIdXP( hThread2 ) )
                        SuspendThread( hThread2 );
                }
                CloseHandle( hThread2 ); //This can fail, but who cares?
            }
        }while( Thread32Next( hSnap, &te32 ) );
    }
    return retVal;
}

void SuspendCurrentThread()
{
    DWORD         processId;
    HANDLE        hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    HANDLE        hThread;

    processId = GetCurrentProcessId();
    te32.dwSize = sizeof( THREADENTRY32 );
    if( (hSnap != INVALID_HANDLE_VALUE) && Thread32First( hSnap, &te32 ) )
    {
        do //For all threads
        {
            if( te32.th32OwnerProcessID == processId ) //If thread is part of process
            {
                hThread = OpenThread( THREAD_QUERY_INFORMATION|THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_INFORMATION|THREAD_SET_CONTEXT, TRUE, te32.th32ThreadID );
                if( hThread != NULL )
                {
                    //if( GetCurrentThreadId() != GetThreadId( hThread ) )
                    //if( GetThreadId( threadHandle ) != GetThreadId( hThread ) )
                    if( GetCurrentThreadId() == GetThreadIdXP( hThread ) )
                        SuspendThread( hThread );
                }
                CloseHandle( hThread ); //This can fail, but who cares?
            }
        }while( Thread32Next( hSnap, &te32 ) );
    }
    return;
}

bool Breakpoint::IsAddressInUse( const int index )
{
    return addr[index].IsInUse();
}

PVOID Breakpoint::GetBreakpointAddress( const int index )
{
    return addr[index].GetAddress();
}

PVOID Breakpoint::GetTerminatorAddress( const int index )
{
    if( index < 4 )
        return addr[index].GetTerminatorAddress();
    else if( originalMemory.count(index) > 0 )
        return (PVOID)originalMemory[index].hookAddr;

    return 0;
}

// Returns true if the breakpoint at index has a terminator
bool Breakpoint::HasTerminatorAddress( const int index )
{
    if( index < 4 )
        return addr[index].GetTerminator();
    else if( originalMemory.count(index) > 0 )
        return originalMemory[index].terminator;

    return false;
}

// Return all addresses in use.  Addresses 0-3 are hardware breakpoints.
// To get the address of those hardware breakpoints, you should call
// GetBreakpointAddress.
std::list<ULONG64> Breakpoint::GetAddressesInUse() {
    std::list<ULONG64> result;

    // hardware breakpoints
    for( int i=0; i < 4; ++i ) {
        if( IsAddressInUse(i) ) {
            result.push_back(i);
        }
    }

    // software breakpoints
    std::map<ULONG64, SoftwareBreakpoint>::iterator iter = originalMemory.begin();
    for( ; iter != originalMemory.end(); ++iter ) {
        result.push_back(iter->first);
    }

    return result;
}
