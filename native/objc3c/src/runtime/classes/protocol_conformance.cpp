#include "runtime/classes/protocol_conformance.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/images/multi_image_ordering.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

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
    std::string &matched_owner_identity) {
  if (start_record == nullptr || protocol_name == nullptr ||
      protocol_name[0] == '\0') {
    return false;
  }
  std::vector<const EmittedProtocolRecord *> stack;
  stack.push_back(start_record);
  while (!stack.empty()) {
    const EmittedProtocolRecord *record = stack.back();
    stack.pop_back();
    if (record == nullptr || !visited.insert(record).second) {
      continue;
    }
    if (record->protocol_name == nullptr || record->owner_identity == nullptr) {
      return false;
    }
    ++visited_protocol_count;
    if (std::strcmp(record->protocol_name, protocol_name) == 0) {
      matched_owner_identity = record->owner_identity;
      return true;
    }
    const objc3_runtime_pointer_aggregate *inherited_refs =
        record->inherited_protocol_refs;
    if (inherited_refs == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < inherited_refs->count; ++index) {
      const auto *inherited_record = static_cast<const EmittedProtocolRecord *>(
          ProtocolAggregateEntry(inherited_refs, index));
      if (inherited_record == nullptr) {
        return false;
      }
      stack.push_back(inherited_record);
    }
  }
  return false;
}

bool QueryProtocolConformanceFromAggregateUnlocked(
    const objc3_runtime_pointer_aggregate *protocol_refs,
    const char *protocol_name,
    std::unordered_set<const EmittedProtocolRecord *> &visited,
    std::uint64_t &visited_protocol_count,
    std::string &matched_owner_identity) {
  if (protocol_refs == nullptr) {
    return false;
  }
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
        ProtocolAggregateEntry(protocol_refs, index));
    if (protocol_record == nullptr) {
      return false;
    }
    if (QueryProtocolConformanceFromProtocolRecordUnlocked(
            protocol_record, protocol_name, visited, visited_protocol_count,
            matched_owner_identity)) {
      return true;
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
      if (std::strcmp(protocol_record->protocol_name, protocol_name) == 0) {
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
    std::string &matched_protocol_owner_identity,
    std::string &matched_attachment_owner_identity) {
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
      return false;
    }
    matched_attachment_owner_identity.clear();
    const EmittedClassRecord &record = node->bundle->class_record;
    if (QueryProtocolConformanceFromAggregateUnlocked(
            record.adopted_protocol_refs, protocol_name, visited_protocols,
            visited_protocol_count, matched_protocol_owner_identity)) {
      return true;
    }
    for (const EmittedCategoryRecord *category_record :
         node->attached_category_records) {
      if (category_record == nullptr) {
        continue;
      }
      if (QueryProtocolConformanceFromAggregateUnlocked(
              category_record->adopted_protocol_refs, protocol_name,
              visited_protocols, visited_protocol_count,
              matched_protocol_owner_identity)) {
        matched_attachment_owner_identity =
            category_record->category_owner_identity != nullptr
                ? category_record->category_owner_identity
                : "";
        return true;
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

}  // namespace objc3c::runtime
