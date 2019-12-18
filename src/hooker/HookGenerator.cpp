#include "StdAfx.h"
#include "HookGenerator.h"

#define OPCODE(op) op

namespace
{
    const char* g_hookMessage = "Hello from hooked 64-bit NtWriteVirtualMemory function\n";
    ULONG64 g_ioStatusBlock = 0;
}

namespace Wow64Hooker
{
    void ShellCode64Generator::generate(X86Assembler& assembler, DWORD64 writeFileFuncAddr)
    {
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
        assembler.push(strlen(g_hookMessage));                              // Length
        assembler.push(reinterpret_cast<uint64_t>(g_hookMessage));          // Buffer
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

        assembler.ret();
    }

    void ShellCode64Generator::generate(X86Assembler& assembler, const HookHandler32* hookHandler32)
    {
        // Label uses to adjust mode after returning from hookHandler32
        Label lBackFromAlert = assembler.newLabel();

        // prepare retaddress to return from hookHandler32
        assembler.sub(x86::esp, 0x04);
        assembler.lea(x86::eax, x86::ptr(lBackFromAlert));
        assembler.mov(x86::dword_ptr(x86::rsp), x86::eax);

        // specify x86 code segment
        const uint32_t x86CodeSegment = 0x23;
        assembler.mov(x86::rax, x86CodeSegment);

        // prepare hookHandler32 address for calling
        const uint32_t sizeToShift = 0x20;
        assembler.shl(x86::rax, sizeToShift);
        assembler.push(x86::rax);
        assembler.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64_t>(hookHandler32)));
        assembler.mov(x86::dword_ptr(x86::rsp), x86::eax);

        // generate retf instruction to switch to x86
        assembler.db(OPCODE(0xCB));

        // begin of code after return from hookHandler32
        assembler.bind(lBackFromAlert);

        // switch back to x64 mode
        assembler.db(OPCODE(0x6A)); // push
        assembler.db(OPCODE(0x33)); // x64 code segment
        assembler.db(OPCODE(0xE8)); // call (current_addr + 5)
        assembler.db(OPCODE(0x00));
        assembler.db(OPCODE(0x00));
        assembler.db(OPCODE(0x00));
        assembler.db(OPCODE(0x00));
        assembler.db(OPCODE(0x83)); // add
        assembler.db(OPCODE(0x04)); // dword
        assembler.db(OPCODE(0x24)); // [esp]
        assembler.db(OPCODE(0x05)); // 0x05
        assembler.db(OPCODE(0xCB)); // retf

        assembler.ret();
    }

    ShellcodeHandler64 ShellCode64Generator::copyHookHandler(ShellcodeHandler64 hookHandlerOrig, size_t size)
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

        ShellcodeHandler64 hookHandlerCopy = reinterpret_cast<ShellcodeHandler64>(address);
        memcpy(hookHandlerCopy, hookHandlerOrig, size);

        return hookHandlerCopy;
    }
}
