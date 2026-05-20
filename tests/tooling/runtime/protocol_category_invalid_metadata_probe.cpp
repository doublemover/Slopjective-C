#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"

#include <cstdint>
#include <iostream>
#include <string>

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
constexpr const char *kMissingCategoryModuleName =
    "protocol-category-missing-target-probe";
constexpr const char *kMissingCategoryClassName = "MissingOwner";
constexpr const char *kConflictingCategoryModuleName =
    "protocol-category-conflicting-owner-probe";
constexpr const char *kConflictingCategoryClassName = "ConflictOwner";
constexpr const char *kConflictingCategoryBundleOwner =
    "interface:ConflictOwner";
constexpr const char *kConflictingCategoryClassOwner = "class:ConflictOwner";
constexpr const char *kConflictingCategoryMetaclassBundleOwner =
    "metaclass:ConflictOwner";
constexpr const char *kConflictingCategoryMetaclassOwner =
    "metaclass-object:ConflictOwner";

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

struct MissingCategoryTargetImage {
  objc3_runtime_image_descriptor image{kMissingCategoryModuleName,
                                       kTranslationUnit, 1, 0, 0, 1, 0, 0};
  objc3c::runtime::EmittedCategoryRecord category_record{
      kMissingCategoryClassName,
      "Tracing",
      "implementation",
      "implementation:MissingOwner(Tracing)",
      "class:MissingOwner",
      "category:MissingOwner(Tracing)",
      nullptr,
      kEmptyRoot,
      nullptr,
      nullptr,
      0,
      0,
      0};
  PointerAggregateStorage<1> category_root_storage{1, {&category_record}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&kEmptyRootStorage, &kEmptyRootStorage, &category_root_storage,
       &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *category_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &category_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, kEmptyRoot,
      kEmptyRoot, category_root, kEmptyRoot, kEmptyRoot, nullptr, nullptr,
      nullptr, &image_local_init_state};
};

struct ConflictingCategoryOwnerImage {
  objc3_runtime_image_descriptor image{kConflictingCategoryModuleName,
                                       kTranslationUnit, 1, 1, 0, 2, 0, 0};
  objc3c::runtime::EmittedClassBundle class_bundle{
      {kConflictingCategoryClassName, kConflictingCategoryBundleOwner,
       kConflictingCategoryClassOwner, "", nullptr, nullptr, kEmptyRoot, false,
       false},
      {kConflictingCategoryClassName, kConflictingCategoryMetaclassBundleOwner,
       kConflictingCategoryMetaclassOwner, "", nullptr, nullptr, kEmptyRoot,
       false, false}};
  objc3c::runtime::EmittedCategoryRecord first_category_record{
      kConflictingCategoryClassName,
      "Tracing",
      "implementation",
      "implementation:ConflictOwner(Tracing)::first",
      kConflictingCategoryClassOwner,
      "category:ConflictOwner(Tracing)::first",
      nullptr,
      kEmptyRoot,
      nullptr,
      nullptr,
      0,
      0,
      0};
  objc3c::runtime::EmittedCategoryRecord second_category_record{
      kConflictingCategoryClassName,
      "Tracing",
      "implementation",
      "implementation:ConflictOwner(Tracing)::second",
      kConflictingCategoryClassOwner,
      "category:ConflictOwner(Tracing)::second",
      nullptr,
      kEmptyRoot,
      nullptr,
      nullptr,
      0,
      0,
      0};
  PointerAggregateStorage<1> class_root_storage{1, {&class_bundle}};
  PointerAggregateStorage<2> category_root_storage{
      2, {&first_category_record, &second_category_record}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&class_root_storage, &kEmptyRootStorage, &category_root_storage,
       &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *class_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &class_root_storage);
  const objc3_runtime_pointer_aggregate *category_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &category_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, class_root,
      kEmptyRoot, category_root, kEmptyRoot, kEmptyRoot, nullptr, nullptr,
      nullptr, &image_local_init_state};
};

struct ProbeResult {
  int registration_status = 0;
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot class_entry{};
  std::string malformed_reason;
};

