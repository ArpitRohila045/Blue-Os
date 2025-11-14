#ifndef __BLUE_OS__MEMORY__MEMORYMANAGMENT_H__
#define __BLUE_OS__MEMORY__MEMORYMANAGMENT_H__

#include <common/types.h>

namespace blueOs{ 
    namespace memory{

        struct MemoryChunk{
            MemoryChunk* next;
            MemoryChunk* prev;
            bool allocated;
            blueOs::common::size_t size;
        } __attribute__((packed));

        class MemoryManager{
            protected:
                MemoryChunk* head, tail;
            public:

                static MemoryManager* activeMemoryManager;
                MemoryManager(common::size_t first, common::size_t size);
                ~MemoryManager();

                void* kmalloc(common::size_t size);
                void kfree(void* ptr);
        };
    }
}

void* operator new(unsigned size);
void* operator new[](unsigned size);

// placement new
void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);

#endif