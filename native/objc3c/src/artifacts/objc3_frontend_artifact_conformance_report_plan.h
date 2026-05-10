#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

struct Objc3FrontendArtifactConformanceReportPlan {
  Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
      frontend_compatibility_strictness_claim_semantics;
  Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
      tooling_legacy_canonical_migration_semantics_summary;
  Objc3VersionedConformanceReportLoweringSummary
      versioned_conformance_report_lowering;
  Objc3ToolingMachineReadableConformanceReportContractSummary
      tooling_machine_readable_conformance_report_contract_summary;
  Objc3ToolingFeatureAwareConformanceReportEmissionSummary
      tooling_feature_aware_conformance_report_emission_summary;
  Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
      tooling_corpus_sharding_release_evidence_packaging_summary;
  Objc3FrontendArtifactPostPipelineFailure post_pipeline_failure;
};

Objc3FrontendArtifactConformanceReportPlan
BuildObjc3FrontendArtifactConformanceReportPlan(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary
        &tooling_feature_specific_fixit_synthesis_summary);

void WriteObjc3FrontendArtifactConformanceReportManifestSurfaces(
    std::ostream &manifest,
    const Objc3FrontendArtifactConformanceReportPlan &plan);

}  // namespace objc3::artifacts::frontend
