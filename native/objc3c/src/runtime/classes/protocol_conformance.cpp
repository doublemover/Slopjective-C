#include "runtime/classes/protocol_conformance.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/images/multi_image_ordering.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

std::vector<const RegisteredImageMetadata *> OrderedProtocolImages(
    const RuntimeState &state);

namespace {

void RecordProtocolQueryFailure(std::string &failure_reason,
                                const char *reason) {
  if (failure_reason.empty() && reason != nullptr && reason[0] != '\0') {
    failure_reason = reason;
  }
}

bool RuntimeNonEmptyCString(const char *value) {
  return value != nullptr && value[0] != '\0';
}

const char *RuntimeProtocolCString(const char *value) {
  return value != nullptr ? value : "";
}

bool ValidateRuntimeProtocolMethodList(
    const EmittedMethodListRef *method_list_ref,
    const char *family_name,
    const char *protocol_name,
    std::string &diagnostic_reason) {
  if (method_list_ref == nullptr || method_list_ref->count == 0) {
    return true;
  }
  if (method_list_ref->method_list == nullptr ||
      !RuntimeNonEmptyCString(method_list_ref->owner_identity)) {
    diagnostic_reason =
        std::string("malformed protocol ") + family_name + " method list for " +
        RuntimeProtocolCString(protocol_name);
    return false;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    diagnostic_reason =
        std::string("malformed protocol ") + family_name +
        " method list count for " + RuntimeProtocolCString(protocol_name);
    return false;
  }
  const auto *entries =
      reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (!RuntimeNonEmptyCString(entry.selector) ||
        !RuntimeNonEmptyCString(entry.owner_identity) ||
        !RuntimeNonEmptyCString(entry.return_type_name)) {
      diagnostic_reason =
          std::string("malformed protocol ") + family_name +
          " method entry for " + RuntimeProtocolCString(protocol_name);
      return false;
    }
  }
  return true;
}

bool AddKnownProtocolRecords(
    const objc3_runtime_pointer_aggregate *protocol_descriptor_root,
    std::uint64_t protocol_descriptor_count,
    std::unordered_set<const EmittedProtocolRecord *> &known_protocol_records,
    std::unordered_map<std::string, std::string>
        &concrete_protocol_owner_by_name,
    std::string &diagnostic_reason) {
  for (std::uint64_t index = 0; index < protocol_descriptor_count; ++index) {
    const auto *record = static_cast<const EmittedProtocolRecord *>(
        RuntimeAggregateEntry(protocol_descriptor_root, index));
    if (record == nullptr) {
      diagnostic_reason = "protocol descriptor root contains a null protocol";
      return false;
    }
    if (!RuntimeNonEmptyCString(record->protocol_name) ||
        !RuntimeNonEmptyCString(record->owner_identity)) {
      diagnostic_reason = "protocol descriptor is missing identity";
      return false;
    }
    if (!ValidateRuntimeProtocolMethodList(
            record->instance_method_list_ref, "instance",
            record->protocol_name, diagnostic_reason) ||
        !ValidateRuntimeProtocolMethodList(
            record->class_method_list_ref, "class", record->protocol_name,
            diagnostic_reason)) {
      return false;
    }
    known_protocol_records.insert(record);
    if (record->is_forward_declaration) {
      continue;
    }
    const auto inserted = concrete_protocol_owner_by_name.emplace(
        record->protocol_name, record->owner_identity);
    if (!inserted.second &&
        inserted.first->second != std::string(record->owner_identity)) {
      diagnostic_reason =
          "conflicting protocol owner for " +
          std::string(record->protocol_name);
      return false;
    }
  }
  return true;
}

bool RuntimeProtocolReferenceAggregateIsSupported(
    const objc3_runtime_pointer_aggregate *protocol_refs,
    const std::unordered_set<const EmittedProtocolRecord *>
        &known_protocol_records,
    const std::string &context,
    std::string &diagnostic_reason) {
  if (protocol_refs == nullptr) {
    return true;
  }
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *record = static_cast<const EmittedProtocolRecord *>(
        RuntimeAggregateEntry(protocol_refs, index));
    if (record == nullptr ||
        known_protocol_records.find(record) == known_protocol_records.end()) {
      diagnostic_reason = "unknown protocol reference in " + context;
      return false;
    }
    if (record->is_forward_declaration) {
      diagnostic_reason = "forward protocol reference in " + context;
      return false;
    }
  }
  return true;
}

