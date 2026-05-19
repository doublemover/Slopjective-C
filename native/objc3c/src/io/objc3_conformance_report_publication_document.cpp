#include "io/objc3_conformance_release_artifact_document.h"

#include <sstream>

#include "io/objc3_process_internal.h"

std::string BuildObjc3ConformanceReportPublicationArtifactDocumentJson(
    const Objc3ConformanceReportPublicationArtifactInputs &inputs) {
  std::ostringstream out;
  JsonObjectWriter publication(out);
  publication.StringField("contract_id", inputs.contract_id);
  publication.StringField("schema_id", inputs.schema_id);
  publication.StringField("selected_profile", inputs.selected_profile);
  publication.BoolField("selected_profile_supported",
                        inputs.selected_profile_supported);
  publication.StringArrayField("supported_profile_ids",
                               inputs.supported_profile_ids);
  publication.StringArrayField("rejected_profile_ids",
                               inputs.rejected_profile_ids);
  publication.StringField("effective_language_profile",
                          inputs.effective_language_profile);
  publication.BoolField("canonical_literal_rejection_diagnostics_enabled",
                        inputs.canonical_literal_rejection_diagnostics_enabled);
  publication.StringField("publication_model", inputs.publication_model);
  publication.StringField("publication_surface_kind",
                          inputs.publication_surface_kind);
  publication.StringField("fail_closed_diagnostic_model",
                          inputs.fail_closed_diagnostic_model);
  publication.StringField("lowered_report_contract_id",
                          inputs.lowered_report_contract_id);
  publication.StringField("runtime_capability_contract_id",
                          inputs.runtime_capability_contract_id);
  publication.StringField("public_conformance_schema_id",
                          inputs.public_conformance_schema_id);
  publication.StringField("advanced_feature_ops_contract_id",
                          inputs.advanced_feature_ops_contract_id);
  publication.StringField("advanced_feature_reporting_contract_id",
                          inputs.advanced_feature_reporting_contract_id);
  publication.StringField(
      "advanced_feature_release_evidence_contract_id",
      inputs.advanced_feature_release_evidence_contract_id);
  publication.StringArrayField("advanced_feature_targeted_profile_ids",
                               inputs.advanced_feature_targeted_profile_ids);
  publication.StringField("ci_release_evidence_gate_script_path",
                          inputs.ci_release_evidence_gate_script_path);
  publication.StringField("runbook_reference_path",
                          inputs.runbook_reference_path);
  publication.StringField("dashboard_schema_path",
                          inputs.dashboard_schema_path);
  publication.StringField("report_artifact",
                          inputs.report_artifact_relative_path);
  publication.BoolField("ready", true);
  return FinishJsonObject(publication, out);
}
