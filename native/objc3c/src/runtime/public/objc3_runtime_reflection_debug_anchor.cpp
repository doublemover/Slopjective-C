#include "runtime/public/objc3_runtime_reflection.h"

#include "runtime/classes/class_metadata_tables.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/method_list_resolution.h"
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
#include <unordered_set>

namespace objc3c::runtime {
namespace {

constexpr const char *kDebugAnchorPolicy =
    "runtime-owned debug anchors fail closed on missing, malformed, or stale "
    "metadata";
constexpr const char *kDebugAnchorSourceIdentityModel =
    "module plus translation-unit identity key plus runtime metadata identity";

bool RuntimeReflectionCStringPresent(const char *value) {
  return value != nullptr && value[0] != '\0';
}

int PublishReflectionStatus(int status, int *target_status) {
  if (target_status != nullptr) {
    *target_status = status;
  }
  return status;
}

const char *NullableRuntimeCString(const char *value) {
  return value != nullptr && value[0] != '\0' ? value : nullptr;
}

void InitializeReflectionDebugAnchorSnapshot(
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_ABI_VERSION;
  snapshot.snapshot_size =
      sizeof(objc3_runtime_reflection_debug_anchor_snapshot);
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  snapshot.anchor_kind = OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_INVALID;
  snapshot.method_family = OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID;
  snapshot.anchor_policy = kDebugAnchorPolicy;
  snapshot.source_identity_model = kDebugAnchorSourceIdentityModel;
}

std::uint64_t AggregateCount(const objc3_runtime_pointer_aggregate *aggregate) {
  return aggregate != nullptr ? aggregate->count : 0;
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
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr) {
      return true;
    }
  }
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

const EmittedProtocolRecord *
FindConcreteProtocolRecordAtUnlocked(const RuntimeState &state,
                                     std::uint64_t target_index, int &status) {
  status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  std::uint64_t concrete_index = 0;
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
      if (record->is_forward_declaration) {
        continue;
      }
      if (record->owner_identity == nullptr) {
        status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
        return nullptr;
      }
      if (concrete_index == target_index) {
        status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
        return record;
      }
      ++concrete_index;
    }
  }
  return nullptr;
}

bool ReflectionDebugAnchorKindValid(int anchor_kind) {
  switch (anchor_kind) {
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS:
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CATEGORY:
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROTOCOL:
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY:
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_IVAR:
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_METHOD:
    return true;
  default:
    return false;
  }
}

std::uint64_t ReflectionDebugAnchorGeneration(const RuntimeState &state,
                                              int anchor_kind) {
  switch (anchor_kind) {
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CATEGORY:
    return state.category_attachment_generation;
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROTOCOL:
    return state.protocol_declaration_generation;
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY:
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_IVAR:
    return state.storage_surface_generation;
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_METHOD:
    return state.method_surface_generation;
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS:
  default:
    return state.class_graph_generation;
  }
}

void PopulateReflectionDebugAnchorGenerations(
    const RuntimeState &state, int anchor_kind,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  snapshot.anchor_generation =
      ReflectionDebugAnchorGeneration(state, anchor_kind);
  snapshot.class_graph_generation = state.class_graph_generation;
  snapshot.category_attachment_generation = state.category_attachment_generation;
  snapshot.protocol_declaration_generation =
      state.protocol_declaration_generation;
  snapshot.storage_surface_generation = state.storage_surface_generation;
  snapshot.method_surface_generation = state.method_surface_generation;
}

bool RegisteredImageHasDebugSourceIdentity(
    const RegisteredImageMetadata *image) {
  return image != nullptr && RuntimeCStringSnapshotHasValue(image->module_name) &&
         RuntimeCStringSnapshotHasValue(image->translation_unit_identity_key);
}

const RegisteredImageMetadata *FindCategoryRecordImageUnlocked(
    const RuntimeState &state, const EmittedCategoryRecord *target) {
  if (target == nullptr) {
    return nullptr;
  }
  for (const RegisteredImageMetadata *image : OrderedClassGraphImages(state)) {
    if (image == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < image->category_descriptor_count;
         ++index) {
      if (RuntimeAggregateEntry(image->category_descriptor_root, index) ==
          target) {
        return image;
      }
    }
  }
  return nullptr;
}

