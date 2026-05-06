#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/dispatch_expectations.h"

#include <cstdint>

namespace {

using objc3c::runtime::probe::HasDispatchStatus;

template <std::size_t EntryCount>
struct PointerAggregateStorage {
  std::uint64_t count;
  const void *entries[EntryCount];
};

struct ManualMethodListRef {
  std::uint64_t count;
  const char *owner_identity;
  const void *method_list;
};

struct ManualMethodListHeader {
  std::uint64_t count;
  const char *declaration_owner_identity;
  const char *export_owner_identity;
};

struct ManualMethodListEntry {
  const char *selector;
  const char *owner_identity;
  const char *return_type_name;
  std::uint64_t parameter_count;
  const void *implementation;
  std::uint64_t has_body;
  bool effective_direct_dispatch;
  bool objc_final_declared;
};

struct ManualMethodListStorage {
  ManualMethodListHeader header;
  ManualMethodListEntry entry;
};

struct ManualClassRecord {
  const char *class_name;
  const char *bundle_owner_identity;
  const char *object_owner_identity;
  const char *super_owner_identity;
  const void *super_bundle;
  const ManualMethodListRef *method_list_ref;
  const objc3_runtime_pointer_aggregate *adopted_protocol_refs;
  bool objc_final_declared;
  bool objc_sealed_declared;
};

struct ManualClassBundle {
  ManualClassRecord class_record;
  ManualClassRecord metaclass_record;
};

int ManualMethod0() {
  return 41;
}

int ManualMethod5(int, int, int, int, int) {
  return 42;
}

const PointerAggregateStorage<1> kEmptyRootStorage = {0, {nullptr}};
const objc3_runtime_pointer_aggregate *kEmptyRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kEmptyRootStorage);

struct ManualImageCase {
  objc3_runtime_image_descriptor image;
  ManualMethodListStorage method_storage;
  ManualMethodListRef method_ref;
  ManualClassBundle class_bundle;
  PointerAggregateStorage<1> class_root_storage;
  PointerAggregateStorage<6> discovery_root_storage;
  const objc3_runtime_pointer_aggregate *class_root = nullptr;
  const objc3_runtime_pointer_aggregate *discovery_root = nullptr;
  const void *discovery_root_anchor = nullptr;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table;
};

ManualImageCase MakeImageCase(const char *module_name, const char *identity_key,
                              const char *selector,
                              const char *return_type_name,
                              std::uint64_t parameter_count,
                              const void *implementation,
                              std::uint64_t method_header_count) {
  ManualImageCase image_case{
      {module_name, identity_key, 1, 1, 0, 0, 0, 0},
      {{method_header_count, "interface:StrictDispatchCase",
        "implementation:StrictDispatchCase"},
       {selector, "implementation:StrictDispatchCase::class_method",
        return_type_name, parameter_count, implementation, 1, false, false}},
      {1, "implementation:StrictDispatchCase",
       &image_case.method_storage.header},
      {{"StrictDispatchCase", "interface:StrictDispatchCase",
        "class:StrictDispatchCase", "", nullptr, nullptr, kEmptyRoot, false,
        false},
       {"StrictDispatchCase", "metaclass:StrictDispatchCase",
        "metaclass-object:StrictDispatchCase", "", nullptr,
        &image_case.method_ref, kEmptyRoot, false, false}},
      {1, {&image_case.class_bundle}},
      {6,
       {&image_case.class_root_storage, &kEmptyRootStorage, &kEmptyRootStorage,
        &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}},
      nullptr,
      nullptr,
      nullptr,
      0,
      {2, 12, &image_case.image, nullptr, nullptr, nullptr, kEmptyRoot,
       kEmptyRoot, kEmptyRoot, kEmptyRoot, kEmptyRoot, nullptr, kEmptyRoot,
       nullptr}};
  image_case.class_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &image_case.class_root_storage);
  image_case.discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &image_case.discovery_root_storage);
  image_case.discovery_root_anchor = image_case.discovery_root;
  image_case.registration_table.discovery_root = image_case.discovery_root;
  image_case.registration_table.linker_anchor =
      &image_case.discovery_root_anchor;
  image_case.registration_table.class_descriptor_root = image_case.class_root;
  image_case.registration_table.image_local_init_state =
      &image_case.image_local_init_state;
  return image_case;
}

bool RegisterCase(ManualImageCase &image_case) {
  image_case.image_local_init_state = 0;
  objc3_runtime_reset_for_testing();
  objc3_runtime_stage_registration_table_for_bootstrap(
      &image_case.registration_table);
  return objc3_runtime_register_image(&image_case.image) ==
         OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

}  // namespace

int main() {
  ManualImageCase unsupported_return = MakeImageCase(
      "strict-dispatch-unsupported-return",
      "strict-dispatch::unsupported-return", "objectValue", "double", 0,
      reinterpret_cast<const void *>(&ManualMethod0), 1);
  if (!RegisterCase(unsupported_return)) {
    return 10;
  }
  const objc3_runtime_dispatch_i32_result unsupported_return_result =
      objc3_runtime_dispatch_i32_checked(1024, "objectValue", 0, 0, 0, 0);
  if (!HasDispatchStatus(
          unsupported_return_result,
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, 0, "O3RT005",
          "runtime dispatch failed: unsupported return type")) {
    return 11;
  }

  ManualImageCase unsupported_arguments = MakeImageCase(
      "strict-dispatch-unsupported-arguments",
      "strict-dispatch::unsupported-arguments", "tooMany:args:for:i32:path:",
      "i32", 5, reinterpret_cast<const void *>(&ManualMethod5), 1);
  if (!RegisterCase(unsupported_arguments)) {
    return 20;
  }
  const objc3_runtime_dispatch_i32_result unsupported_arguments_result =
      objc3_runtime_dispatch_i32_checked(
          1024, "tooMany:args:for:i32:path:", 1, 2, 3, 4);
  if (!HasDispatchStatus(
          unsupported_arguments_result,
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT, 0,
          "O3RT006",
          "runtime dispatch failed: unsupported argument layout")) {
    return 21;
  }

  ManualImageCase malformed_metadata = MakeImageCase(
      "strict-dispatch-malformed-metadata",
      "strict-dispatch::malformed-metadata", "malformedValue", "i32", 0,
      reinterpret_cast<const void *>(&ManualMethod0), 2);
  if (!RegisterCase(malformed_metadata)) {
    return 30;
  }
  const objc3_runtime_dispatch_i32_result malformed_metadata_result =
      objc3_runtime_dispatch_i32_checked(1024, "malformedValue", 0, 0, 0, 0);
  if (!HasDispatchStatus(
          malformed_metadata_result,
          OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, 0, "O3RT004",
          "runtime dispatch failed: malformed metadata")) {
    return 31;
  }

  return 0;
}
