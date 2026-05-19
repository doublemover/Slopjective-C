#pragma once

#include <string>
#include <vector>

#include "io/objc3_process.h"

struct Objc3ConformanceClaimValidationResolvedInputs {
  std::string report_schema_id;
  std::string report_contract_id;
  std::string effective_language_profile;
  std::string runtime_capability_contract_id;
  std::string public_conformance_schema_id;
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::string selected_profile;
  bool selected_profile_supported = false;
  std::vector<std::string> supported_profile_ids;
  std::vector<std::string> rejected_profile_ids;
  std::string publication_surface_kind;
  std::vector<std::string> advanced_feature_targeted_profile_ids;
  std::string ci_release_evidence_gate_script_path;
  std::string runbook_reference_path;
  std::string dashboard_schema_path;
};

bool TryResolveObjc3ConformanceClaimValidationInputs(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    Objc3ConformanceClaimValidationResolvedInputs &resolved,
    std::string &error);
