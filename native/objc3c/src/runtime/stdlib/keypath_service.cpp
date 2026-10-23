#include "runtime/public/objc3_keypath.h"

#include <cstring>
#include <string_view>

namespace {
bool present(const char *text) { return text && *text; }
bool same(const char *a, const char *b) { return std::strcmp(a, b) == 0; }
} // namespace

extern "C" int objc3_keypath_validate_v2(
    const objc3_keypath_descriptor_v2 *descriptor) {
  if (!descriptor) return OBJC3_KEYPATH_INVALID_ARGUMENT;
  if (descriptor->abi_version != OBJC3_KEYPATH_ABI_VERSION)
    return OBJC3_KEYPATH_INCOMPATIBLE_ABI;
  if (!descriptor->component_count || !descriptor->components ||
      !present(descriptor->root_type) || !present(descriptor->value_type) ||
      !present(descriptor->path) ||
      (descriptor->flags & ~(OBJC3_KEYPATH_WRITABLE |
                             OBJC3_KEYPATH_KVC_COMPATIBLE)))
    return OBJC3_KEYPATH_INVALID_METADATA;
  std::string_view remaining(descriptor->path);
  bool writable = true;
  for (uint64_t i = 0; i < descriptor->component_count; ++i) {
    const auto &component = descriptor->components[i];
    if (!present(component.owner_type) || !present(component.declaration_owner) ||
        !present(component.member) ||
        !present(component.value_type) || !component.getter_selector ||
        !component.setter_selector || component.reserved ||
        (component.flags & ~((1u << 8) - 1)) ||
        ((component.flags & OBJC3_KEYPATH_COMPONENT_NULLABLE) &&
         (component.flags & OBJC3_KEYPATH_COMPONENT_NONNULL)))
      return OBJC3_KEYPATH_INVALID_METADATA;
    const bool readonly = component.flags & OBJC3_KEYPATH_COMPONENT_READONLY;
    const bool field = component.flags & OBJC3_KEYPATH_COMPONENT_RECORD_FIELD;
    if (field) {
      if (!i || *component.getter_selector || *component.setter_selector ||
          (descriptor->flags & OBJC3_KEYPATH_KVC_COMPATIBLE))
        return OBJC3_KEYPATH_INVALID_METADATA;
    } else if (!*component.getter_selector ||
               readonly != !*component.setter_selector) {
      return OBJC3_KEYPATH_INVALID_METADATA;
    }
    writable &= !readonly;
    if (i && !same(descriptor->components[i - 1].value_type,
                    component.owner_type))
      return OBJC3_KEYPATH_INVALID_METADATA;
    const std::string_view member(component.member);
    if (!remaining.starts_with(member)) return OBJC3_KEYPATH_INVALID_METADATA;
    remaining.remove_prefix(member.size());
    if (i + 1 < descriptor->component_count) {
      if (remaining.empty() || remaining.front() != '.' ||
          (component.flags & OBJC3_KEYPATH_COMPONENT_NULLABLE))
        return OBJC3_KEYPATH_INVALID_METADATA;
      remaining.remove_prefix(1);
    }
  }
  if (!remaining.empty() ||
      !same(descriptor->root_type, descriptor->components[0].owner_type) ||
      !same(descriptor->value_type,
            descriptor->components[descriptor->component_count - 1].value_type) ||
      (!!(descriptor->flags & OBJC3_KEYPATH_WRITABLE) != writable))
    return OBJC3_KEYPATH_INVALID_METADATA;
  return OBJC3_KEYPATH_OK;
}

extern "C" int objc3_keypath_component_v2_at(
    const objc3_keypath_descriptor_v2 *descriptor, uint64_t index,
    const objc3_keypath_component_v2 **out_component) {
  if (!out_component) return OBJC3_KEYPATH_INVALID_ARGUMENT;
  *out_component = nullptr;
  const int status = objc3_keypath_validate_v2(descriptor);
  if (status != OBJC3_KEYPATH_OK) return status;
  if (index >= descriptor->component_count) return OBJC3_KEYPATH_OUT_OF_BOUNDS;
  *out_component = &descriptor->components[index];
  return OBJC3_KEYPATH_OK;
}

extern "C" int objc3_keypath_equal_v2(
    const objc3_keypath_descriptor_v2 *left,
    const objc3_keypath_descriptor_v2 *right, int *out_equal) {
  if (!out_equal) return OBJC3_KEYPATH_INVALID_ARGUMENT;
  *out_equal = 0;
  int status = objc3_keypath_validate_v2(left);
  if (status != OBJC3_KEYPATH_OK) return status;
  status = objc3_keypath_validate_v2(right);
  if (status != OBJC3_KEYPATH_OK) return status;
  if (left->flags != right->flags ||
      left->component_count != right->component_count ||
      !same(left->root_type, right->root_type) ||
      !same(left->value_type, right->value_type) || !same(left->path, right->path))
    return OBJC3_KEYPATH_OK;
  for (uint64_t i = 0; i < left->component_count; ++i) {
    const auto &a = left->components[i], &b = right->components[i];
    if (a.flags != b.flags || !same(a.owner_type, b.owner_type) ||
        !same(a.declaration_owner, b.declaration_owner) ||
        !same(a.member, b.member) || !same(a.value_type, b.value_type) ||
        !same(a.getter_selector, b.getter_selector) ||
        !same(a.setter_selector, b.setter_selector))
      return OBJC3_KEYPATH_OK;
  }
  *out_equal = 1;
  return OBJC3_KEYPATH_OK;
}
