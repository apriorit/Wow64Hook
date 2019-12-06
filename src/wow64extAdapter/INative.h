#pragma once
#include "Wow64Types.h"

namespace Wow64extAdapter
{
    struct INative
    {
        virtual ~INative() = default;

        virtual NTSTATUS virtualAllocExN(DWORD64& baseAddr, size_t size, DWORD allocationType, DWORD protect) = 0;
        virtual NTSTATUS virtualFreeExN(DWORD64 baseAddr, size_t size, DWORD freeType) = 0;

        virtual NTSTATUS virtualProtectExN(DWORD64 baseAddr, DWORD64 size, DWORD protect, DWORD* oldProtect) = 0;
        virtual NTSTATUS virtualQueryExN(DWORD64 baseAddr, PMEMORY_BASIC_INFORMATION64 memInfo) = 0;

        virtual NTSTATUS readProcessMemoryN(DWORD64 addrFrom, LPVOID addrTo, size_t sizeTo, PDWORD64 numberOfBytesRead) = 0;
        virtual NTSTATUS writeProcessMemoryN(DWORD64 addrTo, LPCVOID addrFrom, size_t sizeFrom, PDWORD64 numberOfBytesWritten) = 0;
    };

    struct INativeWow64ext : public INative
    {
        virtual DWORD64 getModuleHandleN(const wchar_t* moduleName) = 0;
        virtual DWORD64 getProcAddressN(DWORD64 moduleAddr, const char* funcName) = 0;
        virtual DWORD64 getModuleSizeN(const wchar_t* moduleName) = 0;
    };
}
