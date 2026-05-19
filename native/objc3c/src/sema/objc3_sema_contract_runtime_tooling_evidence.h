#pragma once

#include <cstddef>

struct Objc3RetainReleaseOperationSummary {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsSummary {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitSummary {
  // freezes the advanced diagnostics taxonomy/portability contract on
  // top of this deterministic ARC/fix-it baseline rather than inventing a
  // parallel semantic diagnostics summary.
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3ToolingDiagnosticTaxonomyPortabilityContractId =
    "objc3c.tooling.diagnostic.taxonomy.portability.contract.v1";
inline constexpr const char *kObjc3ToolingDiagnosticTaxonomyPortabilityDiagnosticNamespace =
    "O3S";
inline constexpr const char *kObjc3ToolingFeatureSpecificFixitSynthesisContractId =
    "objc3c.tooling.feature.specific.fixit.synthesis.v1";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationSemanticsContractId =
        "objc3c.tooling.legacy.canonical.migration.semantics.v1";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode = "O3S216";
