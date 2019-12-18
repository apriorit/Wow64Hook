#pragma once
#include "DianaHook/IMemoryReaderWriter.h"

namespace Wow64Hooker
{
    struct Wow64MemoryReaderWriter : public DianaHook::IMemoryReaderWriter
    {
        void read(uint64_t address, void* buffer, size_t bytesToRead, size_t& bytesRead, int flags) override;
        void write(uint64_t address, void* buffer, size_t bytesToWrite, size_t& bytesWritten, int flags) override;
    };
}
