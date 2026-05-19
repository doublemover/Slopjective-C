#include "artifacts/reports/frontend_conformance_report_contracts.h"

#include "artifacts/objc3_frontend_conformance_artifacts.h"
#include "artifacts/objc3_frontend_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_truth_artifacts.h"
#include "artifacts/objc3_frontend_runtime_capability_artifacts.h"

namespace objc3::artifacts::reports {

std::string BuildVersionedConformanceReportArtifactJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &packaging_summary) {
  return ::objc3::artifacts::frontend::RenderVersionedConformanceReportArtifactJson(
      summary,
      static_cast<unsigned>(options.language_version),
      ::objc3::artifacts::BuildRunnableFeatureClaimInventoryJson(
          options, pipeline_result),
      ::objc3::artifacts::frontend::BuildFeatureClaimStrictnessTruthSurfaceJson(
          options, pipeline_result),
      BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson(
          semantic_summary),
      ::objc3::artifacts::frontend::BuildRuntimeCapabilityReportJson(summary),
      ::objc3::artifacts::frontend::BuildPublicConformanceReportJson(summary),
      BuildToolingAdvancedFeatureReportingJson(feature_summary),
      BuildToolingAdvancedFeatureReleaseEvidenceJson(packaging_summary));
}

void PopulateObjc3FrontendVersionedConformanceReportOutput(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &packaging_summary) {
  if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)) {
    return;
  }

  bundle.versioned_conformance_report_artifact_json =
      BuildVersionedConformanceReportArtifactJson(
          summary, options, pipeline_result, semantic_summary, feature_summary,
          packaging_summary);
}

}  // namespace objc3::artifacts::reports
