#ifndef __BREAKPOINT
#define __BREAKPOINT

//#include <windows.h>
#include <string>
#include <list>
#include <utility>

#include <TlHelp32.h>
#include <Winternl.h>

#include <wx/string.h>

#include "defs.h"

#include "millitime.h"

extern HANDLE threadHandle;

typedef long (__stdcall *NtQueryInformationThread_PROC)(HANDLE thread,
    int infoclass, void *buf, long size, long *used);
typedef struct _THREAD_BASIC_INFORMATION {
    unsigned long ExitStatus;
    void *TebBaseAddress;
    ULONG UniqueProcessId;
    ULONG UniqueThreadId;
    unsigned long AffinityMask;
    unsigned long BasePriority;
    unsigned long DiffProcessPriority;

} THREAD_BASIC_INFORMATION;


enum DebugControlFlag
{
    BREAK_ON_EXECUTION     = 0x00,
    BREAK_ON_BYTE          = BREAK_ON_EXECUTION,
    BREAK_NEVER            = BREAK_ON_EXECUTION,
    BREAK_ON_WRITE         = 0x01,
    BREAK_ON_WORD          = BREAK_ON_WRITE,
    BREAK_ON_LOCAL         = BREAK_ON_WRITE,
    BREAK_ON_IO_READ_WRITE = 0x02,
    BREAK_ON_QWORD         = BREAK_ON_IO_READ_WRITE,
    BREAK_ON_GLOBAL        = BREAK_ON_IO_READ_WRITE, //BREAK_ON_IO_READ_WRITE is not supported by hardware
    BREAK_ON_READ_WRITE    = 0x03,
    BREAK_ON_DWORD         = BREAK_ON_READ_WRITE,
    BREAK_ON_LOCAL_GLOBAL  = BREAK_ON_READ_WRITE
};

extern bool freezeOnException;

typedef void( *BreakpointCallbackFunction)( EXCEPTION_POINTERS *exceptionInfo );

class Breakpoint
{
private:
    HWND        processHWND;
    std::list<PVOID> exceptionHandlerList;
    DWORD       logHandlerPtr; // A pointer to a function that will handle the logging

    static bool SetContextHardwareBreakpoint( const DWORD breakAddr,
        const DWORD hookAddr, 
        const DebugControlFlag sz, 
        const DebugControlFlag trigType, 
        PCONTEXT contextRecord,
        const unsigned char drToUse );
    static bool  SetThreadHardwareBreakpoint( const DWORD breakAddr, const DWORD hookAddr, const DebugControlFlag sz, const DebugControlFlag trigType, const HANDLE hThread, const unsigned char drToUse, const bool hookIsTerminator );

    DWORD ClearThreadHardwareBreakpoint( const DWORD address, const HANDLE hThread, bool clearAll=false );

    static void SpoofDRCave();

public:
    Breakpoint();

    ~Breakpoint();

    static LONG WINAPI ExceptionFilter( EXCEPTION_POINTERS* exceptionInfo );

    DWORD SetHardwareBreakpoint( const DWORD breakAddr, const DWORD hookAddr, const DebugControlFlag sz = BREAK_ON_BYTE, const DebugControlFlag trigType = BREAK_ON_EXECUTION, int drToUse = -1, const bool hookIsTerminator = false );
    DWORD SetSoftwareBreakpoint( const ULONG64 breakAddr, const ULONG64 hookAddr, const bool hookIsTerminator = false );

    DWORD ClearHardwareBreakpoint( const DWORD addr, bool clearAll = false );
    DWORD ClearSoftwareBreakpoint( const ULONG64 addr );

    void  SetLogAll(bool val = true);

    bool  GetLogAll();

    static void SetCallbackFunction( BreakpointCallbackFunction f );

    void SetLogHandler( PVOID functionPtr )
    {
        logHandlerPtr = (DWORD)functionPtr;
    }

    // spoof debug registers returned by ZwGetContextThread to all 0's.
    // You may include a module substring which will refine the modules
    // which will be spoofed to only those containing the substring.
    bool SpoofDR( bool enable, wxString moduleSubstr = "" );

    static bool IsAddressInUse( const int index );

    static PVOID GetBreakpointAddress( const int index );
    static PVOID GetTerminatorAddress( const int index );
    static bool  HasTerminatorAddress( const int index );

    // Return all addresses in use.  Addresses 0-3 are hardware breakpoints.
    // To get the address of those hardware breakpoints, you should call
    // GetBreakpointAddress.
    static std::list<ULONG64> GetAddressesInUse();
};

unsigned long GetThreadIdXP(HANDLE hThread);

bool ResumeOtherThreads();

bool SuspendOtherThreads(HANDLE threadException = NULL);

void SuspendCurrentThread();

#endif
