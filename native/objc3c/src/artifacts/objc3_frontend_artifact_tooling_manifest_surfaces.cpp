#include "artifacts/objc3_frontend_artifact_tooling_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_tooling_source_artifacts.h"
#include "artifacts/reports/frontend_conformance_report_contracts.h"

namespace objc3::artifacts::frontend {

void WriteToolingManifestSurfaces(
    std::ostream &manifest,
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &tooling_diagnostics_migrator_source_inventory_summary,
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &tooling_migration_canonicalization_source_completion_summary,
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
        &tooling_diagnostic_taxonomy_portability_contract_summary,
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary
        &tooling_feature_specific_fixit_synthesis_summary,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &tooling_legacy_canonical_migration_semantics_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &tooling_machine_readable_conformance_report_contract_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &tooling_feature_aware_conformance_report_emission_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &tooling_corpus_sharding_release_evidence_packaging_summary) {
  manifest
      << ",\"objc_tooling_diagnostics_fixit_and_migrator_source_inventory\":"
      << BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
             tooling_diagnostics_migrator_source_inventory_summary)
      << ",\"objc_tooling_migration_and_canonicalization_source_completion\":"
      << BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
             tooling_migration_canonicalization_source_completion_summary)
      << ",\"objc_tooling_diagnostic_taxonomy_and_portability_contract\":"
      << BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
             tooling_diagnostic_taxonomy_portability_contract_summary)
      << ",\"objc_tooling_feature_specific_fixit_synthesis\":"
      << BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
             tooling_feature_specific_fixit_synthesis_summary)
      << ",\"objc_tooling_legacy_canonical_migration_semantics\":"
      << BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
             tooling_legacy_canonical_migration_semantics_summary)
      << ",\"objc_tooling_machine_readable_conformance_report_contract\":"
      << objc3::artifacts::reports::
             BuildToolingMachineReadableConformanceReportContractSummaryJson(
                 tooling_machine_readable_conformance_report_contract_summary)
      << ",\"objc_tooling_feature_aware_conformance_report_emission\":"
      << objc3::artifacts::reports::
             BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
                 tooling_feature_aware_conformance_report_emission_summary)
      << ",\"objc_tooling_corpus_sharding_release_evidence_packaging\":"
      << objc3::artifacts::reports::
             BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
                 tooling_corpus_sharding_release_evidence_packaging_summary);
}

}  // namespace objc3::artifacts::frontend
