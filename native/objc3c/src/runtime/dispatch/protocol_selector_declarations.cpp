#include "runtime/dispatch/protocol_selector_declarations.h"

#include "runtime/classes/protocol_conformance.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/method_list_resolution.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/metadata/runtime_emitted_records.h"

#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

namespace {

bool ProbeProtocolSelectorDeclarationUnlocked(
    RuntimeState &state, const EmittedProtocolRecord *start_record,
    DispatchFamily family, std::uint64_t selector_stable_id,
    const char *selector_spelling, std::uint64_t &protocol_probe_count,
    bool &declared, bool &ambiguous) {
  // Protocol method lists provide declaration-aware negative resolution while
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
        SelectRuntimeMethodListRef(*record, family);
    if (method_list_ref != nullptr && method_list_ref->count != 0 &&
        method_list_ref->method_list != nullptr) {
      const auto *header =
          static_cast<const EmittedMethodListHeader *>(
              method_list_ref->method_list);
      if (header == nullptr || header->count != method_list_ref->count) {
        return false;
      }
      const EmittedMethodListEntry *entries =
          RuntimeMethodListEntries(header);
      for (std::uint64_t index = 0; index < header->count; ++index) {
        const EmittedMethodListEntry &entry = entries[index];
        if (entry.selector == nullptr || entry.owner_identity == nullptr ||
            entry.return_type_name == nullptr) {
          return false;
        }
        if (!RuntimeMethodEntrySelectorMatchesUnlocked(
                state, entry.selector, selector_stable_id,
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
          RuntimeAggregateEntry(inherited_refs, index));
      if (inherited_record == nullptr) {
        return false;
      }
      stack.push_back(inherited_record);
    }
  }
  return true;
}

}  // namespace

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
        RuntimeAggregateEntry(protocol_refs, index));
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

}  // namespace objc3c::runtime
