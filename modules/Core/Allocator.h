#pragma once

#include <cstddef>
#include <cstdlib>
#include <utility>

namespace wind
{
    inline const std::size_t CalculateAlignmentPadding(const std::size_t baseAddress, const std::size_t alignment)
    {
        const std::size_t multiplier     = (baseAddress / alignment) + 1;
        const std::size_t alignedAddress = multiplier * alignment;
        return alignedAddress - baseAddress;
    }

    class IAllocator
    {
    public:
        virtual ~IAllocator() = default;

        // allocate memory
        virtual void* Allocate(size_t size, size_t alignment) = 0;
        // deallocate memory
        virtual void Deallocate(void* ptr) = 0;
        // reset allocator state
        virtual void Reset() = 0;
        // 获取已分配的内存大小
        virtual size_t GetTotalAllocated() const = 0;
    };

    // Stack allocator for temporary memory allocation
    class StackAllocator : public IAllocator
    {
    public:
        explicit StackAllocator(size_t capacity) : m_Capacity(capacity), m_Used(0), m_BasePtr(nullptr)
        {
            m_BasePtr = std::malloc(capacity);
        }

        ~StackAllocator() override { std::free(m_BasePtr); }

        void* Allocate(size_t size, size_t alignment) override
        {
            const size_t padding   = CalculateAlignmentPadding(m_Used, alignment);
            const size_t totalSize = size + padding;

            if (m_Used + totalSize > m_Capacity)
                return nullptr;

            void* ptr = static_cast<char*>(m_BasePtr) + m_Used + padding;
            m_Used += totalSize;
            return ptr;
        }

        void Deallocate(void* ptr) override
        {
            // Stack allocator doesn't support individual deallocation
            // Use Reset() to clear all memory
        }

        void Reset() override { m_Used = 0; }

        size_t GetTotalAllocated() const override { return m_Used; }

        // construct object on allocated memory
        template<typename T, typename... Args>
        T* Construct(Args&&... args)
        {
            return new (Allocate(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
        }

        // get remaining space
        size_t GetRemainingSpace() const { return m_Capacity - m_Used; }

    private:
        void*  m_BasePtr;
        size_t m_Capacity;
        size_t m_Used;
    };

    class LinearAllocator : public IAllocator
    {
    public:
        LinearAllocator(size_t size) : m_Size(size), m_Offset(0), m_StartPtr(nullptr) { m_StartPtr = malloc(size); }

        ~LinearAllocator() override { free(m_StartPtr); }

        void* Allocate(size_t size, size_t alignment) override
        {
            std::size_t       padding        = 0;
            std::size_t       paddedAddress  = 0;
            const std::size_t currentAddress = (std::size_t)m_StartPtr + m_Offset;

            if (alignment != 0 && m_Offset % alignment != 0)
            {
                // Alignment is required. Find the next aligned memory address and update offset
                padding = CalculateAlignmentPadding(currentAddress, alignment);
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

        template<typename T, typename... Args>
        T* Construct(Args&&... args)
        {
            return new (Allocate(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
        }

        void Deallocate(void* ptr) override
        {
            // Linear allocator doesn't support individual deallocation
        }

        void Reset() override { m_Offset = 0; }

        size_t GetTotalAllocated() const override { return m_Offset; }

    private:
        size_t m_Size;
        size_t m_Offset;
        void*  m_StartPtr;
    };

    // Pool allocator for fixed size objects
    class PoolAllocator : public IAllocator
    {
    public:
        PoolAllocator(size_t blockSize, size_t blockCount) :
            m_BlockSize(blockSize), m_BlockCount(blockCount), m_UsedBlocks(0), m_BasePtr(nullptr), m_FreeList(nullptr)
        {
            m_BasePtr = std::malloc(blockSize * blockCount);
            InitializeFreeList();
        }

        ~PoolAllocator() override { std::free(m_BasePtr); }

        void* Allocate(size_t size, size_t alignment) override
        {
            if (size > m_BlockSize || m_UsedBlocks >= m_BlockCount)
                return nullptr;

            void* ptr  = m_FreeList;
            m_FreeList = *static_cast<void**>(m_FreeList);
            m_UsedBlocks++;
            return ptr;
        }

        void Deallocate(void* ptr) override
        {
            if (!ptr)
                return;

            *static_cast<void**>(ptr) = m_FreeList;
            m_FreeList                = ptr;
            m_UsedBlocks--;
        }

        void Reset() override
        {
            m_UsedBlocks = 0;
            InitializeFreeList();
        }

        size_t GetTotalAllocated() const override { return m_UsedBlocks * m_BlockSize; }

    private:
        void InitializeFreeList()
        {
            m_FreeList    = m_BasePtr;
            char* current = static_cast<char*>(m_BasePtr);

            for (size_t i = 0; i < m_BlockCount - 1; ++i)
            {
                *reinterpret_cast<void**>(current) = current + m_BlockSize;
                current += m_BlockSize;
            }

            *reinterpret_cast<void**>(current) = nullptr;
        }

        void*  m_BasePtr;
        void*  m_FreeList;
        size_t m_BlockSize;
        size_t m_BlockCount;
        size_t m_UsedBlocks;
    };
} // namespace wind