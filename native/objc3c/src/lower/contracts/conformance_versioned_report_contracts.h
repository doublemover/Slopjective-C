#pragma once

#include <string>

// Versioned conformance report lowering owns the canonical machine-readable
// sidecar payload that carries runnable/source-only/unsupported claim truth
// from frontend semantic packets into durable lower-owned artifacts.
inline constexpr const char *kObjc3VersionedConformanceReportLoweringContractId =
    "objc3c.versioned.conformance.report.lowering.v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringSemanticContractId =
        "objc3c.canonical.selection.claim.semantics.v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringArtifactSuffix =
        ".objc3-conformance-report.json";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringArtifactSchemaId =
        "objc3c-versioned-conformance-report-v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringSurfacePath =
        "frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringPayloadModel =
        "frontend-truth-packets-lower-into-one-versioned-machine-readable-conformance-sidecar";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringAuthorityModel =
        "runnable-feature-inventory-plus-truth-surface-plus-fail-closed-semantics";
inline constexpr const char
    *kObjc3VersionedConformanceReportKnownUnsupportedModel =
        "unsupported-claims-remain-published-as-known-unsupported-without-runnable-overclaim";
inline constexpr const char
    *kObjc3VersionedConformanceReportSelectionModel =
        "canonical-selection-only-strictness-and-concurrency-claims-remain-fail-closed";
inline constexpr const char
    *kObjc3VersionedConformanceReportCanonicalInterfaceMode =
        "no-standalone-interface-payload-yet";
inline constexpr const char
    *kObjc3VersionedConformanceReportPublicationModel =
        "written-next-to-manifest-when-out-dir-is-present";

std::string Objc3VersionedConformanceReportLoweringContractSummary();
