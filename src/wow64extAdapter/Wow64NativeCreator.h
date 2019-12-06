#pragma once
#include "Wow64Native.h"

namespace Wow64extAdapter
{
    class Wow64NativeCreator
    {
    public:
        ~Wow64NativeCreator() = default;

        inline virtual INativeWow64ext* create(void* hProcess)
        {
            return new Wow64Native(hProcess);
        }
    };
}
