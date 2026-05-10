#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendRuntimeSupportEvidenceMetadata {
  std::string runtime_metadata_object_inspection_contract_id;
  std::string runtime_metadata_object_inspection_publication_contract_id;
  bool runtime_metadata_object_inspection_matrix_published = false;
  bool runtime_metadata_object_inspection_fail_closed = false;
  bool runtime_metadata_object_inspection_uses_llvm_readobj = false;
  bool runtime_metadata_object_inspection_uses_llvm_objdump = false;
  std::size_t runtime_metadata_object_inspection_matrix_row_count = 0;
  std::string runtime_metadata_object_inspection_fixture_path;
  std::string runtime_metadata_object_inspection_emit_prefix;
  std::string runtime_metadata_object_inspection_object_relative_path;
  std::string runtime_metadata_object_inspection_section_inventory_row_key;
  std::string runtime_metadata_object_inspection_section_inventory_command;
  std::string runtime_metadata_object_inspection_symbol_inventory_row_key;
  std::string runtime_metadata_object_inspection_symbol_inventory_command;
  std::string executable_metadata_debug_projection_contract_id;
  std::string executable_metadata_debug_projection_typed_handoff_contract_id;
  std::string executable_metadata_debug_projection_source_graph_contract_id;
  std::string executable_metadata_debug_projection_named_metadata_name;
  std::string executable_metadata_debug_projection_manifest_surface_path;
  std::string executable_metadata_debug_projection_typed_handoff_surface_path;
  std::string executable_metadata_debug_projection_source_graph_surface_path;
  bool executable_metadata_debug_projection_matrix_published = false;
  bool executable_metadata_debug_projection_fail_closed = false;
  bool executable_metadata_debug_projection_manifest_debug_surface_published =
      false;
  bool executable_metadata_debug_projection_ir_named_metadata_published = false;
  bool executable_metadata_debug_projection_replay_anchor_deterministic = false;
  bool executable_metadata_debug_projection_active_typed_handoff_ready = false;
  std::size_t executable_metadata_debug_projection_matrix_row_count = 0;
  std::string executable_metadata_debug_projection_replay_key;
  std::string executable_metadata_debug_projection_active_typed_handoff_replay_key;
  std::string executable_metadata_debug_projection_row0_descriptor;
  std::string executable_metadata_debug_projection_row1_descriptor;
  std::string executable_metadata_debug_projection_row2_descriptor;
};
