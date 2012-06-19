#include "defs.h"

#include "ripecontainer.h"

#include "breakpoint.h"

namespace RiPE {

HackDLL *Container::HackDLLPtr = NULL;
Breakpoint *Container::breakpointPtr = NULL;

HackDLL * Container::GetHackDLL() {
    return HackDLLPtr;
}

void Container::SetHackDLL(HackDLL *hackDll) {
    HackDLLPtr = hackDll;
}

Breakpoint * Container::GetBreakpoint() {
    if (breakpointPtr == NULL) {
        breakpointPtr = new Breakpoint();
    }
    return breakpointPtr;
}

}
