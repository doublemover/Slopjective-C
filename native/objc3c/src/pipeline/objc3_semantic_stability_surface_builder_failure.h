#pragma once

#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface_keys.h"
#include "pipeline/objc3_semantic_stability_surface_builder_facts.h"

inline void PublishObjc3SemanticStabilitySurfaceFailureReason(
    Objc3SemanticStabilityCoreFeatureImplementationSurface &surface,
    const Objc3SemanticStabilityReadinessFacts &facts) {
  if (!surface.semantic_handoff_deterministic) {
    surface.failure_reason = "semantic handoff is not deterministic";
  } else if (!surface.spec_delta_closed) {
    surface.failure_reason = "semantic stability spec delta is not closed";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason =
        "semantic stability modular split scaffold is not ready";
  } else if (!facts.typed_parse_core_feature_consistent) {
    surface.failure_reason =
        "typed/parse core feature consistency is incomplete";
  } else if (!facts.parse_conformance_consistent) {
    surface.failure_reason = "parse conformance consistency is incomplete";
  } else if (!facts.typed_core_feature_expansion_accounting_consistent) {
    surface.failure_reason =
        "typed core feature expansion case accounting is inconsistent";
  } else if (!facts.parse_conformance_accounting_consistent) {
    surface.failure_reason = "parse guardrails case accounting is inconsistent";
  } else if (!facts.replay_keys_ready) {
    surface.failure_reason = "typed/parse replay keys are not ready";
  } else if (!facts.edge_case_compatibility_ready) {
    surface.failure_reason =
        "semantic stability edge-case compatibility is not ready";
  } else if (!facts.edge_case_expansion_consistent) {
    surface.failure_reason =
        "semantic stability edge-case expansion is inconsistent";
  } else if (!facts.edge_case_robustness_ready) {
    surface.failure_reason =
        "semantic stability edge-case robustness is not ready";
  } else if (!facts.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "semantic stability diagnostics hardening is inconsistent";
  } else if (!facts.diagnostics_hardening_ready) {
    surface.failure_reason =
        "semantic stability diagnostics hardening is not ready";
  } else if (!facts.recovery_determinism_consistent) {
    surface.failure_reason =
        "semantic stability recovery determinism is inconsistent";
  } else if (!facts.recovery_determinism_ready) {
    surface.failure_reason =
        "semantic stability recovery determinism is not ready";
  } else if (!facts.conformance_matrix_consistent) {
    surface.failure_reason =
        "semantic stability conformance matrix is inconsistent";
  } else if (!facts.conformance_matrix_ready) {
    surface.failure_reason =
        "semantic stability conformance matrix is not ready";
  } else if (!facts.conformance_corpus_consistent) {
    surface.failure_reason =
        "semantic stability conformance corpus is inconsistent";
  } else if (!facts.conformance_corpus_ready) {
    surface.failure_reason =
        "semantic stability conformance corpus is not ready";
  } else if (!facts.performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "semantic stability performance quality guardrails are inconsistent";
  } else if (!facts.performance_quality_guardrails_ready) {
    surface.failure_reason =
        "semantic stability performance quality guardrails are not ready";
  } else if (!facts.integration_closeout_consistent) {
    surface.failure_reason =
        "semantic stability integration closeout is inconsistent";
  } else if (!facts.gate_signoff_ready) {
    surface.failure_reason = "semantic stability gate sign-off is not ready";
  } else if (!facts.diagnostics_hardening_expansion_ready) {
    surface.failure_reason =
        "semantic stability core feature expansion is not ready";
  } else if (!facts.recovery_determinism_expansion_ready) {
    surface.failure_reason =
        "semantic stability recovery determinism expansion is not ready";
  } else if (!facts.conformance_matrix_expansion_ready) {
    surface.failure_reason =
        "semantic stability conformance matrix expansion is not ready";
  } else if (!facts.conformance_corpus_expansion_ready) {
    surface.failure_reason =
        "semantic stability conformance corpus expansion is not ready";
  } else if (!facts.performance_quality_guardrails_expansion_ready) {
    surface.failure_reason =
        "semantic stability performance quality guardrails expansion is not ready";
  } else if (!facts.integration_closeout_expansion_ready) {
    surface.failure_reason =
        "semantic stability integration closeout expansion is not ready";
  } else {
    surface.failure_reason =
        "semantic stability core feature implementation is not ready";
  }
}
