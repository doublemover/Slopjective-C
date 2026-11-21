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

  objc3_keypath_component_v2 nullable_components[] = {
      {"Root *", "Root", "child", "Child *", "child", "setChild:",
       OBJC3_KEYPATH_COMPONENT_NULLABLE | OBJC3_KEYPATH_COMPONENT_RETAINABLE, 0},
      {"Child *", "Child", "value", "Value *", "value", "setValue:",
       OBJC3_KEYPATH_COMPONENT_NULLABLE | OBJC3_KEYPATH_COMPONENT_RETAINABLE, 0}};
  objc3_keypath_descriptor_v2 nullable_path{
      OBJC3_KEYPATH_ABI_VERSION,
      OBJC3_KEYPATH_NULLABLE_TRAVERSAL | OBJC3_KEYPATH_KVC_COMPATIBLE |
          OBJC3_KEYPATH_WRITABLE | OBJC3_KEYPATH_NULLABLE_WRITES,
      2, "Root *", "Value *", "child.value", nullable_components};
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_OK);
  objc3_keypath_component_v2 owned_components[] = {
      nullable_components[0], nullable_components[1]};
  auto owned_getter = nullable_path;
  owned_getter.components = owned_components;
  owned_components[1].flags |= OBJC3_KEYPATH_COMPONENT_GETTER_RETURNS_RETAINED;
  assert(objc3_keypath_equal_v2(&nullable_path, &owned_getter, &equal) == 0 &&
         equal == 0);
  owned_components[1].flags &= ~OBJC3_KEYPATH_COMPONENT_RETAINABLE;
  assert(objc3_keypath_validate_v2(&owned_getter) ==
         OBJC3_KEYPATH_INVALID_METADATA);
  const auto nullable_flags = nullable_path.flags;
  nullable_path.flags &= ~OBJC3_KEYPATH_NULLABLE_WRITES;
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_INVALID_METADATA);
  // Old forced-readonly metadata must be regenerated at this cutover.
  nullable_path.flags &= ~OBJC3_KEYPATH_WRITABLE;
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_INVALID_METADATA);
  nullable_path.flags = nullable_flags;
  nullable_components[0].flags |= OBJC3_KEYPATH_COMPONENT_READONLY;
  nullable_components[0].setter_selector = "";
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_INVALID_METADATA);
  nullable_path.flags &= ~(OBJC3_KEYPATH_WRITABLE | OBJC3_KEYPATH_NULLABLE_WRITES);
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_OK);
  nullable_components[0].flags &= ~OBJC3_KEYPATH_COMPONENT_READONLY;
  nullable_components[0].setter_selector = "setChild:";
  nullable_path.flags = nullable_flags;
  auto stray_write = descriptor;
  stray_write.flags |= OBJC3_KEYPATH_NULLABLE_WRITES;
  assert(objc3_keypath_validate_v2(&stray_write) == OBJC3_KEYPATH_INVALID_METADATA);
  nullable_components[1].flags &= ~OBJC3_KEYPATH_COMPONENT_RETAINABLE;
  // Null is also representable by an explicitly nullable native pointer.
  nullable_components[1].value_type = "void *";
  nullable_path.value_type = "void *";
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_OK);
  nullable_components[1].value_type = "Value *";
  nullable_path.value_type = "Value *";
  nullable_components[1].flags |= OBJC3_KEYPATH_COMPONENT_RETAINABLE;
  nullable_components[1].flags &= ~OBJC3_KEYPATH_COMPONENT_NULLABLE;
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_INVALID_METADATA);
  nullable_path.flags |= OBJC3_KEYPATH_CHECKED_MISSING_READ;
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_OK);
  // Scalar and aggregate paths use the same checked missing-route contract;
  // the runtime never fabricates a zero/default object for them.
  const char *checked_types[] = {"int", "struct Pair"};
  for (const char *type : checked_types) {
    nullable_components[1].flags = 0;
    nullable_components[1].value_type = type;
    nullable_path.value_type = type;
    assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_OK);
  }
  nullable_components[1].value_type = "Value *";
  nullable_path.value_type = "Value *";
  nullable_components[1].flags = OBJC3_KEYPATH_COMPONENT_RETAINABLE;
  nullable_components[1].flags |= OBJC3_KEYPATH_COMPONENT_NULLABLE;
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_INVALID_METADATA);
  nullable_path.flags &= ~OBJC3_KEYPATH_CHECKED_MISSING_READ;
  nullable_components[1].flags |= OBJC3_KEYPATH_COMPONENT_NULLABLE;
  nullable_path.flags &= ~OBJC3_KEYPATH_NULLABLE_TRAVERSAL;
  assert(objc3_keypath_validate_v2(&nullable_path) == OBJC3_KEYPATH_INVALID_METADATA);
  auto stray_checked = descriptor;
  stray_checked.flags |= OBJC3_KEYPATH_CHECKED_MISSING_READ;
  assert(objc3_keypath_validate_v2(&stray_checked) == OBJC3_KEYPATH_INVALID_METADATA);

  const objc3_keypath_component_v2 fields[] = {
      {"Root *", "Root", "pair", "struct Pair", "pair", "setPair:", 0, 0},
      {"struct Pair", "struct Pair", "first", "long", "", "",
       OBJC3_KEYPATH_COMPONENT_RECORD_FIELD, 0}};
  objc3_keypath_descriptor_v2 field_path{
      OBJC3_KEYPATH_ABI_VERSION, OBJC3_KEYPATH_WRITABLE, 2,
      "Root *", "long", "pair.first", fields};
  assert(objc3_keypath_validate_v2(&field_path) == OBJC3_KEYPATH_OK);
  auto impossible_root_field = field_path;
  impossible_root_field.component_count = 1;
  impossible_root_field.components = fields + 1;
  impossible_root_field.root_type = "struct Pair";
  impossible_root_field.path = "first";
  assert(objc3_keypath_validate_v2(&impossible_root_field) ==
         OBJC3_KEYPATH_INVALID_METADATA);
  field_path.flags |= OBJC3_KEYPATH_KVC_COMPATIBLE;
  assert(objc3_keypath_validate_v2(&field_path) == OBJC3_KEYPATH_INVALID_METADATA);
}
