#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/storage/property_layout_rules.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"

#include <cstdint>
#include <cstdio>

namespace {

template <std::uint64_t EntryCount>
struct PointerAggregateStorage {
  std::uint64_t count;
  const void *entries[EntryCount];
};

constexpr const char *kModuleName = "property-ivar-invalid-layout-probe";
constexpr const char *kTranslationUnit =
    "tests/tooling/runtime/property_ivar_invalid_layout_probe.cpp";
constexpr const char *kClassName = "InvalidLayoutWidget";
constexpr const char *kOwnerIdentity = "interface:InvalidLayoutWidget";
constexpr const char *kClassOwnerIdentity = "class:InvalidLayoutWidget";
constexpr const char *kMetaclassOwnerIdentity =
    "metaclass:InvalidLayoutWidget";
constexpr const char *kPropertyName = "malformed";
constexpr const char *kGetterSelector = "malformed";
constexpr const char *kSetterSelector = "setMalformed:";
constexpr const char *kBindingSymbol =
    "_OBJC3_IVAR_InvalidLayoutWidget_malformed";
constexpr const char *kLayoutSymbol =
    "_OBJC3_IVAR_LAYOUT_InvalidLayoutWidget_malformed";
constexpr const char *kLayoutReplayKey =
    "InvalidLayoutWidget.malformed.layout";

const PointerAggregateStorage<1> kEmptyRootStorage = {0, {nullptr}};
const objc3_runtime_pointer_aggregate *kEmptyRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kEmptyRootStorage);

struct InvalidLayoutImage {
  objc3_runtime_image_descriptor image{kModuleName, kTranslationUnit, 1, 1, 0,
                                       0, 1, 1};
  objc3c::runtime::EmittedClassBundle class_bundle{
      {kClassName, kOwnerIdentity, kClassOwnerIdentity, "", nullptr, nullptr,
       kEmptyRoot, false, false},
      {kClassName, "metaclass-owner:InvalidLayoutWidget",
       kMetaclassOwnerIdentity, "", nullptr, nullptr, kEmptyRoot, false,
       false}};
  objc3c::runtime::EmittedPropertyDescriptor property{
      kPropertyName,
      "i32",
      kOwnerIdentity,
      kOwnerIdentity,
      kClassOwnerIdentity,
      kGetterSelector,
      kSetterSelector,
      kGetterSelector,
      kSetterSelector,
      kBindingSymbol,
      kBindingSymbol,
      kLayoutSymbol,
      "attributes=nonatomic,assign;nonatomic=1;assign=1",
      "lifetime=assign",
      "runtime_hook=none",
      "accessor_ownership=trivial",
      nullptr,
      nullptr,
      kLayoutReplayKey,
      0,
      4,
      4,
      0,
      0,
      0,
      0,
      4,
      0,
      0,
      true,
      true,
      true,
      true};
  objc3c::runtime::EmittedIvarLayoutRecord layout_record{
      kLayoutSymbol, kLayoutReplayKey, 0, 8, 4, 4, 0, 0, 0, 4, 0, 0, true};
  std::uint64_t offset_global = 0;
  objc3c::runtime::EmittedIvarDescriptor ivar{
      kOwnerIdentity, kOwnerIdentity, kClassOwnerIdentity, kOwnerIdentity,
      kPropertyName, kBindingSymbol, &layout_record, &offset_global,
      kLayoutReplayKey, 0, 0, 4, 4, 0, 0, 0, 4, 0, 0, true};
  PointerAggregateStorage<1> class_root_storage{1, {&class_bundle}};
  PointerAggregateStorage<1> property_root_storage{1, {&property}};
  PointerAggregateStorage<1> ivar_root_storage{1, {&ivar}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&class_root_storage, &kEmptyRootStorage, &kEmptyRootStorage,
       &property_root_storage, &ivar_root_storage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *class_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &class_root_storage);
  const objc3_runtime_pointer_aggregate *property_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &property_root_storage);
  const objc3_runtime_pointer_aggregate *ivar_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &ivar_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, class_root,
      kEmptyRoot, kEmptyRoot, property_root, ivar_root, nullptr, nullptr,
      nullptr, &image_local_init_state};
};

struct ProbeResult {
  int registration_status = 0;
  int strict_published_layout = 0;
  objc3_runtime_image_walk_state_snapshot image_walk{};
  objc3_runtime_realized_class_entry_snapshot class_entry{};
  objc3_runtime_property_entry_snapshot property_entry{};
  objc3_runtime_property_registry_state_snapshot registry_state{};
};

ProbeResult RunProbe() {
  InvalidLayoutImage fixture;
  ProbeResult result;
  objc3_runtime_reset_for_testing();
  result.strict_published_layout =
      objc3c::runtime::RuntimePropertyIvarDescriptorHasStrictPublishedLayout(
          fixture.ivar, fixture.offset_global, fixture.ivar.alignment_bytes)
          ? 1
          : 0;
  objc3_runtime_stage_registration_table_for_bootstrap(
      &fixture.registration_table);
  result.registration_status = objc3_runtime_register_image(&fixture.image);
  (void)objc3_runtime_copy_image_walk_state_for_testing(&result.image_walk);
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kClassName, &result.class_entry);
  (void)objc3_runtime_copy_property_entry_for_testing(
      kClassName, kPropertyName, &result.property_entry);
  (void)objc3_runtime_copy_property_registry_state_for_testing(
      &result.registry_state);
  return result;
}

void PrintProbeResult(const ProbeResult &result) {
  std::printf("{");
  objc3c::runtime::probe::PrintIntField("registration_status",
                                        result.registration_status);
  objc3c::runtime::probe::PrintIntField("strict_published_layout",
                                        result.strict_published_layout);
  std::printf("\"image_walk\":");
  std::printf("{");
  objc3c::runtime::probe::PrintUint64Field(
      "walked_image_count",
      static_cast<unsigned long long>(result.image_walk.walked_image_count));
  objc3c::runtime::probe::PrintUint64Field(
      "last_walked_class_descriptor_count",
      static_cast<unsigned long long>(
          result.image_walk.last_walked_class_descriptor_count));
  objc3c::runtime::probe::PrintUint64Field(
      "last_walked_property_descriptor_count",
      static_cast<unsigned long long>(
          result.image_walk.last_walked_property_descriptor_count));
  objc3c::runtime::probe::PrintUint64Field(
      "last_walked_ivar_descriptor_count",
      static_cast<unsigned long long>(
          result.image_walk.last_walked_ivar_descriptor_count));
  objc3c::runtime::probe::PrintIntField(
      "last_registration_used_staged_table",
      result.image_walk.last_registration_used_staged_table);
  objc3c::runtime::probe::PrintStringField(
      "last_walked_module_name", result.image_walk.last_walked_module_name,
      false);
  std::printf("}");
  std::printf(",\"class_entry\":");
  objc3c::runtime::probe::PrintRealizedClassEntryPropertySummary(
      result.class_entry);
  std::printf(",\"property_entry\":");
  objc3c::runtime::probe::PrintPropertyEntryFull(result.property_entry);
  std::printf(",\"registry_state\":");
  objc3c::runtime::probe::PrintPropertyRegistryStateFull(
      result.registry_state);
  std::printf("}");
}

}  // namespace

int main() {
  PrintProbeResult(RunProbe());
  return 0;
}
