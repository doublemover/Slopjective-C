#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_DISPATCH_FIXTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_DISPATCH_FIXTURE_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstddef>
#include <cstdint>

namespace objc3c::runtime::strict_dispatch_error_status_probe {

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

inline int ManualMethod0() {
  return 41;
}

inline int ManualMethod5(int, int, int, int, int) {
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

inline ManualImageCase MakeImageCase(const char *module_name,
                                     const char *identity_key,
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

inline bool RegisterCase(ManualImageCase &image_case) {
  image_case.image_local_init_state = 0;
  objc3_runtime_reset_for_testing();
  objc3_runtime_stage_registration_table_for_bootstrap(
      &image_case.registration_table);
  return objc3_runtime_register_image(&image_case.image) ==
         OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

}  // namespace objc3c::runtime::strict_dispatch_error_status_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_DISPATCH_FIXTURE_H_
