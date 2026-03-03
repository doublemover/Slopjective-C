#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-performance-quality-guardrails:v1:"
      << "conformance-corpus-consistent=" << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance-corpus-ready=" << (surface.conformance_corpus_ready ? "true" : "false")
      << ";parse-performance-quality-guardrails-consistent="
      << (surface.parse_lowering_performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance-quality-guardrails-consistent="
      << (surface.performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance-quality-guardrails-ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";performance-quality-guardrails-key-ready="
      << (!surface.performance_quality_guardrails_key.empty() ? "true" : "false")
      << ";parse-performance-quality-guardrails-case-count="
      << surface.parse_lowering_performance_quality_guardrails_case_count
      << ";parse-performance-quality-guardrails-passed-case-count="
      << surface.parse_lowering_performance_quality_guardrails_passed_case_count
      << ";parse-performance-quality-guardrails-failed-case-count="
      << surface.parse_lowering_performance_quality_guardrails_failed_case_count
      << ";conformance-corpus-key-ready="
      << (!surface.conformance_corpus_key.empty() ? "true" : "false")
      << ";parse-performance-quality-guardrails-key-ready="
      << (!surface.parse_lowering_performance_quality_guardrails_key.empty() ? "true" : "false");
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface
        &conformance_corpus_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface surface;
  surface.conformance_corpus_consistent = conformance_corpus_surface.conformance_corpus_consistent;
  surface.conformance_corpus_ready = conformance_corpus_surface.conformance_corpus_ready;
  surface.parse_lowering_performance_quality_guardrails_consistent =
      parse_surface.parse_lowering_performance_quality_guardrails_consistent;
  surface.parse_lowering_performance_quality_guardrails_case_count =
      parse_surface.parse_lowering_performance_quality_guardrails_case_count;
  surface.parse_lowering_performance_quality_guardrails_passed_case_count =
      parse_surface.parse_lowering_performance_quality_guardrails_passed_case_count;
  surface.parse_lowering_performance_quality_guardrails_failed_case_count =
      parse_surface.parse_lowering_performance_quality_guardrails_failed_case_count;
  surface.conformance_corpus_key = conformance_corpus_surface.conformance_corpus_key;
  surface.parse_lowering_performance_quality_guardrails_key =
      parse_surface.parse_lowering_performance_quality_guardrails_key;

  const bool performance_quality_guardrails_consistent =
      surface.conformance_corpus_consistent &&
      surface.conformance_corpus_ready &&
      surface.parse_lowering_performance_quality_guardrails_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic &&
      surface.parse_lowering_performance_quality_guardrails_case_count ==
          surface.parse_lowering_performance_quality_guardrails_passed_case_count +
              surface.parse_lowering_performance_quality_guardrails_failed_case_count;
  const bool performance_quality_guardrails_ready =
      performance_quality_guardrails_consistent &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_failed_case_count == 0 &&
      !surface.conformance_corpus_key.empty() &&
      !surface.parse_lowering_performance_quality_guardrails_key.empty();

  surface.performance_quality_guardrails_consistent =
      performance_quality_guardrails_consistent;
  surface.performance_quality_guardrails_ready = performance_quality_guardrails_ready;
  surface.performance_quality_guardrails_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsKey(
          surface);
  surface.performance_quality_guardrails_ready =
      surface.performance_quality_guardrails_ready &&
      !surface.performance_quality_guardrails_key.empty();

  if (surface.performance_quality_guardrails_ready) {
    return surface;
  }

  if (!surface.conformance_corpus_ready) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it conformance corpus is not ready";
  } else if (!surface.performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it performance quality guardrails are inconsistent";
  } else {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it performance quality guardrails are not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface &surface,
    std::string &reason) {
  if (surface.performance_quality_guardrails_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it performance quality guardrails are not ready"
               : surface.failure_reason;
  return false;
}