const RegisteredImageMetadata *FindProtocolRecordImageUnlocked(
    const RuntimeState &state, const EmittedProtocolRecord *target) {
  if (target == nullptr) {
    return nullptr;
  }
  for (const RegisteredImageMetadata *image : OrderedClassGraphImages(state)) {
    if (image == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < image->protocol_descriptor_count;
         ++index) {
      if (RuntimeAggregateEntry(image->protocol_descriptor_root, index) ==
          target) {
        return image;
      }
    }
  }
  return nullptr;
}

const RegisteredImageMetadata *FindPropertyDescriptorImageUnlocked(
    const RuntimeState &state, const EmittedPropertyDescriptor *target) {
  if (target == nullptr) {
    return nullptr;
  }
  for (const RegisteredImageMetadata *image : OrderedClassGraphImages(state)) {
    if (image == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < image->property_descriptor_count;
         ++index) {
      if (RuntimeAggregateEntry(image->property_descriptor_root, index) ==
          target) {
        return image;
      }
    }
  }
  return nullptr;
}

const RegisteredImageMetadata *FindIvarDescriptorImageUnlocked(
    const RuntimeState &state, const EmittedIvarDescriptor *target) {
  if (target == nullptr) {
    return nullptr;
  }
  for (const RegisteredImageMetadata *image : OrderedClassGraphImages(state)) {
    if (image == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < image->ivar_descriptor_count;
         ++index) {
      if (RuntimeAggregateEntry(image->ivar_descriptor_root, index) == target) {
        return image;
      }
    }
  }
  return nullptr;
}

int PublishMissingDebugAnchor(
    int status, objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  if (status == OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND) {
    snapshot.missing_anchor = 1;
  }
  return PublishReflectionStatus(status, &snapshot.status);
}

bool PopulateReflectionDebugAnchorCommon(
    const RuntimeState &state, int anchor_kind,
    const RegisteredImageMetadata *image, const char *runtime_identity_key,
    std::uint64_t registration_order_ordinal,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  if (!RegisteredImageHasDebugSourceIdentity(image) ||
      !RuntimeReflectionCStringPresent(runtime_identity_key)) {
    snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
    return false;
  }
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  snapshot.found = 1;
  snapshot.anchor_kind = anchor_kind;
  snapshot.runtime_owned = 1;
  snapshot.source_identity_backed = 1;
  snapshot.replayable = 1;
  snapshot.registration_order_ordinal = registration_order_ordinal;
  PopulateReflectionDebugAnchorGenerations(state, anchor_kind, snapshot);
  snapshot.module_name = BorrowRuntimeCString(image->module_name);
  snapshot.translation_unit_identity_key =
      BorrowRuntimeCString(image->translation_unit_identity_key);
  snapshot.source_path = snapshot.translation_unit_identity_key;
  snapshot.runtime_identity_key = NullableRuntimeCString(runtime_identity_key);
  snapshot.debug_projection_key = snapshot.runtime_identity_key;
  return true;
}

bool PopulateReflectionClassDebugAnchor(
    const RuntimeState &state, const RealizedClassNode &node,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  if (!RuntimeCStringSnapshotHasValue(node.class_owner_identity)) {
    snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
    return false;
  }
  if (!PopulateReflectionDebugAnchorCommon(
          state, OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS, node.image,
          BorrowRuntimeCString(node.class_owner_identity),
          node.registration_order_ordinal, snapshot)) {
    return false;
  }
  snapshot.class_name = BorrowRuntimeCString(node.class_name);
  return true;
}

