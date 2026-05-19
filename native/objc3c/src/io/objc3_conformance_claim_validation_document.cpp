#include "io/objc3_conformance_release_artifact_document.h"

#include <sstream>

#include "io/objc3_process_internal.h"

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
    const std::string &publication_surface_kind) {
  std::ostringstream out;
  JsonObjectWriter validation(out);
  validation.StringField("contract_id",
                         kObjc3ToolchainConformanceClaimOperationsContractId);
  validation.StringField("schema_id",
                         kObjc3ToolchainConformanceClaimValidationSchemaId);
  validation.StringField("validation_model",
                         kObjc3ToolchainConformanceClaimValidationModel);
  validation.StringField("consumption_model",
                         kObjc3ToolchainConformanceClaimConsumptionModel);
  validation.StringField("format", "json");
  validation.StringField("validated_report_artifact",
                         inputs.report_artifact_path);
  validation.StringField("validated_publication_artifact",
                         inputs.publication_artifact_path);
  validation.StringField("report_schema_id", report_schema_id);
  validation.StringField("report_contract_id", report_contract_id);
  validation.StringField("runtime_capability_contract_id",
                         runtime_capability_contract_id);
  validation.StringField("public_conformance_schema_id",
                         public_conformance_schema_id);
  validation.StringField("advanced_feature_ops_contract_id",
                         kObjc3AdvancedFeatureOpsContractId);
  validation.StringField("advanced_feature_reporting_contract_id",
                         kObjc3AdvancedFeatureReportingContractId);
  validation.StringField("advanced_feature_release_evidence_contract_id",
                         kObjc3AdvancedFeatureReleaseEvidenceContractId);
  validation.StringArrayField("advanced_feature_targeted_profile_ids",
                              advanced_feature_targeted_profile_ids);
  validation.StringField("ci_release_evidence_gate_script_path",
                         ci_release_evidence_gate_script_path);
  validation.StringField("runbook_reference_path", runbook_reference_path);
  validation.StringField("dashboard_schema_path", dashboard_schema_path);
  validation.StringField("selected_profile", selected_profile);
  validation.BoolField("selected_profile_supported", selected_profile_supported);
  validation.StringArrayField("supported_profile_ids", supported_profile_ids);
  validation.StringArrayField("rejected_profile_ids", rejected_profile_ids);
  validation.StringField("effective_language_profile",
                         effective_language_profile);
  validation.BoolField("canonical_literal_rejection_diagnostics_enabled",
                       canonical_literal_rejection_diagnostics_enabled);
  validation.StringField("publication_surface_kind", publication_surface_kind);
  validation.BoolField("ready", true);
  return FinishJsonObject(validation, out);
}
