#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"

#include <cstdint>
#include <iostream>

namespace {

template <std::uint64_t EntryCount>
struct PointerAggregateStorage {
  std::uint64_t count;
  const void *entries[EntryCount];
};

constexpr const char *kModuleName = "protocol-category-invalid-metadata-probe";
constexpr const char *kTranslationUnit =
    "tests/tooling/runtime/protocol_category_invalid_metadata_probe.cpp";
constexpr const char *kClassName = "BrokenProtocolRef";
constexpr const char *kClassBundleOwner = "interface:BrokenProtocolRef";
constexpr const char *kClassOwner = "class:BrokenProtocolRef";
constexpr const char *kMetaclassBundleOwner = "metaclass:BrokenProtocolRef";
constexpr const char *kMetaclassOwner = "metaclass-object:BrokenProtocolRef";

const PointerAggregateStorage<1> kEmptyRootStorage = {0, {nullptr}};
const objc3_runtime_pointer_aggregate *kEmptyRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kEmptyRootStorage);

struct InvalidProtocolReferenceImage {
  objc3_runtime_image_descriptor image{kModuleName, kTranslationUnit, 1, 1, 0,
                                       0, 0, 0};
  objc3c::runtime::EmittedProtocolRecord unregistered_protocol{
      "UnregisteredProtocol", "protocol:UnregisteredProtocol", kEmptyRoot,
      nullptr, nullptr, 0, 0, 0, 0, false};
  PointerAggregateStorage<1> adopted_protocol_refs{
      1, {&unregistered_protocol}};
  const objc3_runtime_pointer_aggregate *adopted_protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &adopted_protocol_refs);
  objc3c::runtime::EmittedClassBundle class_bundle{
      {kClassName, kClassBundleOwner, kClassOwner, "", nullptr, nullptr,
       adopted_protocol_root, false, false},
      {kClassName, kMetaclassBundleOwner, kMetaclassOwner, "", nullptr,
       nullptr, kEmptyRoot, false, false}};
  PointerAggregateStorage<1> class_root_storage{1, {&class_bundle}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&class_root_storage, &kEmptyRootStorage, &kEmptyRootStorage,
       &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *class_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &class_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, class_root,
      kEmptyRoot, kEmptyRoot, kEmptyRoot, kEmptyRoot, nullptr, nullptr,
      nullptr, &image_local_init_state};
};

struct ProbeResult {
  int registration_status = 0;
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot class_entry{};
};

ProbeResult RunProbe() {
  InvalidProtocolReferenceImage fixture;
  ProbeResult result;
  objc3_runtime_reset_for_testing();
  objc3_runtime_stage_registration_table_for_bootstrap(
      &fixture.registration_table);
  result.registration_status = objc3_runtime_register_image(&fixture.image);
  (void)objc3_runtime_copy_registration_state_for_testing(
      &result.registration_state);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &result.graph_state);
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kClassName, &result.class_entry);
  return result;
}

void PrintProbeResult(const ProbeResult &result) {
  using objc3c::runtime::probe::JsonFieldSeparator;
  using objc3c::runtime::probe::WriteJsonIntField;
  using objc3c::runtime::probe::WriteJsonStringField;
  using objc3c::runtime::probe::WriteJsonUInt64Field;

  JsonFieldSeparator separator;
  std::cout << "{";
  WriteJsonIntField(std::cout, separator, "registration_status",
                    result.registration_status);
  WriteJsonIntField(std::cout, separator, "last_registration_status",
                    result.registration_state.last_registration_status);
  WriteJsonUInt64Field(
      std::cout, separator, "registered_image_count",
      static_cast<unsigned long long>(
          result.registration_state.registered_image_count));
  WriteJsonUInt64Field(
      std::cout, separator, "realized_class_count",
      static_cast<unsigned long long>(result.graph_state.realized_class_count));
  WriteJsonUInt64Field(
      std::cout, separator, "malformed_class_metadata_rejection_count",
      static_cast<unsigned long long>(
          result.graph_state.malformed_class_metadata_rejection_count));
  WriteJsonIntField(std::cout, separator, "class_found",
                    result.class_entry.found);
  WriteJsonStringField(std::cout, separator,
                       "last_malformed_class_graph_reason",
                       result.graph_state.last_malformed_class_graph_reason);
  std::cout << "}";
}

}  // namespace

int main() {
  PrintProbeResult(RunProbe());
  return 0;
}
