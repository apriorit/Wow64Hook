#pragma once
#include "MemoryProvider.h"

namespace DianaHook
{
    class DianaHookPatcher
    {
    public:
        DianaHookPatcher(IMemoryReaderWriter& memoryReaderWriter,
            DianaHook_Alloc_type hookAlloc,
            DianaHook_Free_type hookFree);

        void addHook(int processorMode,
            OPERAND_SIZE addressToHook,
            OPERAND_SIZE hookFunction,
            DianaHook_CustomOptions* pCustomOptions);

    private:
        MemoryProvider m_memoryProvider;
    };
}
