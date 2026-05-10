#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_module_import_graph.h"

struct Objc3IRFrontendModuleSourceLinkageMetadata
    : Objc3IRFrontendModuleImportGraphMetadata {
  std::string lowering_incremental_module_cache_invalidation_replay_key;
  std::size_t incremental_module_cache_invalidation_lowering_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_namespace_segment_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_import_edge_candidate_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_object_pointer_type_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_pointer_declarator_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_normalized_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_contract_violation_sites = 0;
  bool deterministic_incremental_module_cache_invalidation_lowering_handoff = false;
  std::string lowering_cross_module_conformance_replay_key;
  std::size_t cross_module_conformance_lowering_sites = 0;
  std::size_t cross_module_conformance_lowering_namespace_segment_sites = 0;
  std::size_t cross_module_conformance_lowering_import_edge_candidate_sites = 0;
  std::size_t cross_module_conformance_lowering_object_pointer_type_sites = 0;
  std::size_t cross_module_conformance_lowering_pointer_declarator_sites = 0;
  std::size_t cross_module_conformance_lowering_normalized_sites = 0;
  std::size_t cross_module_conformance_lowering_cache_invalidation_candidate_sites = 0;
  std::size_t cross_module_conformance_lowering_contract_violation_sites = 0;
  bool deterministic_cross_module_conformance_lowering_handoff = false;
};
