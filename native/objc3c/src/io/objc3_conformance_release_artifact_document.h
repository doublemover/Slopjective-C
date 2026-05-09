#pragma once

#include <string>
#include <vector>

#include "io/objc3_process.h"

std::string BuildObjc3ConformanceReportPublicationArtifactDocumentJson(
    const Objc3ConformanceReportPublicationArtifactInputs &inputs);

std::string BuildObjc3ConformanceClaimValidationArtifactDocumentJson(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_schema_id,
    const std::string &report_contract_id,
    const std::string &runtime_capability_contract_id,
    const std::string &public_conformance_schema_id,
    const std::vector<std::string> &advanced_feature_targeted_profile_ids,
    const std::string &ci_release_evidence_gate_script_path,
    const std::string &runbook_reference_path,
    const std::string &dashboard_schema_path,
    const std::string &selected_profile,
    bool selected_profile_supported,
    const std::vector<std::string> &supported_profile_ids,
    const std::vector<std::string> &rejected_profile_ids,
    const std::string &effective_language_profile,
    bool canonical_literal_rejection_diagnostics_enabled,
    const std::string &publication_surface_kind);

std::string BuildObjc3ReleaseEvidenceOperationArtifactDocumentJson(
    const Objc3ReleaseEvidenceOperationArtifactInputs &inputs);

std::string BuildObjc3AdvancedFeatureGateArtifactDocumentJson(
    const Objc3AdvancedFeatureGateArtifactInputs &inputs);

std::string BuildObjc3ReleaseCandidateMatrixArtifactDocumentJson(
    const Objc3ReleaseCandidateMatrixArtifactInputs &inputs);
