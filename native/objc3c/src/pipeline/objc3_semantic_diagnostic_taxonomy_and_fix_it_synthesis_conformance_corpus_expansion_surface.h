#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_surface.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-conformance-corpus-expansion:v1:"
      << "conformance-matrix-consistent=" << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance-matrix-ready=" << (surface.conformance_matrix_ready ? "true" : "false")
      << ";parse-conformance-corpus-consistent="
      << (surface.parse_lowering_conformance_corpus_consistent ? "true" : "false")
      << ";conformance-corpus-consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance-corpus-ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";conformance-corpus-key-ready="
      << (!surface.conformance_corpus_key.empty() ? "true" : "false")
      << ";parse-conformance-corpus-case-count="
      << surface.parse_lowering_conformance_corpus_case_count
      << ";parse-conformance-corpus-passed-case-count="
      << surface.parse_lowering_conformance_corpus_passed_case_count
      << ";parse-conformance-corpus-failed-case-count="
      << surface.parse_lowering_conformance_corpus_failed_case_count
      << ";conformance-matrix-key-ready="
      << (!surface.conformance_matrix_key.empty() ? "true" : "false")
      << ";parse-conformance-corpus-key-ready="
      << (!surface.parse_lowering_conformance_corpus_key.empty() ? "true" : "false");
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface &conformance_matrix_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface surface;
  surface.conformance_matrix_consistent = conformance_matrix_surface.conformance_matrix_consistent;
  surface.conformance_matrix_ready = conformance_matrix_surface.conformance_matrix_ready;
  surface.parse_lowering_conformance_corpus_consistent =
      parse_surface.parse_lowering_conformance_corpus_consistent;
  surface.parse_lowering_conformance_corpus_case_count =
      parse_surface.parse_lowering_conformance_corpus_case_count;
  surface.parse_lowering_conformance_corpus_passed_case_count =
      parse_surface.parse_lowering_conformance_corpus_passed_case_count;
  surface.parse_lowering_conformance_corpus_failed_case_count =
      parse_surface.parse_lowering_conformance_corpus_failed_case_count;
  surface.conformance_matrix_key = conformance_matrix_surface.conformance_matrix_key;
  surface.parse_lowering_conformance_corpus_key =
      parse_surface.parse_lowering_conformance_corpus_key;

  const bool conformance_corpus_consistent =
      surface.conformance_matrix_consistent &&
      surface.conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_consistent &&
      surface.parse_lowering_conformance_corpus_case_count ==
          surface.parse_lowering_conformance_corpus_passed_case_count +
              surface.parse_lowering_conformance_corpus_failed_case_count;
  const bool conformance_corpus_ready =
      conformance_corpus_consistent &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_conformance_corpus_failed_case_count == 0 &&
      !surface.conformance_matrix_key.empty() &&
      !surface.parse_lowering_conformance_corpus_key.empty();

  surface.conformance_corpus_consistent = conformance_corpus_consistent;
  surface.conformance_corpus_ready = conformance_corpus_ready;
  surface.conformance_corpus_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionKey(surface);
  surface.conformance_corpus_ready =
      surface.conformance_corpus_ready &&
      !surface.conformance_corpus_key.empty();

  if (surface.conformance_corpus_ready) {
    return surface;
  }

  if (!surface.conformance_matrix_ready) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it conformance matrix is not ready";
  } else if (!surface.conformance_corpus_consistent) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it conformance corpus is inconsistent";
  } else {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it conformance corpus is not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface &surface,
    std::string &reason) {
  if (surface.conformance_corpus_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it conformance corpus is not ready"
               : surface.failure_reason;
  return false;
}
