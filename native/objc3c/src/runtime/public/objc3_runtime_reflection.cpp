#include "runtime/public/objc3_runtime_reflection.h"

#include "runtime/classes/class_metadata_tables.h"
#include "runtime/classes/protocol_conformance.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/method_list_resolution.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/selectors/selector_records.h"
#include "runtime/selectors/selector_spelling.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/storage/property_lookup.h"
#include "runtime/strings/borrowed_string.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_set>

namespace objc3c::runtime {
namespace {

constexpr const char *kPublicReflectionApiOwner =
    "objc3c.runtime.public-reflection-api.v1";
constexpr const char *kPublicReflectionLifetimeModel =
    "caller-owned snapshots with runtime-owned borrowed strings";
constexpr const char *kUnsupportedMetadataPolicy =
    "unsupported and internal metadata returns status without private fields";

struct PublicRuntimeReflectionSurfaceRecord {
  int surface_kind;
  const char *surface_name;
  const char *entrypoint_name;
  const char *snapshot_type_name;
  const char *runtime_anchor;
  const char *query_boundary;
};

constexpr PublicRuntimeReflectionSurfaceRecord kPublicReflectionSurfaces[] = {
    {OBJC3_RUNTIME_REFLECTION_SURFACE_STATE, "runtime-state",
     "objc3_runtime_copy_reflection_state",
     "objc3_runtime_reflection_state_snapshot", "ProcessRuntimeState",
     "aggregate counters only"},
    {OBJC3_RUNTIME_REFLECTION_SURFACE_CLASS, "class",
     "objc3_runtime_copy_reflection_class",
     "objc3_runtime_reflection_class_snapshot",
     "RuntimeState::realized_class_nodes", "unique realized class name"},
    {OBJC3_RUNTIME_REFLECTION_SURFACE_PROPERTY, "property",
     "objc3_runtime_copy_reflection_property",
     "objc3_runtime_reflection_property_snapshot",
     "FindRuntimePropertyAccessorByNameUnlocked",
     "realized property accessor by class and property name"},
    {OBJC3_RUNTIME_REFLECTION_SURFACE_METHOD, "method",
     "objc3_runtime_copy_reflection_method",
     "objc3_runtime_reflection_method_snapshot",
     "FindSelectorSlotByCanonicalSpellingUnlocked",
     "realized class method list by selector and method family"},
    {OBJC3_RUNTIME_REFLECTION_SURFACE_PROTOCOL, "protocol",
     "objc3_runtime_copy_reflection_protocol",
     "objc3_runtime_reflection_protocol_snapshot", "OrderedClassGraphImages",
     "concrete protocol descriptor by protocol name"},
    {OBJC3_RUNTIME_REFLECTION_SURFACE_PROTOCOL_CONFORMANCE,
     "protocol-conformance",
     "objc3_runtime_copy_reflection_protocol_conformance",
     "objc3_runtime_reflection_protocol_conformance_snapshot",
     "QueryRealizedClassProtocolConformanceUnlocked",
     "realized class conformance by class and protocol name"},
    {OBJC3_RUNTIME_REFLECTION_SURFACE_CATEGORY, "category",
     "objc3_runtime_copy_reflection_category",
     "objc3_runtime_reflection_category_snapshot",
     "RealizedClassNode::attached_category_records",
     "attached category by class and category name"},
    {OBJC3_RUNTIME_REFLECTION_SURFACE_SELECTOR, "selector",
     "objc3_runtime_copy_reflection_selector",
     "objc3_runtime_reflection_selector_snapshot",
     "FindSelectorSlotByCanonicalSpellingUnlocked",
     "registered selector by canonical spelling"},
};

constexpr std::uint64_t PublicReflectionSurfaceCount() {
  return sizeof(kPublicReflectionSurfaces) /
         sizeof(kPublicReflectionSurfaces[0]);
}

bool RuntimeReflectionCStringPresent(const char *value) {
  return value != nullptr && value[0] != '\0';
}

int PublishReflectionStatus(int status, int *target_status) {
  if (target_status != nullptr) {
    *target_status = status;
  }
  return status;
}

void InitializeReflectionStateSnapshot(
    objc3_runtime_reflection_state_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size = sizeof(objc3_runtime_reflection_state_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_UNAVAILABLE;
  snapshot.public_api_owner = kPublicReflectionApiOwner;
  snapshot.result_lifetime_model = kPublicReflectionLifetimeModel;
  snapshot.unsupported_metadata_policy = kUnsupportedMetadataPolicy;
}

void InitializeReflectionClassSnapshot(
    objc3_runtime_reflection_class_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size = sizeof(objc3_runtime_reflection_class_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
}

void InitializeReflectionPropertySnapshot(
    objc3_runtime_reflection_property_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size = sizeof(objc3_runtime_reflection_property_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
}

void InitializeReflectionMethodSnapshot(
    objc3_runtime_reflection_method_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size = sizeof(objc3_runtime_reflection_method_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  snapshot.family = OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID;
}

void InitializeReflectionProtocolSnapshot(
    objc3_runtime_reflection_protocol_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size = sizeof(objc3_runtime_reflection_protocol_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
}

void InitializeReflectionProtocolConformanceSnapshot(
    objc3_runtime_reflection_protocol_conformance_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size =
      sizeof(objc3_runtime_reflection_protocol_conformance_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
}

void InitializeReflectionCategorySnapshot(
    objc3_runtime_reflection_category_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size = sizeof(objc3_runtime_reflection_category_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
}

void InitializeReflectionSelectorSnapshot(
    objc3_runtime_reflection_selector_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size = sizeof(objc3_runtime_reflection_selector_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
}

void InitializeReflectionSurfaceSnapshot(
    objc3_runtime_reflection_surface_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
  snapshot.snapshot_size = sizeof(objc3_runtime_reflection_surface_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  snapshot.unsupported_metadata_status =
      OBJC3_RUNTIME_REFLECTION_STATUS_UNAVAILABLE;
  snapshot.malformed_metadata_status =
      OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
}

void PopulateReflectionSurfaceSnapshot(
    const PublicRuntimeReflectionSurfaceRecord &record,
    objc3_runtime_reflection_surface_snapshot &snapshot) {
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot.surface_kind = record.surface_kind;
  snapshot.issue_ref = 8174;
  snapshot.supported = 1;
  snapshot.realized_state_backed = 1;
  snapshot.bounded_public_abi = 1;
  snapshot.fail_closed = 1;
  snapshot.creates_dynamic_runtime_state = 0;
  snapshot.exposes_private_testing_snapshot = 0;
  snapshot.unsupported_metadata_status =
      OBJC3_RUNTIME_REFLECTION_STATUS_UNAVAILABLE;
  snapshot.malformed_metadata_status =
      OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
  snapshot.support_claim = "objc3c.behavior.runtime.public-reflection-api";
  snapshot.surface_name = record.surface_name;
  snapshot.entrypoint_name = record.entrypoint_name;
  snapshot.snapshot_type_name = record.snapshot_type_name;
  snapshot.runtime_anchor = record.runtime_anchor;
  snapshot.query_boundary = record.query_boundary;
  snapshot.unsupported_policy = kUnsupportedMetadataPolicy;
}

const RealizedClassNode *
FindUniqueRealizedClassNodeUnlocked(const RuntimeState &state,
                                    const char *class_name, int &status) {
  status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  if (!RuntimeReflectionCStringPresent(class_name)) {
    status = OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY;
    return nullptr;
  }
  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return nullptr;
  }
  if (found->second.size() != 1u) {
    status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
    return nullptr;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
    return nullptr;
  }
  status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  return &state.realized_class_nodes[node_index];
}

const char *NullableRuntimeCString(const char *value) {
  return value != nullptr && value[0] != '\0' ? value : nullptr;
}

std::uint64_t AggregateCount(const objc3_runtime_pointer_aggregate *aggregate) {
  return aggregate != nullptr ? aggregate->count : 0;
}

std::uint64_t MethodListCount(const EmittedMethodListRef *method_list_ref) {
  return method_list_ref != nullptr ? method_list_ref->count : 0;
}

std::uint64_t
CountPublicReflectablePropertiesUnlocked(const RuntimeState &state) {
  std::uint64_t count = 0;
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    count += static_cast<std::uint64_t>(node.runtime_property_accessors.size());
  }
  return count;
}

std::uint64_t CountConcreteProtocolsUnlocked(const RuntimeState &state) {
  std::uint64_t count = 0;
  for (const RegisteredImageMetadata *image : OrderedClassGraphImages(state)) {
    if (image == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < image->protocol_descriptor_count;
         ++index) {
      const auto *record = static_cast<const EmittedProtocolRecord *>(
          RuntimeAggregateEntry(image->protocol_descriptor_root, index));
      if (record != nullptr && record->protocol_name != nullptr &&
          !record->is_forward_declaration) {
        ++count;
      }
    }
  }
  return count;
}

void PopulateReflectionClassSnapshot(
    const RuntimeState &state, const RealizedClassNode &node,
    objc3_runtime_reflection_class_snapshot &snapshot) {
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot.found = 1;
  snapshot.is_root_class = node.is_root_class ? 1 : 0;
  snapshot.has_super_class = node.has_super_node ? 1 : 0;
  snapshot.implementation_backed = node.implementation_backed ? 1 : 0;
  snapshot.objc_final_declared = node.objc_final_declared ? 1 : 0;
  snapshot.objc_sealed_declared = node.objc_sealed_declared ? 1 : 0;
  snapshot.registration_order_ordinal = node.registration_order_ordinal;
  snapshot.direct_protocol_count =
      node.bundle != nullptr
          ? AggregateCount(node.bundle->class_record.adopted_protocol_refs)
          : 0;
  for (const EmittedCategoryRecord *category_record :
       node.attached_category_records) {
    if (category_record != nullptr) {
      snapshot.attached_protocol_count +=
          AggregateCount(category_record->adopted_protocol_refs);
    }
  }
  snapshot.attached_category_count =
      static_cast<std::uint64_t>(node.attached_category_records.size());
  snapshot.runtime_property_count =
      static_cast<std::uint64_t>(node.runtime_property_accessors.size());
  snapshot.runtime_instance_size_bytes =
      static_cast<std::uint64_t>(node.runtime_instance_size_bytes);
  snapshot.class_name = BorrowRuntimeCString(node.class_name);
  if (node.has_super_node &&
      node.super_node_index < state.realized_class_nodes.size()) {
    snapshot.super_class_name = BorrowRuntimeCString(
        state.realized_class_nodes[node.super_node_index].class_name);
  }
  snapshot.module_name = BorrowRuntimeCString(node.module_name);
  snapshot.translation_unit_identity_key =
      BorrowRuntimeCString(node.translation_unit_identity_key);
}

bool PropertyAttributeProfileHas(std::string_view profile,
                                 std::string_view token) {
  const std::size_t position = profile.find(token);
  if (position == std::string_view::npos) {
    return false;
  }
  const std::size_t end = position + token.size();
  const bool starts_token = position == 0 || profile[position - 1] == ';';
  const bool ends_token = end == profile.size() || profile[end] == ';';
  return starts_token && ends_token;
}

bool RenderedPropertyAttributesHave(std::string_view profile,
                                    std::string_view token) {
  constexpr std::string_view kAttributesPrefix = "attributes=";
  const std::size_t position = profile.find(kAttributesPrefix);
  if (position == std::string_view::npos) {
    return false;
  }
  std::string_view attributes =
      profile.substr(position + kAttributesPrefix.size());
  const std::size_t terminator = attributes.find(';');
  if (terminator != std::string_view::npos) {
    attributes = attributes.substr(0, terminator);
  }
  const std::size_t token_position = attributes.find(token);
  if (token_position == std::string_view::npos) {
    return false;
  }
  const std::size_t token_end = token_position + token.size();
  const bool starts_token =
      token_position == 0 || attributes[token_position - 1] == ',';
  const bool ends_token =
      token_end == attributes.size() || attributes[token_end] == ',';
  return starts_token && ends_token;
}

std::uint64_t CountRenderedPropertyAttributes(std::string_view profile) {
  constexpr std::string_view kAttributesPrefix = "attributes=";
  const std::size_t position = profile.find(kAttributesPrefix);
  if (position == std::string_view::npos) {
    return 0;
  }
  std::string_view attributes =
      profile.substr(position + kAttributesPrefix.size());
  const std::size_t terminator = attributes.find(';');
  if (terminator != std::string_view::npos) {
    attributes = attributes.substr(0, terminator);
  }
  if (attributes.empty()) {
    return 0;
  }
  std::uint64_t count = 1;
  for (const char character : attributes) {
    if (character == ',') {
      ++count;
    }
  }
  return count;
}

const char *PropertyBehaviorNameFromProfile(std::string_view profile) {
  if (RenderedPropertyAttributesHave(profile, "behavior=Observed")) {
    return "Observed";
  }
  if (RenderedPropertyAttributesHave(profile, "behavior=Projected")) {
    return "Projected";
  }
  return nullptr;
}

void PopulateReflectionPropertySnapshot(
    const RealizedClassNode &queried_node,
    const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor, bool inherited,
    objc3_runtime_reflection_property_snapshot &snapshot) {
  const EmittedPropertyDescriptor &descriptor = *accessor.property_descriptor;
  const std::string_view property_attribute_profile =
      descriptor.property_attribute_profile != nullptr
          ? std::string_view(descriptor.property_attribute_profile)
          : std::string_view();
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot.found = 1;
  snapshot.inherited = inherited ? 1 : 0;
  snapshot.setter_available = descriptor.effective_setter_available ? 1 : 0;
  snapshot.has_runtime_getter = 1;
  snapshot.has_runtime_setter = !accessor.setter_owner_identity.empty() ? 1 : 0;
  snapshot.is_readonly =
      PropertyAttributeProfileHas(property_attribute_profile, "readonly=1") ? 1
                                                                            : 0;
  snapshot.is_nonatomic =
      PropertyAttributeProfileHas(property_attribute_profile, "nonatomic=1")
          ? 1
          : 0;
  snapshot.is_strong =
      PropertyAttributeProfileHas(property_attribute_profile, "strong=1") ? 1
                                                                          : 0;
  snapshot.is_assign =
      PropertyAttributeProfileHas(property_attribute_profile, "assign=1") ? 1
                                                                          : 0;
  snapshot.is_weak =
      PropertyAttributeProfileHas(property_attribute_profile, "weak=1") ? 1 : 0;
  snapshot.is_copy =
      PropertyAttributeProfileHas(property_attribute_profile, "copy=1") ? 1 : 0;
  snapshot.has_custom_getter =
      RuntimeReflectionCStringPresent(descriptor.getter_selector) ? 1 : 0;
  snapshot.has_custom_setter =
      RuntimeReflectionCStringPresent(descriptor.setter_selector) ? 1 : 0;
  snapshot.attribute_count =
      CountRenderedPropertyAttributes(property_attribute_profile);
  snapshot.slot_index = descriptor.ivar_layout_slot_index;
  snapshot.offset_bytes = descriptor.ivar_layout_offset_bytes;
  snapshot.size_bytes = descriptor.ivar_layout_size_bytes;
  snapshot.alignment_bytes = descriptor.ivar_layout_alignment_bytes;
  snapshot.padding_bytes = descriptor.ivar_layout_padding_bytes;
  snapshot.inherited_slot_count = descriptor.ivar_layout_inherited_slot_count;
  snapshot.inherited_size_bytes = descriptor.ivar_layout_inherited_size_bytes;
  snapshot.owner_size_bytes = descriptor.ivar_layout_owner_size_bytes;
  snapshot.init_order_index = descriptor.ivar_init_order_index;
  snapshot.destroy_order_index = descriptor.ivar_destroy_order_index;
  snapshot.layout_valid = descriptor.ivar_layout_valid ? 1 : 0;
  snapshot.instance_size_bytes =
      static_cast<std::uint64_t>(resolved_node.runtime_instance_size_bytes);
  snapshot.queried_class_name = BorrowRuntimeCString(queried_node.class_name);
  snapshot.resolved_class_name = BorrowRuntimeCString(resolved_node.class_name);
  snapshot.property_name = NullableRuntimeCString(descriptor.property_name);
  snapshot.type_name = NullableRuntimeCString(descriptor.type_name);
  snapshot.getter_selector = NullableRuntimeCString(descriptor.getter_selector);
  snapshot.setter_selector = NullableRuntimeCString(descriptor.setter_selector);
  snapshot.effective_getter_selector =
      NullableRuntimeCString(descriptor.effective_getter_selector);
  snapshot.effective_setter_selector =
      NullableRuntimeCString(descriptor.effective_setter_selector);
  snapshot.property_attribute_profile =
      NullableRuntimeCString(descriptor.property_attribute_profile);
  snapshot.property_behavior_name =
      PropertyBehaviorNameFromProfile(property_attribute_profile);
  snapshot.ownership_lifetime_profile =
      NullableRuntimeCString(descriptor.ownership_lifetime_profile);
  snapshot.ownership_runtime_hook_profile =
      NullableRuntimeCString(descriptor.ownership_runtime_hook_profile);
  snapshot.accessor_ownership_profile =
      NullableRuntimeCString(descriptor.accessor_ownership_profile);
}

DispatchFamily ReflectionDispatchFamily(int family) {
  switch (family) {
  case OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INSTANCE:
    return DispatchFamily::Instance;
  case OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_CLASS:
    return DispatchFamily::Class;
  default:
    return DispatchFamily::Invalid;
  }
}

const EmittedMethodListEntry *
FindMatchingMethodEntry(const EmittedMethodListRef *method_list_ref,
                        const char *canonical_selector) {
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr ||
      canonical_selector == nullptr) {
    return nullptr;
  }
  const auto *header = static_cast<const EmittedMethodListHeader *>(
      method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return nullptr;
  }
  const EmittedMethodListEntry *entries = RuntimeMethodListEntries(header);
  if (entries == nullptr) {
    return nullptr;
  }
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector != nullptr &&
        std::strcmp(entry.selector, canonical_selector) == 0) {
      return &entry;
    }
  }
  return nullptr;
}

bool MethodListIsMalformed(const EmittedMethodListRef *method_list_ref) {
  if (method_list_ref == nullptr || method_list_ref->count == 0) {
    return false;
  }
  if (method_list_ref->method_list == nullptr) {
    return true;
  }
  const auto *header = static_cast<const EmittedMethodListHeader *>(
      method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return true;
  }
  const EmittedMethodListEntry *entries = RuntimeMethodListEntries(header);
  if (entries == nullptr) {
    return true;
  }
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector == nullptr || entry.return_type_name == nullptr) {
      return true;
    }
  }
  return false;
}

bool TryPopulateReflectionMethodFromEntry(
    const RealizedClassNode &queried_node,
    const RealizedClassNode &resolved_node, const EmittedMethodListEntry &entry,
    const EmittedCategoryRecord *category_record,
    const objc3_runtime_selector_handle &selector_slot, DispatchFamily family,
    std::uint64_t category_probe_count, std::uint64_t protocol_probe_count,
    objc3_runtime_reflection_method_snapshot &snapshot) {
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot.found = 1;
  snapshot.family = family == DispatchFamily::Class
                        ? OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_CLASS
                        : OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INSTANCE;
  snapshot.inherited = &queried_node != &resolved_node ? 1 : 0;
  snapshot.declared_in_category = category_record != nullptr ? 1 : 0;
  snapshot.has_body = entry.has_body != 0 ? 1 : 0;
  snapshot.effective_direct_dispatch = entry.effective_direct_dispatch ? 1 : 0;
  snapshot.objc_final_declared = entry.objc_final_declared ? 1 : 0;
  snapshot.selector_stable_id = selector_slot.stable_id;
  snapshot.parameter_count = entry.parameter_count;
  snapshot.category_probe_count = category_probe_count;
  snapshot.protocol_probe_count = protocol_probe_count;
  snapshot.queried_class_name = BorrowRuntimeCString(queried_node.class_name);
  snapshot.resolved_class_name = BorrowRuntimeCString(resolved_node.class_name);
  snapshot.selector = selector_slot.selector;
  snapshot.return_type_name = NullableRuntimeCString(entry.return_type_name);
  snapshot.return_kind = RuntimeMethodReturnKindName(
      ClassifyRuntimeReturnType(entry.return_type_name));
  snapshot.category_name =
      category_record != nullptr
          ? NullableRuntimeCString(category_record->category_name)
          : nullptr;
  return true;
}

bool PopulateReflectionMethodUnlocked(
    RuntimeState &state, const RealizedClassNode &start_node,
    const char *canonical_selector, const SelectorSlot &selector_slot,
    DispatchFamily family, objc3_runtime_reflection_method_snapshot &snapshot) {
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = &start_node;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;
  while (node != nullptr && visited.insert(node).second) {
    if (node->bundle == nullptr || node->image == nullptr) {
      snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
      return false;
    }
    for (auto category_it = node->attached_category_records.rbegin();
         category_it != node->attached_category_records.rend(); ++category_it) {
      const EmittedCategoryRecord *category_record = *category_it;
      ++category_probe_count;
      if (category_record == nullptr ||
          MethodListIsMalformed(
              SelectRuntimeMethodListRef(*category_record, family))) {
        snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
        return false;
      }
      const EmittedMethodListEntry *entry = FindMatchingMethodEntry(
          SelectRuntimeMethodListRef(*category_record, family),
          canonical_selector);
      if (entry != nullptr) {
        return TryPopulateReflectionMethodFromEntry(
            start_node, *node, *entry, category_record, selector_slot.handle,
            family, category_probe_count, protocol_probe_count, snapshot);
      }
    }

    const EmittedClassRecord &class_record =
        family == DispatchFamily::Class ? node->bundle->metaclass_record
                                        : node->bundle->class_record;
    if (MethodListIsMalformed(class_record.method_list_ref)) {
      snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
      return false;
    }
    const EmittedMethodListEntry *entry = FindMatchingMethodEntry(
        class_record.method_list_ref, canonical_selector);
    if (entry != nullptr) {
      return TryPopulateReflectionMethodFromEntry(
          start_node, *node, *entry, nullptr, selector_slot.handle, family,
          category_probe_count, protocol_probe_count, snapshot);
    }
    protocol_probe_count += AggregateCount(class_record.adopted_protocol_refs);
    node = node->has_super_node
               ? &state.realized_class_nodes[node->super_node_index]
               : nullptr;
  }
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  return false;
}

const EmittedProtocolRecord *
FindConcreteProtocolRecordUnlocked(const RuntimeState &state,
                                   const char *protocol_name, int &status) {
  status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  if (!RuntimeReflectionCStringPresent(protocol_name)) {
    status = OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY;
    return nullptr;
  }
  const EmittedProtocolRecord *matched_record = nullptr;
  for (const RegisteredImageMetadata *image : OrderedClassGraphImages(state)) {
    if (image == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < image->protocol_descriptor_count;
         ++index) {
      const auto *record = static_cast<const EmittedProtocolRecord *>(
          RuntimeAggregateEntry(image->protocol_descriptor_root, index));
      if (record == nullptr || record->protocol_name == nullptr) {
        status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
        return nullptr;
      }
      if (std::strcmp(record->protocol_name, protocol_name) != 0) {
        continue;
      }
      if (record->is_forward_declaration) {
        status = OBJC3_RUNTIME_REFLECTION_STATUS_UNAVAILABLE;
        continue;
      }
      if (record->owner_identity == nullptr) {
        status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
        return nullptr;
      }
      if (matched_record != nullptr &&
          (matched_record->owner_identity == nullptr ||
           std::strcmp(matched_record->owner_identity,
                       record->owner_identity) != 0)) {
        status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
        return nullptr;
      }
      if (matched_record == nullptr) {
        matched_record = record;
      }
      status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
    }
  }
  return matched_record;
}

void PopulateReflectionProtocolSnapshot(
    const EmittedProtocolRecord &record,
    objc3_runtime_reflection_protocol_snapshot &snapshot) {
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot.found = 1;
  snapshot.inherited_protocol_count =
      AggregateCount(record.inherited_protocol_refs);
  snapshot.property_count = record.property_count;
  snapshot.method_count = record.method_count;
  snapshot.instance_method_count = record.instance_method_count;
  snapshot.class_method_count = record.class_method_count;
  snapshot.protocol_name = NullableRuntimeCString(record.protocol_name);
}

void PopulateReflectionCategorySnapshot(
    const EmittedCategoryRecord &record,
    objc3_runtime_reflection_category_snapshot &snapshot) {
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot.found = 1;
  snapshot.adopted_protocol_count =
      AggregateCount(record.adopted_protocol_refs);
  snapshot.property_count = record.property_count;
  snapshot.instance_method_count = record.instance_method_count;
  snapshot.class_method_count = record.class_method_count;
  snapshot.class_name = NullableRuntimeCString(record.class_name);
  snapshot.category_name = NullableRuntimeCString(record.category_name);
  snapshot.record_kind = NullableRuntimeCString(record.record_kind);
}

} // namespace
} // namespace objc3c::runtime

extern "C" uint32_t objc3_runtime_reflection_api_abi_version(void) {
  return OBJC3_RUNTIME_REFLECTION_ABI_VERSION;
}

extern "C" uint64_t objc3_runtime_reflection_surface_count(void) {
  return objc3c::runtime::PublicReflectionSurfaceCount();
}

extern "C" int objc3_runtime_copy_reflection_surface(
    uint64_t index, objc3_runtime_reflection_surface_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionSurfaceSnapshot(*snapshot);
  if (index >= objc3c::runtime::PublicReflectionSurfaceCount()) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  }
  objc3c::runtime::PopulateReflectionSurfaceSnapshot(
      objc3c::runtime::kPublicReflectionSurfaces[index], *snapshot);
  return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_reflection_surface_by_kind(
    int surface_kind, objc3_runtime_reflection_surface_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionSurfaceSnapshot(*snapshot);
  if (surface_kind <= OBJC3_RUNTIME_REFLECTION_SURFACE_INVALID) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }
  for (const auto &record : objc3c::runtime::kPublicReflectionSurfaces) {
    if (record.surface_kind == surface_kind) {
      objc3c::runtime::PopulateReflectionSurfaceSnapshot(record, *snapshot);
      return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
    }
  }
  return objc3c::runtime::PublishReflectionStatus(
      OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, &snapshot->status);
}

extern "C" int objc3_runtime_copy_reflection_state(
    objc3_runtime_reflection_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionStateSnapshot(*snapshot);

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot->realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->root_class_count = state.realized_root_class_count;
  snapshot->reflectable_property_count =
      objc3c::runtime::CountPublicReflectablePropertiesUnlocked(state);
  snapshot->attached_category_count = state.realized_attached_category_count;
  snapshot->protocol_descriptor_count =
      objc3c::runtime::CountConcreteProtocolsUnlocked(state);
  snapshot->protocol_conformance_edge_count =
      state.realized_protocol_conformance_edge_count;
  snapshot->selector_table_entry_count =
      static_cast<std::uint64_t>(state.selector_slots.size());
  snapshot->metadata_backed_selector_count =
      state.metadata_backed_selector_count;
  snapshot->dynamic_selector_count = state.dynamic_selector_count;
  snapshot->method_cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->class_graph_generation = state.class_graph_generation;
  snapshot->category_attachment_generation =
      state.category_attachment_generation;
  snapshot->protocol_declaration_generation =
      state.protocol_declaration_generation;
  snapshot->storage_surface_generation = state.storage_surface_generation;
  snapshot->method_surface_generation = state.method_surface_generation;
  return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_reflection_class(
    const char *class_name, objc3_runtime_reflection_class_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionClassSnapshot(*snapshot);

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  const objc3c::runtime::RealizedClassNode *node =
      objc3c::runtime::FindUniqueRealizedClassNodeUnlocked(state, class_name,
                                                           status);
  if (node == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(status, &snapshot->status);
  }
  objc3c::runtime::PopulateReflectionClassSnapshot(state, *node, *snapshot);
  return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_reflection_property(
    const char *class_name, const char *property_name,
    objc3_runtime_reflection_property_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionPropertySnapshot(*snapshot);
  if (!objc3c::runtime::RuntimeReflectionCStringPresent(class_name) ||
      !objc3c::runtime::RuntimeReflectionCStringPresent(property_name)) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  const objc3c::runtime::RealizedClassNode *start_node =
      objc3c::runtime::FindUniqueRealizedClassNodeUnlocked(state, class_name,
                                                           status);
  if (start_node == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(status, &snapshot->status);
  }

  const objc3c::runtime::RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const objc3c::runtime::RealizedPropertyAccessor *accessor =
      objc3c::runtime::FindRuntimePropertyAccessorByNameUnlocked(
          state, *start_node, property_name, resolved_node, inherited,
          used_cache);
  if (accessor == nullptr || resolved_node == nullptr ||
      accessor->property_descriptor == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, &snapshot->status);
  }
  objc3c::runtime::PopulateReflectionPropertySnapshot(
      *start_node, *resolved_node, *accessor, inherited, *snapshot);
  (void)used_cache;
  return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_reflection_method(
    const char *class_name, const char *selector, int family,
    objc3_runtime_reflection_method_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionMethodSnapshot(*snapshot);
  if (!objc3c::runtime::RuntimeReflectionCStringPresent(class_name) ||
      !objc3c::runtime::RuntimeReflectionCStringPresent(selector)) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }
  const objc3c::runtime::DispatchFamily dispatch_family =
      objc3c::runtime::ReflectionDispatchFamily(family);
  if (!objc3c::runtime::RuntimeDispatchFamilyIsValid(dispatch_family)) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }
  snapshot->family = family;

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  const objc3c::runtime::RealizedClassNode *start_node =
      objc3c::runtime::FindUniqueRealizedClassNodeUnlocked(state, class_name,
                                                           status);
  if (start_node == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(status, &snapshot->status);
  }

  const char *canonical_selector =
      objc3c::runtime::NormalizeRuntimeSelectorSpelling(selector);
  if (canonical_selector == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }
  const objc3c::runtime::SelectorSlot *selector_slot =
      objc3c::runtime::FindSelectorSlotByCanonicalSpellingUnlocked(
          state, canonical_selector);
  if (selector_slot == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, &snapshot->status);
  }

