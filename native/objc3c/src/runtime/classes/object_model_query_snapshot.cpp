#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstdint>
#include <mutex>

extern "C" int objc3_runtime_copy_object_model_query_state_for_testing(
    objc3_runtime_object_model_query_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->realized_class_count = 0;
  snapshot->reflectable_property_count = 0;
  snapshot->attached_category_count = 0;
  snapshot->protocol_conformance_edge_count = 0;
  snapshot->method_cache_entry_count = 0;
  snapshot->last_class_query_found = 0;
  snapshot->last_property_query_found = 0;
  snapshot->last_property_query_inherited = 0;
  snapshot->last_protocol_query_class_found = 0;
  snapshot->last_protocol_query_protocol_found = 0;
  snapshot->last_protocol_query_conforms = 0;
  snapshot->last_queried_class_name = nullptr;
  snapshot->last_resolved_class_name = nullptr;
  snapshot->last_resolved_class_owner_identity = nullptr;
  snapshot->last_queried_property_name = nullptr;
  snapshot->last_resolved_property_class_name = nullptr;
  snapshot->last_resolved_property_owner_identity = nullptr;
  snapshot->last_queried_protocol_class_name = nullptr;
  snapshot->last_queried_protocol_name = nullptr;
  snapshot->last_matched_protocol_owner_identity = nullptr;
  snapshot->last_matched_attachment_owner_identity = nullptr;

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->protocol_conformance_edge_count =
      state.realized_protocol_conformance_edge_count;
  snapshot->method_cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->last_class_query_found = state.last_class_query_found ? 1 : 0;
  snapshot->last_property_query_found = state.last_property_query_found ? 1 : 0;
  snapshot->last_property_query_inherited =
      state.last_property_query_inherited ? 1 : 0;
  snapshot->last_protocol_query_class_found =
      state.last_protocol_query_class_found ? 1 : 0;
  snapshot->last_protocol_query_protocol_found =
      state.last_protocol_query_protocol_found ? 1 : 0;
  snapshot->last_protocol_query_conforms =
      state.last_protocol_query_conforms ? 1 : 0;
  for (const objc3c::runtime::RealizedClassNode &node :
       state.realized_class_nodes) {
    snapshot->attached_category_count +=
        static_cast<std::uint64_t>(node.attached_category_records.size());
    snapshot->reflectable_property_count +=
        static_cast<std::uint64_t>(node.runtime_property_accessors.size());
  }
  snapshot->last_queried_class_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_queried_class_name);
  snapshot->last_resolved_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_resolved_class_query_name);
  snapshot->last_resolved_class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_resolved_class_query_owner_identity);
  snapshot->last_queried_property_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_queried_property_name);
  snapshot->last_resolved_property_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_reflected_property_class_name);
  snapshot->last_resolved_property_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_reflected_property_owner_identity);
  snapshot->last_queried_protocol_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_class_name);
  snapshot->last_queried_protocol_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_protocol_name);
  snapshot->last_matched_protocol_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_owner_identity);
  snapshot->last_matched_attachment_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_attachment_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
