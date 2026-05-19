#pragma once

#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3ParseLoweringDiagnosticGrammarHooksReadinessRecord {
  Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface core_feature_expansion;
};

Objc3ParseLoweringDiagnosticGrammarHooksReadinessRecord
BuildObjc3ParseLoweringDiagnosticGrammarHooksReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3ParserContractSnapshot &parser_snapshot);

void ApplyObjc3ParseLoweringDiagnosticGrammarHooksConformanceMatrixReadiness(
    Objc3ParseLoweringReadinessSurface &surface);
