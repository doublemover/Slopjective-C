#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/public/objc3_runtime_api.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

template <std::uint64_t EntryCount> struct PointerAggregateStorage {
  std::uint64_t count;
  const void *entries[EntryCount];
};

template <std::uint64_t EntryCount> struct MethodListStorage {
  objc3c::runtime::EmittedMethodListHeader header;
  objc3c::runtime::EmittedMethodListEntry entries[EntryCount];
};

constexpr const char *kModuleName = "debug-anchor-identity-replay-probe";
constexpr const char *kTranslationUnit =
    "tests/native/runtime/object_model/debug_anchor_identity_replay_probe.cpp";
constexpr const char *kWidgetClassName = "DebugAnchorWidget";
constexpr const char *kClassBundleOwner = "interface:DebugAnchorWidget";
constexpr const char *kClassOwner = "class:DebugAnchorWidget";
constexpr const char *kMetaclassBundleOwner = "metaclass:DebugAnchorWidget";
constexpr const char *kMetaclassOwner = "metaclass-object:DebugAnchorWidget";
constexpr const char *kValuePropertyName = "value";
constexpr const char *kValueSetterSelector = "setValue:";
constexpr const char *kValueIvarSymbol = "_OBJC3_IVAR_DebugAnchorWidget_value";
constexpr const char *kValueLayoutSymbol =
    "_OBJC3_IVAR_LAYOUT_DebugAnchorWidget_value";
constexpr const char *kValueLayoutReplayKey =
    "DebugAnchorWidget.value.layout";
constexpr const char *kTracerProtocolName = "DebugAnchorTraceable";
constexpr const char *kTracerProtocolOwner = "protocol:DebugAnchorTraceable";
constexpr const char *kTracingCategoryName = "ReplayDebugIdentity";
constexpr const char *kTracingCategoryOwner =
    "category:DebugAnchorWidget(ReplayDebugIdentity)";

const PointerAggregateStorage<1> kEmptyRootStorage = {0, {nullptr}};
const objc3_runtime_pointer_aggregate *kEmptyRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kEmptyRootStorage);

