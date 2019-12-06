#pragma once
#include "Wow64MemoryReaderWriter.h"
#include "HookGenerator.h"
#include "DianaHook.h"

namespace Wow64Hooker
{
    struct HookContext
    {
        HookContext(DWORD64 funcToHookAddr, HookHandler64 hookHandler64)
            : funcToHookAddr(funcToHookAddr)
            , hookHandler64(hookHandler64)
        {
        }

        DWORD64 funcToHookAddr = 0;
        HookHandler64 hookHandler64 = nullptr;
    };

    class Hooker
    {
    public:
        Hooker(HookHandler32 onAlertCb);
        void applyHooks();

    private:
        void changeRWEProtection(DWORD64 addr, bool needSetRWE);

        HookContext generateHookContextForNtProtect(DWORD64 ntdllAddr);
        HookContext generateHookContextForNtAllocate(DWORD64 ntdllAddr);

        static int hook_Alloc(void* pThis,
            OPERAND_SIZE size,
            OPERAND_SIZE* pAddress,
            const OPERAND_SIZE* pHintAddress,
            int flags);

        static void hook_Free(void* pThis, const OPERAND_SIZE* pAddress);

    private:
        std::vector<HookContext> m_hookContexts;
        HookHandler32 m_hookHandler32 = nullptr;
        DWORD m_oldProtect = 0;

        HookGenerator m_hookGenerator;

        Wow64MemoryReaderWriter m_wow64ReaderWriter;
        DianaHook::DianaHookPatcher m_dianaHook;
    };
}
