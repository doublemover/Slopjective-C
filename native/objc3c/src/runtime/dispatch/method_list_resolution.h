#pragma once

#include <cstdint>

namespace objc3c::runtime {

enum class DispatchFamily;
struct EmittedCategoryRecord;
struct EmittedMethodListEntry;
struct EmittedMethodListHeader;
struct EmittedMethodListRef;
struct EmittedProtocolRecord;
struct RuntimeState;
struct SlowPathResolution;

const EmittedMethodListEntry *RuntimeMethodListEntries(
    const EmittedMethodListHeader *header);
const EmittedMethodListRef *SelectRuntimeMethodListRef(
    const EmittedProtocolRecord &record, DispatchFamily family);
const EmittedMethodListRef *SelectRuntimeMethodListRef(
    const EmittedCategoryRecord &record, DispatchFamily family);
bool RuntimeMethodEntrySelectorMatchesUnlocked(
    RuntimeState &state, const char *entry_selector,
    std::uint64_t selector_stable_id, const char *selector_spelling);
bool TryResolveMethodFromMethodListRefUnlocked(
    RuntimeState &state,
    const EmittedMethodListRef *method_list_ref,
    const char *resolved_class_name,
    DispatchFamily family,
    std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id,
    const char *selector_spelling,
    SlowPathResolution &resolution,
    bool &ambiguous);

}  // namespace objc3c::runtime
