#ifndef __RIPE_CONTAINER_H
#define __RIPE_CONTAINER_H

class Breakpoint;

namespace RiPE {
    class HackDLL;

    class Container {
    private:
        static HackDLL *HackDLLPtr;
        static Breakpoint *breakpointPtr;

    public:
        static HackDLL * GetHackDLL();

        static void SetHackDLL(HackDLL *hackDll);

        static Breakpoint * GetBreakpoint();
    };
}

#endif // __RIPE_CONTAINER_H
