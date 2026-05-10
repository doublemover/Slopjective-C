#pragma once

#include <vector>

#include "lex/objc3_lexer.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3_frontend_pipeline_orchestration {

void PopulateSourceReadinessSummaries(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options,
    const std::vector<Objc3LexToken> &tokens);

void RefreshProtocolSymbolReadiness(Objc3FrontendPipelineResult &result);

void PopulateSemanticModelSummaries(
    Objc3FrontendPipelineResult &result,
    bool allow_error_handling_error_runtime_surface);

void PopulateRuntimeExportReadiness(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options);

void PublishRuntimeExportDiagnostics(Objc3FrontendPipelineResult &result);

void PublishTerminalPhaseResults(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options);

}  // namespace objc3_frontend_pipeline_orchestration
