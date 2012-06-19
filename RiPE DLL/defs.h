/* This file should be included in most, if not all, header files. */

#ifndef __RIU_DEFS_H
#define __RIU_DEFS_H

#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS 1
#endif
#ifdef __WXDEBUG__
    #undef __WXDEBUG__
#endif

#include <Winsock2.h>
#include <windows.h>

// Convert a function name to a string
#define _str(func) #func

//#define PRIVATE_RIPE // This private version is for personal use only.
#define VIP_RIPE

// The name of the error message log to create in the event of an error
#define ERROR_MESSAGE_LOG std::string("error_log_ripe.txt")

#define PACKETTYPE_BLOCK     std::string("Block")
#define PACKETTYPE_IGNORE    std::string("Ignore")
#define PACKETTYPE_MODIFY    std::string("Modify")
#define PACKETTYPE_INTERCEPT std::string("Ntercept")

// ASM DEFINITIONS
#define JMP(frm,to) (((int)(to) - (int)(frm))-5)
#define _CALL        0xE8
#define _MOV_EBP_ESP 0x8B, 0xEC
#define _MOV_EDI_EDI 0x8B, 0xFF
#define _NOP         0x90
#define _PUSH_EBP    0x55
#define SHORT_JUMP   0xE9

#endif // __RIU_DEFS_H
