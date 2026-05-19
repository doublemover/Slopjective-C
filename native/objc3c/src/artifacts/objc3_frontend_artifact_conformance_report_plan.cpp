#include "artifacts/objc3_frontend_artifact_conformance_report_plan.h"

#include <ostream>

#include "artifacts/objc3_frontend_runtime_capability_artifacts.h"
#include "artifacts/objc3_frontend_tooling_source_artifacts.h"
#include "artifacts/reports/frontend_conformance_report_contracts.h"

namespace objc3::artifacts::frontend {

Objc3FrontendArtifactConformanceReportPlan
BuildObjc3FrontendArtifactConformanceReportPlan(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary
        &tooling_feature_specific_fixit_synthesis_summary) {
  Objc3FrontendArtifactConformanceReportPlan plan;

  plan.frontend_compatibility_strictness_claim_semantics =
      objc3::artifacts::reports::
          BuildFrontendCompatibilityStrictnessClaimSemanticsSummary(
              pipeline_result.sema_parity_surface
                  .compatibility_strictness_claim_semantics_summary);
  plan.tooling_legacy_canonical_migration_semantics_summary =
      BuildToolingLegacyCanonicalMigrationSemanticsSummary(
          plan.frontend_compatibility_strictness_claim_semantics,
          tooling_feature_specific_fixit_synthesis_summary);
  plan.versioned_conformance_report_lowering =
      objc3::artifacts::reports::BuildVersionedConformanceReportLoweringSummary(
          options, pipeline_result,
          plan.frontend_compatibility_strictness_claim_semantics);
  plan.tooling_machine_readable_conformance_report_contract_summary =
      objc3::artifacts::reports::
          BuildToolingMachineReadableConformanceReportContractSummary(
              plan.tooling_legacy_canonical_migration_semantics_summary,
              plan.versioned_conformance_report_lowering);
  plan.tooling_feature_aware_conformance_report_emission_summary =
      objc3::artifacts::reports::
          BuildToolingFeatureAwareConformanceReportEmissionSummary(
              tooling_feature_specific_fixit_synthesis_summary,
              plan.tooling_legacy_canonical_migration_semantics_summary,
              plan.tooling_machine_readable_conformance_report_contract_summary);
  plan.tooling_corpus_sharding_release_evidence_packaging_summary =
      objc3::artifacts::reports::
          BuildToolingCorpusShardingReleaseEvidencePackagingSummary(
              plan.tooling_feature_aware_conformance_report_emission_summary);

  if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(
          plan.versioned_conformance_report_lowering)) {
    RecordObjc3FrontendArtifactPostPipelineFailure(
        plan.post_pipeline_failure, "O3L300",
        "LLVM IR emission failed: incomplete versioned conformance-report lowering summary");
  }

  return plan;
}

void WriteObjc3FrontendArtifactConformanceReportManifestSurfaces(
    std::ostream &manifest,
    const Objc3FrontendArtifactConformanceReportPlan &plan) {
  manifest
      // semantic freeze anchor: sema publishes the fail-closed
      // legality boundary that classifies live canonical selections,
      // source-only claim downgrades, and strictness/macro claim
      // rejections before lowering and conformance gates consume them.
      << ",\"objc_canonical_selection_claim_semantics\":"
      << objc3::artifacts::reports::
             BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson(
                 plan.frontend_compatibility_strictness_claim_semantics)
      << ",\"objc_tooling_legacy_canonical_migration_semantics\":"
      << BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
             plan.tooling_legacy_canonical_migration_semantics_summary)
      << ",\"objc_tooling_machine_readable_conformance_report_contract\":"
      << objc3::artifacts::reports::
             BuildToolingMachineReadableConformanceReportContractSummaryJson(
                 plan.tooling_machine_readable_conformance_report_contract_summary)
      << ",\"objc_tooling_feature_aware_conformance_report_emission\":"
      << objc3::artifacts::reports::
             BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
                 plan.tooling_feature_aware_conformance_report_emission_summary)
      << ",\"objc_tooling_corpus_sharding_release_evidence_packaging\":"
      << objc3::artifacts::reports::
             BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
                 plan.tooling_corpus_sharding_release_evidence_packaging_summary)
      // lowering freeze anchor: lane-C lowers the existing
      // runnable/source-only/unsupported truth packets into one emitted
      // machine-readable conformance sidecar instead of reconstructing
      // capability claims from docs or release evidence later.
      << ",\"objc_versioned_conformance_report_lowering_contract\":"
      << objc3::artifacts::reports::
             BuildVersionedConformanceReportLoweringSummaryJson(
                 plan.versioned_conformance_report_lowering)
      // runtime capability reporting anchor: lane-C must
      // publish the truthful machine-readable runtime/public capability
      // payload inside the semantic surface so later driver publication
      // and release tooling consume one canonical schema.
      << ",\"objc_runtime_capability_report\":"
      << BuildRuntimeCapabilityReportJson(
             plan.versioned_conformance_report_lowering);
}

}  // namespace objc3::artifacts::frontend
