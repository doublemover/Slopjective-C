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

constexpr const char *kModuleName = "public-runtime-reflection-api-probe";
constexpr const char *kTranslationUnit =
    "tests/tooling/runtime/public_runtime_reflection_api_probe.cpp";
constexpr const char *kWidgetClassName = "PublicReflectionWidget";
constexpr const char *kClassBundleOwner = "interface:PublicReflectionWidget";
constexpr const char *kClassOwner = "class:PublicReflectionWidget";
constexpr const char *kMetaclassBundleOwner =
    "metaclass:PublicReflectionWidget";
constexpr const char *kMetaclassOwner =
    "metaclass-object:PublicReflectionWidget";
constexpr const char *kValuePropertyName = "value";
constexpr const char *kValueSetterSelector = "setValue:";
constexpr const char *kValueIvarSymbol =
    "_OBJC3_IVAR_PublicReflectionWidget_value";
constexpr const char *kValueLayoutSymbol =
    "_OBJC3_IVAR_LAYOUT_PublicReflectionWidget_value";
constexpr const char *kValueLayoutReplayKey =
    "PublicReflectionWidget.value.layout";
constexpr const char *kTracerProtocolName = "PublicReflectionTracer";
constexpr const char *kTracerProtocolOwner = "protocol:PublicReflectionTracer";
constexpr const char *kTracingCategoryName = "Tracing";
constexpr const char *kTracingCategoryOwner =
    "category:PublicReflectionWidget(Tracing)";

const PointerAggregateStorage<1> kEmptyRootStorage = {0, {nullptr}};
const objc3_runtime_pointer_aggregate *kEmptyRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kEmptyRootStorage);

struct PublicReflectionFixture {
  objc3_runtime_image_descriptor image{
      kModuleName, kTranslationUnit, 1, 1, 1, 1, 1, 1};
  MethodListStorage<1> value_methods{
      {1, kClassOwner, kClassOwner},
      {{kValuePropertyName, "method:PublicReflectionWidget::value", "id", 0,
        nullptr, 1, false, false}}};
  objc3c::runtime::EmittedMethodListRef value_method_ref{1, kClassOwner,
                                                         &value_methods};
  objc3c::runtime::EmittedProtocolRecord tracer_protocol{kTracerProtocolName,
                                                         kTracerProtocolOwner,
                                                         kEmptyRoot,
                                                         nullptr,
                                                         nullptr,
                                                         0,
                                                         0,
                                                         0,
                                                         0,
                                                         false};
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
      "id",
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
      "attributes=strong,behavior=Observed;strong=1",
      "lifetime=strong",
      "runtime_hook=retain-release",
      "accessor_ownership=retained",
      nullptr,
      nullptr,
      kValueLayoutReplayKey,
      0,
      8,
      8,
      0,
      0,
      0,
      0,
      8,
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
                                                        8,
                                                        8,
                                                        0,
                                                        0,
                                                        0,
                                                        8,
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
                                                    8,
                                                    8,
                                                    0,
                                                    0,
                                                    0,
                                                    8,
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

} // namespace

