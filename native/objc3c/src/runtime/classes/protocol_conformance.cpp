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

bool RuntimeDiagnosticStartsWith(const std::string &value,
                                 const char *prefix) {
  return prefix != nullptr && value.rfind(prefix, 0) == 0;
}

bool RuntimeDiagnosticContains(const std::string &value,
                               const char *fragment) {
  return fragment != nullptr && value.find(fragment) != std::string::npos;
}

const char *RuntimeProtocolCString(const char *value) {
  return value != nullptr ? value : "";
}

std::string RuntimeProtocolReferenceConflictQualifier(
    const std::string &context) {
  if (RuntimeDiagnosticStartsWith(context, "protocol ")) {
    return "inherited ";
  }
  if (RuntimeDiagnosticStartsWith(context, "class ") ||
      RuntimeDiagnosticStartsWith(context, "metaclass ") ||
      RuntimeDiagnosticStartsWith(context, "category ")) {
    return "adopted ";
  }
  return "";
}

struct RuntimeProtocolRequirementSignature {
  std::string return_type_name;
  std::uint64_t parameter_count = 0;
};

using RuntimeProtocolRequirementMap =
    std::unordered_map<std::string, RuntimeProtocolRequirementSignature>;

std::string RuntimeProtocolRequirementKey(const char *family_name,
                                          const char *selector) {
  return std::string(RuntimeProtocolCString(family_name)) + "\n" +
         RuntimeProtocolCString(selector);
}

bool RuntimeProtocolRequirementSignaturesMatch(
    const RuntimeProtocolRequirementSignature &lhs,
    const RuntimeProtocolRequirementSignature &rhs) {
  return lhs.return_type_name == rhs.return_type_name &&
         lhs.parameter_count == rhs.parameter_count;
}

bool AddRuntimeProtocolRequirementSignature(
    const char *family_name,
    const char *selector,
    const char *return_type_name,
    std::uint64_t parameter_count,
    const std::string &context,
    const std::string &conflict_qualifier,
    RuntimeProtocolRequirementMap &requirements_by_key,
    bool duplicate_same_signature_is_error,
    std::string &diagnostic_reason) {
  const RuntimeProtocolRequirementSignature signature{
      RuntimeProtocolCString(return_type_name), parameter_count};
  const std::string requirement_key =
      RuntimeProtocolRequirementKey(family_name, selector);
  const auto inserted =
      requirements_by_key.emplace(requirement_key, signature);
  if (inserted.second) {
    return true;
  }
  if (RuntimeProtocolRequirementSignaturesMatch(inserted.first->second,
                                                signature)) {
    if (duplicate_same_signature_is_error) {
      diagnostic_reason =
          "duplicate protocol " + std::string(family_name) +
          " method requirement " + RuntimeProtocolCString(selector) +
          " in " + context;
      return false;
    }
    return true;
  }
  diagnostic_reason =
      "conflicting " + conflict_qualifier + "protocol " +
      std::string(family_name) +
      " method requirement " + RuntimeProtocolCString(selector) + " in " +
      context;
  return false;
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
  RuntimeProtocolRequirementMap requirements_by_key;
  requirements_by_key.reserve(static_cast<std::size_t>(header->count));
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
    if (!AddRuntimeProtocolRequirementSignature(
            family_name, entry.selector, entry.return_type_name,
            entry.parameter_count,
            "protocol " + std::string(RuntimeProtocolCString(protocol_name)),
            "", requirements_by_key, true, diagnostic_reason)) {
      return false;
    }
  }
  return true;
}

bool AddRuntimeProtocolMethodListRequirements(
    const EmittedMethodListRef *method_list_ref,
    const char *family_name,
    const std::string &context,
    RuntimeProtocolRequirementMap &requirements_by_key,
    std::string &diagnostic_reason) {
  if (method_list_ref == nullptr || method_list_ref->count == 0) {
    return true;
  }
  if (method_list_ref->method_list == nullptr ||
      !RuntimeNonEmptyCString(method_list_ref->owner_identity)) {
    diagnostic_reason =
        std::string("malformed protocol ") + family_name +
        " method list for " + context;
    return false;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    diagnostic_reason =
        std::string("malformed protocol ") + family_name +
        " method list count for " + context;
    return false;
  }
  const auto *entries =
      reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (!RuntimeNonEmptyCString(entry.selector) ||
        !RuntimeNonEmptyCString(entry.return_type_name)) {
      diagnostic_reason =
          std::string("malformed protocol ") + family_name +
          " method entry for " + context;
      return false;
    }
    if (!AddRuntimeProtocolRequirementSignature(
            family_name, entry.selector, entry.return_type_name,
            entry.parameter_count, context,
            RuntimeProtocolReferenceConflictQualifier(context),
            requirements_by_key, false, diagnostic_reason)) {
      return false;
    }
  }
  return true;
}