bool PopulateReflectionCategoryDebugAnchor(
    const RuntimeState &state, const RealizedClassNode &node,
    const EmittedCategoryRecord &record,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  const RegisteredImageMetadata *image =
      FindCategoryRecordImageUnlocked(state, &record);
  if (image == nullptr) {
    image = node.image;
  }
  if (!PopulateReflectionDebugAnchorCommon(
          state, OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CATEGORY, image,
          record.owner_identity, node.registration_order_ordinal, snapshot)) {
    return false;
  }
  snapshot.class_name = NullableRuntimeCString(record.class_name);
  snapshot.category_name = NullableRuntimeCString(record.category_name);
  return true;
}

bool PopulateReflectionProtocolDebugAnchor(
    const RuntimeState &state, const EmittedProtocolRecord &record,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  const RegisteredImageMetadata *image =
      FindProtocolRecordImageUnlocked(state, &record);
  if (!PopulateReflectionDebugAnchorCommon(
          state, OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROTOCOL, image,
          record.owner_identity,
          image != nullptr ? image->registration_order_ordinal : 0, snapshot)) {
    return false;
  }
  snapshot.protocol_name = NullableRuntimeCString(record.protocol_name);
  return true;
}

bool PopulateReflectionPropertyDebugAnchor(
    const RuntimeState &state, const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  if (accessor.property_descriptor == nullptr) {
    snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
    return false;
  }
  const EmittedPropertyDescriptor &descriptor = *accessor.property_descriptor;
  const RegisteredImageMetadata *image =
      FindPropertyDescriptorImageUnlocked(state, &descriptor);
  if (image == nullptr) {
    image = resolved_node.image;
  }
  if (!PopulateReflectionDebugAnchorCommon(
          state, OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY, image,
          descriptor.owner_identity, resolved_node.registration_order_ordinal,
          snapshot)) {
    return false;
  }
  snapshot.class_name = BorrowRuntimeCString(resolved_node.class_name);
  snapshot.property_name = NullableRuntimeCString(descriptor.property_name);
  snapshot.ivar_binding_symbol =
      NullableRuntimeCString(descriptor.ivar_binding_symbol);
  snapshot.ivar_layout_replay_key =
      NullableRuntimeCString(descriptor.ivar_layout_replay_key);
  return true;
}

bool PopulateReflectionIvarDebugAnchor(
    const RuntimeState &state, const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  if (accessor.property_descriptor == nullptr ||
      accessor.ivar_descriptor == nullptr) {
    snapshot.status = accessor.property_descriptor == nullptr
                          ? OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA
                          : OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
    snapshot.missing_anchor =
        accessor.ivar_descriptor == nullptr && accessor.property_descriptor != nullptr
            ? 1
            : 0;
    return false;
  }
  const EmittedPropertyDescriptor &property = *accessor.property_descriptor;
  const EmittedIvarDescriptor &ivar = *accessor.ivar_descriptor;
  const RegisteredImageMetadata *image =
      FindIvarDescriptorImageUnlocked(state, &ivar);
  if (image == nullptr) {
    image = FindPropertyDescriptorImageUnlocked(state, &property);
  }
  if (image == nullptr) {
    image = resolved_node.image;
  }
  if (!PopulateReflectionDebugAnchorCommon(
          state, OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_IVAR, image,
          ivar.owner_identity, resolved_node.registration_order_ordinal,
          snapshot)) {
    return false;
  }
  snapshot.class_name = BorrowRuntimeCString(resolved_node.class_name);
  snapshot.property_name = NullableRuntimeCString(property.property_name);
  snapshot.ivar_binding_symbol =
      NullableRuntimeCString(ivar.ivar_binding_symbol);
  snapshot.ivar_layout_replay_key = NullableRuntimeCString(ivar.layout_replay_key);
  return true;
}