struct DebugAnchorFixture {
  objc3_runtime_image_descriptor image{
      kModuleName, kTranslationUnit, 1, 1, 1, 1, 1, 1};
  MethodListStorage<1> value_methods{
      {1, kClassOwner, kClassOwner},
      {{kValuePropertyName, "method:DebugAnchorWidget::value", "i32", 0,
        nullptr, 1, false, false}}};
  objc3c::runtime::EmittedMethodListRef value_method_ref{1, kClassOwner,
                                                         &value_methods};
  objc3c::runtime::EmittedProtocolRecord tracer_protocol{
      kTracerProtocolName, kTracerProtocolOwner, kEmptyRoot, nullptr, nullptr,
      0, 0, 0, 0, false};
  PointerAggregateStorage<1> category_protocol_refs{1, {&tracer_protocol}};
  const objc3_runtime_pointer_aggregate *category_protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &category_protocol_refs);
  objc3c::runtime::EmittedCategoryRecord tracing_category{
      kWidgetClassName,
      kTracingCategoryName,
      "implementation",
      kTracingCategoryOwner,
      kClassOwner,
      kTracingCategoryOwner,
      kEmptyRoot,
      category_protocol_root,
      nullptr,
      nullptr,
      0,
      0,
      0};
  objc3c::runtime::EmittedClassBundle class_bundle{
      {kWidgetClassName, kClassBundleOwner, kClassOwner, "", nullptr,
       &value_method_ref, kEmptyRoot, false, false},
      {kWidgetClassName, kMetaclassBundleOwner, kMetaclassOwner, "", nullptr,
       nullptr, kEmptyRoot, false, false}};
  objc3c::runtime::EmittedPropertyDescriptor value_property{
      kValuePropertyName,
      "i32",
      kClassBundleOwner,
      kClassBundleOwner,
      kClassOwner,
      kValuePropertyName,
      kValueSetterSelector,
      kValuePropertyName,
      kValueSetterSelector,
      kValueIvarSymbol,
      kValueIvarSymbol,
      kValueLayoutSymbol,
      "attributes=nonatomic,behavior=Observed;nonatomic=1",
      "lifetime=assign",
      "runtime_hook=direct",
      "accessor_ownership=unretained",
      nullptr,
      nullptr,
      kValueLayoutReplayKey,
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
  objc3c::runtime::EmittedIvarLayoutRecord value_layout{kValueLayoutSymbol,
                                                        kValueLayoutReplayKey,
                                                        0,
                                                        0,
                                                        4,
                                                        4,
                                                        0,
                                                        0,
                                                        0,
                                                        4,
                                                        0,
                                                        0,
                                                        true};
  std::uint64_t value_offset = 0;
  objc3c::runtime::EmittedIvarDescriptor value_ivar{kClassBundleOwner,
                                                    kClassBundleOwner,
                                                    kClassOwner,
                                                    kClassBundleOwner,
                                                    kValuePropertyName,
                                                    kValueIvarSymbol,
                                                    &value_layout,
                                                    &value_offset,
                                                    kValueLayoutReplayKey,
                                                    0,
                                                    0,
                                                    4,
                                                    4,
                                                    0,
                                                    0,
                                                    0,
                                                    4,
                                                    0,
                                                    0,
                                                    true};
  PointerAggregateStorage<1> class_root_storage{1, {&class_bundle}};
  PointerAggregateStorage<1> protocol_root_storage{1, {&tracer_protocol}};
  PointerAggregateStorage<1> category_root_storage{1, {&tracing_category}};
  PointerAggregateStorage<1> property_root_storage{1, {&value_property}};
  PointerAggregateStorage<1> ivar_root_storage{1, {&value_ivar}};
  PointerAggregateStorage<1> selector_pool_storage{1, {kValuePropertyName}};
  PointerAggregateStorage<7> discovery_root_storage{
      7,
      {&class_root_storage, &protocol_root_storage, &category_root_storage,
       &property_root_storage, &ivar_root_storage, &selector_pool_storage,
       &kEmptyRootStorage}};
  const objc3_runtime_pointer_aggregate *class_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &class_root_storage);
  const objc3_runtime_pointer_aggregate *protocol_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &protocol_root_storage);
  const objc3_runtime_pointer_aggregate *category_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &category_root_storage);
  const objc3_runtime_pointer_aggregate *property_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &property_root_storage);
  const objc3_runtime_pointer_aggregate *ivar_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &ivar_root_storage);
  const objc3_runtime_pointer_aggregate *selector_pool =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &selector_pool_storage);
  const objc3_runtime_pointer_aggregate *discovery_root =
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &discovery_root_storage);
  const void *discovery_root_anchor = discovery_root;
  unsigned char image_local_init_state = 0;
  objc3_runtime_registration_table registration_table{2,
                                                      12,
                                                      &image,
                                                      discovery_root,
                                                      &discovery_root_anchor,
                                                      class_root,
                                                      protocol_root,
                                                      category_root,
                                                      property_root,
                                                      ivar_root,
                                                      selector_pool,
                                                      nullptr,
                                                      nullptr,
                                                      &image_local_init_state};
};

int Fail(const char *message) {
  std::printf("{\"error\":\"%s\"}\n", message);
  return 1;
}

bool HasCommonDebugIdentity(
    const objc3_runtime_reflection_debug_anchor_snapshot &snapshot,
    const char *expected_source_anchor_kind,
    const char *expected_source_map_record_kind,
    const char *expected_debug_projection_key) {
  return snapshot.found == 1 && snapshot.runtime_owned == 1 &&
         snapshot.source_identity_backed == 1 && snapshot.replayable == 1 &&
         snapshot.snapshot_size ==
             sizeof(objc3_runtime_reflection_debug_anchor_snapshot) &&
         snapshot.abi_governance_policy != nullptr &&
         snapshot.source_anchor_kind != nullptr &&
         std::strcmp(snapshot.source_anchor_kind,
                     expected_source_anchor_kind) == 0 &&
         snapshot.source_map_record_kind != nullptr &&
         std::strcmp(snapshot.source_map_record_kind,
                     expected_source_map_record_kind) == 0 &&
         snapshot.source_map_anchor_policy != nullptr &&
         snapshot.artifact_inspector_compatibility != nullptr &&
         snapshot.module_name != nullptr &&
         std::strcmp(snapshot.module_name, kModuleName) == 0 &&
         snapshot.translation_unit_identity_key != nullptr &&
         std::strcmp(snapshot.translation_unit_identity_key,
                     kTranslationUnit) == 0 &&
         snapshot.source_path != nullptr &&
         std::strcmp(snapshot.source_path, kTranslationUnit) == 0 &&
         snapshot.runtime_identity_key != nullptr &&
         snapshot.debug_projection_key != nullptr &&
         std::strcmp(snapshot.debug_projection_key,
                     expected_debug_projection_key) == 0;
}

