#include "StdAfx.h"
#include "DianaHook.h"

namespace DianaHook
{
    DianaHookPatcher::DianaHookPatcher(IMemoryReaderWriter& memoryReaderWriter,
        DianaHook_Alloc_type hookAlloc,
        DianaHook_Free_type hookFree)
        : m_memoryProvider(memoryReaderWriter, hookAlloc, hookFree)
    {
        Diana_Init();
    }

    void DianaHookPatcher::addHook(int processorMode,
        OPERAND_SIZE addressToHook,
        OPERAND_SIZE hookFunction,
        DianaHook_CustomOptions* customOptions)
    {
        DianaHook_PatchStream(m_memoryProvider.getTargetMemoryProvider()
            , processorMode
            , addressToHook
            , hookFunction
            , customOptions);
    }
}