bool RuntimeForwardProtocolRecordIsEmpty(
    const EmittedProtocolRecord &record) {
  const bool has_inherited_protocols =
      record.inherited_protocol_refs != nullptr &&
      record.inherited_protocol_refs->count != 0;
  const bool has_instance_methods =
      record.instance_method_list_ref != nullptr &&
      record.instance_method_list_ref->count != 0;
  const bool has_class_methods = record.class_method_list_ref != nullptr &&
                                 record.class_method_list_ref->count != 0;
  return !has_inherited_protocols && !has_instance_methods &&
         !has_class_methods && record.property_count == 0 &&
         record.method_count == 0 && record.instance_method_count == 0 &&
         record.class_method_count == 0;
}

bool AddKnownProtocolRecords(
    const objc3_runtime_pointer_aggregate *protocol_descriptor_root,
    std::uint64_t protocol_descriptor_count,
    std::unordered_set<const EmittedProtocolRecord *> &known_protocol_records,
    std::unordered_map<std::string, std::string>
        &concrete_protocol_owner_by_name,
    std::string &diagnostic_reason) {
  std::unordered_set<std::string> concrete_protocol_names_in_root;
  concrete_protocol_names_in_root.reserve(
      static_cast<std::size_t>(protocol_descriptor_count));
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
    if (record->is_forward_declaration &&
        !RuntimeForwardProtocolRecordIsEmpty(*record)) {
      diagnostic_reason =
          "forward protocol descriptor contains concrete requirements for " +
          std::string(record->protocol_name);
      return false;
    }
    known_protocol_records.insert(record);
    if (record->is_forward_declaration) {
      continue;
    }
    if (!concrete_protocol_names_in_root.insert(record->protocol_name).second) {
      diagnostic_reason =
          "duplicate protocol descriptor for " +
          std::string(record->protocol_name);
      return false;
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

bool CollectRuntimeProtocolRequirementClosure(
    const EmittedProtocolRecord *record,
    const std::string &context,
    RuntimeProtocolRequirementMap &requirements_by_key,
    std::unordered_set<const EmittedProtocolRecord *> &visiting,
    std::unordered_set<const EmittedProtocolRecord *> &visited,
    std::string &diagnostic_reason) {
  if (record == nullptr) {
    diagnostic_reason = "unknown protocol reference in " + context;
    return false;
  }
  if (!RuntimeNonEmptyCString(record->protocol_name) ||
      !RuntimeNonEmptyCString(record->owner_identity)) {
    diagnostic_reason = "malformed protocol descriptor in " + context;
    return false;
  }
  if (record->is_forward_declaration) {
    diagnostic_reason = "forward protocol reference in " + context;
    return false;
  }
  if (visited.find(record) != visited.end()) {
    return true;
  }
  if (!visiting.insert(record).second) {
    diagnostic_reason = "cyclic protocol inheritance in " + context;
    return false;
  }

  const objc3_runtime_pointer_aggregate *inherited_refs =
      record->inherited_protocol_refs;
  if (inherited_refs != nullptr) {
    for (std::uint64_t index = 0; index < inherited_refs->count; ++index) {
      const auto *inherited_record =
          static_cast<const EmittedProtocolRecord *>(
              RuntimeAggregateEntry(inherited_refs, index));
      if (!CollectRuntimeProtocolRequirementClosure(
              inherited_record, context, requirements_by_key, visiting,
              visited, diagnostic_reason)) {
        return false;
      }
    }
  }
  if (!AddRuntimeProtocolMethodListRequirements(
          record->instance_method_list_ref, "instance", context,
          requirements_by_key, diagnostic_reason) ||
      !AddRuntimeProtocolMethodListRequirements(
          record->class_method_list_ref, "class", context,
          requirements_by_key, diagnostic_reason)) {
    return false;
  }
  visiting.erase(record);
  visited.insert(record);
  return true;
}

bool RuntimeProtocolReferenceRequirementClosureIsSupported(
    const objc3_runtime_pointer_aggregate *protocol_refs,
    const std::string &context,
    std::string &diagnostic_reason) {
  if (protocol_refs == nullptr) {
    return true;
  }
  RuntimeProtocolRequirementMap requirements_by_key;
  requirements_by_key.reserve(static_cast<std::size_t>(protocol_refs->count));
  std::unordered_set<const EmittedProtocolRecord *> visiting;
  std::unordered_set<const EmittedProtocolRecord *> visited;
  std::unordered_set<std::string> direct_protocol_owners;
  direct_protocol_owners.reserve(static_cast<std::size_t>(protocol_refs->count));
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *record = static_cast<const EmittedProtocolRecord *>(
        RuntimeAggregateEntry(protocol_refs, index));
    if (record == nullptr ||
        !RuntimeNonEmptyCString(record->owner_identity)) {
      diagnostic_reason = "unknown protocol reference in " + context;
      return false;
    }
    if (!direct_protocol_owners.insert(record->owner_identity).second) {
      diagnostic_reason = "duplicate protocol reference in " + context;
      return false;
    }
    if (!CollectRuntimeProtocolRequirementClosure(
            record, context, requirements_by_key, visiting, visited,
            diagnostic_reason)) {
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
    if (!record->is_forward_declaration) {
      RuntimeProtocolRequirementMap requirements_by_key;
      std::unordered_set<const EmittedProtocolRecord *> visiting;
      std::unordered_set<const EmittedProtocolRecord *> visited;
      if (!CollectRuntimeProtocolRequirementClosure(
              record, "protocol " + std::string(record->protocol_name),
              requirements_by_key, visiting, visited, diagnostic_reason)) {
        return false;
      }
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
        !RuntimeProtocolReferenceRequirementClosureIsSupported(
            bundle->class_record.adopted_protocol_refs,
            "class " + std::string(bundle->class_record.class_name),
            diagnostic_reason) ||
        !RuntimeProtocolReferenceAggregateIsSupported(
            bundle->metaclass_record.adopted_protocol_refs,
            known_protocol_records,
            "metaclass " + std::string(bundle->class_record.class_name),
            diagnostic_reason) ||
        !RuntimeProtocolReferenceRequirementClosureIsSupported(
            bundle->metaclass_record.adopted_protocol_refs,
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
            diagnostic_reason) ||
        !RuntimeProtocolReferenceRequirementClosureIsSupported(
            record->adopted_protocol_refs,
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
      RecordProtocolQueryFailure(
          failure_reason, "forward protocol reference in conformance query");
      return false;
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

bool BuildRuntimeProtocolExistentialWitnessMetadata(
    const char *class_name,
    const char *protocol_name,
    const ProtocolConformanceMatch &match,
    ProtocolExistentialWitnessMetadata &metadata,
    std::string &failure_reason) {
  metadata = {};
  failure_reason.clear();
  if (!RuntimeProtocolConformanceEdgeIsMaterializable(class_name,
                                                      protocol_name)) {
    failure_reason =
        "protocol existential witness metadata requires class and protocol names";
    return false;
  }
  if (match.matched_protocol_owner_identity.empty()) {
    failure_reason =
        "protocol existential witness metadata requires protocol owner identity";
    return false;
  }
  const std::string conformance_owner_identity =
      !match.matched_attachment_owner_identity.empty()
          ? match.matched_attachment_owner_identity
          : match.matched_class_owner_identity;
  if (conformance_owner_identity.empty()) {
    failure_reason =
        "protocol existential witness metadata requires conformance owner identity";
    return false;
  }

  metadata.existential_canonical_spelling =
      "id<" + std::string(protocol_name) + ">";
  metadata.object_representation = "id";
  metadata.conforming_type_canonical_spelling = class_name;
  metadata.conforming_type_owner_identity = match.matched_class_owner_identity;
  metadata.protocol_name = protocol_name;
  metadata.protocol_owner_identity = match.matched_protocol_owner_identity;
  metadata.conformance_owner_identity = conformance_owner_identity;
  metadata.attachment_owner_identity = match.matched_attachment_owner_identity;
  metadata.runtime_lookup_anchor =
      kObjc3ProtocolExistentialRuntimeLookupAnchor;
  metadata.witness_metadata_key =
      kObjc3ProtocolExistentialWitnessMetadataKey;
  metadata.requirement_resolution_policy =
      "semantic-requirements-before-runtime-conformance-edge";
  metadata.unsupported_associated_type_diagnostic =
      kObjc3ProtocolExistentialAssociatedTypeDiagnosticCode;
  metadata.unsupported_dynamic_dispatch_diagnostic =
      kObjc3ProtocolExistentialDynamicDispatchDiagnosticCode;
  metadata.matched_protocol_depth = match.matched_protocol_depth;
  metadata.matched_from_category = match.matched_from_category;
  metadata.matched_from_superclass = match.matched_from_superclass;
  metadata.matched_via_inherited_protocol =
      match.matched_via_inherited_protocol;
  metadata.conformance_edge_materializable = true;
  metadata.associated_types_supported = false;
  metadata.dynamic_existential_dispatch_supported = false;
  metadata.fail_closed_for_unsupported_semantics = true;
  return true;
}

bool RuntimeProtocolExistentialWitnessMetadataIsSupported(
    const ProtocolExistentialWitnessMetadata &metadata) {
  return !metadata.existential_canonical_spelling.empty() &&
         metadata.object_representation == "id" &&
         !metadata.conforming_type_canonical_spelling.empty() &&
         !metadata.protocol_name.empty() &&
         !metadata.protocol_owner_identity.empty() &&
         !metadata.conformance_owner_identity.empty() &&
         metadata.runtime_lookup_anchor ==
             kObjc3ProtocolExistentialRuntimeLookupAnchor &&
         metadata.witness_metadata_key ==
             kObjc3ProtocolExistentialWitnessMetadataKey &&
         !metadata.requirement_resolution_policy.empty() &&
         metadata.unsupported_associated_type_diagnostic ==
             kObjc3ProtocolExistentialAssociatedTypeDiagnosticCode &&
         metadata.unsupported_dynamic_dispatch_diagnostic ==
             kObjc3ProtocolExistentialDynamicDispatchDiagnosticCode &&
         metadata.conformance_edge_materializable &&
         !metadata.associated_types_supported &&
         !metadata.dynamic_existential_dispatch_supported &&
         metadata.fail_closed_for_unsupported_semantics;
}

void ClearRuntimeProtocolCategoryDiagnosticFieldsUnlocked(RuntimeState &state) {
  state.last_malformed_class_graph_metadata_surface.clear();
  state.last_malformed_class_graph_target_kind.clear();
  state.last_malformed_class_graph_visibility_state.clear();
  state.last_malformed_class_graph_availability_state.clear();
}

void RecordRuntimeProtocolCategoryDiagnosticFieldsUnlocked(
    RuntimeState &state,
    const std::string &diagnostic_reason) {
  ClearRuntimeProtocolCategoryDiagnosticFieldsUnlocked(state);
  if (RuntimeDiagnosticStartsWith(diagnostic_reason,
                                  "unknown protocol reference in ")) {
    if (RuntimeDiagnosticContains(diagnostic_reason, " in protocol ")) {
      state.last_malformed_class_graph_metadata_surface =
          "protocol.inherited_protocol_refs";
      state.last_malformed_class_graph_availability_state =
          "unavailable-for-inheritance";
    } else if (RuntimeDiagnosticContains(diagnostic_reason, " in metaclass ")) {
      state.last_malformed_class_graph_metadata_surface =
          "metaclass.adopted_protocol_refs";
      state.last_malformed_class_graph_availability_state = "unavailable";
    } else if (RuntimeDiagnosticContains(diagnostic_reason, " in category ")) {
      state.last_malformed_class_graph_metadata_surface =
          "category.adopted_protocol_refs";
      state.last_malformed_class_graph_availability_state = "unavailable";
    } else {
      state.last_malformed_class_graph_metadata_surface =
          "class.adopted_protocol_refs";
      state.last_malformed_class_graph_availability_state = "unavailable";
    }
    state.last_malformed_class_graph_target_kind = "protocol";
    state.last_malformed_class_graph_visibility_state = "unregistered";
    return;
  }
  if (RuntimeDiagnosticStartsWith(diagnostic_reason,
                                  "forward protocol reference in ")) {
    if (RuntimeDiagnosticContains(diagnostic_reason, " in protocol ")) {
      state.last_malformed_class_graph_metadata_surface =
          "protocol.inherited_protocol_refs";
      state.last_malformed_class_graph_availability_state =
          "unavailable-for-inheritance";
    } else if (RuntimeDiagnosticContains(diagnostic_reason, " in metaclass ")) {
      state.last_malformed_class_graph_metadata_surface =
          "metaclass.adopted_protocol_refs";
      state.last_malformed_class_graph_availability_state =
          "unavailable-for-conformance";
    } else if (RuntimeDiagnosticContains(diagnostic_reason, " in category ")) {
      state.last_malformed_class_graph_metadata_surface =
          "category.adopted_protocol_refs";
      state.last_malformed_class_graph_availability_state =
          "unavailable-for-conformance";
    } else {
      state.last_malformed_class_graph_metadata_surface =
          "class.adopted_protocol_refs";
      state.last_malformed_class_graph_availability_state =
          "unavailable-for-conformance";
    }
    state.last_malformed_class_graph_target_kind = "protocol";
    state.last_malformed_class_graph_visibility_state = "forward-declaration";
    return;
  }
  if (RuntimeDiagnosticStartsWith(
          diagnostic_reason,
          "category attachment target class is missing for ")) {
    state.last_malformed_class_graph_metadata_surface = "category.target_class";
    state.last_malformed_class_graph_target_kind = "class";
    state.last_malformed_class_graph_visibility_state = "absent";
    state.last_malformed_class_graph_availability_state = "missing-target";
    return;
  }
  if (RuntimeDiagnosticStartsWith(diagnostic_reason,
                                  "conflicting category ")) {
    state.last_malformed_class_graph_metadata_surface =
        "category.owner_identity";
    state.last_malformed_class_graph_target_kind = "category";
    state.last_malformed_class_graph_visibility_state = "duplicate-category";
    state.last_malformed_class_graph_availability_state = "conflicting-owner";
    return;
  }
  if (RuntimeDiagnosticStartsWith(diagnostic_reason,
                                  "duplicate protocol descriptor for ")) {
    state.last_malformed_class_graph_metadata_surface =
        "protocol.descriptor_root";
    state.last_malformed_class_graph_target_kind = "protocol";
    state.last_malformed_class_graph_visibility_state = "duplicate-protocol";
    state.last_malformed_class_graph_availability_state =
        "duplicate-descriptor";
    return;
  }
  if (RuntimeDiagnosticStartsWith(
          diagnostic_reason,
          "duplicate protocol instance method requirement ")) {
    state.last_malformed_class_graph_metadata_surface =
        "protocol.instance_method_list";
    state.last_malformed_class_graph_target_kind = "protocol";
    state.last_malformed_class_graph_visibility_state = "available";
    state.last_malformed_class_graph_availability_state =
        "duplicate-requirement";
    return;
  }
  if (RuntimeDiagnosticStartsWith(diagnostic_reason,
                                  "conflicting adopted protocol ")) {
    if (RuntimeDiagnosticContains(diagnostic_reason, " in metaclass ")) {
      state.last_malformed_class_graph_target_kind = "metaclass";
      state.last_malformed_class_graph_metadata_surface =
          "metaclass.adopted_protocol_refs";
    } else if (RuntimeDiagnosticContains(diagnostic_reason, " in category ")) {
      state.last_malformed_class_graph_target_kind = "category";
      state.last_malformed_class_graph_metadata_surface =
          "category.adopted_protocol_refs";
    } else {
      state.last_malformed_class_graph_target_kind = "class";
      state.last_malformed_class_graph_metadata_surface =
          "class.adopted_protocol_refs";
    }
    state.last_malformed_class_graph_visibility_state = "available";
    state.last_malformed_class_graph_availability_state =
        "adopted-requirement-conflict";
    return;
  }
  if (RuntimeDiagnosticStartsWith(diagnostic_reason,
                                  "conflicting inherited protocol ")) {
    state.last_malformed_class_graph_target_kind = "protocol";
    state.last_malformed_class_graph_metadata_surface =
        "protocol.inherited_protocol_refs";
    state.last_malformed_class_graph_visibility_state = "available";
    state.last_malformed_class_graph_availability_state =
        "inherited-requirement-conflict";
    return;
  }
  if (RuntimeDiagnosticStartsWith(
          diagnostic_reason,
          "conflicting protocol instance method requirement ")) {
    state.last_malformed_class_graph_target_kind = "protocol";
    state.last_malformed_class_graph_metadata_surface =
        "protocol.instance_method_list";
    state.last_malformed_class_graph_visibility_state = "available";
    state.last_malformed_class_graph_availability_state =
        "duplicate-requirement";
  }
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
