#pragma once

#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness.h"
#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness_typed_sema_advanced_private.h"
#include "pipeline/readiness/objc3_parse_lowering_failure_reason_readiness_typed_sema_alignment_private.h"

namespace objc3_parse_lowering_failure_reason_readiness_detail {

inline const char *FindTypedSemaLoweringSurfaceFailureReason(
    const Objc3ParseLoweringReadinessSurface &surface) {
  if (!surface.semantic_integration_surface_built) {
    return "semantic integration surface not built";
  }

  if (!surface.semantic_diagnostics_deterministic) {
    return "semantic diagnostics handoff is not deterministic";
  }

  if (!surface.semantic_type_metadata_deterministic) {
    return "semantic type metadata handoff is not deterministic";
  }

  if (!surface.protocol_category_deterministic) {
    return "protocol/category handoff is not deterministic";
  }

  if (!surface.class_protocol_category_linking_deterministic) {
    return "class/protocol/category linking handoff is not deterministic";
  }

  if (!surface.selector_normalization_deterministic) {
    return "selector normalization handoff is not deterministic";
  }

  if (!surface.property_attribute_deterministic) {
    return "property attribute handoff is not deterministic";
  }

  if (!surface.symbol_graph_deterministic) {
    return "symbol graph handoff is not deterministic";
  }

  if (!surface.scope_resolution_deterministic) {
    return "scope resolution handoff is not deterministic";
  }

  if (!surface.object_pointer_type_handoff_deterministic) {
    return "object pointer/nullability handoff is not deterministic";
  }

  if (!surface.typed_handoff_key_deterministic) {
    return "typed sema-to-lowering handoff key is not deterministic";
  }

  if (!surface.typed_sema_core_feature_consistent) {
    return "typed sema-to-lowering core feature contract is inconsistent";
  }

  if (!surface.typed_sema_core_feature_expansion_consistent) {
    return "typed sema-to-lowering core feature expansion is inconsistent";
  }

  if (surface.typed_sema_core_feature_expansion_key.empty()) {
    return "typed sema-to-lowering core feature expansion key is empty";
  }

  if (!surface.typed_sema_edge_case_compatibility_consistent) {
    return "typed sema-to-lowering edge-case compatibility is inconsistent";
  }

  if (!surface.typed_sema_edge_case_compatibility_ready) {
    return "typed sema-to-lowering edge-case compatibility is not ready";
  }

  if (surface.typed_sema_edge_case_compatibility_key.empty()) {
    return "typed sema-to-lowering edge-case compatibility key is empty";
  }

  if (!surface.typed_sema_edge_case_expansion_consistent) {
    return "typed sema-to-lowering edge-case expansion is inconsistent";
  }

  if (!surface.typed_sema_edge_case_robustness_ready) {
    return "typed sema-to-lowering edge-case robustness is not ready";
  }

  if (surface.typed_sema_edge_case_robustness_key.empty()) {
    return "typed sema-to-lowering edge-case robustness key is empty";
  }

  if (!surface.typed_sema_diagnostics_hardening_consistent) {
    return "typed sema-to-lowering diagnostics hardening is inconsistent";
  }

  if (!surface.typed_sema_diagnostics_hardening_ready) {
    return "typed sema-to-lowering diagnostics hardening is not ready";
  }

  if (surface.typed_sema_diagnostics_hardening_key.empty()) {
    return "typed sema-to-lowering diagnostics hardening key is empty";
  }

  if (!surface.typed_sema_recovery_determinism_consistent) {
    return "typed sema-to-lowering recovery/determinism is inconsistent";
  }

  if (!surface.typed_sema_recovery_determinism_ready) {
    return "typed sema-to-lowering recovery/determinism is not ready";
  }

  if (surface.typed_sema_recovery_determinism_key.empty()) {
    return "typed sema-to-lowering recovery/determinism key is empty";
  }

  if (!surface.typed_sema_conformance_matrix_consistent) {
    return "typed sema-to-lowering conformance matrix is inconsistent";
  }

  if (!surface.typed_sema_conformance_matrix_ready) {
    return "typed sema-to-lowering conformance matrix is not ready";
  }

  if (surface.typed_sema_conformance_matrix_key.empty()) {
    return "typed sema-to-lowering conformance matrix key is empty";
  }

  if (!surface.typed_sema_conformance_corpus_consistent) {
    return "typed sema-to-lowering conformance corpus is inconsistent";
  }

  if (!surface.typed_sema_conformance_corpus_ready) {
    return "typed sema-to-lowering conformance corpus is not ready";
  }

  if (surface.typed_sema_conformance_corpus_key.empty()) {
    return "typed sema-to-lowering conformance corpus key is empty";
  }

  if (!surface.typed_sema_performance_quality_guardrails_consistent) {
    return "typed sema-to-lowering performance/quality guardrails are inconsistent";
  }

  if (!surface.typed_sema_performance_quality_guardrails_ready) {
    return "typed sema-to-lowering performance/quality guardrails are not ready";
  }

  if (surface.typed_sema_performance_quality_guardrails_key.empty()) {
    return "typed sema-to-lowering performance/quality guardrails key is empty";
  }

  if (!surface.typed_sema_cross_lane_integration_consistent) {
    return "typed sema-to-lowering cross-lane integration is inconsistent";
  }

  if (!surface.typed_sema_cross_lane_integration_ready) {
    return "typed sema-to-lowering cross-lane integration is not ready";
  }

  if (surface.typed_sema_cross_lane_integration_key.empty()) {
    return "typed sema-to-lowering cross-lane integration key is empty";
  }

  if (!surface.typed_sema_docs_runbook_sync_consistent) {
    return "typed sema-to-lowering docs/runbook synchronization is inconsistent";
  }

  if (!surface.typed_sema_docs_runbook_sync_ready) {
    return "typed sema-to-lowering docs/runbook synchronization is not ready";
  }

  if (surface.typed_sema_docs_runbook_sync_key.empty()) {
    return "typed sema-to-lowering docs/runbook synchronization key is empty";
  }

  if (const char *failure_reason =
          FindTypedSemaLoweringAdvancedSurfaceFailureReason(surface)) {
    return failure_reason;
  }

  return nullptr;
}

}  // namespace objc3_parse_lowering_failure_reason_readiness_detail