bool PopulateReflectionMethodDebugAnchor(
    const RuntimeState &state, const RealizedClassNode &resolved_node,
    const EmittedMethodListEntry &entry,
    const EmittedCategoryRecord *category_record, DispatchFamily family,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  const RegisteredImageMetadata *image =
      category_record != nullptr
          ? FindCategoryRecordImageUnlocked(state, category_record)
          : resolved_node.image;
  if (image == nullptr) {
    image = resolved_node.image;
  }
  if (!PopulateReflectionDebugAnchorCommon(
          state, OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_METHOD, image,
          entry.owner_identity, resolved_node.registration_order_ordinal,
          snapshot)) {
    return false;
  }
  snapshot.method_family =
      family == DispatchFamily::Class
          ? OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_CLASS
          : OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INSTANCE;
  snapshot.class_name = BorrowRuntimeCString(resolved_node.class_name);
  snapshot.category_name =
      category_record != nullptr
          ? NullableRuntimeCString(category_record->category_name)
          : nullptr;
  snapshot.selector = NullableRuntimeCString(entry.selector);
  return true;
}

const EmittedMethodListEntry *RuntimeMethodListEntryAt(
    const EmittedMethodListRef *method_list_ref, std::uint64_t index,
    int &status) {
  status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  if (method_list_ref == nullptr || index >= method_list_ref->count) {
    return nullptr;
  }
  if (MethodListIsMalformed(method_list_ref)) {
    status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
    return nullptr;
  }
  const auto *header = static_cast<const EmittedMethodListHeader *>(
      method_list_ref->method_list);
  const EmittedMethodListEntry *entries = RuntimeMethodListEntries(header);
  if (entries == nullptr) {
    status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
    return nullptr;
  }
  status = OBJC3_RUNTIME_REFLECTION_STATUS_OK;
  return &entries[static_cast<std::size_t>(index)];
}

std::uint64_t ReflectionDebugAnchorMethodListCount(
    const EmittedMethodListRef *method_list_ref) {
  if (method_list_ref == nullptr || MethodListIsMalformed(method_list_ref)) {
    return 0;
  }
  return method_list_ref->count;
}

std::uint64_t ReflectionDebugAnchorCountUnlocked(const RuntimeState &state) {
  std::uint64_t count = 0;
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    ++count;
    count += static_cast<std::uint64_t>(node.runtime_property_accessors.size());
    for (const RealizedPropertyAccessor &accessor :
         node.runtime_property_accessors) {
      if (accessor.ivar_descriptor != nullptr) {
        ++count;
      }
    }
    if (node.bundle != nullptr) {
      count += ReflectionDebugAnchorMethodListCount(
          node.bundle->class_record.method_list_ref);
      count += ReflectionDebugAnchorMethodListCount(
          node.bundle->metaclass_record.method_list_ref);
    }
    for (const EmittedCategoryRecord *category_record :
         node.attached_category_records) {
      if (category_record == nullptr) {
        continue;
      }
      ++count;
      count += ReflectionDebugAnchorMethodListCount(
          SelectRuntimeMethodListRef(*category_record,
                                     DispatchFamily::Instance));
      count += ReflectionDebugAnchorMethodListCount(
          SelectRuntimeMethodListRef(*category_record, DispatchFamily::Class));
    }
  }
  count += CountConcreteProtocolsUnlocked(state);
  return count;
}

bool AdvanceReflectionDebugAnchorCursor(std::uint64_t &cursor,
                                        std::uint64_t target_index) {
  if (cursor == target_index) {
    return true;
  }
  ++cursor;
  return false;
}

bool TryPopulateReflectionDebugAnchorMethodAt(
    const RuntimeState &state, const RealizedClassNode &node,
    const EmittedCategoryRecord *category_record,
    const EmittedMethodListRef *method_list_ref, DispatchFamily family,
    std::uint64_t target_index, std::uint64_t &cursor,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  if (method_list_ref == nullptr || method_list_ref->count == 0) {
    return false;
  }
  if (MethodListIsMalformed(method_list_ref)) {
    if (target_index >= cursor &&
        target_index < cursor + method_list_ref->count) {
      snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA;
      return true;
    }
    cursor += method_list_ref->count;
    return false;
  }
  for (std::uint64_t index = 0; index < method_list_ref->count; ++index) {
    if (cursor == target_index) {
      int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
      const EmittedMethodListEntry *entry =
          RuntimeMethodListEntryAt(method_list_ref, index, status);
      if (entry == nullptr) {
        snapshot.status = status;
        return true;
      }
      PopulateReflectionMethodDebugAnchor(state, node, *entry, category_record,
                                          family, snapshot);
      return true;
    }
    ++cursor;
  }
  return false;
}

