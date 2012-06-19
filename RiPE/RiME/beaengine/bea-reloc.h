#ifndef BEA_ENGINE_RELOC
#define BEA_ENGINE_RELOC
typedef int (__stdcall *DisasmFunc)(void*);

#pragma code_seg(".text")
__declspec(allocate(".text")) unsigned char __bea_engine__[];
#pragma code_seg()

extern unsigned char __bea_engine__[];
extern DisasmFunc _Disasm;

#endif
