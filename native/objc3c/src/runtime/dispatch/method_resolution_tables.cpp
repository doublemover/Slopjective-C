#include "runtime/dispatch/method_resolution_tables.h"

#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/method_list_resolution.h"
#include "runtime/dispatch/protocol_selector_declarations.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"

#include <cstdint>

namespace objc3c::runtime {

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
            state, SelectRuntimeMethodListRef(*category_record, family),
            node.class_name.c_str(), family, normalized_receiver_identity,
            selector_stable_id, selector_spelling, resolution, ambiguous)) {
      return false;
    }
    if (ambiguous || resolution.resolved ||
        HasTerminalStrictDispatchError(resolution)) {
      if (ambiguous || HasTerminalStrictDispatchError(resolution)) {
        return true;
      }
      continue;
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