  objc3c::runtime::PopulateReflectionMethodUnlocked(
      state, *start_node, canonical_selector, *selector_slot, dispatch_family,
      *snapshot);
  return snapshot->status;
}

extern "C" int objc3_runtime_copy_reflection_protocol(
    const char *protocol_name,
    objc3_runtime_reflection_protocol_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionProtocolSnapshot(*snapshot);

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  const objc3c::runtime::EmittedProtocolRecord *record =
      objc3c::runtime::FindConcreteProtocolRecordUnlocked(state, protocol_name,
                                                          status);
  if (record == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(status, &snapshot->status);
  }
  objc3c::runtime::PopulateReflectionProtocolSnapshot(*record, *snapshot);
  return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_reflection_protocol_conformance(
    const char *class_name, const char *protocol_name,
    objc3_runtime_reflection_protocol_conformance_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionProtocolConformanceSnapshot(*snapshot);
  if (!objc3c::runtime::RuntimeReflectionCStringPresent(class_name) ||
      !objc3c::runtime::RuntimeReflectionCStringPresent(protocol_name)) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  int class_status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  const objc3c::runtime::RealizedClassNode *start_node =
      objc3c::runtime::FindUniqueRealizedClassNodeUnlocked(state, class_name,
                                                           class_status);
  state.last_protocol_conformance_class_name = class_name;
  state.last_protocol_conformance_protocol_name = protocol_name;
  state.last_protocol_conformance_matched_class_name.clear();
  state.last_protocol_conformance_failure_reason.clear();
  snapshot->class_name = objc3c::runtime::BorrowRuntimeCString(
      state.last_protocol_conformance_class_name);
  snapshot->protocol_name = objc3c::runtime::BorrowRuntimeCString(
      state.last_protocol_conformance_protocol_name);
  snapshot->protocol_found =
      objc3c::runtime::ProtocolExistsByNameUnlocked(state, protocol_name) ? 1
                                                                          : 0;
  if (start_node == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(class_status,
                                                    &snapshot->status);
  }
  snapshot->class_found = 1;
  snapshot->attached_category_count =
      static_cast<std::uint64_t>(start_node->attached_category_records.size());

  objc3c::runtime::ProtocolConformanceMatch match;
  std::string failure_reason;
  if (objc3c::runtime::QueryRealizedClassProtocolConformanceUnlocked(
          state, start_node, protocol_name, snapshot->visited_protocol_count,
          match, failure_reason)) {
    snapshot->status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
    snapshot->conforms = 1;
    snapshot->matched_from_category = match.matched_from_category ? 1 : 0;
    snapshot->matched_from_superclass = match.matched_from_superclass ? 1 : 0;
    snapshot->matched_via_inherited_protocol =
        match.matched_via_inherited_protocol ? 1 : 0;
    snapshot->matched_protocol_depth = match.matched_protocol_depth;
    state.last_protocol_conformance_matched_class_name =
        match.matched_class_name;
    snapshot->matched_class_name = objc3c::runtime::BorrowRuntimeCString(
        state.last_protocol_conformance_matched_class_name);
    return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  }
  if (!failure_reason.empty()) {
    state.last_protocol_conformance_failure_reason = failure_reason;
    snapshot->failure_reason = objc3c::runtime::BorrowRuntimeCString(
        state.last_protocol_conformance_failure_reason);
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA, &snapshot->status);
  }
  return objc3c::runtime::PublishReflectionStatus(
      OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, &snapshot->status);
}

