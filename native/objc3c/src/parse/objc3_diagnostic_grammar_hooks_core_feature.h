#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "parse/objc3_diagnostic_source_precision_scaffold.h"
#include "parse/objc3_parser_contract.h"

struct Objc3DiagnosticGrammarHooksCoreFeatureSurface {
  std::size_t parser_diagnostic_count = 0;
  std::size_t grammar_hook_code_count = 0;
  bool grammar_hook_namespace_consistent = false;
  bool coordinate_order_consistent = false;
  bool source_precision_consistent = false;
  bool core_feature_consistent = false;
  std::string core_feature_key;
};

std::string BuildObjc3DiagnosticGrammarHooksCoreFeatureKey(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &surface,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold,
    const Objc3ParserContractSnapshot &parser_snapshot);

Objc3DiagnosticGrammarHooksCoreFeatureSurface BuildObjc3DiagnosticGrammarHooksCoreFeatureSurface(
    const std::vector<std::string> &parser_diagnostics,
    const Objc3ParserContractSnapshot &parser_snapshot,
    const Objc3ParserDiagnosticSourcePrecisionScaffold &scaffold);

bool IsObjc3DiagnosticGrammarHooksCoreFeatureReady(
    const Objc3DiagnosticGrammarHooksCoreFeatureSurface &surface);
