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
};

bool AddPreferredCategoryRecord(
    const EmittedCategoryRecord &category_record,
    std::unordered_map<std::string, PreferredCategoryRecord> &grouped_records) {
  if (category_record.class_name == nullptr ||
      category_record.category_name == nullptr ||
      category_record.record_kind == nullptr ||
      category_record.owner_identity == nullptr) {
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
      return false;
    }
    preferred_record.implementation_record = &category_record;
    return true;
  }
  if (record_kind == "interface") {
    if (preferred_record.interface_record != nullptr &&
        std::string(preferred_record.interface_record->owner_identity) !=
            std::string(category_record.owner_identity)) {
      return false;
    }
    preferred_record.interface_record = &category_record;
    return true;
  }
  return false;
}

bool CollectPreferredCategoryRecords(
    const RuntimeState &state,
    const std::string &class_name,
    std::vector<const EmittedCategoryRecord *> &preferred_records) {
  // Class graph publication owns attachment selection: implementation records
  // win over interface records for the same category name, and conflicting
  // same-tier owners fail closed across the full registered-image set.
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
      if (!AddPreferredCategoryRecord(*category_record, grouped_records)) {
        return false;
      }
    }
  }
  preferred_records.clear();
  preferred_records.reserve(grouped_records.size());
  for (const auto &entry : grouped_records) {
    const PreferredCategoryRecord &record = entry.second;
    preferred_records.push_back(record.implementation_record != nullptr
                                    ? record.implementation_record
                                    : record.interface_record);
  }
  std::sort(
      preferred_records.begin(), preferred_records.end(),
      [](const EmittedCategoryRecord *lhs,
         const EmittedCategoryRecord *rhs) {
        return std::make_tuple(std::string(lhs->class_name),
                               std::string(lhs->category_name),
                               std::string(lhs->record_kind),
                               std::string(lhs->owner_identity)) <
               std::make_tuple(std::string(rhs->class_name),
                               std::string(rhs->category_name),
                               std::string(rhs->record_kind),
                               std::string(rhs->owner_identity));
      });
  return true;
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
    return false;
  }
  std::vector<const EmittedCategoryRecord *> category_records;
  if (!CollectPreferredCategoryRecords(state, node.class_name,
                                       category_records)) {
    return false;
  }
  for (const EmittedCategoryRecord *category_record : category_records) {
    if (category_record == nullptr ||
        category_record->class_owner_identity == nullptr ||
        category_record->category_owner_identity == nullptr ||
        category_record->owner_identity == nullptr ||
        !RuntimeCategoryAttachmentIsMaterializable(
            category_record->category_name, node.class_name.c_str())) {
      return false;
    }
    if (node.class_owner_identity != category_record->class_owner_identity) {
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