int main() {
  PublicReflectionFixture fixture{};
  objc3_runtime_stage_registration_table_for_bootstrap(
      &fixture.registration_table);
  const int registration_status = objc3_runtime_register_image(&fixture.image);
  if (registration_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK) {
    return Fail("public reflection fixture registration failed");
  }

  objc3_runtime_reflection_state_snapshot state{};
  objc3_runtime_reflection_class_snapshot widget_class{};
  objc3_runtime_reflection_class_snapshot indexed_class{};
  objc3_runtime_reflection_property_snapshot count_property{};
  objc3_runtime_reflection_property_snapshot value_property{};
  objc3_runtime_reflection_property_snapshot indexed_property{};
  objc3_runtime_reflection_method_snapshot count_method{};
  objc3_runtime_reflection_protocol_snapshot tracer_protocol{};
  objc3_runtime_reflection_protocol_conformance_snapshot tracer_conformance{};
  objc3_runtime_reflection_category_snapshot category{};
  objc3_runtime_reflection_category_snapshot indexed_category{};
  objc3_runtime_reflection_selector_snapshot selector{};
  objc3_runtime_reflection_selector_snapshot indexed_selector{};
  objc3_runtime_reflection_surface_snapshot indexed_surface{};
  objc3_runtime_reflection_surface_snapshot surface{};
  objc3_runtime_reflection_surface_snapshot invalid_surface{};
  objc3_runtime_reflection_property_snapshot invalid_property{};
  objc3_runtime_reflection_property_snapshot missing_indexed_property{};

  const std::uint64_t surface_count = objc3_runtime_reflection_surface_count();
  const int indexed_surface_status =
      objc3_runtime_copy_reflection_surface(0u, &indexed_surface);
  const int surface_status = objc3_runtime_copy_reflection_surface_by_kind(
      OBJC3_RUNTIME_REFLECTION_SURFACE_PROPERTY, &surface);
  const int invalid_surface_status =
      objc3_runtime_copy_reflection_surface_by_kind(
          OBJC3_RUNTIME_REFLECTION_SURFACE_INVALID, &invalid_surface);
  const int state_status = objc3_runtime_copy_reflection_state(&state);
  const int class_status =
      objc3_runtime_copy_reflection_class(kWidgetClassName, &widget_class);
  const int indexed_class_status =
      objc3_runtime_copy_reflection_class_at(0u, &indexed_class);
  const int property_status = objc3_runtime_copy_reflection_property(
      kWidgetClassName, kValuePropertyName, &count_property);
  const int value_property_status = objc3_runtime_copy_reflection_property(
      kWidgetClassName, kValuePropertyName, &value_property);
  const int indexed_property_status = objc3_runtime_copy_reflection_property_at(
      kWidgetClassName, 0u, &indexed_property);
  const int missing_indexed_property_status =
      objc3_runtime_copy_reflection_property_at(kWidgetClassName, 99u,
                                                &missing_indexed_property);
  const int method_status = objc3_runtime_copy_reflection_method(
      kWidgetClassName, kValuePropertyName,
      OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INSTANCE, &count_method);
  const int protocol_status = objc3_runtime_copy_reflection_protocol(
      kTracerProtocolName, &tracer_protocol);
  const int conformance_status =
      objc3_runtime_copy_reflection_protocol_conformance(
          kWidgetClassName, kTracerProtocolName, &tracer_conformance);
  const int category_status = objc3_runtime_copy_reflection_category(
      kWidgetClassName, kTracingCategoryName, &category);
  const int indexed_category_status = objc3_runtime_copy_reflection_category_at(
      kWidgetClassName, 0u, &indexed_category);
  const int selector_status =
      objc3_runtime_copy_reflection_selector(kValuePropertyName, &selector);
  const int indexed_selector_status =
      objc3_runtime_copy_reflection_selector_at(0u, &indexed_selector);
  const int invalid_status = objc3_runtime_copy_reflection_property(
      nullptr, kValuePropertyName, &invalid_property);
  const int invalid_output_status =
      objc3_runtime_copy_reflection_state(nullptr);

  if (state_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection state status drifted");
  }
  if (surface_count != 8u) {
    return Fail("public reflection surface count drifted");
  }
  if (indexed_surface_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection indexed surface status drifted");
  }
  if (surface_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection surface status drifted");
  }
  if (invalid_surface_status != OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY) {
    return Fail("public reflection invalid surface status drifted");
  }
  if (class_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection class status drifted");
  }
  if (indexed_class_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection indexed class status drifted");
  }
  if (property_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection count property status drifted");
  }
  if (value_property_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection value property status drifted");
  }
  if (indexed_property_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection indexed property status drifted");
  }
  if (missing_indexed_property_status !=
      OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND) {
    return Fail("public reflection indexed missing property status drifted");
  }
  if (method_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection method status drifted");
  }
  if (protocol_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection protocol status drifted");
  }
  if (conformance_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection conformance status drifted");
  }
  if (category_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection category status drifted");
  }
  if (indexed_category_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection indexed category status drifted");
  }
  if (selector_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection selector status drifted");
  }
  if (indexed_selector_status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return Fail("public reflection indexed selector status drifted");
  }
  if (invalid_status != OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY) {
    return Fail("public reflection invalid-query status drifted");
  }
  if (invalid_output_status != OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT) {
    return Fail("public reflection invalid-output status drifted");
  }
  if (widget_class.found != 1 || indexed_class.found != 1 ||
      count_property.found != 1 || value_property.found != 1 ||
      indexed_property.found != 1 || count_method.found != 1 ||
      tracer_protocol.found != 1 || tracer_conformance.conforms != 1 ||
      category.found != 1 || indexed_category.found != 1 ||
      selector.found != 1 || indexed_selector.found != 1) {
    return Fail("public reflection realized-state lookup drifted");
  }
  if (indexed_property.property_name == nullptr ||
      std::strcmp(indexed_property.property_name, kValuePropertyName) != 0 ||
      indexed_category.category_name == nullptr ||
      std::strcmp(indexed_category.category_name, kTracingCategoryName) != 0 ||
      indexed_selector.canonical_selector == nullptr ||
      std::strcmp(indexed_selector.canonical_selector, kValuePropertyName) !=
          0) {
    return Fail("public reflection deterministic indexed snapshots drifted");
  }
  if (value_property.property_behavior_name == nullptr ||
      std::strcmp(value_property.property_behavior_name, "Observed") != 0) {
    return Fail("public property behavior reflection drifted");
  }
  if (surface.issue_ref != 8174 || surface.supported != 1 ||
      surface.realized_state_backed != 1 || surface.bounded_public_abi != 1 ||
      surface.fail_closed != 1 || surface.creates_dynamic_runtime_state != 0 ||
      surface.exposes_private_testing_snapshot != 0 ||
      surface.unsupported_metadata_status !=
          OBJC3_RUNTIME_REFLECTION_STATUS_UNAVAILABLE ||
      surface.malformed_metadata_status !=
          OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA ||
      surface.surface_name == nullptr ||
      std::strcmp(surface.surface_name, "property") != 0) {
    return Fail("public reflection surface contract drifted");
  }

  std::printf("{");
  std::printf("\"abi_version\":%u,",
              objc3_runtime_reflection_api_abi_version());
  std::printf("\"registration_status\":%d,", registration_status);
  std::printf("\"surface_count\":%llu,",
              static_cast<unsigned long long>(surface_count));
  std::printf("\"indexed_surface_status\":%d,", indexed_surface_status);
  std::printf("\"surface_status\":%d,", surface_status);
  std::printf("\"invalid_surface_status\":%d,", invalid_surface_status);
  std::printf("\"surface_issue_ref\":%d,", surface.issue_ref);
  std::printf("\"state_status\":%d,", state_status);
  std::printf("\"class_status\":%d,", class_status);
  std::printf("\"indexed_class_status\":%d,", indexed_class_status);
  std::printf("\"property_status\":%d,", property_status);
  std::printf("\"value_property_status\":%d,", value_property_status);
  std::printf("\"indexed_property_status\":%d,", indexed_property_status);
  std::printf("\"missing_indexed_property_status\":%d,",
              missing_indexed_property_status);
  std::printf("\"method_status\":%d,", method_status);
  std::printf("\"protocol_status\":%d,", protocol_status);
  std::printf("\"conformance_status\":%d,", conformance_status);
  std::printf("\"category_status\":%d,", category_status);
  std::printf("\"indexed_category_status\":%d,", indexed_category_status);
  std::printf("\"selector_status\":%d,", selector_status);
  std::printf("\"indexed_selector_status\":%d,", indexed_selector_status);
  std::printf("\"invalid_status\":%d,", invalid_status);
  std::printf("\"invalid_output_status\":%d,", invalid_output_status);
  std::printf("\"class_found\":%d,", widget_class.found);
  std::printf("\"indexed_class_found\":%d,", indexed_class.found);
  std::printf("\"property_found\":%d,", count_property.found);
  std::printf("\"value_property_found\":%d,", value_property.found);
  std::printf("\"indexed_property_found\":%d,", indexed_property.found);
  std::printf("\"method_found\":%d,", count_method.found);
  std::printf("\"protocol_found\":%d,", tracer_protocol.found);
  std::printf("\"conforms\":%d,", tracer_conformance.conforms);
  std::printf("\"category_found\":%d,", category.found);
  std::printf("\"indexed_category_found\":%d,", indexed_category.found);
  std::printf("\"selector_found\":%d,", selector.found);
  std::printf("\"indexed_selector_found\":%d,", indexed_selector.found);
  std::printf("\"realized_class_count\":%llu,",
              static_cast<unsigned long long>(state.realized_class_count));
  std::printf("\"value_property_behavior\":\"%s\",",
              value_property.property_behavior_name);
  std::printf("\"property_slot\":%llu",
              static_cast<unsigned long long>(count_property.slot_index));
  std::printf("}");
  return 0;
}
