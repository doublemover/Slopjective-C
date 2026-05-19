#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendModuleCacheInvalidationMetadata {
  std::string lowering_incremental_module_cache_invalidation_replay_key;
  std::size_t incremental_module_cache_invalidation_lowering_sites = 0;
  std::size_t
      incremental_module_cache_invalidation_lowering_namespace_segment_sites =
          0;
  std::size_t
      incremental_module_cache_invalidation_lowering_import_edge_candidate_sites =
          0;
  std::size_t
      incremental_module_cache_invalidation_lowering_object_pointer_type_sites =
          0;
  std::size_t
      incremental_module_cache_invalidation_lowering_pointer_declarator_sites =
          0;
  std::size_t incremental_module_cache_invalidation_lowering_normalized_sites =
      0;
  std::size_t
      incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites =
          0;
  std::size_t
      incremental_module_cache_invalidation_lowering_contract_violation_sites =
          0;
  bool deterministic_incremental_module_cache_invalidation_lowering_handoff =
      false;
};
