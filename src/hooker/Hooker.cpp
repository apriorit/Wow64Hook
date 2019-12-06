#include "StdAfx.h"
#include "Hooker.h"

namespace Wow64Hooker
{
    Hooker::Hooker(HookHandler32 hookHandler32)
        : m_hookHandler32(hookHandler32)
        , m_dianaHook(m_wow64ReaderWriter, Hooker::hook_Alloc, Hooker::hook_Free)
    {
        DWORD64 ntdllAddr = GetModuleHandle64(L"ntdll.dll");
        m_hookContexts.push_back(generateHookContextForNtProtect(ntdllAddr));
    }

    void Hooker::applyHooks()
    {
        for (auto hookContext : m_hookContexts)
        {
            changeRWEProtection(hookContext.funcToHookAddr, true);

            m_dianaHook.addHook(DIANA_MODE64
                , hookContext.funcToHookAddr
                , static_cast<DI_UINT64>(reinterpret_cast<ULONG_PTR>(hookContext.hookHandler64))
                , nullptr);

            changeRWEProtection(hookContext.funcToHookAddr, false);
        }
    }

    HookContext Hooker::generateHookContextForNtProtect(DWORD64 ntdllAddr)
    {
        DWORD64 ntAllocateFuncAddr = GetProcAddress64(ntdllAddr, "NtAllocateVirtualMemory"); // change function
        DWORD64 zwWriteFileAddr = GetProcAddress64(ntdllAddr, "ZwWriteFile");

        auto hookHandler64 = m_hookGenerator.generateHookHandler(zwWriteFileAddr);
        if (nullptr == hookHandler64)
        {
            throw std::runtime_error("Can't apply Wow64 hooks");
        }

        return HookContext(ntAllocateFuncAddr, hookHandler64);
    }

    //HookContext Hooker::generateHookContextForNtAllocate(DWORD64 ntdllAddr)
    //{
    //    DWORD64 ntProtectFuncAddr = GetProcAddress64(ntdllAddr, "NtProtectVirtualMemory");
    //}

    void Hooker::changeRWEProtection(DWORD64 addr, bool needSetRWE)
    {
        BOOL result = false;
        const size_t hookJmpSize = 0xE;

        if (needSetRWE && (!m_oldProtect || (m_oldProtect & PAGE_EXECUTE_READWRITE)))
        {
            result = VirtualProtectEx64(GetCurrentProcess()
                , addr
                , hookJmpSize
                , PAGE_EXECUTE_READWRITE
                , &m_oldProtect);
        }
        else
        {
            result = VirtualProtectEx64(GetCurrentProcess()
                , addr
                , hookJmpSize
                , m_oldProtect
                , &m_oldProtect);
        }

        if (!result)
        {
            throw std::runtime_error("Can't change RWE protection");
        }
    }

    int Hooker::hook_Alloc(void* pThis,
        OPERAND_SIZE size,
        OPERAND_SIZE* pAddress,
        const OPERAND_SIZE* /*pHintAddress*/,
        int /*flags*/)

    {
        assert((nullptr != pThis) && (nullptr != pAddress));

         DWORD64 address = VirtualAllocEx64(GetCurrentProcess()
            , 0ull
            , static_cast<size_t>(size)
            , MEM_COMMIT
            , PAGE_EXECUTE_READWRITE);

         *pAddress = address;

         return address ? DI_SUCCESS : DI_ERROR;
    }

    void Hooker::hook_Free(void* pThis, const OPERAND_SIZE* pAddress)
    {
        assert((nullptr != pThis) && (nullptr != pAddress));
        VirtualFreeEx64(GetCurrentProcess(), *pAddress, 0, MEM_RELEASE);
    }
}
