#pragma once
#include "Wow64MemoryReaderWriter.h"
#include "HookGenerator.h"
#include "DianaHook/DianaHook.h"

namespace Wow64Hooker
{
    struct HookContext
    {
        HookContext(DWORD64 funcToHookAddr, ShellcodeHandler64 hookHandler64);

        DWORD64 funcToHookAddr;
        ShellcodeHandler64 hookHandler64;
    };

    class Hooker
    {
    public:
        explicit Hooker(const HookHandler32* onAlertCb);
        void applyHooks();

    private:
        template<typename Type>
        HookContext generateHookContext(DWORD64 moduleAddr, char* funcName, Type addr)
        {
            DWORD64 funcToHookAddr = getFunctionAddress64(moduleAddr, funcName);

            ShellcodeHandler64 hookHandler64 = m_hookGenerator.generateShellcodeHandler64(addr);
            if (!hookHandler64)
            {
                throw std::runtime_error("Can't generate hook handler");
            }

            return HookContext(funcToHookAddr, hookHandler64);
        }

    private:
        void changeRWEProtection(DWORD64 addr, bool needSetRWE);
        DWORD64 getFunctionAddress64(DWORD64 moduleAddr, char* funcName);

        static int hook_Alloc(void* pThis,
            OPERAND_SIZE size,
            OPERAND_SIZE* pAddress,
            const OPERAND_SIZE* pHintAddress,
            int flags);

        static void hook_Free(void* pThis, const OPERAND_SIZE* pAddress);

    private:
        DianaHook::DianaHookPatcher m_dianaHook;
        Wow64MemoryReaderWriter m_wow64ReaderWriter;
        ShellCode64Generator m_hookGenerator;

        std::vector<HookContext> m_hookContexts;
        DWORD m_oldProtect = 0;
    };
}
