#pragma once

#include <cstddef>
#include <string>

#include "ast/objc3_ast_contracts_source_property_metadata.h"
#include "runtime/metadata/runtime_metadata_model.h"

struct Objc3RuntimeMetadataSourceOwnershipBoundary {
  std::string contract_id = kObjc3RuntimeMetadataSourceOwnershipContractId;
  std::string canonical_source_schema = kObjc3RuntimeMetadataCanonicalSourceSchema;
  std::string class_record_ast_anchor = kObjc3RuntimeMetadataClassAstAnchor;
  std::string protocol_record_ast_anchor = kObjc3RuntimeMetadataProtocolAstAnchor;
  std::string category_record_ast_anchor = kObjc3RuntimeMetadataCategoryAstAnchor;
  std::string property_record_ast_anchor = kObjc3RuntimeMetadataPropertyAstAnchor;
  std::string method_record_ast_anchor = kObjc3RuntimeMetadataMethodAstAnchor;
  std::string ivar_record_ast_anchor = kObjc3RuntimeMetadataIvarAstAnchor;
  std::string ivar_record_source_model = kObjc3RuntimeMetadataIvarSourceModel;
  bool frontend_owns_runtime_metadata_source_records = false;
  bool runtime_metadata_source_records_ready_for_lowering = false;
  bool native_runtime_library_present = false;
  bool runtime_link_test_only = true;
  bool deterministic_source_schema = false;
  bool fail_closed = false;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_interface_record_count = 0;
  std::size_t category_implementation_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::string failure_reason;

  std::size_t category_record_count() const {
    return category_interface_record_count + category_implementation_record_count;
  }
};

inline bool IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary &boundary) {
  return boundary.frontend_owns_runtime_metadata_source_records &&
         !boundary.runtime_metadata_source_records_ready_for_lowering &&
         !boundary.native_runtime_library_present &&
         boundary.runtime_link_test_only &&
         boundary.deterministic_source_schema &&
         boundary.fail_closed &&
         !boundary.contract_id.empty() &&
         !boundary.canonical_source_schema.empty() &&
         !boundary.class_record_ast_anchor.empty() &&
         !boundary.protocol_record_ast_anchor.empty() &&
         !boundary.category_record_ast_anchor.empty() &&
         !boundary.property_record_ast_anchor.empty() &&
         !boundary.method_record_ast_anchor.empty() &&
         !boundary.ivar_record_ast_anchor.empty() &&
         !boundary.ivar_record_source_model.empty() &&
         boundary.ivar_record_count <= boundary.property_record_count &&
         boundary.failure_reason.empty();
}

struct Objc3RuntimeExportLegalityBoundary {
  std::string contract_id = kObjc3RuntimeExportLegalityContractId;
  bool semantic_boundary_frozen = false;
  bool metadata_export_enforcement_ready = false;
  bool fail_closed = false;
  bool semantic_integration_surface_built = false;
  bool sema_type_metadata_handoff_deterministic = false;
  bool typed_sema_surface_ready = false;
  bool typed_sema_surface_deterministic = false;
  bool runtime_metadata_source_boundary_ready = false;
  bool protocol_category_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool selector_normalization_deterministic = false;
  bool property_attribute_deterministic = false;
  bool object_pointer_surface_deterministic = false;
  bool symbol_graph_scope_resolution_deterministic = false;
  bool property_synthesis_ivar_binding_deterministic = false;
  bool duplicate_runtime_identity_enforcement_pending = true;
  bool incomplete_declaration_export_blocking_pending = true;
  bool illegal_redeclaration_mix_export_blocking_pending = true;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  std::size_t property_attribute_invalid_entries = 0;
  std::size_t property_attribute_contract_violations = 0;
  std::size_t invalid_type_annotation_sites = 0;
  std::size_t property_ivar_binding_missing = 0;
  std::size_t property_ivar_binding_conflicts = 0;
  std::size_t implementation_resolution_misses = 0;
  std::size_t method_resolution_misses = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeExportLegalityBoundary(
    const Objc3RuntimeExportLegalityBoundary &boundary) {
  return !boundary.contract_id.empty() &&
         boundary.semantic_boundary_frozen &&
         !boundary.metadata_export_enforcement_ready &&
         boundary.fail_closed &&
         boundary.sema_type_metadata_handoff_deterministic &&
         boundary.typed_sema_surface_ready &&
         boundary.typed_sema_surface_deterministic &&
         boundary.runtime_metadata_source_boundary_ready &&
         boundary.protocol_category_deterministic &&
         boundary.class_protocol_category_linking_deterministic &&
         boundary.selector_normalization_deterministic &&
         boundary.property_attribute_deterministic &&
         boundary.object_pointer_surface_deterministic &&
         boundary.symbol_graph_scope_resolution_deterministic &&
         boundary.property_synthesis_ivar_binding_deterministic &&
         boundary.duplicate_runtime_identity_enforcement_pending &&
         boundary.incomplete_declaration_export_blocking_pending &&
         boundary.illegal_redeclaration_mix_export_blocking_pending &&
         boundary.invalid_protocol_composition_sites <=
             boundary.protocol_record_count + boundary.category_record_count &&
         boundary.ivar_record_count <= boundary.property_record_count &&
         boundary.failure_reason.empty();
}

struct Objc3RuntimeExportEnforcementSummary {
  std::string contract_id = kObjc3RuntimeExportEnforcementContractId;
  bool metadata_completeness_enforced = false;
  bool duplicate_runtime_identity_suppression_enforced = false;
  bool illegal_redeclaration_mix_blocking_enforced = false;
  bool metadata_shape_drift_blocking_enforced = false;
  bool fail_closed = false;
  bool ready_for_runtime_export = false;
  std::size_t duplicate_runtime_identity_sites = 0;
  std::size_t incomplete_declaration_sites = 0;
  std::size_t illegal_redeclaration_mix_sites = 0;
  std::size_t metadata_shape_drift_sites = 0;
  unsigned first_failure_line = 1;
  unsigned first_failure_column = 1;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeExportEnforcementSummary(
    const Objc3RuntimeExportEnforcementSummary &summary) {
  return !summary.contract_id.empty() &&
         summary.metadata_completeness_enforced &&
         summary.duplicate_runtime_identity_suppression_enforced &&
         summary.illegal_redeclaration_mix_blocking_enforced &&
         summary.metadata_shape_drift_blocking_enforced &&
         summary.fail_closed &&
         summary.ready_for_runtime_export &&
         summary.duplicate_runtime_identity_sites == 0 &&
         summary.incomplete_declaration_sites == 0 &&
         summary.illegal_redeclaration_mix_sites == 0 &&
         summary.metadata_shape_drift_sites == 0 &&
         summary.failure_reason.empty();
}
