#pragma once

namespace Wow64Hooker
{
    using HookHandler32 = void(__stdcall*)();
    using HookHandler64 = void(*)(void);

    class HookGenerator
    {
    public:
        HookGenerator() = default;
        HookHandler64 generateHookHandler(DWORD64 addr);
        HookHandler64 generateHookHandler(const HookHandler32* hookHandler32);

    private:
        HookHandler64 HookGenerator::copyHookHandler(HookHandler64 hookHandler, size_t size);
    };
}
