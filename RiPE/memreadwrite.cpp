#include "stdafx.h"
#include "memreadwrite.h"

const int OFFSET_END = 0xDEADBEEF;

inline BOOL IsPageReadable(PMEMORY_BASIC_INFORMATION mbi)
{
    BOOL bRet = FALSE;

    if (mbi->Protect & (PAGE_EXECUTE_READ      | 
                        PAGE_EXECUTE_READWRITE |
                        PAGE_EXECUTE_WRITECOPY | 
                        PAGE_READONLY          |
                        PAGE_READWRITE         | 
                        PAGE_WRITECOPY) )
        bRet = TRUE;

    return bRet;
}

// Arguments should be passed as destination, base1, base2, etc.
// example:  To read [[[[0x00400000]+10]]+10], you would put:
// ReadPointer( &someDestination, (LPCVOID)0x00400000, 10, 0, 10);
BOOL _ReadPointer(PULONG_PTR pulValue, LPCVOID lpcvBase, ...)
{
    MEMORY_BASIC_INFORMATION  mbi;
    ULONG_PTR  ulTemp;
    va_list    pArguments;
    int        iOffset;

    ulTemp = (ULONG_PTR)lpcvBase;
    va_start(pArguments, lpcvBase);

    // return false if we're NULL
    if( ulTemp == 0 )
        return FALSE;

    while ((iOffset = va_arg(pArguments, int)) != OFFSET_END)
    {
        // first check the base
        if (!VirtualQuery((LPCVOID)ulTemp, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
            return FALSE;
        if (!IsPageReadable(&mbi))
            return FALSE;
        // if outside of page
        if( (unsigned long)mbi.BaseAddress + mbi.RegionSize < ulTemp + sizeof(ULONG_PTR) )
            return FALSE;


        // now check the offset
        if (!VirtualQuery( (LPCVOID)(*(ULONG_PTR *)ulTemp + iOffset), &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
            return FALSE;
        if (!IsPageReadable(&mbi))
            return FALSE;
        // if outside of page
        if( (unsigned long)mbi.BaseAddress + mbi.RegionSize < ulTemp + sizeof(ULONG_PTR) )
            return FALSE;

        ulTemp = *(ULONG_PTR *)((*(ULONG_PTR *)ulTemp) + iOffset);
    }

    va_end(pArguments);
    if( !VirtualQuery((LPCVOID)ulTemp, &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
        return FALSE;

    // if page is readable
    if( !IsPageReadable(&mbi) )
        return FALSE;

    // if outside of page
    if( (unsigned long)mbi.BaseAddress + mbi.RegionSize < ulTemp + sizeof(ULONG_PTR) )
        return FALSE;

    *pulValue = *(ULONG_PTR *)ulTemp;

    return TRUE;
}

// convenience function which returns the data instead of TRUE/FALSE
// Given an address and offsets for dereference (optional), return
// the value stored at that address
DWORD _ReadAddress( LPCVOID address, ... )
{
    DWORD result = 0;

    // Try reading 4, then 3, then 2, then 1 bytes
    // This way we can get a char at the very end of a page :)
    for( int i=0; i < 4; ++i )
    {
        BOOL succeeded = FALSE;
        succeeded = ReadPointer( &result, (LPCVOID)((DWORD)address-i), __VA_ARGS__ );
        result >>= i*8; // Shift the value an appropriate number of places to the right
        if( succeeded )
            break; // We're done!  Return the result!
    }
    return result;
}

bool WriteAddress(__in LPVOID lpcvBase, __in LPCVOID lpcvWriteValue, __in size_t uSize)
{
    DWORD old_protection = 0;

    __try 
    {  
        if( VirtualProtect(lpcvBase, uSize, PAGE_READWRITE, &old_protection) ) 
        {
            memcpy_s( lpcvBase, uSize, lpcvWriteValue, uSize );
            VirtualProtect( lpcvBase, uSize, old_protection, &old_protection );
        }
        else
            return false;
    }  
    __except( EXCEPTION_EXECUTE_HANDLER )
    {      
        return false;
    }
    return true;
}
