#include <memory/memorymanagment.h>

using namespace blueOs::memory;
using namespace blueOs::common;

MemoryManager* MemoryManager::activeMemoryManager = 0;
MemoryManager::MemoryManager(size_t first, size_t size){
    activeMemoryManager = this;
    if(size < sizeof(MemoryChunk)){
        head = 0;
    }else{
        head = (MemoryChunk*)first;
        head->allocated = false;
        head->next = 0;
        head->prev = 0;
        head->size = size - sizeof(MemoryChunk);
    }
}

MemoryManager::~MemoryManager(){
    activeMemoryManager = 0;
}

void* MemoryManager::kmalloc(size_t size){
    MemoryChunk* result = 0;

    for(MemoryChunk* chunk = head; chunk != 0 && result != 0; chunk = chunk->next){
        if(!chunk->allocated && chunk->size > size){
            result = chunk;
        }
    }

    if(result == 0) return 0;

    if(result->size > size + sizeof(MemoryChunk)){
        MemoryChunk* newChunk = (MemoryChunk*)((size_t)result + size + sizeof(MemoryChunk));
        newChunk->allocated = false;
        newChunk->next = result->next;
        newChunk->prev = result;
        newChunk->size = result->size - size - sizeof(MemoryChunk);

        result->next = newChunk;
        result->size = size;

        if(newChunk->next != 0){
            newChunk->next->prev = newChunk;
        }
    }

    result->allocated = true;
    return (void*)(sizeof(MemoryChunk) + ((size_t)result));
}



void MemoryManager::kfree(void* ptr){
    if(ptr == 0) return;

    MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    chunk->allocated = false;

    if(chunk->next != 0 && !chunk->next->allocated){
        chunk->size += sizeof(MemoryChunk) + chunk->next->size;
        chunk->next = chunk->next->next;

        if(chunk->next != 0){
            chunk->next->prev = chunk;
        }
    }

    if(chunk->prev != 0 && !chunk->prev->allocated){
        chunk->prev->size = sizeof(MemoryChunk) + chunk->size;
        chunk->prev->next = chunk->next;

        if(chunk->next != 0){
            chunk->next->prev = chunk->prev;
        }
    }
}



void* operator new(unsigned size){
    if(blueOs::memory::MemoryManager::activeMemoryManager == 0){
        return 0;
    }
    return blueOs::memory::MemoryManager::activeMemoryManager->kmalloc(size);
}


void* operator new[](unsigned size){
    if(blueOs::memory::MemoryManager::activeMemoryManager == 0){
        return 0;
    }
    return blueOs::memory::MemoryManager::activeMemoryManager->kmalloc(size);
}


void* operator new(unsigned size, void* ptr){
    return ptr;
}


void* operator new[](unsigned size, void* ptr){
    return ptr;
}


void operator delete(void* ptr){
    if(blueOs::memory::MemoryManager::activeMemoryManager == 0){
        return;
    }
    blueOs::memory::MemoryManager::activeMemoryManager->kfree(ptr);
}


void operator delete[](void* ptr){
    if(blueOs::memory::MemoryManager::activeMemoryManager == 0){
        return;
    }
    blueOs::memory::MemoryManager::activeMemoryManager->kfree(ptr);
}





