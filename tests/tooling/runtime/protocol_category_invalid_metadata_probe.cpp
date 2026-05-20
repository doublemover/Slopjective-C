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

template <std::uint64_t EntryCount>
struct MethodListStorage {
  objc3c::runtime::EmittedMethodListHeader header;
  objc3c::runtime::EmittedMethodListEntry entries[EntryCount];
};

constexpr const char *kModuleName = "protocol-category-invalid-metadata-probe";
constexpr const char *kTranslationUnit =
    "tests/tooling/runtime/protocol_category_invalid_metadata_probe.cpp";
constexpr const char *kClassName = "BrokenProtocolRef";
constexpr const char *kClassBundleOwner = "interface:BrokenProtocolRef";
constexpr const char *kClassOwner = "class:BrokenProtocolRef";
constexpr const char *kMetaclassBundleOwner = "metaclass:BrokenProtocolRef";
constexpr const char *kMetaclassOwner = "metaclass-object:BrokenProtocolRef";
constexpr const char *kForwardProtocolModuleName =
    "protocol-category-forward-protocol-reference-probe";
constexpr const char *kForwardProtocolClassName = "ForwardProtocolRef";
constexpr const char *kForwardProtocolBundleOwner =
    "interface:ForwardProtocolRef";
constexpr const char *kForwardProtocolClassOwner =
    "class:ForwardProtocolRef";
constexpr const char *kForwardProtocolMetaclassBundleOwner =
    "metaclass:ForwardProtocolRef";
constexpr const char *kForwardProtocolMetaclassOwner =
    "metaclass-object:ForwardProtocolRef";
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
constexpr const char *kMalformedMetadataDiagnosticCode = "O3RT004";
constexpr const char *kMalformedMetadataDiagnosticMessage =
    "runtime dispatch failed: malformed metadata";
constexpr const char *kMalformedMetadataDiagnosticClass =
    "malformed-runtime-metadata";
constexpr const char *kInvalidRegistrationStatusName =
    "OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS";
constexpr const char *kDuplicateRequirementModuleName =
    "protocol-duplicate-requirement-runtime-probe";
constexpr const char *kInheritedConflictModuleName =
    "protocol-inherited-requirement-conflict-runtime-probe";
constexpr const char *kAdoptedConflictModuleName =
    "protocol-adopted-requirement-conflict-runtime-probe";
constexpr const char *kAdoptedConflictClassName = "RequirementConflictAdopter";
constexpr const char *kAdoptedConflictBundleOwner =
    "interface:RequirementConflictAdopter";
constexpr const char *kAdoptedConflictClassOwner =
    "class:RequirementConflictAdopter";
constexpr const char *kAdoptedConflictMetaclassBundleOwner =
    "metaclass:RequirementConflictAdopter";
constexpr const char *kAdoptedConflictMetaclassOwner =
    "metaclass-object:RequirementConflictAdopter";
constexpr const char *kForwardInheritedModuleName =
    "protocol-forward-inherited-reference-runtime-probe";

const PointerAggregateStorage<1> kEmptyRootStorage = {0, {nullptr}};
const objc3_runtime_pointer_aggregate *kEmptyRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kEmptyRootStorage);

struct InvalidMetadataCaseDescriptor {
  const char *case_id;
  const char *source_path;
  const char *metadata_surface;
  const char *target_kind;
  const char *target_name;
  const char *category_name;
  const char *owner_identity;
  const char *visibility_state;
  const char *availability_state;
  const char *expected_reason;
};

constexpr InvalidMetadataCaseDescriptor kInvalidProtocolReferenceCase{
    "unregistered-protocol-reference",
    kTranslationUnit,
    "class.adopted_protocol_refs",
    "protocol",
    "UnregisteredProtocol",
    "",
    "protocol:UnregisteredProtocol",
    "unregistered",
    "unavailable",
    "unknown protocol reference in class BrokenProtocolRef"};

