#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"

bool IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering modular split scaffold not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.expansion_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering core feature expansion is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.edge_case_compatibility_ready &&
      scaffold.edge_case_robustness_ready &&
      !scaffold.edge_case_robustness_key.empty() &&
      scaffold.diagnostics_hardening_ready &&
      !scaffold.diagnostics_hardening_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering edge-case compatibility is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.edge_case_expansion_consistent &&
      scaffold.edge_case_robustness_ready &&
      !scaffold.edge_case_robustness_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering edge-case robustness is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.diagnostics_hardening_consistent &&
      scaffold.diagnostics_hardening_ready &&
      !scaffold.diagnostics_hardening_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering diagnostics hardening is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.recovery_determinism_consistent &&
      scaffold.recovery_determinism_ready &&
      !scaffold.recovery_determinism_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering recovery determinism hardening is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorConformanceMatrixReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.conformance_matrix_consistent &&
      scaffold.conformance_matrix_ready &&
      !scaffold.conformance_matrix_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering conformance matrix is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.conformance_corpus_consistent &&
      scaffold.conformance_corpus_ready &&
      !scaffold.conformance_corpus_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering conformance corpus is not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.performance_quality_guardrails_consistent &&
      scaffold.performance_quality_guardrails_ready &&
      !scaffold.performance_quality_guardrails_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering performance quality guardrails are not ready"
               : scaffold.failure_reason;
  return false;
}

bool IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady(
    const Objc3OwnershipAwareLoweringBehaviorScaffold &scaffold,
    std::string &reason) {
  if (scaffold.cross_lane_integration_consistent &&
      scaffold.cross_lane_integration_ready &&
      !scaffold.cross_lane_integration_key.empty()) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "ownership-aware lowering cross-lane integration is not ready"
               : scaffold.failure_reason;
  return false;
}
