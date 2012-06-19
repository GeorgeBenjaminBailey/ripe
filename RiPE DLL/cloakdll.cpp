#include "cloakdll.h"

// #pragma comment(lib, "advapi32.lib")
// #pragma comment(lib, "shlwapi.lib")
// #include<windows.h>
// #include <stdio.h>
// #include <tlhelp32.h>
// #include <shlwapi.h>

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _LDR_MODULE
{
    LIST_ENTRY              InLoadOrderModuleList;
    LIST_ENTRY              InMemoryOrderModuleList;
    LIST_ENTRY              InInitializationOrderModuleList;
    PVOID                   BaseAddress;
    PVOID                   EntryPoint;
    ULONG                   SizeOfImage;
    UNICODE_STRING          FullDllName;
    UNICODE_STRING          BaseDllName;
    ULONG                   Flags;
    SHORT                   LoadCount;
    SHORT                   TlsIndex;
    LIST_ENTRY              HashTableEntry;
    ULONG                   TimeDateStamp;
} LDR_MODULE , *PLDR_MODULE;

typedef struct _PEB_LDR_DATA
{
    ULONG               Length;
    BOOLEAN             Initialized;
    PVOID               SsHandle;
    LIST_ENTRY          InLoadOrderModuleList; 
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;


typedef struct _PEB
{
    BOOLEAN                      InheritedAddressSpace; 
    BOOLEAN                      ReadImageFileExecOptions;
    BOOLEAN                      BeingDebugged;           
    BOOLEAN                      SpareBool;               
    HANDLE                       Mutant;                   
    HMODULE                      ImageBaseAddress;         
    PPEB_LDR_DATA                LdrData;                  
    int *ProcessParameters;                
    PVOID                        SubSystemData;                
    HANDLE                       ProcessHeap;                  
    PRTL_CRITICAL_SECTION        FastPebLock;                  
    PVOID                        FastPebLockRoutine;           
    PVOID                        FastPebUnlockRoutine;         
    ULONG                        EnvironmentUpdateCount;       
    PVOID                        KernelCallbackTable;          
    PVOID                        EventLogSection;              
    PVOID                        EventLog;                     
    PVOID                        FreeList;                     
    ULONG                        TlsExpansionCounter;          
    int                          TlsBitmap;                      
    ULONG                        TlsBitmapBits[2];           
    PVOID                        ReadOnlySharedMemoryBase;   
    PVOID                        ReadOnlySharedMemoryHeap;   
    PVOID                       *ReadOnlyStaticServerData;   
    PVOID                        AnsiCodePageData;           
    PVOID                        OemCodePageData;            
    PVOID                        UnicodeCaseTableData;       
    ULONG                        NumberOfProcessors;         
    ULONG                        NtGlobalFlag;               
    BYTE                         Spare2[4];                  
    LARGE_INTEGER                CriticalSectionTimeout;     
    ULONG                        HeapSegmentReserve;         
    ULONG                        HeapSegmentCommit;              
    ULONG                        HeapDeCommitTotalFreeThreshold; 
    ULONG                        HeapDeCommitFreeBlockThreshold;  
    ULONG                        NumberOfHeaps;                   
    ULONG                        MaximumNumberOfHeaps;            
    PVOID                       *ProcessHeaps;                    
    PVOID                        GdiSharedHandleTable;            
    PVOID                        ProcessStarterHelper;            
    PVOID                        GdiDCAttributeList;              
    PVOID                        LoaderLock;                      
    ULONG                        OSMajorVersion;                  
    ULONG                        OSMinorVersion;                  
    ULONG                        OSBuildNumber;                   
    ULONG                        OSPlatformId;                    
    ULONG                        ImageSubSystem;                  
    ULONG                        ImageSubSystemMajorVersion;      
    ULONG                        ImageSubSystemMinorVersion;      
    ULONG                        ImageProcessAffinityMask;        
    ULONG                        GdiHandleBuffer[34];             
    ULONG                        PostProcessInitRoutine;          
    ULONG                        TlsExpansionBitmap;              
    ULONG                        TlsExpansionBitmapBits[32];      
    ULONG                        SessionId;                       
} PEB, *PPEB;


void CloakDll(HMODULE hModule)
{
    DWORD dwPEB_LDR_DATA = 0;

    __asm
    {
        pushad
        pushfd
        mov eax, fs:[0x30]		   // PEB
        mov eax, [eax+0xC]		  // PEB->ProcessModuleInfo
        mov dwPEB_LDR_DATA, eax	 // Save ProcessModuleInfo
//InLoadOrderModuleList:
        mov esi, [eax+0Ch]					  // ProcessModuleInfo->InLoadOrderModuleList[FORWARD]
        mov edx, [eax+10h]					  //  ProcessModuleInfo->InLoadOrderModuleList[BACKWARD]
LoopInLoadOrderModuleList:
        lodsd							   //  Load First Module
        mov esi, eax		    			//  ESI points to Next Module
        mov ecx, [eax+0x18]		    		//  LDR_MODULE->BaseAddress
        cmp ecx, hModule		    		//  Is it Our Module ?
        jne SkipA		    		    	//  If Not, Next Please (@f jumps to nearest Unamed Lable :)
        mov ebx, [eax]				  //  [FORWARD] Module 
        mov ecx, [eax+0x4]    		    	//  [BACKWARD] Module
        mov [ecx], ebx				  //  Previous Module's [FORWARD] Notation, Points to us, Replace it with, Module++
        mov [ebx+0x4], ecx			    //  Next Modules, [BACKWARD] Notation, Points to us, Replace it with, Module--
        jmp InMemoryOrderModuleList		//  Hidden, so Move onto Next Set
SkipA:
        cmp edx, esi					    //  Reached End of Modules ?
        jne LoopInLoadOrderModuleList		//  If Not, Re Loop
InMemoryOrderModuleList:
        mov eax, dwPEB_LDR_DATA		  //  PEB->ProcessModuleInfo
        mov esi, [eax+0x14]			   //  ProcessModuleInfo->InMemoryOrderModuleList[START]
        mov edx, [eax+0x18]			   //  ProcessModuleInfo->InMemoryOrderModuleList[FINISH]
LoopInMemoryOrderModuleList:
        lodsd
        mov esi, eax
        mov ecx, [eax+0x10]
        cmp ecx, hModule
        jne SkipB
        mov ebx, [eax]
        mov ecx, [eax+0x4]
        mov [ecx], ebx
        mov [ebx+0x4], ecx
        jmp InInitializationOrderModuleList
SkipB:
        cmp edx, esi
        jne LoopInMemoryOrderModuleList
InInitializationOrderModuleList:
        mov eax, dwPEB_LDR_DATA				    //  PEB->ProcessModuleInfo
        mov esi, [eax+0x1C]						 //  ProcessModuleInfo->InInitializationOrderModuleList[START]
        mov edx, [eax+0x20]						 //  ProcessModuleInfo->InInitializationOrderModuleList[FINISH]

LoopInInitializationOrderModuleList:
        lodsd
        mov esi, eax		
        mov ecx, [eax+0x8]
        cmp ecx, hModule		
        jne SkipC

        mov ebx, [eax]
        mov ecx, [eax+4]
        //mov [ecx], ebx // Don't remove our node from here, because we want PROCESS_DETACH events
        //mov [ebx+4], ecx
        jmp Finished
SkipC:
        cmp edx, esi
        jne LoopInInitializationOrderModuleList
Finished:
        popfd
        popad
    }
}
/*
#define CUT_LIST(item) \
    item.Blink->Flink = item.Flink; \
    item.Flink->Blink = item.Blink

void CloakDll(HMODULE hModule)
{
    ULONG_PTR DllHandle = (ULONG_PTR)hModule;
    PPEB_LDR_DATA pebLdrData;
    PLDR_MODULE mod;

    __asm {
        mov eax, fs:[0x30]        //get PEB ADDR
        add eax, 0x0C        
        mov eax, [eax]            //get LoaderData ADDR
        mov pebLdrData, eax
    }

    for (
        mod = (PLDR_MODULE)pebLdrData->InLoadOrderModuleList.Flink;
        mod->BaseAddress != 0;
    mod = (PLDR_MODULE)mod->InLoadOrderModuleList.Flink
        ) {
            if ((HMODULE)mod->BaseAddress == hModule) {
                CUT_LIST(mod->InLoadOrderModuleList);
                CUT_LIST(mod->InInitializationOrderModuleList);
                CUT_LIST(mod->InMemoryOrderModuleList);

                //ZeroMemory(mod, sizeof(LDR_MODULE));
                return;
            }
    }
}
*/