constexpr InvalidMetadataCaseDescriptor kForwardProtocolReferenceCase{
    "forward-protocol-reference",
    kTranslationUnit,
    "class.adopted_protocol_refs",
    "protocol",
    "ForwardOnly",
    "",
    "protocol:ForwardOnly",
    "forward-declaration",
    "unavailable-for-conformance",
    "forward protocol reference in class ForwardProtocolRef"};

constexpr InvalidMetadataCaseDescriptor kMissingCategoryTargetCase{
    "missing-category-target",
    kTranslationUnit,
    "category.target_class",
    "class",
    kMissingCategoryClassName,
    "Tracing",
    "category:MissingOwner(Tracing)",
    "absent",
    "missing-target",
    "category attachment target class is missing for MissingOwner(Tracing)"};

constexpr InvalidMetadataCaseDescriptor kConflictingCategoryOwnerCase{
    "conflicting-category-owner",
    kTranslationUnit,
    "category.owner_identity",
    "category",
    kConflictingCategoryClassName,
    "Tracing",
    "category:ConflictOwner(Tracing)::second",
    "duplicate-category",
    "conflicting-owner",
    "conflicting category implementation owner for ConflictOwner(Tracing)"};

constexpr InvalidMetadataCaseDescriptor kDuplicateProtocolRequirementCase{
    "duplicate-protocol-requirement",
    kTranslationUnit,
    "protocol.instance_method_list",
    "protocol",
    "Worker",
    "",
    "protocol:Worker",
    "available",
    "duplicate-requirement",
    "conflicting protocol instance method requirement work in protocol Worker"};

constexpr InvalidMetadataCaseDescriptor
    kInheritedProtocolRequirementConflictCase{
        "inherited-protocol-requirement-conflict",
        kTranslationUnit,
        "protocol.inherited_protocol_refs",
        "protocol",
        "MixedReadable",
        "",
        "protocol:MixedReadable",
        "available",
        "inherited-requirement-conflict",
        "conflicting protocol instance method requirement readValue in protocol "
        "MixedReadable"};

constexpr InvalidMetadataCaseDescriptor kAdoptedProtocolRequirementConflictCase{
    "adopted-protocol-requirement-conflict",
    kTranslationUnit,
    "class.adopted_protocol_refs",
    "class",
    kAdoptedConflictClassName,
    "",
    kAdoptedConflictClassOwner,
    "available",
    "adopted-requirement-conflict",
    "conflicting protocol instance method requirement readValue in class "
    "RequirementConflictAdopter"};

constexpr InvalidMetadataCaseDescriptor kForwardInheritedProtocolReferenceCase{
    "forward-inherited-protocol-reference",
    kTranslationUnit,
    "protocol.inherited_protocol_refs",
    "protocol",
    "ConcreteChild",
    "",
    "protocol:ForwardOnly",
    "forward-declaration",
    "unavailable-for-inheritance",
    "forward protocol reference in protocol ConcreteChild"};

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

struct ForwardProtocolReferenceImage {
  objc3_runtime_image_descriptor image{kForwardProtocolModuleName,
                                       kTranslationUnit, 1, 1, 1, 0, 0, 0};
  objc3c::runtime::EmittedProtocolRecord forward_protocol{
      "ForwardOnly", "protocol:ForwardOnly", kEmptyRoot, nullptr, nullptr, 0,
      0, 0, 0, true};
  PointerAggregateStorage<1> adopted_protocol_refs{1, {&forward_protocol}};
  const objc3_runtime_pointer_aggregate *adopted_protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &adopted_protocol_refs);
  objc3c::runtime::EmittedClassBundle class_bundle{
      {kForwardProtocolClassName, kForwardProtocolBundleOwner,
       kForwardProtocolClassOwner, "", nullptr, nullptr,
       adopted_protocol_root, false, false},
      {kForwardProtocolClassName, kForwardProtocolMetaclassBundleOwner,
       kForwardProtocolMetaclassOwner, "", nullptr, nullptr, kEmptyRoot,
       false, false}};
  PointerAggregateStorage<1> class_root_storage{1, {&class_bundle}};
  PointerAggregateStorage<1> protocol_root_storage{1, {&forward_protocol}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&class_root_storage, &protocol_root_storage, &kEmptyRootStorage,
       &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *class_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &class_root_storage);
  const objc3_runtime_pointer_aggregate *protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &protocol_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, class_root,
      protocol_root, kEmptyRoot, kEmptyRoot, kEmptyRoot, nullptr, nullptr,
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

