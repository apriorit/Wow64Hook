#pragma once

namespace Wow64Hooker
{
    using namespace asmjit;

    using HookHandler32 = void(__stdcall*)();
    using ShellcodeHandler64 = void(*)(void);

    class ShellCode64Generator
    {
    public:
        ShellCode64Generator() = default;

        template<typename Type>
        ShellcodeHandler64 generateShellcodeHandler64(Type addr)
        {
            JitRuntime runtime;
            CodeHolder code;
            code.init(runtime.getCodeInfo());

            X86Assembler assembler(&code);
            generate(assembler, addr);

            // Add the generated code to the runtime
            ShellcodeHandler64 hookHandler;
            Error error = runtime.add(&hookHandler, &code);
            if (kErrorOk != error)
            {
                return nullptr;
            }

            return copyHookHandler(hookHandler, code.getCodeSize());
        }

    private:
        void generate(X86Assembler& assembler, const HookHandler32* hookHandler32);
        void generate(X86Assembler& assembler, DWORD64 writeFileFuncAddr);

        ShellcodeHandler64 copyHookHandler(ShellcodeHandler64 hookHandler, size_t size);
    };
}
