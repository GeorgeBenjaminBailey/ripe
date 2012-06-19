/*********************************************************************
 * Structures and definitions undocumented or included in the NTDDK. *
 *********************************************************************/

#ifndef __STRUCT_H__
#define __STRUCT_H__

///////////////// Windows NT ///////////////

typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef LONG NTSTATUS, *PNTSTATUS;
typedef ULONG KAFFINITY, *PKAFFINITY;
typedef LONG KPRIORITY;

typedef void (CALLBACK *PKNORMAL_ROUTINE)(PVOID, PVOID, PVOID);

typedef struct _CLIENT_ID {
   HANDLE UniqueProcess;
   HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );        \
    (p)->RootDirectory = r;                           \
    (p)->Attributes = a;                              \
    (p)->ObjectName = n;                              \
    (p)->SecurityDescriptor = s;                      \
    (p)->SecurityQualityOfService = NULL;             \
    }

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L

typedef struct _PEB_FREE_BLOCK {  // Size = 8
struct _PEB_FREE_BLOCK *Next;
ULONG Size;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

// Structured Exception Handler
typedef struct _SEH {
    struct _SEH *pNext;
    FARPROC     pfnHandler;
} SEH, *PSEH;

#pragma pack(1)

// Thread Information Block (FS:[0])
typedef struct _TIB_NT {
    PSEH       pvExcept;             // 00 Head of exception record list
    PVOID      pvStackBase;          // 04
    PVOID      pvStackLimit;         // 08
    PVOID      SubSystemTib;         // 0C
    union {
        PVOID FiberData;             // 10
        DWORD Version;
    };
    PVOID      pvArbitrary;          // 14 Available for application use
    struct _TIB_NT *pTIBSelf;        // 18 Linear address of TIB structure
} TIB_NT, *PTIB_NT;

typedef void (*PPEBLOCKROUTINE)(PVOID);

#pragma pack()

typedef struct _THREAD_BASIC_INFORMATION {
     NTSTATUS  ExitStatus;
     PTIB_NT   TebBaseAddress;
     CLIENT_ID ClientId;
     KAFFINITY AffinityMask;
     KPRIORITY Priority;
     KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define SystemProcessesAndThreadsInformation    5

typedef struct _VM_COUNTERS {
    SIZE_T          PeakVirtualSize;
    SIZE_T          VirtualSize;
    ULONG           PageFaultCount;
    SIZE_T          PeakWorkingSetSize;
    SIZE_T          WorkingSetSize;
    SIZE_T          QuotaPeakPagedPoolUsage;
    SIZE_T          QuotaPagedPoolUsage;
    SIZE_T          QuotaPeakNonPagedPoolUsage;
    SIZE_T          QuotaNonPagedPoolUsage;
    SIZE_T          PagefileUsage;
    SIZE_T          PeakPagefileUsage;
} VM_COUNTERS;

typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER   KernelTime;
    LARGE_INTEGER   UserTime;
    LARGE_INTEGER   CreateTime;
    ULONG                 WaitTime;
    PVOID                 StartAddress;
    CLIENT_ID     ClientId;
    KPRIORITY     Priority;
    KPRIORITY     BasePriority;
    ULONG                 ContextSwitchCount;
    LONG                  State;
    LONG                  WaitReason;
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

/////////////////////// Windows 95 /////////////////////////

#pragma pack(1)

#define VA_SHARED 0x8000000             // Undocumented flag to allocate shared memory in Win9x

// Kernel32 objects (WIN95)
#define WIN95_K32OBJ_SEMAPHORE            0x1
#define WIN95_K32OBJ_EVENT                0x2
#define WIN95_K32OBJ_MUTEX                0x3
#define WIN95_K32OBJ_CRITICAL_SECTION     0x4
#define WIN95_K32OBJ_PROCESS              0x5
#define WIN95_K32OBJ_THREAD               0x6
#define WIN95_K32OBJ_FILE                 0x7
#define WIN95_K32OBJ_CHANGE               0x8
#define WIN95_K32OBJ_CONSOLE              0x9
#define WIN95_K32OBJ_SCREEN_BUFFER        0xA
#define WIN95_K32OBJ_MEM_MAPPED_FILE      0xB
#define WIN95_K32OBJ_SERIAL               0xC
#define WIN95_K32OBJ_DEVICE_IOCTL         0xD
#define WIN95_K32OBJ_PIPE                 0xE
#define WIN95_K32OBJ_MAILSLOT             0xF
#define WIN95_K32OBJ_TOOLHELP_SNAPSHOT    0x10
#define WIN95_K32OBJ_SOCKET               0x11

// TIB flags
#define TIBF_WIN32                    0x00000001
#define TIBF_TRAP                     0x00000002

// Process Database flags (WIN95)
#define fDebugSingle                  0x00000001  // Set if process is being debugged
#define fCreateProcessEvent           0x00000002  // Set in debugged process after starting
#define fExitProcessEvent             0x00000004  // Might be set in debugged process at exit time
#define fWin16Process                 0x00000008  // 16-bit process
#define fDosProcess                   0x00000010  // DOS process
#define fConsoleProcess               0x00000020  // 32-bit console process
#define fFileApisAreOem               0x00000040  // SetFileAPIsToOEM
#define fNukeProcess                  0x00000080
#define fServiceProcess               0x00000100  // RegisterServiceProcess
#define fLoginScriptHack              0x00000800  // Might be a Novell network login process
#define fSendDllNotifications         0x00200000
#define fDebugEventPending            0x00400000  // e.g. stopped in debugger
#define fNearlyTerminating            0x00800000
#define fFaulted                      0x08000000
#define fTerminating                  0x10000000
#define fTerminated                   0x20000000
#define fInitError                    0x40000000
#define fSignaled                     0x80000000

// Thread Database flags (WIN95)
#define fCreateThreadEvent            0x00000001   // Set if thread is being debugged
#define fCancelExceptionAbort         0x00000002
#define fOnTempStack                  0x00000004
#define fGrowableStack                0x00000008
#define fDelaySingleStep              0x00000010
#define fOpenExeAsImmovableFile       0x00000020
#define fCreateSuspended              0x00000040   // CREATE_SUSPENDED flag to CreateProcess()
#define fStackOverflow                0x00000080
#define fNestedCleanAPCs              0x00000100
#define fWasOemNowAnsi                0x00000200   // ANSI/OEM file function
#define fOKToSetThreadOem             0x00000400   // ANSI/OEM file function

// TDBX flags (WIN95)
#define WAITEXBIT                     0x00000001
#define WAITACKBIT                    0x00000002
#define SUSPEND_APC_PENDING           0x00000004
#define SUSPEND_TERMINATED            0x00000008
#define BLOCKED_FOR_TERMINATION       0x00000010
#define EMULATE_NPX                   0x00000020
#define WIN32_NPX                     0x00000040
#define EXTENDED_HANDLES              0x00000080
#define FROZEN                        0x00000100
#define DONT_FREEZE                   0x00000200
#define DONT_UNFREEZE                 0x00000400
#define DONT_TRACE                    0x00000800
#define STOP_TRACING                  0x00001000
#define WAITING_FOR_CRST_SAFE         0x00002000
#define CRST_SAFE                     0x00004000
#define BLOCK_TERMINATE_APC           0x00040000

// Environment Database
typedef struct _ENVIRONMENT_DATABASE {
    PSTR    pszEnvironment;             //00 Pointer to Process Environment
    DWORD   un1;                        //04 (always 0)
    PSTR    pszCmdLine;                 //08 Pointer to command line
    PSTR    pszCurrDirectory;           //0C Pointer to current directory
    LPSTARTUPINFOA pStartupInfo;        //10 Pointer to STARTUPINFOA struct
    HANDLE  hStdIn;                     //14 Standard Input handle
    HANDLE  hStdOut;                    //18 Standard Output handle
    HANDLE  hStdErr;                    //1C Standard Error handle
    DWORD   un2;                        //20 (always 1)
    DWORD   InheritConsole;             //24 Inherit console from parent
    DWORD   BreakType;                  //28 Handle console events (like CTRL+C)
    DWORD   BreakSem;                   //2C Pointer to K32OBJ_SEMAPHORE
    DWORD   BreakEvent;                 //30 Pointer to K32OBJ_EVENT
    DWORD   BreakThreadID;              //34 Pointer to K32OBJ_THREAD
    DWORD   BreakHandlers;              //38 Pointer to list of installed console control handlers
} EDB, *PEDB;

// Handle Table Entry
typedef struct _HANDLE_TABLE_ENTRY {
    DWORD  flags;                   // Valid flags depend on what type of object this is
    PVOID  pObject;                 // Pointer to the object that the handle refers to
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

// Handle Table
typedef struct _HANDLE_TABLE {
    DWORD cEntries;                 // Max number of handles in table
    HANDLE_TABLE_ENTRY array[1];    // An array (number is given by cEntries)
} HANDLE_TABLE, *PHANDLE_TABLE;

typedef struct _IMTE
{
    DWORD           un1;            // 00h
    PIMAGE_NT_HEADERS   pNTHdr;     // 04h
    DWORD           un2;            // 08h
    PSTR            pszFileName;    // 0Ch
    PSTR            pszModName;     // 10h
    WORD            cbFileName;     // 14h
    WORD            cbModName;      // 16h
    DWORD           un3;            // 18h
    DWORD           cSections;      // 1Ch
    DWORD           un5;            // 20h
    DWORD           baseAddress;    // 24h
    WORD            hModule16;      // 28h
    WORD            cUsage;         // 2Ah
    DWORD           un7;            // 2Ch
    PSTR            pszFileName2;   // 30h
    WORD            cbFileName2;    // 34h
    PSTR            pszModName2;    // 36h
    WORD            cbModName2;     // 3Ah
} IMTE, *PIMTE;

// Thread Information Block (FS:[0x18])
typedef struct _TIB95 {         // Size = 0x34
    PSEH    pvExcept;           // 00 Pointer to head of structured exception handling chain
    PVOID   pvStackUserTop;     // 04 Max. address for stack
    PVOID   pvStackUserBase;    // 08 Lowest page aligned addr. of stack
    WORD    pvTDB;              // 0C Ptr to win-16 task database
    WORD    pvThunksSS;         // 0E SS selector used for thunking to 16 bits
    DWORD   SelmanList;         // 10 Pointer to selector manager list
    PVOID   pvArbitrary;        // 14 Available for application use
    struct _TIB95 *pTIBSelf;    // 18 Linear address of TIB structure
    WORD    TIBFlags;           // 1C TIBF_WIN32 = 1, TIBF_TRAP = 2
    WORD    Win16MutexCount;    // 1E Win16Lock
    DWORD   DebugContext;       // 20 Pointer to debug context structure
    DWORD   pCurrentPriority;   // 24 Pointer to DWORD containing current priority level
    DWORD   pvQueue;            // 28 Message Queue selector
    PVOID*  pvTLSArray;         // 2C Thread Local Storage (TLS) array
    PVOID*  pProcess;           // 30 Pointer to owning process database (PDB)
} TIB95, *PTIB95;

// Thread Database Extension
typedef struct _TDBX95 {
    DWORD  ptdb;              // 00 TDB
    DWORD  ppdb;              // 04 PDB
    DWORD  ContextHandle;     // 08 R0 memory context
    DWORD  un1;               // 0C
    DWORD  TimeOutHandle;     // 10
    DWORD  WakeParam;         // 14
    DWORD  BlockHandle;       // 18 R0 semaphore on which thread will wait inside VWIN32
    DWORD  BlockState;        // 1C
    DWORD  SuspendCount;      // 20 Number of times SuspendThread() was called
    DWORD  SuspendHandle;     // 24
    DWORD  MustCompleteCount; // 28 Count of EnterMustComplete's minus LeaveMustComplete's
    DWORD  WaitExFlags;       // 2C Flags
    DWORD  SyncWaitCount;     // 30
    DWORD  QueuedSyncFuncs;   // 34
    DWORD  UserAPCList;       // 38
    DWORD  KernAPCList;       // 3C
    DWORD  pPMPSPSelector;    // 40 Pointer to protected mode PSP selector
    DWORD  BlockedOnID;       // 44
    DWORD  un2[7];            // 48
    DWORD  TraceRefData;      // 64
    DWORD  TraceCallBack;     // 68
    DWORD  TraceEventHandle;  // 6C
    WORD   TraceOutLastCS;    // 70
    WORD   K16TDB;            // 72 Win16 TDB selector
    WORD   K16PDB;            // 74 Win16 PSP selector
    WORD   DosPDBSeg;         // 76 Real mode segment value of PSP
    WORD   ExceptionCount;    // 78
} TDBX95, *PTDBX95;

/////////////////////// Windows 98 /////////////////////////

// Kernel32 objects (WIN98)
#define WIN98_K32OBJ_SEMAPHORE            0x1
#define WIN98_K32OBJ_EVENT                0x2
#define WIN98_K32OBJ_MUTEX                0x3
#define WIN98_K32OBJ_CRITICAL_SECTION     0x4
#define WIN98_K32OBJ_CHANGE               0x5
#define WIN98_K32OBJ_PROCESS              0x6
#define WIN98_K32OBJ_THREAD               0x7
#define WIN98_K32OBJ_FILE                 0x8
#define WIN98_K32OBJ_CONSOLE              0x9
#define WIN98_K32OBJ_SCREEN_BUFFER        0xA
#define WIN98_K32OBJ_MAILSLOT             0xB
#define WIN98_K32OBJ_SERIAL               0xC
#define WIN98_K32OBJ_MEM_MAPPED_FILE      0xD
#define WIN98_K32OBJ_PIPE                 0xE
#define WIN98_K32OBJ_DEVICE_IOCTL         0xF
#define WIN98_K32OBJ_TOOLHELP_SNAPSHOT    0x10
#define WIN98_K32OBJ_SOCKET               0x11

typedef struct _TDBX98 TDBX98;
typedef struct _PDB98  PDB98;

// Thread Information Block (FS:[0x18])
typedef struct _TIB98 {        // Size = 0x38
    PSEH    pvExcept;          // 00 Head of exception record list
    PVOID   pvStackUserTop;    // 04 Top of user stack
    PVOID   pvStackUserBase;   // 08 Base of user stack
    WORD    pvTDB;             // 0C Ptr to win-16 task database
    WORD    pvThunksSS;        // 0E SS selector used for thunking to 16 bits
    DWORD   SelmanList;        // 10 Pointer to selector manager list
    PVOID   pvArbitrary;       // 14 Available for application use
    struct _TIB98 *pTIBSelf;   // 18 Linear address of TIB structure
    WORD    TIBFlags;          // 1C TIBF_WIN32 = 1, TIBF_TRAP = 2
    WORD    Win16MutexCount;   // 1E Win16Lock
    DWORD   DebugContext;      // 20 Pointer to debug context structure
    DWORD   pCurrentPriority;  // 24 Pointer to DWORD containing current priority level
    DWORD   pvQueue;           // 28 Message Queue selector
    PVOID   *pvTLSArray;       // 2C Thread Local Storage (TLS) array
    PVOID   *pProcess;         // 30 Pointer to owning process database (PDB)
    DWORD   Unknown;           // 34 Pointer to ???
} TIB98, *PTIB98;

// Thread database (FS:[0x18] - 0x8)
typedef struct _TDB98 {        // Size = 0x228 (from Kernel32)
    WORD    Type;              // 00 K32 object type
    WORD    cReference;        // 02 Reference count
    DWORD   pSomeEvent;        // 04 K32 event object used when someone waits on the thread object
    TIB98   tib;               // 08 Thread information block (TIB)
    DWORD   Unknown;           // 40
    DWORD   Flags;             // 44 Flags
    DWORD   TerminationStatus; // 48 Exit code
    WORD    TIBSelector;       // 4C Selector used in FS to point to TIB
    WORD    EmulatorSelector;  // 4E Memory block for saving x87 state
    DWORD   cHandles;          // 50 Handle count
    DWORD   Ring0Thread;       // 54 R0 thread control block (TCB)
    TDBX98  *pTDBX;            // 58 R0 thread database extension (TDBX)
    DWORD   un1[109];          // 5C
    DWORD   APISuspendCount;   // 210 Count of SuspendThread's minus ResumeThread's
} TDB98, *PTDB98;

// Thread database extension
typedef struct _TDBX98 {
    DWORD  un0;                // 00
    TDB98  *ptdb;              // 04 R3 thread database
    PDB98  *ppdb;              // 08 R3 process database
    DWORD  ContextHandle;      // 0C R0 memory context
    DWORD  Ring0Thread;        // 10 R0 thread control block [TCB *]
    DWORD  WaitNodeList;       // 14 Anchor of things we're waiting on  [WAITNODE *]
    DWORD  WaitFlags;          // 18 Blocking flags
    DWORD  un1;                // 1C
    DWORD  TimeOutHandle;      // 20
    DWORD  WakeParam;          // 24
    DWORD  BlockHandle;        // 28 R0 semaphore on which thread will wait inside VWIN32
    DWORD  BlockState;         // 2C
    DWORD  SuspendCount;       // 30
    DWORD  SuspendHandle;      // 34
    DWORD  MustCompleteCount;  // 38 Count of EnterMustComplete's minus LeaveMustComplete's
    DWORD  WaitExFlags;        // 3C Flags
    DWORD  SyncWaitCount;      // 40
    DWORD  QueuedSyncFuncs;    // 44
    DWORD  UserAPCList;        // 48
    DWORD  KernAPCList;        // 4C
    DWORD  pPMPSPSelector;     // 50
    DWORD  BlockedOnID;        // 54
} TDBX98, *PTDBX98;

/////////////////////// Windows Me /////////////////////////

typedef TIB98   TIBME;
typedef TIBME   *PTIBME;

typedef struct _TDBME {        // Size = 0x228 (from Kernel32)
    WORD    Type;              // 00 K32 object type
    WORD    cReference;        // 02 Reference count
    DWORD   pSomeEvent;        // 04 K32 event object used when someone waits on the thread object
    TIB98   tib;               // 08 Thread information block (TIB)
    DWORD   Unknown;           // 40
    DWORD   Unknown2;          // 44
    WORD    TIBSelector;       // 46 Selector used in FS to point to TIB
    DWORD   TerminationStatus; // 48 Exit code
    DWORD   Flags;             // 4C Flags
    DWORD   cHandles;          // 50 Handle count
    DWORD   Ring0Thread;       // 54 R0 thread control block (TCB)
    DWORD   Unknown3;          // 58 Selector for ???
    DWORD   un1[109];          // 5C
    DWORD   APISuspendCount;   // 210 Count of SuspendThread's minus ResumeThread's
} TDBME, *PTDBME;

#pragma pack()

#endif // __STRUCT_H__
