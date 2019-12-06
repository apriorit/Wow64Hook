#include "StdAfx.h"
#include "Wow64MemoryReaderWriter.h"

namespace Wow64Hooker
{
    void Wow64MemoryReaderWriter::read(uint64_t address, void* buffer, size_t bytesToRead, size_t& bytesRead, int /*flags*/)
    {
        assert(nullptr != buffer);

        SIZE_T read = 0;
        BOOL result = ReadProcessMemory64(GetCurrentProcess()
            , address
            , buffer
            , bytesToRead
            , &read);

        if (!result)
        {
            throw std::runtime_error("Can't read process memory");
        }

        bytesRead = bytesToRead;
    }

    void Wow64MemoryReaderWriter::write(uint64_t address, void* buffer, size_t bytesToWrite, size_t& bytesWritten, int /*flags*/)
    {
        assert(nullptr != buffer);

        SIZE_T written = 0;
        BOOL result = WriteProcessMemory64(GetCurrentProcess()
            , address
            , buffer
            , bytesToWrite
            , &written);

        if (!result)
        {
            throw std::runtime_error("Can't write process memory");
        }

        bytesWritten = bytesToWrite;
    }
}
