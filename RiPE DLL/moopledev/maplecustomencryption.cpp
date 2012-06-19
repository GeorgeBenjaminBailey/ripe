#include "../defs.h"

#include <vector>

#include <WinDef.h>

#include "maplecustomencryption.h"

const unsigned char AesKeySize = 32;
const unsigned char AesKey[AesKeySize] = {
    0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00,
    0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00 };

unsigned char MapleCustomEncryption::rol(unsigned char val, int num)
{
    int highbit;
    for (int i = 0; i < num; ++i)
    {
        highbit = ((val & 0x80) ? 1 : 0);
        val <<= 1;
        val |= highbit;
    }
    return val;
}

unsigned char MapleCustomEncryption::ror(unsigned char val, int num)
{
    int lowbit;
    for (int i = 0; i < num; ++i)
    {
        lowbit = ((val & 1) ? 1 : 0);
        val >>= 1; 
        val |= (lowbit << 7);
    }
    return val;
}

void MapleCustomEncryption::mapleEncrypt(unsigned char *buf, int size) {
    int j;
    unsigned char a, c;
    for (unsigned char i = 0; i < 3; i++) {
        a = 0;
        for (j = size; j > 0; j--) {
            c = buf[size - j];
            c = rol(c, 3);
            c = (unsigned char)(c + j); // Guess this is supposed to be right?
            c = c ^ a;
            a = c;
            c = ror(a, j);
            c = c ^ 0xFF;
            c = c + 0x48;
            buf[size - j] = c;
        }
        a = 0;
        for (j = size; j > 0; j--) {
            c = buf[j - 1];
            c = rol(c, 4);
            c = (unsigned char)(c + j); // Guess this is supposed to be right?
            c = c ^ a;
            a = c;
            c = c ^ 0x13;
            c = ror(c, 3);
            buf[j - 1] = c;
        }
    }
}

void MapleCustomEncryption::mapleDecrypt(unsigned char *buf, int size) {
    int j;
    unsigned char a, b, c;
    for (unsigned char i = 0; i < 3; i++) {
        a = 0;
        b = 0;
        for (j = size; j > 0; j--) {
            c = buf[j - 1];
            c = rol(c, 3);
            c = c ^ 0x13;
            a = c;
            c = c ^ b;
            c = (unsigned char)(c - j); // Guess this is supposed to be right?
            c = ror(c, 4);
            b = a;
            buf[j - 1] = c;
        }
        a = 0;
        b = 0;
        for (j = size; j > 0; j--) {
            c = buf[size - j];
            c = c - 0x48;
            c = c ^ 0xFF;
            c = rol(c, j);
            a = c;
            c = c ^ b;
            c = (unsigned char)(c - j); // Guess this is supposed to be right?
            c = ror(c, 3);
            b = a;
            buf[size - j] = c;
        }
    }
}


