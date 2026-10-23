#ifndef OBJC3_SYSTEM_H
#define OBJC3_SYSTEM_H

#include "core.h"
#if __OBJC3_PROFILE__ != 3
#error                                                                         \
    "objc3.system requires the strict-system profile (-fobjc3-profile=strict-system)"
#endif
#if !defined(__linux__)
#error "objc3.system resource helpers require Linux"
#endif

#include <stdlib.h>
#include <unistd.h>

#define OBJC3_SYSTEM_CAPABILITY_ID "objc3.cap.system"

/* Close exactly once, returning the POSIX status and preserving errno.
 * Retrying close after EINTR can close a descriptor reused by another thread.
 * close is a POSIX cancellation point, so this wrapper is not marked nothrow.
 */
static inline int objc3_file_descriptor_close(int descriptor) {
  return close(descriptor);
}

typedef int objc3_file_descriptor __attribute__((
    objc_requires_cleanup(objc3_file_descriptor_close),
    objc_resource_family(ObjC3FileDescriptors), objc_invalid(-1)));

/* The caller promises that ownership of the raw descriptor is transferred.
 * Use take(resource) when adopting from an existing resource variable. */
static inline objc3_file_descriptor objc3_file_descriptor_unsafe_adopt(int raw)
    __attribute__((nothrow));
static inline objc3_file_descriptor
objc3_file_descriptor_unsafe_adopt(int raw) {
  return raw;
}

/* The input is borrowed; success returns a separately owned descriptor.
 * Failure is -1. Descriptor flags follow the POSIX dup contract. */
static inline objc3_file_descriptor
objc3_file_descriptor_duplicate(objc3_file_descriptor descriptor)
    __attribute__((nothrow));
static inline objc3_file_descriptor
objc3_file_descriptor_duplicate(objc3_file_descriptor descriptor) {
  return dup(descriptor);
}

static inline void objc3_heap_buffer_free(void *buffer)
    __attribute__((nothrow));
static inline void objc3_heap_buffer_free(void *buffer) { free(buffer); }

typedef void *objc3_heap_buffer __attribute__((
    objc_requires_cleanup(objc3_heap_buffer_free),
    objc_resource_family(ObjC3HeapBuffers), objc_invalid((void *)0)));

/* malloc failure is the null sentinel. Zero-size allocations follow libc. */
static inline objc3_heap_buffer objc3_heap_buffer_allocate(size_t size)
    __attribute__((nothrow, malloc, alloc_size(1)));
static inline objc3_heap_buffer objc3_heap_buffer_allocate(size_t size) {
  return malloc(size);
}

/* Adopt only storage whose ownership and deallocator are known to be free(). */
static inline objc3_heap_buffer objc3_heap_buffer_unsafe_adopt(void *raw)
    __attribute__((nothrow));
static inline objc3_heap_buffer objc3_heap_buffer_unsafe_adopt(void *raw) {
  return raw;
}

#endif
