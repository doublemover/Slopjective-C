#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

#include "parse/objc3_diagnostic_grammar_hooks_core_feature.h"
#include "parse/objc3_diagnostic_source_precision_scaffold.h"

struct Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface {
  std::size_t parser_diagnostic_count = 0;
  std::size_t parser_snapshot_diagnostic_count = 0;
  std::size_t grammar_hook_code_count = 0;
  std::size_t unique_diagnostic_code_count = 0;
  std::uint64_t diagnostic_code_fingerprint = 1469598103934665603ull;
  std::uint64_t source_precision_fingerprint = 1469598103934665603ull;
  bool core_feature_ready = false;
  bool accounting_consistent = false;
  bool source_precision_coverage_consistent = false;
  bool replay_keys_ready = false;
  bool core_feature_expansion_ready = false;
  std::string expansion_key;
  std::string failure_reason;
};

inline std::string BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionKey(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface,
    const std::string &source_precision_scaffold_key,
    const std::string &core_feature_key) {
  std::ostringstream key;
  key << "parser-diagnostic-grammar-hooks-core-feature-expansion:v1:"
      << "parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";parser_snapshot_diagnostic_count=" << surface.parser_snapshot_diagnostic_count
      << ";grammar_hook_code_count=" << surface.grammar_hook_code_count
      << ";unique_diagnostic_code_count=" << surface.unique_diagnostic_code_count
      << ";diagnostic_code_fingerprint=" << surface.diagnostic_code_fingerprint
      << ";source_precision_fingerprint=" << surface.source_precision_fingerprint
      << ";core_feature_ready=" << (surface.core_feature_ready ? "true" : "false")
      << ";accounting_consistent=" << (surface.accounting_consistent ? "true" : "false")
      << ";source_precision_coverage_consistent="
      << (surface.source_precision_coverage_consistent ? "true" : "false")
      << ";replay_keys_ready=" << (surface.replay_keys_ready ? "true" : "false")
      << ";source_precision_scaffold_key=" << source_precision_scaffold_key
      << ";core_feature_key=" << core_feature_key
      << ";ready=" << (surface.core_feature_expansion_ready ? "true" : "false");
  return key.str();
}

inline Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionSurface(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &core_feature_surface,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &source_precision_scaffold,
    std::size_t unique_diagnostic_code_count,
    std::uint64_t diagnostic_code_fingerprint,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count) {
  Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface surface;
  surface.parser_diagnostic_count = parser_diagnostic_count;
  surface.parser_snapshot_diagnostic_count = parser_snapshot_diagnostic_count;
  surface.grammar_hook_code_count = core_feature_surface.grammar_hook_code_count;
  surface.unique_diagnostic_code_count = unique_diagnostic_code_count;
  surface.diagnostic_code_fingerprint = diagnostic_code_fingerprint;
  surface.source_precision_fingerprint = source_precision_scaffold.coordinate_fingerprint;
  surface.core_feature_ready = IsObjc3DiagnosticGrammarHooksCoreFeatureReady(core_feature_surface);
  surface.accounting_consistent =
      surface.grammar_hook_code_count <= surface.parser_diagnostic_count &&
      surface.unique_diagnostic_code_count <= surface.grammar_hook_code_count &&
      surface.parser_snapshot_diagnostic_count == surface.parser_diagnostic_count;
  surface.source_precision_coverage_consistent =
      source_precision_scaffold.scaffold_consistent &&
      source_precision_scaffold.coordinate_tagged_diagnostic_count ==
          surface.parser_diagnostic_count;
  surface.replay_keys_ready =
      !source_precision_scaffold.scaffold_key.empty() &&
      !core_feature_surface.core_feature_key.empty() &&
      surface.diagnostic_code_fingerprint != 0 &&
      surface.source_precision_fingerprint != 0;
  surface.core_feature_expansion_ready =
      surface.core_feature_ready &&
      surface.accounting_consistent &&
      surface.source_precision_coverage_consistent &&
      surface.replay_keys_ready;
  surface.expansion_key = BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionKey(
      surface,
      source_precision_scaffold.scaffold_key,
      core_feature_surface.core_feature_key);
  if (surface.core_feature_expansion_ready) {
    return surface;
  }

  if (!surface.core_feature_ready) {
    surface.failure_reason = "parser diagnostic grammar hooks core feature is not ready";
  } else if (!surface.accounting_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion accounting is inconsistent";
  } else if (!surface.source_precision_coverage_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion source precision coverage is inconsistent";
  } else if (!surface.replay_keys_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion replay keys are not ready";
  } else {
    surface.failure_reason = "parser diagnostic grammar hooks core feature expansion is not ready";
  }
  return surface;
}

inline bool IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface) {
  return surface.core_feature_expansion_ready && !surface.expansion_key.empty();
}
