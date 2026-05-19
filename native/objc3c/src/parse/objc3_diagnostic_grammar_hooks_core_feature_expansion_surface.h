#pragma once

#include <cstddef>
#include <cstdint>
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

std::string BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionKey(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface,
    const std::string &source_precision_scaffold_key,
    const std::string &core_feature_key);

Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionSurface(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &core_feature_surface,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &source_precision_scaffold,
    std::size_t unique_diagnostic_code_count,
    std::uint64_t diagnostic_code_fingerprint,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count);

bool IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface &surface);
