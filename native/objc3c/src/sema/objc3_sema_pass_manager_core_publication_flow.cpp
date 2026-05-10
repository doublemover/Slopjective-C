#include "sema/objc3_sema_pass_manager_contract_flow.h"

#include "objc3_sema_pass_manager_core_publication_flow_record_seed.inc"
#include "objc3_sema_pass_manager_core_publication_flow_core_readiness.inc"
#include "objc3_sema_pass_manager_core_publication_flow_type_readiness.inc"
#include "objc3_sema_pass_manager_core_publication_flow_boundary_readiness.inc"
#include "objc3_sema_pass_manager_core_publication_flow_finalization.inc"

Objc3SemaCoreSemanticParityPublicationReadinessRecord
BuildObjc3SemaCoreSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff,
    bool deterministic_interface_implementation_handoff,
    bool deterministic_protocol_category_composition_handoff,
    bool deterministic_class_protocol_category_linking_handoff,
    bool deterministic_selector_normalization_handoff,
    bool deterministic_property_attribute_handoff,
    bool deterministic_type_annotation_surface_handoff,
    bool deterministic_lightweight_generic_constraint_handoff,
    bool deterministic_nullability_flow_warning_precision_handoff,
    bool deterministic_protocol_qualified_object_type_handoff,
    bool deterministic_variance_bridge_cast_handoff) {
  Objc3SemaCoreSemanticParityPublicationReadinessRecord record =
      BuildObjc3SemaCorePublicationFlowRecordSeed(input);
  PopulateObjc3SemaCorePublicationFlowCoreReadiness(
      record, surface, deterministic_semantic_diagnostics,
      deterministic_type_metadata_handoff,
      deterministic_interface_implementation_handoff,
      deterministic_protocol_category_composition_handoff,
      deterministic_class_protocol_category_linking_handoff);
  PopulateObjc3SemaCorePublicationFlowTypeReadiness(
      record, surface, deterministic_selector_normalization_handoff,
      deterministic_property_attribute_handoff,
      deterministic_type_annotation_surface_handoff);
  PopulateObjc3SemaCorePublicationFlowBoundaryReadiness(
      record, surface, deterministic_lightweight_generic_constraint_handoff,
      deterministic_nullability_flow_warning_precision_handoff,
      deterministic_protocol_qualified_object_type_handoff,
      deterministic_variance_bridge_cast_handoff);
  FinalizeObjc3SemaCorePublicationFlowRecord(record);
  return record;
}
