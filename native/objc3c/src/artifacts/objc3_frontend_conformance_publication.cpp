#include "artifacts/objc3_frontend_conformance_publication.h"

#include <sstream>

#include "io/json/json_writer.h"
#include "token/objc3_token_contract.h"

namespace objc3::artifacts::frontend {

std::string RenderVersionedConformanceReportPublicationJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    const VersionedConformancePublicationInputs &inputs) {
  std::ostringstream out;
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("schema_id", summary.artifact_schema_id);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("semantic_contract_id", summary.semantic_contract_id);
  object.StringField("runnable_feature_claim_inventory_contract_id",
                     summary.runnable_feature_claim_inventory_contract_id);
  object.StringField("feature_claim_truth_surface_contract_id",
                     summary.feature_claim_truth_surface_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("payload_model", summary.payload_model);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("known_unsupported_model", summary.known_unsupported_model);
  object.StringField("selection_model", summary.selection_model);
  object.StringField("canonical_interface_mode",
                     summary.canonical_interface_mode);
  object.StringField("publication_model", summary.publication_model);
  object.StringField("language_mode", kObjc3RunnableFeatureClaimModeName);
  object.UnsignedField("language_version", inputs.language_version);
  object.StringField("effective_language_profile",
                     summary.effective_language_profile);
  object.BoolField("canonical_literal_rejection_diagnostics_enabled",
                   summary.canonical_literal_rejection_diagnostics_enabled);
  object.StringArrayField("runnable_feature_claim_ids",
                          summary.runnable_feature_claim_ids);
  object.StringArrayField("source_only_feature_claim_ids",
                          summary.source_only_feature_claim_ids);
  object.StringArrayField("unsupported_feature_claim_ids",
                          summary.unsupported_feature_claim_ids);
  object.StringArrayField("suppressed_macro_claim_ids",
                          summary.suppressed_macro_claim_ids);
  object.SizeField("live_unsupported_feature_family_count",
                   summary.live_unsupported_feature_family_count);
  object.SizeField("live_unsupported_feature_site_count",
                   summary.live_unsupported_feature_site_count);
  object.SizeField("live_unsupported_feature_diagnostic_count",
                   summary.live_unsupported_feature_diagnostic_count);
  object.BoolField("ready",
                   IsReadyObjc3VersionedConformanceReportLoweringSummary(
                       summary));
  object.RawJsonField("runnable_feature_claim_inventory",
                      inputs.runnable_feature_claim_inventory_json);
  object.RawJsonField("feature_claim_truth_surface",
                      inputs.feature_claim_truth_surface_json);
  object.RawJsonField("canonical_selection_claim_semantics",
                      inputs.canonical_selection_claim_semantics_json);
  object.RawJsonField("runtime_capability_report",
                      inputs.runtime_capability_report_json);
  object.RawJsonField("public_conformance_report",
                      inputs.public_conformance_report_json);
  object.RawJsonField("advanced_feature_reporting",
                      inputs.advanced_feature_reporting_json);
  object.RawJsonField("advanced_feature_release_evidence",
                      inputs.advanced_feature_release_evidence_json);
  object.StringField("runnable_feature_claim_inventory_replay_key",
                     summary.runnable_feature_claim_inventory_replay_key);
  object.StringField("feature_claim_truth_surface_replay_key",
                     summary.feature_claim_truth_surface_replay_key);
  object.StringField("semantic_boundary_replay_key",
                     summary.semantic_boundary_replay_key);
  object.StringField("replay_key", summary.replay_key);
  object.End();
  out << "\n";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