void RequestFullDebugAnchorSnapshot(
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  snapshot.snapshot_size =
      sizeof(objc3_runtime_reflection_debug_anchor_snapshot);
}

} // namespace

int main() {
  DebugAnchorFixture fixture{};
  objc3_runtime_stage_registration_table_for_bootstrap(
      &fixture.registration_table);
  const int registration_status = objc3_runtime_register_image(&fixture.image);
  if (registration_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK) {
    return Fail("debug anchor fixture registration failed");
  }

  objc3_runtime_reflection_debug_anchor_snapshot class_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot category_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot protocol_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot property_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot ivar_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot method_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot indexed_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot missing_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot stale_anchor{};
  objc3_runtime_reflection_debug_anchor_snapshot legacy_anchor{};
  RequestFullDebugAnchorSnapshot(class_anchor);
  RequestFullDebugAnchorSnapshot(category_anchor);
  RequestFullDebugAnchorSnapshot(protocol_anchor);
  RequestFullDebugAnchorSnapshot(property_anchor);
  RequestFullDebugAnchorSnapshot(ivar_anchor);
  RequestFullDebugAnchorSnapshot(method_anchor);
  RequestFullDebugAnchorSnapshot(indexed_anchor);
  RequestFullDebugAnchorSnapshot(missing_anchor);
  RequestFullDebugAnchorSnapshot(stale_anchor);

  const int class_status = objc3_runtime_copy_reflection_debug_anchor(
      OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS, kWidgetClassName, nullptr,
      OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID, &class_anchor);
  const int legacy_status = objc3_runtime_copy_reflection_debug_anchor(
      OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS, kWidgetClassName, nullptr,
      OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID, &legacy_anchor);
  const int category_status = objc3_runtime_copy_reflection_debug_anchor(
      OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CATEGORY, kWidgetClassName,
      kTracingCategoryName, OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID,
      &category_anchor);
  const int protocol_status = objc3_runtime_copy_reflection_debug_anchor(
      OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROTOCOL, kTracerProtocolName,
      nullptr, OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID,
      &protocol_anchor);
  const int property_status = objc3_runtime_copy_reflection_debug_anchor(
      OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY, kWidgetClassName,
      kValuePropertyName, OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID,
      &property_anchor);
  const int ivar_status = objc3_runtime_copy_reflection_debug_anchor(
      OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_IVAR, kWidgetClassName,
      kValuePropertyName, OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID,
      &ivar_anchor);
  const int method_status = objc3_runtime_copy_reflection_debug_anchor(
      OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_METHOD, kWidgetClassName,
      kValuePropertyName, OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INSTANCE,
      &method_anchor);
  const int indexed_status =
      objc3_runtime_copy_reflection_debug_anchor_at(0u, &indexed_anchor);
  const int missing_status = objc3_runtime_copy_reflection_debug_anchor(
      OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY, kWidgetClassName,
      "missing", OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID,
      &missing_anchor);
  const int stale_status =
      objc3_runtime_copy_reflection_debug_anchor_with_generation(
          OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS, kWidgetClassName,
          nullptr, OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID,
          class_anchor.anchor_generation + 1u, &stale_anchor);

  if (class_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK ||
      legacy_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK ||
      category_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK ||
      protocol_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK ||
      property_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK ||
      ivar_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK ||
      method_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK ||
      indexed_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("debug anchor status drifted");
  }
  if (legacy_anchor.snapshot_size >=
          sizeof(objc3_runtime_reflection_debug_anchor_snapshot) ||
      legacy_anchor.debug_projection_key == nullptr ||
      legacy_anchor.abi_governance_policy != nullptr) {
    return Fail("debug anchor legacy ABI prefix drifted");
  }
  if (!HasCommonDebugIdentity(class_anchor, "class", "declaration",
                              "runtime.anchor.object_model.class") ||
      !HasCommonDebugIdentity(category_anchor, "category", "declaration",
                              "runtime.anchor.object_model.category") ||
      !HasCommonDebugIdentity(protocol_anchor, "protocol", "declaration",
                              "runtime.anchor.object_model.protocol") ||
      !HasCommonDebugIdentity(property_anchor, "property",
                              "property-access",
                              "runtime.anchor.object_model.property") ||
      !HasCommonDebugIdentity(ivar_anchor, "ivar", "generated-accessor",
                              "runtime.anchor.object_model.ivar") ||
      !HasCommonDebugIdentity(method_anchor, "method", "method",
                              "runtime.anchor.object_model.method")) {
    return Fail("debug anchor source identity drifted");
  }
  if (class_anchor.anchor_kind != OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS ||
      category_anchor.anchor_kind !=
          OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CATEGORY ||
      protocol_anchor.anchor_kind !=
          OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROTOCOL ||
      property_anchor.anchor_kind !=
          OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY ||
      ivar_anchor.anchor_kind != OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_IVAR ||
      method_anchor.anchor_kind !=
          OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_METHOD) {
    return Fail("debug anchor kind drifted");
  }
  if (property_anchor.property_name == nullptr ||
      std::strcmp(property_anchor.property_name, kValuePropertyName) != 0 ||
      ivar_anchor.ivar_binding_symbol == nullptr ||
      std::strcmp(ivar_anchor.ivar_binding_symbol, kValueIvarSymbol) != 0 ||
      method_anchor.selector == nullptr ||
      std::strcmp(method_anchor.selector, kValuePropertyName) != 0 ||
      category_anchor.category_name == nullptr ||
      std::strcmp(category_anchor.category_name, kTracingCategoryName) != 0 ||
      protocol_anchor.protocol_name == nullptr ||
      std::strcmp(protocol_anchor.protocol_name, kTracerProtocolName) != 0) {
    return Fail("debug anchor reflected row identity drifted");
  }
  if (missing_status != OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND ||
      missing_anchor.missing_anchor != 1 ||
      stale_status != OBJC3_RUNTIME_REFLECTION_STATUS_STALE_ANCHOR ||
      stale_anchor.stale_generation != 1) {
    return Fail("debug anchor negative boundary drifted");
  }
  if (objc3_runtime_reflection_debug_anchor_count() < 6u ||
      objc3_runtime_reflection_debug_anchor_abi_version() !=
          OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_ABI_VERSION ||
      objc3_runtime_reflection_debug_anchor_min_reader_abi_version() !=
          OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_ABI_MIN_READER_VERSION) {
    return Fail("debug anchor replay surface drifted");
  }

  std::printf("{");
  std::printf("\"registration_status\":%d,", registration_status);
  std::printf("\"debug_anchor_abi_version\":%u,",
              objc3_runtime_reflection_debug_anchor_abi_version());
  std::printf("\"debug_anchor_min_reader_abi_version\":%u,",
              objc3_runtime_reflection_debug_anchor_min_reader_abi_version());
  std::printf("\"debug_anchor_count\":%llu,",
              static_cast<unsigned long long>(
                  objc3_runtime_reflection_debug_anchor_count()));
  std::printf("\"class_status\":%d,", class_status);
  std::printf("\"category_status\":%d,", category_status);
  std::printf("\"protocol_status\":%d,", protocol_status);
  std::printf("\"property_status\":%d,", property_status);
  std::printf("\"ivar_status\":%d,", ivar_status);
  std::printf("\"method_status\":%d,", method_status);
  std::printf("\"indexed_status\":%d,", indexed_status);
  std::printf("\"missing_status\":%d,", missing_status);
  std::printf("\"stale_status\":%d,", stale_status);
  std::printf("\"class_anchor_generation\":%llu",
              static_cast<unsigned long long>(class_anchor.anchor_generation));
  std::printf("}");
  return 0;
}
