#pragma once
#include "MemoryStream.h"

namespace DianaHook
{
    class MemoryProvider
    {
    public:
        MemoryProvider(const MemoryProvider&) = delete;
        MemoryProvider& operator = (const MemoryProvider&) = delete;

        MemoryProvider(IMemoryReaderWriter& memoryReaderWriter,
            DianaHook_Alloc_type hookAlloc,
            DianaHook_Free_type hookFree);

        DianaHook_TargetMemoryProvider *getTargetMemoryProvider();

    private:
        MemoryStream m_memoryStream;
        DianaHook_Allocator m_allocator;
        DianaHook_TargetMemoryProvider m_impl;
    };
}
