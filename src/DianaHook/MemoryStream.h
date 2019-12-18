#pragma once
#include "IMemoryReaderWriter.h"

extern "C"
{
#include <diana_ultimate_patcher.h>
#include <diana_streams.h>
}

namespace DianaHook
{
    class MemoryStream : public DianaReadWriteRandomStream
    {
    public:
        MemoryStream(IMemoryReaderWriter& memoryReaderWriter,
            OPERAND_SIZE currentOffset,
            OPERAND_SIZE moduleSize);

        static int AnalyzeMoveTo(void* pThis, OPERAND_SIZE offset);
        static int Read(void* pThis, void* pBuffer, int iBufferSize, int* bytesRead);

        static int RandomRead(void* pThis,
            OPERAND_SIZE offset,
            void* pBuffer,
            int iBufferSize,
            OPERAND_SIZE* readBytes,
            int flags);

        static int RandomWrite(void* pThis,
            OPERAND_SIZE offset,
            void* pBuffer,
            int iBufferSize,
            OPERAND_SIZE* writtenBytes,
            int flags);

    private:
        IMemoryReaderWriter& m_memoryReaderWriter;
        OPERAND_SIZE m_currentOffset;
        OPERAND_SIZE m_moduleSize;
    };
}
