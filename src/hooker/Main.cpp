#include "StdAfx.h"
#include "DianaHook.h"
#include "Wow64MemoryReaderWriter.h"
#include "Hooker.h"

using namespace DianaHook;
using namespace Wow64Hooker;

namespace
{
    DWORD g_counter = 0;

    void __stdcall hookHandler32()
    {
        ++g_counter;
    }
}

int main()
{
    try
    {
        Hooker hooker(hookHandler32);
        hooker.applyHooks();

        auto memory = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READONLY);
        assert(nullptr != memory);

        DWORD oldProtect = 0;
        assert(VirtualProtect(memory, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect));
        assert(VirtualProtect(memory, 0x1000, oldProtect, &oldProtect));

        std::cout << "NtProtectVirtualMemory hook was called - " << g_counter << " times" << std::endl;
    }
    catch (const std::exception& exc)
    {
        __asm int 3;
        std::cout << "Exception: " << exc.what() << std::endl;
    }
    
    __asm int 3;
    return 0;
}
