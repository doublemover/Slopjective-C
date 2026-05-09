#pragma once

#include <string>

// Tooling conformance reporting owns machine-readable tooling overlays,
// feature-aware report emission, and corpus/release-evidence packaging that
// extend the lowered conformance report without creating a parallel sidecar.
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportContractId =
        "objc3c.tooling.machine.readable.conformance.report.contract.v1";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportDependencyContractId =
        "objc3c.tooling.legacy.canonical.migration.semantics.v1";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_machine_readable_conformance_report_contract";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportPayloadModel =
        "tooling-advanced-feature-truth-reuses-the-existing-versioned-conformance-sidecar-and-runtime-capability-publication-path";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportAuthorityModel =
        "tooling-machine-readable-reporting-remains-bounded-to-the-lowered-versioned-conformance-sidecar-and-live-migration-semantics";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionDependencyContractId =
        "objc3c.tooling.machine.readable.conformance.report.contract.v1";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_feature_aware_conformance_report_emission";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionPayloadModel =
        "versioned-conformance-report-now-embeds-tooling-feature-aware-migration-and-fixit-state-without-introducing-a-second-report-sidecar";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionAuthorityModel =
        "tooling-feature-aware-reporting-remains-bounded-to-live-fixit-migration-and-machine-readable-report-contract-surfaces";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingContractId =
        "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingDependencyContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_corpus_sharding_release_evidence_packaging";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingPayloadModel =
        "versioned-conformance-report-now-embeds-tooling-corpus-shard-and-release-evidence-packaging-without-introducing-a-parallel-report-format";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingAuthorityModel =
        "tooling-release-evidence-packaging-remains-bounded-to-emitted-report-payloads-checklist-refs-and-stable-conformance-bucket-manifests";

std::string Objc3ToolingMachineReadableConformanceReportContractLoweringSummary();
std::string Objc3ToolingFeatureAwareConformanceReportEmissionLoweringSummary();
std::string Objc3ToolingCorpusShardingReleaseEvidencePackagingLoweringSummary();