struct DuplicateProtocolRequirementImage {
  objc3_runtime_image_descriptor image{kDuplicateRequirementModuleName,
                                       kTranslationUnit, 1, 0, 1, 0, 0, 0};
  MethodListStorage<2> worker_instance_methods{
      {2, "protocol:Worker", "protocol:Worker"},
      {{"work", "protocol:Worker::instance_method:work:i32", "i32", 0,
        nullptr, 0, false, false},
       {"work", "protocol:Worker::instance_method:work:bool", "bool", 0,
        nullptr, 0, false, false}}};
  objc3c::runtime::EmittedMethodListRef worker_instance_method_ref{
      2, "protocol:Worker", &worker_instance_methods};
  objc3c::runtime::EmittedProtocolRecord worker_protocol{
      "Worker", "protocol:Worker", kEmptyRoot, &worker_instance_method_ref,
      nullptr, 0, 2, 2, 0, false};
  PointerAggregateStorage<1> protocol_root_storage{1, {&worker_protocol}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&kEmptyRootStorage, &protocol_root_storage, &kEmptyRootStorage,
       &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &protocol_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, kEmptyRoot,
      protocol_root, kEmptyRoot, kEmptyRoot, kEmptyRoot, nullptr, nullptr,
      nullptr, &image_local_init_state};
};

struct InheritedProtocolRequirementConflictImage {
  objc3_runtime_image_descriptor image{kInheritedConflictModuleName,
                                       kTranslationUnit, 1, 0, 3, 0, 0, 0};
  MethodListStorage<1> text_instance_methods{
      {1, "protocol:TextReadable", "protocol:TextReadable"},
      {{"readValue", "protocol:TextReadable::instance_method:readValue",
        "id", 0, nullptr, 0, false, false}}};
  MethodListStorage<1> number_instance_methods{
      {1, "protocol:NumberReadable", "protocol:NumberReadable"},
      {{"readValue", "protocol:NumberReadable::instance_method:readValue",
        "i32", 0, nullptr, 0, false, false}}};
  objc3c::runtime::EmittedMethodListRef text_instance_method_ref{
      1, "protocol:TextReadable", &text_instance_methods};
  objc3c::runtime::EmittedMethodListRef number_instance_method_ref{
      1, "protocol:NumberReadable", &number_instance_methods};
  objc3c::runtime::EmittedProtocolRecord text_protocol{
      "TextReadable", "protocol:TextReadable", kEmptyRoot,
      &text_instance_method_ref, nullptr, 0, 1, 1, 0, false};
  objc3c::runtime::EmittedProtocolRecord number_protocol{
      "NumberReadable", "protocol:NumberReadable", kEmptyRoot,
      &number_instance_method_ref, nullptr, 0, 1, 1, 0, false};
  PointerAggregateStorage<2> mixed_inherited_refs{
      2, {&text_protocol, &number_protocol}};
  const objc3_runtime_pointer_aggregate *mixed_inherited_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &mixed_inherited_refs);
  objc3c::runtime::EmittedProtocolRecord mixed_protocol{
      "MixedReadable", "protocol:MixedReadable", mixed_inherited_root,
      nullptr, nullptr, 0, 0, 0, 0, false};
  PointerAggregateStorage<3> protocol_root_storage{
      3, {&text_protocol, &number_protocol, &mixed_protocol}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&kEmptyRootStorage, &protocol_root_storage, &kEmptyRootStorage,
       &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &protocol_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, kEmptyRoot,
      protocol_root, kEmptyRoot, kEmptyRoot, kEmptyRoot, nullptr, nullptr,
      nullptr, &image_local_init_state};
};

