#if defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#if defined(_WIN32) && !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "runtime/concurrency/runtime_debug.h"
#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/errors/error_bridge.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/state/runtime_thread_records.h"

#if defined(_WIN32)
#include <windows.h>
#endif

using objc3c::runtime::EmittedCategoryRecord;
using objc3c::runtime::EmittedClassBundle;
using objc3c::runtime::EmittedClassRecord;
using objc3c::runtime::EmittedIvarDescriptor;
using objc3c::runtime::EmittedIvarLayoutRecord;
using objc3c::runtime::EmittedKeyPathDescriptor;
using objc3c::runtime::EmittedMethodListEntry;
using objc3c::runtime::EmittedMethodListHeader;
using objc3c::runtime::EmittedMethodListRef;
using objc3c::runtime::EmittedPropertyDescriptor;
using objc3c::runtime::EmittedProtocolRecord;
using objc3c::runtime::ClearImageWalkSnapshotUnlocked;
using objc3c::runtime::FindRuntimePropertyAccessorByNameUnlocked;
using objc3c::runtime::HasTerminalStrictDispatchError;
using objc3c::runtime::InvokeRuntimeBuiltinMethod;
using objc3c::runtime::MethodCacheEntry;
using objc3c::runtime::PropertyLookupCacheEntry;
using objc3c::runtime::PropertyLookupCacheKey;
using objc3c::runtime::PropertyLookupCacheKeyHash;
using objc3c::runtime::ProcessRuntimeState;
using objc3c::runtime::RealizedClassNode;
using objc3c::runtime::RealizedPropertyAccessor;
using objc3c::runtime::RegisteredImageMetadata;
using objc3c::runtime::RuntimeInstanceRecord;
using objc3c::runtime::RuntimeState;
using objc3c::runtime::RuntimeDispatchFrame;
using objc3c::runtime::RuntimeWeakSlotRef;
using objc3c::runtime::ReplayRegisteredImagesForTestingUnlocked;
using objc3c::runtime::SelectorSlot;
using objc3c::runtime::SlowPathResolution;
namespace {

using objc3c::runtime::ClearRuntimeTestingDispatchFrame;
using objc3c::runtime::CountQueuedAutoreleaseValues;
using objc3c::runtime::CurrentRuntimeDispatchFrame;
using objc3c::runtime::EffectiveIvarOffset;
using objc3c::runtime::EffectiveIvarSize;
using objc3c::runtime::EnqueueAutoreleaseValue;
using objc3c::runtime::ExchangeRuntimeManagedPropertyValueUnlocked;
using objc3c::runtime::MethodCacheKey;
using objc3c::runtime::MethodCacheKeyHash;
using objc3c::runtime::BuildReceiverBaseIdentity;
using objc3c::runtime::ClassifyRuntimeReturnType;
using objc3c::runtime::DecodeReceiverIdentity;
using objc3c::runtime::DescribeResolvedImplementationKind;
using objc3c::runtime::DispatchFamily;
using objc3c::runtime::IsRuntimeReceiverBaseIdentity;
using objc3c::runtime::RuntimeBuiltinKind;
using objc3c::runtime::RuntimeMethodReturnKind;
using objc3c::runtime::RuntimeMethodShapeStatus;
using objc3c::runtime::RuntimeReceiverIdentityBase;
using objc3c::runtime::RuntimeReceiverIdentityStride;
using objc3c::runtime::PopRuntimeAutoreleasePoolFrameValues;
using objc3c::runtime::PopRuntimeDispatchFrameAutoreleaseValues;
using objc3c::runtime::PushRuntimeAutoreleasePoolFrame;
using objc3c::runtime::PushRuntimeDispatchFrame;
using objc3c::runtime::ReadRuntimeManagedPropertyValueUnlocked;
using objc3c::runtime::RecordRuntimeAutoreleasePoolDrainedValue;
using objc3c::runtime::ReleaseRuntimeValueUnlocked;
using objc3c::runtime::RetainRuntimeValueUnlocked;
using objc3c::runtime::RuntimeAutoreleasePoolDepth;
using objc3c::runtime::RuntimeAutoreleasePoolDrainedValueCount;
using objc3c::runtime::RuntimeAutoreleasePoolMaxDepth;
using objc3c::runtime::RuntimeLastAutoreleasedValue;
using objc3c::runtime::RuntimeLastDrainedAutoreleaseValue;
using objc3c::runtime::SetRuntimeTestingDispatchFrame;
using objc3c::runtime::RuntimeTypedDispatchFailure;
using objc3c::runtime::RuntimeTypedDispatchResult;
using objc3c::runtime::RuntimeTypedDispatchSuccess;
using objc3c::runtime::UsesStrongOwnedRuntimeHooks;
using objc3c::runtime::BorrowRuntimeCString;
using objc3c::runtime::ClearMethodCacheStateUnlocked;
using objc3c::runtime::ClearRealizedClassGraphUnlocked;
using objc3c::runtime::ClearRuntimeInstanceStateUnlocked;
using objc3c::runtime::LookupSelectorUnlocked;
using objc3c::runtime::MaterializeKeyPathDescriptorUnlocked;
using objc3c::runtime::MaterializeSelectorLookupEntryUnlocked;
using objc3c::runtime::RecordPostResolutionStrictDispatchFailure;
using objc3c::runtime::RecordTypedDispatchSuccess;
using objc3c::runtime::StoreDispatchResultContractUnlocked;
using objc3c::runtime::WriteRuntimeManagedPropertyValueUnlocked;

const void *AggregateEntry(const objc3_runtime_pointer_aggregate *aggregate,
                           std::uint64_t index);
std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name);

