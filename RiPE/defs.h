/* This file should be included in most, if not all, header files. */

#ifndef __RIU_DEFS_H
#define __RIU_DEFS_H

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS 1
#endif

#define _str(func) #func

#define RIPE_DLL wxT("ripe_dll")

// The name of the error message log to create in the event of an error
#define ERROR_MESSAGE_LOG wxT("error_log.txt")

//#define PRIVATE_RIPE // This private version is for personal use only.
#define VIP_RIPE
#define USING_PACKET_EDITOR

#define INVALID_ADDRESS (::DWORD)( -1 ) //Invalid mem address

#define PACKETTYPE_BLOCK     wxT("Block")
#define PACKETTYPE_IGNORE    wxT("Ignore")
#define PACKETTYPE_MODIFY    wxT("Modify")
#define PACKETTYPE_INTERCEPT wxT("Ntercept")

// ASM DEFINITIONS
#define JMP(frm,to) (((int)(to) - (int)(frm))-5)
#define _CALL       0xE8
#define _NOP        0x90
#define SHORT_JUMP  0xE9

#endif
