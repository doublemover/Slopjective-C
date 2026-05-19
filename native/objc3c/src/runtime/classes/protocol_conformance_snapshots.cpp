#include "runtime/classes/protocol_conformance.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>

extern "C" int objc3_runtime_copy_protocol_conformance_query_for_testing(
    const char *class_name,
    const char *protocol_name,
    objc3_runtime_protocol_conformance_query_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->class_found = 0;
  snapshot->protocol_found = 0;
  snapshot->conforms = 0;
  snapshot->visited_protocol_count = 0;
  snapshot->attached_category_count = 0;
  snapshot->class_name = nullptr;
  snapshot->protocol_name = nullptr;
  snapshot->matched_protocol_owner_identity = nullptr;
  snapshot->matched_attachment_owner_identity = nullptr;

  if (class_name == nullptr || class_name[0] == '\0' ||
      protocol_name == nullptr || protocol_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_protocol_conformance_class_name = class_name;
  state.last_protocol_conformance_protocol_name = protocol_name;
  state.last_protocol_conformance_owner_identity.clear();
  state.last_protocol_conformance_attachment_owner_identity.clear();
  state.last_protocol_query_class_found = false;
  state.last_protocol_query_protocol_found = false;
  state.last_protocol_query_conforms = false;
  snapshot->class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_class_name);
  snapshot->protocol_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_protocol_name);
  snapshot->protocol_found =
      objc3c::runtime::ProtocolExistsByNameUnlocked(state, protocol_name) ? 1
                                                                          : 0;
  state.last_protocol_query_protocol_found =
      snapshot->protocol_found != 0;

  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const objc3c::runtime::RealizedClassNode &node =
      state.realized_class_nodes[node_index];
  snapshot->class_found = 1;
  state.last_protocol_query_class_found = true;
  snapshot->attached_category_count =
      static_cast<std::uint64_t>(node.attached_category_records.size());

  std::string matched_protocol_owner_identity;
  std::string matched_attachment_owner_identity;
  if (objc3c::runtime::QueryRealizedClassProtocolConformanceUnlocked(
          state, &node, protocol_name, snapshot->visited_protocol_count,
          matched_protocol_owner_identity, matched_attachment_owner_identity)) {
    snapshot->conforms = 1;
    state.last_protocol_query_conforms = true;
    state.last_protocol_conformance_owner_identity =
        matched_protocol_owner_identity;
    state.last_protocol_conformance_attachment_owner_identity =
        matched_attachment_owner_identity;
    snapshot->matched_protocol_owner_identity =
        objc3c::runtime::BorrowRuntimeCString(
            state.last_protocol_conformance_owner_identity);
    snapshot->matched_attachment_owner_identity =
        objc3c::runtime::BorrowRuntimeCString(
            state.last_protocol_conformance_attachment_owner_identity);
  }
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
