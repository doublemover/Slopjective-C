#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/objc3_sema_contract.h"
#include "sema/model/semantic_symbol_advanced_source_contracts.h"

struct Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary {
  std::string contract_id = kObjc3ConcurrencyTaskGroupCancellationSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3ConcurrencyTaskGroupCancellationSourceClosureSurfacePath;
  std::string source_model = kObjc3ConcurrencyTaskGroupCancellationSourceClosureSourceModel;
  std::string failure_model = kObjc3ConcurrencyTaskGroupCancellationSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimTaskCreationSites,
      kObjc3SourceOnlyFeatureClaimSupportedTaskGroupSurface,
      kObjc3SourceOnlyFeatureClaimTaskExecutorCancellationProfiles,
  };
  std::size_t async_callable_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  bool task_creation_source_supported = false;
  bool task_group_source_supported = false;
  bool cancellation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendSymbolGraphScopeResolutionSummary {
  std::size_t global_symbol_nodes = 0;
  std::size_t function_symbol_nodes = 0;
  std::size_t interface_symbol_nodes = 0;
  std::size_t implementation_symbol_nodes = 0;
  std::size_t interface_property_symbol_nodes = 0;
  std::size_t implementation_property_symbol_nodes = 0;
  std::size_t interface_method_symbol_nodes = 0;
  std::size_t implementation_method_symbol_nodes = 0;
  std::size_t top_level_scope_symbols = 0;
  std::size_t nested_scope_symbols = 0;
  std::size_t scope_frames_total = 0;
  std::size_t implementation_interface_resolution_sites = 0;
  std::size_t implementation_interface_resolution_hits = 0;
  std::size_t implementation_interface_resolution_misses = 0;
  std::size_t method_resolution_sites = 0;
  std::size_t method_resolution_hits = 0;
  std::size_t method_resolution_misses = 0;
  bool deterministic_symbol_graph_handoff = true;
  bool deterministic_scope_resolution_handoff = true;
  std::string deterministic_handoff_key;

  std::size_t symbol_nodes_total() const {
    return global_symbol_nodes + function_symbol_nodes + interface_symbol_nodes +
           implementation_symbol_nodes + interface_property_symbol_nodes +
           implementation_property_symbol_nodes + interface_method_symbol_nodes +
           implementation_method_symbol_nodes;
  }

  std::size_t resolution_sites_total() const {
    return implementation_interface_resolution_sites + method_resolution_sites;
  }

  std::size_t resolution_hits_total() const {
    return implementation_interface_resolution_hits + method_resolution_hits;
  }

  std::size_t resolution_misses_total() const {
    return implementation_interface_resolution_misses + method_resolution_misses;
  }
};
