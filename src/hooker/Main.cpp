#include "StdAfx.h"
#include "DianaHook/DianaHook.h"
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

    HookHandler32 g_hookHandler32 = hookHandler32;
}

int main()
{
    try
    {
        Hooker hooker(&g_hookHandler32);
        hooker.applyHooks();

        auto memoryAddr = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
        if (!memoryAddr)
        {
            std::cout << "Can not allocate memory for testing" << std::endl;
            return -1;
        }

        const char* str = "teststring";
        const size_t str_length = 11;
        SIZE_T lpNumberOfBytesWritten = 0;

        //
        // call WinAPI WriteProcessMemory and ReadProcessMemory functions to check how hooks work
        //

        BOOL result = WriteProcessMemory(GetCurrentProcess(), memoryAddr, str, str_length, &lpNumberOfBytesWritten);
        if (!result || lpNumberOfBytesWritten != str_length)
        {
            std::cout << "Cannot write process memory" << std::endl;
            return -1;
        }

        char buffer[str_length] = { 0 };
        SIZE_T lpNumberOfBytesRead = 0;

        result = ReadProcessMemory(GetCurrentProcess(), memoryAddr, buffer, sizeof(buffer), &lpNumberOfBytesRead);
        if (!result || lpNumberOfBytesRead != sizeof(buffer) || 0 != strcmp(str, buffer))
        {
            std::cout << "Cannot read process memory" << std::endl;
            return -1;
        }
        
        std::cout << "NtReadVirtualMemory hook was called - " << g_counter << " times" << std::endl;

        VirtualFree(memoryAddr, 0, MEM_RELEASE);
    }
    catch (const std::exception& exc)
    {
        std::cout << "Exception: " << exc.what() << std::endl;
    }
    
    return 0;
}
