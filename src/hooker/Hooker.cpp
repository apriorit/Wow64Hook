#include "StdAfx.h"
#include "Hooker.h"

namespace Wow64Hooker
{
    HookContext::HookContext(DWORD64 funcToHookAddr, ShellcodeHandler64 hookHandler64)
        : funcToHookAddr(funcToHookAddr)
        , hookHandler64(hookHandler64)
    {
    }

    Hooker::Hooker(const HookHandler32* hookHandler32)
        : m_dianaHook(m_wow64ReaderWriter, Hooker::hook_Alloc, Hooker::hook_Free)
    {
        DWORD64 ntdllAddr = GetModuleHandle64(L"ntdll.dll");
        if (!ntdllAddr)
        {
            throw std::runtime_error("Can't get 64-bit ntdll address");
        }

        DWORD64 funcToCallAddr = getFunctionAddress64(ntdllAddr, "ZwWriteFile");

        m_hookContexts.push_back(generateHookContext(ntdllAddr, "NtReadVirtualMemory", hookHandler32));
        m_hookContexts.push_back(generateHookContext(ntdllAddr, "NtWriteVirtualMemory", funcToCallAddr));
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

    DWORD64 Hooker::getFunctionAddress64(DWORD64 moduleAddr, char* funcName)
    {
        DWORD64 funcAddr = GetProcAddress64(moduleAddr, funcName);

        if (!funcAddr)
        {
            std::stringstream ss;
            ss << "Can't get 64-bit " << funcName << " function address";

            throw std::runtime_error(ss.str());
        }

        return funcAddr;
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
