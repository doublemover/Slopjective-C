#pragma once

#include <string>
#include <vector>

#include "pipeline/objc3_frontend_types.h"
#include "token/objc3_token_contract.h"

std::vector<Objc3LexToken> RunObjc3FrontendLexStage(
    const std::string &source, const Objc3FrontendOptions &options,
    Objc3FrontendPipelineResult &result);

void RunObjc3FrontendParseStageIfLexClean(
    const std::vector<Objc3LexToken> &tokens,
    Objc3FrontendPipelineResult &result);

void TransportObjc3FrontendPipelineDiagnostics(
    Objc3FrontendPipelineResult &result);