struct AdoptedProtocolRequirementConflictImage {
  objc3_runtime_image_descriptor image{kAdoptedConflictModuleName,
                                       kTranslationUnit, 1, 1, 2, 0, 0, 0};
  MethodListStorage<1> text_instance_methods{
      {1, "protocol:TextReadable", "protocol:TextReadable"},
      {{"readValue", "protocol:TextReadable::instance_method:readValue",
        "id", 0, nullptr, 0, false, false}}};
  MethodListStorage<1> number_instance_methods{
      {1, "protocol:NumberReadable", "protocol:NumberReadable"},
      {{"readValue", "protocol:NumberReadable::instance_method:readValue",
        "i32", 0, nullptr, 0, false, false}}};
  objc3c::runtime::EmittedMethodListRef text_instance_method_ref{
      1, "protocol:TextReadable", &text_instance_methods};
  objc3c::runtime::EmittedMethodListRef number_instance_method_ref{
      1, "protocol:NumberReadable", &number_instance_methods};
  objc3c::runtime::EmittedProtocolRecord text_protocol{
      "TextReadable", "protocol:TextReadable", kEmptyRoot,
      &text_instance_method_ref, nullptr, 0, 1, 1, 0, false};
  objc3c::runtime::EmittedProtocolRecord number_protocol{
      "NumberReadable", "protocol:NumberReadable", kEmptyRoot,
      &number_instance_method_ref, nullptr, 0, 1, 1, 0, false};
  PointerAggregateStorage<2> adopted_protocol_refs{
      2, {&text_protocol, &number_protocol}};
  const objc3_runtime_pointer_aggregate *adopted_protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &adopted_protocol_refs);
  objc3c::runtime::EmittedClassBundle class_bundle{
      {kAdoptedConflictClassName, kAdoptedConflictBundleOwner,
       kAdoptedConflictClassOwner, "", nullptr, nullptr,
       adopted_protocol_root, false, false},
      {kAdoptedConflictClassName, kAdoptedConflictMetaclassBundleOwner,
       kAdoptedConflictMetaclassOwner, "", nullptr, nullptr, kEmptyRoot,
       false, false}};
  PointerAggregateStorage<1> class_root_storage{1, {&class_bundle}};
  PointerAggregateStorage<2> protocol_root_storage{
      2, {&text_protocol, &number_protocol}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&class_root_storage, &protocol_root_storage, &kEmptyRootStorage,
       &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *class_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &class_root_storage);
  const objc3_runtime_pointer_aggregate *protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &protocol_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, class_root,
      protocol_root, kEmptyRoot, kEmptyRoot, kEmptyRoot, nullptr, nullptr,
      nullptr, &image_local_init_state};
};

