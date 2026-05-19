#include "runtime/dispatch/method_list_resolution.h"

#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/runtime_method_shape.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/selectors/selector_table.h"

namespace objc3c::runtime {

const EmittedMethodListEntry *RuntimeMethodListEntries(
    const EmittedMethodListHeader *header) {
  return header == nullptr
             ? nullptr
             : reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
}

const EmittedMethodListRef *SelectRuntimeMethodListRef(
    const EmittedProtocolRecord &record, DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

const EmittedMethodListRef *SelectRuntimeMethodListRef(
    const EmittedCategoryRecord &record, DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

bool RuntimeMethodEntrySelectorMatchesUnlocked(
    RuntimeState &, const char *entry_selector,
    std::uint64_t selector_stable_id, const char *selector_spelling) {
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
  const EmittedMethodListEntry *entries = RuntimeMethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr) {
      resolution.strict_error_status =
          OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
      return false;
    }
    if (!RuntimeMethodEntrySelectorMatchesUnlocked(
            state, entry.selector, selector_stable_id, selector_spelling)) {
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

}  // namespace objc3c::runtime
