#include "stdafx.h"

#include "millitime.h"

int GetMilliCount() {
    // Something like GetTickCount but portable
    // It rolls over every ~ 12.1 days (0x100000/24/60/60)
    // Use GetMilliSpan to correct for rollover
    timeb tb;
    ftime( &tb );
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

int GetMilliSpan( int nTimeStart, int nTimeEnd )
{
    int nSpan = nTimeEnd - nTimeStart;
    if ( nSpan < 0 )
        nSpan += 0x100000 * 1000;
    return nSpan;
}

int GetMilliSpan( int nTimeStart )
{
    return GetMilliSpan( nTimeStart, GetMilliCount() );
}