struct ForwardInheritedProtocolReferenceImage {
  objc3_runtime_image_descriptor image{kForwardInheritedModuleName,
                                       kTranslationUnit, 1, 0, 2, 0, 0, 0};
  objc3c::runtime::EmittedProtocolRecord forward_protocol{
      "ForwardOnly", "protocol:ForwardOnly", kEmptyRoot, nullptr, nullptr, 0,
      0, 0, 0, true};
  PointerAggregateStorage<1> inherited_refs{1, {&forward_protocol}};
  const objc3_runtime_pointer_aggregate *inherited_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &inherited_refs);
  objc3c::runtime::EmittedProtocolRecord concrete_protocol{
      "ConcreteChild", "protocol:ConcreteChild", inherited_root, nullptr,
      nullptr, 0, 0, 0, 0, false};
  PointerAggregateStorage<2> protocol_root_storage{
      2, {&forward_protocol, &concrete_protocol}};
  PointerAggregateStorage<6> discovery_root_storage{
      6,
      {&kEmptyRootStorage, &protocol_root_storage, &kEmptyRootStorage,
       &kEmptyRootStorage, &kEmptyRootStorage, &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &protocol_root_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{
      2, 12, &image, discovery_root, &discovery_root_anchor, kEmptyRoot,
      protocol_root, kEmptyRoot, kEmptyRoot, kEmptyRoot, nullptr, nullptr,
      nullptr, &image_local_init_state};
};

struct ProbeResult {
  const InvalidMetadataCaseDescriptor *descriptor = nullptr;
  int registration_status = 0;
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot class_entry{};
  std::string malformed_reason;
};

struct ProbeRun {
  ProbeResult invalid_protocol_reference;
  ProbeResult forward_protocol_reference;
  ProbeResult missing_category_target;
  ProbeResult conflicting_category_owner;
  ProbeResult duplicate_protocol_requirement;
  ProbeResult inherited_protocol_requirement_conflict;
  ProbeResult adopted_protocol_requirement_conflict;
  ProbeResult forward_inherited_protocol_reference;
};

ProbeResult CaptureInvalidRegistration(
    const InvalidMetadataCaseDescriptor &descriptor,
    const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *registration_table,
    const char *class_name) {
  ProbeResult result;
  result.descriptor = &descriptor;
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
  ForwardProtocolReferenceImage forward_protocol_fixture;
  MissingCategoryTargetImage missing_category_fixture;
  ConflictingCategoryOwnerImage conflicting_category_fixture;
  DuplicateProtocolRequirementImage duplicate_requirement_fixture;
  InheritedProtocolRequirementConflictImage inherited_conflict_fixture;
  AdoptedProtocolRequirementConflictImage adopted_conflict_fixture;
  ForwardInheritedProtocolReferenceImage forward_inherited_fixture;
  ProbeRun run;
  run.invalid_protocol_reference = CaptureInvalidRegistration(
      kInvalidProtocolReferenceCase,
      &invalid_protocol_fixture.image,
      &invalid_protocol_fixture.registration_table, kClassName);
  run.forward_protocol_reference = CaptureInvalidRegistration(
      kForwardProtocolReferenceCase,
      &forward_protocol_fixture.image,
      &forward_protocol_fixture.registration_table,
      kForwardProtocolClassName);
  run.missing_category_target = CaptureInvalidRegistration(
      kMissingCategoryTargetCase,
      &missing_category_fixture.image,
      &missing_category_fixture.registration_table, kMissingCategoryClassName);
  run.conflicting_category_owner = CaptureInvalidRegistration(
      kConflictingCategoryOwnerCase,
      &conflicting_category_fixture.image,
      &conflicting_category_fixture.registration_table,
      kConflictingCategoryClassName);
  run.duplicate_protocol_requirement = CaptureInvalidRegistration(
      kDuplicateProtocolRequirementCase,
      &duplicate_requirement_fixture.image,
      &duplicate_requirement_fixture.registration_table, "");
  run.inherited_protocol_requirement_conflict = CaptureInvalidRegistration(
      kInheritedProtocolRequirementConflictCase,
      &inherited_conflict_fixture.image,
      &inherited_conflict_fixture.registration_table, "");
  run.adopted_protocol_requirement_conflict = CaptureInvalidRegistration(
      kAdoptedProtocolRequirementConflictCase,
      &adopted_conflict_fixture.image,
      &adopted_conflict_fixture.registration_table,
      kAdoptedConflictClassName);
  run.forward_inherited_protocol_reference = CaptureInvalidRegistration(
      kForwardInheritedProtocolReferenceCase,
      &forward_inherited_fixture.image,
      &forward_inherited_fixture.registration_table, "");
  return run;
}

void WriteProbeResultFields(
    std::ostream &out,
    objc3c::runtime::probe::JsonFieldSeparator &separator,
    const ProbeResult &result) {
  using objc3c::runtime::probe::WriteJsonIntField;
  using objc3c::runtime::probe::WriteJsonBoolField;
  using objc3c::runtime::probe::WriteJsonStringField;
  using objc3c::runtime::probe::WriteJsonUInt64Field;

  const InvalidMetadataCaseDescriptor *descriptor = result.descriptor;
  WriteJsonStringField(out, separator, "case_id",
                       descriptor != nullptr ? descriptor->case_id : nullptr);
  WriteJsonStringField(
      out, separator, "source_path",
      descriptor != nullptr ? descriptor->source_path : nullptr);
  WriteJsonStringField(
      out, separator, "metadata_surface",
      descriptor != nullptr ? descriptor->metadata_surface : nullptr);
  WriteJsonStringField(
      out, separator, "target_kind",
      descriptor != nullptr ? descriptor->target_kind : nullptr);
  WriteJsonStringField(
      out, separator, "target_name",
      descriptor != nullptr ? descriptor->target_name : nullptr);
  WriteJsonStringField(
      out, separator, "category_name",
      descriptor != nullptr ? descriptor->category_name : nullptr);
  WriteJsonStringField(
      out, separator, "owner_identity",
      descriptor != nullptr ? descriptor->owner_identity : nullptr);
  WriteJsonStringField(
      out, separator, "visibility_state",
      descriptor != nullptr ? descriptor->visibility_state : nullptr);
  WriteJsonStringField(
      out, separator, "availability_state",
      descriptor != nullptr ? descriptor->availability_state : nullptr);
  WriteJsonIntField(out, separator, "registration_status",
                    result.registration_status);
  WriteJsonStringField(out, separator, "registration_status_name",
                       kInvalidRegistrationStatusName);
  WriteJsonStringField(out, separator, "diagnostic_code",
                       kMalformedMetadataDiagnosticCode);
  WriteJsonStringField(out, separator, "diagnostic_message",
                       kMalformedMetadataDiagnosticMessage);
  WriteJsonStringField(out, separator, "diagnostic_class",
                       kMalformedMetadataDiagnosticClass);
  WriteJsonStringField(
      out, separator, "snapshot_diagnostic_code",
      result.graph_state.last_malformed_class_graph_diagnostic_code);
  WriteJsonStringField(
      out, separator, "snapshot_diagnostic_message",
      result.graph_state.last_malformed_class_graph_diagnostic_message);
  WriteJsonStringField(
      out, separator, "snapshot_diagnostic_class",
      result.graph_state.last_malformed_class_graph_diagnostic_class);
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
  WriteJsonStringField(
      out, separator, "expected_last_malformed_class_graph_reason",
      descriptor != nullptr ? descriptor->expected_reason : nullptr);
  WriteJsonBoolField(
      out, separator, "reason_matches_expected",
      descriptor != nullptr && malformed_reason != nullptr &&
          result.malformed_reason == descriptor->expected_reason);
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
  WriteJsonFieldName(std::cout, "forward_protocol_reference");
  WriteProbeResultObject(std::cout, run.forward_protocol_reference);
  separator.BeforeField(std::cout);
  WriteJsonFieldName(std::cout, "missing_category_target");
  WriteProbeResultObject(std::cout, run.missing_category_target);
  separator.BeforeField(std::cout);
  WriteJsonFieldName(std::cout, "conflicting_category_owner");
  WriteProbeResultObject(std::cout, run.conflicting_category_owner);
  separator.BeforeField(std::cout);
  WriteJsonFieldName(std::cout, "duplicate_protocol_requirement");
  WriteProbeResultObject(std::cout, run.duplicate_protocol_requirement);
  separator.BeforeField(std::cout);
  WriteJsonFieldName(std::cout, "inherited_protocol_requirement_conflict");
  WriteProbeResultObject(std::cout,
                         run.inherited_protocol_requirement_conflict);
  separator.BeforeField(std::cout);
  WriteJsonFieldName(std::cout, "adopted_protocol_requirement_conflict");
  WriteProbeResultObject(std::cout,
                         run.adopted_protocol_requirement_conflict);
  separator.BeforeField(std::cout);
  WriteJsonFieldName(std::cout, "forward_inherited_protocol_reference");
  WriteProbeResultObject(std::cout, run.forward_inherited_protocol_reference);
  std::cout << "}";
}

}  // namespace

int main() {
  PrintProbeResult(RunProbe());
  return 0;
}
