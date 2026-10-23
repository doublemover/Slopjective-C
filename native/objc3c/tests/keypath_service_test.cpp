#include "runtime/public/objc3_keypath.h"

#include <cassert>
#include <cstdint>

int main() {
  const objc3_keypath_component_v2 components[] = {
      {"Root *", "Root", "child", "Child *", "child", "setChild:",
       OBJC3_KEYPATH_COMPONENT_STRONG | OBJC3_KEYPATH_COMPONENT_NONNULL, 0},
      {"Child *", "Child", "count", "int", "readCount", "writeCount:", 0, 0}};
  objc3_keypath_descriptor_v2 descriptor{
      OBJC3_KEYPATH_ABI_VERSION,
      OBJC3_KEYPATH_WRITABLE | OBJC3_KEYPATH_KVC_COMPATIBLE,
      2, "Root *", "int", "child.count", components};
  assert(objc3_keypath_validate_v2(&descriptor) == OBJC3_KEYPATH_OK);
  const objc3_keypath_component_v2 *component = nullptr;
  assert(objc3_keypath_component_v2_at(&descriptor, 1, &component) == 0);
  assert(component == components + 1);
  assert(objc3_keypath_component_v2_at(&descriptor, UINT64_MAX, &component) ==
         OBJC3_KEYPATH_OUT_OF_BOUNDS && !component);
  assert(objc3_keypath_component_v2_at(&descriptor, 0, nullptr) ==
         OBJC3_KEYPATH_INVALID_ARGUMENT);

  // Descriptor identity compares component/type/selector contracts rather than
  // addresses. Separate images may emit equivalent immutable descriptions.
  objc3_keypath_component_v2 other_components[] = {components[0], components[1]};
  auto other = descriptor;
  other.components = other_components;
  int equal = -1;
  assert(objc3_keypath_equal_v2(&descriptor, &other, &equal) == 0 && equal == 1);
  other_components[1].getter_selector = "otherCount";
  assert(objc3_keypath_equal_v2(&descriptor, &other, &equal) == 0 && equal == 0);
  other_components[1] = components[1];
  other_components[1].declaration_owner = "Child(Override)";
  assert(objc3_keypath_equal_v2(&descriptor, &other, &equal) == 0 && equal == 0);
  other_components[1] = components[1];

  other_components[0].flags |= OBJC3_KEYPATH_COMPONENT_NULLABLE;
  assert(objc3_keypath_validate_v2(&other) == OBJC3_KEYPATH_INVALID_METADATA);
  other_components[0] = components[0];
  other_components[1].owner_type = nullptr;
  assert(objc3_keypath_validate_v2(&other) == OBJC3_KEYPATH_INVALID_METADATA);
  other_components[1] = components[1];
  other_components[1].reserved = 1;
  assert(objc3_keypath_validate_v2(&other) == OBJC3_KEYPATH_INVALID_METADATA);
  other_components[1] = components[1];
  other.path = "child.counter";
  assert(objc3_keypath_validate_v2(&other) == OBJC3_KEYPATH_INVALID_METADATA);
  other = descriptor;
  other.abi_version = 999;
  assert(objc3_keypath_equal_v2(&descriptor, &other, &equal) ==
         OBJC3_KEYPATH_INCOMPATIBLE_ABI && equal == 0);
  assert(objc3_keypath_validate_v2(nullptr) == OBJC3_KEYPATH_INVALID_ARGUMENT);
}