bool RuntimeRegisteredProtocolMetadataIsSupported(
    const objc3_runtime_registration_table *registration_table,
    const std::unordered_set<const EmittedProtocolRecord *>
        &known_protocol_records,
    std::string &diagnostic_reason) {
  for (std::uint64_t index = 0;
       index < registration_table->image_descriptor->protocol_descriptor_count;
       ++index) {
    const auto *record = static_cast<const EmittedProtocolRecord *>(
        RuntimeAggregateEntry(registration_table->protocol_descriptor_root,
                              index));
    if (record == nullptr) {
      diagnostic_reason = "protocol descriptor root contains a null protocol";
      return false;
    }
    if (!RuntimeProtocolReferenceAggregateIsSupported(
            record->inherited_protocol_refs, known_protocol_records,
            "protocol " + std::string(record->protocol_name),
            diagnostic_reason)) {
      return false;
    }
  }
  return true;
}

bool RuntimeClassProtocolReferencesAreSupported(
    const objc3_runtime_registration_table *registration_table,
    const std::unordered_set<const EmittedProtocolRecord *>
        &known_protocol_records,
    std::string &diagnostic_reason) {
  for (std::uint64_t index = 0;
       index < registration_table->image_descriptor->class_descriptor_count;
       ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        RuntimeAggregateEntry(registration_table->class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr) {
      diagnostic_reason =
          "class descriptor root contains malformed protocol refs";
      return false;
    }
    if (!RuntimeProtocolReferenceAggregateIsSupported(
            bundle->class_record.adopted_protocol_refs, known_protocol_records,
            "class " + std::string(bundle->class_record.class_name),
            diagnostic_reason) ||
        !RuntimeProtocolReferenceAggregateIsSupported(
            bundle->metaclass_record.adopted_protocol_refs,
            known_protocol_records,
            "metaclass " + std::string(bundle->class_record.class_name),
            diagnostic_reason)) {
      return false;
    }
  }
  return true;
}

bool RuntimeCategoryProtocolReferencesAreSupported(
    const objc3_runtime_registration_table *registration_table,
    const std::unordered_set<const EmittedProtocolRecord *>
        &known_protocol_records,
    std::string &diagnostic_reason) {
  for (std::uint64_t index = 0;
       index < registration_table->image_descriptor->category_descriptor_count;
       ++index) {
    const auto *record = static_cast<const EmittedCategoryRecord *>(
        RuntimeAggregateEntry(registration_table->category_descriptor_root,
                              index));
    if (record == nullptr || record->class_name == nullptr ||
        record->category_name == nullptr) {
      diagnostic_reason =
          "category descriptor root contains malformed protocol refs";
      return false;
    }
    if (!RuntimeProtocolReferenceAggregateIsSupported(
            record->adopted_protocol_refs, known_protocol_records,
            "category " + std::string(record->class_name) + "(" +
                std::string(record->category_name) + ")",
            diagnostic_reason)) {
      return false;
    }
  }
  return true;
}

struct RuntimeCategoryTargetClassRecord {
  std::string class_owner_identity;
  bool found = false;
  bool implementation_backed = false;
};

bool RuntimeCategoryTargetClassBundleHasImplementationBacking(
    const EmittedClassBundle &bundle) {
  return (bundle.class_record.method_list_ref != nullptr &&
          RuntimeNonEmptyCString(
              bundle.class_record.method_list_ref->owner_identity) &&
          std::string(bundle.class_record.method_list_ref->owner_identity)
                  .rfind("implementation:", 0) == 0) ||
         (bundle.metaclass_record.method_list_ref != nullptr &&
          RuntimeNonEmptyCString(
              bundle.metaclass_record.method_list_ref->owner_identity) &&
          std::string(bundle.metaclass_record.method_list_ref->owner_identity)
                  .rfind("implementation:", 0) == 0);
}

