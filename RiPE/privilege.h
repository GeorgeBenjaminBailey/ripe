#ifndef __PRIVILEGE_H
#define __PRIVILEGE_H

#include <windows.h>
#include <stdio.h>
//#pragma comment(lib, "cmcfg32.lib")

#include "lasterror.h"

BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if ( !LookupPrivilegeValue( 
        NULL,            // lookup privilege on local system
        lpszPrivilege,   // privilege to lookup 
        &luid ) )        // receives LUID of privilege
    {
        PrintLastError( L"LookupPrivilegeValue" );
        return FALSE; 
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.

    if ( !AdjustTokenPrivileges(
        hToken, 
        FALSE, 
        &tp, 
        sizeof(TOKEN_PRIVILEGES), 
        (PTOKEN_PRIVILEGES) NULL, 
        (PDWORD) NULL) )
    {
        PrintLastError( L"AdjustTokenPrivileges" );
        return FALSE; 
    }

    return TRUE;
}


#endif //__PRIVILEGE_H