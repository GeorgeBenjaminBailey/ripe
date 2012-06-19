#ifndef __MILLI_TIME_H
#define __MILLI_TIME_H

#include <sys/timeb.h>
#include <time.h>

int GetMilliCount();

int GetMilliSpan( int nTimeStart, int nTimeEnd );

int GetMilliSpan( int nTimeStart );

#endif //__MILLI_TIME_H