namespace
{
    const unsigned char shuffle[256] = {
        0xEC, 0x3F, 0x77, 0xA4, 0x45, 0xD0, 0x71, 0xBF, 0xB7, 0x98, 0x20, 0xFC, 0x4B, 
        0xE9, 0xB3, 0xE1, 0x5C, 0x22, 0xF7, 0x0C, 0x44, 0x1B, 0x81, 0xBD, 0x63, 0x8D, 
        0xD4, 0xC3, 0xF2, 0x10, 0x19, 0xE0, 0xFB, 0xA1, 0x6E, 0x66, 0xEA, 0xAE, 0xD6, 
        0xCE, 0x06, 0x18, 0x4E, 0xEB, 0x78, 0x95, 0xDB, 0xBA, 0xB6, 0x42, 0x7A, 0x2A, 
        0x83, 0x0B, 0x54, 0x67, 0x6D, 0xE8, 0x65, 0xE7, 0x2F, 0x07, 0xF3, 0xAA, 0x27, 
        0x7B, 0x85, 0xB0, 0x26, 0xFD, 0x8B, 0xA9, 0xFA, 0xBE, 0xA8, 0xD7, 0xCB, 0xCC, 
        0x92, 0xDA, 0xF9, 0x93, 0x60, 0x2D, 0xDD, 0xD2, 0xA2, 0x9B, 0x39, 0x5F, 0x82, 
        0x21, 0x4C, 0x69, 0xF8, 0x31, 0x87, 0xEE, 0x8E, 0xAD, 0x8C, 0x6A, 0xBC, 0xB5, 
        0x6B, 0x59, 0x13, 0xF1, 0x04, 0x00, 0xF6, 0x5A, 0x35, 0x79, 0x48, 0x8F, 0x15, 
        0xCD, 0x97, 0x57, 0x12, 0x3E, 0x37, 0xFF, 0x9D, 0x4F, 0x51, 0xF5, 0xA3, 0x70, 
        0xBB, 0x14, 0x75, 0xC2, 0xB8, 0x72, 0xC0, 0xED, 0x7D, 0x68, 0xC9, 0x2E, 0x0D, 
        0x62, 0x46, 0x17, 0x11, 0x4D, 0x6C, 0xC4, 0x7E, 0x53, 0xC1, 0x25, 0xC7, 0x9A, 
        0x1C, 0x88, 0x58, 0x2C, 0x89, 0xDC, 0x02, 0x64, 0x40, 0x01, 0x5D, 0x38, 0xA5, 
        0xE2, 0xAF, 0x55, 0xD5, 0xEF, 0x1A, 0x7C, 0xA7, 0x5B, 0xA6, 0x6F, 0x86, 0x9F, 
        0x73, 0xE6, 0x0A, 0xDE, 0x2B, 0x99, 0x4A, 0x47, 0x9C, 0xDF, 0x09, 0x76, 0x9E, 
        0x30, 0x0E, 0xE4, 0xB2, 0x94, 0xA0, 0x3B, 0x34, 0x1D, 0x28, 0x0F, 0x36, 0xE3, 
        0x23, 0xB4, 0x03, 0xD8, 0x90, 0xC8, 0x3C, 0xFE, 0x5E, 0x32, 0x24, 0x50, 0x1F, 
        0x3A, 0x43, 0x8A, 0x96, 0x41, 0x74, 0xAC, 0x52, 0x33, 0xF0, 0xD9, 0x29, 0x80, 
        0xB1, 0x16, 0xD3, 0xAB, 0x91, 0xB9, 0x84, 0x7F, 0x61, 0x1E, 0xCF, 0xC5, 0xD1, 
        0x56, 0x3D, 0xCA, 0xF4, 0x05, 0xC6, 0xE5, 0x08, 0x49 };
}

unsigned long __declspec(naked) __cdecl innoHash( unsigned char *pSrc, 
    int nLen, unsigned long *pdwKey )
{
    __asm
    {
        push ecx
            mov eax,[esp+0x10]
        mov dword ptr [esp],0xC65053F2
            test eax,eax
            jne NotZero
            lea eax,[esp]
NotZero:
        push ebp
            mov  ebp,[esp+0x10]
        push esi
            xor  esi,esi
            test ebp,ebp
            jle  ZeroLen
            push ebx
            push edi
ForLoop:
        mov ecx,[esp+0x18]
        mov dl,[esi+ecx]
        mov cl,[eax+1]
        movzx edi,cl
            lea   ebx,shuffle
            add   ebx,edi
            movzx ebx,[ebx]
        sub   bl,dl
            add   [eax],bl
            movzx edi,dl
            lea   ebx,shuffle
            add   ebx,edi
            movzx ebx,[ebx]
        mov   [esp+0x20],dl
            mov   dl,[eax+2]
        xor   bl,dl
            sub   cl,bl
            mov   [eax+1],cl
            mov   cl,[eax+3]
        movzx edx,cl
            push  eax
            lea   eax,shuffle
            add   edx,eax
            pop   eax
            movzx edx,[edx]
        add   dl,[esp+0x20]
        sub   cl,[eax]
        xor   dl,[eax+2]
        inc   esi
            mov   [eax+2],dl
            push  eax
            lea   eax,shuffle
            add   cl,[eax+edi]
        pop   eax
            mov   [eax+3],cl
            mov   ecx,[eax]
        rol   ecx,3
            cmp   esi,ebp
            mov   [eax],ecx
            jnge  ForLoop
            pop   edi
            pop   ebx
            pop   esi
            mov   eax,ecx
            pop   ebp
            pop   ecx
            ret
ZeroLen:
        mov   eax,[eax]
        pop   esi
            pop   ebp
            pop   ecx
    }
}