bool AddRuntimeCategoryTargetClassRecords(
    const objc3_runtime_pointer_aggregate *class_descriptor_root,
    std::uint64_t class_descriptor_count,
    std::unordered_map<std::string, RuntimeCategoryTargetClassRecord>
        &target_classes,
    std::string &diagnostic_reason) {
  for (std::uint64_t index = 0; index < class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        RuntimeAggregateEntry(class_descriptor_root, index));
    if (bundle == nullptr ||
        !RuntimeNonEmptyCString(bundle->class_record.class_name) ||
        !RuntimeNonEmptyCString(bundle->class_record.object_owner_identity)) {
      diagnostic_reason =
          "class descriptor root contains malformed category targets";
      return false;
    }
    RuntimeCategoryTargetClassRecord &target =
        target_classes[bundle->class_record.class_name];
    const bool implementation_backed =
        RuntimeCategoryTargetClassBundleHasImplementationBacking(*bundle);
    if (!target.found ||
        (!target.implementation_backed && implementation_backed)) {
      target.class_owner_identity = bundle->class_record.object_owner_identity;
      target.found = true;
      target.implementation_backed = implementation_backed;
    }
  }
  return true;
}

std::string RuntimeCategoryQualifiedName(const EmittedCategoryRecord &record) {
  return std::string(RuntimeProtocolCString(record.class_name)) + "(" +
         RuntimeProtocolCString(record.category_name) + ")";
}

bool RuntimeCategoryRecordKindIsSupported(const char *record_kind) {
  if (!RuntimeNonEmptyCString(record_kind)) {
    return false;
  }
  const std::string kind = record_kind;
  return kind == "interface" || kind == "implementation";
}

bool RuntimeCategoryRecordIdentityIsSupported(
    const EmittedCategoryRecord &record,
    std::string &diagnostic_reason) {
  if (!RuntimeNonEmptyCString(record.class_name) ||
      !RuntimeNonEmptyCString(record.category_name) ||
      !RuntimeNonEmptyCString(record.record_kind) ||
      !RuntimeNonEmptyCString(record.owner_identity) ||
      !RuntimeNonEmptyCString(record.class_owner_identity) ||
      !RuntimeNonEmptyCString(record.category_owner_identity)) {
    diagnostic_reason = "category descriptor is missing identity";
    return false;
  }
  if (!RuntimeCategoryRecordKindIsSupported(record.record_kind)) {
    diagnostic_reason = "unknown category record kind " +
                        std::string(record.record_kind) + " for " +
                        RuntimeCategoryQualifiedName(record);
    return false;
  }
  return true;
}

bool AddRuntimeCategoryRecordConflictEntries(
    const objc3_runtime_pointer_aggregate *category_descriptor_root,
    std::uint64_t category_descriptor_count,
    std::unordered_map<std::string, std::string> &category_owner_by_key,
    std::string &diagnostic_reason) {
  for (std::uint64_t index = 0; index < category_descriptor_count; ++index) {
    const auto *record = static_cast<const EmittedCategoryRecord *>(
        RuntimeAggregateEntry(category_descriptor_root, index));
    if (record == nullptr) {
      diagnostic_reason = "category descriptor root contains a null category";
      return false;
    }
    if (!RuntimeCategoryRecordIdentityIsSupported(*record,
                                                  diagnostic_reason)) {
      return false;
    }
    const std::string category_key =
        std::string(record->class_name) + "\n" +
        std::string(record->category_name) + "\n" +
        std::string(record->record_kind);
    const auto inserted =
        category_owner_by_key.emplace(category_key, record->owner_identity);
    if (!inserted.second &&
        inserted.first->second != std::string(record->owner_identity)) {
      diagnostic_reason = "conflicting category " +
                          std::string(record->record_kind) +
                          " owner for " +
                          RuntimeCategoryQualifiedName(*record);
      return false;
    }
  }
  return true;
}

