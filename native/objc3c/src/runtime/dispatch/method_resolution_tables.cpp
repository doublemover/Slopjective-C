#include "runtime/dispatch/method_resolution_tables.h"

#include "runtime/classes/protocol_conformance.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/runtime_method_shape.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/selectors/selector_table.h"

#include <cstdint>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

namespace {

const void *MethodResolutionAggregateEntry(
    const objc3_runtime_pointer_aggregate *aggregate, std::uint64_t index) {
  return RuntimeAggregateEntry(aggregate, index);
}

const EmittedMethodListEntry *MethodListEntries(
    const EmittedMethodListHeader *header) {
  return header == nullptr
             ? nullptr
             : reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
}

const EmittedMethodListRef *SelectMethodListRef(
    const EmittedProtocolRecord &record, DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

const EmittedMethodListRef *SelectMethodListRef(
    const EmittedCategoryRecord &record, DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

bool SelectorMatchesMethodEntryUnlocked(RuntimeState &,
                                        const char *entry_selector,
                                        std::uint64_t selector_stable_id,
                                        const char *selector_spelling) {
  if (entry_selector == nullptr) {
    return false;
  }
  if (selector_stable_id == 0 && selector_spelling != nullptr &&
      selector_spelling[0] != '\0') {
    const objc3_runtime_selector_handle *lookup_handle =
        LookupSelectorUnlocked(selector_spelling);
    selector_stable_id = lookup_handle != nullptr ? lookup_handle->stable_id : 0;
  }
  if (selector_stable_id == 0) {
    return false;
  }
  const objc3_runtime_selector_handle *entry_handle =
      LookupSelectorUnlocked(entry_selector);
  return entry_handle != nullptr &&
         entry_handle->stable_id == selector_stable_id;
}

bool ProbeProtocolSelectorDeclarationUnlocked(
    RuntimeState &state, const EmittedProtocolRecord *start_record,
    DispatchFamily family, std::uint64_t selector_stable_id,
    const char *selector_spelling, std::uint64_t &protocol_probe_count,
    bool &declared, bool &ambiguous) {
  // protocol/category-aware resolution anchor: adopted/inherited protocol
  // method lists provide declaration-aware negative resolution while
  // remaining non-callable.
  (void)ambiguous;
  if (start_record == nullptr) {
    return true;
  }
  std::vector<const EmittedProtocolRecord *> stack;
  stack.push_back(start_record);
  std::unordered_set<const EmittedProtocolRecord *> visited;
  while (!stack.empty()) {
    const EmittedProtocolRecord *record = stack.back();
    stack.pop_back();
    if (record == nullptr || !visited.insert(record).second) {
      continue;
    }
    if (record->protocol_name == nullptr || record->owner_identity == nullptr ||
        !RuntimeProtocolConformanceEdgeIsMaterializable("protocol-adopter",
                                                        record->protocol_name)) {
      return false;
    }
    ++protocol_probe_count;
    const EmittedMethodListRef *method_list_ref =
        SelectMethodListRef(*record, family);
    if (method_list_ref != nullptr && method_list_ref->count != 0 &&
        method_list_ref->method_list != nullptr) {
      const auto *header =
          static_cast<const EmittedMethodListHeader *>(
              method_list_ref->method_list);
      if (header == nullptr || header->count != method_list_ref->count) {
        return false;
      }
      const EmittedMethodListEntry *entries = MethodListEntries(header);
      for (std::uint64_t index = 0; index < header->count; ++index) {
        const EmittedMethodListEntry &entry = entries[index];
        if (entry.selector == nullptr || entry.owner_identity == nullptr ||
            entry.return_type_name == nullptr) {
          return false;
        }
        if (!SelectorMatchesMethodEntryUnlocked(state, entry.selector,
                                                selector_stable_id,
                                                selector_spelling)) {
          continue;
        }
        declared = true;
      }
    }
    const objc3_runtime_pointer_aggregate *inherited_refs =
        record->inherited_protocol_refs;
    if (inherited_refs == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < inherited_refs->count; ++index) {
      const auto *inherited_record = static_cast<const EmittedProtocolRecord *>(
          MethodResolutionAggregateEntry(inherited_refs, index));
      if (inherited_record == nullptr) {
        return false;
      }
      stack.push_back(inherited_record);
    }
  }
  return true;
}

}  // namespace

bool TryResolveMethodFromMethodListRefUnlocked(
    RuntimeState &state, const EmittedMethodListRef *method_list_ref,
    const char *resolved_class_name, DispatchFamily family,
    std::uint64_t normalized_receiver_identity, std::uint64_t selector_stable_id,
    const char *selector_spelling, SlowPathResolution &resolution,
    bool &ambiguous) {
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr) {
    return true;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    resolution.strict_error_status =
        OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
    return false;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr) {
      resolution.strict_error_status =
          OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
      return false;
    }
    if (!SelectorMatchesMethodEntryUnlocked(state, entry.selector,
                                            selector_stable_id,
                                            selector_spelling)) {
      continue;
    }
    if (entry.has_body == 0 || entry.implementation == nullptr) {
      continue;
    }
    const objc3_runtime_dispatch_status_code shape_status =
        RuntimeMethodShapeStatus(entry.return_type_name, entry.parameter_count);
    if (shape_status != OBJC3_RUNTIME_DISPATCH_STATUS_OK) {
      resolution.strict_error_status = shape_status;
      return true;
    }
    const RuntimeMethodReturnKind entry_return_kind =
        ClassifyRuntimeReturnType(entry.return_type_name);
    if (resolution.resolved &&
        (resolution.implementation != entry.implementation ||
         resolution.parameter_count != entry.parameter_count ||
         resolution.return_kind != entry_return_kind ||
         resolution.owner_identity != entry.owner_identity ||
         resolution.class_name !=
             (resolved_class_name != nullptr ? resolved_class_name : ""))) {
      ambiguous = true;
      return true;
    }
    resolution.resolved = true;
    resolution.dispatch_family_is_class = family == DispatchFamily::Class;
    resolution.selector_storage =
        selector_spelling != nullptr ? selector_spelling : "";
    resolution.class_name =
        resolved_class_name != nullptr ? resolved_class_name : "";
    resolution.owner_identity = entry.owner_identity;
    resolution.normalized_receiver_identity = normalized_receiver_identity;
    resolution.selector_stable_id = selector_stable_id;
    resolution.parameter_count = entry.parameter_count;
    resolution.return_kind = entry_return_kind;
    resolution.implementation = entry.implementation;
    resolution.effective_direct_dispatch = entry.effective_direct_dispatch;
    resolution.objc_final_declared = entry.objc_final_declared;
  }
  return true;
}

