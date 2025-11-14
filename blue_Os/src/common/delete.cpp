// ...existing code...
extern "C" void operator delete(void* ptr) noexcept {
    (void)ptr;
    // TODO: hook into kernel allocator if available.
}

extern "C" void operator delete(void* ptr, unsigned int) noexcept {
    operator delete(ptr);
}