bool TryPopulateReflectionDebugAnchorAtUnlocked(
    const RuntimeState &state, std::uint64_t target_index,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  std::uint64_t cursor = 0;
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    if (AdvanceReflectionDebugAnchorCursor(cursor, target_index)) {
      PopulateReflectionClassDebugAnchor(state, node, snapshot);
      return true;
    }
    for (const RealizedPropertyAccessor &accessor :
         node.runtime_property_accessors) {
      if (AdvanceReflectionDebugAnchorCursor(cursor, target_index)) {
        PopulateReflectionPropertyDebugAnchor(state, node, accessor, snapshot);
        return true;
      }
      if (accessor.ivar_descriptor != nullptr) {
        if (AdvanceReflectionDebugAnchorCursor(cursor, target_index)) {
          PopulateReflectionIvarDebugAnchor(state, node, accessor, snapshot);
          return true;
        }
      }
    }
    if (node.bundle != nullptr) {
      if (TryPopulateReflectionDebugAnchorMethodAt(
              state, node, nullptr, node.bundle->class_record.method_list_ref,
              DispatchFamily::Instance, target_index, cursor, snapshot)) {
        return true;
      }
      if (TryPopulateReflectionDebugAnchorMethodAt(
              state, node, nullptr,
              node.bundle->metaclass_record.method_list_ref,
              DispatchFamily::Class, target_index, cursor, snapshot)) {
        return true;
      }
    }
    for (const EmittedCategoryRecord *category_record :
         node.attached_category_records) {
      if (category_record == nullptr) {
        continue;
      }
      if (AdvanceReflectionDebugAnchorCursor(cursor, target_index)) {
        PopulateReflectionCategoryDebugAnchor(state, node, *category_record,
                                              snapshot);
        return true;
      }
      if (TryPopulateReflectionDebugAnchorMethodAt(
              state, node, category_record,
              SelectRuntimeMethodListRef(*category_record,
                                         DispatchFamily::Instance),
              DispatchFamily::Instance, target_index, cursor, snapshot)) {
        return true;
      }
      if (TryPopulateReflectionDebugAnchorMethodAt(
              state, node, category_record,
              SelectRuntimeMethodListRef(*category_record,
                                         DispatchFamily::Class),
              DispatchFamily::Class, target_index, cursor, snapshot)) {
        return true;
      }
    }
  }
  std::uint64_t protocol_index = 0;
  int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  while (const EmittedProtocolRecord *record =
             FindConcreteProtocolRecordAtUnlocked(state, protocol_index,
                                                  status)) {
    if (AdvanceReflectionDebugAnchorCursor(cursor, target_index)) {
      PopulateReflectionProtocolDebugAnchor(state, *record, snapshot);
      return true;
    }
    ++protocol_index;
  }
  if (status == OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA) {
    snapshot.status = status;
    return true;
  }
  snapshot.status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  snapshot.missing_anchor = 1;
  return false;
}

int PopulateReflectionPropertyOrIvarDebugAnchorUnlocked(
    RuntimeState &state, int anchor_kind, const char *class_name,
    const char *property_name,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  const RealizedClassNode *start_node =
      FindUniqueRealizedClassNodeUnlocked(state, class_name, status);
  if (start_node == nullptr) {
    return PublishMissingDebugAnchor(status, snapshot);
  }
  const RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const RealizedPropertyAccessor *accessor =
      FindRuntimePropertyAccessorByNameUnlocked(state, *start_node,
                                                property_name, resolved_node,
                                                inherited, used_cache);
  (void)inherited;
  (void)used_cache;
  if (accessor == nullptr || resolved_node == nullptr) {
    return PublishMissingDebugAnchor(
        OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, snapshot);
  }
  if (anchor_kind == OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY) {
    PopulateReflectionPropertyDebugAnchor(state, *resolved_node, *accessor,
                                          snapshot);
  } else {
    PopulateReflectionIvarDebugAnchor(state, *resolved_node, *accessor,
                                      snapshot);
  }
  return snapshot.status;
}

