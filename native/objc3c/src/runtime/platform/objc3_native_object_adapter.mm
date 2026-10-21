#include "runtime/platform/objc3_native_object_adapter.h"

#if defined(__APPLE__)
#include <Block.h>
#include <new>
#if !__has_feature(objc_arc)
#error "Compile the Apple runtime adapter with ARC enabled"
#endif
using ObjC3WeakStorage = id __weak;
static_assert(sizeof(ObjC3WeakStorage) <= sizeof(objc3_native_weak_slot),
              "opaque weak-slot storage is too small");
static_assert(alignof(objc3_native_weak_slot) >= alignof(ObjC3WeakStorage),
              "opaque weak-slot storage is under-aligned");
static ObjC3WeakStorage *WeakStorage(objc3_native_weak_slot *slot) {
  return reinterpret_cast<ObjC3WeakStorage *>(&slot->runtime_storage);
}
static void InitializeWeakStorage(objc3_native_weak_slot *slot, id value) {
  /* Placement construction lets ARC register a real weak location. */
  new (&slot->runtime_storage) ObjC3WeakStorage(value);
}
static void DestroyWeakStorage(objc3_native_weak_slot *slot) {
  WeakStorage(slot)->~ObjC3WeakStorage();
}
#elif defined(OBJC3_NATIVE_RUNTIME_GNUSTEP)
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <objc/blocks_runtime.h>
#if __has_feature(objc_arc)
#error "Compile the libobjc2 adapter with ARC disabled"
#endif
static_assert(sizeof(id) <= sizeof(objc3_native_weak_slot),
              "opaque weak-slot storage is too small");
static_assert(alignof(objc3_native_weak_slot) >= alignof(id),
              "opaque weak-slot storage is under-aligned");
static id *WeakStorage(objc3_native_weak_slot *slot) {
  return reinterpret_cast<id *>(&slot->runtime_storage);
}
#else
#error "Select the Apple Objective-C runtime or GNUstep-v2 libobjc2 explicitly"
#endif

extern "C" objc3_native_object_t
objc3_native_object_retain(objc3_native_object_t object) {
  if (object == nullptr) return nullptr;
#if defined(__APPLE__)
  id borrowed = (__bridge id)object;
  return (__bridge_retained void *)borrowed;
#else
  return objc_retain((id)object);
#endif
}

extern "C" void objc3_native_object_release(objc3_native_object_t object) {
  if (object != nullptr) {
#if defined(__APPLE__)
    (void)(__bridge_transfer id)object;
#else
    objc_release((id)object);
#endif
  }
}

extern "C" void objc3_native_weak_init(objc3_native_weak_slot *slot,
                                        objc3_native_object_t object) {
  if (slot != nullptr)
#if defined(__APPLE__)
    InitializeWeakStorage(slot, (__bridge id)object);
#else
    (void)objc_initWeak(WeakStorage(slot), (id)object);
#endif
}

extern "C" objc3_native_object_t
objc3_native_weak_load_retained(objc3_native_weak_slot *slot) {
  if (slot == nullptr) return nullptr;
#if defined(__APPLE__)
  /* The weak read initializes one strong owner; bridge-retained transfers it. */
  id __strong loaded = *WeakStorage(slot);
  if (loaded == nullptr) return nullptr;
  return (__bridge_retained void *)loaded;
#else
  return objc_loadWeakRetained(WeakStorage(slot));
#endif
}

extern "C" objc3_native_object_t
objc3_native_weak_store(objc3_native_weak_slot *slot,
                        objc3_native_object_t object) {
  if (slot == nullptr) return nullptr;
#if defined(__APPLE__)
  *WeakStorage(slot) = (__bridge id)object;
#else
  (void)objc_storeWeak(WeakStorage(slot), (id)object);
#endif
  return object;
}

extern "C" void objc3_native_weak_destroy(objc3_native_weak_slot *slot) {
  if (slot != nullptr) {
#if defined(__APPLE__)
    DestroyWeakStorage(slot);
#else
    objc_destroyWeak(WeakStorage(slot));
#endif
  }
}

extern "C" objc3_native_block_t
objc3_native_block_copy(objc3_native_block_t block) {
  return block == nullptr ? nullptr : _Block_copy(block);
}

extern "C" void objc3_native_block_release(objc3_native_block_t block) {
  if (block != nullptr) _Block_release(block);
}