bool RuntimeCategoryRecordConflictsAreSupported(
    const RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    std::string &diagnostic_reason) {
  std::unordered_map<std::string, std::string> category_owner_by_key;
  category_owner_by_key.reserve(
      static_cast<std::size_t>(
          registration_table->image_descriptor->category_descriptor_count) +
      state.registered_image_metadata_by_identity_key.size());
  for (const RegisteredImageMetadata *record : OrderedProtocolImages(state)) {
    if (record == nullptr) {
      continue;
    }
    if (!AddRuntimeCategoryRecordConflictEntries(
            record->category_descriptor_root, record->category_descriptor_count,
            category_owner_by_key, diagnostic_reason)) {
      return false;
    }
  }
  return AddRuntimeCategoryRecordConflictEntries(
      registration_table->category_descriptor_root,
      registration_table->image_descriptor->category_descriptor_count,
      category_owner_by_key, diagnostic_reason);
}

bool RuntimeCategoryTargetsAreSupported(
    const RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    std::string &diagnostic_reason) {
  std::unordered_map<std::string, RuntimeCategoryTargetClassRecord>
      target_classes;
  target_classes.reserve(
      static_cast<std::size_t>(
          registration_table->image_descriptor->class_descriptor_count) +
      state.registered_image_metadata_by_identity_key.size());
  if (!AddRuntimeCategoryTargetClassRecords(
          registration_table->class_descriptor_root,
          registration_table->image_descriptor->class_descriptor_count,
          target_classes, diagnostic_reason)) {
    return false;
  }
  for (const RegisteredImageMetadata *record : OrderedProtocolImages(state)) {
    if (record == nullptr) {
      continue;
    }
    if (!AddRuntimeCategoryTargetClassRecords(
            record->class_descriptor_root, record->class_descriptor_count,
            target_classes, diagnostic_reason)) {
      return false;
    }
  }

  for (std::uint64_t index = 0;
       index < registration_table->image_descriptor->category_descriptor_count;
       ++index) {
    const auto *record = static_cast<const EmittedCategoryRecord *>(
        RuntimeAggregateEntry(registration_table->category_descriptor_root,
                              index));
    if (record == nullptr || !RuntimeNonEmptyCString(record->class_name) ||
        !RuntimeNonEmptyCString(record->category_name)) {
      diagnostic_reason =
          "category descriptor root contains malformed category targets";
      return false;
    }
    const std::string category_name = RuntimeCategoryQualifiedName(*record);
    const auto found = target_classes.find(record->class_name);
    if (found == target_classes.end() || !found->second.found) {
      diagnostic_reason =
          "category attachment target class is missing for " + category_name;
      return false;
    }
    if (!RuntimeNonEmptyCString(record->class_owner_identity)) {
      diagnostic_reason =
          "category attachment class owner is missing for " + category_name;
      return false;
    }
    if (found->second.class_owner_identity != record->class_owner_identity) {
      diagnostic_reason =
          "category attachment class owner mismatch for " + category_name;
      return false;
    }
  }
  return true;
}

}  // namespace

const void *ProtocolAggregateEntry(
    const objc3_runtime_pointer_aggregate *aggregate, std::uint64_t index) {
  return RuntimeAggregateEntry(aggregate, index);
}

std::vector<const RegisteredImageMetadata *> OrderedProtocolImages(
    const RuntimeState &state) {
  std::vector<const RegisteredImageMetadata *> ordered;
  ordered.reserve(state.registered_image_metadata_by_identity_key.size());
  for (const auto &entry : state.registered_image_metadata_by_identity_key) {
    ordered.push_back(&entry.second);
  }
  std::sort(
      ordered.begin(), ordered.end(),
      [](const RegisteredImageMetadata *lhs,
         const RegisteredImageMetadata *rhs) {
        return CompareRuntimeImageRegistrationOrder(
                   lhs->registration_order_ordinal,
                   lhs->translation_unit_identity_key.c_str(),
                   rhs->registration_order_ordinal,
                   rhs->translation_unit_identity_key.c_str()) < 0;
      });
  return ordered;
}

