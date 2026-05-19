#pragma once

#include <iosfwd>

struct Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary;
struct Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary;
struct Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary;
struct Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary;
struct Objc3ToolingFeatureAwareConformanceReportEmissionSummary;
struct Objc3ToolingFeatureSpecificFixitSynthesisSummary;
struct Objc3ToolingLegacyCanonicalMigrationSemanticsSummary;
struct Objc3ToolingMachineReadableConformanceReportContractSummary;

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
        &tooling_corpus_sharding_release_evidence_packaging_summary);

}  // namespace objc3::artifacts::frontend
