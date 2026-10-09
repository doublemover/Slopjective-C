#ifndef OBJC3_NATIVE_OBJECT_ADAPTER_H
#define OBJC3_NATIVE_OBJECT_ADAPTER_H

/* C ABI for real native Objective-C objects and Blocks, never integer handles. */
#ifdef __cplusplus
extern "C" {
#endif
typedef void *objc3_native_object_t;
typedef void *objc3_native_block_t;
typedef struct objc3_native_weak_slot { void *runtime_storage; }
    objc3_native_weak_slot;

/* retain returns +1; release consumes one +1. Null is accepted. */
objc3_native_object_t objc3_native_object_retain(objc3_native_object_t object);
void objc3_native_object_release(objc3_native_object_t object);

/* Weak slots are opaque, noncopyable runtime locations; destroy once. */
void objc3_native_weak_init(objc3_native_weak_slot *slot,
                            objc3_native_object_t object);
objc3_native_object_t objc3_native_weak_load_retained(
    objc3_native_weak_slot *slot); /* returns +1 or null */
objc3_native_object_t objc3_native_weak_store(
    objc3_native_weak_slot *slot, objc3_native_object_t object); /* borrowed */
void objc3_native_weak_destroy(objc3_native_weak_slot *slot);

objc3_native_block_t objc3_native_block_copy(objc3_native_block_t block); /* +1 */
void objc3_native_block_release(objc3_native_block_t block); /* consumes +1 */
#ifdef __cplusplus
}
#endif
#endif