extern "C" int objc3_runtime_copy_reflection_category(
    const char *class_name, const char *category_name,
    objc3_runtime_reflection_category_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionCategorySnapshot(*snapshot);
  if (!objc3c::runtime::RuntimeReflectionCStringPresent(class_name) ||
      !objc3c::runtime::RuntimeReflectionCStringPresent(category_name)) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  const objc3c::runtime::RealizedClassNode *node =
      objc3c::runtime::FindUniqueRealizedClassNodeUnlocked(state, class_name,
                                                           status);
  if (node == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(status, &snapshot->status);
  }
  for (const objc3c::runtime::EmittedCategoryRecord *record :
       node->attached_category_records) {
    if (record == nullptr || record->category_name == nullptr) {
      return objc3c::runtime::PublishReflectionStatus(
          OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA,
          &snapshot->status);
    }
    if (std::strcmp(record->category_name, category_name) == 0) {
      objc3c::runtime::PopulateReflectionCategorySnapshot(*record, *snapshot);
      return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
    }
  }
  return objc3c::runtime::PublishReflectionStatus(
      OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, &snapshot->status);
}

extern "C" int objc3_runtime_copy_reflection_selector(
    const char *selector,
    objc3_runtime_reflection_selector_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionSelectorSnapshot(*snapshot);
  if (!objc3c::runtime::RuntimeReflectionCStringPresent(selector)) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const char *canonical_selector =
      objc3c::runtime::NormalizeRuntimeSelectorSpelling(selector);
  if (canonical_selector == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot->status);
  }
  const objc3c::runtime::SelectorSlot *slot =
      objc3c::runtime::FindSelectorSlotByCanonicalSpellingUnlocked(
          state, canonical_selector);
  if (slot == nullptr) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, &snapshot->status);
  }
  snapshot->status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot->found = 1;
  snapshot->metadata_backed = slot->metadata_backed ? 1 : 0;
  snapshot->stable_id = slot->handle.stable_id;
  snapshot->metadata_provider_count = slot->metadata_provider_count;
  snapshot->first_registration_order_ordinal =
      slot->first_registration_order_ordinal;
  snapshot->last_registration_order_ordinal =
      slot->last_registration_order_ordinal;
  snapshot->canonical_selector = slot->handle.selector;
  return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
}