int PopulateReflectionMethodDebugAnchorUnlocked(
    RuntimeState &state, const char *class_name, const char *selector,
    int method_family,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  const DispatchFamily family = ReflectionDispatchFamily(method_family);
  if (!RuntimeDispatchFamilyIsValid(family)) {
    return PublishReflectionStatus(OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY,
                                   &snapshot.status);
  }
  int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
  const RealizedClassNode *start_node =
      FindUniqueRealizedClassNodeUnlocked(state, class_name, status);
  if (start_node == nullptr) {
    return PublishMissingDebugAnchor(status, snapshot);
  }
  const char *canonical_selector = NormalizeRuntimeSelectorSpelling(selector);
  if (canonical_selector == nullptr) {
    return PublishReflectionStatus(OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY,
                                   &snapshot.status);
  }
  if (FindSelectorSlotByCanonicalSpellingUnlocked(state, canonical_selector) ==
      nullptr) {
    return PublishMissingDebugAnchor(
        OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, snapshot);
  }
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = start_node;
  while (node != nullptr && visited.insert(node).second) {
    if (node->bundle == nullptr || node->image == nullptr) {
      return PublishReflectionStatus(
          OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA,
          &snapshot.status);
    }
    for (auto category_it = node->attached_category_records.rbegin();
         category_it != node->attached_category_records.rend(); ++category_it) {
      const EmittedCategoryRecord *category_record = *category_it;
      if (category_record == nullptr ||
          MethodListIsMalformed(SelectRuntimeMethodListRef(*category_record,
                                                           family))) {
        return PublishReflectionStatus(
            OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA,
            &snapshot.status);
      }
      const EmittedMethodListEntry *entry = FindMatchingMethodEntry(
          SelectRuntimeMethodListRef(*category_record, family),
          canonical_selector);
      if (entry != nullptr) {
        PopulateReflectionMethodDebugAnchor(state, *node, *entry,
                                            category_record, family, snapshot);
        return snapshot.status;
      }
    }
    const EmittedClassRecord &class_record =
        family == DispatchFamily::Class ? node->bundle->metaclass_record
                                        : node->bundle->class_record;
    if (MethodListIsMalformed(class_record.method_list_ref)) {
      return PublishReflectionStatus(
          OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA,
          &snapshot.status);
    }
    const EmittedMethodListEntry *entry =
        FindMatchingMethodEntry(class_record.method_list_ref,
                                canonical_selector);
    if (entry != nullptr) {
      PopulateReflectionMethodDebugAnchor(state, *node, *entry, nullptr, family,
                                          snapshot);
      return snapshot.status;
    }
    node = node->has_super_node
               ? &state.realized_class_nodes[node->super_node_index]
               : nullptr;
  }
  return PublishMissingDebugAnchor(OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND,
                                   snapshot);
}

