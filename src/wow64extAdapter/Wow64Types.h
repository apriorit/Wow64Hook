#pragma once

// to avoid linkage warnings
#define WOW64EXT_EXPORTS
#include <wow64ext.h>
#include <internal.h>

#define WOW64_x64_Start() X64_Start()
#define WOW64_x64_End() X64_End()
#define WOW64_x64_Push(r) X64_Push(r)
#define WOW64_x64_Pop(r) X64_Pop(r)

namespace Wow64extAdapter
{
    using WOW64_LDR_DATA_TABLE_ENTRY32 = LDR_DATA_TABLE_ENTRY32;
    using WOW64_LDR_DATA_TABLE_ENTRY64 = LDR_DATA_TABLE_ENTRY64;

    using WOW64_PEB_LDR_DATA32 = PEB_LDR_DATA32;
    using WOW64_PEB_LDR_DATA64 = PEB_LDR_DATA64;

    using WOW64_TEB32 = TEB32;
    using WOW64_TEB64 = TEB64;

    using WOW64_PEB32 = PEB32;
    using WOW64_PEB64 = PEB64;

    using WOW64_CONTEXT64 = _CONTEXT64;

    using WOW64_REGISTER64 = reg64;
}
