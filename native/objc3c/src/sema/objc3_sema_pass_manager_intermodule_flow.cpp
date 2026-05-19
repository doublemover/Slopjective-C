#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaIntermoduleFlowSummaryReadinessRecord
BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff) {
  Objc3SemaIntermoduleFlowSummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const auto &cross_module_summary =
      surface.cross_module_conformance_summary;
  record.cross_module_conformance_ready =
      deterministic_cross_module_conformance_handoff &&
      cross_module_summary.cross_module_conformance_sites ==
          surface.cross_module_conformance_sites_total &&
      cross_module_summary.namespace_segment_sites ==
          surface.cross_module_conformance_namespace_segment_sites_total &&
      cross_module_summary.import_edge_candidate_sites ==
          surface
              .cross_module_conformance_import_edge_candidate_sites_total &&
      cross_module_summary.object_pointer_type_sites ==
          surface.cross_module_conformance_object_pointer_type_sites_total &&
      cross_module_summary.pointer_declarator_sites ==
          surface.cross_module_conformance_pointer_declarator_sites_total &&
      cross_module_summary.normalized_sites ==
          surface.cross_module_conformance_normalized_sites_total &&
      cross_module_summary.cache_invalidation_candidate_sites ==
          surface
              .cross_module_conformance_cache_invalidation_candidate_sites_total &&
      cross_module_summary.contract_violation_sites ==
          surface.cross_module_conformance_contract_violation_sites_total &&
      cross_module_summary.namespace_segment_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.import_edge_candidate_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.normalized_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.cache_invalidation_candidate_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.normalized_sites +
              cross_module_summary.cache_invalidation_candidate_sites ==
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.contract_violation_sites <=
          cross_module_summary.cross_module_conformance_sites &&
      cross_module_summary.deterministic;
  const auto &throws_summary = surface.throws_propagation_summary;
  record.throws_propagation_ready =
      deterministic_throws_propagation_handoff &&
      throws_summary.throws_propagation_sites ==
          surface.throws_propagation_sites_total &&
      throws_summary.namespace_segment_sites ==
          surface.throws_propagation_namespace_segment_sites_total &&
      throws_summary.import_edge_candidate_sites ==
          surface.throws_propagation_import_edge_candidate_sites_total &&
      throws_summary.object_pointer_type_sites ==
          surface.throws_propagation_object_pointer_type_sites_total &&
      throws_summary.pointer_declarator_sites ==
          surface.throws_propagation_pointer_declarator_sites_total &&
      throws_summary.normalized_sites ==
          surface.throws_propagation_normalized_sites_total &&
      throws_summary.cache_invalidation_candidate_sites ==
          surface
              .throws_propagation_cache_invalidation_candidate_sites_total &&
      throws_summary.contract_violation_sites ==
          surface.throws_propagation_contract_violation_sites_total &&
      throws_summary.namespace_segment_sites <=
          throws_summary.throws_propagation_sites &&
      throws_summary.import_edge_candidate_sites <=
          throws_summary.throws_propagation_sites &&
      throws_summary.normalized_sites <= throws_summary.throws_propagation_sites &&
      throws_summary.cache_invalidation_candidate_sites <=
          throws_summary.throws_propagation_sites &&
      throws_summary.normalized_sites +
              throws_summary.cache_invalidation_candidate_sites ==
          throws_summary.throws_propagation_sites &&
      throws_summary.contract_violation_sites <=
          throws_summary.throws_propagation_sites &&
      throws_summary.deterministic;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.intermodule_flow_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.cross_module_conformance_ready &&
      record.throws_propagation_ready;
  return record;
}

Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
BuildObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff) {
  Objc3SemaIntermoduleFlowParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const Objc3SemaIntermoduleFlowSummaryReadinessRecord
      intermodule_summary_readiness =
          BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
              input,
              surface,
              deterministic_cross_module_conformance_handoff,
              deterministic_throws_propagation_handoff);
  record.cross_module_conformance_ready =
      intermodule_summary_readiness.cross_module_conformance_ready;
  record.throws_propagation_ready =
      intermodule_summary_readiness.throws_propagation_ready;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.cross_module_conformance_ready) +
      Objc3SemaEvidenceCount(record.throws_propagation_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.intermodule_flow_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_publication_count == 2u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u &&
      IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          intermodule_summary_readiness);
  return record;
}
