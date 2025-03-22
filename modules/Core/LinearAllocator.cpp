#include "Allocator.h"

namespace wind
{
    void* LinearAllocator::Allocate(size_t size, size_t alignment)
    {
        std::size_t       padding        = 0;
        std::size_t       paddedAddress  = 0;
        const std::size_t currentAddress = (std::size_t)m_StartPtr + m_Offset;

        if (alignment != 0 && m_Offset % alignment != 0)
        {
            // Alignment is required. Find the next aligned memory address and update offset
            padding = CalculatePadding(currentAddress, alignment);
        }

        if (m_Offset + padding + size > m_Size)
        {
            return nullptr;
        }

        m_Offset += padding;
        const std::size_t nextAddress = currentAddress + padding;
        m_Offset += size;

#ifdef _DEBUG
        std::cout << "A" << "\t@C " << (void*)currentAddress << "\t@R " << (void*)nextAddress << "\tO " << m_offset
                  << "\tP " << padding << std::endl;
#endif
        return (void*)nextAddress;
    }

    void LinearAllocator::Free(void* ptr)
    {
        // No-op for linear allocator
    }

    void LinearAllocator::Reset()
    {
        m_Offset = 0;
    }
} // namespace wind