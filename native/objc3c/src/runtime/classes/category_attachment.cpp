#include "runtime/classes/category_attachment.h"

#include "runtime/classes/class_metadata_tables.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_cache_invalidation.h"
#include "runtime/state/runtime_state_records.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

namespace {

const void *CategoryAggregateEntry(
    const objc3_runtime_pointer_aggregate *aggregate, std::uint64_t index) {
  return RuntimeAggregateEntry(aggregate, index);
}

struct PreferredCategoryRecord {
  const EmittedCategoryRecord *interface_record = nullptr;
  const EmittedCategoryRecord *implementation_record = nullptr;
  std::uint64_t interface_registration_order_ordinal = 0;
  std::uint64_t implementation_registration_order_ordinal = 0;
  std::uint64_t interface_descriptor_index = 0;
  std::uint64_t implementation_descriptor_index = 0;
};

struct PreferredCategoryAttachment {
  const EmittedCategoryRecord *record = nullptr;
  std::uint64_t registration_order_ordinal = 0;
  std::uint64_t descriptor_index = 0;
};

const char *RuntimeCategoryCString(const char *value) {
  return value != nullptr ? value : "";
}

bool RuntimeCategoryCStringPresent(const char *value) {
  return value != nullptr && value[0] != '\0';
}

void RecordCategoryAttachmentFailure(RuntimeState &state,
                                     const std::string &class_name,
                                     const std::string &reason) {
  ++state.malformed_class_metadata_rejection_count;
  state.last_malformed_class_graph_reason =
      "category-attachment:" + reason + ":class=" + class_name;
}

bool AddPreferredCategoryRecord(
    const EmittedCategoryRecord &category_record,
    std::uint64_t registration_order_ordinal,
    std::uint64_t descriptor_index,
    std::unordered_map<std::string, PreferredCategoryRecord> &grouped_records,
    std::string &failure_reason) {
  if (category_record.class_name == nullptr ||
      category_record.category_name == nullptr ||
      category_record.record_kind == nullptr ||
      category_record.owner_identity == nullptr) {
    failure_reason = "malformed-category-record";
    return false;
  }
  const std::string key =
      std::string(category_record.class_name) + "\n" +
      std::string(category_record.category_name);
  PreferredCategoryRecord &preferred_record = grouped_records[key];
  const std::string record_kind = category_record.record_kind;
  if (record_kind == "implementation") {
    if (preferred_record.implementation_record != nullptr &&
        std::string(preferred_record.implementation_record->owner_identity) !=
            std::string(category_record.owner_identity)) {
      failure_reason =
          "conflicting-category-implementation-owner:" +
          std::string(category_record.class_name) + "(" +
          std::string(category_record.category_name) + ")";
      return false;
    }
    preferred_record.implementation_record = &category_record;
    preferred_record.implementation_registration_order_ordinal =
        registration_order_ordinal;
    preferred_record.implementation_descriptor_index = descriptor_index;
    return true;
  }
  if (record_kind == "interface") {
    if (preferred_record.interface_record != nullptr &&
        std::string(preferred_record.interface_record->owner_identity) !=
            std::string(category_record.owner_identity)) {
      failure_reason =
          "conflicting-category-interface-owner:" +
          std::string(category_record.class_name) + "(" +
          std::string(category_record.category_name) + ")";
      return false;
    }
    preferred_record.interface_record = &category_record;
    preferred_record.interface_registration_order_ordinal =
        registration_order_ordinal;
    preferred_record.interface_descriptor_index = descriptor_index;
    return true;
  }
  failure_reason = "unknown-category-record-kind:" + record_kind;
  return false;
}

bool CollectPreferredCategoryRecords(
    const RuntimeState &state,
    const std::string &class_name,
    std::vector<PreferredCategoryAttachment> &preferred_records,
    std::string &failure_reason) {
  // Class graph publication owns attachment selection: implementation records
  // win over interface records for the same category name, conflicting
  // same-tier owners fail closed across the full registered-image set, and
  // the published vector preserves loader order for later newest-first lookup.
  std::unordered_map<std::string, PreferredCategoryRecord> grouped_records;
  for (const RegisteredImageMetadata *record : OrderedClassGraphImages(state)) {
    if (record == nullptr) {
      continue;
    }
    grouped_records.reserve(grouped_records.size() +
                            static_cast<std::size_t>(
                                record->category_descriptor_count));
    for (std::uint64_t index = 0; index < record->category_descriptor_count;
         ++index) {
      const auto *category_record = static_cast<const EmittedCategoryRecord *>(
          CategoryAggregateEntry(record->category_descriptor_root, index));
      if (category_record == nullptr ||
          category_record->class_name == nullptr ||
          class_name != category_record->class_name) {
        continue;
      }
      if (!AddPreferredCategoryRecord(*category_record,
                                      record->registration_order_ordinal,
                                      index, grouped_records,
                                      failure_reason)) {
        return false;
      }
    }
  }
  preferred_records.clear();
  preferred_records.reserve(grouped_records.size());
  for (const auto &entry : grouped_records) {
    const PreferredCategoryRecord &record = entry.second;
    if (record.interface_record != nullptr &&
        record.implementation_record == nullptr) {
      const EmittedCategoryRecord *interface_record = record.interface_record;
      failure_reason =
          "missing-category-implementation:" +
          std::string(RuntimeCategoryCString(interface_record->class_name)) +
          "(" +
          std::string(RuntimeCategoryCString(interface_record->category_name)) +
          ")";
      return false;
    }
    if (record.implementation_record != nullptr) {
      preferred_records.push_back(PreferredCategoryAttachment{
          record.implementation_record,
          record.implementation_registration_order_ordinal,
          record.implementation_descriptor_index});
    }
  }
  std::sort(
      preferred_records.begin(), preferred_records.end(),
      [](const PreferredCategoryAttachment &lhs,
         const PreferredCategoryAttachment &rhs) {
        const EmittedCategoryRecord *lhs_record = lhs.record;
        const EmittedCategoryRecord *rhs_record = rhs.record;
        return std::make_tuple(
                   lhs.registration_order_ordinal, lhs.descriptor_index,
                   std::string(RuntimeCategoryCString(
                       lhs_record != nullptr ? lhs_record->class_name
                                             : nullptr)),
                   std::string(RuntimeCategoryCString(
                       lhs_record != nullptr ? lhs_record->category_name
                                             : nullptr)),
                   std::string(RuntimeCategoryCString(
                       lhs_record != nullptr ? lhs_record->owner_identity
                                             : nullptr))) <
               std::make_tuple(
                   rhs.registration_order_ordinal, rhs.descriptor_index,
                   std::string(RuntimeCategoryCString(
                       rhs_record != nullptr ? rhs_record->class_name
                                             : nullptr)),
                   std::string(RuntimeCategoryCString(
                       rhs_record != nullptr ? rhs_record->category_name
                                             : nullptr)),
                   std::string(RuntimeCategoryCString(
                       rhs_record != nullptr ? rhs_record->owner_identity
                                             : nullptr)));
      });
  return true;
}

const EmittedMethodListEntry *RuntimeCategoryMethodListEntries(
    const EmittedMethodListHeader *header) {
  return header == nullptr
             ? nullptr
             : reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
}

bool ValidateCategoryMethodList(
    const EmittedMethodListRef *method_list_ref,
    const char *family_name,
    const std::string &class_name,
    const char *category_name,
    std::unordered_set<std::string> &category_method_keys,
    std::string &failure_reason) {
  if (method_list_ref == nullptr || method_list_ref->count == 0) {
    return true;
  }
  if (method_list_ref->method_list == nullptr ||
      !RuntimeCategoryCStringPresent(method_list_ref->owner_identity)) {
    failure_reason =
        std::string("malformed-category-") + family_name + "-method-list:" +
        class_name + "(" + RuntimeCategoryCString(category_name) + ")";
    return false;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    failure_reason =
        std::string("malformed-category-") + family_name +
        "-method-list-count:" + class_name + "(" +
        RuntimeCategoryCString(category_name) + ")";
    return false;
  }
  const EmittedMethodListEntry *entries =
      RuntimeCategoryMethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (!RuntimeCategoryCStringPresent(entry.selector) ||
        !RuntimeCategoryCStringPresent(entry.owner_identity) ||
        !RuntimeCategoryCStringPresent(entry.return_type_name)) {
      failure_reason =
          std::string("malformed-category-") + family_name + "-method-entry:" +
          class_name + "(" + RuntimeCategoryCString(category_name) + ")";
      return false;
    }
    const std::string method_key =
        std::string(family_name) + "\n" + std::string(entry.selector);
    if (!category_method_keys.insert(method_key).second) {
      failure_reason =
          std::string("duplicate-category-") + family_name +
          "-method-selector:" + std::string(entry.selector);
      return false;
    }
  }
  return true;
}