bool QueryProtocolConformanceFromProtocolRecordUnlocked(
    const EmittedProtocolRecord *start_record,
    const char *protocol_name,
    std::unordered_set<const EmittedProtocolRecord *> &visited,
    std::uint64_t &visited_protocol_count,
    ProtocolConformanceMatch &match,
    std::string &failure_reason) {
  if (start_record == nullptr || protocol_name == nullptr ||
      protocol_name[0] == '\0') {
    return false;
  }
  std::vector<std::pair<const EmittedProtocolRecord *, std::uint64_t>> stack;
  stack.push_back({start_record, 0});
  while (!stack.empty()) {
    const auto entry = stack.back();
    stack.pop_back();
    const EmittedProtocolRecord *record = entry.first;
    const std::uint64_t protocol_depth = entry.second;
    if (record == nullptr || !visited.insert(record).second) {
      continue;
    }
    if (record->protocol_name == nullptr || record->owner_identity == nullptr) {
      RecordProtocolQueryFailure(failure_reason,
                                 "malformed protocol descriptor");
      return false;
    }
    ++visited_protocol_count;
    if (record->is_forward_declaration) {
      continue;
    }
    if (std::strcmp(record->protocol_name, protocol_name) == 0) {
      match.matched_protocol_owner_identity = record->owner_identity;
      match.matched_protocol_depth = protocol_depth;
      match.matched_via_inherited_protocol = protocol_depth != 0;
      return true;
    }
    const objc3_runtime_pointer_aggregate *inherited_refs =
        record->inherited_protocol_refs;
    if (inherited_refs == nullptr) {
      continue;
    }
    for (std::uint64_t offset = 0; offset < inherited_refs->count; ++offset) {
      const std::uint64_t index = inherited_refs->count - 1 - offset;
      const auto *inherited_record = static_cast<const EmittedProtocolRecord *>(
          ProtocolAggregateEntry(inherited_refs, index));
      if (inherited_record == nullptr) {
        RecordProtocolQueryFailure(failure_reason,
                                   "malformed inherited protocol reference");
        return false;
      }
      stack.push_back({inherited_record, protocol_depth + 1});
    }
  }
  return false;
}

bool QueryProtocolConformanceFromAggregateUnlocked(
    const objc3_runtime_pointer_aggregate *protocol_refs,
    const char *protocol_name,
    std::unordered_set<const EmittedProtocolRecord *> &visited,
    std::uint64_t &visited_protocol_count,
    ProtocolConformanceMatch &match,
    std::string &failure_reason) {
  if (protocol_refs == nullptr) {
    return false;
  }
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
        ProtocolAggregateEntry(protocol_refs, index));
    if (protocol_record == nullptr) {
      RecordProtocolQueryFailure(failure_reason,
                                 "malformed adopted protocol reference");
      return false;
    }
    if (QueryProtocolConformanceFromProtocolRecordUnlocked(
            protocol_record, protocol_name, visited, visited_protocol_count,
            match, failure_reason)) {
      return true;
    }
    if (!failure_reason.empty()) {
      return false;
    }
  }
  return false;
}

bool ProtocolExistsByNameUnlocked(const RuntimeState &state,
                                  const char *protocol_name) {
  if (protocol_name == nullptr || protocol_name[0] == '\0') {
    return false;
  }
  for (const RegisteredImageMetadata *record : OrderedProtocolImages(state)) {
    for (std::uint64_t index = 0; index < record->protocol_descriptor_count;
         ++index) {
      const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
          ProtocolAggregateEntry(record->protocol_descriptor_root, index));
      if (protocol_record == nullptr ||
          protocol_record->protocol_name == nullptr) {
        continue;
      }
      if (!protocol_record->is_forward_declaration &&
          std::strcmp(protocol_record->protocol_name, protocol_name) == 0) {
        return true;
      }
    }
  }
  return false;
}

