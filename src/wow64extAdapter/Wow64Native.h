#pragma once
#include "INative.h"

namespace Wow64extAdapter
{
    class Wow64Native : public INativeWow64ext
    {
    public:
        Wow64Native(HANDLE hProcess);

        NTSTATUS virtualAllocExN(DWORD64& baseAddr, size_t size, DWORD allocationType, DWORD protect) override;
        NTSTATUS virtualFreeExN(DWORD64 baseAddr, size_t size, DWORD freeType) override;

        NTSTATUS virtualProtectExN(DWORD64 baseAddr, DWORD64 size, DWORD protect, DWORD* oldProtect) override;
        NTSTATUS virtualQueryExN(DWORD64 baseAddr, PMEMORY_BASIC_INFORMATION64 memInfo) override;

        NTSTATUS readProcessMemoryN(DWORD64 addrFrom, LPVOID addrTo, size_t sizeTo, PDWORD64 numberOfBytesRead) override;
        NTSTATUS writeProcessMemoryN(DWORD64 addrTo, LPCVOID addrFrom, size_t sizeFrom, PDWORD64 numberOfBytesWritten) override;

        DWORD64 getModuleHandleN(const wchar_t* moduleName) override;
        DWORD64 getProcAddressN(DWORD64 moduleAddr, const char* funcName) override;
        DWORD64 getModuleSizeN(const wchar_t* moduleName) override;

        DWORD64 getNtdll64();

    private:
        HANDLE m_hProcess;
    };
}