bool ValidateCategoryPropertyDescriptors(
    const RuntimeState &state,
    const std::vector<const EmittedCategoryRecord *> &category_records,
    std::string &failure_reason) {
  std::unordered_set<std::string> category_owner_identities;
  category_owner_identities.reserve(category_records.size());
  for (const EmittedCategoryRecord *category_record : category_records) {
    if (category_record != nullptr &&
        RuntimeCategoryCStringPresent(category_record->owner_identity)) {
      category_owner_identities.insert(category_record->owner_identity);
    }
  }
  if (category_owner_identities.empty()) {
    return true;
  }

  std::unordered_set<std::string> category_property_names;
  for (const RegisteredImageMetadata *record : OrderedClassGraphImages(state)) {
    if (record == nullptr || record->property_descriptor_root == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < record->property_descriptor_count;
         ++index) {
      const auto *descriptor = static_cast<const EmittedPropertyDescriptor *>(
          RuntimeAggregateEntry(record->property_descriptor_root, index));
      if (descriptor == nullptr) {
        failure_reason = "malformed-category-property-descriptor";
        return false;
      }
      if (!RuntimeCategoryCStringPresent(
              descriptor->declaration_owner_identity)) {
        continue;
      }
      if (category_owner_identities.find(
              descriptor->declaration_owner_identity) ==
          category_owner_identities.end()) {
        continue;
      }
      if (!RuntimeCategoryCStringPresent(descriptor->property_name)) {
        failure_reason = "malformed-category-property-descriptor";
        return false;
      }
      if (!category_property_names.insert(descriptor->property_name).second) {
        failure_reason =
            "duplicate-category-property:" +
            std::string(descriptor->property_name);
        return false;
      }
    }
  }
  return true;
}

