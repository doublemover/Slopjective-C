#include "artifacts/reports/frontend_conformance_report_contracts.h"

#include <sstream>

#include "artifacts/reports/frontend_conformance_report_contracts_json_helpers.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::reports {

using objc3::artifacts::reports::detail::BuildStringArrayJson;
using objc3::io::EscapeJsonString;

std::string BuildToolingMachineReadableConformanceReportContractReplayKey(
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";migration=" << (summary.migration_semantics_ready ? "true" : "false")
      << ";lowering=" << (summary.lowering_contract_ready ? "true" : "false")
      << ";runtime-capability="
      << (summary.runtime_capability_surface_published ? "true" : "false")
      << ";ready="
      << (summary.ready_for_runtime_publication ? "true" : "false");
  return out.str();
}

Objc3ToolingMachineReadableConformanceReportContractSummary
BuildToolingMachineReadableConformanceReportContractSummary(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &migration_summary,
    const Objc3VersionedConformanceReportLoweringSummary &lowering_summary) {
  Objc3ToolingMachineReadableConformanceReportContractSummary summary;
  summary.effective_language_profile =
      lowering_summary.effective_language_profile;
  summary.migration_semantics_ready =
      migration_summary.ready_for_lowering_and_runtime;
  summary.lowering_contract_ready =
      IsReadyObjc3VersionedConformanceReportLoweringSummary(lowering_summary);
  summary.runtime_capability_surface_published =
      summary.lowering_contract_ready &&
      lowering_summary.ready_for_runtime_conformance_publication;
  summary.deterministic_handoff =
      summary.migration_semantics_ready && summary.lowering_contract_ready &&
      summary.runtime_capability_surface_published &&
      lowering_summary.artifact_suffix ==
          kObjc3VersionedConformanceReportLoweringArtifactSuffix &&
      lowering_summary.artifact_schema_id ==
          kObjc3VersionedConformanceReportLoweringArtifactSchemaId;
  summary.ready_for_runtime_publication = summary.deterministic_handoff;
  if (!summary.migration_semantics_ready) {
    summary.failure_reason =
        "tooling legacy/canonical migration semantics prerequisite is not ready";
  } else if (!summary.lowering_contract_ready) {
    summary.failure_reason =
        "versioned conformance-report lowering prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingMachineReadableConformanceReportContractReplayKey(summary);
  return summary;
}

std::string BuildToolingMachineReadableConformanceReportContractSummaryJson(
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(summary.lowering_contract_id)
      << "\",\"runtime_capability_contract_id\":\""
      << EscapeJsonString(summary.runtime_capability_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\"" << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"artifact_suffix\":\"" << EscapeJsonString(summary.artifact_suffix)
      << "\",\"artifact_schema_id\":\""
      << EscapeJsonString(summary.artifact_schema_id)
      << "\",\"runtime_capability_schema_id\":\""
      << EscapeJsonString(summary.runtime_capability_schema_id)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"migration_semantics_ready\":"
      << (summary.migration_semantics_ready ? "true" : "false")
      << ",\"lowering_contract_ready\":"
      << (summary.lowering_contract_ready ? "true" : "false")
      << ",\"runtime_capability_surface_published\":"
      << (summary.runtime_capability_surface_published ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_runtime_publication\":"
      << (summary.ready_for_runtime_publication ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingFeatureAwareConformanceReportEmissionReplayKey(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";families=" << summary.fixit_family_count
      << ";canonical-literal-rejection-sites="
      << summary.current_run_canonical_literal_rejection_sites
      << ";ready="
      << (summary.ready_for_runtime_publication ? "true" : "false");
  return out.str();
}

Objc3ToolingFeatureAwareConformanceReportEmissionSummary
BuildToolingFeatureAwareConformanceReportEmissionSummary(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &fixit_summary,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &migration_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &report_summary) {
  Objc3ToolingFeatureAwareConformanceReportEmissionSummary summary;
  summary.effective_language_profile =
      migration_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      migration_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.fixit_family_ids = fixit_summary.fixit_family_ids;
  summary.fixit_family_count = fixit_summary.fixit_family_count;
  summary.current_run_canonical_literal_rejection_sites =
      migration_summary.current_run_legacy_literal_sites;
  summary.canonical_mode_rejection_code =
      migration_summary.canonical_mode_rejection_code;
  summary.report_payload_emitted =
      report_summary.ready_for_runtime_publication;
  summary.deterministic_handoff =
      fixit_summary.ready_for_lowering_and_runtime &&
      migration_summary.ready_for_lowering_and_runtime &&
      report_summary.ready_for_runtime_publication &&
      summary.fixit_family_count == summary.fixit_family_ids.size();
  summary.ready_for_runtime_publication = summary.deterministic_handoff;
  if (!report_summary.ready_for_runtime_publication) {
    summary.failure_reason =
        "tooling machine-readable conformance/report contract prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingFeatureAwareConformanceReportEmissionReplayKey(summary);
  return summary;
}

std::string BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"machine_readable_report_contract_id\":\""
      << EscapeJsonString(summary.machine_readable_report_contract_id)
      << "\",\"fixit_contract_id\":\""
      << EscapeJsonString(summary.fixit_contract_id)
      << "\",\"migration_semantics_contract_id\":\""
      << EscapeJsonString(summary.migration_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\"" << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"fixit_family_ids\":"
      << BuildStringArrayJson(summary.fixit_family_ids)
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"current_run_canonical_literal_rejection_sites\":"
      << summary.current_run_canonical_literal_rejection_sites
      << ",\"canonical_mode_rejection_code\":\""
      << EscapeJsonString(summary.canonical_mode_rejection_code)
      << "\",\"report_payload_emitted\":"
      << (summary.report_payload_emitted ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_runtime_publication\":"
      << (summary.ready_for_runtime_publication ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingAdvancedFeatureReportingJson(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  return BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(summary);
}

std::string BuildToolingCorpusShardingReleaseEvidencePackagingReplayKey(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";profiles=" << summary.targeted_profile_count
      << ";corpus-shards=" << summary.corpus_shard_count
      << ";release-artifacts=" << summary.release_evidence_artifact_count
      << ";ready="
      << (summary.ready_for_release_evidence_packaging ? "true" : "false");
  return out.str();
}

Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
BuildToolingCorpusShardingReleaseEvidencePackagingSummary(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary) {
  Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary summary;
  summary.effective_language_profile =
      feature_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      feature_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.feature_report_payload_emitted =
      feature_summary.report_payload_emitted;
  summary.deterministic_handoff =
      IsReadyObjc3ToolingFeatureAwareConformanceReportEmissionSummary(
          feature_summary) &&
      summary.targeted_profile_count == summary.targeted_profile_ids.size() &&
      summary.corpus_shard_count == summary.corpus_shard_ids.size() &&
      summary.corpus_shard_count ==
          summary.corpus_shard_manifest_paths.size() &&
      summary.release_evidence_artifact_count ==
          summary.release_evidence_artifact_ids.size();
  summary.ready_for_release_evidence_packaging =
      summary.deterministic_handoff;
  if (!summary.feature_report_payload_emitted) {
    summary.failure_reason =
        "tooling feature-aware conformance report emission prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingCorpusShardingReleaseEvidencePackagingReplayKey(summary);
  return summary;
}

std::string BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"feature_aware_report_contract_id\":\""
      << EscapeJsonString(summary.feature_aware_report_contract_id)
      << "\",\"machine_readable_report_contract_id\":\""
      << EscapeJsonString(summary.machine_readable_report_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\"" << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"targeted_profile_ids\":"
      << BuildStringArrayJson(summary.targeted_profile_ids)
      << ",\"targeted_profile_count\":" << summary.targeted_profile_count
      << ",\"corpus_shard_ids\":"
      << BuildStringArrayJson(summary.corpus_shard_ids)
      << ",\"corpus_shard_manifest_paths\":"
      << BuildStringArrayJson(summary.corpus_shard_manifest_paths)
      << ",\"corpus_shard_count\":" << summary.corpus_shard_count
      << ",\"release_evidence_artifact_ids\":"
      << BuildStringArrayJson(summary.release_evidence_artifact_ids)
      << ",\"release_evidence_artifact_count\":"
      << summary.release_evidence_artifact_count
      << ",\"release_evidence_checklist_path\":\""
      << EscapeJsonString(summary.release_evidence_checklist_path)
      << "\",\"release_evidence_schema_path\":\""
      << EscapeJsonString(summary.release_evidence_schema_path)
      << "\",\"report_artifact_suffix\":\""
      << EscapeJsonString(summary.report_artifact_suffix)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"feature_report_payload_emitted\":"
      << (summary.feature_report_payload_emitted ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_release_evidence_packaging\":"
      << (summary.ready_for_release_evidence_packaging ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingAdvancedFeatureReleaseEvidenceJson(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  return BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(summary);
}

}  // namespace objc3::artifacts::reports
