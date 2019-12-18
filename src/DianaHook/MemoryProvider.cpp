#include "StdAfx.h"
#include "MemoryProvider.h"

namespace DianaHook
{
    MemoryProvider::MemoryProvider(IMemoryReaderWriter& memoryReaderWriter,
        DianaHook_Alloc_type hookAlloc,
        DianaHook_Free_type hookFree)
        : m_memoryStream(memoryReaderWriter, 0, ULLONG_MAX)
    {
        DianaHook_Allocator_Init(&m_allocator, hookAlloc, hookFree);
        DianaHook_TargetMemoryProvider_Init(&m_impl, &m_memoryStream, &m_allocator);
    }

    DianaHook_TargetMemoryProvider* MemoryProvider::getTargetMemoryProvider()
    {
        return &m_impl;
    }
}
