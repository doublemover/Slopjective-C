#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendModuleImportGraphCoreMetadata {
  std::string lowering_module_import_graph_replay_key;
  std::size_t module_import_graph_lowering_sites = 0;
  std::size_t module_import_graph_lowering_import_edge_candidate_sites = 0;
  std::size_t module_import_graph_lowering_namespace_segment_sites = 0;
  std::size_t module_import_graph_lowering_object_pointer_type_sites = 0;
  std::size_t module_import_graph_lowering_pointer_declarator_sites = 0;
  std::size_t module_import_graph_lowering_normalized_sites = 0;
  std::size_t module_import_graph_lowering_contract_violation_sites = 0;
  bool deterministic_module_import_graph_lowering_handoff = false;
};