bool QueryRealizedClassProtocolConformanceUnlocked(
    RuntimeState &state,
    const RealizedClassNode *start_node,
    const char *protocol_name,
    std::uint64_t &visited_protocol_count,
    ProtocolConformanceMatch &match,
    std::string &failure_reason) {
  // category-attachment-protocol-conformance anchor: runtime-facing protocol
  // conformance queries walk realized class nodes, attached category protocol
  // refs, and inherited protocol closures without widening the public dispatch
  // ABI.
  if (start_node == nullptr || protocol_name == nullptr ||
      protocol_name[0] == '\0') {
    return false;
  }
  std::unordered_set<const EmittedProtocolRecord *> visited_protocols;
  std::unordered_set<const RealizedClassNode *> visited_nodes;
  const RealizedClassNode *node = start_node;
  while (node != nullptr && visited_nodes.insert(node).second) {
    if (node->bundle == nullptr) {
      RecordProtocolQueryFailure(failure_reason,
                                 "malformed realized class node");
      return false;
    }
    match.matched_attachment_owner_identity.clear();
    const EmittedClassRecord &record = node->bundle->class_record;
    if (QueryProtocolConformanceFromAggregateUnlocked(
            record.adopted_protocol_refs, protocol_name, visited_protocols,
            visited_protocol_count, match, failure_reason)) {
      match.matched_class_name = node->class_name;
      match.matched_class_owner_identity = node->class_owner_identity;
      match.matched_from_category = false;
      match.matched_from_superclass = node != start_node;
      return true;
    }
    if (!failure_reason.empty()) {
      return false;
    }
    for (const EmittedCategoryRecord *category_record :
         node->attached_category_records) {
      if (category_record == nullptr) {
        continue;
      }
      if (QueryProtocolConformanceFromAggregateUnlocked(
              category_record->adopted_protocol_refs, protocol_name,
              visited_protocols, visited_protocol_count, match,
              failure_reason)) {
        match.matched_attachment_owner_identity =
            category_record->category_owner_identity != nullptr
                ? category_record->category_owner_identity
                : "";
        match.matched_class_name = node->class_name;
        match.matched_class_owner_identity = node->class_owner_identity;
        match.matched_from_category = true;
        match.matched_from_superclass = node != start_node;
        return true;
      }
      if (!failure_reason.empty()) {
        return false;
      }
    }
    node = node->has_super_node
               ? &state.realized_class_nodes[node->super_node_index]
               : nullptr;
  }
  return false;
}

bool RuntimeProtocolConformanceEdgeIsMaterializable(const char *class_name,
                                                    const char *protocol_name) {
  return class_name != nullptr && class_name[0] != '\0' &&
         protocol_name != nullptr && protocol_name[0] != '\0';
}

bool RuntimeProtocolCategoryMetadataTableIsSupported(
    const RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    std::string &diagnostic_reason) {
  diagnostic_reason.clear();
  if (registration_table == nullptr ||
      registration_table->image_descriptor == nullptr ||
      registration_table->protocol_descriptor_root == nullptr ||
      registration_table->class_descriptor_root == nullptr ||
      registration_table->category_descriptor_root == nullptr) {
    diagnostic_reason = "registration table is missing protocol metadata";
    return false;
  }

  std::unordered_set<const EmittedProtocolRecord *> known_protocol_records;
  std::unordered_map<std::string, std::string>
      concrete_protocol_owner_by_name;
  const std::size_t estimated_protocol_count =
      static_cast<std::size_t>(
          registration_table->image_descriptor->protocol_descriptor_count) +
      state.registered_image_metadata_by_identity_key.size();
  known_protocol_records.reserve(estimated_protocol_count);
  concrete_protocol_owner_by_name.reserve(estimated_protocol_count);

  if (!AddKnownProtocolRecords(
          registration_table->protocol_descriptor_root,
          registration_table->image_descriptor->protocol_descriptor_count,
          known_protocol_records, concrete_protocol_owner_by_name,
          diagnostic_reason)) {
    return false;
  }
  for (const RegisteredImageMetadata *record : OrderedProtocolImages(state)) {
    if (record == nullptr) {
      continue;
    }
    if (!AddKnownProtocolRecords(record->protocol_descriptor_root,
                                 record->protocol_descriptor_count,
                                 known_protocol_records,
                                 concrete_protocol_owner_by_name,
                                 diagnostic_reason)) {
      return false;
    }
  }

  return RuntimeRegisteredProtocolMetadataIsSupported(
             registration_table, known_protocol_records, diagnostic_reason) &&
         RuntimeCategoryRecordConflictsAreSupported(
             state, registration_table, diagnostic_reason) &&
         RuntimeCategoryTargetsAreSupported(state, registration_table,
                                            diagnostic_reason) &&
         RuntimeClassProtocolReferencesAreSupported(
             registration_table, known_protocol_records, diagnostic_reason) &&
         RuntimeCategoryProtocolReferencesAreSupported(
             registration_table, known_protocol_records, diagnostic_reason);
}

}  // namespace objc3c::runtime
