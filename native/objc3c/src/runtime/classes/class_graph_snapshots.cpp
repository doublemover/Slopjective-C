#include "runtime/classes/class_graph.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/strings/borrowed_string.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <mutex>

extern "C" int objc3_runtime_copy_realized_class_graph_state_for_testing(
    objc3_runtime_realized_class_graph_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->root_class_count = state.realized_root_class_count;
  snapshot->metaclass_edge_count = state.realized_metaclass_edge_count;
  snapshot->receiver_class_binding_count = state.receiver_class_binding_count;
  snapshot->attached_category_count = state.realized_attached_category_count;
  snapshot->protocol_conformance_edge_count =
      state.realized_protocol_conformance_edge_count;
  snapshot->class_graph_generation = state.class_graph_generation;
  snapshot->category_attachment_generation =
      state.category_attachment_generation;
  snapshot->protocol_declaration_generation =
      state.protocol_declaration_generation;
  snapshot->storage_surface_generation = state.storage_surface_generation;
  snapshot->method_surface_generation = state.method_surface_generation;
  snapshot->live_instance_count = state.live_runtime_instance_count;
  snapshot->malformed_class_metadata_rejection_count =
      state.malformed_class_metadata_rejection_count;
  snapshot->last_allocated_receiver_identity =
      state.last_allocated_runtime_instance_receiver;
  snapshot->last_allocated_base_identity =
      state.last_allocated_runtime_instance_base_identity;
  snapshot->last_allocated_instance_size_bytes =
      state.last_allocated_runtime_instance_size_bytes;
  snapshot->last_allocated_allocation_ordinal =
      state.last_allocated_runtime_instance_allocation_ordinal;
  snapshot->last_initialized_receiver_identity =
      state.last_initialized_runtime_instance_receiver;
  snapshot->last_initialized_initialization_ordinal =
      state.last_initialized_runtime_instance_initialization_ordinal;
  snapshot->last_realized_class_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_realized_class_name);
  snapshot->last_realized_class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_realized_class_owner_identity);
  snapshot->last_realized_metaclass_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_realized_metaclass_owner_identity);
  snapshot->last_attached_category_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_attached_category_owner_identity);
  snapshot->last_attached_category_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_attached_category_name);
  snapshot->last_allocated_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_allocated_runtime_instance_class_name);
  snapshot->last_instance_lifecycle_failure_reason =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_runtime_instance_lifecycle_failure_reason);
  snapshot->last_malformed_class_graph_reason =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_malformed_class_graph_reason);
  const bool has_malformed_class_graph_reason =
      !state.last_malformed_class_graph_reason.empty();
  snapshot->last_malformed_class_graph_diagnostic_code =
      has_malformed_class_graph_reason ? "O3RT004" : "";
  snapshot->last_malformed_class_graph_diagnostic_message =
      has_malformed_class_graph_reason
          ? "runtime dispatch failed: malformed metadata"
          : "";
  snapshot->last_malformed_class_graph_diagnostic_class =
      has_malformed_class_graph_reason ? "malformed-runtime-metadata" : "";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_realized_class_entry_for_testing(
    const char *class_name,
    objc3_runtime_realized_class_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->base_identity = 0;
  snapshot->instance_receiver_identity = 0;
  snapshot->class_receiver_identity = 0;
  snapshot->registration_order_ordinal = 0;
  snapshot->is_root_class = 0;
  snapshot->has_super_node = 0;
  snapshot->implementation_backed = 0;
  snapshot->attached_category_count = 0;
  snapshot->direct_protocol_count = 0;
  snapshot->attached_protocol_count = 0;
  snapshot->runtime_property_accessor_count = 0;
  snapshot->runtime_instance_size_bytes = 0;
  snapshot->super_base_identity = 0;
  snapshot->module_name = nullptr;
  snapshot->translation_unit_identity_key = nullptr;
  snapshot->class_name = nullptr;
  snapshot->super_class_name = nullptr;
  snapshot->class_owner_identity = nullptr;
  snapshot->metaclass_owner_identity = nullptr;
  snapshot->super_class_owner_identity = nullptr;
  snapshot->super_metaclass_owner_identity = nullptr;
  snapshot->instance_isa_owner_identity = nullptr;
  snapshot->class_object_isa_owner_identity = nullptr;
  snapshot->metaclass_object_isa_owner_identity = nullptr;
  snapshot->root_class_owner_identity = nullptr;
  snapshot->root_metaclass_owner_identity = nullptr;
  snapshot->last_attached_category_owner_identity = nullptr;
  snapshot->last_attached_category_name = nullptr;

  if (class_name == nullptr || class_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_queried_class_name = class_name;
  state.last_resolved_class_query_name.clear();
  state.last_resolved_class_query_owner_identity.clear();
  state.last_class_query_found = false;
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
  state.last_class_query_found = true;
  state.last_resolved_class_query_name = node.class_name;
  state.last_resolved_class_query_owner_identity = node.class_owner_identity;
  snapshot->found = 1;
  snapshot->base_identity = node.base_identity;
  snapshot->instance_receiver_identity = node.base_identity + 1u;
  snapshot->class_receiver_identity = node.base_identity + 2u;
  snapshot->registration_order_ordinal = node.registration_order_ordinal;
  snapshot->is_root_class = node.is_root_class ? 1 : 0;
  snapshot->has_super_node = node.has_super_node ? 1 : 0;
  snapshot->implementation_backed = node.implementation_backed ? 1 : 0;
  snapshot->attached_category_count =
      static_cast<std::uint64_t>(node.attached_category_records.size());
  snapshot->runtime_property_accessor_count =
      static_cast<std::uint64_t>(node.runtime_property_accessors.size());
  snapshot->runtime_instance_size_bytes =
      static_cast<std::uint64_t>(node.runtime_instance_size_bytes);
  if (node.has_super_node &&
      node.super_node_index < state.realized_class_nodes.size()) {
    const objc3c::runtime::RealizedClassNode &super_node =
        state.realized_class_nodes[node.super_node_index];
    snapshot->super_base_identity = super_node.base_identity;
    snapshot->super_class_name =
        objc3c::runtime::BorrowRuntimeCString(super_node.class_name);
  }
  snapshot->direct_protocol_count =
      (node.bundle != nullptr &&
       node.bundle->class_record.adopted_protocol_refs != nullptr)
          ? node.bundle->class_record.adopted_protocol_refs->count
          : 0;
  snapshot->module_name =
      objc3c::runtime::BorrowRuntimeCString(node.module_name);
  snapshot->translation_unit_identity_key =
      objc3c::runtime::BorrowRuntimeCString(
          node.translation_unit_identity_key);
  snapshot->class_name =
      objc3c::runtime::BorrowRuntimeCString(node.class_name);
  snapshot->class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(node.class_owner_identity);
  snapshot->metaclass_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(node.metaclass_owner_identity);
  snapshot->super_class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(node.super_class_owner_identity);
  snapshot->super_metaclass_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          node.super_metaclass_owner_identity);
  snapshot->instance_isa_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(node.class_owner_identity);
  snapshot->class_object_isa_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(node.metaclass_owner_identity);
  const objc3c::runtime::RealizedClassNode *root_node = &node;
  std::uint64_t visited_count = 0;
  while (root_node->has_super_node &&
         root_node->super_node_index < state.realized_class_nodes.size() &&
         visited_count < state.realized_class_nodes.size()) {
    root_node = &state.realized_class_nodes[root_node->super_node_index];
    ++visited_count;
  }
  snapshot->metaclass_object_isa_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(root_node->metaclass_owner_identity);
  snapshot->root_class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(root_node->class_owner_identity);
  snapshot->root_metaclass_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(root_node->metaclass_owner_identity);
  std::uint64_t attached_protocol_count = 0;
  for (const objc3c::runtime::EmittedCategoryRecord *category_record :
       node.attached_category_records) {
    if (category_record != nullptr &&
        category_record->adopted_protocol_refs != nullptr) {
      attached_protocol_count += category_record->adopted_protocol_refs->count;
    }
  }
  snapshot->attached_protocol_count = attached_protocol_count;
  if (!node.attached_category_records.empty()) {
    const objc3c::runtime::EmittedCategoryRecord *last_category =
        node.attached_category_records.back();
    snapshot->last_attached_category_owner_identity =
        last_category->category_owner_identity != nullptr
            ? last_category->category_owner_identity
            : nullptr;
    snapshot->last_attached_category_name =
        last_category->category_name != nullptr ? last_category->category_name
                                                : nullptr;
  }
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_instance_entry_for_testing(
    int receiver_identity, objc3_runtime_instance_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->receiver_identity = 0;
  snapshot->base_identity = 0;
  snapshot->normalized_receiver_identity = 0;
  snapshot->class_receiver_identity = 0;
  snapshot->allocation_ordinal = 0;
  snapshot->initialized = 0;
  snapshot->initialization_ordinal = 0;
  snapshot->instance_size_bytes = 0;
  snapshot->storage_size_bytes = 0;
  snapshot->zero_initialized_storage_byte_count = 0;
  snapshot->retain_count = 0;
  snapshot->class_name = nullptr;
  snapshot->class_owner_identity = nullptr;
  snapshot->metaclass_owner_identity = nullptr;
  snapshot->instance_isa_owner_identity = nullptr;
  snapshot->class_object_isa_owner_identity = nullptr;

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto found = state.runtime_instances_by_receiver.find(receiver_identity);
  if (found == state.runtime_instances_by_receiver.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const objc3c::runtime::RuntimeInstanceRecord &instance = found->second;
  snapshot->found = 1;
  snapshot->receiver_identity = instance.receiver_identity;
  snapshot->base_identity = instance.base_identity;
  snapshot->normalized_receiver_identity =
      instance.normalized_receiver_identity;
  snapshot->class_receiver_identity = instance.class_receiver_identity;
  snapshot->allocation_ordinal = instance.allocation_ordinal;
  snapshot->initialized = instance.initialized ? 1u : 0u;
  snapshot->initialization_ordinal = instance.initialization_ordinal;
  snapshot->instance_size_bytes =
      static_cast<std::uint64_t>(instance.instance_size_bytes);
  snapshot->storage_size_bytes =
      static_cast<std::uint64_t>(instance.storage_bytes.size());
  snapshot->zero_initialized_storage_byte_count =
      static_cast<std::uint64_t>(std::count(instance.storage_bytes.begin(),
                                           instance.storage_bytes.end(), 0u));
  snapshot->retain_count = instance.retain_count;
  snapshot->class_name =
      objc3c::runtime::BorrowRuntimeCString(instance.class_name);
  snapshot->class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(instance.class_owner_identity);
  snapshot->metaclass_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(instance.metaclass_owner_identity);
  snapshot->instance_isa_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          instance.instance_isa_owner_identity);
  snapshot->class_object_isa_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          instance.class_object_isa_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