std::size_t AlignTo(std::size_t value, std::size_t alignment) {
  const std::size_t effective_alignment = std::max<std::size_t>(alignment, 1u);
  const std::size_t remainder = value % effective_alignment;
  return remainder == 0u ? value : value + (effective_alignment - remainder);
}

std::string BuildSynthesizedInstanceMethodOwnerIdentity(
    const char *declaration_owner_identity, const char *selector) {
  const std::string owner =
      declaration_owner_identity != nullptr ? declaration_owner_identity : "";
  const std::string selector_text = selector != nullptr ? selector : "";
  return owner + "::instance_method:" + selector_text;
}

const EmittedMethodListEntry *MethodListEntries(
    const EmittedMethodListHeader *header) {
  return header == nullptr
             ? nullptr
             : reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
}

const EmittedMethodListRef *SelectMethodListRef(const EmittedProtocolRecord &record,
                                                DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

const EmittedMethodListRef *SelectMethodListRef(const EmittedCategoryRecord &record,
                                                DispatchFamily family) {
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
    resolution.selector_storage = selector_spelling != nullptr ? selector_spelling : "";
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

bool ProbeProtocolSelectorDeclarationUnlocked(
    RuntimeState &state, const EmittedProtocolRecord *start_record,
    DispatchFamily family, std::uint64_t selector_stable_id,
    const char *selector_spelling, std::uint64_t &protocol_probe_count,
    bool &declared, bool &ambiguous) {
  // protocol/category-aware resolution anchor: adopted/inherited
  // protocol method lists now provide declaration-aware negative resolution
  // while remaining non-callable, so unresolved selectors publish strict
  // dispatch errors with explicit protocol probe evidence.
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
        !objc3c::runtime::RuntimeProtocolConformanceEdgeIsMaterializable(
            "protocol-adopter", record->protocol_name)) {
      return false;
    }
    ++protocol_probe_count;
    const EmittedMethodListRef *method_list_ref = SelectMethodListRef(*record, family);
    if (method_list_ref != nullptr && method_list_ref->count != 0 &&
        method_list_ref->method_list != nullptr) {
      const auto *header =
          static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
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
          AggregateEntry(inherited_refs, index));
      if (inherited_record == nullptr) {
        return false;
      }
      stack.push_back(inherited_record);