bool ValidateCategoryMemberSurface(
    const RuntimeState &state,
    const std::string &class_name,
    const std::vector<PreferredCategoryAttachment> &category_attachments,
    std::string &failure_reason) {
  std::vector<const EmittedCategoryRecord *> category_records;
  category_records.reserve(category_attachments.size());
  for (const PreferredCategoryAttachment &attachment : category_attachments) {
    category_records.push_back(attachment.record);
  }
  std::unordered_set<std::string> category_method_keys;
  for (const EmittedCategoryRecord *category_record : category_records) {
    if (category_record == nullptr) {
      failure_reason = "malformed-category-record";
      return false;
    }
    if (!ValidateCategoryMethodList(category_record->instance_method_list_ref,
                                    "instance", class_name,
                                    category_record->category_name,
                                    category_method_keys, failure_reason) ||
        !ValidateCategoryMethodList(category_record->class_method_list_ref,
                                    "class", class_name,
                                    category_record->category_name,
                                    category_method_keys, failure_reason)) {
      return false;
    }
  }
  return ValidateCategoryPropertyDescriptors(state, category_records,
                                             failure_reason);
}

}  // namespace

bool RuntimeCategoryAttachmentIsMaterializable(const char *category_name,
                                               const char *class_name) {
  return category_name != nullptr && category_name[0] != '\0' &&
         class_name != nullptr && class_name[0] != '\0';
}

bool AttachRealizedCategoryRecordsUnlocked(RuntimeState &state,
                                           RealizedClassNode &node) {
  // category-attachment-protocol-conformance anchor: realized class nodes own
  // preferred category attachments and direct protocol edges so dispatch/query
  // paths consume the published graph instead of rediscovering relationships.
  node.attached_category_records.clear();
  node.runtime_attachment_ready = false;
  if (node.image == nullptr) {
    RecordCategoryAttachmentFailure(state, node.class_name,
                                    "missing-realized-class-image");
    return false;
  }
  std::vector<PreferredCategoryAttachment> category_records;
  std::string failure_reason;
  if (!CollectPreferredCategoryRecords(state, node.class_name,
                                       category_records, failure_reason)) {
    RecordCategoryAttachmentFailure(state, node.class_name, failure_reason);
    return false;
  }
  if (!ValidateCategoryMemberSurface(state, node.class_name, category_records,
                                     failure_reason)) {
    RecordCategoryAttachmentFailure(state, node.class_name, failure_reason);
    return false;
  }
  for (const PreferredCategoryAttachment &attachment : category_records) {
    const EmittedCategoryRecord *category_record = attachment.record;
    if (category_record == nullptr ||
        category_record->class_owner_identity == nullptr ||
        category_record->category_owner_identity == nullptr ||
        category_record->owner_identity == nullptr ||
        !RuntimeCategoryAttachmentIsMaterializable(
            category_record->category_name, node.class_name.c_str())) {
      RecordCategoryAttachmentFailure(state, node.class_name,
                                      "malformed-category-attachment");
      return false;
    }
    if (node.class_owner_identity != category_record->class_owner_identity) {
      RecordCategoryAttachmentFailure(
          state, node.class_name,
          "category-class-owner-mismatch:" +
              std::string(RuntimeCategoryCString(category_record->category_name)));
      return false;
    }
    node.attached_category_records.push_back(category_record);
    state.last_attached_category_owner_identity =
        category_record->category_owner_identity;
    state.last_attached_category_name =
        category_record->category_name != nullptr ? category_record->category_name
                                                  : "";
    if (category_record->adopted_protocol_refs != nullptr) {
      state.realized_protocol_conformance_edge_count +=
          category_record->adopted_protocol_refs->count;
    }
  }
  state.realized_attached_category_count +=
      static_cast<std::uint64_t>(node.attached_category_records.size());
  if (!node.attached_category_records.empty()) {
    BumpRuntimeCategoryAttachmentGenerationUnlocked(state);
    BumpRuntimeMethodSurfaceGenerationUnlocked(state);
  }
  if (node.bundle != nullptr &&
      node.bundle->class_record.adopted_protocol_refs != nullptr) {
    state.realized_protocol_conformance_edge_count +=
        node.bundle->class_record.adopted_protocol_refs->count;
  }
  node.runtime_attachment_ready = true;
  return true;
}

}  // namespace objc3c::runtime
