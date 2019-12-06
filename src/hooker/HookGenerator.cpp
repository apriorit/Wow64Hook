#include "StdAfx.h"
#include "HookGenerator.h"

#define OPCODE(op) op
using namespace asmjit;

namespace
{
    std::string g_hookMessage = "Hello from 64-bit Wow64 hook handler";
    ULONG64 g_ioStatusBlock = 0;
}

namespace Wow64Hooker
{
    HookHandler64 HookGenerator::generateHookHandler(DWORD64 writeFileFuncAddr)
    {
        // Runtime specialized for JIT code execution
        JitRuntime runtime;
        // Holds code and relocation information
        CodeHolder code;
        // Initialize to the same arch as JIT runtime
        code.init(runtime.getCodeInfo());
        // Create and attach X86Assembler to code
        X86Assembler assembler(&code);

        assembler.int3();

        // save stack pointer
        assembler.push(x86::rbp);
        assembler.mov(x86::rbp, x86::rsp);

        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (INVALID_HANDLE_VALUE == handle || !handle)
        {
            throw std::runtime_error("Cannot get stdout handle");
        }

        // prepare parameters to call ZwWriteFile
        assembler.mov(x86::rcx, reinterpret_cast<uint64_t>(handle));        // FileHandle
        assembler.mov(x86::rdx, 0);                                         // Event
        assembler.mov(x86::r8, 0);                                          // ApcRoutine
        assembler.mov(x86::r9, 0);                                          // ApcContext
        assembler.push(0);                                                  // Key
        assembler.push(0);                                                  // ByteOffset
        assembler.push(g_hookMessage.size());                               // Length
        assembler.push(reinterpret_cast<uint64_t>(g_hookMessage.c_str()));  // Buffer
        assembler.push(reinterpret_cast<uint64_t>(&g_ioStatusBlock));       // IoStatusBlock

        // align stack
        assembler.push(0);
        assembler.push(0);
        assembler.push(0);
        assembler.push(0);

        assembler.call(writeFileFuncAddr);

        // restore stack pointer
        assembler.mov(x86::rsp, x86::rbp);
        assembler.pop(x86::rbp);

        assembler.int3();

        assembler.ret();

        HookHandler64 hookHandler;
        // Add the generated code to the runtime
        Error error = runtime.add(&hookHandler, &code);

        if (kErrorOk != error)
        {
            return nullptr;
        }

        return copyHookHandler(hookHandler, code.getCodeSize());
    }

    HookHandler64 HookGenerator::generateHookHandler(const HookHandler32* hookHandler32)
    {
        HookHandler64 h;
        return h;
    }

    HookHandler64 HookGenerator::copyHookHandler(HookHandler64 hookHandler, size_t size)
    {
        //
        // need to copy generated handler to avoid 
        // its deallocation from CodeHolder destructor
        //
        DWORD64 address = VirtualAllocEx64(GetCurrentProcess()
            , 0ull
            , size
            , MEM_COMMIT
            , PAGE_EXECUTE_READWRITE);

        if (!address)
        {
            return nullptr;
        }

        HookHandler64 hookHandlerCopy = reinterpret_cast<HookHandler64>(address);
        memcpy(hookHandlerCopy, hookHandler, size);

        return hookHandlerCopy;
    }
}