struct ProbeRun {
  ProbeResult invalid_protocol_reference;
  ProbeResult missing_category_target;
  ProbeResult conflicting_category_owner;
};

ProbeResult CaptureInvalidRegistration(
    const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *registration_table,
    const char *class_name) {
  ProbeResult result;
  objc3_runtime_reset_for_testing();
  objc3_runtime_stage_registration_table_for_bootstrap(registration_table);
  result.registration_status = objc3_runtime_register_image(image);
  (void)objc3_runtime_copy_registration_state_for_testing(
      &result.registration_state);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &result.graph_state);
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      class_name, &result.class_entry);
  result.malformed_reason =
      result.graph_state.last_malformed_class_graph_reason != nullptr
          ? result.graph_state.last_malformed_class_graph_reason
          : "";
  return result;
}

ProbeRun RunProbe() {
  InvalidProtocolReferenceImage invalid_protocol_fixture;
  MissingCategoryTargetImage missing_category_fixture;
  ConflictingCategoryOwnerImage conflicting_category_fixture;
  ProbeRun run;
  run.invalid_protocol_reference = CaptureInvalidRegistration(
      &invalid_protocol_fixture.image,
      &invalid_protocol_fixture.registration_table, kClassName);
  run.missing_category_target = CaptureInvalidRegistration(
      &missing_category_fixture.image,
      &missing_category_fixture.registration_table, kMissingCategoryClassName);
  run.conflicting_category_owner = CaptureInvalidRegistration(
      &conflicting_category_fixture.image,
      &conflicting_category_fixture.registration_table,
      kConflictingCategoryClassName);
  return run;
}

void WriteProbeResultFields(
    std::ostream &out,
    objc3c::runtime::probe::JsonFieldSeparator &separator,
    const ProbeResult &result) {
  using objc3c::runtime::probe::WriteJsonIntField;
  using objc3c::runtime::probe::WriteJsonStringField;
  using objc3c::runtime::probe::WriteJsonUInt64Field;

  WriteJsonIntField(out, separator, "registration_status",
                    result.registration_status);
  WriteJsonIntField(out, separator, "last_registration_status",
                    result.registration_state.last_registration_status);
  WriteJsonUInt64Field(
      out, separator, "registered_image_count",
      static_cast<unsigned long long>(
          result.registration_state.registered_image_count));
  WriteJsonUInt64Field(
      out, separator, "realized_class_count",
      static_cast<unsigned long long>(result.graph_state.realized_class_count));
  WriteJsonUInt64Field(
      out, separator, "malformed_class_metadata_rejection_count",
      static_cast<unsigned long long>(
          result.graph_state.malformed_class_metadata_rejection_count));
  WriteJsonIntField(out, separator, "class_found",
                    result.class_entry.found);
  const char *malformed_reason =
      result.malformed_reason.empty()
          ? result.graph_state.last_malformed_class_graph_reason
          : result.malformed_reason.c_str();
  WriteJsonStringField(out, separator,
                       "last_malformed_class_graph_reason",
                       malformed_reason);
}

void WriteProbeResultObject(std::ostream &out, const ProbeResult &result) {
  objc3c::runtime::probe::JsonFieldSeparator separator;
  out << "{";
  WriteProbeResultFields(out, separator, result);
  out << "}";
}

void PrintProbeResult(const ProbeRun &run) {
  using objc3c::runtime::probe::JsonFieldSeparator;
  using objc3c::runtime::probe::WriteJsonFieldName;

  JsonFieldSeparator separator;
  std::cout << "{";
  WriteProbeResultFields(std::cout, separator,
                         run.invalid_protocol_reference);
  separator.BeforeField(std::cout);
  WriteJsonFieldName(std::cout, "missing_category_target");
  WriteProbeResultObject(std::cout, run.missing_category_target);
  separator.BeforeField(std::cout);
  WriteJsonFieldName(std::cout, "conflicting_category_owner");
  WriteProbeResultObject(std::cout, run.conflicting_category_owner);
  std::cout << "}";
}

}  // namespace

int main() {
  PrintProbeResult(RunProbe());
  return 0;
}
