#pragma once

#include <string>
#include <vector>

#include "pipeline/objc3_frontend_types.h"
#include "token/objc3_token.h"

std::vector<Objc3LexToken> RunObjc3FrontendLexParseStageSequence(
    const std::string &source,
    const Objc3FrontendOptions &options,
    Objc3FrontendPipelineResult &result);
