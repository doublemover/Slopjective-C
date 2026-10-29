#ifndef OBJC3_RUNTIME_KEYPATH_H
#define OBJC3_RUNTIME_KEYPATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Immutable compiler-emitted metadata. Strings and component storage have
 * image lifetime; a key path never owns or captures an application root. */
#define OBJC3_KEYPATH_ABI_VERSION 2u
#define OBJC3_KEYPATH_CAPABILITY_ID "objc3.cap.keypath"

enum objc3_keypath_flags_v2 {
  OBJC3_KEYPATH_WRITABLE = 1u << 0,
  OBJC3_KEYPATH_KVC_COMPATIBLE = 1u << 1,
  /* A missing intermediate returns nil; nullable writes make no update. */
  OBJC3_KEYPATH_NULLABLE_TRAVERSAL = 1u << 2,
  OBJC3_KEYPATH_NULLABLE_WRITES = 1u << 3
};
enum objc3_keypath_component_flags_v2 {
  OBJC3_KEYPATH_COMPONENT_READONLY = 1u << 0,
  OBJC3_KEYPATH_COMPONENT_NULLABLE = 1u << 1,
  OBJC3_KEYPATH_COMPONENT_WEAK = 1u << 2,
  OBJC3_KEYPATH_COMPONENT_COPY = 1u << 3,
  OBJC3_KEYPATH_COMPONENT_STRONG = 1u << 4,
  OBJC3_KEYPATH_COMPONENT_ATOMIC = 1u << 5,
  OBJC3_KEYPATH_COMPONENT_NONNULL = 1u << 6,
  OBJC3_KEYPATH_COMPONENT_RECORD_FIELD = 1u << 7,
  OBJC3_KEYPATH_COMPONENT_RETAINABLE = 1u << 8,
  OBJC3_KEYPATH_COMPONENT_GETTER_RETURNS_RETAINED = 1u << 9
};

typedef struct objc3_keypath_component_v2 {
  const char *owner_type;
  const char *declaration_owner;
  const char *member;
  const char *value_type;
  const char *getter_selector; /* Empty for a native record field. */
  const char *setter_selector; /* Empty for readonly properties/record fields. */
  uint32_t flags;
  uint32_t reserved;
} objc3_keypath_component_v2;

typedef struct objc3_keypath_descriptor_v2 {
  uint32_t abi_version;
  uint32_t flags;
  uint64_t component_count;
  const char *root_type;
  const char *value_type;
  const char *path;
  const objc3_keypath_component_v2 *components;
} objc3_keypath_descriptor_v2;

enum objc3_keypath_status_v2 {
  OBJC3_KEYPATH_OK = 0,
  OBJC3_KEYPATH_INVALID_ARGUMENT = 1,
  OBJC3_KEYPATH_INCOMPATIBLE_ABI = 2,
  OBJC3_KEYPATH_INVALID_METADATA = 3,
  OBJC3_KEYPATH_OUT_OF_BOUNDS = 4
};

int objc3_keypath_validate_v2(const objc3_keypath_descriptor_v2 *descriptor);
int objc3_keypath_component_v2_at(const objc3_keypath_descriptor_v2 *descriptor,
                                uint64_t index,
                                const objc3_keypath_component_v2 **out_component);
/* Structural identity survives different literal addresses and image imports. */
int objc3_keypath_equal_v2(const objc3_keypath_descriptor_v2 *left,
                          const objc3_keypath_descriptor_v2 *right,
                          int *out_equal);

#ifdef __OBJC__
/* Explicit property-only KVC boxing. Returns an autoreleased NSString when
 * Foundation is present; nil for invalid/non-KVC metadata or an absent factory.
 * Typed key-path access remains independent of Foundation. */
id objc3_keypath_kvc_string(const objc3_keypath_descriptor_v2 *descriptor);
#endif

#ifdef __cplusplus
}
#endif
#endif