bool ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
    RuntimeState &state,
    const objc3_runtime_pointer_aggregate *protocol_refs, DispatchFamily family,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    std::uint64_t &protocol_probe_count, bool &ambiguous) {
  if (protocol_refs == nullptr) {
    return true;
  }
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
        MethodResolutionAggregateEntry(protocol_refs, index));
    if (protocol_record == nullptr) {
      return false;
    }
    bool declared = false;
    if (!ProbeProtocolSelectorDeclarationUnlocked(
            state, protocol_record, family, selector_stable_id,
            selector_spelling, protocol_probe_count, declared, ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
  }
  return true;
}

bool TryResolveMethodFromAttachedCategoriesUnlocked(
    RuntimeState &state, const RealizedClassNode &node, DispatchFamily family,
    std::uint64_t normalized_receiver_identity, std::uint64_t selector_stable_id,
    const char *selector_spelling, SlowPathResolution &resolution,
    bool &ambiguous, std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count) {
  // protocol/category-aware resolution anchor: once class bodies miss,
  // preferred category implementation records become the next live method tier.
  if (node.class_name.empty() || !node.runtime_attachment_ready) {
    return false;
  }
  for (const EmittedCategoryRecord *category_record :
       node.attached_category_records) {
    ++category_probe_count;
    if (!TryResolveMethodFromMethodListRefUnlocked(
            state, SelectMethodListRef(*category_record, family),
            node.class_name.c_str(), family, normalized_receiver_identity,
            selector_stable_id, selector_spelling, resolution, ambiguous)) {
      return false;
    }
    if (ambiguous || resolution.resolved ||
        HasTerminalStrictDispatchError(resolution)) {
      return true;
    }
    if (!ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
            state, category_record->adopted_protocol_refs, family,
            selector_stable_id, selector_spelling, protocol_probe_count,
            ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
  }
  return true;
}

}  // namespace objc3c::runtime
