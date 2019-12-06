#include "StdAfx.h"
#include "Wow64Native.h"
#include <vector>

#define NIL64 0ull
#define ARGC(count) count
#define GET_AND_CHECK_NTDLL_PROCEDURE_ADDRESS(name)             \
    DWORD64 procAddr = GetProcAddress64(getNtdll64(), #name);   \
    if (0 == procAddr) { return STATUS_ORDINAL_NOT_FOUND; }

namespace Wow64extAdapter
{
    using namespace std;

    Wow64Native::Wow64Native(HANDLE hProcess)
        : m_hProcess(hProcess)
    {
    }

    NTSTATUS Wow64Native::virtualAllocExN(DWORD64& baseAddr, size_t size, DWORD allocationType, DWORD protect)
    {
        GET_AND_CHECK_NTDLL_PROCEDURE_ADDRESS(NtAllocateVirtualMemory);

        DWORD64 size64 = size;

        return static_cast<NTSTATUS>(X64Call(procAddr
            , ARGC(6)
            , reinterpret_cast<DWORD64>(m_hProcess)
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(&baseAddr))
            , NIL64
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(&size64))
            , static_cast<DWORD64>(allocationType)
            , static_cast<DWORD64>(protect)));
    }

    NTSTATUS Wow64Native::virtualFreeExN(DWORD64 baseAddr, size_t size, DWORD freeType)
    {
        GET_AND_CHECK_NTDLL_PROCEDURE_ADDRESS(NtFreeVirtualMemory);

        DWORD64 size64 = size;

        return static_cast<NTSTATUS>(X64Call(procAddr
            , ARGC(4)
            , reinterpret_cast<DWORD64>(m_hProcess)
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(&baseAddr))
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(&size64))
            , static_cast<DWORD64>(freeType)));
    }

    NTSTATUS Wow64Native::virtualProtectExN(DWORD64 baseAddr, DWORD64 size, DWORD protect, DWORD* oldProtect)
    {
        GET_AND_CHECK_NTDLL_PROCEDURE_ADDRESS(NtProtectVirtualMemory);

        return static_cast<NTSTATUS>(X64Call(procAddr
            , ARGC(5)
            , reinterpret_cast<DWORD64>(m_hProcess)
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(&baseAddr))
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(&size))
            , static_cast<DWORD64>(protect)
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(oldProtect))));
    }

    NTSTATUS Wow64Native::virtualQueryExN(DWORD64 baseAddr, PMEMORY_BASIC_INFORMATION64 memInfo)
    {
        GET_AND_CHECK_NTDLL_PROCEDURE_ADDRESS(NtQueryVirtualMemory);

        return static_cast<NTSTATUS>(X64Call(procAddr
            , ARGC(6)
            , reinterpret_cast<DWORD64>(m_hProcess)
            , baseAddr
            , NIL64
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(memInfo))
            , static_cast<DWORD64>(sizeof(MEMORY_BASIC_INFORMATION64))
            , NIL64));
    }

    NTSTATUS Wow64Native::readProcessMemoryN(DWORD64 addrFrom, LPVOID addrTo, size_t sizeTo, DWORD64* numberOfBytesRead)
    {
        GET_AND_CHECK_NTDLL_PROCEDURE_ADDRESS(NtReadVirtualMemory);

        return static_cast<NTSTATUS>(X64Call(procAddr
            , ARGC(5)
            , reinterpret_cast<DWORD64>(m_hProcess)
            , addrFrom
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(addrTo))
            , static_cast<DWORD64>(sizeTo)
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(numberOfBytesRead))));
    }

    NTSTATUS Wow64Native::writeProcessMemoryN(DWORD64 addrTo, LPCVOID addrFrom, size_t sizeFrom, DWORD64* numberOfBytesWritten)
    {
        GET_AND_CHECK_NTDLL_PROCEDURE_ADDRESS(NtWriteVirtualMemory);

        return static_cast<NTSTATUS>(X64Call(procAddr
            , ARGC(5)
            , reinterpret_cast<DWORD64>(m_hProcess)
            , addrTo
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(addrFrom))
            , static_cast<DWORD64>(sizeFrom)
            , static_cast<DWORD64>(reinterpret_cast<ULONG_PTR>(numberOfBytesWritten))));
    }

    DWORD64 Wow64Native::getModuleHandleN(const wchar_t* moduleName)
    {
        vector<wchar_t> name(moduleName, moduleName + (wcslen(moduleName) + 1));
        return GetModuleHandle64(name.data());
    }

    DWORD64 Wow64Native::getProcAddressN(DWORD64 moduleAddr, const char* funcName)
    {
        vector<char> name(funcName, funcName + (strlen(funcName) + 1));
        return GetProcAddress64(moduleAddr, name.data());
    }

    DWORD64 Wow64Native::getModuleSizeN(const wchar_t* moduleName)
    {
        GET_AND_CHECK_NTDLL_PROCEDURE_ADDRESS(RtlImageNtHeader);

        vector<wchar_t> name(moduleName, moduleName + (wcslen(moduleName) + 1));
        DWORD64 moduleSize = 0;

        DWORD64 headersAddr = X64Call(procAddr
            , ARGC(1)
            , GetModuleHandle64(name.data()));

        if (headersAddr)
        {
            IMAGE_NT_HEADERS64 headers{};
            DWORD64 numberOfBytesRead = 0;

            NTSTATUS status = readProcessMemoryN(headersAddr, &headers, sizeof(IMAGE_NT_HEADERS64), &numberOfBytesRead);

            if (STATUS_SUCCESS == status
                && IMAGE_NT_SIGNATURE == headers.Signature
                && IMAGE_FILE_MACHINE_AMD64 == headers.FileHeader.Machine)
            {
                moduleSize = headers.OptionalHeader.SizeOfImage;
            }
        }

        return moduleSize;
    }

    DWORD64 Wow64Native::getNtdll64()
    {
        return GetModuleHandle64(L"ntdll.dll");
    }
}