int PopulateReflectionDebugAnchorByQueryUnlocked(
    RuntimeState &state, int anchor_kind, const char *container_name,
    const char *member_name, int method_family,
    objc3_runtime_reflection_debug_anchor_snapshot &snapshot) {
  snapshot.anchor_kind = anchor_kind;
  if (!ReflectionDebugAnchorKindValid(anchor_kind) ||
      !RuntimeReflectionCStringPresent(container_name)) {
    return PublishReflectionStatus(OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY,
                                   &snapshot.status);
  }
  switch (anchor_kind) {
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS: {
    int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
    const RealizedClassNode *node =
        FindUniqueRealizedClassNodeUnlocked(state, container_name, status);
    if (node == nullptr) {
      return PublishMissingDebugAnchor(status, snapshot);
    }
    PopulateReflectionClassDebugAnchor(state, *node, snapshot);
    return snapshot.status;
  }
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROTOCOL: {
    int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
    const EmittedProtocolRecord *record =
        FindConcreteProtocolRecordUnlocked(state, container_name, status);
    if (record == nullptr) {
      return PublishMissingDebugAnchor(status, snapshot);
    }
    PopulateReflectionProtocolDebugAnchor(state, *record, snapshot);
    return snapshot.status;
  }
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CATEGORY: {
    if (!RuntimeReflectionCStringPresent(member_name)) {
      return PublishReflectionStatus(
          OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot.status);
    }
    int status = OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND;
    const RealizedClassNode *node =
        FindUniqueRealizedClassNodeUnlocked(state, container_name, status);
    if (node == nullptr) {
      return PublishMissingDebugAnchor(status, snapshot);
    }
    for (const EmittedCategoryRecord *record :
         node->attached_category_records) {
      if (record == nullptr || record->category_name == nullptr) {
        return PublishReflectionStatus(
            OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA,
            &snapshot.status);
      }
      if (std::strcmp(record->category_name, member_name) == 0) {
        PopulateReflectionCategoryDebugAnchor(state, *node, *record, snapshot);
        return snapshot.status;
      }
    }
    return PublishMissingDebugAnchor(
        OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, snapshot);
  }
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY:
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_IVAR:
    if (!RuntimeReflectionCStringPresent(member_name)) {
      return PublishReflectionStatus(
          OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot.status);
    }
    return PopulateReflectionPropertyOrIvarDebugAnchorUnlocked(
        state, anchor_kind, container_name, member_name, snapshot);
  case OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_METHOD:
    if (!RuntimeReflectionCStringPresent(member_name)) {
      return PublishReflectionStatus(
          OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY, &snapshot.status);
    }
    return PopulateReflectionMethodDebugAnchorUnlocked(
        state, container_name, member_name, method_family, snapshot);
  default:
    return PublishReflectionStatus(OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY,
                                   &snapshot.status);
  }
}

} // namespace
} // namespace objc3c::runtime

extern "C" uint32_t objc3_runtime_reflection_debug_anchor_abi_version(void) {
  return OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_ABI_VERSION;
}

extern "C" uint64_t objc3_runtime_reflection_debug_anchor_count(void) {
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return objc3c::runtime::ReflectionDebugAnchorCountUnlocked(state);
}

extern "C" int objc3_runtime_copy_reflection_debug_anchor(
    int anchor_kind, const char *container_name, const char *member_name,
    int method_family,
    objc3_runtime_reflection_debug_anchor_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionDebugAnchorSnapshot(*snapshot);
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return objc3c::runtime::PopulateReflectionDebugAnchorByQueryUnlocked(
      state, anchor_kind, container_name, member_name, method_family,
      *snapshot);
}

extern "C" int objc3_runtime_copy_reflection_debug_anchor_with_generation(
    int anchor_kind, const char *container_name, const char *member_name,
    int method_family, uint64_t expected_anchor_generation,
    objc3_runtime_reflection_debug_anchor_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionDebugAnchorSnapshot(*snapshot);
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int status =
      objc3c::runtime::PopulateReflectionDebugAnchorByQueryUnlocked(
          state, anchor_kind, container_name, member_name, method_family,
          *snapshot);
  if (status != OBJC3_RUNTIME_REFLECTION_STATUS_OK) {
    return status;
  }
  if (snapshot->anchor_generation != expected_anchor_generation) {
    snapshot->stale_generation = 1;
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_STALE_ANCHOR, &snapshot->status);
  }
  return OBJC3_RUNTIME_REFLECTION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_reflection_debug_anchor_at(
    uint64_t index, objc3_runtime_reflection_debug_anchor_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeReflectionDebugAnchorSnapshot(*snapshot);
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (!objc3c::runtime::TryPopulateReflectionDebugAnchorAtUnlocked(
          state, index, *snapshot)) {
    return objc3c::runtime::PublishReflectionStatus(
        OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND, &snapshot->status);
  }
  return snapshot->status;
}
