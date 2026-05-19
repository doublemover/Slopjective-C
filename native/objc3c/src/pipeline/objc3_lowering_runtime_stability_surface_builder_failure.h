#pragma once

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_lowering_runtime_stability_surface_builder_facts.h"

inline void PublishObjc3LoweringRuntimeStabilitySurfaceFailureReason(
    Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    const Objc3LoweringRuntimeStabilityReadinessFacts &facts) {
  if (!surface.lowering_boundary_ready) {
    surface.failure_reason = "lowering boundary is not ready";
  } else if (!surface.runtime_dispatch_contract_consistent) {
    surface.failure_reason = "runtime dispatch contract is inconsistent";
  } else if (!surface.typed_handoff_key_deterministic) {
    surface.failure_reason = "typed handoff key is not deterministic";
  } else if (!surface.typed_core_feature_consistent) {
    surface.failure_reason = "typed core feature is inconsistent";
  } else if (!surface.parse_ready_for_lowering) {
    surface.failure_reason = "parse-lowering readiness is false";
  } else if (!surface.invariant_proofs_ready) {
    surface.failure_reason = "lowering/runtime invariant proofs are not ready";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason =
        "lowering/runtime modular split scaffold is not ready";
  } else if (!facts.typed_case_accounting_consistent) {
    surface.failure_reason =
        "typed core feature case accounting is inconsistent";
  } else if (!facts.typed_expansion_case_accounting_consistent) {
    surface.failure_reason =
        "typed core feature expansion case accounting is inconsistent";
  } else if (!facts.parse_matrix_case_count_ready) {
    surface.failure_reason = "parse conformance matrix case count is not ready";
  } else if (!facts.parse_corpus_case_accounting_consistent) {
    surface.failure_reason =
        "parse conformance corpus case accounting is inconsistent";
  } else if (!facts.parse_guardrails_case_accounting_consistent) {
    surface.failure_reason = "parse guardrails case accounting is inconsistent";
  } else if (!facts.typed_expansion_accounting_consistent) {
    surface.failure_reason =
        "typed core feature expansion accounting is inconsistent";
  } else if (!facts.parse_conformance_accounting_consistent) {
    surface.failure_reason = "parse conformance accounting is inconsistent";
  } else if (!facts.replay_keys_ready) {
    surface.failure_reason = "lowering/runtime replay keys are not ready";
  } else if (!facts.edge_case_compatibility_ready) {
    surface.failure_reason =
        "lowering/runtime edge-case compatibility is not ready";
  } else if (!facts.edge_case_expansion_consistent) {
    surface.failure_reason =
        "lowering/runtime edge-case expansion is inconsistent";
  } else if (!facts.edge_case_robustness_ready) {
    surface.failure_reason =
        "lowering/runtime edge-case robustness is not ready";
  } else if (!facts.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is inconsistent";
  } else if (!facts.diagnostics_hardening_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is not ready";
  } else if (!facts.recovery_determinism_consistent) {
    surface.failure_reason =
        "lowering/runtime recovery determinism is inconsistent";
  } else if (!facts.recovery_determinism_ready) {
    surface.failure_reason =
        "lowering/runtime recovery determinism is not ready";
  } else if (!facts.conformance_matrix_consistent) {
    surface.failure_reason =
        "lowering/runtime conformance matrix is inconsistent";
  } else if (!facts.conformance_matrix_ready) {
    surface.failure_reason =
        "lowering/runtime conformance matrix is not ready";
  } else if (!facts.conformance_corpus_consistent) {
    surface.failure_reason =
        "lowering/runtime conformance corpus is inconsistent";
  } else if (!facts.conformance_corpus_ready) {
    surface.failure_reason =
        "lowering/runtime conformance corpus is not ready";
  } else if (!facts.performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "lowering/runtime performance quality guardrails are inconsistent";
  } else if (!facts.performance_quality_guardrails_ready) {
    surface.failure_reason =
        "lowering/runtime performance quality guardrails are not ready";
  } else if (!facts.cross_lane_integration_consistent) {
    surface.failure_reason =
        "lowering/runtime cross-lane integration is inconsistent";
  } else if (!facts.cross_lane_integration_ready) {
    surface.failure_reason =
        "lowering/runtime cross-lane integration is not ready";
  } else if (!facts.integration_closeout_consistent) {
    surface.failure_reason =
        "lowering/runtime integration closeout is inconsistent";
  } else if (!facts.gate_signoff_ready) {
    surface.failure_reason = "lowering/runtime gate sign-off is not ready";
  } else if (!facts.diagnostics_hardening_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime core feature expansion is not ready";
  } else if (!facts.recovery_determinism_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime recovery determinism expansion is not ready";
  } else if (!facts.conformance_matrix_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime conformance matrix expansion is not ready";
  } else if (!facts.conformance_corpus_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime conformance corpus expansion is not ready";
  } else if (!facts.performance_quality_guardrails_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime performance quality guardrails expansion is not ready";
  } else if (!facts.cross_lane_integration_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime cross-lane integration expansion is not ready";
  } else if (!facts.integration_closeout_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime integration closeout expansion is not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime core feature implementation is not ready";
  }
}
