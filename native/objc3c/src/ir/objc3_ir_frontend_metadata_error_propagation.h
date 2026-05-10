#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendErrorPropagationMetadata {
  std::string lowering_error_handling_throws_abi_propagation_replay_key;
  std::string lowering_throws_propagation_replay_key;
  std::string lowering_result_like_replay_key;
  bool deterministic_result_like_lowering_handoff = false;
  std::size_t throws_propagation_lowering_sites = 0;
  std::size_t throws_propagation_lowering_namespace_segment_sites = 0;
  std::size_t throws_propagation_lowering_import_edge_candidate_sites = 0;
  std::size_t throws_propagation_lowering_object_pointer_type_sites = 0;
  std::size_t throws_propagation_lowering_pointer_declarator_sites = 0;
  std::size_t throws_propagation_lowering_normalized_sites = 0;
  std::size_t
      throws_propagation_lowering_cache_invalidation_candidate_sites = 0;
  std::size_t throws_propagation_lowering_contract_violation_sites = 0;
  bool deterministic_throws_propagation_lowering_handoff = false;
};
