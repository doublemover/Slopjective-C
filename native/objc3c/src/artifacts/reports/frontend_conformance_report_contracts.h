#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactBundle;

namespace objc3::artifacts::reports {

std::string BuildFrontendCompatibilityStrictnessClaimSemanticsReplayKey(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary);

Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
BuildFrontendCompatibilityStrictnessClaimSemanticsSummary(
    const Objc3CompatibilityStrictnessClaimSemanticsSummary &semantic_boundary);

std::string BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary);

std::string BuildVersionedConformanceReportLoweringReplayKey(
    const Objc3VersionedConformanceReportLoweringSummary &summary);

Objc3VersionedConformanceReportLoweringSummary
BuildVersionedConformanceReportLoweringSummary(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary);

std::string BuildVersionedConformanceReportLoweringSummaryJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary);

std::string BuildToolingMachineReadableConformanceReportContractReplayKey(
    const Objc3ToolingMachineReadableConformanceReportContractSummary &summary);

Objc3ToolingMachineReadableConformanceReportContractSummary
BuildToolingMachineReadableConformanceReportContractSummary(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &migration_summary,
    const Objc3VersionedConformanceReportLoweringSummary &lowering_summary);

std::string BuildToolingMachineReadableConformanceReportContractSummaryJson(
    const Objc3ToolingMachineReadableConformanceReportContractSummary &summary);

std::string BuildToolingFeatureAwareConformanceReportEmissionReplayKey(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary);

Objc3ToolingFeatureAwareConformanceReportEmissionSummary
BuildToolingFeatureAwareConformanceReportEmissionSummary(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &fixit_summary,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &migration_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &report_summary);

std::string BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary);

std::string BuildToolingAdvancedFeatureReportingJson(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary);

std::string BuildToolingCorpusShardingReleaseEvidencePackagingReplayKey(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary);

Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
BuildToolingCorpusShardingReleaseEvidencePackagingSummary(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary);

std::string BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary);

std::string BuildToolingAdvancedFeatureReleaseEvidenceJson(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary);

std::string BuildVersionedConformanceReportArtifactJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &packaging_summary);

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
        &packaging_summary);

}  // namespace objc3::artifacts::reports
