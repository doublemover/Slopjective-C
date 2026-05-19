#include "io/objc3_conformance_claim_validation_inputs.h"
#include "io/objc3_conformance_release_artifact_document.h"

bool TryBuildObjc3ConformanceClaimValidationArtifact(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  Objc3ConformanceClaimValidationResolvedInputs resolved;
  if (!TryResolveObjc3ConformanceClaimValidationInputs(
          inputs, report_json, publication_json, resolved, error)) {
    return false;
  }

  artifact_json = BuildObjc3ConformanceClaimValidationArtifactDocumentJson(
      inputs, resolved.report_schema_id, resolved.report_contract_id,
      resolved.runtime_capability_contract_id,
      resolved.public_conformance_schema_id,
      resolved.advanced_feature_targeted_profile_ids,
      resolved.ci_release_evidence_gate_script_path,
      resolved.runbook_reference_path, resolved.dashboard_schema_path,
      resolved.selected_profile, resolved.selected_profile_supported,
      resolved.supported_profile_ids, resolved.rejected_profile_ids,
      resolved.effective_language_profile,
      resolved.canonical_literal_rejection_diagnostics_enabled,
      resolved.publication_surface_kind);
  return true;
}
