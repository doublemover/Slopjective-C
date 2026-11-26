#ifndef OBJC3_DARWIN_H
#define OBJC3_DARWIN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct class_record objc3_darwin_class;

const objc3_darwin_class *objc3_darwin_class_v1(const char *name);
const objc3_darwin_class *objc3_darwin_object_class_v1(const void *object);
const objc3_darwin_class *objc3_darwin_superclass_v1(const objc3_darwin_class *cls);
const char *objc3_darwin_class_name_v1(const objc3_darwin_class *cls);
size_t objc3_darwin_instance_size_v1(const objc3_darwin_class *cls);

/* Returns a zero-initialized instance with one owning reference. Under ARC,
 * transfer the result into an Objective-C reference with __bridge_transfer. */
void *objc3_darwin_allocate_v1(const objc3_darwin_class *cls);
void objc3_darwin_deallocate_v1(void *instance);

#ifdef __cplusplus
}
#endif

#endif
