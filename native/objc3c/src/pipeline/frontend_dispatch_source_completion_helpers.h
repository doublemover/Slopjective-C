#pragma once

#include <string>

#include "ast/objc3_ast_declarations.h"
#include "pipeline/frontend_source_closure_replay_keys.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendDispatchDispatchIntentSourceClosureSummary
BuildDispatchDispatchIntentSourceClosureSummary(const Objc3Program &program);

Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
BuildDispatchDispatchIntentSourceCompletionSummary(const Objc3Program &program);

}  // namespace objc3c::pipeline::orchestration
