#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_block_execution.h"

struct Objc3IRFrontendBlockMetadata : Objc3IRFrontendBlockExecutionMetadata {
  std::string lowering_block_source_model_completion_replay_key;
  std::size_t block_source_model_completion_block_literal_sites = 0;
  std::size_t block_source_model_completion_signature_entries_total = 0;
  std::size_t block_source_model_completion_explicit_typed_parameter_entries_total = 0;
  std::size_t block_source_model_completion_implicit_parameter_entries_total = 0;
  std::size_t block_source_model_completion_capture_inventory_entries_total = 0;
  std::size_t block_source_model_completion_byvalue_readonly_capture_entries_total = 0;
  std::size_t block_source_model_completion_invoke_surface_entries_total = 0;
  std::size_t block_source_model_completion_non_normalized_sites = 0;
  std::size_t block_source_model_completion_contract_violation_sites = 0;
  bool deterministic_block_source_model_completion_handoff = false;
  std::string lowering_block_source_storage_annotation_replay_key;
  std::size_t block_source_storage_annotation_block_literal_sites = 0;
  std::size_t block_source_storage_annotation_capture_entries_total = 0;
  std::size_t block_source_storage_annotation_mutated_capture_entries_total = 0;
  std::size_t block_source_storage_annotation_byref_capture_entries_total = 0;
  std::size_t block_source_storage_annotation_copy_helper_intent_sites = 0;
  std::size_t block_source_storage_annotation_dispose_helper_intent_sites = 0;
  std::size_t block_source_storage_annotation_heap_candidate_sites = 0;
  std::size_t block_source_storage_annotation_expression_sites = 0;
  std::size_t block_source_storage_annotation_global_initializer_sites = 0;
  std::size_t block_source_storage_annotation_binding_initializer_sites = 0;
  std::size_t block_source_storage_annotation_assignment_value_sites = 0;
  std::size_t block_source_storage_annotation_return_value_sites = 0;
  std::size_t block_source_storage_annotation_call_argument_sites = 0;
  std::size_t block_source_storage_annotation_message_argument_sites = 0;
  std::size_t block_source_storage_annotation_non_normalized_sites = 0;
  std::size_t block_source_storage_annotation_contract_violation_sites = 0;
  bool deterministic_block_source_storage_annotation_handoff = false;
};
