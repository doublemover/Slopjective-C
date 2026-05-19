#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendRuntimeExportMetadata {
  std::string runtime_export_legality_contract_id;
  bool runtime_export_semantic_boundary_frozen = false;
  bool runtime_export_metadata_export_enforcement_ready = false;
  bool runtime_export_fail_closed = false;
  bool runtime_export_duplicate_runtime_identity_enforcement_pending = true;
  bool runtime_export_incomplete_declaration_export_blocking_pending = true;
  bool runtime_export_illegal_redeclaration_mix_export_blocking_pending = true;
  std::size_t runtime_export_class_record_count = 0;
  std::size_t runtime_export_protocol_record_count = 0;
  std::size_t runtime_export_category_record_count = 0;
  std::size_t runtime_export_property_record_count = 0;
  std::size_t runtime_export_method_record_count = 0;
  std::size_t runtime_export_ivar_record_count = 0;
  std::size_t runtime_export_invalid_protocol_composition_sites = 0;
  std::size_t runtime_export_property_attribute_invalid_entries = 0;
  std::size_t runtime_export_property_attribute_contract_violations = 0;
  std::size_t runtime_export_invalid_type_annotation_sites = 0;
  std::size_t runtime_export_property_ivar_binding_missing = 0;
  std::size_t runtime_export_property_ivar_binding_conflicts = 0;
  std::size_t runtime_export_implementation_resolution_misses = 0;
  std::size_t runtime_export_method_resolution_misses = 0;
  bool runtime_export_boundary_ready = false;
  std::string runtime_export_enforcement_contract_id;
  bool runtime_export_metadata_completeness_enforced = false;
  bool runtime_export_duplicate_runtime_identity_suppression_enforced = false;
  bool runtime_export_illegal_redeclaration_mix_blocking_enforced = false;
  bool runtime_export_metadata_shape_drift_blocking_enforced = false;
  bool runtime_export_enforcement_fail_closed = false;
  bool runtime_export_ready_for_runtime_export = false;
  std::size_t runtime_export_duplicate_runtime_identity_sites = 0;
  std::size_t runtime_export_incomplete_declaration_sites = 0;
  std::size_t runtime_export_illegal_redeclaration_mix_sites = 0;
  std::size_t runtime_export_metadata_shape_drift_sites = 0;
};
