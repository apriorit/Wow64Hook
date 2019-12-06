#include "StdAfx.h"
#include "MemoryStream.h"

namespace DianaHook
{
    MemoryStream::MemoryStream(IMemoryReaderWriter& memoryReaderWriter,
        OPERAND_SIZE currentOffset,
        OPERAND_SIZE moduleSize)
        : m_memoryReaderWriter(memoryReaderWriter)
        , m_currentOffset(currentOffset)
        , m_moduleSize(moduleSize)
    {
        DianaMovableReadStream_Init(&parent
            , Read
            , AnalyzeMoveTo
            , RandomRead);

        pRandomWrite = RandomWrite;
    }

    int MemoryStream::AnalyzeMoveTo(void* pThis, OPERAND_SIZE offset)
    {
        assert(nullptr != pThis);
        auto self = static_cast<MemoryStream*>(pThis);

        if (self->m_moduleSize && offset > self->m_moduleSize)
        {
            return DI_END_OF_STREAM;
        }

        self->m_currentOffset = offset;
        return DI_SUCCESS;
    }

    int MemoryStream::Read(void* pThis, void* pBuffer, int iBufferSize, int* bytesRead)
    {
        try
        {
            assert((nullptr != pThis) && (nullptr != pBuffer) && (nullptr != bytesRead));

            auto self = static_cast<MemoryStream*>(pThis);
            size_t read = 0;
            int flags = 0;

            self->m_memoryReaderWriter.read(self->m_currentOffset
                , pBuffer
                , iBufferSize
                , read
                , flags);

            *bytesRead = static_cast<int>(read);
            self->m_currentOffset += static_cast<OPERAND_SIZE>(read);

            return DI_SUCCESS;
        }
        catch (std::exception&)
        {
            return DI_ERROR;
        }
    }

    int MemoryStream::RandomRead(void* pThis,
        OPERAND_SIZE offset,
        void* pBuffer,
        int iBufferSize,
        OPERAND_SIZE* readBytes,
        int flags)
    {
        try
        {
            assert((nullptr != pThis) && (nullptr != pBuffer) && (nullptr != readBytes));
            auto self = static_cast<MemoryStream*>(pThis);

            size_t read = 0;
            self->m_memoryReaderWriter.read(offset
                , pBuffer
                , iBufferSize
                , read
                , flags);

            *readBytes = read;
            return DI_SUCCESS;
        }
        catch (std::exception&)
        {
            return DI_ERROR;
        }
    }

    int MemoryStream::RandomWrite(void* pThis,
        OPERAND_SIZE offset,
        void* pBuffer,
        int iBufferSize,
        OPERAND_SIZE* writtenBytes,
        int flags)
    {
        try
        {
            assert((nullptr != pThis) && (nullptr != pBuffer) && (nullptr != writtenBytes));
            auto self = static_cast<MemoryStream*>(pThis);

            size_t written = 0;
            self->m_memoryReaderWriter.write(offset
                , pBuffer
                , iBufferSize
                , written
                , flags);

            *writtenBytes = written;
            return DI_SUCCESS;
        }
        catch (std::exception&)
        {
            return DI_ERROR;
        }
    }
}
