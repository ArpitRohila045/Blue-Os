// Minimal C++ runtime support stubs for freestanding/kernel environment
// Provides guard and atexit stubs used by compiler-generated C++ runtime

extern "C" {

// Support for static local initialization guards
int __cxa_guard_acquire(void* guard) {
    // guard is usually one byte; return non-zero to indicate initialization needed
    return (*(char*)guard) == 0;
}

void __cxa_guard_release(void* guard) {
    (*(char*)guard) = 1;
}

void __cxa_guard_abort(void* /*guard*/) {
    // No-op in kernel environment
}

// atexit/__cxa_atexit: in kernel we typically do not run exit handlers.
int __cxa_atexit(void (* /*func*/)(void*), void* /*arg*/, void* /*dso_handle*/) {
    return 0; // indicate success but do not store the handler
}

int atexit(void (* /*func*/)()) {
    return 0; // no-op
}

// Pure virtual call handler to avoid link-time errors if used
void __cxa_pure_virtual() {
    // You might want to halt or log here; keep no-op to avoid crash during link
}

}
